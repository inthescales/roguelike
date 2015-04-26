#include "action.h"
#include "actor.h"
#include "argmap.h"
#include "classdefs.h"
#include "error.h"
#include "feature.h"
#include "interface.h"
#include "map.h"
#include "object.h"
#include "objclass.h"
#include "requirement.h"
#include "rogue.h"
#include "stringdefs.h"
#include "stringutils.h"
#include "symboldefs.h"
#include "tile.h"
#include "window.h"

#include <algorithm>
#include <stdio.h>
#include <math.h>

using std::max;

std::map<int, direction_t> * direction_key;
std::map<int, action *> * action_key;

void UI::setup_ui() {

    direction_key = new std::map<int, direction_t>();
    action_key = new std::map<int, action *>();
    
    (*direction_key)['0'+1] = DIR_DOWNLEFT;
    (*direction_key)['0'+2] = DIR_DOWN;
    (*direction_key)['0'+3] = DIR_DOWNRIGHT;
    (*direction_key)['0'+4] = DIR_LEFT;
    (*direction_key)['0'+5] = DIR_CENTER;
    (*direction_key)['0'+6] = DIR_RIGHT;
    (*direction_key)['0'+7] = DIR_UPLEFT;
    (*direction_key)['0'+8] = DIR_UP;
    (*direction_key)['0'+9] = DIR_UPRIGHT;
    
    // change action bindings to purposes - scan the actor's list
    
    // Bind keys
    (*action_key)[','] = actiondef[ACTION_TAKE_BASIC];
    (*action_key)['d'] = actiondef[ACTION_DROP_BASIC];
    (*action_key)['w'] = actiondef[ACTION_EQUIP_BASIC];
    (*action_key)['u'] = actiondef[ACTION_UNEQUIP_BASIC];
    (*action_key)['e'] = actiondef[ACTION_EAT_BASIC];
    (*action_key)['q'] = actiondef[ACTION_DRINK_BASIC];
    (*action_key)['o'] = actiondef[ACTION_OPEN_BASIC];
    (*action_key)['c'] = actiondef[ACTION_CLOSE_BASIC];
    
    (*action_key)['p'] = actiondef[ACTION_POINT];
    (*action_key)['l'] = actiondef[ACTION_LINE];
}

void UI::get_action(){

	int input;
	bool done = false;
    
    while (!done) {
    
        input = get_input();
        // Player pressed a motion key
        if (direction_key->count(input)) {
            done = command_direction(direction_key->at(input));
        }
        
        switch(input){            
            // Interface commands
            case 'i':
                command_inventory();
            break;
            case 'E':
                command_equipment();
            break;
            case 'C':
                command_conditions();
            break;
            case 'Q':
                command_quit();
            break;
            
            case 's':
            case '.':
            break;
            
            default:
                if (action_key->count(input) != 0) {
                    done = act_player->execute_action(action_key->at(input));
                }
                break;
        }
    
        if (win_output->should_update) {
            win_output->clear();
            win_output->print_buf(buf_main);
            break_buffer(buf_main);
            win_output->should_update = false;
        }
    }

}

int UI::get_input(){

	int input = wgetch(stdscr);
	
	return input;

}

// COMMANDS ==============================================

bool UI::command_direction(direction_t dir) {

	std::pair<int, int> offset = dir_to_offset(dir);
	
	actionPurpose_t purpose = ACTPUR_NONE;
	
	tile * dest = &map_current->tiles[act_player->x + offset.first][act_player->y + offset.second];
    argmap * args = new argmap();
    args->add_actor(ARG_ACTION_AGENT, act_player);
	
	if(dest->my_actor != NULL) {
        purpose = ACTPUR_HARM;
        vector<void*> * vect = new vector<void*>();
        vect->push_back((void*)(dest->my_actor));
        args->add_vector(ARG_ACTION_PATIENT, vect);
    } else if(dest->my_feature != NULL
           && act_player->can_open(dest->my_feature) == ERR_NONE) {
        purpose = ACTPUR_OPEN_FEAT;
        vector<void*> * vect = new vector<void*>();
        vect->push_back((void*)(dest->my_feature));
        args->add_vector(ARG_ACTION_PATIENT, vect);
    } else {
        // Check for a way to move and do so if possible.
        vector<action*> * moves = act_player->how_to_travel(&act_player->current_map->tiles[act_player->x][act_player->y], dest);
		if(moves->size() > 0) {
			purpose = ACTPUR_MOVE;
            vector<void*> * vect = new vector<void*>();
            vect->push_back((void*)dest);
            args->add_actor(ARG_ACTION_AGENT, act_player);
            args->add_vector(ARG_ACTION_LOCATION, vect);
            return (act_player->execute_action(moves->front(), args, false) == NULL);
        }
	}
	
    // Pick an action for a non-move purpose and do it. May merge with above later - but currently
    // only move actions are tested fully before attempting.
    bool done = false;
	vector<int> * actions = get_context_action(purpose);
    for(int i = 0; !done && i < actions->size(); ++i) {
        done = (act_player->execute_action(actiondef[actions->at(i)], args, false) == NULL);
    }
		
	return done;
}

vector<int> * UI::get_context_action(actionPurpose_t purp) {

    vector<int> * actions = act_player->get_actions_for(purp);
    action * r = NULL;
    
    vector<int>::iterator it = actions->begin();
    for(; it != actions->end(); ++it) {
        if (!actiondef[*it]->contextOk) {
            it = actions->erase(it);
        }
    }
    
    std::sort(actions->begin(), actions->end(), priority_comp);
    
    return actions;
}

// Info screens and UI commands ==========================

bool UI::command_inventory(){
	win_screen->display_inventory(*act_player, "Inventory:");
	window::display_all();
    
    return false;
}

bool UI::command_equipment(){
	win_screen->display_equipment(*act_player);
	window::display_all();
    
    return false;
}

bool UI::command_conditions(){
	win_screen->display_conditions(act_player);
	window::display_all();
    
    return false;
}

bool UI::command_quit() {
    exit_game(0);
}

// Master target selection function =====================================

vector<void*> * UI::prompt_target(targetActionBlock * in) {

    vector<tile*> * tile_vect = NULL;
    vector<void*> * r;
    bool didAssume = false;
    switch (in->target_type) {
    
        case TAR_SELF:
            if (in->args->get_int(ARG_TARGET_ASSUME) > 0) {
                tile_vect = assume_self(in->extract_type, in->args, in->requirements);
            }
            if (tile_vect != NULL) {
                didAssume = true;
            } else {
                tile_vect = prompt_self(act_player);
            }
            break;

        case TAR_ADJ:
            // If we can assume, give that a try before prompting
            if (in->args->get_int(ARG_TARGET_ASSUME) > 0) {
                tile_vect = assume_adjacent(in->extract_type, in->args, in->requirements);
            }
            if (tile_vect != NULL) {
                didAssume = true;
            } else {
                tile_vect = prompt_adjacent(in->prompt, in->args, in->requirements);
            }
            break;
        case TAR_TILE:
            if (in->radius_type == RAD_SINGLE) {
                tile_vect = prompt_tile(in->prompt, false, &act_player->current_map->tiles[act_player->x][act_player->y], in->args, in->requirements);
            } else if (in->radius_type == RAD_LINE) {
                tile_vect = prompt_tile(in->prompt, true, &act_player->current_map->tiles[act_player->x][act_player->y], in->args, in->requirements);
            }
        case TAR_NONE:
        default:
            break;
    
    }
    
    if (tile_vect == NULL) return NULL;
    
    if (in->extract_type == EXT_ACTOR) {
    
        r = (vector<void*>*)extract_actors(tile_vect, in->requirements);
    } if (in->extract_type == EXT_OBJECTS) {
    
        if (!didAssume) {
            r = (vector<void*>*)prompt_objects(in->prompt, extract_objects(tile_vect, in->requirements), in->args, in->requirements);
        } else {
            r = (vector<void*>*)extract_objects(tile_vect, in->requirements);
        }
        in->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_OBJECT);
    } else if (in->extract_type == EXT_INVENTORY) {
    
        if (tile_vect->size() == 1 && tile_vect->back()->my_actor == act_player) {
            r = (vector<void*>*)prompt_inventory(in->prompt, in->args, in->requirements);
        } else {
            r = (vector<void*>*)prompt_objects(in->prompt, extract_inventories(tile_vect, in->requirements), in->args, in->requirements);
        }
        window::display_all();
        in->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_OBJECT);
    } else if (in->extract_type == EXT_FEATURE) {
    
        r = (vector<void*>*)extract_features(tile_vect, in->requirements);
        in->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_FEATURE);
    }

    return r;
}

// Target selections prompts ============================================

vector<tile*> * UI::prompt_self(actor * agent) {

    vector<tile*> * r = new vector<tile*>();
    r->push_back(&(agent->current_map->tiles[agent->x][agent->y]));
    return r;
}

vector<tile*> * UI::prompt_adjacent(string prompt, argmap * args, vector<requirement*> * reqs) {

    vector<tile*> * r = new vector<tile*>();
    
    direction_t dir = prompt_direction(prompt);
    if (dir == DIR_NULL) {
        win_output->print(error_string[ERR_CANCELLED]);
        return NULL;
    }
        
    std::pair<int, int> offset = dir_to_offset(dir);
    std::pair<int, int> cur = std::pair<int, int>(act_player->x + offset.first, act_player->y + offset.second);
    
    int minDist = (args->has_value(ARG_TARGET_MIN_DISTANCE) ? args->get_int(ARG_TARGET_MIN_DISTANCE) : 1);
    int maxDist = (args->has_value(ARG_TARGET_MAX_DISTANCE) ? args->get_int(ARG_TARGET_MAX_DISTANCE) : 1);
    int maxNum = (args->has_value(ARG_TARGET_NUMBER) ? args->get_int(ARG_TARGET_NUMBER) : 1);
        
    for(int i = minDist; i <= maxDist && r->size() < maxNum; ++i) {
    
        tile * cur_tile = &map_current->tiles[cur.first][cur.second];
        if (requirement::check_requirements_for(reqs, cur_tile, NULL)) {
            r->push_back(cur_tile);
        }
        cur.first += offset.first;
        cur.second += offset.second;
    }
    
    return r;
}

/*
    Prompt for a specific tile. The origin argument specifies an origin for
    range-based targeting and resolving lines. If it is null, only a single
    tile can be returned - the one selected.
*/
vector<tile*> * UI::prompt_tile(string prompt, bool line, tile * origin, argmap * args, vector<requirement*> * reqs) {
    
    map * m = map_current;
    window * w = win_world;
    int cur_x = 0, cur_y = 0;
    vector<tile*> * ret = new vector<tile*>();
    vector<tile*> * last_sel = new vector<tile*>();
    vector<tile*> * cur_sel = new vector<tile*>();
    
    int minDist = (args->has_value(ARG_TARGET_MIN_DISTANCE) ? args->get_int(ARG_TARGET_MIN_DISTANCE) : 1);
    int maxDist = (args->has_value(ARG_TARGET_MAX_DISTANCE) ? args->get_int(ARG_TARGET_MAX_DISTANCE) : 1);
    
    if (origin != NULL) {
        cur_x = origin->x;
        cur_y = origin->y;
        //cur_sel->push_back(origin);
    }
    
    if (prompt != "") {
        win_output->print(prompt);
        win_output->clear();
        win_output->print_buf(buf_main);
        break_buffer(buf_main);
    }
    
    bool done = false;
    int input;
    
    while (!done) {
        
        // Display latest markers
        if (!line) {
            float dist = floor(sqrt( pow( fabs(cur_x - origin->x), 2) + pow( fabs(cur_y - origin->y), 2) ));
            colorName col = (dist >= minDist && dist <= maxDist) ? C_YELLOW : C_RED;
            w->display_glyph(glyph(symboldef[CHAR_X], col), w->x + cur_x, w->y + cur_y);
        } else {
        
            float dist;
            colorName col;
            bool broken = false;
            cur_sel = tile::line_between(origin, &m->tiles[cur_x][cur_y]);
            if (cur_sel->size() > 0) {
                vector<tile*>::iterator it = cur_sel->begin();
                for(; it != cur_sel->end(); ++it) {
                    dist = tile::distance_between(origin, (*it));
                    col = (!broken && dist >= minDist && dist <= maxDist) ? C_YELLOW : C_RED;
                    w->display_glyph(glyph(symboldef[CHAR_X], col), w->x + (*it)->x, w->y + (*it)->y);
                    
                    // Break
                    if (!broken && args->has_flag(FLAG_TARGET_BREAK_PROJECTILE) && ((*it)->my_actor != NULL || ((*it)->my_feature != NULL && (*it)->my_feature->has_flag(FLAG_FEAT_NO_FLY)))) {
                        broken = true;
                    }
                }
            }
        }
        move(w->y + cur_y, w->x + cur_x);
         
        // Handle input
        input = wgetch(stdscr);
        
        if (input == ESCAPE_KEY) {
        
            done = true;
        }
        else if (input == ENTER_KEY) {
        
            ret->insert(ret->begin(), cur_sel->begin(), cur_sel->end());
            done = true;
        } else if (direction_key->count(input) != 0) {
            
            // The target changed - undo our old markers
            if (!line) {
                w->display_tile(m, cur_x, cur_y);
            } else {
                last_sel = cur_sel;
                if (last_sel->size() > 0) {
                    vector<tile*>::iterator it = last_sel->begin();
                    for(; it != last_sel->end(); ++it) {
                        w->display_tile(*it);
                    }
                }
            }
            
            // Update target
            std::pair<int,int> pair = dir_to_offset(direction_key->at(input));
            if (cur_x + pair.first < m->width &&
                cur_x + pair.first >= 0 &&
                cur_y + pair.second < m->height &&
                cur_y + pair.second >= 0) {
                cur_x += pair.first;
                cur_y += pair.second;
            }
        }
    }
    
    if (!line) {
        w->display_tile(m, cur_x, cur_y);
    } else {
        // Remove last markers
        vector<tile*>::iterator it = cur_sel->begin();
        for(; it != cur_sel->end(); ++it) {
            w->display_tile(*it);
        }
    }
    
    return ret;
}

// Prompt from a set of objects using interactive menu
vector<object*> * UI::prompt_objects(string prompt, vector<object*> * items, argmap * args, vector<requirement*> * reqs){

	vector<object*> * ret = new vector<object*>();
	if (items->size() <= 0) {
        return ret;
    }

    bool gold_ok = args->has_flag(FLAG_TARGET_GOLDOK);

    args->add_flag(FLAG_MENU_SORT);
	ret = menu_select_objects(win_screen, prompt, items, args);
	return ret;
}

// Prompt the player's inventory (special case because of letters)
vector<object*> * UI::prompt_inventory(string prompt, argmap * args, vector<requirement*> * reqs){

	int input;
    vector<object*> * items = act_player->inventory;
	vector<object*> * ret = new vector<object*>();
    if (items->size() <= 0) {
        return ret;
    }
    
	win_output->print(prompt);
    win_output->clear();
    win_output->print_buf(buf_main);
    break_buffer(buf_main);
    bool gold_ok = args->has_flag(FLAG_TARGET_GOLDOK);

	while(true){
		input = wgetch(stdscr);
		
		if(input == '$' && gold_ok){
            object * goldObj = prompt_gold_to_object();
            if (requirement::check_requirements_for(reqs, goldObj, NULL)) {
                ret->push_back(goldObj);
            }
			return ret;
		} else if(input == '*' || input == '?'){
            args->add_flag(FLAG_MENU_SORT);
            args->add_flag(FLAG_MENU_PLAYER);
			ret = menu_select_objects(win_screen, prompt, items, args);
			return ret;
		} else if(input == ESCAPE_KEY){
			return ret;
		} else {
			int slot = letter_to_int(input);
			if(slot != -1 && obj_letter[slot] != NULL) {
				ret->push_back(obj_letter[slot]);
			}
			return ret;
		}
	}
}

// Assume targets if possible =============================================

vector<tile*> * UI::assume_self(extract_t extr, argmap * args, vector<requirement*> * reqs) {

    vector<tile*> * tiles = prompt_self(act_player);
    return assume(tiles, extr, args, reqs);
}

vector<tile*> * UI::assume_adjacent(extract_t extr, argmap * args, vector<requirement*> * reqs) {

    vector<tile*> * tiles = tile::adjacent_to(&(act_player->current_map->tiles[act_player->x][act_player->y]));
    return assume(tiles, extr, args, reqs);
}

vector<tile*> * UI::assume(vector<tile*> * tiles, extract_t extr, argmap * args, vector<requirement*> * reqs) {

    
    vector<tile*> * ret = new vector<tile*>();
    int found = 0;
    
    // For each adjacent tile, see if it has the kind of thing we're looking for
    // Add to the return list, but return NULL if there are too many
    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
    
        // Check relevant entities in each tile, return NULL if too many are found
        if (extr == EXT_ACTOR) {
        
            if ((*it)->my_actor != NULL && requirement::check_requirements_for(reqs, *it, NULL)) {
                ret->push_back(*it);
                ++found;
            }
        } if (extr == EXT_OBJECTS) {
        
            if ((*it)->my_objects->size() > 0) {
                if ((*it)->my_objects->size() > args->get_int(ARG_TARGET_ASSUME)) {
                    return NULL;
                } else {
                    // Get the number of acceptable objects
                    vector<object*>::iterator it2 = (*it)->my_objects->begin();
                    bool newly_found = 0;
                    for(; it2 != (*it)->my_objects->end(); ++it2) {
                        if(requirement::check_requirements_for(reqs, *it2, NULL)) {
                            ++newly_found;
                        }
                    }
                    // If this tile contains objects we can assume, add it
                    if (newly_found > 0) {
                        found += newly_found;
                        ret->push_back(*it);
                    }
                }
            }
        } else if (extr == EXT_FEATURE) {
        
            if ((*it)->my_feature != NULL && requirement::check_requirements_for(reqs, *it, NULL)) {
                ret->push_back(*it);
                ++found;
            }
        }
        
        if (found > args->get_int(ARG_TARGET_ASSUME)) {
            return NULL;
        }
    }
    
    if (ret->size() == 0) {
        return NULL;
    }
    
    return ret;
}

// Extract targets from tiles =========================================

vector<actor*> * UI::extract_actors(vector<tile*> * tiles, vector<requirement*> * reqs) {
    
    vector<actor*> * r = new vector<actor*>();
    
    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        if ((*it)->my_actor != NULL) {
            if (requirement::check_requirements_for(reqs, (*it)->my_actor, NULL)) {
                r->push_back((*it)->my_actor);
            }
        }
    }
    
    return r;
}

vector<object*> * UI::extract_objects(vector<tile*> * tiles, vector<requirement*> * reqs) {
    
    vector<object*> * r = new vector<object*>();

    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        vector<object*>::iterator it2 = (*it)->my_objects->begin();
        for(; it2 != (*it)->my_objects->end(); ++it2) {
            if (requirement::check_requirements_for(reqs, *it2, NULL)) {
                r->push_back(*it2);
            }
        }
    }
    
    return r;
}

vector<object*> * UI::extract_inventories(vector<tile*> * tiles, vector<requirement*> * reqs) {
    
    vector<object*> * r = new vector<object*>();
    
    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        if ((*it)->my_actor != NULL) {
            actor * cur_act = (*it)->my_actor;
            vector<object*>::iterator it2 = cur_act->inventory->begin();
            for(; it2 != cur_act->inventory->end(); ++it2) {
                if (requirement::check_requirements_for(reqs, *it2, NULL)) {
                    r->push_back((*it2));
                }
            }
        }
    }
    
    return r;
}

vector<feature*> * UI::extract_features(vector<tile*> * tiles, vector<requirement*> * reqs) {
    
    vector<feature*> * r = new vector<feature*>();
    
    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        if ((*it)->my_feature != NULL) {
            if (requirement::check_requirements_for(reqs, (*it)->my_feature, NULL)) {
                r->push_back((*it)->my_feature);
            }
        }
    }
    
    return r;
}

// Targeting menus =============================================

/*
    Menu for selecting a number of items from a set.
    Uses special rules for when that set is the player's inventory, as in that case
    we're using the letters attached to the objects to refer to them. In any other case,
    we simply assign them letters in order.
    
    The selected array stores whether an item has been selected by the player. This letter
    corresponds to the letter of the item in player mode, not necessarily its order in the list
    since we're sorting things.
*/
vector<object*> * UI::menu_select_objects(window * win, string prompt, vector<object*> * in_items, argmap * args){

	curs_set(0);
	
    
    int start = 0, winsize = 10;
	int x = 3, y = 3;
    int max_select = args->get_int(ARG_TARGET_NUMBER);
    int player_inv = args->has_flag(FLAG_MENU_PLAYER);
    
    const char sym[] = {'-', '+'};
    int sel_size = (player_inv) ? max(INV_MAX, (int)(in_items->size())) : in_items->size();
    bool selected[sel_size];
    for(int i = 0; i < sel_size; ++i) selected[i] = 0;
    vector<object*> * items = new vector<object*>();
    (*items) = (*in_items);
    
    int sel_count = 0;
	int input, headers;
	vector<object*> * ret = new vector<object*>();
	
	if(args->has_flag(FLAG_MENU_SORT)){
		std::sort(items->begin(), items->end(), object::compare_type);
	}
	
	while(true){
	
		clear();
		printcolor(x, y, prompt);
		headers = 1;
        
		for(int i = start; i - start < winsize && i < items->size(); ++i){
		
			if(args->has_flag(FLAG_MENU_SORT) && (i == start || items->at(i)->get_type() != items->at(i-1)->get_type())) {
				//We need to print a header
				printcolor(x, y + i + headers++ - start, color_string(str_obj_type[items->at(i)->get_type()], C_YELLOW));
			}
            char sel_symb;
            string to_print = "";
            if (player_inv) {
                sel_symb = sym[selected[letter_to_int(items->at(i)->letter)]];
                to_print = to_print + (char)(items->at(i)->letter) + " " + sel_symb + " " + items->at(i)->get_name();
            } else {
                sel_symb = sym[selected[i]];
                to_print = to_print + int_to_letter(i) + " " + sel_symb + " " + items->at(i)->get_name();
            }

            printcolor(x, y + (i - start) + headers, to_print);
		}
		
		input = wgetch(stdscr);
		
		if((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')){
		
            if(max_select == 1) {
                // With no multi select, return immediately        
                if (player_inv) {
                    int num = letter_to_int(input);
                    if (num != -1 && obj_letter[num] != NULL) {
                        ret->push_back(obj_letter[num]);
                    }
                } else {
                    int num = letter_to_int(input) + start;
                    if (num < items->size()) {
                        ret->push_back(items->at(num));
                    }
                }
                return ret;
            } else if (max_select == -1 || max_select > sel_count) {
                // Toggle selection for this letter, and update count
                int to_add;
                to_add = letter_to_int(input) + start;
                
                if (to_add != -1) {
                    sel_count += (selected[to_add]) ? -1 : 1;
                    selected[to_add] = !selected[to_add];
                }
            }
		} else {
		
            if (player_inv) {
                for(int i = 0; i < items->size(); ++i) {
                    if (selected[letter_to_int(items->at(i)->letter)]) {
                        ret->push_back(items->at(i));
                    }
                }
            } else {
                for(int i = 0; i < items->size(); ++i) {
                    if (selected[i]) {
                        ret->push_back(items->at(i));
                    }
                }
            }
            
			switch(input){
				case ENTER_KEY:			
					return ret;
				case ESCAPE_KEY:
					return new vector<object*>();
				case KEY_UP:
					if(start > 0)
						start -= 1;
					break;
				case KEY_DOWN:
					if(start + winsize < items->size())
						start += 1;
					break;
				default:
					break;
			}
		}
		
	}
		
	curs_set(1);
}

// Simple prompts ============================================

// Prompt for a yes/no question
bool UI::prompt_yesno(string prompt){

	win_output->print(prompt + "(y/n)");
    win_output->clear();
	win_output->print_buf(buf_main);
    break_buffer(buf_main);
    
	char r = ' ';
	
	while(r != 'y' && r != 'n'){
		r = wgetch(stdscr);
	}
	
	return r == 'y';
}

object * UI::prompt_gold_to_object(){

	int amount;
	
	win_output->print("How much? (have ?)");
    win_output->clear();
    win_output->print_buf(buf_main);
    break_buffer(buf_main);
	scanf("%d", &amount);
	
	if(amount > act_player->gold){
		win_output->print("You don't have that much.");
		return NULL;
	} else {
		object * ret = new object(3, amount);
		act_player->gold -= amount;
		return ret;
	}
}

direction_t UI::prompt_direction(string prompt) {

    if (prompt != "") {
        win_output->print(prompt);
        win_output->clear();
        win_output->print_buf(buf_main);
        break_buffer(buf_main);
    }
    
    
    int input = wgetch(stdscr);
        
    if (direction_key->count(input) != 0) {
        return direction_key->at(input);
    }
    
    return DIR_NULL;
}

// UTILITIES =======================================

// Get an element from a size-one vector.
object * UI::get_single(vector<object*> * vect){
	if(vect->size() == 1){
		return vect->back();
	} else {
		return NULL;
	}
}

std::pair<int,int> UI::dir_to_offset(direction_t dir){
	std::pair<int, int> r;

    switch(dir) {
        case DIR_UPLEFT:
            r.first = -1; r.second = -1; break;
        case DIR_UP:
            r.first = 0; r.second = -1; break;
        case DIR_UPRIGHT:
            r.first = 1; r.second = -1; break;
        case DIR_LEFT:
            r.first = -1; r.second = 0; break;
        case DIR_RIGHT:
            r.first = 1; r.second = 0; break;
        case DIR_DOWNLEFT:
            r.first = -1; r.second = 1; break;
        case DIR_DOWN:
            r.first = 0; r.second = 1; break;
        case DIR_DOWNRIGHT:
            r.first = 1; r.second = 1; break;
            
        case DIR_CENTER:
        case DIR_NULL:
        default:
            r.first = r.second = 0; break;
    }
	
	return r;
}

char UI::int_to_letter(int in){
	if(in >= 0 && in < 'z' - 'a')
		return 'a' + in;
	else if(in > 'z' && in < 'Z' - 'A')
		return 'A' + (in - ('z' -'a'));
	else return 0;
}

int UI::letter_to_int(char in){
	if(in >= 'a' && in <= 'z')
		return in - 'a';
	else if(in >= 'A' && in <= 'Z')
		return in - 'A' + ('z' - 'a');
	else return -1;
}

char UI::get_next_letter(){

	int i;
	for(i = 0; obj_letter[i] != NULL && i < INV_MAX; ++i);
	
	if(i == INV_MAX) return 0;
	
	return int_to_letter(i);
}
