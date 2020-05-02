
intrinsic class skse.plugins.AHZmoreHUDInventory
{
    static function InstallHooks():Void;
    static function ShowBookRead():Boolean;
    static function AHZLog(data:String, isReleaseMode:Boolean):Void;
    static function GetCurrentMenu():String;
	static function EnableItemCardResize():Boolean;
	static function GetWasBookRead():Boolean;
	static function GetIconForItemId(formId: Number, formName:String, returnValue:Object):Void;	
}