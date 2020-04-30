import flash.utils.*;
import gfx.events.EventDispatcher;

class AHZConfigManager
{
  /* CONSTANTS */
  
	private static var CONFIG_PATH = "moreHUDIE/config.txt";

	private static var eventObject: Object;

	private static var configObject: Object;
		
  /* INITIALIATZION */
  
  	private static var managerSetup:Boolean = false;
  
	public static function loadConfig(a_scope: Object, a_loadedCallBack: String, a_errorCallBack: String):Void
	{		
		_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadConfig start for '" + CONFIG_PATH + "'" , false);
		if (managerSetup){
			return;
		}
		managerSetup = true;
		eventObject = {};
		EventDispatcher.initialize(eventObject);
		eventObject.addEventListener("configLoad", a_scope, a_loadedCallBack);
		eventObject.addEventListener("configError", a_scope, a_errorCallBack);		
		
		var lv = new LoadVars();
		lv.onData = parseConfigData;
		lv.onLoad = onLoadConfig;
		lv.load(CONFIG_PATH);
		_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadConfig end", false);
	}
		
	private static function onLoadConfig(success:Boolean):Void {
		if (success){
			_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadConfig loaded successfully", false);
			eventObject.dispatchEvent({type: "configLoad", config: configObject});
		}
		else{
			_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadConfig loaded with error", false);
			eventObject.dispatchEvent({type: "configError", config: undefined});
		}	
	}
		
	private static function parseConfigData(a_data:String): Void
	{
		configObject = {};
		_global.skse.plugins.AHZmoreHUDInventory.AHZLog("parseConfigData", false);
		
		if (!a_data){
			eventObject.dispatchEvent({type: "configError", config: undefined});
			return;
		}
		
		var lines = a_data.split("\r\n");
		if (lines.length == 1)
			lines = a_data.split("\n");

		var section = undefined;

		for (var i = 0; i < lines.length; i++) {

			// Comment
			if (lines[i].charAt(0) == ";")
				continue;

			// Section start
			if (lines[i].charAt(0) == "[") {
				section = lines[i].slice(1, lines[i].lastIndexOf("]"));					
				continue;
			}

			if (lines[i].length < 3 || section == undefined)
				continue;
			
			// Get raw key string
			var key = normalize(lines[i].slice(0, lines[i].indexOf("=")));
			if (key == undefined)
				continue;
				
			// Detect value type & extract
			var val = parseValue(normalize(lines[i].slice(lines[i].indexOf("=") + 1)));	
			if (val == undefined)
				continue;
					
			configObject[key.toLowerCase() + ":" + section.toLowerCase()] = val;
		}		
		
		eventObject.dispatchEvent({type: "configLoad", config: configObject});
	}
	
	private static function extract(a_str: String, a_startChar: String, a_endChar: String): String
	{
		return a_str.slice(a_str.indexOf(a_startChar) + 1,a_str.lastIndexOf(a_endChar));
	}

	// Remove comments and leading/trailing white space
	private static function normalize(a_str: String): String
	{
		if (a_str.indexOf(";") > 0)
			a_str = a_str.slice(0,a_str.indexOf(";"));

		var i = 0;
		while (a_str.charAt(i) == " " || a_str.charAt(i) == "\t")
			i++;

		var j = a_str.length - 1;
		while (a_str.charAt(j) == " " || a_str.charAt(j) == "\t")
			j--;

		return a_str.slice(i,j + 1);
	}

	private static function unescape(a_str: String): String 
	{
		a_str = a_str.split("\\n").join("\n");
		a_str = a_str.split("\\t").join("\t");
		return a_str;
	}
	
	private static function parseValue(a_str: String): Object
	{
		if (a_str == undefined)
			return undefined;
			
		// Number?
		if (!isNaN(a_str)) {
			return Number(a_str);
			
		// Bool true?
		} else if (a_str.toLowerCase() == "true") {
			return true;
			
		// Bool false?
		} else if (a_str.toLowerCase() == "false") {
			return false;

		// Undefined?
		} else if (a_str.toLowerCase() == "undefined") {
			return undefined;
			
		// Explicit String?
		} else if (a_str.charAt(0) == "'") {
			return extract(a_str, "'", "'");
		}
		
		// Default String
		return a_str;
	}
}