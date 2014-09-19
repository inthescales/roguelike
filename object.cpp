#include "condition.h"
#include "globals.h"
#include "object.h"
#include "objclass.h"
#include "stringutils.h"

object::object(short code) : type(code), equipped(0), quantity(1) {

}

object::object(short code, short quant) : type(code), equipped(0), quantity(quant) {

}

glyph object::get_glyph(){
  return oclass[type]->get_glyph();
}

symbol_code object::get_symbol(){
  return get_glyph().get_symbol();
}

colorName object::get_color(){
  return get_glyph().get_color();
}

string object::get_name(){
	return oclass[type]->name;
}

string object::get_name_color(){
  return color_string(get_name(), oclass[type]->get_color());
}

bool object::compare_type(object * a, object * b){
	return oclass[a->type]->type < oclass[b->type]->type;
}

// Stats management =========================

bool object::has_stat(stats_t code){

	return oclass[type]->stats->has_stat(code);
}

int object::get_stat(stats_t code, bool always_return){

	int value = 0;

	if (has_stat(code)){
	
		value += oclass[type]->stats->get_stat(code);
		value += get_cond_stat(code);
		return value;
	}
	else if (always_return) {
		value += get_cond_stat(code);
		return value;
	}
	
	return -1;
}

int object::get_stat(stats_t code){

	return get_stat(code, true);
}

// Effect management =========================

effect * object::get_effect(trigger_t trigger){

	for(int i = 0; i < oclass[type]->effects.size(); ++i){
		
		if(oclass[type]->effects[i].trigger == trigger) return &(oclass[type]->effects[i].eff);
	}
	
	return NULL;
}

void object::resolve_trigger(trigger_t trigger, argmap * args) {
	
	effect * my_effect = get_effect(trigger);
	argmap * my_map = new argmap();
	my_map->add_object(ARG_HOLDER_OBJECT, this);
	if (args != NULL) {
		my_map->add_args(args);
	}
	
	if (my_effect != NULL) {
		do_effect(my_map, my_effect);
	}
	
	vector<condition*>::iterator cit = conditions.begin();
	for(; cit != conditions.end(); ++cit) {
	
		(*cit)->resolve_trigger(trigger, my_map);
	}
}

// Condition management =========================

bool object::has_condition(int code) {

	vector<condition*>::iterator it = conditions.begin();
	
	for(; it != conditions.end(); ++it) {
		
		if ((*it)->type == code) {
			return true;
		}
	}
	
	return false;
}

condition * object::get_condition(int code) {

	vector<condition*>::iterator it = conditions.begin();
	
	for(; it != conditions.end(); ++it) {
		
		if ((*it)->type == code) {
			return *it;
		}
	}
	
	return NULL;
}

bool object::add_condition(condition * cond) {

	vector<condition*>::iterator it = conditions.begin();
	
	for(; it != conditions.end(); ++it) {
		
		if ((*it)->type == cond->type) {
		
			(*it)->add_condition(cond);
			return true;
		}
	}
	
	conditions.push_back(cond);
	return false;
}

bool object::remove_condition(int code) {

	vector<condition*>::iterator it = conditions.begin();
	
	for(; it != conditions.end(); ++it) {
		
		if ((*it)->type == code) {
			conditions.erase(it);
			return true;
		}
	}
	
	return false;
}

int object::get_cond_stat(stats_t code) {
	
	int ret = 0;
	
	for(int i = 0; i < conditions.size(); ++i){
	
		int value = conditions[i]->get_stat(code);
		if(value != -1) {
			ret += value;
		}
	}
	
	return ret;
}

// Process decay on my conditions, and my inventory's
void object::decay_conditions() {

	vector<condition*>::iterator it = conditions.begin();
	for(; it != conditions.end(); ++it) {
		
		if(!(*it)->do_decay()) {
			condition * dead_cond = (*it);
			it = conditions.erase(it);
			delete(dead_cond);
		}
	}
}
