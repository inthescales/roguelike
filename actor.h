#ifndef ACTOR_H
#define ACTOR_H

#include "action.h"
#include "enums.h"
#include "condition.h"
#include "config.h"
#include "display.h"
#include "mapentity.h"
#include "object.h"

#include <map>
#include <stdlib.h>
#include <string>
#include <utility> //pair
#include <vector>

using std::string;
using std::vector;

enum act_can_t {

    ACTCAN_CAN_USE = 0,
    ACTCAN_CANT_USE,
    ACTCAN_TOO_FAR,
    ACTCAN_TOO_CLOSE,
    ACTCAN_NEED_EQUIP,
    ACTCAN_NEED_INSTRUMENT
};

class actclass;
class action;
class mind;
class object;
class targetActionBlock;
class tile;

class actor : public mapentity {

	public:

	// stats
	short HP, maxHP;
	short level;
	
	// inventory
	vector<object*> * inventory;
	object * equipped_item[ES_MAX]; // TODO - implement body plans
	long gold; //TODO - change this to stacked items list eventually?
    
    //actions
    std::map<actionPurpose_t, vector<action*>*> actionsList;
    
	//ai
	mind * my_mind;
	int state;

	// Functions =====================
	actor(short code);
    void init();
    
    actclass * get_class();
    string get_name();
    string get_name_color();
    vector<int> * get_actions();
    vector<int> * get_actions_for(actionPurpose_t);        

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
    void do_equip(object *, int);
    void do_unequip(object *, int);
    void move(tile*);
    void enter_tile(tile *);
    
    // AI
    act_can_t check_can(action *, entity *);
    int check_effort(action *, entity *);
    bool move_toward(mapentity *);
    bool can_move_to(action *, tile *);
    vector<object*> * objects_of_type(object_type, object_subtype);
    vector<object*> * objects_for_slot(equip_slot);
    
    // Actions
    int take_turn();
    void queue_turn(int);
    bool execute_action(action *);
    bool execute_action(action *, argmap *, bool);
    vector<void*> * select_target(targetActionBlock *);
    
    void walk(tile *);
    void swim(tile *);
    void fly(tile *);
	bool pick_up(object *, tile *);
    bool pick_up(object *);
	bool drop(object *);
    int pick_slot_for(object *);
    bool equip(object *);
	bool equip(object *, int);
	bool unequip(object *);
	bool eat(object *);
	bool drink(object *);
	bool read(object *);
	bool use(object *);
    bool open_feature(feature *);
    bool close_feature(feature *);
    bool strike(actor *);
    bool punch(actor *);
    
    int can_travel(tile *);
    int can_walk(tile*);
    int can_swim(tile*);
    int can_fly(tile*);
    int can_take(object *);
    int can_drop(object *);
    int can_equip(object *);
    int can_unequip(object *);
    int can_eat(object *);
    int can_drink(object *);
    int can_open(feature *);
    int can_close(feature *);
    int can_strike(actor *);
    int can_punch(actor *);
    
    bool is_opaque();
};

#endif
