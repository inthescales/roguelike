#ifndef OBJECT_H

#define OBJECT_H

#include "actiondefs.h"

#include <utility>
#include <vector>

using namespace std;

class object {

	public:

	short letter;
	int quantity;
	int type;
	
	object(short);
	object(short, short);

};

#endif
