#include "string_methods.h"
#include "usertool.h"

#include <cstring>
#include <map>
#include <unistd.h>

using namespace std;

enum Mode {
    BSD2LINUX,DAEMON,FILTERSYSTEM,FLAWSEARCH,LINUX2BSD,PASSWORDUPDATE,REMOVEUSERS,SYSTEM2YAML,SYSTEMUPDATE,UNSET,UPDATEDATABASE
};

static const std::map<std::string, Mode> modeStrings {
    {"BSD2LINUX", BSD2LINUX},
    {"DAEMON", DAEMON},
    {"FILTERSYSTEM", FILTERSYSTEM},
    {"FLAWSEARCH", FLAWSEARCH},
    {"LINUX2BSD", LINUX2BSD},
    {"PASSWORDUPDATE", PASSWORDUPDATE},
    {"REMOVEUSERS", REMOVEUSERS},
    {"SYSTEM2YAML", SYSTEM2YAML},
    {"SYSTEMUPDATE", SYSTEMUPDATE},
    {"UPDATEDATABASE", UPDATEDATABASE},
    {"UNSET", UNSET}
};

std::string modeToString(Mode m)
{
    for (auto it = modeStrings.begin(); it != modeStrings.end(); ++it)
        if(it->second == m)
            return it->first;
    
    return "UNSET";
}

Mode stringToMode(string m)
{
    auto it = modeStrings.find(m);
    if(it != modeStrings.end())
        return it->second;

    return UNSET;
}

std::string groupsYAMLFile;
std::string usersYAMLFile;
std::string workingDirectory;

std::string& getGroupsFile()
{
    return groupsYAMLFile;
}
std::string& getUsersFile()
{
    return usersYAMLFile;
}
std::string& getWorkingDirectory()
{
    return workingDirectory;
}

int main(int argc, char* argv[])
{
    if(argc < 2) {
        printUsage();
        return 0;
    }

    int c;
    enum Mode m = UNSET;
    string parameter("");

    //Parse Option
    while((c = getopt (argc, argv, "hHm:o:p:u:g:")) != -1) {
        switch (c) {
        case 'g':
            groupsYAMLFile = optarg;
            break;
        case 'm':
            m = stringToMode(optarg);
            break;
        case 'o':
            workingDirectory = optarg;
            break;
        case 'p':
            parameter = optarg;
            break;
        case 'u':
            usersYAMLFile = optarg;
            break;
        case 'h':
        case 'H':
        default:
            m = UNSET;
        }
    }
    if(m != UNSET)
        cout << "Mode " << modeToString(m) <<  " with parameter: " << parameter << endl;
    if(parameter.empty())
        m = UNSET;

    switch(m) {
    case BSD2LINUX:
        bsd2Linux(parameter);
        break;
    case DAEMON:
        daemon(parameter);
        break;
    case FILTERSYSTEM:
        filterSystem(parameter);
        break;
    case FLAWSEARCH:
        flawSearch(parameter);
        break;
    case LINUX2BSD:
        linux2BSD(parameter);
        break;
    case PASSWORDUPDATE:
        passwordUpdate(parameter);
        break;
    case REMOVEUSERS:
        removeUsers(parameter);
        break;
    case SYSTEM2YAML:
        system2YAML(parameter);
        break;
    case SYSTEMUPDATE:
        systemUpdate(parameter);
        break;
    case UPDATEDATABASE:
        updateDatabase(parameter);
        break;
    case UNSET:
    default:
        printUsage();
    }

    return 0;
}
