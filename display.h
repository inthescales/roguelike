#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "enums.h"

#include LIB_CURSES

#include <string>

class glyph;

using std::string;

#if DISPLAY_TYPE == DISPLAY_CURSES
typedef chtype symbol_code;
#elif DISPLAY_TYPE == DISPLAY_WCURSES
typedef wchar_t symbol_code;
#endif

void printcolor(int, int, string);
void printglyph(glyph);
void printchar_cw(symbol_code);
void printchar_cw(symbol_code, colorName);

#endif
