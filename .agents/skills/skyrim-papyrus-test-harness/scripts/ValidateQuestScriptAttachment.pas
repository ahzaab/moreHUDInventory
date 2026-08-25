unit UserScript;

const
  TestPluginName = 'REPLACE-Test.esp';
  TestQuestEditorID = 'REPLACETestQuest';
  TestScriptName = 'REPLACETestScript';

function ContainsScript(Element: IInterface): Boolean;
var
  Index: Integer;
begin
  Result := False;
  if SameText(Name(Element), 'ScriptName') then begin
    Result := SameText(GetEditValue(Element), TestScriptName);
    Exit;
  end;
  for Index := 0 to Pred(ElementCount(Element)) do
    if ContainsScript(ElementByIndex(Element, Index)) then begin
      Result := True;
      Exit;
    end;
end;

function Initialize: Integer;
var
  Index: Integer;
  TestFile, TestQuest: IInterface;
begin
  Result := 1;
  for Index := 0 to Pred(FileCount) do
    if SameText(GetFileName(FileByIndex(Index)), TestPluginName) then
      TestFile := FileByIndex(Index);

  if Assigned(TestFile) then
    TestQuest := MainRecordByEditorID(
      GroupBySignature(TestFile, 'QUST'), TestQuestEditorID);

  if Assigned(TestQuest) and
    ContainsScript(ElementBySignature(TestQuest, 'VMAD')) then begin
    AddMessage('PASS: quest VMAD contains ' + TestScriptName);
    Result := 0;
  end else
    AddMessage('FAIL: expected quest/script attachment was not found');
end;

end.
