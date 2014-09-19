#include "color.h"
#include "config.h"
#include "enums.h"
#include "globals.h"
#include <stdlib.h>
#include <stdio.h>
#include LIB_CURSES

// Returns the color pair component of a color
long get_color(short colorName) {

  switch(colorName) {
  case C_BLACK: return CP_BLACK_BLACK;
  case C_GRAY: return CP_GRAY_BLACK;
  case C_WHITE: return CP_GRAY_BLACK;
  case C_RED: return CP_RED_BLACK;
  case C_ORANGE: return CP_RED_BLACK;
  case C_GREEN: return CP_GREEN_BLACK;
  case C_LIME: return CP_GREEN_BLACK;
  case C_BLUE: return CP_BLUE_BLACK;
  case C_SKY: return CP_BLUE_BLACK;
  case C_PURPLE: return CP_PURPLE_BLACK;
  case C_MAGENTA: return CP_PURPLE_BLACK;
  case C_BROWN: return CP_BROWN_BLACK;
  case C_YELLOW: return CP_BROWN_BLACK;
  case C_GRAY_INV: return CP_BLACK_GRAY;
  case C_WHITE_INV: return CP_BLACK_GRAY;
  }

  return 0;
}

// Returns whether the color should have the bold attribute
bool get_bold(short colorName) {
  switch(colorName) {
  case C_WHITE:
  case C_ORANGE:
  case C_LIME:
  case C_SKY:
  case C_MAGENTA:
  case C_YELLOW:
  case C_CYAN:
  case C_WHITE_INV:
    return true;
  }

  return false;
}

// Returns attributes needed for a color
int get_attr(short colorName) {
  int attr = 0;
  switch(colorName) {
  case C_WHITE:
  case C_ORANGE:
  case C_LIME:
  case C_SKY:
  case C_MAGENTA:
  case C_YELLOW:
  case C_CYAN:
  case C_WHITE_INV:
    attr |= A_BOLD;
  }

  return attr;
}
