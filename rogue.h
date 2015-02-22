#ifndef ROGUE_H

#define ROGUE_H

#include "display.h"
#include "window.h"
#include "classdefs.h"

#include <set>

using std::set;

class tile;

#define INV_MAX 52

// Extern values:

// Display objects
extern window * win_output;
extern window * win_world;
extern window * win_status;
extern window * win_screen;
extern buffer * buf_main;

// Window and screen properties
extern short term_w, term_h; // Terminal size
extern short scroll_border;

// Current map and player
extern map * map_current;
extern actor * act_player;

// Current player interface details
extern set<tile*> * cur_seen_tiles;
extern set<tile*> * last_seen_tiles;
extern object * obj_letter[];

// Functions relevant to rogue.cpp
void init_game();
void game_loop();
void redraw_windows();
void exit_game(int sig);

#endif
