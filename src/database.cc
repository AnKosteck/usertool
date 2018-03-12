#include "database.h"
#include "string_methods.h"
#include "usertool.h"

#include <cassert>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

std::multiset<Group, GroupOrder>* dbGroups = NULL;
std::multiset<User, UserOrder>* dbUsers = NULL;

std::multiset<Group, GroupOrder>* getDatabaseGroups()
{
    return dbGroups;
}
std::multiset<User, UserOrder>* getDatabaseUsers()
{
    return dbUsers;
}

void closeDatabase()
{
    dbGroups->clear();
    dbUsers->clear();
    delete dbGroups;
    delete dbUsers;
}

void readDatabase(std::multiset<Group, GroupOrder>* systemGroups)
{
    std::cout << "Loading database groups from " << getGroupsFile() << std::endl;

    YAML::Node groups = YAML::LoadFile(getGroupsFile());
    if(groups["cluster_groups"]) {
        assert(groups["cluster_groups"].Type() == YAML::NodeType::Map);
        dbGroups = new std::multiset<Group, GroupOrder>();
        for(YAML::const_iterator it=groups["cluster_groups"].begin(); it!=groups["cluster_groups"].end(); ++it)
            dbGroups->insert(Group(it));
    }

    std::cout << "Loading database users from " << getUsersFile() << std::endl;

    YAML::Node users = YAML::LoadFile(getUsersFile());
    if(users["cluster_users"]) {
        assert(users["cluster_users"].Type() == YAML::NodeType::Map);
        dbUsers = new std::multiset<User, UserOrder>();
        for(YAML::const_iterator it=users["cluster_users"].begin(); it!=users["cluster_users"].end(); ++it)
            dbUsers->insert(User(it, dbGroups, systemGroups));
    }
}

void writeDatabase()
{
    std::ofstream groups(getGroupsFile());
    YAML::Emitter g_out;
    g_out << YAML::BeginMap;
    g_out << YAML::Key << "cluster_groups";
    g_out << YAML::Value << YAML::BeginMap;
    for(auto group: *dbGroups)
        g_out << group;

    g_out << YAML::EndMap;
    g_out << YAML::EndMap;

    groups << "---" << std::endl << std::endl;
    groups << g_out.c_str();
    groups << std::endl << std::endl << "..." << std::endl;

    groups.close();

    std::ofstream users(getUsersFile());
    YAML::Emitter u_out;
    u_out << YAML::BeginMap;
    u_out << YAML::Key << "cluster_users";
    u_out << YAML::Value << YAML::BeginMap;
    for(auto user: *dbUsers)
        u_out << user;

    u_out << YAML::EndMap;
    u_out << YAML::EndMap;

    users << "---" << std::endl << std::endl;
    users << u_out.c_str();
    users << std::endl << std::endl << "..." << std::endl;

    users.close();
}
