#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#include "rogue.h"
#include "setup.h"
#include "ui.h"
#include "window.h"

// Main! Setup and action loop
int main(void){

	setup_window();
	setup();
	init_game();

	while(1)
	{
		
		curs_set(1);
		win_world->display_map(map_current);
		win_status->display_status();
		win_output->print_buf(*buf_main);
		
		move(act_player->y - scrn_y + win_world->y, act_player->x - scrn_x + win_world->x);

		UI::get_action();
		//run_ai( map_current );
		
		++turn;
	}
	
	exit_game(0);

}

// Initialize the starting game state
void init_game() {

	turn = 0;
	map_current = new map(60, 32, 2, 1, NULL);
	
	act_player = new actor(0);
	map_current->put_actor( 18, 18, act_player);
	map_current->put_actor( 20, 20, new actor(1));
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 10, 10, 1 );
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 10, 10, 1 );
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 10, 10, 1 );
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 10, 10, 1 );
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 10, 10, 1 );
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 10, 10, 1 );
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 10, 10, 1 );
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 10, 10, 1 );
	
	win_world->display_map(map_current);
	win_status->display_status();
	
	win_output->print("Welcome to the game!");
	
	refresh();
	//point_curs(act_player);
}

// Redraw the main gameplay screen
void redraw_windows() {

	win_output->print_buf(*buf_main);
	win_world->display_map(map_current);
	win_status->display_status();
}

// Shut everything down and exit
void exit_game(int sig){

	endwin();
	exit(0);
}
