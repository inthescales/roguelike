#include "config.h"
#include "enums.h"
#include "globals.h"
#include "stringutils.h"

#include <ostream>
#include <vector>

#include LIB_CURSES

using std::vector;

char color_escape_start = '|';
char color_escape_end = '~';

// Split a string into chunks of size width, not including color codes
// and handling wraparound text
vector<string> string_slice(string in, int width){
	
	vector<string> r;
    r.push_back("");
	
	int count = 0;
    vector<colorName> colorStack;
    colorStack.push_back(C_WHITE);
    
	for(int i = 0; i < in.size(); ++i){
	
		r.back() += in.at(i);
    
		// Ignore color code information in count
        if(i < in.size() && in.at(i) == color_escape_start){ 
            colorStack.push_back((colorName)(in.at(i+1)));
        } else if( i > 0 && in.at(i-1) == color_escape_start){ 
            continue;
        } else if (in.at(i) == color_escape_end) {
            colorStack.pop_back();
        } else {
            ++count;
        }
		
		// When we're at the end of a line, create a new string
		// Check for wrapover and r
		if(count == width){
			if(i + 1 < in.size()){
				if(in.at(i+1) == ' ') {
                    i += 1;
				} else if (in.at(i) != ' '){
                    int j;
					for(j = 0; in.at(i) != ' '; ++j){
                        --i;
                        if (in.at(i) == color_escape_start) {
                            colorStack.pop_back();
                        }
                    }
					r.back().resize( r.back().size() - j);
				}
			}
            colorName cur_color = colorStack.back();
            for(int j = 1; j < colorStack.size(); ++j) {
                r.back() += color_escape_end;
            }
			
            count = 0;
            r.push_back("");
            for(int j = 1; j < colorStack.size(); ++j) {
                r.back() += color_escape_start;
                r.back() += colorStack.at(j);
            }
            
		}
	}
    
    for(int j = 1; j < colorStack.size(); ++j) {
        r.back() += color_escape_end;
    }
	
	return r;
}

// Put a color tag before the string, and a color end tag at the end
string color_string(string text, int color){
	string r = escape_color(color) + text + color_escape_end;
	return r;
}

// Get a color code string for the input color
string escape_color(int color){
	return char_string(color_escape_start) + char_string((char)color);
}

string end_color() {

    return char_string(color_escape_end);
}

// Convert int to string
string int_string(int in){

	ostringstream convert;
	convert << in;
	return convert.str();
}

// Convert char to string
string char_string(char in) {

    ostringstream convert;
    convert << in;
    return convert.str();
}
