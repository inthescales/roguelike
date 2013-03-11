#include "enums.h"
#include "stringutils.h"
#include "globals.h"

#include "curses.h"

// Split a string into chunks of size width, not including color codes
// and handling wraparound texter
vector<string> string_slice(string in, int width){
	
	vector<string> r;
	
	int count = 0, lastcolor = -1;
	
	for(int i = 0; i < in.size(); ++i){
	
		if(count == 0) r.push_back("");
	
		r.back() += in.at(i);
	
		// Ignore pipes and the color codes that follow
		if(in.at(i) == '|'){
			r.back() += in.at(++i);
		} else {
			++count;
		}
		
		// When we're at the end of a line, create a new string
		// Check for wrapover and r
		if(count == width){
			if(i + 1 < in.size()){
				if(in.at(i+1) == ' ') ++i;
				else if (in.at(i) != ' '){
					int j;
					while(in.at(--i) != ' ') j++;
					r.back().resize( r.back().size() - j);					
				}
			}
			count = 0;
		}
	}
	
	return r;
}

// Put a color tag before the string, and return to default color after
string color_string(string text, int color){
	string r = escape_color(color) + text + escape_color(C_WHITE);
	return r;
}

// Get a color code string for the input color
string escape_color(int color){
	string r = "|";
	r += (char)color;
	
	return r;
}