#include "database.h"
#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <algorithm>

using namespace std;

void updateDatabase(string& parameter)
{
    readSystem(parameter);
    multiset<User, UserOrder>* users = getUsers();
    multiset<Group, GroupOrder>* groups = getGroups();

    if(users != NULL && groups != NULL) {
        readDatabase();
        multiset<User, UserOrder>* usersInDatabase = getDatabaseUsers();
        multiset<Group, GroupOrder>* groupsInDatabase = getDatabaseGroups();
        if(usersInDatabase != NULL && groupsInDatabase != NULL) {
            //now update the "database"
            int usersAdded = 0, groupsAdded = 0, usersUpdated = 0, groupsUpdated = 0;
            int updatedGroupsOfUser = 0;
            cout << "Updating database users" << endl;
            for(auto it: (*users)) {
                if(it.uid >= MIN_ID && it.uid <= MAX_ID) {	//filter system users, no need to have them in database
                    const User tmp(it.name);
                    auto uIt = find(usersInDatabase->begin(), usersInDatabase->end(), tmp);
                    if(uIt == usersInDatabase->end()) {		//So user is not yet in the database
                        usersInDatabase->insert(it);
                        usersAdded++;
                    } else if(it.status != (*uIt).status ||
                              it.password.compare((*uIt).password) != 0 ) {		//user is in database, so update status or password
                        User user(*uIt);
                        user.status = it.status;
                        user.password = it.password;
                        usersUpdated++;
                        usersInDatabase->erase(uIt);
                        usersInDatabase->insert(user);
                    }
                }
            }
            cout << "Updating database groups" << endl;
            for(auto it: (*groups)) {
                if(it.gid >= MIN_ID && it.gid <= MAX_ID) {	//filter system groups, no need to have them in database
                    Group tmp(it.name);
                    auto gIt = find(groupsInDatabase->begin(), groupsInDatabase->end(), tmp);
                    if(gIt == groupsInDatabase->end()) {		//So group is not yet in the database
                        groupsInDatabase->insert(it);
                        groupsAdded++;
                    } else if(gIt->members.compare(it.members) != 0) {		//db knows group, so update members
                        vector<string> dbMembers;
                        split(it.members,dbMembers,",");
                        Group g(*gIt);
                        for(string dbMember: dbMembers) {
                            if(g.members.find(dbMember) == string::npos) {
                                if(g.members.empty())
                                    g.members = dbMember;
                                else
                                    g.members += "," + dbMember;

                                //Find user and update groups value
                                User tmpUser(dbMember);
                                auto memberUser = find(usersInDatabase->begin(), usersInDatabase->end(), tmpUser);
                                if(memberUser != usersInDatabase->end()) {
                                    User updatedUser(*memberUser);
                                    const Group* gTemp = new Group(it);
                                    if(find(updatedUser.otherGroups.begin(), updatedUser.otherGroups.end(), &it) == updatedUser.otherGroups.end()) {
                                        updatedUser.otherGroups.push_back(&it);
                                        updatedGroupsOfUser ++;
                                    }
                                    delete gTemp;
                                } else
                                    cout << "ATTENTION: User " << dbMember << " is part of group " << it.name << " but seems to not exist!" << endl;
                            }
                        }
                        groupsInDatabase->erase(gIt);
                        groupsInDatabase->insert(g);
                        groupsUpdated++;
                    }
                }
            }
            writeDatabase();
            closeDatabase();

            cout << "Added " << usersAdded << " users and " << groupsAdded << " groups to database!" << endl;
            cout << "Modified " << usersUpdated << " users and " << groupsUpdated << " groups in database!" << endl;
            cout << "Added " << updatedGroupsOfUser << " times a group to groups value of users!" << endl;
        }

    }
    closeSystem();
}
