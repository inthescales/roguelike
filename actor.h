#ifndef ACTOR_Hp

#define ACTOR_H

#include "enums.h"
#include "condition.h"
#include "config.h"
#include "display.h"

#include <stdlib.h>
#include <string>
#include <utility> //pair
#include <vector>

using std::string;

class glyph;
class object;
class tile;

class actor {

	public:

	// basics
	string name; //special name
	short x, y; //position
	int type; //actor class

	// stats
	short HP, maxHP;
	short level;
	
	// inventory
	std::vector<object*> inventory;
	object * equipped_item[ES_MAX];
	long gold; //TODO - change this to stacked items list eventually?
	
	//ai
	int aitype;
	int state;
	
	std::vector<condition*> conditions;

	// Functions:
	actor(short code);
    
    // Display
	glyph get_glyph();
    symbol_code get_symbol();
	colorName get_color();
	string get_name();
    string get_name_color();
	void print(string, string);
    
    // Getting properties
	int get_stat(stats_t stat);
	int get_stat(stats_t stat, bool);
	int get_equip_stat(stats_t stat);
	int get_cond_stat(stats_t stat);
	int get_calc_stat(stats_t stat);
	effect * get_effect(trigger_t);
    
    // State management
    void get_item(object *);
	bool remove_object(object * item);
    void resolve_trigger(trigger_t, argmap *);
    bool add_condition(condition *);
    bool remove_condition(int);
    bool has_condition(int);
    condition * get_condition(int);
    void decay_conditions();
    //void transition(stair);
	//void switch_map(map, map, int, int );
    
    // Actions
    int take_turn();
    void queue_turn(int);
    void move(std::pair<int,int>);
	void attack(std::pair<int,int>);
	void attack(actor *);
    
	bool pick_up(object *, tile *);
	bool drop(object *, tile*);
	bool equip(object *, int);
	bool unequip(object *);
	bool eat(object *);
	bool drink(object *);
	bool read(object *);
	bool use(object *);
	//void open(int, int);
	//void close(int, int);
};

#endif
