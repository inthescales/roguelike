#ifndef UI_H
#define UI_H

#include <utility> //pair

enum ui_action{
	UIA_NONE = 0,
	UIA_MOVE,
	UIA_ATTACK
};

class UI {

	public:
	static void get_action();
	static int get_input();
	
	static bool command_move(int);
	
	static std::pair<int,int> dir_to_offset(int);
	
};

#endif