#include "condition.h"
#include "condclass.h"
#include "mapentity.h"
#include "mapentityclass.h"
#include "stringutils.h"
#include "timer.h"

mapentity::mapentity() {

    init();
}

void mapentity::init() {
    
    entity::init();
    condition_list = new vector<condition *>();
}

// Basics ==========================

mapentityclass * mapentity::get_class() {

    return (mapentityclass *)entity::get_class();
}

void mapentity::destroy() {
    destroy_timers(current_map);
}

// Display functions =============================

glyph mapentity::get_glyph(){
    return get_class()->my_glyph;
}

symbol_code mapentity::get_symbol(){
    return get_glyph().get_symbol();
}

colorName mapentity::get_color(){
    return get_glyph().get_color();
}

string mapentity::get_name_color() {
    return color_string(get_name(), get_color());
}

// Stat management ============================

bool mapentity::has_cond_stat(stats_t code) {
	
    vector<condition *>::iterator it = condition_list->begin();
	for(; it != condition_list->end(); ++it){
	
		if((*it)->has_stat(code)) {
			return true;
		}
	}
	
	return false;
}

bool mapentity::has_stat(stats_t code) {

    if (entity::has_stat(code)) {
        return true;
    } else if(get_cond_stat(code)) {
        return true;
    }
	return false;
}

int mapentity::get_cond_stat(stats_t code) {
	
	int ret = 0;
	
    vector<condition *>::iterator it = condition_list->begin();
	for(; it != condition_list->end(); ++it){
	
		if((*it)->has_stat(code)) {
			ret += (*it)->get_stat(code);
		}
	}
	
	return ret;
}

int mapentity::get_stat(stats_t code) {

	int value = entity::get_stat(code);
	value += get_cond_stat(code);
	return value;

}

// Flag management ============================

bool mapentity::has_cond_flag(flags_t code) {
	
	bool ret = 0;
	
    vector<condition *>::iterator it = condition_list->begin();
	for(; it != condition_list->end(); ++it){
	
		if((*it)->has_flag(code)) {
			return true;
		}
	}
	
	return false;
}

bool mapentity::has_flag(flags_t code) {

	bool conds = has_cond_flag(code);
    if (conds) {
        return true;
    } else {
        return entity::has_flag(code);
    }
}

// Trigger effect management =============================

void mapentity::resolve_trigger(trigger_t trigger, argmap * args)
{
    argmap * my_map = new argmap(args);
    my_map->add_entity(ARG_HOLDER_ENTITY, this);
    
    entity::resolve_trigger(trigger, args);
    
    // Effects from attached conditions
    vector<condition*>::iterator cit = condition_list->begin();
	for(; cit != condition_list->end(); ++cit) {
	
        (*cit)->resolve_trigger(trigger, my_map);
	}
}

// Condition management =========================

bool mapentity::has_condition(int code) {

	vector<condition*>::iterator it = condition_list->begin();
	
	for(; it != condition_list->end(); ++it) {
		
		if ((*it)->get_class()->index == code) {
			return true;
		}
	}
	
	return false;
}

condition * mapentity::get_condition(int code) {

	vector<condition*>::iterator it = condition_list->begin();
	
	for(; it != condition_list->end(); ++it) {
		
		if ((*it)->get_class()->index == code) {
			return *it;
		}
	}
	
	return NULL;
}

bool mapentity::add_condition(condition * cond) {

	vector<condition*>::iterator it = condition_list->begin();
	
    condition * found = get_condition(cond->get_class()->index);
	if (found != NULL) {
		found->add_condition(cond);
        return true;
	}
	
	condition_list->push_back(cond);
	return false;
}

bool mapentity::remove_condition(int code) {

	vector<condition*>::iterator it = condition_list->begin();
	
	for(; it != condition_list->end(); ++it) {
		
		if ((*it)->get_class()->index == code) {
			condition_list->erase(it);
			return true;
		}
	}
	
	return false;
}
