#include "database.h"
#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <algorithm>
#include <exception>
#include <fstream>

using namespace std;

void systemUpdate(string& parameter)
{
    readSystem(parameter);

    multiset<User, UserOrder>* users = getUsers();
    multiset<Group, GroupOrder>* groups = getGroups();

    if(users != NULL && groups != NULL) {
        //Open database files
        readDatabase();
        multiset<User, UserOrder>* dbUsers = getDatabaseUsers();
        multiset<Group, GroupOrder>* dbGroups = getDatabaseGroups();
        int usersAdded = 0, usersModified = 0, groupsAdded = 0, groupsModified = 0;
        if(dbUsers != NULL && dbGroups != NULL) {
            for(auto uDb: (*dbUsers)) {
                auto it = find(users->begin(), users->end(), uDb);
                if(it == users->end()) {	//new user
                    users->insert(uDb);
                    usersAdded ++;
                } else {											//system knows user, so update account status/password
                    //Since elements are stored as const in set/multiset, to avoid undefined behaviour (otherwise
                    //elements could be changed so that ordering is inconsistent)
                    //construct new Object;
                    User user(*it);
                    if(uDb.status != (*it).status) {	//but only, if status or password is different
                        user.status = uDb.status;
                        user.password = uDb.password;
                        usersModified ++;
                    } else if(uDb.status == NORMAL && uDb.password.compare((*it).password) != 0) {
                        user.password = uDb.password;
                        usersModified++;
                    }
                    users->erase(it);
                    users->insert(user);
                }
            }
            for(auto gDb: (*dbGroups)) {
                auto it = find(groups->begin(), groups->end(), gDb);
                if(it == groups->end()) {		//new group
                    groups->insert(gDb);
                    groupsAdded ++;
                } else if(gDb.members.compare((*it).name) != 0) {										//system knows group, so update members
                    Group g(*it);
                    vector<string> dbMembers;
                    split(gDb.members,dbMembers,",");
                    for(string dbMember: dbMembers)
                        if(g.members.find(dbMember) == string::npos) {
                            if(g.members.empty())
                                g.members = dbMember;
                            else
                                g.members += "," + dbMember;
                        }
                    groups->erase(it);
                    groups->insert(g);
                    groupsModified ++;
                }
            }
            cout << "Added " << usersAdded << " new users to system, updated " << usersModified << " users on system" << endl;
            cout << "Added " << groupsAdded << " new groups to system, updated " << groupsModified << " groups on system" << endl;
            writeSystem();
            closeDatabase();
        } else
            cout << "Cannot open database users or groups file! Aborting!" << endl;

    }
    closeSystem();
}
