#ifndef STRINGUTILS_H

#define STRINGUTILS_H

#include <string>
#include <vector>

using std::string;
using std::vector;

vector<string> string_slice(string, int);
string escape_color(int);
string color_string(string, int);

#endif