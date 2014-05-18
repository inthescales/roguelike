#include <stdio.h>

#include "actor.h"
#include "globals.h"
#include "map.h"
#include "objclass.h"
#include "stringutils.h"
#include "tile.h"
#include "ui.h"
#include "window.h"

#include <sstream>

using std::ostringstream;

void UI::get_action(){

	int input = get_input();
	
	switch(input){
		case '0' + 1:
		case '0' + 2:
		case '0' + 3:
		case '0' + 4:
		case '0' + 6:
		case '0' + 7:
		case '0' + 8:
		case '0' + 9:
			command_move(input - '0');
			break;
		case ',':
			command_pick_up();
			break;
		case 'i':
			command_inventory();
			break;
		case 'E':
			command_equipment();
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
		case 'e':
			command_eat();
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
		case 's':
		case '.':
		default:
			break;
	}

}

int UI::get_input(){

	int input = getch();
	
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

bool UI::command_move(int dir){
	std::pair<int, int> offset = dir_to_offset(dir);
	actor * controlled = act_player;
	
	ui_action action = UIA_NONE;
	
	tile dest = map_current->tiles[controlled->x + offset.first][controlled->y + offset.second];
	
	if(dest.my_actor == NULL) {
		if(dest.can_walk())
			action = UIA_MOVE;
	} else
		action = UIA_ATTACK;
	
	if(action == UIA_MOVE)
		controlled->move(offset);
	else if(action == UIA_ATTACK)
		controlled->attack(offset);
		
	return true;	
}

/*
	Pick one or more objects up from the ground.
	Multiple pickup broken by: weight, slots.
*/
bool UI::command_pick_up(){

	actor * controlled = act_player;
	tile * current = &map_current->tiles[controlled->x][controlled->y];
	bool ok = true;
	
	if(current->my_objects.empty()){
	
		// If no objects
		win_output->print("There is nothing here to take.");
	} else {
		if(current->my_objects.size() == 1){

			// Take a single object		
			string error;

			object * target = current->my_objects.back();
			error = command_pick_up_helper(current->my_objects.back());
			if( error == ""){
				act_player->pick_up(target, current);
				win_output->print("You take " + target->get_name_color() + ".");
			} else {
				win_output->print(error);
			}
		} else {
			
			// Take multiple objects
			vector<object*> selected = win_screen->menu_select_objects(current->my_objects, true, true);
			string error = "";
			string out_string = "";
			int taken = 0;
			
			while(error == "" && !selected.empty()){
			
				error = command_pick_up_helper(selected.back());
				if(error == "") {
				
					act_player->pick_up(selected.back(), current);
					
					if(taken++ > 0) out_string += ", ";
					out_string += selected.back()->get_name_color();
					selected.pop_back();
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
	if(act_player->inventory.size() >= MAX_INVENTORY){
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

	actor * controlled = act_player;
	tile * current = &map_current->tiles[controlled->x][controlled->y];
	
	if( !(controlled->inventory.empty() && controlled->gold == 0) ){
	
		vector<object*> items = prompt_inventory(controlled, "Drop what?", false, true);
		if(items.size() > 0){
		
			for(int i = 0; i < items.size(); ++i){
				controlled->drop(items[i], current);
			}
			
			if(items.size() == 1){
				win_output->print("You drop the " + items.back()->get_name_color() + ".");
			} else {
				ostringstream convert;
				convert << items.size();
				win_output->print("You drop " + convert.str() + " objects.");
			}
		} else win_output->print("Nevermind");
		
	} else win_output->print("You have nothing to drop.");
}

bool UI::command_equip(){
	
	actor * controlled = act_player;
	object * item = get_single( prompt_inventory(controlled, "Equip what?", false, false) );
	
	if(item != NULL){
	
		int slot = type_to_slot(oclass[item->type]->type, oclass[item->type]->subtype);
		
		if(slot != -1){
		
			if(slot == ES_RING1 && controlled->equipped_item[ES_RING1] != NULL){
				slot = ES_RING2;
			}
			
			if(controlled->equipped_item[slot] != NULL){
				bool temp = prompt_yesno("Replace " + controlled->equipped_item[slot]->get_name() + "?");
				if(temp){
					controlled->unequip(item);
					win_output->print("Unequipped " + item->get_name());
				} else {
					win_output->print("Nevermind");
					return false;
				}
			}
			
			controlled->equip(item, slot);
			win_output->print("Equipped " + item->get_name());
		}
	}
}

bool UI::command_unequip(){

	actor * controlled = act_player;
	object * item = get_single( prompt_inventory(controlled, "Unequip what?", false, false) );
	
	if(item != NULL){
		
		if(item->equipped){
			controlled->unequip(item);
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

bool UI::command_eat(){
	
	actor * controlled = act_player;
	object * item = get_single( prompt_inventory(controlled, "Eat what?", false, false) );

	if(item != NULL){

		controlled->eat(item);
	} else {
		win_output->print("Nevermind...");
	}
}

bool UI::command_drink(){
	
	actor * controlled = act_player;
	object * item = get_single( prompt_inventory(controlled, "Drink what?", false, false) );
	
	if(item != NULL){
		
		controlled->drink(item);
	} else {
		win_output->print("Nevermind...");
	}
}

bool UI::command_read(){
	
	actor * controlled = act_player;
	object * item = get_single( prompt_inventory(controlled, "Read what?", false, false) );
	
	if(item != NULL){
		
		controlled->read(item);
	} else {
		win_output->print("Nevermind...");
	}
}

bool UI::command_use(){
	
	actor * controlled = act_player;
	object * item = get_single( prompt_inventory(controlled, "Use what?", false, false) );
	
	if(item != NULL){
		
		controlled->use(item);
	} else {
		win_output->print("Nevermind...");
	}
}

// PROMPTS =========================================

/*
	Prompt for a yes/no question
*/
bool UI::prompt_yesno(string prompt){

	win_output->print(prompt + "(y/n)");
	
	char r = ' ';
	
	while(r != 'y' && r != 'n'){
		r = getch();
	}
	
	return r == 'y';
}

/*
	Prompt the user for a letter to select an item. $ creates a gold object, * or ? opens
	inventory for viewing (and multi-select, for now)
*/
vector<object*> UI::prompt_inventory(actor * controlled, string prompt, bool allow_multi, bool allow_gold){

	int input;
	vector<object*> ret;
	
	win_output->print(prompt);
			
	while(true){
		input = wgetch(stdscr);
		
		if(input == '$' && allow_gold){
			ret.push_back(prompt_gold_to_object(controlled));
			return ret;
		} else if(input == '*' || input == '?'){
			ret = win_screen->menu_select_objects(controlled->inventory, true, true);
			return ret;
		} else if(input == 27){
			return ret;
		} else {
			int slot = letter_to_int(input);
			if(slot != -1 && obj_letter[slot] != NULL)
				ret.push_back(obj_letter[slot]);
			if(obj_letter[slot] == NULL)
				exit(0);
			return ret;
		}
	}
}

// Get an element from a size-one vector.
object * UI::get_single(vector<object*> vect){
	if(vect.size() == 1){
		return vect.back();
	} else {
		return NULL;
	}
}

object * UI::prompt_gold_to_object(actor * controlled){

	int amount;
	
	win_output->print("How much? (have ?)");
	scanf("%d", &amount);
	
	if(amount > controlled->gold){
		win_output->print("You don't have that much.");
		return NULL;
	} else {
		object * ret = new object(3, amount);
		controlled->gold -= amount;
		return ret;
	}
}

// UTILITIES =======================================

std::pair<int,int> UI::dir_to_offset(int dir){
	std::pair<int, int> r;

	if(dir % 3 == 1) r.first = -1;
	else if(dir % 3 == 2) r.first = 0;
	else r.first = 1;
	
	if(dir <= 3) r.second = 1;
	else if(dir <= 6) r.second = 0;
	else if(dir <= 9) r.second = -1;
	
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