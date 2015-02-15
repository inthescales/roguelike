#ifndef FEATURE_H

#define FEATURE_H

#include "mapentity.h"

class feature : public mapentity {

	public:
	
	feature(short);
    void init();

    featclass * get_class();
    
    int can_open();
    int can_close();
};

#endif
