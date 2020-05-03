﻿$sourcePath = "$($Env:Skyrim64Path)\Data"
$destinationDataPath = "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUDInventory\SKSE64\skse64\moreHUDInventory\Data"
$destinationAS2Path = "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUDInventory\SKSE64\skse64\moreHUDInventory\AS2"

if (!$(Test-Path "$destinationDataPath\Interface"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface"
    New-Item -ItemType Directory "$destinationDataPath\Source\Scripts"
}

Copy-Item "$sourcePath\Source\Scripts\ahzMoreHudIE.psc" -Destination "$destinationDataPath\Source\Scripts"
Copy-Item "$sourcePath\AHZmoreHUDInventory.esl" -Destination "$destinationDataPath"
Copy-Item "$sourcePath\AHZmoreHUDInventory.esl" -Destination "$destinationDataPath"
Copy-Item "$sourcePath\Interface\AHZmoreHUDInventory.swf" -Destination "$destinationDataPath\Interface"

if ($(Test-Path "$sourcePath\Interface\moreHUDIE"))
{
    if (!$(Test-Path "$destinationDataPath\Interface\moreHUDIE"))
    {
        New-Item -ItemType Directory "$destinationDataPath\Interface\moreHUDIE"
    }
    Copy-Item "$sourcePath\Interface\moreHUDIE\*.*" -Destination "$destinationDataPath\Interface\moreHUDIE"
}


#AS2


Copy-Item "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUDInventory\ScaleForm\src\HUDWidgets\*" -Destination $destinationAS2Path -Exclude .git* -Recurse -Force