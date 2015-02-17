#ifndef CONFIG_H
#define CONFIG_H

// INTEGER SIZE ====================================
// for void / int casts in argmap
//# define size_32
#define size_64

// DISPLAY =========================================
#define DISPLAY_CURSES       0
#define DISPLAY_WCURSES      1
#define DISPLAY_TILES        2

#define DISPLAY_TYPE DISPLAY_CURSES

#if DISPLAY_TYPE == DISPLAY_CURSES

    #define LIB_CURSES <curses.h>
#elif DISPLAY_TYPE == DISPLAY_WCURSES

    #define LIB_CURSES <ncursesw/curses.h>
#endif

#if DISPLAY_TYPE == DISPLAY_WCURSES && !defined(_XOPEN_SOURCE_EXTENDED)
#define _XOPEN_SOURCE_EXTENDED
#endif

#endif

// GAMEPLAY ===========================================

#define FOV_SHADOW 0

#define FOV_ALGORITHM FOV_SHADOW

#define LINE_BRESENHAM 0

#define LINE_ALGORITHM LINE_BRESENHAM