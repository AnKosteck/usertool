#ifndef USER_H
#define USER_H

#include "group.h"

#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

enum Account_Status {
    NORMAL, LOCKED, DEACTIVATED, UNSET, CRIPPLED
};

std::string accountStatusToString(const Account_Status status);
Account_Status accountStatusStringToStatus(const std::string& statusString);

class User
{
public:
    unsigned int uid;
    std::string name;
    const Group* mainGroup;
    std::vector<const Group*> otherGroups;
    //std::vector<std::string> slurmAccounts;
    std::string homePath;
    std::string shellPath;
    std::string password;
    std::string gecko;		//special information
    std::string fullName;
    std::string information;
    std::string email;
    std::string homePermissions;
    std::string slurmAccounts;
    std::vector<std::string> unknownGroups;
    
    Account_Status status;

    User(std::string& n)
    {
        name = n;
    }
    User(std::vector<std::string>& tokens, bool isDatabaseUser, bool bsd, std::multiset<Group, GroupOrder>* allGroups, std::vector<std::string>& groupsForUser);
    User(YAML::const_iterator& it, std::multiset<Group, GroupOrder>* allGroups);
    User(const User& u): otherGroups(u.otherGroups), unknownGroups(u.unknownGroups)//, slurmAccounts(u.slurmAccounts)
    {
        uid = u.uid;
        name = u.name;
        homePath = u.homePath;
        shellPath = u.shellPath;
        password = u.password;
        status = u.status;
        mainGroup = u.mainGroup;
        fullName = u.fullName;
        information = u.information;
        email = u.email;
        gecko = u.gecko;
        homePermissions = u.homePermissions;
        slurmAccounts = u.slurmAccounts;
    }
    ~User() {}
    void printUser(std::ostream& out) const
    {
        out << name << ":" << password << ":" << uid << ":" << mainGroup->gid << std::endl;
    }
    void printUserYAML(YAML::Emitter& out) const
    {

        std::string groups("");
        for(const Group* g: otherGroups) {
            if(groups.empty())
                groups = g->name;
            else
                groups += "," + g->name;
        }

        out << YAML::Key << uid;
        out << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "passwd";
        out << YAML::Value << YAML::Flow << YAML::BeginMap <<
            YAML::Key << "name" << YAML::Value << name << YAML::Key << "group" << YAML::Value << mainGroup->name <<
            YAML::Key << "password" << YAML::Value << password << YAML::Key << "groups" << YAML::Value << groups <<
            YAML::Key << "shellpath" << YAML::Value << shellPath << YAML::Key << "homepath" << YAML::Value << homePath <<
            YAML::Key << "permissions" << YAML::Value << homePermissions << YAML::EndMap;
        out << YAML::Key << "personal" << YAML::Value << YAML::Flow << YAML::BeginMap <<
            YAML::Key << "fullname" << YAML::Value << fullName << YAML::Key << "information" << YAML::Value << information <<
            YAML::Key << "email" << YAML::Value << email << YAML::EndMap;
        out << YAML::Key << "slurm_accounts" << YAML::Value << slurmAccounts;
        out << YAML::EndMap;

        //std::cout << "name " << name << " uid " << uid << " gid " << gid << " password " << password << std::endl;
        //std::cout << "fullname: " << fullName << " info: " << information << " email: " << email << std::endl;
    }
    void printShadow(std::ostream& out) const
    {
        out << name << ":";
        if(status != DEACTIVATED) {
            if(status == LOCKED) {
                out << "!!:";
            } else
                out << password << ":";
        } else
            out << "*:";
        time_t seconds = time(NULL);
        if(seconds > 0) {
            unsigned daysSinceEpoch = seconds / (60 * 60 * 24);	//since 0:00, Jan 1, 1970
            out << daysSinceEpoch << ":0:99999:7:::";
        } else
            out << "16639:0:99999:7:::";
        out << std::endl;
    }
    void printPasswd(std::ostream& out) const
    {
        out << name << ":x:" << uid << ":" << mainGroup->gid << "::";
        if(!homePath.empty())
            out << homePath << ":";
        else
            out << "/home/" << name << ":";
        if(!shellPath.empty())
            out << shellPath << std::endl;
        else
            out << "/bin/bash" << std::endl;
    }
    void printMasterPasswd(std::ostream& out) const
    {
        switch(status) {
        case NORMAL:
            //name:password:uid:gid:class:change:expire:gecko:homepath:shellpath
            out << name << ":" << password << ":" << uid << ":" << mainGroup->gid << "::0:0:User &:" << homePath << ":" << shellPath << std::endl;
            break;
        case DEACTIVATED:
            out << name << ":*:" << uid << ":" << mainGroup->gid << "::0:0:User &:" << homePath << ":" << shellPath << std::endl;
            break;
        case LOCKED:
            out << name << ":*LOCKED*:" << uid << ":" << mainGroup->gid << "::0:0:User &:" << homePath << ":" << shellPath << std::endl;
            break;
        default:
            std::cout << "User: " << name << ":" << uid << ":" << mainGroup->gid << " has wrong status " << accountStatusToString(status) << std::endl;
        }
    }
    bool operator==(const User& u2) const
    {
        return name.compare(u2.name) == 0;
    };
    void updatePassword(std::string& pw, bool bsd);
    void checkUnknownGroups(std::multiset<Group, GroupOrder>* groups);

private:
};
inline std::ostream& operator<<(std::ostream& out, const User& u)
{
    u.printUser(out);
    return out;
};
inline YAML::Emitter& operator<<(YAML::Emitter& out, const User& u)
{
    u.printUserYAML(out);
    return out;
};

struct UserOrder {
    bool operator() (const User& a, const User& b) const
    {
        return a.uid < b.uid;
    }
};
#endif
