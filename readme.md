# RBST: CMake isn't all that scary Edition

https://thiago-dfb.itch.io/rbst

## PRE-RECORDED VIDEO

https://drive.google.com/file/d/1wsL0v7RvVR1O1-ugBXPX9bT9yLrIXoPD/view?usp=sharing

## ABOUT THE GAME

OS: Windows 8/10/11

### Default controls
- in-game
    - WASD: movement
    - Left mouse button: shot
    - Right mouse button: rail
    - Spacebar: dash
    - (overrideable in the RBST_controls.toml config file, along with mouse sensitivity)
- home screen
    - F1: connect as player 1
    - F2: connect as player 2
    - F4: show background demo
    - Ctrl+V: paste remote IP address from clipboard

### Connection methods
- be within the same local area network
- use a VPN such as Radmin
- have each peer forward the UDP port defined in RBST_home.toml (default 8001)
- by GGPO's own features, connecting to 127.0.0.1 (localhost) will start a mirror match against yourself

### Game rules
- You have three actions: shot, rail and dash.
- Shot and rail spend ammo, and dash spends stamina. Both resources constantly refill.
- Shot is a slow projectile, and rail is a hitscan that freezes you in place for a split second while revealing your aim's direction.
- Hitting a shot with a rail results in a combo with a large area of effect.
- Dashing towards an enemy is a tackle and damages on contact, while dashing away from attacks is an evade with a perfect window that can deflect every attack except a combo.
- Mid-tackle, you're invulnerable to tackles and shots inputted after yours.
- If you're hurt you're stunned until you completely stop, but while stunned you're invulnerable.
- Dashing while in this state alerts you out of the stun, but spends all your stamina. (denoted by red exclamation point particles)
- Standing close enough to a rail without getting hit is a graze, completely filling your ammo and stamina. (denoted by green plus sign particles)

### Troubleshooting
"VCRUNTIME140.dll missing": install the latest Microsoft Visual C++ Redistributable.

### Game modding
- Gameplay parameters are all defined in RBST_config.toml and can be modified (with caveats informed in the file).
- Make sure both peers have the exact same file.
- the sprite folder has the atlas.toml file which contains sprite size information, allowing for brave enough souls to mod the game's appearance

## CHALLENGES AND CONSIDERATIONS

- As part of the argument that rollback networking had real use in online action games besides fighting games, GGPO, a library planned around being applied to conventional 2D fighting games, was applied as-is in a third-person shooter.
- The game was designed with a concise ruleset, focused on showing rollback's biggest strengths over other methods that can result in:
    - "lying" about your or your opponent's real position, such as client-server and dead reckoning;
    - negatively affecting your capability to react on time, such as lockstep and delay-based (lockstep with input delay).
- As a side effect of determinism, a replay feature was as simple as storing input data as soon as each peer confirms their inputs for each frame.
- Tying into the application of rollback into more advanced games, particle corrections for sharp game state changes upon rollback were implemented, albeit through crude heuristics, such as spatial and temporal proximity.
- Due to inconclusive research on how cross-platform determinism can be achieved even with floating point at the time of writing the thesis, a fixed point math library was used instead, gravely diminishing maximum decimal values that could be used.

## ABOUT THE CODE AND LIBRARIES ##

### Requirements
- Visual Studio 2019
- CMake 3.29 or later

### Open source attributions
- Embedded Template Library 20.35.14 (c) 2016 jwellbelove, MIT license
- fpm 1.1.0 (c) 2019 Mike Lankamp, MIT license
- GGPO (c) 2009-2019 GroundStorm Studios, LLC., MIT license
- raylib 4.5.0 (c) 2013-2023 Ramon Santamaria, Zlib license
- toml++ 3.3.0 (c) Mark Gillard, MIT license

### Code excerpts

GGPOController.hpp contains public code excerpts that are properly credited within the file, but here are their sources:

- A fix using defines to resolve conflicts between raylib and the Windows API, taken from https://github.com/raysan5/raylib/issues/1217
- Dynamic memory code for saving and loading the game state is largely the same as in GGPO's example project
