#ifndef STRINGUTILS_H

#define STRINGUTILS_H

#include <string>
#include <sstream>
#include <vector>

using std::string;
using std::vector;
using std::ostringstream;

extern char color_escape_start;
extern char color_escape_end;

vector<string> string_slice(string, int);
string escape_color(int);
string color_string(string, int);
string int_string(int);
string char_string(char);

#endif