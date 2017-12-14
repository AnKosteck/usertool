#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <yaml-cpp/yaml.h>

using namespace std;

void writeAndCloseFile(const char* fileName, YAML::Emitter& oY);
void writeGroups2YAML(multiset<Group, GroupOrder>* groups);
void writeUsers2YAML(multiset<User, UserOrder>* users);

void system2YAML(string& parameter)
{

    readSystem(parameter);

    multiset<User, UserOrder>* users = getUsers();
    multiset<Group, GroupOrder>* groups = getGroups();

    if(users != NULL && groups != NULL) {

        writeUsers2YAML(users);
        cout << "Users have been written to users.yml in CWD" << endl;
        writeGroups2YAML(groups);
        cout << "Groups have been written to groups.yml CWD" << endl;
    }
    closeSystem();
}

void writeAndCloseFile(const char* fileName, YAML::Emitter& oY)
{
    ofstream of(fileName);
    if(of) {
        of << "---" << endl << endl << oY.c_str() << endl << endl << "..." << endl;
        of.close();
    } else
        std::cout << "Cannot open " << fileName << endl;
}

void writeGroups2YAML(multiset<Group, GroupOrder>* groups)
{
    YAML::Emitter g_out;

    g_out << YAML::BeginMap << YAML::Key << "cluster_groups" << YAML::Value << YAML::BeginMap;
    for(auto g: *groups) {
        if(g.gid >= MIN_ID && g.gid <= MAX_ID) {
            g_out << g;
        }
    }
    g_out << YAML::EndMap << YAML::EndMap;

    writeAndCloseFile("groups.yml", g_out);
}
void writeUsers2YAML(multiset<User, UserOrder>* users)
{
    YAML::Emitter u_out;

    u_out << YAML::BeginMap << YAML::Key << "cluster_users" << YAML::Value << YAML::BeginMap;
    for(auto u: *users) {
        if(u.uid >= MIN_ID && u.uid <= MAX_ID) {
            u_out << u;
        }
    }

    writeAndCloseFile("users.yml", u_out);
}
