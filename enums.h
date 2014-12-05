#ifndef ENUMS_H

#define ENUMS_H

//Cardinal directions, for easy reference
enum direction {
	DIR_UP = 2,
	DIR_RIGHT = 5,
	DIR_DOWN = 7,
	DIR_LEFT = 4,
	DIR_UPRIGHT = 9,
	DIR_UPLEFT = 7,
	DIR_DOWNLEFT = 1,
	DIR_DOWNRIGHT = 3
};

//Equipment slots
enum equip_slot {
	ES_MAINHAND = 0,
	ES_OFFHAND,
	ES_HEAD,
	ES_BODY,
	ES_BACK,
	ES_HANDS,
	ES_FEET,
	ES_RING1,
	ES_RING2,
	ES_AMULET,
	ES_LIGHT,
	ES_MAX
};

//Item types
enum object_type {
	OT_FOOD = 0,
	OT_WEAPON,
	OT_ARMOR,
	OT_ACCESSORY,
	OT_POTION,
	OT_SCROLL,
	OT_WAND,
	OT_TOOL,
	OT_TREASURE,
	OT_MISC
};

//Item subtypes
enum object_subtype {
	OST_FOOD = 0,
	OST_SWORD,
	OST_KNIFE,
	OST_SPEAR,
	OST_AXE,
	OST_MACE,
	OST_BOW,
	OST_HAT,
	OST_HELM,
	OST_SUIT,
	OST_GLOVES,
	OST_BOOTS,
	OST_CLOAK,
	OST_RING,
	OST_AMULET,
	OST_LAMP,
	OST_COIN
};

//Color IDs
enum colorName {
	C_BLACK = 0,
	C_GRAY, C_WHITE,
	C_RED, C_ORANGE,
	C_GREEN, C_LIME,
	C_BLUE, C_SKY,
	C_PURPLE, C_MAGENTA,
	C_BROWN, C_YELLOW,
	C_TEAL, C_CYAN,
	C_GRAY_INV, C_WHITE_INV
};

// Color pair IDs for non-bold colors
enum colorPair {
  CP_BLACK_BLACK = 0,
  CP_GRAY_BLACK,
  CP_RED_BLACK,
  CP_GREEN_BLACK,
  CP_BLUE_BLACK,
  CP_PURPLE_BLACK,
  CP_BROWN_BLACK,
  CP_TEAL_BLACK,
  CP_BLACK_GRAY
};

//Object materials
enum material_t {
	MAT_VEGETATION = 0,
	MAT_FLESH,
	MAT_CLOTH,
	MAT_LEATHER,
	MAT_METAL,
	MAT_PLASTIC
};

//Triggers
enum trigger_t {
	TRG_NULL = -1,
    TRG_COND_END,
    TRG_COND_TIMEOUT,
    TRG_COND_REMOVED,
	TRG_EAT,
	TRG_DRINK,
	TRG_READ,
	TRG_USE
};

//Effect targets
enum target_t {
    TAR_NONE = 0,
	TAR_SELF,
	TAR_ADJ
};

//Effect radii (patterns of tiles they affect)
enum radius_t {
    RAD_NONE = 0,
	RAD_SINGLE
};

//The actual effect that occurs
enum effect_t {
    EFF_TURN = 0,
    EFF_COND_TIMEOUT,
	EFF_BREAD,
    EFF_PRINT_INT,
    EFF_PRINT_INT_2,
    EFF_PRINT_INT_3    //FOR DEBUG
};

//Condition stacking policies
enum cond_stack_policy_t {
	CSPOL_STACK = 0,
    CSPOL_SET,
	CSPOL_ADD,
	CSPOL_MAX,
	CSPOL_MIN,
	CSPOL_IGNORE
};

//Condition timer policies
enum cond_timer_policy_t {
	CTPOL_SET = 0,
	CTPOL_ADD,
	CTPOL_MAX,
	CTPOL_MIN,
	CTPOL_IGNORE
};

//Tastes for food (determines message, at least)
enum taste_t {
	TST_BREAD = 0
};

// Stats for objects and actors
enum stats_t {
	// Actor stats
	ASTAT_BODY = 0,
	ASTAT_MIND,
	// Object stats
	OSTAT_MIN,
	OSTAT_WEP_DMG,
	OSTAT_ARM_BLOCK,
	OSTAT_NUTRITION,
	// Calculated stats
	CSTAT_MIN,
	CSTAT_WEP_DMG,
	CSTAT_BLOCK
};

#endif
