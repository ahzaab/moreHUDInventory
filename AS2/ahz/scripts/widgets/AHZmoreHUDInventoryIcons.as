import ahz.scripts.widgets.AHZDefines.AHZCCSkyUIFrames;
import ahz.scripts.widgets.AHZDefines.AHZCCSurvFrames;
import ahz.scripts.widgets.AHZDefines.AHZVanillaFrames;
import flash.display.BitmapData;
import flash.filters.DropShadowFilter;
import mx.managers.DepthManager;

class ahz.scripts.widgets.AHZmoreHUDInventoryIcons extends MovieClip
{
    //Widgets
    public var IconHolder_mc: MovieClip;
	public var IconHolder_tf: TextField;
	var iconTextFormat:TextFormat;
	var _imageSubs:Array;
	
    public function AHZmoreHUDInventoryIcons()
    {
        super(); 
			_global.skse.plugins.AHZmoreHUDInventory.AHZLog("--AHZmoreHUDInventoryIcons--", false);	
		_imageSubs = new Array();
		//IconHolder_tf = IconHolder_mc.IconHolder_tf;
		IconHolder_tf.verticalAlign = "center";
        IconHolder_tf.textAutoSize = "shrink";
		IconHolder_tf.text = ""
    }

	public function SetHeight(heightValue:Number):Void {
		IconHolder_tf._height = heightValue;
	}

	public function SetWidth(widthValue:Number):Void {
		IconHolder_tf._width = widthValue;
	}

    public function Reset()
    {
		_global.skse.plugins.AHZmoreHUDInventory.AHZLog("--RESET--", false);
		_imageSubs = new Array();
		IconHolder_tf.setImageSubstitutions(null);
        IconHolder_tf.html = false;
		IconHolder_tf.verticalAlign = "center";
        IconHolder_tf.textAutoSize = "shrink";
		IconHolder_tf.text = ""
    }
	
    public function SetString(textToAppend:String, asHtml:Boolean):Void
    {
		IconHolder_tf.html = asHtml;
		
		if (IconHolder_tf.html){
			IconHolder_tf.htmlText = textToAppend;
		}
		else{
			IconHolder_tf.text = textToAppend;
		}		
    }

    public function AppendString(textToAppend:String, asHtml:Boolean):Void
    {
		IconHolder_tf.html = asHtml;
		
		if (IconHolder_tf.html){
			var stringIndex:Number;
			stringIndex = IconHolder_tf.htmlText.lastIndexOf("</P></TEXTFORMAT>");
			var firstText:String = IconHolder_tf.htmlText.substr(0,stringIndex);
			var secondText:String = IconHolder_tf.htmlText.substr(stringIndex,IconHolder_tf.htmlText.length - stringIndex);
			IconHolder_tf.htmlText =  firstText + textToAppend + secondText;			
		}
		else{
			IconHolder_tf.text += textToAppend;
		}		
    }
	

    public function SetImage(imageName:String, asHtml:Boolean):Void
    {
		IconHolder_tf.html = asHtml;
		
		if (IconHolder_tf.html){
			IconHolder_tf.htmlText = "["+ imageName + "]";
		}
		else{
			IconHolder_tf.text = "["+ imageName + "]";
		}	
		
		addImageSub(imageName, 20,20);
		if (_imageSubs.length)
		{
			IconHolder_tf.setImageSubstitutions(_imageSubs);
		}		
    }

    public function AppendImage(imageName:String, asHtml:Boolean):Void
    {
		IconHolder_tf.html = asHtml;
		
		if (IconHolder_tf.html){
			var stringIndex:Number;
			stringIndex = IconHolder_tf.htmlText.lastIndexOf("</P></TEXTFORMAT>");
			var firstText:String = IconHolder_tf.htmlText.substr(0,stringIndex);
			var secondText:String = IconHolder_tf.htmlText.substr(stringIndex,IconHolder_tf.htmlText.length - stringIndex);
			IconHolder_tf.htmlText =  firstText + "["+ imageName + "]" + secondText;			
		}
		else{
			IconHolder_tf.text += "["+ imageName + "]";
		}
		
		addImageSub(imageName, 20,20);
		if (_imageSubs.length)
		{
			IconHolder_tf.setImageSubstitutions(_imageSubs);
		}		
    }	
	
	// Private
	function getImageSub(imageName:String):Object
	{
		var i:Number;
		_global.skse.plugins.AHZmoreHUDInventory.AHZLog("--getImageSub--", false);
        for (i = 0; i < _imageSubs.length; i++)
		{
			_global.skse.plugins.AHZmoreHUDInventory.AHZLog("    _imageSubs[" + i + "]:" + _imageSubs[i], false);
			for (var o in _imageSubs[i])
			{
				_global.skse.plugins.AHZmoreHUDInventory.AHZLog("      " + o + ":" + _imageSubs[i][o], false);
			}
			if (_imageSubs[i].subString && _imageSubs[i].subString == "[" + imageName + "]")
			{
				return _imageSubs[i];
			}
		}
		
		return null;
	}
	
	function removeImageSub(imageName:String)
	{
		var i:Number;
        for (i = 0; i < _imageSubs.length; i++)
		{
			if (_imageSubs[i].subString && _imageSubs[i].subString == "[" + imageName + "]")
			{
				_imageSubs = _imageSubs.splice(i,1);
			}
		}
	}

	function addImageSub(imageName:String, imageWidth:Number, imageHeight:Number)
	{
		_global.skse.plugins.AHZmoreHUDInventory.AHZLog("--addImageSub--", false);
		if (getImageSub(imageName))   // Already exists
		{
			return;
		}
		
	 	var loadedImage:BitmapData = BitmapData.loadBitmap(imageName);
		
		if (loadedImage)
		{
			_imageSubs.push({ subString:"[" + imageName + "]", image:loadedImage, width:imageWidth, height:imageHeight, id:"id" + imageName });
		}
	}	
}