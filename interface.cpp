#include "action.h"
#include "actor.h"
#include "argmap.h"
#include "classdefs.h"
#include "error.h"
#include "feature.h"
#include "flagset.h"
#include "globals.h"
#include "interface.h"
#include "map.h"
#include "object.h"
#include "objclass.h"
#include "requirement.h"
#include "stringutils.h"
#include "tile.h"
#include "window.h"

#include <algorithm>
#include <stdio.h>

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
		if(act_player->can_travel(dest) == ERR_NONE) {
			purpose = ACTPUR_MOVE;
            vector<void*> * vect = new vector<void*>();
            vect->push_back((void*)dest);
            args->add_vector(ARG_ACTION_PATIENT, vect);
        }
	}
	
    bool done = false;
	vector<int> * actions = get_context_action(purpose);
    for(int i = 0; !done && i < actions->size(); ++i) {
        done = act_player->execute_action(actiondef[actions->at(i)], args, false);
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
	redraw_windows();
    
    return false;
}

bool UI::command_equipment(){
	win_screen->display_equipment(*act_player);
	redraw_windows();
    
    return false;
}

bool UI::command_conditions(){
	win_screen->display_conditions(act_player);
	redraw_windows();
    
    return false;
}

bool UI::command_quit() {
  exit_game(0);
}

// Target prompts =====================================

// Handle all prompts related to targeting for actions.
vector<void*> * UI::prompt_target(targetActionBlock * in) {

    vector<tile*> * tile_vect;;
    vector<void*> * r;
    switch (in->target_type) {
    
        case TAR_SELF:
            tile_vect = target_self(act_player);
            break;
        case TAR_ADJ:
            tile_vect = prompt_adjacent(in->prompt, in->args, in->requirements);
            break;
        case TAR_NONE:
        default:
            break;
    
    }
    
    if (tile_vect == NULL) return NULL;
    
    if (in->extract_type == EXT_ACTOR) {
    
        r = (vector<void*>*)extract_actors(tile_vect);
    } if (in->extract_type == EXT_OBJECTS) {
    
        r = (vector<void*>*)prompt_objects(in->prompt, extract_objects(tile_vect), in->args, in->requirements);
        in->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_OBJECT);
    } else if (in->extract_type == EXT_INVENTORY) {
    
        if (tile_vect->size() == 1 && tile_vect->back()->my_actor == act_player) {
            r = (vector<void*>*)prompt_inventory(in->prompt, in->args, in->requirements);
        } else {
            r = (vector<void*>*)prompt_objects(in->prompt, extract_inventories(tile_vect), in->args, in->requirements);
        }
        window::display_all();
        in->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_OBJECT);
    } else if (in->extract_type == EXT_FEATURE) {
    
        r = (vector<void*>*)extract_features(tile_vect);
        in->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_FEATURE);
    }

    return r;
}

vector<tile*> * UI::target_self(actor * agent) {

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
    
    int maxDist = (args->has_value(ARG_TARGET_DISTANCE) ? args->get_int(ARG_TARGET_DISTANCE) : 1);
    int maxNum = (args->has_value(ARG_TARGET_NUMBER) ? args->get_int(ARG_TARGET_NUMBER) : 1);
        
    for(int i = 0; i < maxDist && r->size() < maxNum; ++i) {
    
        tile * cur_tile = &map_current->tiles[cur.first][cur.second];
        if (requirement::check_requirements_for(cur_tile, reqs)) {
            r->push_back(cur_tile);
        }
        cur.first += offset.first;
        cur.second += offset.second;
    }
    
    return r;
}

vector<actor*> * UI::extract_actors(vector<tile*> * tiles) {
    
    vector<actor*> * r = new vector<actor*>();
    
    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        if ((*it)->my_actor != NULL) {
            r->push_back((*it)->my_actor);
        }
    }
    
    return r;
}

vector<object*> * UI::extract_objects(vector<tile*> * tiles) {
    
    vector<object*> * r = new vector<object*>();

    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        vector<object*>::iterator it2 = (*it)->my_objects->begin();
        for(; it2 != (*it)->my_objects->end(); ++it2) {
            r->push_back(*it2);
        }
    }
    
    return r;
}

vector<object*> * UI::extract_inventories(vector<tile*> * tiles) {
    
    vector<object*> * r = new vector<object*>();
    
    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        if ((*it)->my_actor != NULL) {
            actor * cur_act = (*it)->my_actor;
            vector<object*>::iterator it2 = cur_act->inventory->begin();
            for(; it2 != cur_act->inventory->end(); ++it2) {
                r->push_back((*it2));
            }
        }
    }
    
    return r;
}

vector<feature*> * UI::extract_features(vector<tile*> * tiles) {
    
    vector<feature*> * r = new vector<feature*>();
    
    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        if ((*it)->my_feature != NULL) {
            r->push_back((*it)->my_feature);
        }
    }
    
    return r;
}

vector<object*> * UI::prompt_objects(string prompt, vector<object*> * items, argmap * args, vector<requirement*> * reqs){

	vector<object*> * ret = new vector<object*>();
	if (items->size() <= 0) {
        return ret;
    }

    bool gold_ok = (args->get_int(ARG_TARGET_GOLDOK) == 1);
    int assume_threshold = args->get_int(ARG_TARGET_ASSUME);
    
    if (items->size() <= assume_threshold) {
        for(int i = 0; i < assume_threshold && i < items->size(); ++i) {
            ret->push_back(items->at(i));
        }
        return ret;
    }
    
    flagset * flags = new flagset();
    flags->add_flag(FLAG_MENU_SORT);
	ret = menu_select_objects(win_screen, prompt, items, args, flags);
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
    bool gold_ok = (args->get_int(ARG_TARGET_GOLDOK) == 1);
    int assume_threshold = args->get_int(ARG_TARGET_ASSUME);
    
    if (items->size() <= assume_threshold) {
        ret = items;
        return ret;
    }
    
	while(true){
		input = wgetch(stdscr);
		
		if(input == '$' && gold_ok){
            object * goldObj = prompt_gold_to_object();
            if (requirement::check_requirements_for(goldObj, reqs)) {
                ret->push_back(goldObj);
            }
			return ret;
		} else if(input == '*' || input == '?'){
            flagset * flags = new flagset();
            flags->add_flag(FLAG_MENU_SORT);
            flags->add_flag(FLAG_MENU_PLAYER);
			ret = menu_select_objects(win_screen, prompt, items, args, flags);
			return ret;
		} else if(input == 27){
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

// Complex menus =============================================

/*
    Menu for selecting a number of items from a set.
    Uses special rules for when that set is the player's inventory, as in that case
    we're using the letters attached to the objects to refer to them. In any other case,
    we simply assign them letters in order.
    
    The selected array stores whether an item has been selected by the player. This letter
    corresponds to the letter of the item in player mode, not necessarily its order in the list
    since we're sorting things.
*/
vector<object*> * UI::menu_select_objects(window * win, string prompt, vector<object*> * in_items, argmap * args, flagset * flags){

	curs_set(0);
	
    
    int start = 0, winsize = 10;
	int x = 3, y = 3;
    int max_select = args->get_int(ARG_TARGET_NUMBER);
    int player_inv = flags->has_flag(FLAG_MENU_PLAYER);
    
    const char sym[] = {'-', '+'};
    int sel_size = (player_inv) ? max(INV_MAX, (int)(in_items->size())) : in_items->size();
    bool selected[sel_size];
    for(int i = 0; i < sel_size; ++i) selected[i] = 0;
    vector<object*> * items = new vector<object*>();
    (*items) = (*in_items);
    
    int sel_count = 0;
	int input, headers;
	vector<object*> * ret = new vector<object*>();
	
	if(flags->has_flag(FLAG_MENU_SORT)){
		std::sort(items->begin(), items->end(), object::compare_type);
	}
	
	while(true){
	
		clear();
		printcolor(x, y, prompt);
		headers = 1;
        
		for(int i = start; i - start < winsize && i < items->size(); ++i){
		
			if(flags->has_flag(FLAG_MENU_SORT) && (i == start || items->at(i)->get_type() != items->at(i-1)->get_type())) {
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
				case 10:			
					return ret;
				case 27:
					return ret;
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

    win_output->print(prompt);
    win_output->clear();
    win_output->print_buf(buf_main);
    break_buffer(buf_main);
    
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
