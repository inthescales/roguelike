#ifndef UI_H
#define UI_H

#include <utility> //pair

enum ui_action{
	UIA_NONE = 0,
	UIA_MOVE,
	UIA_ATTACK
};

class actor;
class object;

class UI {

	public:
	static void get_action();
	static int get_input();
	
	static bool command_inventory();
	static bool command_move(int);
	static bool command_pick_up();
	static bool command_pick_up_helper(object *);
	static bool command_drop();
	
	static std::pair<int,int> dir_to_offset(int);
	static char int_to_letter(int);
	static int letter_to_int(char);
	static vector<object*> prompt_inventory(actor *, string, bool, bool);
	static object * prompt_gold_to_object(actor *);
	
};

#endif