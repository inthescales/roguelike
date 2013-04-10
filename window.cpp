#include "curses.h"

#include "globals.h"
#include "stringutils.h"
#include "window.h"

using namespace std;

window::window(int n_x, int n_y, int n_w, int n_h) : x(n_x), y(n_y), width(n_w), height(n_h) {

}

// Clear the window
void window::clear(){

	for(int j = y; j < height + y; ++j){
		move(j, x);
		for(int i = x; i < width + x; i++){
			addch(' ');
		}
	}
}

// Display the player's map
void window::display_map(map * m){

	for(int i = 0; i < height; ++i){
	
		move(i + y, x);
		for(int j = 0; j < width; ++j){
		
			tile * cur = &m->tiles[j + scrn_x][i + scrn_y];
			addch( cur->get_img() );
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
	
	vector<object*>::iterator it = act.inventory.begin();
	
	int i = 3, j = 3;
	string s;
	
	for(; it != act.inventory.end(); ++it){
	
		move(y + j++, x + i);
		s = (*it)->get_name();
		printw("%s", s.c_str());
	}
	
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
	int len = in.size();
	int color = C_WHITE;

	move(y + pos, x);
	for(int i = 0; i < len; ++i){
	
		if(in.at(i) == '|'){ 
			color = in.at(i+1);
			++i;
		} else {
			addch(in.at(i) | color_value[color]);
		}
	}
}
