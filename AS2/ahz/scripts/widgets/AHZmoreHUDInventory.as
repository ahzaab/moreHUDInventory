import ahz.scripts.widgets.AHZDefines.AHZCCSkyUIFrames;
import ahz.scripts.widgets.AHZDefines.AHZCCSurvFrames;
import ahz.scripts.widgets.AHZDefines.AHZVanillaFrames;
import flash.display.BitmapData;
import flash.filters.DropShadowFilter;
import mx.managers.DepthManager;

class ahz.scripts.widgets.AHZmoreHUDInventory extends MovieClip
{
	//Widgets
	public var itemCard: MovieClip;
	public var rootMenuInstance:MovieClip;
	public var cardBackground:MovieClip;
	public var additionDescriptionHolder:MovieClip;
	static var IconContainer:AHZIconContainer = new AHZIconContainer();
	var iconTextFormat:TextFormat;

	// Public vars

	// Options

	// private variables
	private var isSkyui:Boolean = false;
	private var _platform: Number;
	private var _currentMenu: String;
	private var _equipHand: Number;
	private var iSelectedCategory: Number;
	private var originalWidth:Number;
	private var originalX:Number;
	private var newWidth:Number;
	private var newX:Number;
	private var prevHeight:Number;
	private var _lastFrame:Number = -1;
	private var _itemCardOverride:Boolean = false;
	private var _enableItemCardResize:Boolean = false;
	private var _craftingMenuCardShifted:Boolean = false;
	private var _isCCSurvCard:Boolean = false;
	private var _frameDefines:Object;
	private var _entryList:Object;
	private var _lastSelectedIndex:Number;
	private var _selectedIndex:Number;
	private var _selectedItem:Object;
	private var _lastItemCardVisibility:Boolean;
	private var _readyToUpdate:Boolean = false;
	private var _imageSubs:Array;
	private var LoadedLargeItemCard_mc:MovieClip;
	private var _config:Object;
	private var _iconContainerTextFormat:TextFormat;
	public var ICBackground_mc:MovieClip;
	private var mcLoader:MovieClipLoader;

	// Statics
	private static var hooksInstalled:Boolean = false;
	private static var AHZ_XMargin:Number           = 15;
	private static var AHZ_YMargin:Number           = 0;
	private static var AHZ_YMargin_WithItems:Number = 44;
	private static var AHZ_YMargin_Crafting:Number  = 20;
	private static var AHZ_FontScale:Number         = 0.90;
	private static var AHZ_CraftingMenuYShift:Number = -25;
	private static var AHZ_NormalALPHA:Number = 60;
	private static var AHZ_IconsFile:String = 'moreHUDIE/baseIcons.swf'
	private static var AHZ_ItemCardFile:String = 'moreHUDIE/baseLargeItemCard.swf'
	
	
	

	// Types from ItemCard
	private static var ICT_ARMOR: Number            = 1;
	private static var ICT_WEAPON: Number           = 2;
	private static var ICT_BOOK: Number             = 4;
	private static var ICT_POTION: Number           = 6;        // Used for Spell Tomes (shrug)
	private static var ICT_FOOD: Number = 5;
	private static var ICT_MISC: Number = 3;
	private static var ICT_KEY: Number = 9;
	private static var ICT_CRAFT_ENCHANTING: Number = 15;
	private static var ICT_HOUSE_PART: Number = 16;

	private static var BOOKFLAG_READ: Number        = 0x08;
	private static var SHOW_PANEL                   = 1;

	/* INITIALIZATION */

	// Get all unnamed movie clips.  Items that were not named in the fla file.
	// The property name for "unnamed" elements is in the format "instance<nnn>"
	function getUnnamedInstances(target:MovieClip, getOnlyMovieClips:Boolean) :Array
	{
		var arr:Array = new Array();
		for (var i in target)
		{
			var proName = i.toString();
			if (proName.indexOf("instance") == 0)
			{
				var unnamedIndex: String = proName.substring("instance".length);
				// If the value following the name "Instance" in the property name is a number
				if (int(unnamedIndex))
				{
					if (getOnlyMovieClips)
					{
						if (target[i] instanceof MovieClip)
						{
							arr.push(target[i]);
						}
					}
					else
					{
						arr.push(target[i]);
					}
				}
			}
		}
		return arr;
	}

	function GetBackgroundMovie():MovieClip
	{
		
		if (itemCard.itemInfo.type == ICT_CRAFT_ENCHANTING || itemCard.itemInfo.type == ICT_HOUSE_PART)
		{
			var background_mc:MovieClip;
			if (itemCard.itemInfo.effects != undefined && itemCard.itemInfo.effects.length > 0) {
				background_mc = itemCard.Enchanting_Background;
			} else {
				background_mc = itemCard.Enchanting_Slim_Background;
			}	
			if (background_mc)
			{
				return background_mc;
			}
		}
		
		if (itemCard["background"])
		{
			return MovieClip(itemCard["background"]);
		}	
		else
		{
			// Vanilla does not name the background clip.  So we must
			// enumerate for a movie clip without children.  It "Should" be the background
			// Unnamed instances will have a name in the form of "instance<nnn>"
			var arry:Array = getUnnamedInstances(itemCard, true);
			if (arry && arry.length > 0)
			{
				var i:Number;
				for (i = 0; i < arry.length; i++)
				{
					var children:Array = getUnnamedInstances(arry[i], false);

					// Skip movie clips that have unnamed children.  The background will not have any
					if (children && children.length > 0)
					{
						// Skip
					}
					else
					{
						return MovieClip(arry[i]);
					}
				}
			}
		}
		
		if (itemCard["Enchanting_Background"])
		{
			return MovieClip(itemCard["Enchanting_Background"]);
		}	
		
		if (itemCard["Enchanting_Slim_Background"])
		{
			return MovieClip(itemCard["Enchanting_Slim_Background"]);
		}	
						
		return undefined;
	}

	// This function is used to shift every item in the item card that is below the description
	// text field.  Since we are increasing the height of the text field we need to make sure the
	// items below do not overlap
	function GetItemsBelowDescription(targetMovie:MovieClip, targetTextField: TextField):Array
	{
		var arr:Array = new Array();

		for (var i in targetMovie)
		{
			if (targetMovie[i] instanceof TextField)
			{
				var target:TextField = TextField(targetMovie[i]);
				if (target._y > targetTextField._y && target._parent == targetMovie)
				{
					arr.push(TextField(targetMovie[i]));
				}
			}
			else if (targetMovie[i] instanceof MovieClip)
			{
				var target:MovieClip = MovieClip(targetMovie[i]);
				if (target._y > targetTextField._y && target._parent == targetMovie)
				{
					arr.push(MovieClip(targetMovie[i]));
				}
			}
		}
		return arr;
	}

	public function iconsLoaded(event:Object):Void
	{
		IconContainer.Clear();
		clipReady();
	}

	public function iconsLoadedError(event:Object):Void
	{
		IconContainer.Clear();
		clipReady();
	}

	public function AHZmoreHUDInventory()
	{
		super();
		this._alpha = 0;

		_currentMenu = _global.skse.plugins.AHZmoreHUDInventory.GetCurrentMenu();

		// Sneak in the main menu and enable extended data before any inventory menu can load
		if (_currentMenu == "Main Menu")
		{
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog(
				//"AHZmoreHUDInventory turning on extended data.", true);
			_global.skse.ExtendData(true);

			return;
		}

		rootMenuInstance = _root.Menu_mc;

		if (_currentMenu == "Crafting Menu")
		{
			rootMenuInstance = _root["Menu"];
		}

		mcLoader = new MovieClipLoader();
		mcLoader.addListener(this);
	}

	function initializeClips():Void
	{
		AttachIconHolder();
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("initializeClips", false);
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("this.ICBackground_mc: " + this.ICBackground_mc, false);
		if (_config[AHZDefines.CFG_LIC_PATH])
		{
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("Loading: " + AHZConfigManager.ResolvePath(_config[AHZDefines.CFG_LIC_PATH]), false);
			LoadedLargeItemCard_mc = this.createEmptyMovieClip("LoadedLargeItemCard_mc", ICBackground_mc.getDepth());
			mcLoader.loadClip(AHZConfigManager.ResolvePath(_config[AHZDefines.CFG_LIC_PATH]), LoadedLargeItemCard_mc);

			// Make the built-in movieclip disapear, it is being replaced, pending any loading errors
			ICBackground_mc._alpha = 0;
		}
		else
		{
			LoadedLargeItemCard_mc = ICBackground_mc;
			if (_config[AHZDefines.CFG_ICON_PATH])
			{
				IconContainer.Load(AHZConfigManager.ResolvePath(_config[AHZDefines.CFG_ICON_PATH]), this, "iconsLoaded", "iconsLoadedError");
			}
			else
			{
				clipReady();
			}
		}
	}

	function AttachIconHolder():Void
	{
		var tf:TextField;

		if (IconContainer.textField)
		{
			return;
		}

		// if the item card has this property name then this is SKYUI
		if (rootMenuInstance.itemCard)
		{
			itemCard = rootMenuInstance.itemCard;
			tf = (itemCard.createTextField("iconHolder", itemCard.getNextHighestDepth(), 0, 10, itemCard._width, 32));
		}
		// if the item card has this property name then this is Vanilla
		else if (rootMenuInstance.ItemCard_mc)
		{
			itemCard = rootMenuInstance.ItemCard_mc;
			tf = (itemCard.createTextField("iconHolder", itemCard.getNextHighestDepth(), 0, 10, itemCard._width, 32));
		}
		else if (_currentMenu == "Crafting Menu" && rootMenuInstance.ItemInfoHolder.ItemInfo)
		{
			itemCard = rootMenuInstance.ItemInfoHolder.ItemInfo;
			tf = (itemCard.createTextField("iconHolder", itemCard.getNextHighestDepth(), 0, 10, itemCard._width, 32));
		}

		if (tf)
		{
			// Creation the text field that holds the extra data
			tf.verticalAlign = "center";
			tf.textAutoSize = "shrink";
			tf.multiLine = false;
			//tf.border = true;
			_iconContainerTextFormat = new TextFormat();
			_iconContainerTextFormat.align = "center";
			var withoutHash = _config[AHZDefines.CFG_ICON_TEXT_FIELD_COLOR].substr(1,_config[AHZDefines.CFG_ICON_TEXT_FIELD_COLOR].length-1);
			_iconContainerTextFormat.color = parseInt(withoutHash, 16);
			_iconContainerTextFormat.size = 24;
			_iconContainerTextFormat.font = "$EverywhereMediumFont";
			tf.setNewTextFormat(_iconContainerTextFormat);
			tf.text = "";
			
			if (_config[AHZDefines.CFG_ICON_DROP_SHADOW])
			{
				var filter:DropShadowFilter = new DropShadowFilter(2,45,0,100,2,2,1.5);
				var filterArray:Array = new Array();
				filterArray.push(filter);
				tf.filters = filterArray;
			}
			else
			{
				var filterArray:Array = new Array();
				tf.filters = filterArray;
			}

			IconContainer.textField = tf;
			IconContainer.setNewTextFormat(_iconContainerTextFormat);
		}
	}

	function prepareConfigs() :Void
	{
		if (!_config)
			_config = {};

		if (!_config[AHZDefines.CFG_LIC_XOFFSET])
			_config[AHZDefines.CFG_LIC_XOFFSET] = 0;

		if (!_config[AHZDefines.CFG_LIC_YOFFSET])
			_config[AHZDefines.CFG_LIC_YOFFSET] = 0;

		if (!_config[AHZDefines.CFG_LIC_XSCALE])
			_config[AHZDefines.CFG_LIC_XSCALE] = 1.0;

		if (!_config[AHZDefines.CFG_LIC_YSCALE])
			_config[AHZDefines.CFG_LIC_YSCALE] = 1.0;

		if (!_config[AHZDefines.CFG_LIC_ALPHA])
			_config[AHZDefines.CFG_LIC_ALPHA] = AHZ_NormalALPHA;

		if (!_config[AHZDefines.CFG_LIC_DESCRIPTION_BOTTOMMARGIN])
			_config[AHZDefines.CFG_LIC_DESCRIPTION_BOTTOMMARGIN] = AHZ_YMargin;

		if (!_config[AHZDefines.CFG_LIC_DESCRIPTION_LEFTMARGIN])
			_config[AHZDefines.CFG_LIC_DESCRIPTION_LEFTMARGIN] = AHZ_XMargin;

		if (!_config[AHZDefines.CFG_LIC_DESCRIPTION_RIGHTMARGIN])
			_config[AHZDefines.CFG_LIC_DESCRIPTION_RIGHTMARGIN] = AHZ_XMargin;

		if (!_config[AHZDefines.CFG_LIC_DESCRIPTION_BOTTOMMARGIN_EXTRA])
			_config[AHZDefines.CFG_LIC_DESCRIPTION_BOTTOMMARGIN_EXTRA] = AHZ_YMargin_WithItems;

		if (!_config[AHZDefines.CFG_LIC_CRAFTING_YOFFSET])
			_config[AHZDefines.CFG_LIC_CRAFTING_YOFFSET] = AHZ_CraftingMenuYShift;

		if (!_config[AHZDefines.CFG_ICON_POS_EFFECT_COLOR])
			_config[AHZDefines.CFG_ICON_POS_EFFECT_COLOR] = '#189515';

		if (!_config[AHZDefines.CFG_ICON_NEG_EFFECT_COLOR])
			_config[AHZDefines.CFG_ICON_NEG_EFFECT_COLOR] = '#FF0000';
			
		if (!_config[AHZDefines.CFG_LIC_PATH])
			_config[AHZDefines.CFG_LIC_PATH] = AHZ_ItemCardFile;
					
		if (!_config[AHZDefines.CFG_ICON_PATH])
			_config[AHZDefines.CFG_ICON_PATH] = AHZ_IconsFile;
		
		// Kind of unnecessary if defaulting to false
		if (!_config[AHZDefines.CFG_ICON_DROP_SHADOW])
			_config[AHZDefines.CFG_ICON_DROP_SHADOW] = false;
		
		if (!_config[AHZDefines.CFG_LIC_DRAW_BORDERS])
			_config[AHZDefines.CFG_LIC_DRAW_BORDERS] = false;
			
		if (!_config[AHZDefines.CFG_LIC_DESCRIPTION_EXTRADATA_PADDING])
			_config[AHZDefines.CFG_LIC_DESCRIPTION_EXTRADATA_PADDING] = 5;			
			
		if (!_config[AHZDefines.CFG_ICON_TEXT_FIELD_COLOR])
			_config[AHZDefines.CFG_ICON_TEXT_FIELD_COLOR] = '#999999';
	}

	function configLoaded(event:Object):Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("configLoaded: " + event, false);
		_config = event.config;
		prepareConfigs();
		initializeClips();
	}

	function configError(event:Object):Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("configError: " + event, false);
		prepareConfigs();
		initializeClips();
	}

	function clipReady():Void
	{
		if (!LoadedLargeItemCard_mc)
		{
			LoadedLargeItemCard_mc = ICBackground_mc;
		}

		this._yscale = _config[AHZDefines.CFG_LIC_YSCALE] * 100;
		this._xscale = _config[AHZDefines.CFG_LIC_XSCALE] * 100;

		// if the item card has this property name then this is SKYUI
		if (rootMenuInstance.itemCard)
		{
			itemCard = rootMenuInstance.itemCard;
			_entryList = rootMenuInstance.inventoryLists.itemList._entryList;
			isSkyui = true;
		}
		// if the item card has this property name then this is Vanilla
		else if (rootMenuInstance.ItemCard_mc)
		{
			itemCard = rootMenuInstance.ItemCard_mc;
			_entryList = rootMenuInstance.InventoryLists_mc._ItemsList.EntriesA;
			isSkyui = false;
		}
		else if (_currentMenu == "Crafting Menu" && rootMenuInstance.ItemInfoHolder.ItemInfo)
		{
			itemCard = rootMenuInstance.ItemInfoHolder.ItemInfo;
			additionDescriptionHolder = rootMenuInstance.ItemInfoHolder.AdditionalDescriptionHolder;
			if (rootMenuInstance.InventoryLists.ItemsListHolder.List_mc.EntriesA)
			{
				_entryList = rootMenuInstance.InventoryLists.ItemsListHolder.List_mc.EntriesA;
				isSkyui = false;
			}
			else if (rootMenuInstance.InventoryLists.panelContainer.itemList._entryList)
			{
				_entryList = rootMenuInstance.InventoryLists.panelContainer.itemList._entryList;
				isSkyui = true;
			}
			else  // Cannot tell if its skyui or vanilla
			{
				_global.skse.plugins.AHZmoreHUDInventory.AHZLog(
					"Could not obtain a reference to the item card.", true)
				return;
			}
		}
		else
		{
			_global.skse.plugins.AHZmoreHUDInventory.AHZLog(
				"Could not obtain a reference to the item card.", true)
			return;
		}

		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("Frame Count: " + itemCard._totalframes, false);

		if (itemCard._totalframes >= AHZCCSurvFrames.MAX_FRAMES)
		{
			_global.skse.plugins.AHZmoreHUDInventory.AHZLog("Survival Card Detected", false);
			_isCCSurvCard = true;
			_frameDefines = AHZCCSurvFrames;
		}
		else if (itemCard._totalframes >= AHZCCSkyUIFrames.MAX_FRAMES)
		{
			_global.skse.plugins.AHZmoreHUDInventory.AHZLog("SkyUI Survival Integration Card Detected", false);
			_isCCSurvCard = true;
			_frameDefines = AHZCCSkyUIFrames;
		}
		else
		{
			_global.skse.plugins.AHZmoreHUDInventory.AHZLog("Vanilla Card Detected", false);
			_isCCSurvCard = false;
			_frameDefines = AHZVanillaFrames;
		}

		if (! hooksInstalled)
		{
			_global.skse.plugins.AHZmoreHUDInventory.InstallHooks();
			hooksInstalled = true;
		}
		_enableItemCardResize = _global.skse.plugins.AHZmoreHUDInventory.EnableItemCardResize();

		
		if (_enableItemCardResize)
		{
			cardBackground = GetBackgroundMovie();
			newWidth = this._width;
			originalX = cardBackground._x;
			originalWidth = cardBackground._width;
			newX = ((originalX - (newWidth - originalWidth)) / 2);
		}

		var t = this;
		t.setDepthTo(-16384);

		// Start monitoring the frames
		this.onEnterFrame = ItemCardOnEnterFrame;
	}

	function DrawBorders():Void {
		
		for (var a in itemCard)
		{
			if (itemCard[a] instanceof TextField)
			{
				var target:TextField = TextField(itemCard[a]);
				target.border = true;
			}
			else if (itemCard[a] instanceof MovieClip)
			{
				// Two levels is enough
				for (var b in itemCard[a])
				{
					if (itemCard[a][b] instanceof TextField)
					{
						var target:TextField = TextField(itemCard[a][b]);
						target.border = true;
					}
				}	
			}
		}
		
		for (var a in this)
		{
			if (this[a] instanceof TextField)
			{
				var target:TextField = TextField(this[a]);
				target.border = true;
			}
			else if (this[a] instanceof MovieClip)
			{
				// Two levels is enough
				for (var b in this[a])
				{
					if (this[a][b] instanceof TextField)
					{
						var target:TextField = TextField(this[a][b]);
						target.border = true;
					}
				}	
			}
		}	
		
		IconContainer.textField.border = true;
	}

	function GetSelectedIndex():Number
	{
		var selectedItemIndex:Number;
		if (isSkyui)
		{
			if (_currentMenu == "Crafting Menu")
			{
				selectedItemIndex = rootMenuInstance.InventoryLists.panelContainer.itemList._selectedIndex;
			}
			else
			{
				selectedItemIndex = rootMenuInstance.inventoryLists.itemList._selectedIndex;
			}
		}
		else //Vanilla
		{
			if (_currentMenu == "Crafting Menu")
			{
				selectedItemIndex = rootMenuInstance.InventoryLists.ItemsListHolder.List_mc.iSelectedIndex;
			}
			else
			{
				selectedItemIndex = rootMenuInstance.InventoryLists_mc._ItemsList.iSelectedIndex;
			}
		}
		return selectedItemIndex;
	}

	function ItemCardOnEnterFrame(): Void
	{
		var itemCardVisible:Boolean = (itemCard._alpha > 0 && rootMenuInstance._alpha == 100);
		if (itemCardVisible)
		{
			_readyToUpdate = true;
		}

		if (!_readyToUpdate)
		{
			return;
		}
		
		if (itemCard.itemInfo.type == ICT_CRAFT_ENCHANTING || itemCard.itemInfo.type == ICT_HOUSE_PART)
		{
			if (itemCard.itemInfo.effects != undefined && itemCard.itemInfo.effects.length > 0) {
				itemCard.Enchanting_Background._alpha = _config[AHZDefines.CFG_LIC_ALPHA];
				itemCard.Enchanting_Slim_Background._alpha = 0;
			} else {
				itemCard.Enchanting_Background._alpha = 0;
				itemCard.Enchanting_Slim_Background ._alpha = _config[AHZDefines.CFG_LIC_ALPHA];
			}	
		}

		if(_config[AHZDefines.CFG_LIC_DRAW_BORDERS])
		{
			DrawBorders();
		}

		if (_enableItemCardResize)
		{
			if (itemCard._currentframe != _lastFrame)
			{
				cardBackground = GetBackgroundMovie();
				_lastFrame = itemCard._currentframe;
				AdjustItemCard(_lastFrame);
			}

			if (_itemCardOverride)
			{
				cardBackground._alpha = 0;
				this._alpha = itemCard._alpha - (100 - _config[AHZDefines.CFG_LIC_ALPHA]);
			}
			else
			{
				this._alpha = 0;
				SetVanillaAlpha();
			}
		}
		else
		{
			// When not changing item card sizes, just make sure the alpha is set from the
			// config
			if (itemCard._currentframe != _lastFrame)
			{
				_lastFrame = itemCard._currentframe;
				this._alpha = 0;
				cardBackground = GetBackgroundMovie();
				SetVanillaAlpha();
			}
		}

		_selectedIndex = GetSelectedIndex();

		if (_lastSelectedIndex != _selectedIndex ||
				_selectedItem.formId != _entryList[_selectedIndex].formId ||
				_selectedItem.text != _entryList[_selectedIndex].text)
		{
			_lastSelectedIndex = _selectedIndex;
			_selectedItem = _entryList[_selectedIndex];
			UpdateItemCardInfo();
		}
		else
		{
			// Books are unique, they can be updated while in the menu and there is no way of
			// knowing if we are in the book menu without doing a bunch of hooks
			if (itemCard.itemInfo.type == ICT_BOOK &&
			(IconContainer.text == "" || !IconContainer.GetImageSub("ahzEye")) && 			// Not showing the book read icon
			_global.skse.plugins.AHZmoreHUDInventory.ShowBookRead() &&
			_global.skse.plugins.AHZmoreHUDInventory.GetWasBookRead(_selectedItem.formId))
			{
				IconContainer.appendImage("ahzEye");
			}
		}

		// If we advance to somethine like the confirmation frame, then make sure the icons are not visible
		// Dont wipe the value because we need to restore it when returning to the same item card
		if (itemCard._currentframe >= _frameDefines.EMPTY_LOW && itemCard._currentframe <= _frameDefines.EMPTY_HIGH)
		{
			IconContainer._alpha = 0;
		}
		else
		{
			IconContainer._alpha = 100;
		}

		// Make sure this movie clip does not stay visible
		if (rootMenuInstance._alpha == 0 || itemCard._alpha == 0)
		{
			this._alpha = 0;
		}
	}

	function ResetIconText():Void
	{
		IconContainer.Clear();
		// Creation the text field that holds the extra data
		IconContainer.textField.verticalAlign = "center";
		IconContainer.textField.textAutoSize = "shrink";
		IconContainer.textField.multiLine = false;
		//IconContainer.textField.border = true;
		_iconContainerTextFormat = new TextFormat();
		_iconContainerTextFormat.align = "center";
		var withoutHash = _config[AHZDefines.CFG_ICON_TEXT_FIELD_COLOR].substr(1,_config[AHZDefines.CFG_ICON_TEXT_FIELD_COLOR].length-1);
		_iconContainerTextFormat.color = parseInt(withoutHash, 16);
		_iconContainerTextFormat.size = 24;
		_iconContainerTextFormat.font = "$EverywhereMediumFont";
		IconContainer.setNewTextFormat(_iconContainerTextFormat);

		if (_config[AHZDefines.CFG_ICON_DROP_SHADOW])
		{
			var filter:DropShadowFilter = new DropShadowFilter(2,45,0,100,2,2,1.5);
			var filterArray:Array = new Array();
			filterArray.push(filter);
			IconContainer.textField.filters = filterArray;
		}
		else
		{
			var filterArray:Array = new Array();
			IconContainer.textField.filters = filterArray;
		}

		IconContainer.html = false;
		IconContainer.text = "";
	}

	function stringReplace(block:String, findStr:String, replaceStr:String):String
	{
		return block.split(findStr).join(replaceStr);
	}

	// The Scaleform Extension is broken for center justify for Shrink so I rolled my own
	function ShrinkToFit(tf:TextField):Void
	{
		tf.multiline = true;
		tf.wordWrap = true
		var tfText:String = tf.htmlText;
		var fontSize:Number = 20;
		var htmlSize = "SIZE=\"" + fontSize.toString() + "\""
		tf.textAutoSize = "none";
		tf.SetText(tfText, true);
		tf.textAutoSize = "none";
		var tfHeight:Number = tf.getLineMetrics(0).height * tf.numLines;
		while (tfHeight > tf._height && fontSize > 5)
		{
			var beforeHtmlSize = "SIZE=\"" + fontSize.toString() + "\"";
			fontSize -= 1;
			htmlSize = "SIZE=\"" + fontSize.toString() + "\"";
			tfText = stringReplace(tfText, beforeHtmlSize, htmlSize);
			tf.textAutoSize = "none";
			tf.SetText(tfText, true);
			tf.textAutoSize = "none";
			tfHeight = tf.getLineMetrics(0).height * tf.numLines;
		}
	}

	function SetVanillaAlpha():Void{
		if (itemCard.itemInfo.type == ICT_CRAFT_ENCHANTING || itemCard.itemInfo.type == ICT_HOUSE_PART)
		{
			if (itemCard.itemInfo.effects != undefined && itemCard.itemInfo.effects.length > 0) {
				itemCard.Enchanting_Background._alpha = _config[AHZDefines.CFG_LIC_ALPHA];
				itemCard.Enchanting_Slim_Background._alpha = 0;
			} else {
				itemCard.Enchanting_Background._alpha = 0;
				itemCard.Enchanting_Slim_Background ._alpha = _config[AHZDefines.CFG_LIC_ALPHA];
			}	
		}					
		else{
			cardBackground._alpha = _config[AHZDefines.CFG_LIC_ALPHA];
		}		
	}

	function AdjustItemCard(itemCardFrame:Number):Void
	{
		var processedTextField:TextField = undefined;
		var itemCardX:Number;
		var itemCardY:Number;
		var itemCardBottom:Number;
		var marginRequired:Boolean = false;

		itemCardX = itemCard._parent._x + itemCard._x;
		itemCardY = itemCard._parent._y + itemCard._y;
		this._y = itemCardY + _config[AHZDefines.CFG_LIC_YOFFSET];
		this._x = (itemCardX - ((this._width - originalWidth) / 2)) + _config[AHZDefines.CFG_LIC_XOFFSET];
		itemCardBottom = this._height;
		var oldDescrptionHeight:Number;

		_global.skse.plugins.AHZmoreHUDInventory.AHZLog("<<<FRAME>>>: " + itemCardFrame, false);

		// If we advance to somethine like the confirmation frame, then make sure the icons are not visible
		// Dont wipe the value because we need to restore it when returning to the same item card
		if (itemCardFrame >= _frameDefines.EMPTY_LOW && itemCardFrame <= _frameDefines.EMPTY_HIGH)
		{
			IconContainer._alpha = 0;
		}
		else
		{
			IconContainer._alpha = 100;
		}

		switch (itemCardFrame)
		{
			case _frameDefines.WEAPONS_ENCH:
				{
					processedTextField = itemCard.WeaponEnchantedLabel;
				}
				break;
			case _frameDefines.ARMOR_ENCH:
			case _frameDefines.ARMOR_SURV_ENCH:
				{
					processedTextField = itemCard.ApparelEnchantedLabel;
				}
				break;
			case _frameDefines.POTION_REF:
				{
					processedTextField = itemCard.PotionsLabel;
				}
				break;
			case _frameDefines.BOOKS_DESCRIPTION:
				{
					processedTextField = itemCard.BookDescriptionLabel;
				}
				break;

			case _frameDefines.MAGIC_REG:
			case _frameDefines.POW_REG:
			case _frameDefines.MAGIC_TIME_LABEL:
			case _frameDefines.POWER_TIME_LABEL:
			case _frameDefines.ACTIVEEFFECTS:
				{
					processedTextField = itemCard.MagicEffectsLabel;
					marginRequired = true;
				}
				break;
			case _frameDefines.MAG_SHORT:
				{
					processedTextField = itemCard.MagicEffectsLabel;
				}
				break;
				
			// All other frames are not going to be resized
			default:
				{
					processedTextField = undefined;
					this._alpha = 0;
					SetVanillaAlpha();
				}
				break;
		}

		if (processedTextField)
		{			
			_itemCardOverride = true;
			//this._alpha = AHZ_NormalALPHA;
			cardBackground._alpha = 0;
			processedTextField._width = this._width - (_config[AHZDefines.CFG_LIC_DESCRIPTION_RIGHTMARGIN] + _config[AHZDefines.CFG_LIC_DESCRIPTION_LEFTMARGIN]);
			processedTextField._x = newX + _config[AHZDefines.CFG_LIC_DESCRIPTION_LEFTMARGIN];
			oldDescrptionHeight = processedTextField._height;

			if (marginRequired)
			{
				processedTextField._height = (itemCardBottom - processedTextField._y) - (_config[AHZDefines.CFG_LIC_DESCRIPTION_BOTTOMMARGIN_EXTRA] + _config[AHZDefines.CFG_LIC_DESCRIPTION_EXTRADATA_PADDING]);
			}
			else
			{
				processedTextField._height = (itemCardBottom - processedTextField._y) - _config[AHZDefines.CFG_LIC_DESCRIPTION_BOTTOMMARGIN];
			}

			ShrinkToFit(processedTextField);

			// Shift any control the is below the processedTextField, down to the new
			// height
			var itemsBelow:Array = GetItemsBelowDescription(itemCard, processedTextField);
			var itemBelow:Number;
			for (itemBelow = 0; itemBelow < itemsBelow.length; itemBelow++)
			{
				// If the height changed then move the controls down
				if ((processedTextField._height - oldDescrptionHeight) != 0)
				{
					// add the margin back to preserve it
					itemsBelow[itemBelow]._y = itemsBelow[itemBelow]._y + (processedTextField._height - oldDescrptionHeight) + _config[AHZDefines.CFG_LIC_DESCRIPTION_EXTRADATA_PADDING];
				}
			}

			// Need to shift up to make room for the requied crafting materials
			if (_currentMenu == "Crafting Menu" && !_craftingMenuCardShifted)
			{
				itemCard._parent._y = itemCard._parent._y + _config[AHZDefines.CFG_LIC_CRAFTING_YOFFSET] ;
				this._y = this._y + _config[AHZDefines.CFG_LIC_CRAFTING_YOFFSET] ;
				additionDescriptionHolder._y = additionDescriptionHolder._y - _config[AHZDefines.CFG_LIC_CRAFTING_YOFFSET] ;
				_craftingMenuCardShifted = true;
			}
		}
		else
		{
			_itemCardOverride = false;
			this._alpha = 0;
			SetVanillaAlpha();			
			
			// Shift back to normal
			if (_currentMenu == "Crafting Menu" && _craftingMenuCardShifted)
			{
				itemCard._parent._y = itemCard._parent._y - _config[AHZDefines.CFG_LIC_CRAFTING_YOFFSET] ;
				this._y = this._y - _config[AHZDefines.CFG_LIC_CRAFTING_YOFFSET] ;
				additionDescriptionHolder._y = additionDescriptionHolder._y + _config[AHZDefines.CFG_LIC_CRAFTING_YOFFSET] ;
				_craftingMenuCardShifted = false;
			}
		}
	}

	function checkDbm(selectedItemIn:Object):Void
	{
		if (!selectedItemIn)
		{
			return;
		}
		
		if (selectedItemIn.AHZdbmNew)
		{
			IconContainer.appendImage("dbmNew");
		}		
		if (selectedItemIn.AHZdbmDisp)
		{
			IconContainer.appendImage("dbmDisp");
		}	
		if (selectedItemIn.AHZdbmFound)
		{
			IconContainer.appendImage("dbmFound");
		}			
	}

	// A hook to update the item card with extended items
	// Note this function does not get called by the crafting menus.  If we need to extend
	// the crafting menu, I need to find another way.  I found no publically accessable hooks
	// in the crafting menu.
	function UpdateItemCardInfo(): Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("-->UpdateItemCardInfo", false);
		var type:Number;
		var itemCardFrame:Number = itemCard._currentframe;
		type = itemCard.itemInfo.type;
		ResetIconText();
		var iconName:String;

		IconContainer._x = itemCard.ItemText._x + itemCard.ItemText.ItemTextField._x;
		IconContainer.textWidth = itemCard.ItemText.ItemTextField._width;		
		IconContainer._y = ((itemCard.ItemText._y + itemCard.ItemText.ItemTextField._y) - IconContainer.textHeight) + 10;

		if (_enableItemCardResize)
		{
			AdjustItemCard(_lastFrame);
		}

		// Keep icons off of frames like the confirmation frame etc.
		if (itemCardFrame >= _frameDefines.EMPTY_LOW && itemCardFrame <= _frameDefines.EMPTY_HIGH)
		{
			return;
		}

		// Magic Menu cannot be extended by plugins so call a function to get the iconName
		if (_currentMenu == "MagicMenu")
		{
			// Only for iEquip 
			var returnValue:Object = {returnObject:{itemIcon:""}}
									 _global.skse.plugins.AHZmoreHUDInventory.GetIconForItemId(_selectedItem.formId, _selectedItem.text, returnValue);
			iconName = returnValue.returnObject.iconName;
			if (!iconName || !iconName.length)
			{
				return
			}
		}
		else if (type != ICT_BOOK &&
				 type != ICT_ARMOR && 
				 type != ICT_WEAPON && 
				 type != ICT_POTION && 
				 type != ICT_FOOD && !_selectedItem.AHZItemIcon)
		{
			// Allow for checking Misc and Keys (Empty Frame)
			checkDbm(_selectedItem);
			return;
		}
		
		if (_selectedItem.AHZItemCardObj.enchantmentKnown)
		{
			if (_selectedItem.AHZItemCardObj.enchantmentKnown == 1){
				IconContainer.appendImage("ahzKnown");
			}
			if (_selectedItem.AHZItemCardObj.enchantmentKnown == 2){
				IconContainer.appendImage("ahzEnch");
			}			
		}
		// Fortunately, extraData is not required for getting the Book Read Status.  This allows us to check
		// it in real time and make sure the read status is accurate
		else if (_global.skse.plugins.AHZmoreHUDInventory.GetWasBookRead(_selectedItem.formId))
		{
			if (_global.skse.plugins.AHZmoreHUDInventory.ShowBookRead())
			{
				IconContainer.appendImage("ahzEye");
			}
		}
		else if (_selectedItem.AHZItemCardObj.bookSkill &&
				 String(_selectedItem.AHZItemCardObj.bookSkill).length )
		{
			IconContainer.text = String(_selectedItem.AHZItemCardObj.bookSkill.toUpperCase());
		}
		else if (_selectedItem.AHZItemCardObj.PosEffects > 0||
				 _selectedItem.AHZItemCardObj.NegEffects > 0)
		{
			IconContainer.html = true;
			if (_selectedItem.AHZItemCardObj.PosEffects > 0)
			{
				IconContainer.appendImage("ahzHealth");
				IconContainer.appendHtml("<font face=\'$EverywhereBoldFont\' size=\'18\' color=\'"+_config[AHZDefines.CFG_ICON_POS_EFFECT_COLOR]+"\'>&nbsp;" + _selectedItem.AHZItemCardObj.PosEffects + "</font>");
				IconContainer.appendHtml("<font face=\'$EverywhereBoldFont\' size=\'18\' color=\'"+_config[AHZDefines.CFG_ICON_POS_EFFECT_COLOR]+"\'>&nbsp;&nbsp;&nbsp;</font>");
			}
			if (_selectedItem.AHZItemCardObj.NegEffects > 0)
			{				
				IconContainer.appendImage("ahzPoison");
				IconContainer.appendHtml("<font face=\'$EverywhereBoldFont\' size=\'18\' color=\'"+_config[AHZDefines.CFG_ICON_NEG_EFFECT_COLOR]+"\'>&nbsp;" + _selectedItem.AHZItemCardObj.NegEffects + "</font>");
				IconContainer.appendHtml("<font face=\'$EverywhereBoldFont\' size=\'18\' color=\'"+_config[AHZDefines.CFG_ICON_POS_EFFECT_COLOR]+"\'>&nbsp;&nbsp;&nbsp;</font>");	
			}
		}

		// If coming from the Magic Menu (No extended data) or other menus (the name is part of the extended data for all other menus
		if (_selectedItem.AHZItemIcon || (iconName && iconName.length))
		{
			var customIcon:String;
			if (iconName && iconName.length)
			{
				customIcon = string(iconName);
				IconContainer.appendImage(customIcon);
			}
			else
			{
				customIcon = string(_selectedItem.AHZItemIcon);
				IconContainer.appendImage(customIcon);
			}
		}

		checkDbm(_selectedItem);
	}

	function interpolate(pBegin:Number, pEnd:Number, pMax:Number, pStep:Number):Number
	{
		return pBegin + Math.floor((pEnd - pBegin) * pStep / pMax);
	}

	// @override WidgetBase
	public function onLoad():Void
	{
		super.onLoad();
		AHZConfigManager.loadConfig(this, "configLoaded", "configError");
	}

	public function onLoadInit(s_mc: MovieClip): Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("CLIP LOADED: " + LoadedLargeItemCard_mc, false);
		s_mc.gotoAndStop(0);

		if (LoadedLargeItemCard_mc == s_mc)
		{
			AttachIconHolder();
			if (_config[AHZDefines.CFG_ICON_PATH])
			{
				IconContainer.Load(AHZConfigManager.ResolvePath(_config[AHZDefines.CFG_ICON_PATH]), this, "iconsLoaded", "iconsLoadedError");
			}
			else
			{
				clipReady();
			}
		}
	}

	public function onLoadError(s_mc:MovieClip, a_errorCode: String): Void
	{
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("CLIP ERROR: " + a_errorCode, false);
		AttachIconHolder();
		if (_config[AHZDefines.CFG_ICON_PATH])
		{
			IconContainer.Load(AHZConfigManager.ResolvePath(_config[AHZDefines.CFG_ICON_PATH]), this, "iconsLoaded", "iconsLoadedError");
		}
		else
		{
			clipReady();
		}
	}

	public static function hookFunction(a_scope:Object, a_memberFn:String, a_hookScope:Object, a_hookFn:String):Boolean
	{
		var memberFn:Function = a_scope[a_memberFn];
		if (memberFn == null || a_scope[a_memberFn] == null)
		{
			return false;
		}

		a_scope[a_memberFn] = function ()
		{
			memberFn.apply(a_scope,arguments);
			a_hookScope[a_hookFn].apply(a_hookScope,arguments);
		};
		return true;
	}
}