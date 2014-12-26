#ifndef ACTOR_H

#define ACTOR_H

#include "enums.h"
#include "condition.h"
#include "config.h"
#include "display.h"
#include "mapentity.h"

#include <stdlib.h>
#include <string>
#include <utility> //pair
#include <vector>

using std::string;
using std::vector;

class action;
class targetActionBlock;
class object;
class tile;

class actor : public mapentity {

	public:

	// basics
	string individual_name;

	// stats
	short HP, maxHP;
	short level;
	
	// inventory
	vector<object*> * inventory;
	object * equipped_item[ES_MAX]; // TODO - implement body plans
	long gold; //TODO - change this to stacked items list eventually?
	
	//ai
	int aitype;
	int state;

	// Functions =====================
	actor(short code);
    void init();
    
    actclass * get_class();
    string get_name();
    string get_name_color();
    
    // Display
	void print(string, string);
    
    // Effects
    void resolve_trigger(trigger_t, argmap *);
    
    // Getting properties
	int get_stat(stats_t stat);
	int get_equip_stat(stats_t stat);
	int get_calc_stat(stats_t stat);
    bool has_flag(flags_t);
    bool has_equip_flag(flags_t stat);
    
    // State management
    void get_object(object *);
	bool remove_object(object * item);
    
    // Actions
    int take_turn();
    void queue_turn(int);
    bool execute_action(action *);
    vector<void*> * select_target(targetActionBlock *);
    
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
    
    bool can_travel(tile *);
    bool can_take(object *);
};

#endif
