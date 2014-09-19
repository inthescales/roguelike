#include "enums.h"

// Returns color pair corresponding to color name
long get_color(short colorName);

// Returns the color pair component of a color
bool get_bold(short colorName);

// Returns attributes needed for a color
int get_attr(short colorName);
