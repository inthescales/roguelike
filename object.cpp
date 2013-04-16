#include "globals.h"
#include "graphics.h"
#include "object.h"

object::object(short code) : type(code), quantity(1) {

}

object::object(short code, short quant) : type(code), quantity(quant) {

}

string object::get_name(){
	return oclass[type].name;
}

chtype object::get_img(){
	return comp(oclass[type].symbol, oclass[type].color);
}