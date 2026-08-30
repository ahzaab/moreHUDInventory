unit UserScript;

const
  TestPluginName = 'REPLACE-Test.esp';
  TestQuestEditorID = 'REPLACETestQuest';
  TestScriptName = 'REPLACETestScript';

var
  TestFile: IInterface;

function Initialize: Integer;
var
  SkyrimFile, SkyrimQuestGroup, QuestGroup, TemplateQuest, QuestRecord,
    VMAD, Scripts, ScriptEntry: IInterface;
begin
  Result := 1;
  TestFile := AddNewFileName(TestPluginName);
  if not Assigned(TestFile) then begin
    AddMessage('Unable to create ' + TestPluginName);
    Exit;
  end;

  AddMasterIfMissing(TestFile, 'Skyrim.esm');
  SkyrimFile := FileByIndex(0);
  SkyrimQuestGroup := GroupBySignature(SkyrimFile, 'QUST');
  TemplateQuest := wbCopyElementToFile(
    ElementByIndex(SkyrimQuestGroup, 0), TestFile, True, True);

  QuestGroup := GroupBySignature(TestFile, 'QUST');
  QuestRecord := Add(QuestGroup, 'QUST', True);
  Remove(TemplateQuest);
  SetElementEditValues(QuestRecord, 'EDID', TestQuestEditorID);

  VMAD := Add(QuestRecord, 'VMAD', True);
  SetElementNativeValues(VMAD, 'Version', 5);
  SetElementNativeValues(VMAD, 'Object Format', 2);
  Scripts := ElementByPath(QuestRecord, 'VMAD\Data\Scripts');
  if not Assigned(Scripts) then
    Scripts := Add(VMAD, 'Scripts', True);
  if not Assigned(Scripts) then begin
    AddMessage('Could not create the VMAD scripts array');
    Exit;
  end;

  ScriptEntry := ElementAssign(Scripts, HighInteger, nil, False);
  if not Assigned(ScriptEntry) then begin
    AddMessage('Could not add the VMAD script entry');
    Exit;
  end;
  SetElementEditValues(ScriptEntry, 'ScriptName', TestScriptName);
  SetElementNativeValues(ScriptEntry, 'Flags', 0);

  AddMessage('Created ' + TestPluginName + ' with quest ' +
    TestQuestEditorID + ' and script ' + TestScriptName);
  Result := 0;
end;

end.
