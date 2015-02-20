#ifndef FEATURE_H

#define FEATURE_H

#include "mapentity.h"

class featclass;

class feature : public mapentity {

	public:
	
	feature(short);
    void init();

    featclass * get_class();
    
    bool is_opaque();
    int can_open();
    int can_close();
};

#endif
