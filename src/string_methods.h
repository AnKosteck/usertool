#ifndef STRING_METHODS
#define STRING_METHODS

#include <string>
#include <vector>

void split(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);

int convertStringToInt(const std::string& s);

char convertStringToChar(const std::string& s);

bool stringStartsWith(const std::string& s, const char* begin);

#endif
