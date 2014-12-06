#include "featclass.h"
#include "feature.h"
#include "globals.h"
#include "map.h"

#include "feature.h"

feature::feature(short code){

	//my_class = (entityclass*)featclass[code];
    init();
}

void feature::init() {
    mapentity::init();
}