// Config information for roguelike

// INTEGER SIZE ====================================
// for void / int casts in argmap
//# define size_32
#define size_64

// DISPLAY =========================================
#define DISPLAY_CURSES       0
#define DISPLAY_WCURSES      1
#define DISPLAY_TILES        2

#define DISPLAY_TYPE DISPLAY_WCURSES

#define LIB_CURSES <ncursesw/curses.h>

#if DISPLAY_TYPE == DISPLAY_WCURSES && !defined(_XOPEN_SOURCE_EXTENDED)
#define _XOPEN_SOURCE_EXTENDED
#endif
