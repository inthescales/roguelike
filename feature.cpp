#include "featclass.h"
#include "feature.h"
#include "globals.h"
#include "map.h"

#include "feature.h"

feature::feature(short code){

	status = 0;
	type = code;
}

void feature::on_open(){
	
}

void feature::on_close(){

}
