#ifndef GLOBALS_H

#define GLOBALS_H

#include "classdefs.h"
#include "display.h" // need symbol code size
#include "symboldefs.h"

#include <set>
#include <string>
#include <vector>

using std::set;
using std::string;
using std::vector;

typedef vector<string> buffer;

class actor;
class object;
class tile;
class action;
class actclass;
class objclass;
class featclass;
class tileclass;
class condclass;
class map;
class window;

// SYSTEM STUFF ========================

//#define 32_BIT
//#define 64_BIT

// =====================================

#define INV_MAX 52

/*
int map_id = 1;
object *letter[53];
*/

// Windows
extern window * win_output;
extern window * win_world;
extern window * win_status;
extern window * win_screen;

extern buffer * buf_main;

// Current map and player
extern map * map_current;
extern actor * act_player;

// Inventory letters
extern object * obj_letter[];
extern set<tile*> * cur_seen_tiles;
extern set<tile*> * last_seen_tiles;

//Position of game window on map
extern short scrn_x, scrn_y;
extern short term_w, term_h;

// Global counters
extern int map_id;
extern int turn;

// Entity class arrays
extern actclass * aclass[ACT_MAX];
extern objclass * oclass[OBJ_MAX];
extern featclass * fclass[FEAT_MAX];
extern tileclass * tclass[TILE_MAX];
extern condclass * cclass[COND_MAX];

extern action * actiondef[ACTION_MAX];
extern symbol_code symboldef[CHAR_MAX];

// Other predefined arrays
extern string str_obj_type[];
extern string str_eq_slot[];

// Color setup
extern int color_value[];

// Functions of general use
void exit_game(int sig);
void redraw_windows();

#endif
