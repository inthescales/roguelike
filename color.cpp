#include "color.h"
#include "config.h"
#include "globals.h"
#include <stdlib.h>
#include <stdio.h>
#include LIB_CURSES

// Returns the color pair component of a color
short color::get_pair(short colorName) {

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

// Returns a grayed-out version of the color
colorName color::get_grayed(short colorName) {

  switch(colorName) {
      case C_BLACK: return C_GRAY;
      case C_GRAY: return C_GRAY;
      case C_WHITE: return C_GRAY;
      case C_RED: return C_GRAY;
      case C_ORANGE: return C_GRAY;
      case C_GREEN: return C_GRAY;
      case C_LIME: return C_GRAY;
      case C_BLUE: return C_GRAY;
      case C_SKY: return C_GRAY;
      case C_PURPLE: return C_GRAY;
      case C_MAGENTA: return C_GRAY;
      case C_BROWN: return C_GRAY;
      case C_YELLOW: return C_GRAY;
      case C_GRAY_INV: return C_GRAY_INV;
      case C_WHITE_INV: return C_GRAY_INV;
  }

  return C_GRAY;
}

// Returns whether the color should have the bold attribute
bool color::get_bold(short colorName) {

    switch(colorName) {
        case C_WHITE:
        case C_ORANGE:
        case C_LIME:
        case C_SKY:
        case C_MAGENTA:
        case C_YELLOW:
        case C_CYAN:
            return true;
    }

    return false;
}

// Returns whether the color should have the bold attribute
bool color::get_blink(short colorName) {

    switch(colorName) {
        case C_WHITE_INV:
            return true;
    }

    return false;
}

// Returns attributes needed for a color
int color::get_attr(short colorName) {
 
    int attr = 0;
    
    if (get_bold(colorName)) {
        attr |= A_BOLD;
    }

    if (get_blink(colorName)) {
        attr |= A_BLINK;
    }
    
    return attr;
}
