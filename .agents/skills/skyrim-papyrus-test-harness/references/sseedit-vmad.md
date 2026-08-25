# SSEEdit quest and VMAD attachment

SSEEdit/xEdit can execute Pascal user scripts from its `Edit Scripts` directory. Copy the skill templates there, edit their constants for the harness, and run the creation script with `Skyrim.esm` loaded. Exact command-line switches vary by xEdit build; inspect that installation's help before automating a headless run.

The creation template deliberately creates a `QUST` group from a copied Skyrim quest before adding the new record. This avoids failures seen when attempting to create the first quest group directly in a new plugin. It then removes the copied template record.

For a quest script attachment, the record must contain:

- `EDID` matching the chosen quest editor ID.
- Quest flags appropriate for automatic startup when `OnInit()` is the entry point.
- `VMAD` version 5 and object format 2.
- A `VMAD\Data\Scripts` entry whose `ScriptName` exactly matches the compiled PSC/PEX basename.

After saving, reopen the plugin and run the validator. Do not infer success from the ESP existing: malformed VMAD serialization can leave the script invisible to the game. Verify the test mod contains both the ESP and `Scripts/<ScriptName>.pex`.

Useful in-game reset sequence:

```text
stopquest <QuestEditorID>
resetquest <QuestEditorID>
startquest <QuestEditorID>
```

Use `setstage <QuestEditorID> <stage>` when the harness is stage-driven.
