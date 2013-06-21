#include "globals.h"
#include "graphics.h"
#include "object.h"
#include "stringutils.h"

object::object(short code) : type(code), equipped(0), quantity(1) {

}

object::object(short code, short quant) : type(code), equipped(0), quantity(quant) {

}

string object::get_name(){
	return oclass[type]->name;
}

string object::get_name_color(){
	return color_string(get_name(), oclass[type]->color);
}

chtype object::get_img(){
	return comp(oclass[type]->symbol, oclass[type]->color);
}

bool object::compare_type(object * a, object * b){
	return oclass[a->type]->type < oclass[b->type]->type;
}

effect * object::get_effect(trigger_t trigger){

	for(int i = 0; i < oclass[type]->effects.size(); ++i){
		
		if(oclass[type]->effects[i].trigger == trigger) return &(oclass[type]->effects[i].eff);
	}
}