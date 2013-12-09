#ifndef GLOBALS_H

#define GLOBALS_H

#include <string>
#include <vector>
using std::string;
using std::vector;

typedef vector<string> buffer;

#include "classdefs.h"
class object;
class window;
class map;
class actor;
class actclass;
class objclass;

// SYSTEM STUFF ========================

//#define 32_BIT
//#define 64_BIT

// =====================================

#define MAX_INVENTORY 48

/*
int map_id = 1;
object *letter[53];
*/

// Inventory letters
extern object * obj_letter[];

// Windows
extern window * win_output;
extern window * win_world;
extern window * win_status;
extern window * win_screen;

extern buffer * buf_main;

// Current map and player
extern map * map_current;
extern actor * act_player;

// Player's item letter assignments
//extern object * letter[];

//Position of game window on map
extern short scrn_x, scrn_y;
extern short term_w, term_h;

// Global counters
extern int map_id;
extern int turn;

// Entity class arrays
extern actclass * aclass[ACT_MAX];
extern objclass * oclass[OBJ_MAX];
extern struct tileclass tclass[];

// Other predefined arrays
extern string str_obj_type[];
extern string str_eq_slot[];

// Color setup
extern int color_value[];

// Functions of general use
void redraw_windows();

#endif
