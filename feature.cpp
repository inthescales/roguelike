#include "error.h"
#include "featclass.h"
#include "feature.h"
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

bool feature::is_opaque() {
    return has_flag(FLAG_FEAT_OPAQUE);
}

int feature::can_open() {

    if (has_state(STATE_FEAT_OPEN) &&
        has_flag(FLAG_FEAT_CAN_OPEN)) {
        return ERR_NONE;
    }
    
    return ERR_CANT_OPEN;
}

int feature::can_close() {

    if (has_state(STATE_FEAT_CLOSED) &&
        has_flag(FLAG_FEAT_CAN_CLOSE)) {
        return ERR_NONE;
    }
    
    return ERR_CANT_CLOSE;
}
