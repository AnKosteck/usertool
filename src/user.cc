#include "string_methods.h"
#include "user.h"

#include <algorithm>
#include <cctype>
#include <climits>
#include <fstream>

User::User(std::vector<std::string>& tokens, bool isDatabaseUser, bool bsd, std::multiset<Group, GroupOrder>* allGroups, std::vector<std::string>& groupsForUser)
{
    uid = -1;
    unsigned gid = -1;
    name = "invalid";
    password = "*";
    homePath = "/dev/zero";
    shellPath = "/usr/sbin/nologin";
    status = CRIPPLED;

    if( (bsd && (tokens.size() == 10 || tokens.size() == 9)) ||		//bsd with/without class info
        (!bsd && (tokens.size() == 7 || tokens.size() == 6))) {			//linux with/without user info
        uid = std::stoi(tokens[2]);
        gid = std::stoi(tokens[3]);
        name = tokens[0];
        status = UNSET;
        homePath = tokens[tokens.size()-2];
        shellPath = tokens[tokens.size()-1];
    }
    if(isDatabaseUser) {
        name = tokens[0];
        status = UNSET;
        uid = std::stoi(tokens[2]);
        gid = std::stoi(tokens[3]);
    }
    if(status == UNSET) {
        updatePassword(tokens[1], bsd);

        for(std::string gName: groupsForUser) {
            auto g = find(allGroups->begin(), allGroups->end(), Group(gName));
            if(g != allGroups->end())
                otherGroups.push_back(&(*g));
            else
                std::cout << "Could not find group object for " << gName << " for user " << name << std::endl;
        }

        const Group gTemp(gid);
        auto g = find(allGroups->begin(), allGroups->end(), gTemp);
        if(g != allGroups->end())
            mainGroup = &(*g);
        else
            std::cout << "User " << name << " has no group object for main group " << gid << std::endl;

    } else
        std::cout << "Invalid user!" << std::endl;
}

void User::updatePassword(std::string& pw, bool bsd)
{
    pw.erase(remove_if(pw.begin(), pw.end(), isspace), pw.end());		//remove whitespaces, front and back
    if(pw.length() == 0) {
        std::cout << "Deactivating user for empty password: " << name << ":" << uid << std::endl;
        password = "*";
        status = DEACTIVATED;
    } else if(bsd) {
        if(stringStartsWith(pw,"*LOCKED*")) {
            status = LOCKED;
            //password = pw.substr(8,std::string::npos);
            password = "*LOCKED*";
        } else if(stringStartsWith(pw,"*")) {
            status = DEACTIVATED;
            password = "*";
        } else {
            status = NORMAL;
            password = pw;
        }
    } else {
        if(stringStartsWith(pw,"!")) {
            status = LOCKED;
            //password = (pw.length() > 2) ? pw.substr(stringStartsWith(pw,"!!") ? 2 : 1, std::string::npos) : "!!";
            password = "!!";
        } else if(stringStartsWith(pw,"*")) {	//the initial entry could have *, so that is already done here
            status = DEACTIVATED;
            password = "*";
        } else {		//the initial passwd entry could have x, so code would move here -> either normal or deactivated status
            status = NORMAL;
            password = pw;
        }
    }
}

User::User(YAML::const_iterator& it, std::multiset<Group, GroupOrder>* databaseGroups, std::multiset<Group, GroupOrder>* systemGroups)
{
    uid = it->first.as<unsigned>();
    assert(it->second.Type() == YAML::NodeType::Map);
    YAML::Node userInformation = it->second["passwd"];
    if(userInformation["name"])
        name = userInformation["name"].as<std::string>();
    if(userInformation["password"])
        password = userInformation["password"].as<std::string>();

    if(userInformation["group"]) {
        std::string mainGroupName = userInformation["group"].as<std::string>();
        auto g = find(databaseGroups->begin(), databaseGroups->end(), Group(mainGroupName));
        if(g != databaseGroups->end())
            mainGroup = &(*g);
        else
            std::cout << "Could not find group object " << mainGroupName << " for user " << name << std::endl;
    } else
        std::cout << "User " << name << " has no group field and thus cannot be set" << std::endl;
    if(userInformation["groups"]) {
        std::vector<std::string> groupNames;
        split(userInformation["groups"].as<std::string>(),groupNames,",");
        for(std::string groupName: groupNames) {
            auto g = find(databaseGroups->begin(), databaseGroups->end(), Group(groupName));
            if(g != databaseGroups->end())
                otherGroups.push_back(&(*g));
            else {
                g = find(systemGroups->begin(), systemGroups->end(), Group(groupName));
                if(g != systemGroups->end()) {
                    //At this point the groups should actually be a system group, since every other group should
                    //be known to the database
                    //if (g->gid >= MIN_ID && g->gid <= MAX_ID)
                    //    std::cout << "User " << name << " knows a group from local system: " << g->name << std::endl;
                    otherGroups.push_back(&(*g));
                } else {
                     std::cout << "Databsae user " << name << " knows an unknown group from: " << groupName << std::endl;
                }
            }
        }
    } else
        std::cout << "User " << name << " has no groups field and thus cannot be set" << std::endl;

    if(userInformation["homepath"])
        homePath = userInformation["homepath"].as<std::string>();
    else
        homePath = "/home/" + name;
    if(userInformation["shellpath"])
        shellPath = userInformation["shellpath"].as<std::string>();
    else
        shellPath = "/bin/bash";
    if(userInformation["permissions"])
        homePermissions = userInformation["permissions"].as<std::string>();
    else
        homePermissions = "0700";
    updatePassword(password, false);

    YAML::Node personalUserInformation = it->second["personal"];
    if(personalUserInformation["fullname"])
        fullName = personalUserInformation["fullname"].as<std::string>();
    if(personalUserInformation["information"])
        information = personalUserInformation["information"].as<std::string>();
    if(personalUserInformation["email"])
        email = personalUserInformation["email"].as<std::string>();

    if(it->second["slurm_accounts"]) {
        slurmAccounts = it->second["slurm_accounts"].as<std::string>();
    }
}

std::string accountStatusToString(const Account_Status status)
{
    switch(status) {
    case NORMAL:
        return "NORMAL";
    case LOCKED:
        return "LOCKED";
    case DEACTIVATED:
        return "DEACTIVATED";
    case CRIPPLED:
        return "CRIPPLED";
    default:
        return "UNSET";
    }
}
Account_Status accountStatusStringToStatus(const std::string& statusString)
{
    if(statusString.compare("NORMAL") == 0)
        return NORMAL;
    if(statusString.compare("LOCKED") == 0)
        return LOCKED;
    if(statusString.compare("DEACTIVATED") == 0)
        return DEACTIVATED;
    if(statusString.compare("CRIPPLED") == 0)
        return CRIPPLED;
    return UNSET;
}
