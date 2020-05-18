
# Description

This Repositiory is for the Scaleform elements used by the [moreHUD Inventory Edition](https://www.nexusmods.com/skyrimspecialedition/mods/18619) mod for Skyrim Special Edition.  
The Scaleform elements work in conjunction with the [ahzaab/moreHUDInventory](https://github.com/ahzaab/moreHUDInventory) SKSE64 plugin.  

## How it Works

* The SKSE64 plugin loads this scaleform movie dynamically into the open menu
* The associated ActionScript 2.0 code reads the extended data that was extended by the [SKSE64 Plugin](https://github.com/ahzaab/moreHUDInventory).
* In addition the ActionScript 2.0 code dynamically resizes the item card based on the current item card frame or "mode".
  * Different frame tables exists for:
    * Vanilla (Survival Mode Frames Exist even when you don't own the Survival Mode CC Content)
    * [SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604)
    * [SkyUI Survival Mode Integration](https://www.nexusmods.com/skyrimspecialedition/mods/17729)
    
  _**Note: If there are other mods that change the frames, then they will be added as they are found.**_

## Installation
The compiled .swf is installed in the Skyrim Data Folder (through a bsa file) to `Data/Interfaces/exported` as well as `Data/Interfaces/` for maximum compatibility.

## Does it need papyrus?
No.  Processing is in the SKSE64 plugin and the ActionScript code

## Configuration
Starting in version 1.0.15 Mod authors can change parameters and load custom large item card backgrounds as shown [here](https://github.com/ahzaab/moreHUDInventory/tree/master/Data/Interface/moreHUDIE).
