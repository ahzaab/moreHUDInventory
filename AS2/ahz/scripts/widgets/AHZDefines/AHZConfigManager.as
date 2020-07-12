import flash.utils.*;
import gfx.events.EventDispatcher;

class AHZConfigManager
{
  /* CONSTANTS */
  
	private static var CONFIG_PATH = "moreHUDIE/config.txt";
	private static var EXPORTED_PATH = "exported/moreHUDIE/config.txt";
	private static var EXPORTED_PREFIX = "exported/";
	private static var eventObject: Object;
	private static var configObject: Object;
	private static var exportedTried:Boolean = false;
	private static var lv:LoadVars;
		
  /* INITIALIATZION */
  
  	private static var managerSetup:Boolean = false;
  
	public static function loadConfig(a_scope: Object, a_loadedCallBack: String, a_errorCallBack: String):Void
	{		
	_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("loadConfig start");
		if (managerSetup){
			return;
		}
		managerSetup = true;
		eventObject = {};
		EventDispatcher.initialize(eventObject);
		eventObject.addEventListener("configLoad", a_scope, a_loadedCallBack);
		eventObject.addEventListener("configError", a_scope, a_errorCallBack);		
		
		lv = new LoadVars();
		lv.onData = parseConfigData;
		lv.onLoad = onLoadConfig;
		lv.load(CONFIG_PATH);
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("loadConfig end");
	}
		
	public static function ResolvePath(path:String):String{
		if (!path){
			return undefined;
		}
		
		if (configObject.useExported && path.toLowerCase().indexOf(EXPORTED_PREFIX)<0)
		{
			return EXPORTED_PREFIX + path;
		}	
		else
		{
			return path;
		}
	}
		
	private static function onLoadConfig(success:Boolean):Void {
		if (success){
			configObject["useExported"] = exportedTried;
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("loadConfig loaded successfully");
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("configObject.useExported: " + configObject.useExported);
			eventObject.dispatchEvent({type: "configLoad", config: configObject});
		}
		else{
			if (!exportedTried){
				_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Loading Failed, trying: " + EXPORTED_PATH);
				exportedTried = true;
				lv.load(EXPORTED_PATH);
			}
			else
			{
				_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("loadConfig loaded with error");
				eventObject.dispatchEvent({type: "configError", config: undefined});
			}
		}	
	}
		
	private static function parseConfigData(a_data:String): Void
	{
		configObject = {};
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("parseConfigData");
		
		if (!a_data){
			//eventObject.dispatchEvent({type: "configError", config: undefined});
			onLoadConfig(false);
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
		
		onLoadConfig(true);
		//eventObject.dispatchEvent({type: "configLoad", config: configObject});
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