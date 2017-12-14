#include "group.h"
#include "string_methods.h"

#include <fstream>

Group::Group(std::vector<std::string>& tokens, bool bsd)
{
    if(tokens.size() >= 3) {
        name = tokens[0];
        gid = convertStringToInt(tokens[2]);
        if(tokens.size() == 4)
            members = tokens[3];
        else
            members = "";
        if (!bsd)
            password = "x";
        else
            password = "*";
    } else {
        std::cout << "Invalid group information!" << std::endl;
        name = "invalid";
        gid = -1;
        members = "";
        password = "";
    }
}
