# AS2, FLA, SWF, and Scaleform workflow

## Choose the source of truth

Use Adobe Flash CS5/CS6 plus the repository's pinned `flc` publisher when the owning movie has a tracked FLA. This preserves its library, timelines, linkage, and publish settings. Use FFDec/JPEXS when inspecting compiled movies, recovering information, comparing tags/scripts, exporting XML, or working in a build explicitly based on `-xml2swf` and `-importScript`.

Do not assume FFDec can faithfully round-trip a complex authored FLA. Its AS1/2 editing and FLA export are useful, but a decompiled FLA is a reconstruction rather than the original authoring document.

## Editing AS2

- Identify the owning FLA and confirm its source path/classpath before changing a `.as` file.
- Preserve AS2 typing and syntax (`function ...:Void`, `var`, prototype/timeline semantics); do not introduce AS3 packages, namespaces, E4X, or AVM2 APIs.
- Treat frame labels, instance names, linkage IDs, exported class names, and loaded SWF paths as API contracts.
- Consider Scaleform extensions and Skyrim's embedded GFx runtime rather than desktop Flash Player behavior alone.
- When an item-card description expands above a reserved footer, cache the description's authored height and each footer control's authored gap below it. Position each control from the expanded description bottom plus that gap; do not increment current _y or anchor from whichever control happens to have the lowest rendered edge. This keeps the footer in the card's reserved bottom region and prevents long descriptions from overlapping it across vanilla, original SkyUI, and SkyUI Community refresh behavior.
- SkyUI Community exposes the header separator as the root-level itemCard.line instance instead of baking it into the item-card background. When replacing that background, cache and hide this instance with a null check, then restore its original alpha when returning to the normal card. Vanilla and original SkyUI do not expose this separate instance.
- For loaded resource movies, verify both the physical destination and the string passed to the loader. Case may be tolerated on Windows but should remain consistent for tooling and archives.

## Editing a FLA

FLA edits require Adobe Flash/Animate or JSFL automation executed by it. Before changing the document, record the target document, timeline, layer, frame/frame label, symbol/library path, linkage identifier, instance name, and publish target. Make the smallest authoring change and save the FLA before publishing.

When automation is appropriate, create a narrowly scoped `.jsfl` helper in a temporary or repository tooling location, launch it through the selected Flash executable, and make it fail visibly when the expected document/symbol/layer is absent. Avoid screen-coordinate automation. Do not run concurrent Flash publishers against the same authoring files or output directory.

After publishing, verify that all three expected movies exist and that their timestamps/sizes changed only where expected. Inspect the result with FFDec when the change affects scripts, linkage, frames, or exported symbols.

## Local publishing prerequisites

- Resolve Flash from `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\Flash.exe` (including the WOW6432Node view) when it is not on `PATH`. This workstation's CS6 registration currently points to `J:\AdobeCS6\Adobe Flash CS6\Flash.exe`; treat that as discovered machine state, not a path to commit.
- The main FLA imports `gfx.events.EventDispatcher`. If Flash cannot resolve it, make the pinned `external/CommonLibSSE-NG/Flash/AS2/CLIK/gfx` package temporarily visible as `AS2/gfx` or supply the equivalent AS2 classpath. Verify and remove any temporary junction after publishing; do not duplicate the CLIK source.
- `flc` tails `info.txt` while Flash writes it. On synced or virtual drives this can fail with `EBUSY`. Publish to a unique directory under the local system temp directory, do not read its live status files, wait for `Shutdown,0`, then copy the verified SWFs to their repository and deployment destinations.
- The FLA references local authoring faces named `FuturaCondensed`, `FuturaStd-CondensedBold`, and `FuturaStd-CondensedLight`, while runtime text imports SkyUI/Skyrim `$Everywhere*Font` resources. Missing local faces affect authoring preview and can open a modal Font Mapping dialog. Keep proprietary font files outside Git and distributable packages. Local substitutes may be mapped for preview, but do not rename the FLA fonts or change their runtime linkage merely to silence the dialog.


## FFDec/JPEXS

Prefer the pinned CLI already vendored at:

`../moreHUDSE/external/SkyUI-Community/tools/FFDec/ffdec-cli.exe`

Resolve it at runtime and allow a user-supplied/system `ffdec-cli` fallback; do not copy the binary into this repository. Keep exports under `build/` or a temporary directory.

Useful read/transform modes include script export/dumps, FLA export for investigation, SWF XML export/import, `-xml2swf`, and `-importScript`. Check the installed CLI's `-help` before composing a mutation because flags can vary by FFDec version. Always write mutations to a separate output first, inspect it, and only then replace a tracked/generated destination as part of the requested build.

The vendored SkyUI Community implementation demonstrates the reliable injection pattern:

1. Rebuild a base SWF from tracked XML with `-xml2swf`.
2. Stage only the AS2 files belonging to that target, preserving class paths.
3. Copy the base to a distinct output.
4. Run FFDec `-importScript` against the output.
5. Depend on the base XML and every staged AS2 source in the build graph.

Consult `../moreHUDSE/external/SkyUI-Community/cmake/SWFXMLPatch.cmake`, `cmake/ImportToSWF.cmake`, and `source/swfsources.cmake` when implementing or debugging that mode.

## Verification

For script-only changes, confirm the published SWF contains the expected AS2 class/script and that unrelated movies are unchanged. For timeline/library changes, inspect frame labels, symbol linkage, instance names, transforms, text properties, and imported fonts. A successful publish is not proof of in-game behavior; state the menu and interaction that still need a Skyrim smoke test.
