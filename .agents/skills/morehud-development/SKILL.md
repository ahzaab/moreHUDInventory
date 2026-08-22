---
name: morehud-development
description: Modify, diagnose, build, and verify moreHUD Inventory Edition across its C++/MSVC plugin, Papyrus source, ActionScript 2, FLA, SWF, and Scaleform integration. Use for work in this repository that crosses or touches those runtime boundaries.
---

# moreHUD Inventory development

Treat the repository as one feature spanning native SKSE code and Scaleform. Trace a change end to end before editing: native data production and registration, the ActionScript bridge declaration, the AS2 consumer, the FLA library/timeline when applicable, and the installed SWF path.

Read [references/repository-map.md](references/repository-map.md) before changing code or generated assets. For any AS2, FLA, SWF, or visual UI task, also read [references/scaleform.md](references/scaleform.md). For Papyrus work, read the Papyrus section in the repository map.

## Working rules

- Preserve the existing CMake/MSVC wrappers and pinned dependencies. Use the repository scripts instead of inventing parallel build commands.
- Edit sources, not generated artifacts, unless the request specifically concerns binary inspection or patching. Rebuild tracked PEX/SWF outputs from their matching source and report which compiler produced them.
- Keep native-to-AS2 contracts synchronized: function names, object paths, member names, value types, sentinel values, and initialization timing must agree on both sides.
- ActionScript here is AS2 for Scaleform/GFx, not browser JavaScript or AS3. Preserve AS2 syntax, timeline assumptions, `_root`/`_parent` behavior, linkage identifiers, frame labels, depth ordering, and CLIK conventions.
- FLA files are binary authoring documents. Do not byte-edit them. Use Adobe Flash/Animate publishing for tracked FLA documents and use FFDec for inspection, export, comparison, or a deliberately selected SWF/XML/script injection workflow.
- Do not silently replace an authoritative FLA-based pipeline with decompiler output. A SWF-to-FLA reconstruction can lose authoring fidelity even when the movie still runs.
- Validate the narrowest changed layer first, then its adjacent boundary, then the integrated build when practical. Distinguish compiler/build success from in-game verification.

## Tool routing

- C++/MSVC, CMake, SKSE, or CommonLibSSE-NG: use the repository build wrappers described in the repository map.
- `.psc` Papyrus source: compile with the repository's existing Papyrus/release tooling when present; never claim a `.pex` was updated without compiler output.
- `.as` beside a tracked `.fla`: edit the AS2 source and publish the owning FLA with `Scripts/BuildScaleform.ps1`.
- Timeline, symbols, library items, linkage, instance names, fonts, or layout: edit the FLA through Adobe Flash CS5/CS6 automation or an explicitly prepared JSFL operation, then publish with the pinned `flc` workflow.
- Existing SWF investigation: use FFDec CLI to export/dump scripts or XML into a temporary/build directory and compare structure without overwriting source assets.
- SkyUI base movie reconstruction or AS2 injection: follow the FFDec XML/importScript model documented in the vendored SkyUI Community project; do not apply it to this repository's authored FLAs unless the task deliberately migrates the build system.

## Completion evidence

Report the source files changed, outputs regenerated, exact build/publish checks run, and anything that still requires launching Skyrim. For UI changes, include the affected menu, movie path, symbol/frame/instance involved, and the native or Papyrus producer of its data when relevant.
