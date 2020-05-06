class ahz.scripts.widgets.AHZDefines.AHZVanillaFrames
{
	public static var INVALID:Number 					= -1;  // Invalid for non survival
	
	// For all non-magic menus
    public static var WEAPONS_ENCH:Number 				= 10;
    public static var ARMOR_ENCH:Number 				= 30;
	public static var ARMOR_SURV_ENCH:Number 			= INVALID;

    public static var POTION_REF:Number 				= 40;
    public static var BOOKS_DESCRIPTION:Number 			= 80;
    public static var MAGIC_REG:Number 					= 90;
	
	// For all magic menus
	
		// Default Spell With Cost/No Cost
	public static var POW_REG:Number 					= 95;	
		// Spell With Cast Time
	public static var MAGIC_TIME_LABEL:Number 			= 100;
	public static var POWER_TIME_LABEL:Number 			= 105;
	public static var ACTIVEEFFECTS:Number 				= 171;	
	
	// Crafting
	    // Used for house part
	public static var MAG_SHORT:Number					= 110;
	public static var CFT_ENCHANTING:Number				= 192;
	public static var CFT_ENCHANTING_WEAPON:Number		= 193;
	public static var CFT_ENCHANTING_ARMOR:Number		= 194;
	public static var CFT_ENCHANTING_ENCHANTMENT:Number	= 195;
	
	// This range includes confirmation cards, we will ignore these
	public static var EMPTY_LOW:Number 					= 130;	
	public static var EMPTY_HIGH:Number 				= 170;		
	
	public static var MAX_FRAMES:Number 				= 201;	
}