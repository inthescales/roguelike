#include "enums.h"
#include "globals.h"

#include <curses.h>

// Map from color enum to color pairs
int color_value[] = 
{
	0<<24,
	1<<24, (1<<24) | A_BOLD,
	2<<24, (2<<24) | A_BOLD,
	3<<24, (3<<24) | A_BOLD,
	4<<24, (4<<24) | A_BOLD,
	5<<24, (5<<24) | A_BOLD,
	6<<24, (6<<24) | A_BOLD,
	7<<24, (7<<24) | A_BOLD,
};
