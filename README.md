[![C/C++ CI](https://github.com/gurka/OpenCrystalCaves/actions/workflows/cpp.yml/badge.svg)](https://github.com/gurka/OpenCrystalCaves/actions/workflows/cpp.yml)
[![#CrystalCaves on Mastodon](https://img.shields.io/badge/-%23CrystalCaves-%23303030?logo=mastodon)](https://mastodon.gamedev.place/tags/CrystalCaves)
[![Download at itch.io](https://img.shields.io/badge/-Downloads-%23303030?logo=itchdotio)](https://congusbongus.itch.io/opencrystalcaves)

# OpenCrystalCaves

OCC is an unofficial open source engine reimplementation of the game trilogy [Crystal Caves](https://en.wikipedia.org/wiki/Crystal_Caves).

Download binaries and project info at [itch.io](https://congusbongus.itch.io/opencrystalcaves)

**This repository includes the shareware version of Crystal Caves. To play the full, retail version, you can copy over the data files or purchase them at [Steam](http://store.steampowered.com/app/358260/Crystal_Caves/) or [GoG](https://www.gog.com/game/crystal_caves).**

Current state of OCC:

![Animation](/screenshots/animation.gif?raw=true "Animation")

![Level 4](/screenshots/screenshot2.png?raw=true "Level 4")

![Main Level](/screenshots/screenshot3.png?raw=true "Main Level")

Currently the engine can:

- Load sprite, item and level data.
- Main level and entering levels.
- Render items that, both behind and in front of the player, as well as animated items.
- Move the player around, jumping, colliding with solid items and moving platforms.

Features that are missing:

- Loading and saving games.
- Some items.
- Some enemies.
- (and much more).

## Differences with Crystal Caves

OCC aims to be a better, modern remake of Crystal Caves while preserving the same gameplay experience. Therefore there are some changes and improvements made where they make sense. Below is a list of differences:

| Feature                      | Crystal Caves                                                                                       | OpenCrystalCaves                                                                                          |
| ---------------------------- | --------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------- |
| BBS menu item                | ![](/screenshots/bbs_cc.png) "Call our BBS" menu item which shows BBS phone numbers                 | ![](/screenshots/bbs_occ.png) "Visit our Website" which opens the browser to the OCC game page            |
| Earth and moon orbits        | ![](/screenshots/earthmoon_cc.gif?raw=true) Earth and Moon move at constant speed                   | ![](/screenshots/earthmoon_occ.gif?raw=true) Earth and Moon move in a circular arc                        |
| Enemy and hazard detection   | ![](/screenshots/web_cc.gif?raw=true) Detection can go through walls                                | ![](/screenshots/web_occ.gif?raw=true) Detection is blocked by walls                                      |
| Head bump momentum           | ![](/screenshots/headbump_cc.gif?raw=true) Horizontal momentum is lost when player bumps their head | ![](/screenshots/headbump_occ.gif?raw=true) Horizontal momentum is preserved when player bumps their head |
| Instructions                 | ![](/screenshots/instructions_cc.png) Sequential, text-only instructions                            | ![](/screenshots/instructions_occ.png) Added inline icons showing what game objects look like             |
| Laser position               | ![](/screenshots/laser_cc.gif?raw=true) Laser is fired slightly below the muzzle                    | ![](/screenshots/laser_occ.gif?raw=true) Laser is aligned with the muzzle                                 |
| Moving platform animation    | ![](/screenshots/movingplatform_cc.gif?raw=true) Animation is the same regardless of direction      | ![](/screenshots/movingplatform_occ.gif?raw=true) Animation reverses so that monitor follows background   |
| Question mark spin animation | ![](/screenshots/question_cc.gif?raw=true) Animation repeats and skips some frames                  | ![](/screenshots/question_occ.gif?raw=true) Animation shows fully rotating question mark                  |
| Quit To OS menu label        | ![](/screenshots/quitto_cc.png) Quit to DOS                                                         | ![](/screenshots/quitto_occ.png) Quit to OS depending on build                                            |
| Restarting levels            | Menu > Quit Current Game > Main Level, re-enter level                                               | Menu > Restart                                                                                            |
| Transitions                  | ![](/screenshots/transition_cc.gif?raw=true) Fade by palette color                                  | ![](/screenshots/transition_occ.gif?raw=true) Fade to black                                               |
| Hammer crush                 | ![](/screenshots/crush_cc.gif?raw=true) Getting hurt normally by hammers                            | ![](/screenshots/crush_occ.gif?raw=true) Getting crushed by hammers                                       |
| Candle pickup sound          | No sound                                                                                            | Same sound as other similar pickups (shovel, pickaxe)                                                     |
| Snoozer roll animation       | ![](/screenshots/roll_cc.gif?raw=true) Third frame has slight Y offset                              | ![](/screenshots/roll_occ.gif?raw=true) No irregular Y offsets                                            |
| Flame on/off animation       | ![](/screenshots/flame_cc.gif?raw=true) Transition in but not out                                   | ![](/screenshots/flame_occ.gif?raw=true) Transition both in and out                                       |
| Lights off                   | ![](/screenshots/dark_cc.png?raw=true) EGA palette swapped                                          | ![](/screenshots/dark_occ.png?raw=true) Option to use blue monochrome dark palette                        |

## Compiling OCC

OCC is built using C++20 and requires external libraries: [SDL 2.0](https://www.libsdl.org/), and [SDL_mixer](https://www.libsdl.org/projects/old/SDL_mixer/). SDL2 must be installed and available in `/usr/include/SDL2`. Additionally the build system `cmake` must be installed.

Steps to compile (Linux, macOS):

```
git clone --recursive https://github.com/gurka/OpenCrystalCaves.git
cd OpenCrystalCaves
mkdir debug
cmake -Bdebug -DCMAKE_BUILD_TYPE=debug occ
cd debug
make
```

The binary will be available at `OpenCrystalCaves/build/occ`

Or in Windows (with [vcpkg](https://vcpkg.io)):

```
# In vcpkg dir
./vcpkg install --triplet x64-windows sdl2 sdl2-mixer --recurse
# In your projects dir
git clone --recursive https://github.com/gurka/OpenCrystalCaves.git
cd OpenCrystalCaves
cmake -Bbuild -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake occ -A x64
```

The Visual Studio project will be available at `OpenCrystalCaves/build`

## Running OCC

OCC includes the shareware episode for convenience, but for the other episodes requires data files from the original Crystal Caves (any episode). Either install it via Steam or GoG, or copy the game data to the `media` folder in the occ package (such as `CC1.GFX`).
