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
class window;

#define ENTER_KEY 10
#define ESCAPE_KEY 27

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
    DIR_CENTER,
    DIR_MAX
};

extern std::map<int, direction_t> * direction_key;
extern std::map<int, action *> * action_key;

class UI {

	public:
    static void setup_ui();
    
	static void get_action();
	static int get_input();
    static vector<action*> * get_context_action(actionPurpose_t);
    
    static bool command_direction(direction_t);
	static bool command_inventory();
	static bool command_equipment();
	static bool command_conditions();
	static bool command_move(direction_t);
	static bool command_quit();
	
    static vector<void*> * prompt_target(targetActionBlock *);
    static vector<tile*> * prompt_self(actor *);
    static vector<tile*> * prompt_adjacent(string, argmap *, vector<requirement*> *);
    static vector<object*> * prompt_objects(string, vector<object*>*, argmap *, vector<requirement*> *);
    static vector<object*> * prompt_inventory(string, argmap *, vector<requirement*> *);
    static vector<tile*> * prompt_tile(string, bool, tile*, argmap *, vector<requirement*> *);
    
    static vector<tile*> * assume(vector<tile*> *, extract_t, argmap *, vector<requirement*> *);
    static vector<tile*> * assume_self(extract_t, argmap *, vector<requirement*> *);
    static vector<tile*> * assume_adjacent(extract_t, argmap *, vector<requirement*> *);
    
    static vector<actor*> * extract_actors(vector<tile*> *, vector<requirement*> *);
    static vector<object*> * extract_objects(vector<tile*> *, vector<requirement*> *);
    static vector<object*> * extract_inventories(vector<tile*> *, vector<requirement*> *);
    static vector<feature*> * extract_features(vector<tile*> *, vector<requirement*> *);
    
    // Menus
	static vector<object*> * menu_select_objects(window *, string, vector<object*> *, argmap *);

    // Smaller prompts
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
