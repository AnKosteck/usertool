#include "string_methods.h"

#include <climits>
#include <exception>
#include <iostream>

void split(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)  {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/**
* This method tries to convert a std::string to an integer value.
* On an error, INT_MIN is returned.
**/
int convertStringToInt(const std::string& s)
{
    int intValue = INT_MIN;
    try {
        intValue = stoi(s);
    } catch(std::exception& e) {
        //std::cout << "Cannot convert string \"" << s << "\" to int: " << e.what() << std::endl;
    }
    return intValue;
}

char convertStringToChar(const std::string& s)
{
    char c = -1;
    if (s.length() == 1)
        c = s.c_str()[0];
    return c;
}

bool stringStartsWith(const std::string& s, const char* begin)
{
    bool isBegin = false;
    unsigned i = 0;
    const char* compare = s.c_str();
    while(i < s.length() && begin[i] != '\0') {
        if(compare[i] != begin[i])
            break;
        i++;
        isBegin = begin[i] == '\0';		//if code reaches here, every character was the same
    }
    return isBegin;
}
