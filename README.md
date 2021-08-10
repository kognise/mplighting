> ⚠️ **This project is temporarily on hold!** ⚠️
> 
> I don't have the time or energy to update this right now but I will get back to it soon.

# MultiplayerLighting

**A beat saber quest mod which enables lighting (including chromalite/rainbow) in multiplayer.** This is my first mod and I just learned C++ yesterday so don't expect anything amazing.

I made this in a short amount of time (including learning how to mod) so it basically only just picks a lighting event every time interval and applies it. Still looks significantly better than the default multiplayer static lighting in my opinion. Should be stable enough if you want to try it out.

## Installation

Download the latest release from the [releases page](https://github.com/kognise/mplighting/releases/latest) and drag it into BMBF.

## Custom Events

I don't really expect anyone to use this, mainly because no tooling currently supports this so it would require manual editing of the map file. But, for potential future flexibility, I implemented a way to create lighting events specifically for multiplayer.

After receiving a multiplayer lighting event, this mod will ignore all regular lighting events afterwards in the song - so if you're making a song you should probably include one of these first.

The system is really easy to use - just use `1111111111` as the [event type](https://bsmg.wiki/mapping/map-format.html#type-3) in the beatmap file.

## Credits

- Thanks to people from the Beat Saber Modding Group community for helping me figure out what I was doing, especially Sc2ad
- [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) for [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook) which basically enables all modding
- [Lauriethefish](https://github.com/Lauriethefish) and [danrouse](https://github.com/danrouse) for [this template](https://github.com/Lauriethefish/quest-mod-template)
- [danrouse](https://github.com/danrouse) for [this great tutorial](https://github.com/danrouse/beatsaber-quest-modding-guide) which was super helpful
