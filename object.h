#ifndef OBJECT_H

#define OBJECT_H

#include "actiondefs.h"
#include "curses.h"

#include <utility>
#include <string>
#include <vector>

using namespace std;

class object {

	public:

	short letter;
	int quantity;
	int type;
	bool equipped;
	
	object(short);
	object(short, short);
	
	static bool compare_type(object *, object *);
	
	chtype get_img();
	string get_name();
	string get_name_color();

};

#endif
