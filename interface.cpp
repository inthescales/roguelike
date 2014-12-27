#include <stdio.h>

#include "action.h"
#include "actor.h"
#include "argmap.h"
#include "globals.h"
#include "interface.h"
#include "map.h"
#include "object.h"
#include "objclass.h"
#include "requirement.h"
#include "stringutils.h"
#include "tile.h"
#include "window.h"

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
    
    // Basic actions for players ========================
    // Eat
    action * eat_action = new action();
    targetActionBlock * eat_target_block = new targetActionBlock("Eat what?", TAR_INV , RAD_SINGLE, ACTROLE_PATIENT);
    eat_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    eat_target_block->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_OBJECT);
    eat_action->add_block(eat_target_block);
    requirementActionBlock * eat_req_block = new requirementActionBlock(false, false);
    eat_req_block->requirements->push_back(new requirement("You can't eat that.", REQ_ACTOR_CAN_EAT));
    eat_action->add_block(eat_req_block);    
    argmap * eat_effect_args = new argmap();
    eat_action->add_block(new effectActionBlock(eat_effect_args, new effect(EFF_EAT)));
    
    (*action_key)['e'] = eat_action;

}

void UI::get_action(){

	int input = get_input();
	
    // Player pressed a motion key
    if (direction_key->count(input)) {
        command_move(direction_key->at(input));
    }
    
	switch(input){
		case ',':
			command_pick_up();
			break;
		case 'i':
			command_inventory();
			break;
		case 'E':
			command_equipment();
			break;
		case 'C':
			command_conditions();
			break;
		case 'w':
			command_equip();
			break;
		case 'u':
			command_unequip();
			break;
		case 'd':
			command_drop();
			break;
		case 'q':
			command_drink();
			break;
		case 'r':
			command_read();
			break;
		case 'a':
			command_use();
			break;
	    case 'Q':
		    command_quit();
		    break;
		case 's':
		case '.':
        
		default:
            if (action_key->count(input) != 0) {
                act_player->execute_action(action_key->at(input));
            }
			break;
	}

}

int UI::get_input(){

	int input = wgetch(stdscr);
	
	return input;

}

// COMMANDS ==============================================

bool UI::command_inventory(){
	win_screen->display_inventory(*act_player);
	redraw_windows();
}

bool UI::command_equipment(){
	win_screen->display_equipment(*act_player);
	redraw_windows();
}

bool UI::command_conditions(){
	win_screen->display_conditions(act_player);
	redraw_windows();
}

bool UI::command_move(direction_t dir){

	std::pair<int, int> offset = dir_to_offset(dir);
	
	ui_action action = UIA_NONE;
	
	tile * dest = &map_current->tiles[act_player->x + offset.first][act_player->y + offset.second];
	
	if(dest->my_actor == NULL) {
		if(act_player->can_travel(dest)) {
			action = UIA_MOVE;
        }
	} else
		action = UIA_ATTACK;
	
	if(action == UIA_MOVE) {
		act_player->move(offset);
	} else if(action == UIA_ATTACK) {
		act_player->attack(offset);
    }
		
	return true;	
}

/*
	Pick one or more objects up from the ground.
	Multiple pickup broken by: weight, slots.
*/
bool UI::command_pick_up(){

	tile * current = &map_current->tiles[act_player->x][act_player->y];
	bool ok = true;
	
	if(current->my_objects->empty()){
	
		// If no objects
		win_output->print("There is nothing here to take.");
	} else {
		if(current->my_objects->size() == 1){

			// Take a single object		
			string error;

			object * target = current->my_objects->back();
			error = command_pick_up_helper(current->my_objects->back());
			if( error == ""){
				act_player->pick_up(target, current);
				win_output->print("You take " + target->get_name_color() + ".");
			} else {
				win_output->print(error);
			}
		} else {
			
			// Take multiple objects
			vector<object*> * selected = win_screen->menu_select_objects("Pick up what?", current->my_objects, -1, true);
			string error = "";
			string out_string = "";
			int taken = 0;
			
			while(error == "" && !selected->empty()){
			
				error = command_pick_up_helper(selected->back());
				if(error == "") {
				
					act_player->pick_up(selected->back(), current);
					
					if(taken++ > 0) out_string += ", ";
					out_string += selected->back()->get_name_color();
					selected->pop_back();
				}
			}
			
			if(taken == 0) win_output->print(error);
			else if(taken == 1) win_output->print("You take " + out_string);
			else if(taken > 1) win_output->print("You take: " + out_string);
			
			redraw_windows();
			ok = false;
		}
	}
	
	return ok;
}

/*
	Checks whether you are able to pick up a particular item. Returns appropriate
	error messages.
*/
string UI::command_pick_up_helper(object * target){

	int cond = 1;
	if(act_player->inventory->size() >= MAX_INVENTORY){
		cond = -1;
	}
	
	switch(cond){
		case 1:
			return "";
		case -1:
			return "You cannot carry any more items.";
		default:
			return "SOMETHING WENT WRONG!";
	}

}

bool UI::command_drop(){

	tile * current = &map_current->tiles[act_player->x][act_player->y];
	
	if( !(act_player->inventory->empty() && act_player->gold == 0) ){
	
        argmap * args = new argmap();
        args->add_int(ARG_TARGET_NUMBER, -1);
        args->add_int(ARG_TARGET_GOLDOK, 1);
        vector<requirement*> * reqs = new vector<requirement*>();
		vector<object*> * items = prompt_inventory("Drop what?", args, reqs);
        
		if(items->size() > 0){
		
			for(int i = 0; i < items->size(); ++i){
				act_player->drop(items->at(i), current);
			}
			
			if(items->size() == 1){
				win_output->print("You drop the " + items->back()->get_name_color() + ".");
			} else {
				win_output->print("You drop " + int_string(items->size()) + " objects.");
			}
		} else win_output->print("Nevermind");
		
	} else win_output->print("You have nothing to drop.");
}

bool UI::command_equip(){
	
    argmap * args = new argmap();
    args->add_int(ARG_TARGET_NUMBER, 1);
    args->add_int(ARG_TARGET_GOLDOK, 0);
    vector<requirement*> * reqs = new vector<requirement*>();
      
	object * item = get_single( prompt_inventory("Equip what?", args, reqs) );
	
	if(item != NULL){
	
		int slot = type_to_slot(item->get_type(), item->get_subtype());
		
		if(slot != -1){
		
			if(slot == ES_RING1 && act_player->equipped_item[ES_RING1] != NULL){
				slot = ES_RING2;
			}
			
			if(act_player->equipped_item[slot] != NULL){
				bool temp = prompt_yesno("Replace " + act_player->equipped_item[slot]->get_name() + "?");
				if(temp){
					act_player->unequip(item);
					win_output->print("Unequipped " + item->get_name());
				} else {
					win_output->print("Nevermind");
					return false;
				}
			}
			
			act_player->equip(item, slot);
			win_output->print("Equipped " + item->get_name());
		}
	}
}

bool UI::command_unequip(){

    argmap * args = new argmap();
    args->add_int(ARG_TARGET_NUMBER, 1);
    args->add_int(ARG_TARGET_GOLDOK, 0);
    vector<requirement*> * reqs = new vector<requirement*>();
    
	object * item = get_single(prompt_inventory("Unequip what?", args, reqs) );
	
	if(item != NULL){
		
		if(item->equipped){
			act_player->unequip(item);
			win_output->print("Unequipped " + item->get_name());
			return true;
		} else {
			win_output->print("That item is not equipped.");
			return false;
		}
	} else {
		win_output->print("Nevermind...");
	}
}

bool UI::command_eat() {

    argmap * args = new argmap();
    args->add_int(ARG_TARGET_NUMBER, 1);
    args->add_int(ARG_TARGET_GOLDOK, 0);
    vector<requirement*> * reqs = new vector<requirement*>();
	
    object * item = get_single( prompt_inventory("Eat what?", args, reqs));
	if(item != NULL){

		act_player->eat(item);
	} else {
		win_output->print("Nevermind...");
	}
}

bool UI::command_drink(){
	
    argmap * args = new argmap();
    args->add_int(ARG_TARGET_NUMBER, 1);
    args->add_int(ARG_TARGET_GOLDOK, 0);
    vector<requirement*> * reqs = new vector<requirement*>();
	object * item = get_single( prompt_inventory("Drink what?", args, reqs));
	
	if(item != NULL){
		
		act_player->drink(item);
	} else {
		win_output->print("Nevermind...");
	}
}

bool UI::command_read(){
	
    argmap * args = new argmap();
    args->add_int(ARG_TARGET_NUMBER, 1);
    args->add_int(ARG_TARGET_GOLDOK, 0);
    vector<requirement*> * reqs = new vector<requirement*>();
	object * item = get_single( prompt_inventory("Read what?", args, reqs) );
	
	if(item != NULL){
		
		act_player->read(item);
	} else {
		win_output->print("Nevermind...");
	}
}

bool UI::command_use(){
	
    argmap * args = new argmap();
    args->add_int(ARG_TARGET_NUMBER, 1);
    args->add_int(ARG_TARGET_GOLDOK, 0);
    vector<requirement*> * reqs = new vector<requirement*>();
	object * item = get_single( prompt_inventory("Use what?", args, reqs) );
	
	if(item != NULL){
		
		act_player->use(item);
	} else {
		win_output->print("Nevermind...");
	}
}

bool UI::command_quit() {
  exit_game(0);
}

// Interface =========================================

// Handle all prompts related to targeting for actions.
vector<void*> * UI::prompt_target(targetActionBlock * in) {

    vector<void*> * r;
    switch (in->target_type) {
    
        case TAR_SELF:
            r = new vector<void*>();
            r->push_back((void*)prompt_self(in->requirements));
            break;
        case TAR_ADJ:
            break;
        case TAR_INV:
            r = (vector<void*>*)prompt_inventory(in->prompt, in->args, in->requirements);
            break;
        case TAR_NONE:
        default:
            break;
    
    }

    return r;
}

// Get the currently controlled player
actor * UI::prompt_self(vector<requirement*> * reqs) {

    actor * selected = act_player;
    
    if (!requirement::check_requirements_for(act_player, reqs)) {
        selected = NULL;
    }
    
    return selected;
}

// Prompt the user for a letter to select an item. $ creates a gold object, * or ? opens
// inventory for viewing (and multi-select, for now)
vector<object*> * UI::prompt_inventory(string prompt, argmap * args, vector<requirement*> * reqs){

	int input;
	vector<object*> * ret = new vector<object*>();
	
	win_output->print(prompt);
			
    bool gold_ok = (args->get_int(ARG_TARGET_GOLDOK) == 1);
    int max_selects = args->get_int(ARG_TARGET_NUMBER);
    
	while(true){
		input = wgetch(stdscr);
		
		if(input == '$' && gold_ok){
            object * goldObj = prompt_gold_to_object();
            if (requirement::check_requirements_for(goldObj, reqs)) {
                ret->push_back(goldObj);
            }
			return ret;
		} else if(input == '*' || input == '?'){
			ret = win_screen->menu_select_objects(prompt, act_player->inventory, max_selects, true);
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

// Prompt for a yes/no question
bool UI::prompt_yesno(string prompt){

	win_output->print(prompt + "(y/n)");
	
	char r = ' ';
	
	while(r != 'y' && r != 'n'){
		r = wgetch(stdscr);
	}
	
	return r == 'y';
}

object * UI::prompt_gold_to_object(){

	int amount;
	
	win_output->print("How much? (have ?)");
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
    
    int input = 0;
    
    while (input == 0) {
        input = wgetch(stdscr);
        
        if (direction_key->count(input) != 0) {
            return direction_key->at(input);
        }
    }
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
	for(i = 0; obj_letter[i] != NULL && i < 52; ++i);
	
	if(i == 52) return 0;
	
	return int_to_letter(i);
}

int UI::type_to_slot(int type, int subtype){
	if(type == OT_WEAPON){
		return ES_MAINHAND;
	} else if(type == OT_ARMOR){
		switch(subtype){
			case OST_HAT:
			case OST_HELM:
				return ES_HEAD;
			case OST_SUIT:
				return ES_BODY;
			case OST_CLOAK:
				return ES_BACK;
			case OST_GLOVES:
				return ES_HANDS;
			case OST_BOOTS:
				return ES_FEET;
			default:
				return -1;
		}
	} else if(type == OT_ACCESSORY){
		switch(subtype){
			case OST_RING:
				return ES_RING1;
			case OST_AMULET:
				return ES_AMULET;
			default:
				return -1;
		}
	} else if(type == OT_TOOL){
		switch(subtype){
			case OST_LAMP:
				return ES_LIGHT;
			default:
				return -1;
		}
	} else
		return -1;
}
