#ifndef WINDOW_H

#define WINDOW_H

#include "glyph.h"

#include <string>
#include <vector>

using std::string;
using std::vector;
typedef vector<string> buffer;

class actor;
class object;
class map;

class window{

	public:
	
    bool should_update;
    
	window(int, int, int, int);
    void clear();
    
    // Display map and info panels
	void display_map(map *, bool);
    void display_tile(map *, int, int);
    void display_tile(tile *);
    void display_glyph(glyph, int, int);
	void display_status();
	void display_inventory(actor &, string prompt);
	void display_equipment(actor &);
	void display_conditions(actor *);
    static void display_all();
    static void center_cursor();
    
    // Text output
	void print(string);
	void print(string, buffer *);
    void break_buffer();
	void print_buf(buffer *);
	void print_line(string, int);
    void print_error(string);
	
	int x, y, width, height;

};

void break_buffer(buffer *);

#endif
