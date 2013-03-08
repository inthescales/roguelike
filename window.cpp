#include "globals.h"
#include "window.h"

using namespace std;

window::window(int n_x, int n_y, int n_w, int n_h) : x(n_x), y(n_y), width(n_w), height(n_h) {

}

void window::clear(){

	for(int j = y; j < height + y; ++j){
		move(j, x);
		for(int i = x; i < width + x; i++){
			addch(' ');
		}
	}
}

void window::display_map(map * m){

	for(int i = 0; i < height; ++i){
	
		move(i + y, x);
		for(int j = 0; j < width; ++j){
		
			tile * cur = &m->tiles[j + scrn_x][i + scrn_y];
			addch( cur->get_img() );
		}
	}
}

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

void window::print(string text){
	print(text, buf_main);
}

void window::print(string text, buffer * buf){

	clear();
	move(0, 0);
	

}
