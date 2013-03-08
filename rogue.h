#ifndef ROGUE_H

#define ROGUE_H

#include "actor.h"
#include "globals.h"
#include "window.h"

void init_game();
void exit_game(int sig);

// some globals:

window * win_output;
window * win_world;
window * win_status;

buffer * buf_main;

map * map_current = 0;
actor * act_player = 0;

short scrn_x = 0, scrn_y = 0;

int turn = 1;

#endif
