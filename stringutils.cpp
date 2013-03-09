#include "stringutils.h"

int string_length(string in){

	int count = 0;
	bool inside = false;
	
	for(int i = 0; i < in.size(); ++i){
	
		if(in.at(i) == '|') inside = !inside;
		
		if(!inside && in.at(i) != '|'){
			++count;
		}
	}
	
	return count;
}

int string_lines(string in, int x){
	
	int r = x / string_length(in);
	return r;
}

vector<string> string_slice(string in, int width){
	
	vector<string> r;
	
	int count = 0;
	bool inside = false;
	
	for(int i = 0; i < in.size(); ++i){
	
		if(count == 0) r.push_back("");
	
		if(in.at(i) == '|') inside = !inside;
		
		if(!inside && in.at(i) != '|'){
			r.back() += in.at(i);
			++count;
		}
		
		if(count == width) count = 0;
	}
	
	return r;
}
