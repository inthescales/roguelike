#include "featclass.h"
#include "feature.h"
#include "globals.h"
#include "map.h"

#include "feature.h"

feature::feature(short code){

	my_class = (entityclass*)fclass[code];
    individual_name = "";
    init();
}

void feature::init() {
    mapentity::init();
}

// Basic info ==============================

featclass * feature::get_class(){
    return (featclass *)entity::get_class();
}

bool feature::can_open() {

    bool test = has_state(STATE_FEAT_OPEN);
    bool test2 = has_state(STATE_FEAT_CLOSED);
    
    if (has_state(STATE_FEAT_OPEN) &&
        has_flag(FLAG_FEAT_CAN_OPEN)) {
        return true;
    }
    
    return false;
}

bool feature::can_close() {

    if (has_state(STATE_FEAT_CLOSED) &&
        has_flag(FLAG_FEAT_CAN_CLOSE)) {
        return true;
    }
    
    return false;
}
