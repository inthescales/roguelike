#ifndef STRINGUTILS_H

#define STRINGUTILS_H

#include <string>
#include <vector>

using namespace std;

int string_length(string);
int string_lines(string, int);
vector<string> string_slice(string, int);
string escape_color(int);
string color_string(string, int);

#endif