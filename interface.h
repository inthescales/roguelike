#ifndef INTERFACE_H
#define INTERFACE_H

#include "action.h"
#include <map>
#include <string>
#include <utility> //pair
#include <vector>

using std::string;
using std::vector;

class action;
class actor;
class argmap;
class feature;
class object;
class requirement;
class targetActionBlock;
class tile;

//Cardinal directions, for easy reference
enum direction_t {
	DIR_NULL = 0,
    DIR_UP,
    DIR_UPRIGHT,
    DIR_RIGHT,
    DIR_DOWNRIGHT,
    DIR_DOWN,
    DIR_DOWNLEFT,
    DIR_LEFT,
    DIR_UPLEFT,
    DIR_CENTER
};

extern std::map<int, direction_t> * direction_key;
extern std::map<int, action *> * action_key;

class UI {

	public:
    static void setup_ui();
    
	static void get_action();
	static int get_input();
    static action * get_context_action(actionPurpose_t);
    
    static bool command_direction(direction_t);
	static bool command_inventory();
	static bool command_equipment();
	static bool command_conditions();
	static bool command_move(direction_t);
	static bool command_pick_up();
	static string command_pick_up_helper(object *);
	static bool command_drop();
	static bool command_equip();
	static bool command_unequip();
	static bool command_eat();
	static bool command_drink();
	static bool command_read();
	static bool command_use();
	static bool command_quit();
	
    static vector<void*> * prompt_target(targetActionBlock *);
    static actor * prompt_self(vector<requirement*> *);
    static vector<tile*> * prompt_adjacent(string, argmap *, vector<requirement*> *);
    static vector<object*> * prompt_inventory(string, argmap *, vector<requirement*> *);
	
    static vector<feature*> * extract_features(vector<tile*> *);
    
    static bool prompt_yesno(string);
	static object * prompt_gold_to_object();
    static direction_t prompt_direction(string);
    
	static object * get_single(vector<object*> *);
	static std::pair<int,int> dir_to_offset(direction_t);
	static char int_to_letter(int);
	static int letter_to_int(char);
	static char get_next_letter();
	static int type_to_slot(int, int);
	
};

#endif
