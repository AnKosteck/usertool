#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <climits>
#include <fstream>
#include <map>

bool bsd = false;
std::multiset<Group, GroupOrder>* groups = NULL;
std::multiset<User, UserOrder>* users = NULL;

void readGroupFile(const char* groupPath, const char* duplicatesPath, std::map<std::string, std::vector<std::string>>& groupsPerUser);
void readUsersFile(const char* passwdPath, const char* shadowPath, const char* duplicatesPath, std::multiset<Group, GroupOrder>* allGroups, std::map<std::string, std::vector<std::string>>& groupsPerUser);
void writeGroupFile();
void writeUsersFiles();

bool isBSD()
{
    return bsd;
}

std::multiset<Group, GroupOrder>* getGroups()
{
    return groups;
}
std::multiset<User, UserOrder>* getUsers()
{
    return users;
}

void closeSystem()
{
    if(groups != NULL) {
        groups->clear();
        delete groups;
    }
    if(users != NULL) {
        users->clear();
        delete users;
    }
}

void readSystem(std::string& parameter)
{
    char systemType = convertStringToChar(parameter);

    std::map<std::string, std::vector<std::string>> groupsPerUser;
    
    std::string groupsPath = getGroupsPath();
    std::string shadowPath = getShadowPath();
    std::string usersPath = getUsersPath();
    
    if(systemType == 'b' || systemType == 'l') {
        if(systemType == 'b') {
            bsd = true;
            if (!groupsPath.empty())
                readGroupFile(groupsPath.c_str(),"duplicates", groupsPerUser);
            else
                readGroupFile("/etc/group","duplicates", groupsPerUser);
            if (!usersPath.empty())
                readUsersFile(usersPath.c_str(),NULL,"duplicates",groups, groupsPerUser);
            else
                readUsersFile("/etc/master.passwd",NULL,"duplicates",groups, groupsPerUser);
        } else {
            if (!groupsPath.empty())
                readGroupFile(groupsPath.c_str(),"duplicates", groupsPerUser);
            else
                readGroupFile("/etc/group","duplicates", groupsPerUser);
            if (!usersPath.empty() && !shadowPath.empty())
                readUsersFile(usersPath.c_str(),shadowPath.c_str(),"duplicates",groups, groupsPerUser);
            else
                readUsersFile("/etc/passwd","/etc/shadow","duplicates",groups, groupsPerUser);
        }

        if(groups == NULL)
            std::cout << "Could not read the groups for system " << systemType << std::endl;
        if(users == NULL)
            std::cout << "Could not read the users for system " << systemType << std::endl;
    } else
        printUsage();
}

void readGroupFile(const char* groupPath, const char* duplicatesPath, std::map<std::string, std::vector<std::string>>& groupsPerUser)
{
    std::ofstream duplicates(duplicatesPath,std::ofstream::app);
    std::ifstream gsource(groupPath);
    if(!gsource || !duplicates) {
        std::cout << "Cannot open/create the file group/duplicates, aborting!" << std::endl;
    } else {
        groups = new std::multiset<Group, GroupOrder>();
        std::set<int> gids;
        std::string gnames = "";
        std::string line;
        //group layout:
        //name:password:gid:members
        //for linux set password to x
        //for bsd set password to *
        while(std::getline(gsource, line)) {
            std::vector<std::string> tokens;
            split(line, tokens, ":");
            if(tokens.size() >= 3) {
                int gid = convertStringToInt(tokens[2]);
                Group g(tokens, bsd);
                if(gnames.find(tokens[0] + ";") != std::string::npos)
                    duplicates << "GROUP: " << tokens[0] << std::endl;
                else
                    gnames += tokens[0] + ";";
                if(gids.find(gid) != gids.end())
                    duplicates << "GID: " << gid << std::endl;
                else
                    gids.insert(gid);
                if(!g.members.empty()) {
                    std::vector<std::string> members;
                    split(g.members,members,",");
                    for(std::string member: members) {
                        if(groupsPerUser.count(member) > 0)
                            groupsPerUser[member].push_back(g.name);
                        else {
                            std::vector<std::string> v;
                            v.push_back(g.name);
                            groupsPerUser[member] = v;
                        }
                    }
                }
                groups->insert(g);
            } else
                std::cout << "Not a normal group line: " << line.c_str() << std::endl;
        }
        duplicates.close();
        gsource.close();
    }
}

void readUsersFile(const char* passwdPath, const char* shadowPath, const char* duplicatesPath, std::multiset<Group, GroupOrder>* allGroups, std::map<std::string, std::vector<std::string>>& groupsPerUser)
{
    std::ifstream passwd(passwdPath);
    std::ofstream duplicates(duplicatesPath, std::ofstream::app);
    if(!passwd || !duplicates) {
        std::cout << "Cannot open passwd or duplicates file!" << std::endl;
    } else {

        users = new std::multiset<User, UserOrder>();
        std::multiset<User, UserOrder>* usersWithoutPassword = new std::multiset<User, UserOrder>();
        std::multiset<User, UserOrder>* crippledUsers = new std::multiset<User, UserOrder>();
        std::set<int> uids;
        std::string names = "";
        std::string line;
        //passwd entry:
        //username:password-indicator:uid:gid:user info:home directory:shell
        //master.passwd entry:
        //name:password:uid:gid:class:change:expire:gecko:homepath:shellpath
        //std::cout << "Parsing start" << std::endl;
        std::vector<std::string> emptyVector;
        while(std::getline(passwd, line)) {
            std::vector<std::string> tokens;
            split(line,tokens,":");
            if( (bsd && (tokens.size() == 9 || tokens.size() == 10)) ||				//bsd with/without class info
                (!bsd && (tokens.size() == 6 || tokens.size() == 7))) {		//linux with/without user info
                unsigned uid = convertStringToInt(tokens[2]);
                auto groupsForUser = groupsPerUser.find(tokens[0]);
                //std::cout << "First user creation of " << tokens[0] << std::endl;
                User user(tokens, false, bsd, allGroups, (groupsForUser == groupsPerUser.end())?emptyVector:groupsForUser->second);
                user.homePermissions = DEFAULT_PERMISSIONS;
                if(names.find(tokens[0] + ";") != std::string::npos)
                    duplicates << "USER: " << tokens[0] << std::endl;
                else
                    names += tokens[0] + ";";
                if(uids.find(uid) != uids.end())
                    duplicates << "UID: " << uid << std::endl;
                else
                    uids.insert(uid);
                if(user.status == CRIPPLED)
                    crippledUsers->insert(user);
                else if(bsd)	//bsd has already set everything, since master.passwd has every possible info on users
                    users->insert(user);
                else	//linux users have normal, or deactivated status here
                    if(user.status != DEACTIVATED)
                        usersWithoutPassword->insert(user);
                    else
                        users->insert(user);
            } else if(bsd)
                std::cout << "Not a normal master.passwd line: " << line << std::endl;
            else
                std::cout << "Not a normal passwd line: " << line << std::endl;
        }
        //for(auto it: (*usersWithoutPassword))
        //	std::cout << it;
        //std::cout << "Grippled users: " << crippledUsers->size() << ", without password: " << usersWithoutPassword->size() << ", users: " << users->size() << std::endl;
        //Now update passwords using the shadow file
        //std::cout << "NON BSD password updates" << std::endl;
        if(!bsd) {
            std::ifstream shadow(shadowPath);
            if(shadow) {
                while(std::getline(shadow,line)) {
                    std::vector<std::string> tokens;
                    split(line,tokens,":");
                    if(tokens.size() >= 2) {
                        //look if user exists
                        if(names.find(tokens[0] + ";") != std::string::npos) {
                            //name:password:last-change:min-days-before-next-change:max-days-valid-pw:days-for-warning:days-after-expiration:account-expiration:reserved
                            //std::cout << "TMP User creation of " << tokens[0] << std::endl;
                            const User tmp(tokens[0]);
                            //std::cout << "After tmp" << std::endl;
                            auto it = std::find(usersWithoutPassword->begin(), usersWithoutPassword->end(), tmp);
                            if(it != usersWithoutPassword->end()) {
                                //std::cout << "Creating new user for " << it->name << std::endl;
                                User u(*it);
                                //std::cout << "New user object created" << std::endl;
                                if(convertStringToInt(tokens[1]) == INT_MIN) {		//it is a password and not a following number field
                                    u.updatePassword(tokens[1],false);
                                } else {			//not a password -> empty password -> should not be allowed
                                    std::string pw("");
                                    u.updatePassword(pw,false);
                                }
                                usersWithoutPassword->erase(it);
                                users->insert(u);
                            } else {
                                std::cout << "Could not find a user object for name " << tokens[0] << std::endl;
                            }
                        } else {		//no user passwd entry, shadow line should not be used any more
                            std::cout << "ATTENTION: No user for shadow line: " << line << std::endl;
                        }
                    } else {
                        std::cout << "Not a normal shadow line: " << line << std::endl;
                    }
                }
                shadow.close();
            } else
                std::cout << "Cannot open shadow file " << shadowPath << std::endl;
        }
        std::cout << crippledUsers->size() + usersWithoutPassword->size() << " bad user accounts found!" << std::endl;
        usersWithoutPassword->clear();
        crippledUsers->clear();
        delete usersWithoutPassword;
        delete crippledUsers;
        duplicates.close();
        passwd.close();
    }
}

void writeSystem()
{
    writeGroupFile();
    writeUsersFiles();
}

void writeGroupFile()
{

    if(bsd) {
        std::ofstream group("/etc/group");
        if(group) {
            for(auto g: (*groups))
                g.printGroupBSD(group);
            group.close();
        } else
            std::cout << "Cannot write to /etc/group!" << std::endl;
    } else {
        std::ofstream group("/etc/group");
        if(group) {
            std::ofstream gshadow("/etc/gshadow");
            if(gshadow) {
                for(auto g: (*groups)) {
                    g.printGroupLinux(group);
                    g.printGShadow(group);
                }
                gshadow.close();
            } else
                std::cout << "Cannot write to /etc/gshadow!" << std::endl;
            group.close();
        } else
            std::cout << "Cannot write to /etc/group!" << std::endl;
    }
}

void writeUsersFiles()
{
    if(bsd) {
        std::ofstream mpasswd("/etc/master.passwd");
        if(mpasswd) {
            for(auto u: (*users))
                u.printMasterPasswd(mpasswd);
            mpasswd.close();
        } else
            std::cout << "Cannot write to /etc/master.passwd!" << std::endl;
    } else {
        std::ofstream passwd("/etc/passwd");
        if(passwd) {
            std::ofstream shadow("/etc/shadow");
            if(shadow) {
                for(auto u: (*users)) {
                    u.printPasswd(passwd);
                    u.printShadow(shadow);
                }
                shadow.close();
            } else
                std::cout << "Cannot write to /etc/shadow!" << std::endl;
            passwd.close();
        } else
            std::cout << "Cannot write to /etc/passwd!" << std::endl;
    }
}
