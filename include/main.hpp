#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"

// Codegen includes for nice utilities
#include "codegen/include/GlobalNamespace/MultiplayerController.hpp"
#include "codegen/include/GlobalNamespace/MultiplayerGameplayAnimator.hpp"
#include "codegen/include/GlobalNamespace/IConnectedPlayer.hpp"
#include "codegen/include/GlobalNamespace/ConnectedPlayerHelpers.hpp"
#include "codegen/include/GlobalNamespace/LightsAnimator.hpp"
#include "codegen/include/GlobalNamespace/ColorSO.hpp"
#include "codegen/include/GlobalNamespace/SimpleColorSO.hpp"
#include "codegen/include/GlobalNamespace/BeatmapEventData.hpp"
#include "codegen/include/GlobalNamespace/BeatmapEventTypeExtensions.hpp"
#include "codegen/include/GlobalNamespace/BeatmapEventType.hpp"
#include "codegen/include/GlobalNamespace/BeatmapObjectCallbackController.hpp"
#include "codegen/include/GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "codegen/include/GlobalNamespace/EnvironmentInfoSO.hpp"
#include "codegen/include/GlobalNamespace/ColorSchemeSO.hpp"
#include "codegen/include/GlobalNamespace/GameCoreSceneSetupData.hpp"
#include "codegen/include/GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "codegen/include/GlobalNamespace/BeatmapDifficulty.hpp"
#include "codegen/include/GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "codegen/include/GlobalNamespace/IDifficultyBeatmap.hpp"
#include "codegen/include/GlobalNamespace/GameplayModifiers.hpp"
#include "codegen/include/GlobalNamespace/PlayerSpecificSettings.hpp"
#include "codegen/include/GlobalNamespace/PracticeSettings.hpp"
#include "codegen/include/GlobalNamespace/ColorScheme.hpp"
#include "codegen/include/UnityEngine/Color.hpp"
#include "codegen/include/UnityEngine/Resources.hpp"
#include "codegen/include/UnityEngine/ScriptableObject.hpp"

// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
const Logger& getLogger();
