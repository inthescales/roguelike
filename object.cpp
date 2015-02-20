#include "condition.h"
#include "object.h"
#include "objclass.h"
#include "stringutils.h"

// Setup and teardown ======================

object::object(short code) {

	my_class = (entityclass*)oclass[code];
    individual_name = "";
    equipped = false;
    quantity = 1;
    init();
}

object::object(short code, short quant) {

	my_class = (entityclass*)oclass[code];
    individual_name = "";
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

object_type object::get_type() {
    return get_class()->type;
}

object_subtype object::get_subtype() {
    return get_class()->subtype;
}

bool object::compare_type(object * a, object * b){
	return a->get_type() < b->get_type();
}

bool object::can_stack(object * a, object * b) {
    return a->get_class() == b->get_class();
}

// Effect management =========================

void object::resolve_trigger(trigger_t trigger, argmap * args)
{
    argmap * my_map = new argmap(args);
    my_map->add_object(ARG_HOLDER_OBJECT, this);
    
    mapentity::resolve_trigger(trigger, my_map);
}
