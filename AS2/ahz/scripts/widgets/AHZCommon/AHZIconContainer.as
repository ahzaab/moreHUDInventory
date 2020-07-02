import flash.utils.*;
import gfx.events.EventDispatcher;
import flash.display.BitmapData;

class AHZIconContainer
{
  /* CONSTANTS */
  	private static var MAX_CONCURRENT_ICONS:Number = 32;
	private static var ICON_WIDTH:Number = 20;
	private static var ICON_HEIGHT:Number = 20;
	private static var ICON_XOFFSET:Number = -6;//-7;
	private static var ICON_YOFFSET:Number = 0;//-7;

	/* Static */
	private static var eventObject: Object;
  	private static var managerSetup:Boolean = false;		
		
  /* INITIALIATZION */
  
  	public var IconContainer_mc:MovieClip;
  	private var iconLoader:MovieClipLoader;
  	private var loadedIcons:Array;
    private var _imageSubs:Array;
	private var _currentImageIndex:Number;
  	private var _tf:TextField;
  	private var _lastX:Number;
	private var _textFormat:TextFormat;
	
	public function set textField(textFieldValue:TextField):Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~set textField~ ", false);
		_tf = textFieldValue;
		_tf.textAutoSize = "shrink";
		_tf.verticalAlign = "center";
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("_tf: " + _tf, false);
		_currentImageIndex = 0;
		_imageSubs = new Array();
		loadedIcons = new Array();			
		_lastX = (_tf.getLineMetrics(0).x + _tf._x);
		updatePosition();
	}
	
	public function get _y():Number
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~set _y~ ", false);
		return _tf._y;
	}	
	
	public function set _y(yValue:Number):Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~set _y~ " + yValue, false);
		if (_tf._y != yValue)
		{
			_tf._y = yValue;
			updatePosition();
		}
	}	
	
	public function get _x():Number
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~get _x~ ", false);
		return _tf._x;
	}	
	
	public function set _x(xValue:Number):Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~set _x~ " +xValue , false);
		if (_tf._x != xValue)
		{
			_tf._x = xValue;
			updatePosition();
		}
	}		
	
	public function get textField():TextField
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~get textField~ ", false);
		return _tf;
	}	
	
	public function set _alpha(alphaValue:Number):Void{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~set _alpha~ " + alphaValue, false);
		if (_tf._alpha != alphaValue)
		{
			_tf._alpha = alphaValue;
			for (var i = 0; i <= _currentImageIndex; i++)
			{
				loadedIcons[i]._alpha = _tf._alpha;
			}			
		}
	}
	
	public function get _alpha():Number{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~get _alpha~ ", false);
		return _tf._alpha;
	}	
	
	public function set text(textValue:String):Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~set text~ " + textValue, false);
		if (!textValue){
			Clear();
			return;
		}
		if (_tf.text != textValue)
		{
			_tf.text = textValue;
			updatePosition();
		}
	}
	
	public function get text():String
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~get text~ " + _tf.text, false);
		return _tf.text;
	}	
	
	public function set textWidth(textWidth:Number):Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~set textWidth~ " + textWidth, false);
		if (textWidth != _tf._width)
		{
			_tf._width = textWidth;
			updatePosition();
		}
	}
	
	public function get textWidth():Number
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~get textWidth~ ", false);
		return _tf._width;
	}	
	
	public function set htmlText(textValue:String):Void
	{
		if (!textValue){
			Clear();
			return;
		}		
		//if (_tf.htmlText != textValue)
		//{
			_tf.htmlText = textValue;
			updatePosition();
		//}
	}
	
	public function get htmlText():String
	{
		return _tf.htmlText;
	}	
	
	public function set html(htmlValue:Boolean):Void
	{
		_tf.html = htmlValue;
		//updatePosition();
	}
	
	public function get html():Boolean
	{
		return _tf.html;
	}	
	
	public function setNewTextFormat(tf:TextFormat) : Void
	{
		_textFormat = tf;
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("    ~setNewTextFormat~ " + tf,  false);
		_tf.setNewTextFormat(tf);
		updatePosition();
	}
	
  	public function AHZIconContainer()
	{	
	}
	
	public function appendHtml(newHtml:String):Void
	{
		_tf.appendHtml(newHtml);
		updatePosition();
	}
	
	function updatePosition ():Void {
		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("    ~updatePosition~ ", false);
		if (!loadedIcons.length)
		{
			return;
		}
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("    ~updatePosition Icons Loaded~ ", false);
						
		var newLineMetrics = _tf.getLineMetrics(0);
		var xDelta = _lastX - (newLineMetrics.x + _tf._x);
		for (var i = 0; i < _currentImageIndex; i++)
		{
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("old loadedIcons["+i+"]._x: " + loadedIcons[i]._x, false);
			loadedIcons[i]._x = loadedIcons[i]._x - (xDelta);
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("new loadedIcons["+i+"]._x: " + loadedIcons[i]._x, false);
			loadedIcons[_currentImageIndex]._y = (_tf._y + _tf._height) - ( _tf._height/ 2) - (ICON_HEIGHT / 2);
		}		
		_lastX = (newLineMetrics.x + _tf._x);
	}
	
	public function Load(s_filePath:String, a_scope: Object, a_loadedCallBack: String, a_errorCallBack: String):Void
	{		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~ LOAD ~ '" + s_filePath + "'" , false);
				
		if (managerSetup){
			return;
		}
		managerSetup = true;
		eventObject = {};
		EventDispatcher.initialize(eventObject);
		eventObject.addEventListener("iconsLoaded", a_scope, a_loadedCallBack);
		eventObject.addEventListener("iconLoadError", a_scope, a_errorCallBack);		
		
		for (var i:Number = 0; i < MAX_CONCURRENT_ICONS; i++)
		{
			var clip = MovieClip(_tf._parent).createEmptyMovieClip("clip" + i, MovieClip(_tf._parent).getNextHighestDepth());
			clip._y = _tf._y;
			clip._x = _tf._x;
			iconLoader = new MovieClipLoader();
			iconLoader.addListener(this);
			iconLoader.loadClip(s_filePath, clip);
		}
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadIcons end", false);
	}
								
	public function onLoadInit(a_mc: MovieClip): Void
	{		
		a_mc._quality = "BEST";
		a_mc.gotoAndStop("ahzEmpty");		
		loadedIcons.push(a_mc);
		if (loadedIcons.length == MAX_CONCURRENT_ICONS)
		{
			eventObject.dispatchEvent({type: "iconsLoaded", tf: this});	
		}
	}
	
	public function onLoadError(a_mc:MovieClip, a_errorCode: String): Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("Error Loading Icon: " + a_errorCode, false);
		eventObject.dispatchEvent({type: "iconLoadError", error: a_errorCode});	
	}
	
	public function GetImageSub(a_imageName:String):Object
	{
		var i:Number;
        for (i = 0; i < _imageSubs.length; i++)
		{
			if (_imageSubs[i].subString && _imageSubs[i].subString == "[" + a_imageName + "]")
			{
				return _imageSubs[i];
			}
		}
		
		return null;
	}

	private function getDefaultHtml(textValue:String):String
	{
		var returnValue = "<font face=\'"+_textFormat.font+"\' size=\'"+_textFormat.size+"\' color=\'#"+_textFormat.color.toString(16)+"\'>" + textValue + "</font>";
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("returnValue: " + returnValue, false);
		return returnValue;
	}
	
	function appendImage(a_imageName:String):Void
	{		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~AppendImage~ " + a_imageName, false);
		var loadedImage:BitmapData;
		if (loadedIcons.length)
		{
	 		loadedImage = BitmapData.loadBitmap("ahzEmpty");
		}
		else
		{		
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("OLD LOADING", false);
			loadedImage = BitmapData.loadBitmap(a_imageName);
		}
		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadedImage: " + loadedImage, false);
		if (loadedImage)
		{
			if (loadedIcons.length || !GetImageSub(a_imageName))   // Already exists
			{
				_imageSubs.push({ subString:"[" + a_imageName + "]", image:loadedImage, width:ICON_WIDTH, height:ICON_WIDTH, id:"id" + a_imageName });
			}	
		}
				
		if (_imageSubs.length)
		{
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("_tf.html: " + _tf.html, false);
			
			// get the line metrics before addeding the next image
			var currentLineMetrics = _tf.getLineMetrics(0);
			if (_tf.html) 
			{
				_tf.appendHtml(getDefaultHtml("[" + a_imageName + "]"));
			}
			else
			{
				_tf.text += "[" + a_imageName + "]";
			}			
			
			_tf.setImageSubstitutions(_imageSubs);
			
			if (loadedIcons.length){	
				//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("SHIFT WITH THIS VALUE: " + _tf.text, false);
				loadedIcons[_currentImageIndex].gotoAndStop(a_imageName);
				loadedIcons[_currentImageIndex]._quality = "BEST";
				loadedIcons[_currentImageIndex]._height = ICON_HEIGHT;
				loadedIcons[_currentImageIndex]._width = ICON_WIDTH;
				loadedIcons[_currentImageIndex]._x = (currentLineMetrics.x + currentLineMetrics.width) + ICON_XOFFSET + _tf._x ;
				loadedIcons[_currentImageIndex]._y = (_tf._y + _tf._height) - ( _tf._height/ 2) - (ICON_HEIGHT / 2);
				updatePosition();
				
				if (_currentImageIndex < loadedIcons.length - 1)
				{
					_currentImageIndex++;			
				}
			}
		}	
	}	
	
    public function Clear()
    {
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~Clear~", false);
		for (var i:Number = 0; i < loadedIcons.length; i++)
		{
			loadedIcons[i].gotoAndStop("ahzEmpty");
		}			
		_tf.setImageSubstitutions(null);
        _tf.html = false;
		_tf.text = ""	
		for (var i:Number = 0; i < loadedIcons.length; i++)
		{
			loadedIcons[i]._x = 0;
			loadedIcons[i]._y = 0;
		}			
		_currentImageIndex = 0;
		_imageSubs = new Array();
		_lastX = (_tf.getLineMetrics(0).x + _tf._x);
    }
}