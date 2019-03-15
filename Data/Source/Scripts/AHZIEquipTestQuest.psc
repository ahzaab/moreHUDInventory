Scriptname AHZIEquipTestQuest extends Quest  

Float fVersion
Int counter
int[] myArray
string[] myStringArray

Event OnInit()
	Maintenance() ; OnPlayerLoadGame will not fire the first time
EndEvent

Event OnKeyUp(Int KeyCode, Float HoldTime)
	
	If KeyCode == 55
		Debug.Notification("NUM* is registered and has been released after being held for " + HoldTime + " seconds")
		AhzMoreHudIE.AddIconItem(counter, "Icon1")
	EndIf

	If KeyCode == 181
		Debug.Notification("NUM* is registered and has been released after being held for " + HoldTime + " seconds")
		AhzMoreHudIE.AddIconItem(counter, "Icon2")
	EndIf

	If KeyCode == 74
		Debug.Notification("NUM- is registered and has been released after being held for " + HoldTime + " seconds")
		AhzMoreHudIE.AddIconItem(counter, "Icon2")
		AhzMoreHudIE.AddIconItems(myArray, myStringArray)
	EndIf

	counter = counter + 1
EndEvent 


Function Maintenance()
	If fVersion < 1.01 ; <--- Edit this value when updating
		fVersion = 1.01 ; and this
		Debug.Notification("Now running YourModName version: " + fVersion)
		; Update Code
	EndIf
	; Other maintenance code that only needs to run once per save load

	RegisterForKey(55)  ;NUM*
	RegisterForKey(181) ;  NUM/
      RegisterForKey(74 ) ; NUM-
	counter = 0
	myArray= new int[128]
       myStringArray = new string[128]
	int i = 0
	while i < myArray.Length
			myArray[i] = 1000 + i
			myStringArray[i] = "Icon" +  i
			i += 1
	endWhile

EndFunction


; Scriptname AhzMoreHudIE Hidden

; Gets the version e.g 10008 for 1.0.8
; int Function GetVersion() global native

; Returns true if the Item ID is registered
; bool Function IsIconItemRegistered(int aItemId) global native

; Add an Item ID with the icon that you want to display
; Function AddIconItem(int aItemId, string aIconName) global native

; Add an Item ID
; Function RemoveIconItem(int aItemId) global native

; Adds an array of Item ID's with the icon that you want to display
; Function AddIconItems(int[] aItemIds, string[] aIconNames) global native

; Removes an array of Item ID's 
; Function RemoveIconItems(int[] aItemIds) global native

