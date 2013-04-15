#include <stdio.h>

#include "actor.h"
#include "globals.h"
#include "map.h"
#include "stringutils.h"
#include "tile.h"
#include "ui.h"

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

bool UI::command_inventory(){
	win_screen->display_inventory(*act_player);
	getch();
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

bool UI::command_pick_up(){
	actor * controlled = act_player;
	
	tile * current = &map_current->tiles[controlled->x][controlled->y];
	bool ok = true;
	
	if(current->my_objects.empty()){
	
		win_output->print("There is nothing here to take.");
	} else {
		if(current->my_objects.size() == 1){
		
			object * target = current->my_objects.back();
			bool ok = command_pick_up_helper(current->my_objects.back());
			if(ok) act_player->pick_up(target, current);
		} else {
			
			vector<object*> selected = win_screen->menu_select_objects(current->my_objects);
			bool going = true;
			while(going && !selected.empty()){
				going = command_pick_up_helper(selected.back());
				if(going) act_player->pick_up(selected.back(), current);
				selected.pop_back();
			}
			redraw_windows();
			ok = false;
		}
	}
	
	return ok;
}

bool UI::command_pick_up_helper(object * target){

	int cond = 1;
	if(act_player->inventory.size() >= MAX_INVENTORY){
		cond = -1;
	}
	
	switch(cond){
		case 1:
			win_output->print("You take the " + color_string(target->get_name(), oclass[target->type].color) + ".");
			return true;
		case -1:
			win_output->print("You cannot carry any more items.");
			return false;
		default:
			return false;
	}

}

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
	if(in < 'z' - 'a')
		return 'a' + in;
	else
		return 'A' + (in - ('z' -'a'));
}

int UI::letter_to_int(char in){
	if(in >= 'a' && in <= 'z')
		return in - 'a';
	else if(in >= 'A' && in <= 'Z')
		return in - 'A' + ('z' - 'a');
}
