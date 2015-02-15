#ifndef ROGUE_H

#define ROGUE_H

#include "display.h"
#include "globals.h"
#include "window.h"
#include "classdefs.h"

void init_game();
void game_loop();

// some globals:

actclass * aclass[ACT_MAX];
objclass * oclass[OBJ_MAX];
featclass * fclass[FEAT_MAX];
tileclass * tclass[TILE_MAX];
condclass * cclass[COND_MAX];

action * actiondef[ACTION_MAX];
symbol_code symboldef[CHAR_MAX];

object * obj_letter[INV_MAX];

window * win_output;
window * win_world;
window * win_status;
window * win_screen;

buffer * buf_main;

map * map_current = 0;
actor * act_player = 0;

short scrn_x = 0, scrn_y = 0;

int turn = 1;

#endif
