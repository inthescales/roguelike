#ifndef ACTIONDEFS_H

#define ACTIONDEFS_H

enum action_t {
	ACT_GET = 0,
	ACT_DROP,
	ACT_THROW,
	ACT_EAT,
	ACT_DRINK,
	ACT_WIELD,
	ACT_WEAR,
	ACT_USE
};

enum act_eat_t {
	EAT_INEDIBLE = 0,
	EAT_BAD,
	EAT_OK,
	EAT_GOOD
};

enum act_drink_t {
	DRINK_UNDRINKABLE = 0,
	
};

#endif