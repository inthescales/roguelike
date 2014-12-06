#include "condition.h"
#include "globals.h"
#include "object.h"
#include "objclass.h"
#include "stringutils.h"

// Setup and teardown ======================

object::object(short code) {

	my_class = (entityclass*)oclass[code];
    equipped = false;
    quantity = 1;
    init();
}

object::object(short code, short quant) {

	my_class = (entityclass*)oclass[code];
    equipped = false;
    quantity = quant;
    
    init();
}

void object::init() {

    mapentity::init();
}

// Basic info functions ======================

objclass * object::get_class(){
    return (objclass *)entity::get_class();
}

string object::get_name() {
    if (individual_name.length() > 0) {
        return individual_name;
    } else if (get_class()->assigned_name.length() > 0) {
        return get_class()->assigned_name;
    } else {
        return get_class()->name;
    }
}

string object::get_name_color() {
    return color_string(get_name(), get_color());
}

object_type object::get_type() {
    return get_class()->type;
}

object_subtype object::get_subtype() {
    return get_class()->subtype;
}

bool object::compare_type(object * a, object * b){
	return a->get_type() < b->get_type();
}

// Effect management =========================

void object::resolve_trigger(trigger_t trigger, argmap * args)
{
    argmap * my_map = new argmap(args);
    my_map->add_object(ARG_HOLDER_OBJECT, this);
    
    mapentity::resolve_trigger(trigger, my_map);
}
