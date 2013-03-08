#include <stdio.h>

#include "actor.h"
#include "globals.h"
#include "map.h"
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

