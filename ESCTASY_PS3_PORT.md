# Esctasy PS3 Menu Port (GTA V 1.12 BLES)

This branch replaces the small hard-coded menu in \`GTA112SPRX/menu.h\` with a submenu-based PS3 C++ port inspired by the existing **Esctasy** PC menu. It uses the repository's own native lookup, GTA rendering functions and controller hooks; **it does not depend on ScriptHookV**.

## Controls

- **R1 + D-pad Right** — open
- **D-pad Up / Down** — navigate
- **X** — select / toggle
- **D-pad Left / Right** — change a vehicle mod (or toggle)
- **Circle** — back, and close from the home menu

The new interface includes a dark menu, neon cyan/grey accent option, selected row, scroll tracking, per-option values and page navigation.

## Included pages and functions

| Page | Functions |
| --- | --- |
| Player | God Mode, Never Wanted, Invisible, Infinite Clip, Max Health / Armour, Clear Wanted, starter weapons |
| Model Changer | Michael, Franklin, Trevor, Skater, Chop, **Reset Original Character** |
| Vehicle | Spawner, Vehicle Customiser, Repair, Clean, Cyan Paint, Spawn Inside toggle |
| Vehicle Spawner | Static selection of base-game vehicles; loads model asynchronously before spawning |
| Vehicle Customiser | Spoiler, **Front Bumper (mod type 1), Rear Bumper (mod type 2)**, Side Skirts, Exhaust, Frame, Grille, Hood, Fenders, Roof, Engine, Brakes, Transmission, Suspension, Armour, Turbo |
| Teleport | Waypoint (only when terrain is loaded), LS Airport, Sandy Shores, Mt Chiliad, Maze Bank roof |
| World | Clear, Extra Sunny, Rain, Thunder, Morning, Noon, Night |
| Settings | Neon Accent, Spawn Inside Vehicle |

### Important limitations

- This targets **GTA V PS3 version 1.12 BLES** as stated by the original project README, not GTA IV, current GTA V, or other PS3 game builds. It retains the existing \`prx.cpp\` hooks and version-sensitive signature scanning.
- This is a **first feature and UI port**, not yet a 1:1 reproduction of the PC menu. The complete PC \`src/features.h\`, menu-building code and asset/JSON vehicle catalog are not in this repository. Bring those files into the project for exact page parity.
- The vehicle spawner uses a static base-game catalog. Models from later game updates are intentionally omitted.
- The waypoint teleport only acts when GTA returns a ground height, to avoid placing the player below the world or dropping them from a large height. If it says "Waypoint terrain not loaded", drive closer or use a preset.
- Player model changes are queued; no blocking loop runs inside the native hook. The original model is recorded before the first successful swap and can be restored by selecting **Reset Original Character**.
- Native hooks/addresses are version-sensitive and need testing on the supported game and console. Do not inject the SPRX into unrelated game versions.

## Build / install

The repository contains an old Visual Studio **PS3 SNC** project (\`GTA112SPRX/GTA112SPRX.vcxproj\`). Build with the matching PS3 SDK toolchain and select **PS3 / Debug** or **PS3 / Release**. The existing \`prx.cpp\` includes \`menu.h\`, so no additional C++ source file is needed.

After building, deploy **your newly compiled** \`GTA112.sprx\` through the CFW/plugin load method you already use. **\`PS3_Debug/GTA112.sprx\` is an older prebuilt file and does not contain this port.** Do not assume merely updating a header on GitHub updates the compiled SPRX.

A standard Arch Linux \`ppu-g++\` / PSL1GHT installation is *not* necessarily compatible with this solution, because the original code references PS3 SDK/SNC-specific headers and build targets. A PSL1GHT conversion would be separate work.

## Verification

The menu source was checked against the names exposed in this repository's PS3 \`Natives.h\` (including streaming, ped, vehicle mod, UI, and time/weather functions). **No PS3 SDK build or runtime test was performed**; compile and console testing are still required.
