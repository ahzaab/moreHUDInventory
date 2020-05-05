import flash.utils.*;
import gfx.events.EventDispatcher;

class AHZIconContainer extends MovieClip
{
  /* CONSTANTS */
  	private static var MAX_CONCURRENT_ICONS:Number = 32;


	private static var eventObject: Object;
		
  /* INITIALIATZION */
  
  	private var iconLoader:MovieClipLoader;
	private var iconContainer:MovieClip;
  
  	private static var managerSetup:Boolean = false;
  
	public function loadIcons(s_filePath:String, a_scope: Object, a_loadedCallBack: String, a_errorCallBack: String):Void
	{		
		_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadIcons start for '" + s_filePath + "'" , false);
				
		if (managerSetup){
			return;
		}
		managerSetup = true;
		eventObject = {};
		EventDispatcher.initialize(eventObject);
		eventObject.addEventListener("configLoad", a_scope, a_loadedCallBack);
		eventObject.addEventListener("configError", a_scope, a_errorCallBack);		
		iconContainer = this.createEmptyMovieClip("iconContainer", this.getNextHighestDepth());
		iconLoader = new MovieClipLoader();
		iconLoader.addListener(this);
		iconLoader.loadClip(s_filePath, iconContainer);
		
		_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadIcons end", false);
		
		
		
		//eventObject.dispatchEvent({type: "configLoad", config: configObject});
	}
			
	public function onLoadInit(a_mc: MovieClip): Void
	{
		a_mc.gotoAndStop('ahzEye');
				
	}
	
	public function onLoadError(a_mc:MovieClip, a_errorCode: String): Void
	{
	}
}