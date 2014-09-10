#ifndef COLOR_H

#define COLOR_H

#include "config.h"
#include "enums.h"

#include LIB_CURSES

#include <string>

using std::string;

#if DISPLAY_TYPE == DISPLAY_CURSES
typedef chtype d_glyph;
#elif DISPLAY_TYPE == DISPLAY_WCURSES
typedef wchar_t d_glyph;
#endif

void printcolor(int, int, string);
void printchar_cw(d_glyph);
void printchar_cw(d_glyph, int);

#endif
