#include "actor.h"
#include "condition.h"
#include "condclass.h"
#include "config.h"
#include "display.h"
#include "globals.h"
#include "interface.h"
#include "map.h"
#include "object.h"
#include "objclass.h"
#include "stringutils.h"
#include "symboldefs.h"
#include "tile.h"
#include "window.h"

#include LIB_CURSES

#include <algorithm>
#include <stack>

using std::stack;

//using ;

window::window(int n_x, int n_y, int n_w, int n_h) : x(n_x), y(n_y), width(n_w), height(n_h) {

    should_update = false;
}

// Window display =============================================

// Clear the window
void window::clear(){

	for(int j = y; j < height + y; ++j){
		move(j, x);
		for(int i = x; i < width + x; i++){
		    printchar_cw(' ');
		}
	}
}

// Display a map
void window::display_map(map * m){

	for(int i = 0; i < height; ++i){
	
		move(i + y, x);
		for(int j = 0; j < width; ++j){
		
            if (i + scrn_y < m->height && j + scrn_x < m->width) {
                tile * cur = &(m->tiles[j + scrn_x][i + scrn_y]);
                printglyph(cur->get_display_glyph());
            }
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
void window::display_inventory(actor & act, string prompt){

	curs_set(0);
	clear();
	
	int index = 0, tx = x + 3, ty = y + 3;
	int start = 0, winsize = 10;
	int input, headers;
	vector<object*> * items = act_player->inventory;
	
	while(true){
	
		clear();
		move(ty++, tx);
		printcolor(tx, ty, prompt);
		
		headers = 1;
		for(int i = start; i - start < winsize && i < items->size(); ++i){
		
			if(i == start || items->at(i)->get_type() != items->at(i-1)->get_type()){
				//We need to print a header
				printcolor(tx, ty + i + headers++ - start, color_string(str_obj_type[items->at(i)->get_type()], C_YELLOW));
			}
			move(ty + i + headers - start, tx);
			printw("%c - %s", items->at(i)->letter, items->at(i)->get_name().c_str());
			if(items->at(i)->equipped == true)
				printw(" (equipped)");
		}
	
		input = wgetch(stdscr);
		
		switch(input){
			case KEY_UP:
				if(start > 0)
					start -= 1;
				break;
			case KEY_DOWN:
				if(start + winsize < items->size())
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
	
	vector<condition *>::iterator it = act->condition_list->begin();
	
	for(; it != act->condition_list->end(); ++it){
		
		string name = (*it)->get_name_color();
		
		char buff[10];
		sprintf(buff, "x%-3d ", (*it)->stack);
		string details = buff;
		
		printcolor(tx, ty + i, details + name);
		++i;
	}
	
	getch();
	
	curs_set(1);
}

void window::display_all() {
	win_world->display_map(map_current);
    win_status->display_status();
    win_output->clear();
    win_output->print_buf(buf_main);
}

// Text output ================================================

// Add a string to the main buffer and print it
void window::print(string text){
	print(text, buf_main);
}

void break_buffer(buffer * buf) {
    if (buf->size() > 0 && buf->back() != "") {
        buf->push_back("");
    }
}

// Add a string to the buffer and print it
void window::print(string text, buffer * buf){

	clear();
	move(0, 0); // TODO - make text window position more configurable
    if (buf->size() > 0) {
        string temp = buf->back();
        if (temp != "") {
            temp = temp + " ";
        }
        buf->pop_back();
        buf->push_back(temp + text);
    } else {
        buf->push_back(text);
    }

    should_update = true;
}

// Print as much of the buffer as will fit in this window
void window::print_buf(buffer * buf){

	int screen_pointer = 0, buffer_pointer = 0;
    string test = "";
	
	while(buffer_pointer < buf->size()){
		
		string next = buf->at(buf->size() - buffer_pointer++ - 1);
        if (next == "") continue;
        
		vector<string> cut = string_slice(next, width);
		
        string in;
             
		for(int i = cut.size() - 1; i >= 0 && screen_pointer < height; --i){

            in = cut[i];
            test += in;
            stack<colorName> colorStack;
            colorStack.push(C_WHITE);
            move(y + height - screen_pointer++ - 1, x);
            for(int j = 0; j < in.size(); ++j){
            
                if(in.at(j) == color_escape_start){ 
                    colorStack.push((colorName)(in.at(++j)));
                } else if (in.at(j) == color_escape_end) {
                    colorStack.pop();
                } else {
                    printchar_cw(in.at(j), colorStack.top());
                }
            }
		}
	}
}

void window::print_line(string in, int pos){

	printcolor(x, y + pos, in);
}

void window::print_error(string text) {

    clear();
    print_line("ERROR -- " + text, 0);
    getch();
}
