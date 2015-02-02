#include "actor.h"
#include "config.h"
#include "interface.h"
#include "map.h"
#include "rogue.h"
#include "setup.h"
#include "window.h"

#include LIB_CURSES

#include <stdio.h>
#include <stdlib.h>
#include <clocale>

// Setup and action loop
int main(void){
  
	setup_window();
	setup();
	init_game();
    
	game_loop();
	
	exit_game(0);
}

void game_loop()
{
    while(1)
	{
		curs_set(1);
		win_world->display_map(map_current);
		win_status->display_status();
        if (win_output->should_update) {
            win_output->clear();
            win_output->print_buf(buf_main);
            win_output->should_update = false;
        }
		break_buffer(buf_main);
        
		move(act_player->y - scrn_y + win_world->y, act_player->x - scrn_x + win_world->x);    
        
		map_current->advance_time();
		
		++turn;
	}
}

// Initialize the starting game state
void init_game() {

	turn = 0;
	map_current = new map(60, 32, 1, NULL);
	act_player = new actor(0);
    
	map_current->put_actor( 18, 18, act_player);
    act_player->add_condition(new condition(act_player, COND_CURSED));
	//map_current->add_actor( 20, 20, 1);
    
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 11, 10, 1 );
	map_current->add_object( 12, 10, 2 );
	map_current->add_object( 13, 10, 3 );
    map_current->add_object( 14, 10, 4 );
    
    map_current->add_feature( 12, 20, 0);
	
	win_world->display_map(map_current);
	win_status->display_status();	
	win_output->print("You enter another dungeon...");
    
	refresh();
}

// Redraw the main gameplay screen
void redraw_windows()
{
	win_output->print_buf(buf_main);
	win_world->display_map(map_current);
	win_status->display_status();
}

// Shut everything down and exit
void exit_game(int sig)
{
	endwin();
	exit(sig);
}
