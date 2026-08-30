---
name: skyrim-papyrus-test-harness
description: Build isolated Skyrim Papyrus test scripts, compile them to PEX, and create or update test ESP quest attachments through SSEEdit/xEdit. Use for temporary in-game SKSE/Papyrus integration harnesses, not production script packaging.
---

# Skyrim Papyrus Test Harness

Keep test harnesses separate from production `Data/Scripts`. Stage the PSC, PEX, and ESP in a dedicated build directory or MO2 test mod so normal moreHUD builds cannot overwrite them.

## Workflow

1. Identify the game root, Papyrus compiler, matching `Data/Source/Scripts`, flags file, SSEEdit installation, test plugin name, quest editor ID, and script name. Do not commit workstation-specific absolute paths.
2. Write the test as a `Quest` script. Prefer `OnInit()` or a quest stage fragment for automatic entry. Skyrim's console does not provide Fallout's `cqf`; use `startquest`, `stopquest`, `resetquest`, or `setstage` for repeatable invocation.
3. Compile with [scripts/Compile-PapyrusTest.ps1](scripts/Compile-PapyrusTest.ps1). Confirm compiler exit code and the expected PEX timestamp before touching the ESP.
4. Read [references/sseedit-vmad.md](references/sseedit-vmad.md), then adapt the constants in [scripts/CreateQuestScriptAttachment.pas](scripts/CreateQuestScriptAttachment.pas). Run it through SSEEdit to create a test ESP with a quest and VMAD script attachment. Configure quest startup flags or stages to match the chosen entry mechanism.
5. Run [scripts/ValidateQuestScriptAttachment.pas](scripts/ValidateQuestScriptAttachment.pas) after editing. Treat the harness as ready only when the validator passes and the PEX is beside the ESP under the test mod's `Scripts` directory.
6. Package the staging directory as an MO2 zip only when requested. Never copy test PEX files into the production mod.

Keep generated ESP/PEX/zip files out of source control unless the user explicitly wants a maintained test fixture. Remove temporary harness source after the investigation, but preserve reusable skill resources.
