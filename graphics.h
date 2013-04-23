#ifndef COLOR_H

#define COLOR_H

#include "curses.h"
#include "enums.h"

#include <string>

using std::string;

chtype comp(unsigned char, colorName);
void printcolor(int, int, string);

#endif
