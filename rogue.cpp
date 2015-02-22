#include "actor.h"
#include "config.h"
#include "interface.h"
#include "map.h"
#include "rogue.h"
#include "setup.h"
#include "vision.h"
#include "window.h"
#include "world.h"

#include LIB_CURSES

#include <stdio.h>
#include <stdlib.h>
#include <clocale>

// Display objects
window * win_output = NULL;
window * win_world = NULL;
window * win_status = NULL;
window * win_screen = NULL;
buffer * buf_main = NULL;

// Window and screen properties
short term_w = 0, term_h = 0; // Terminal size
short scroll_border = 5;

// Current map and player
map * map_current = NULL;
actor * act_player = NULL;

// Current player interface details
set<tile*> * cur_seen_tiles = NULL;
set<tile*> * last_seen_tiles = NULL;
object * obj_letter[INV_MAX];

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
        vision::run_player_vision();
        window::display_all();
		break_buffer(buf_main);
        
		move(act_player->y + win_world->y - win_world->frame_y, act_player->x + win_world->x - win_world->frame_x);    
        
		map_current->advance_time();
	}
}

// Initialize the starting game state
void init_game() {

	map_current = new map(75, 47, 1, NULL);
    world::pond(map_current, 30, 12, 10);
    world::forest(map_current, 25, 7, 20);
    
    // player setup
	act_player = new actor(ACT_HUMAN);
    map_current->put_actor( 18, 18, act_player);
    
    act_player->add_condition(new condition(act_player, COND_CURSED));
    map_current->add_actor(14, 18, ACT_ROBOT);
    
	map_current->add_object( 10, 10, 0 );
	map_current->add_object( 11, 10, 1 );
	map_current->add_object( 12, 10, 2 );
	map_current->add_object( 13, 10, 3 );
    map_current->add_object( 14, 10, 4 );
    
    map_current->add_feature( 12, 20, 0);
	
	win_world->display_map(map_current, true);
	win_status->display_status();	
	win_output->print("You enter another dungeon...");
    
	refresh();
}

// Redraw the main gameplay screen
void redraw_windows()
{
    
    if (win_output->should_update) {
        win_output->clear();
        win_output->print_buf(buf_main);
        win_output->should_update = false;
    }
    win_world->display_map(map_current, win_world->should_update);
	win_status->display_status();
}

// Shut everything down and exit
void exit_game(int sig)
{
	endwin();
	exit(sig);
}
