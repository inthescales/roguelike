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
	
	object(short);
	object(short, short);
	
	chtype get_img();
	string get_name();

};

#endif
