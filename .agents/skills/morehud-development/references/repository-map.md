# moreHUD Inventory repository map

## Native plugin

- C++ sources: `src/`
- Headers: `include/`
- Project definition: `CMakeLists.txt`, `CMakePresets.json`
- Release build: `./build.ps1`
- Debug build: `./build-debug.ps1`
- Deployment is opt-in through the wrappers' `-DeployTarget`; do not infer a game or MO2 directory.

Use the wrappers from the repository root. They establish the x64 Visual Studio environment process-locally and select the repository presets. Preserve a caller-supplied `VCPKG_ROOT`. When diagnosing a native/Scaleform boundary, search native registration and object attachment sites together with the AS2 bridge stubs under `AS2/ahz/scripts/widgets/AHZCommon/skse`.

When maintaining the wrapper's Visual Studio environment import, invoke `VsDevCmd.bat` through `call` with `-arch=x64 -host_arch=x64` before capturing `set`. Finding `cl.exe` alone is not sufficient: a partially imported environment can compile and then fail at link time with widespread unresolved CRT and Windows symbols because `LIB` and `INCLUDE` were not populated. Discover Visual Studio with `vswhere.exe`; do not commit a workstation-specific installation path.

Before retaining a project-fork runtime patch, fetch the latest tagged upstream `ng` branch and compare each custom commit's behavior with the upstream address-library, runtime-detection, and layout changes. When upstream fully covers the intent, fast-forward the fork's clean `ng` branch, keep the obsolete feature branch only as history, and pin this repository's submodule directly to the upstream-equivalent commit. Verify the fork, upstream, and submodule commit IDs agree, then rebuild the consumer DLL.

## Scaleform source and output

- Authoritative FLA and AS2 tree: `AS2/`
- Main FLA: `AS2/AHZmoreHudInventory.fla`
- Supporting FLAs: `AS2/AHZmoreHudIE_BaseIcons.fla`, `AS2/AHZmoreHudIE_BaseLargeItemCard.fla`
- Main AS2 consumer: `AS2/ahz/scripts/widgets/AHZmoreHUDInventory.as`
- Native bridge declaration: `AS2/ahz/scripts/widgets/AHZCommon/skse/plugins/AHZmoreHUDInventory.as`
- Publisher: `Scripts/BuildScaleform.ps1`
- Pinned publisher dependency: `Scripts/package.json` (`flc` 3.1.0)
- Installed main movie: `Data/Interface/AHZmoreHUDInventory.swf`
- Installed resource movies: `Data/Interface/exported/moreHUDIE/baseIcons.swf` and `baseLargeItemCard.swf`

The publisher accepts `-FlashExe`, then checks `FLASH_EXE`, `FlashPath`, and `PATH`. Supply a real `Flash.exe`/`Animate.exe`; do not commit a workstation-specific Adobe path. It installs npm dependencies beneath `Scripts/node_modules`, publishes into `build/scaleform`, verifies all expected movie names, and copies them into `Data`.

If the repository is on a synced/virtual drive and `flc` reports `EBUSY` on its status files, use a unique local temporary output directory and perform the publisher's destination-copy and expected-file checks explicitly. Do not treat a generated SWF as successful unless Flash reports zero errors and `Shutdown,0`.

Publish from the repository root:

```powershell
./Scripts/BuildScaleform.ps1 -FlashExe '<Adobe Flash CS5/CS6 executable>'
```

For release staging and BSA packaging, use `Scripts/ReleaseFiles.ps1`; its `-BuildScaleform` and `-FlashExe` parameters compose the UI build with the native release workflow.

The release script accepts explicit `-SevenZipExe` and `-ArchiveExe` paths. If either tool is absent from `PATH`, discover its installed location and pass it for that invocation rather than hardcoding a machine-specific path. A successful release check includes the DLL file version, both standard and loose archive inventories, and archive hashes.

For MO2 deployment, obtain the mods root from a caller-supplied environment variable such as `$env:MOREHUD_MO2_MODS_ROOT`; keep its workstation-specific value outside the repository. Resolve the selected mod directory beneath that root. A loose deployment installs the main SWF at the relative paths `Interface/AHZmoreHUDInventory.swf` and `Interface/exported/AHZmoreHUDInventory.swf`, plus the two resource movies beneath `Interface/exported/moreHUDIE/`.

## Papyrus

- Source: `Data/Source/Scripts/AhzMoreHudIE.psc`
- Compiled output: `Data/Scripts/AhzMoreHudIE.pex`

This project currently describes its normal behavior as native plus AS2, so first confirm that a reported issue actually flows through Papyrus. If Papyrus changes are required, locate the existing compiler configuration or add build integration only when the task requests it. Papyrus compilation needs the matching game/Creation Kit flags and dependency sources; editing a PSC does not itself update the PEX.

## Cross-layer tracing

For a value displayed in an item card, check this sequence:

1. C++ data extraction and Scaleform value construction.
2. Scaleform registration/callback and the exposed plugin method/object name.
3. AS2 bridge declaration and call site.
4. AS2 data transformation and frame/mode selection.
5. FLA instance names, linkage identifiers, text-field settings, and timeline frame labels.
6. Published SWF destination and the path used by `MovieClipLoader`/configuration.

Search exact strings across `src`, `include`, `AS2`, and `Data/Source/Scripts`; contract drift often appears as a spelling, casing, type, or timing mismatch.
