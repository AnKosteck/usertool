#include "database.h"
#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <algorithm>

using namespace std;

void removeUsers(std::string& parameter)
{
    readSystem(parameter);
    vector<string> usersToDelete;
    split(parameter, usersToDelete, ",");

    readDatabase(getGroups());
    multiset<User, UserOrder>* dbUsers = getDatabaseUsers();
    multiset<Group, GroupOrder>* dbGroups = getDatabaseGroups();
    int max_deletions, user_deletions = 0, group_deletions = 0;

    if(dbUsers == NULL || dbGroups == NULL) {
        cout << "Cannot delete users and/or groups because of missing database!" << endl;
        return;
    }

    max_deletions = usersToDelete.size();

    for(auto username: usersToDelete) {
        const User tmpUser(username);
        const Group tmpGroup(username);
        auto userIt = find(dbUsers->begin(), dbUsers->end(), tmpUser);
        auto groupIt = find(dbGroups->begin(), dbGroups->end(), tmpGroup);

        if(userIt != dbUsers->end()) {
            dbUsers->erase(userIt);
            user_deletions++;
        } else
            cout << "No user entry for username " << username << endl;
        if(groupIt != dbGroups->end()) {
            dbGroups->erase(groupIt);
            group_deletions ++;
        } else
            cout << "No group entry for username " << username << endl;
    }

    writeDatabase();
    closeDatabase();
    closeSystem();

    cout << "Deleted " << user_deletions << " users and " << group_deletions << " groups of max. " << max_deletions << endl;

}
