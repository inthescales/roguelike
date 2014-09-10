#include "actor.h"
#include "condition.h"
#include "condclass.h"
#include "config.h"
#include "display.h"
#include "globals.h"
#include "map.h"
#include "object.h"
#include "objclass.h"
#include "stringutils.h"
#include "ui.h"
#include "tile.h"
#include "window.h"

#include LIB_CURSES

#include <algorithm>

//using ;

window::window(int n_x, int n_y, int n_w, int n_h) : x(n_x), y(n_y), width(n_w), height(n_h) {

}

// Clear the window
void window::clear(){

	for(int j = y; j < height + y; ++j){
		move(j, x);
		for(int i = x; i < width + x; i++){
		    printchar_cw(' ');
		}
	}
}

// Display the player's map
void window::display_map(map * m){

	for(int i = 0; i < height; ++i){
	
		move(i + y, x);
		for(int j = 0; j < width; ++j){
		
			tile * cur = &m->tiles[j + scrn_x][i + scrn_y];
			printchar_cw(cur->get_img(), cur->get_color());
		}
	}
}

// Display player's status
void window::display_status(){

	clear();
	move(y, x);
	printw("NAME");
	move(y + 1, x);
	printw("Level %d CLASS", act_player->level);
	move(y + 2, x);
	printw("HP: %d / %d", act_player->HP, act_player->maxHP);
	move(y + 3, x);
	printw("$%d", act_player->gold);
}

// Display actor's inventory
void window::display_inventory(actor & act){

	curs_set(0);
	clear();
	
	int index = 0, tx = x + 3, ty = y + 3;
	int start = 0, winsize = 10;
	int input, headers;
	vector<object*> items = act_player->inventory;
	
	while(true){
	
		clear();
		move(ty++, tx);
		printw("Inventory:");
		
		headers = 0;
		for(int i = start; i - start < winsize && i < items.size(); ++i){
		
			if(i == start || oclass[items[i]->type]->type != oclass[items[i-1]->type]->type){
				//We need to print a header
				printcolor(tx, ty + i + headers++ - start, color_string(str_obj_type[oclass[items[i]->type]->type], C_YELLOW));
			}
			move(ty + i + headers - start, tx);
			printw("%c - %s", items[i]->letter, items[i]->get_name().c_str());
			if(items[i]->equipped == true)
				printw(" (equipped)");
		}
	
		input = wgetch(stdscr);
		
		switch(input){
			case KEY_UP:
				if(start > 0)
					start -= 1;
				break;
			case KEY_DOWN:
				if(start + winsize < items.size())
					start += 1;
				break;
			default:
				return;
		}
	}	
	
	curs_set(1);
}

void window::display_equipment(actor & act){

	curs_set(0);
	clear();
	
	int tx = x + 3, ty = y + 3;
	
	move(ty++, tx);
	printw("Equipped items:");
	
	for(int i = 0; i < ES_MAX; ++i){
		
		string name = act.equipped_item[i] != NULL ? act.equipped_item[i]->get_name() : "(empty)";
		printcolor(tx, ty + i, color_string(str_eq_slot[i] + ": ", C_YELLOW) );
		printcolor(tx + 10, ty + i, name);
	}
	
	getch();
	
	curs_set(1);
}

void window::display_conditions(actor * act){

	curs_set(0);
	clear();
	
	int i = 0;
	int tx = x + 3, ty = y + 3;
	
	move(ty++, tx);
	printw("Active conditions:");
	
	vector<condition *>::iterator it = act->conditions.begin();
	
	for(; it != act->conditions.end(); ++it){
		
		string name = cclass[(*it)->type]->name;
		
		char buff[10];
		sprintf(buff, "%-3d x%-3d ", (*it)->duration, (*it)->stack);
		string details = buff;
		
		printcolor(tx, ty + i, details + color_string(cclass[(*it)->type]->name, cclass[(*it)->type]->color) );
		++i;
	}
	
	getch();
	
	curs_set(1);
}

// Print a string
void window::print(string text){
	print(text, *buf_main);
}

// Add a string to the buffer and print it
void window::print(string text, buffer & buf){

	clear();
	move(0, 0);
	buf.push_back(text);
	print_buf(buf);
}

// Print as much of the buffer as will fit in this window
void window::print_buf(buffer & buf){

	int screen_pointer = 0, buffer_pointer = 0;
	
	while(buffer_pointer < buf.size()){
		
		string next = buf[buf.size() - buffer_pointer++ - 1];
		vector<string> cut = string_slice(next, width);
		
		for(int i = cut.size() - 1; i >= 0 && screen_pointer < height; --i){
			print_line(cut[i], height - screen_pointer++ - 1);
		}
	}
}

void window::print_line(string in, int pos){

	printcolor(x, y + pos, in);
}

// MENUS (interactive) ============================================

vector<object*> window::menu_select_objects(vector<object*> & items, bool multi, bool sort){
	curs_set(0);
	
	int index = 0, x = 3, y = 3;
	int start = 0, winsize = 10;
	const d_glyph sym[] = {'-', '+'};
	int input, headers;
	vector<bool> selected(items.size(), false);
	vector<object*> ret;
	
	if(sort){
		std::sort(items.begin(), items.end(), object::compare_type);
	}
	
	while(true){
	
		clear();
		move(y, x);
		printw("Select some items:");
		
		headers = 0;
		for(int i = start; i - start < winsize && i < items.size(); ++i){
		
			if(i == start || oclass[items[i]->type]->type != oclass[items[i-1]->type]->type){
				//We need to print a header
				printcolor(x, y + i + headers++ - start + 1, color_string(str_obj_type[items[i]->type], C_YELLOW));
			}
			move(y + i + headers - start + 1, x);
			printw("%c ", UI::int_to_letter(i));
			printchar_cw(sym[selected[i]]);
			printw(" %s", items[i]->get_name().c_str());
		}
		
		input = wgetch(stdscr);
		
		if((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')){
		
			selected[UI::letter_to_int(input)] = !selected[UI::letter_to_int(input)];				
		} else {
		
			switch(input){
				case 10:			
					for(int j = 0; j < items.size(); ++j)
						if(selected[j])
							ret.push_back(items[j]);
					return ret;
				case 27:
					return ret;
				case KEY_UP:
					if(start > 0)
						start -= 1;
					break;
				case KEY_DOWN:
					if(start + winsize < items.size())
						start += 1;
					break;
				default:
					break;
			}
		}
		
	}
		
	curs_set(1);
}
