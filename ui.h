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
	static bool command_equipment();
	static bool command_conditions();
	static bool command_move(int);
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
	
	static bool prompt_yesno(string);
	static vector<object*> prompt_inventory(actor *, string, bool, bool);
	static object * prompt_gold_to_object(actor *);
	
	static object * get_single(vector<object*>);
	static std::pair<int,int> dir_to_offset(int);
	static char int_to_letter(int);
	static int letter_to_int(char);
	static char get_next_letter();
	static int type_to_slot(int, int);
	
};

#endif
