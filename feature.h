#ifndef FEATURE_H

#define FEATURE_H

#include "mapentity.h"

class feature : public mapentity {

	public:
	
	feature(short);
    void init();

    featclass * get_class();
    
    bool can_open();
    bool can_close();
};

#endif
