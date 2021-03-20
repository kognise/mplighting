#include "main.hpp"
#include <chrono>
using namespace GlobalNamespace;

// The event type value for multiplayer-only events
const int MULTIPLAYER_EVENT_TYPE = 1111111111;

// Stores the ID and version of our mod, and is sent to the modloader upon startup
static ModInfo modInfo;

// Current color data
int currentChromaColor = -1;
int currentColor = 0;

// List of active lighting animators
Array<MultiplayerGameplayAnimator*>* animators;

// Last event time
float lastTime = -1.0;

// Whether a specifically multiplayer lighting event has been received,
// if true other lighting will be disabled
bool gotMultiplayerLightingEvent = false;

// Player settings
ColorScheme* colorScheme;
bool staticLights = false;


// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}


// Utilities for working with colors
ColorSO* getColorSO(UnityEngine::Color color) {
    SimpleColorSO* colorSO = reinterpret_cast<SimpleColorSO*>(
        UnityEngine::ScriptableObject::CreateInstance(csTypeOf(SimpleColorSO*))
    );
    colorSO->SetColor(color);
    return colorSO;
}
ColorSO* getColorSO(float r, float g, float b, float a) {
    UnityEngine::Color color = { r, g, b, a };
    return getColorSO(color);
}

// Stolen from PinkUtils, utility for working with rgb colors stored as binary
float getChannelFromNumber(int rgb, int shift) {
	int value = (rgb >> shift) & 0x0ff; // Shifts the bits the passed number times to the right and masks to only the last two bytes
	float floatValue = (value / 255.0); // Make a float which a color can use, a number between 0 and 1 hence we devide by 255.0
	return floatValue;
}

// There are very rare cases in which get_isMe will error for some reason,
// this just exists to catch this - janky, but helps prevent crashes
bool isMe(IConnectedPlayer* player) {
    if (!player) return false;
    try {
        return player->get_isMe();
    } catch (...) {
        getLogger().warning("get_isMe threw an error!");
        return false;
    }
}


// Multiplayer level transition setup
MAKE_HOOK_OFFSETLESS(
    TransitionSetup,
    void,
    MultiplayerLevelScenesTransitionSetupDataSO* self,
    Il2CppString* gameMode,
    IPreviewBeatmapLevel* previewBeatmapLevel,
    BeatmapDifficulty beatmapDifficulty,
    BeatmapCharacteristicSO* beatmapCharacteristic,
    IDifficultyBeatmap* difficultyBeatmap,
    ColorScheme* overrideColorScheme,
    GameplayModifiers* gameplayModifiers,
    PlayerSpecificSettings* playerSpecificSettings,
    EnvironmentEffectsFilterPreset* environmentEffectsFilterPreset,
    PracticeSettings* practiceSettings,
    bool useTestNoteCutSoundEffects
) {
    colorScheme = overrideColorScheme ? overrideColorScheme : self->multiplayerEnvironmentInfo->colorScheme->colorScheme;
    //staticLights = playerSpecificSettings->staticLights;
    //staticLights = environmentEffectsFilterPreset->

    TransitionSetup(
        self,
        gameMode,
        previewBeatmapLevel,
        beatmapDifficulty,
        beatmapCharacteristic,
        difficultyBeatmap,
        overrideColorScheme,
        gameplayModifiers,
        playerSpecificSettings,
        environmentEffectsFilterPreset,
        practiceSettings,
        useTestNoteCutSoundEffects
    );
}

// Lighting hook
MAKE_HOOK_OFFSETLESS(MPLighting, void, MultiplayerGameplayAnimator* self, MultiplayerController::State state) {
    if (state != -1) {
        // We have to update the animators list on every non-lighting update
        animators = UnityEngine::Resources::FindObjectsOfTypeAll<MultiplayerGameplayAnimator*>();
    }

    if (isMe(self->connectedPlayer)) {
        if (state == MultiplayerController::State::Gameplay) {
            gotMultiplayerLightingEvent = false;
        } else if (state != -1) {
            gotMultiplayerLightingEvent = false;
            currentColor = 0;
            currentChromaColor = -1;
            lastTime = -1.0;
        }

        if (!colorScheme) {
            getLogger().warning("Color scheme is null");
        } else if (!staticLights) {
            ColorSO* color;

            if (currentColor == 0) {
                color = getColorSO(0.0, 0.0, 0.0, 0.0);
            } else if (currentChromaColor != -1) {
                // Chroma colors are all greater than 2000000000 for separation
                int value = currentChromaColor - 2000000000;
                color = getColorSO(
                    getChannelFromNumber(value, 16),
                    getChannelFromNumber(value, 8),
                    getChannelFromNumber(value, 0),
                    1.0
                );
            } else if (currentColor == 1) {
                color = getColorSO(colorScheme->environmentColor0);
            } else if (currentColor == 2) {
                color = getColorSO(colorScheme->environmentColor1);
            }

            self->activeLightsColor = color;
            self->leadingLightsColor = color;
        }
    } else {
        self->activeLightsColor = getColorSO(colorScheme->environmentColor0);
        self->leadingLightsColor = getColorSO(colorScheme->environmentColor1);
    }

    MPLighting(self, state == -1 ? static_cast<MultiplayerController::State>(MultiplayerController::State::Gameplay) : state);
}

// Beatmap event hook
MAKE_HOOK_OFFSETLESS(BeatmapEvent, void, BeatmapObjectCallbackController* self, BeatmapEventData* event) {
    BeatmapEvent(self, event);
    if (staticLights) return; // To reduce any potential lag

    // long t1 = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    if (!animators || lastTime == event->time) return;
    if (event->type <= 4 || event->type == MULTIPLAYER_EVENT_TYPE) {
        // If we've previously gotten multiplayer lighting events, ignore regular ones
        if (gotMultiplayerLightingEvent && event->type != MULTIPLAYER_EVENT_TYPE) return;

        if (event->value >= 2000000000) {
            currentChromaColor = event->value;
            return;
        }

        // TODO: Implement lights that fade out or flash, right now all lighting events
        // are treated as turn on and a turn off event is required to go black
        if (event->value == 0) {
            currentColor = 0;
        } else if (event->value >= 1 && event->value <= 3) {
            currentColor = 1;
            lastTime = event->time;
        } else if (event->value >= 5 && event->value <= 7) {
            currentColor = 2;
            lastTime = event->time;
        }
        
        for (int i = 0; i < animators->Length(); i++) {
            MultiplayerGameplayAnimator* animator = animators->values[i];
            if (!animator->connectedPlayer || !isMe(animator->connectedPlayer)) continue;
            animator->HandleStateChanged(-1);
        }
    }

    // long t2 = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    // getLogger().info("BeatmapEvent took " + std::to_string((t2 - t1) / 1000000.0) + "ms");
}


// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    Logger& hkLog = getLogger();

    getLogger().info("Installing hooks...");
    INSTALL_HOOK_OFFSETLESS(hkLog, MPLighting, il2cpp_utils::FindMethodUnsafe("", "MultiplayerGameplayAnimator", "HandleStateChanged", 1));
    INSTALL_HOOK_OFFSETLESS(hkLog, BeatmapEvent, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectCallbackController", "SendBeatmapEventDidTriggerEvent", 1));
    INSTALL_HOOK_OFFSETLESS(hkLog, TransitionSetup, il2cpp_utils::FindMethodUnsafe("", "MultiplayerLevelScenesTransitionSetupDataSO", "Init", 10));
    getLogger().info("Installed all hooks!");
}
