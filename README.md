# moreHUD Inventory Edition

This repository contains the SKSE64 plugin and Scaleform source used by [moreHUD Inventory Edition](https://www.nexusmods.com/skyrimspecialedition/mods/18619) for Skyrim Special Edition and Anniversary Edition.

The plugin loads `AHZmoreHUDInventory.swf` into the Inventory, Crafting, Container, Barter, and Magic menus. Its C++ code extends item-card data with details such as known enchantments and positive/negative effect counts, while the ActionScript 2 code renders and resizes the cards.

## Runtime dependencies

- [SKSE64](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
- [SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604)
- The current Microsoft Visual C++ Redistributable

## Build dependencies

- Visual Studio 2022 with the x64 MSVC C++ toolchain
- CMake and Ninja (the Visual Studio bundled copies are supported)
- [vcpkg](https://github.com/microsoft/vcpkg), with `VCPKG_ROOT` set
- [CommonLibSSE-NG v6.6.0](https://github.com/alandtse/CommonLibSSE-NG), included as a pinned submodule from the project fork

Clone recursively so the pinned CommonLibSSE-NG revision is available:

```powershell
git clone --recurse-submodules https://github.com/ahzaab/moreHUDInventory.git
```

## Building

The PowerShell wrappers initialize MSVC only in their own process and explicitly select Visual Studio's CMake and Ninja when available. They do not depend on CLion and do not modify machine or user environment variables.

```powershell
# Optimized release DLL with a matching PDB
.\build.ps1

# Debug DLL and PDB
.\build-debug.ps1
```

If Visual Studio cannot be discovered, supply its x64 environment script with `-VsDevCmd`, or set the process-local `VCVARS64` variable. To deploy a build, pass the MO2 mod's Data directory explicitly:

```powershell
.\build.ps1 -DeployTarget '<MO2 mod Data directory>'
```

The DLL and PDB are copied to `SKSE\Plugins` beneath that target. No Skyrim installation path is stored in the repository.

## Scaleform

The tracked FLA and AS2 sources are under `AS2`. Publishing uses the pinned `flc` npm package and resolves Adobe Flash/Animate through `-FlashExe`, `FLASH_EXE`, `FlashPath`, or `PATH`; no application installation path is hard-coded.

```powershell
.\Scripts\BuildScaleform.ps1 -FlashExe '<Flash or Animate executable>'
```

The main movie is placed in `Data\Interface`. The release staging step also installs it in `Data\Interface\exported` for vanilla UI and SkyUI compatibility. Author resources remain under `Data\Interface\exported\moreHUDIE`.

## Nexus packages

Install the current Creation Kit archive tool and 7-Zip, then run:

```powershell
.\Scripts\ReleaseFiles.ps1
```

You can supply `-ArchiveExe` and `-SevenZipExe`, or set `ARCHIVE_EXE` and `SEVENZIP_EXE`. `SKYRIM_AE_ROOT` may also point to a game installation containing `Tools\Archive\Archive.exe`.

The script builds the Release preset, creates the BSA, and writes `moreHUD Inventory Edition - AE-<version>.7z` and `moreHUD Inventory Edition Loose Version - AE-<version>.7z` under `release\<version>`. Nexus adds its mod and upload IDs to the downloaded filenames. Both packages include the optimized DLL and its matching PDB so crash loggers can resolve plugin symbols. Generated DLL, PDB, BSA, and 7z artifacts remain ignored by Git.

The separate Skyrim VR variant is intentionally outside this migration and will be integrated later.

## License

moreHUD Inventory Edition is licensed under the GNU General Public License v3.0 or later. See `LICENSE` and `COPYING`.
