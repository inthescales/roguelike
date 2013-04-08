#ifndef DEFINE_H

#define DEFINE_H

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
	ES_MAINHAND = 1,
	ES_OFFHAND,
	ES_HEAD,
	ES_BODY,
	ES_BACK,
	ES_HANDS,
	ES_FEET,
	ES_RING1,
	ES_RING2,
	ES_AMULET,
	ES_LIGHT
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
	OT_MISC
};

//Color IDs
enum colorName {
	C_BLACK = 0,
	C_GRAY, C_WHITE,
	C_RED, C_ORANGE,
	C_GREEN, C_LIME,
	C_BLUE, C_GOLUBOI,
	C_PURPLE, C_MAGENTA,
	C_BROWN, C_YELLOW,
	C_TEAL, C_CYAN
};

enum material_t {
	MAT_VEGETATION = 0,
	MAT_FLESH,
	MAT_CLOTH,
	MAT_LEATHER,
	MAT_METAL,
	MAT_PLASTIC
};

#endif