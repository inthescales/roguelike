#include "enums.h"
#include "stringutils.h"
#include "globals.h"

#include "curses.h"

int string_length(string in){

	int count = 0;
	
	for(int i = 0; i < in.size(); ++i){
	
		if(in.at(i) == '|') count -= 2;

		++count;
	}
	
	return count;
}

int string_lines(string in, int x){
	
	int r = x / string_length(in);
	return r;
}

vector<string> string_slice(string in, int width){
	
	vector<string> r;
	
	int count = 0, lastcolor = -1;
	
	for(int i = 0; i < in.size(); ++i){
	
		if(count == 0) r.push_back("");
	
		r.back() += in.at(i);
	
		if(in.at(i) == '|'){
			r.back() += in.at(++i);
		} else {
			++count;
		}
		
		if(count == width) count = 0;
	}
	
	return r;
}

string color_string(string text, int color){
	string r = escape_color(color) + text + escape_color(C_WHITE);
	return r;
}

string escape_color(int color){
	string r = "|";
	r += (char)color;
	
	return r;
}