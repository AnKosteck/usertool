#include "database.h"
#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <algorithm>

using namespace std;

void passwordUpdate(string& parameter)
{
    readSystem(parameter);

    multiset<User,UserOrder>* users = getUsers();

    if(users != NULL) {
        readDatabase(getGroups());
        multiset<User,UserOrder>* dbUsers = getDatabaseUsers();
        if(dbUsers != NULL) {
            int passwordChanges = 0;
            for(auto u: (*users)) {
                if(u.uid >= MIN_ID && u.uid <= MAX_ID) {
                    auto uDb = find(dbUsers->begin(), dbUsers->end(), u);
                    if(uDb != dbUsers->end()) {
                        if(u.status != uDb->status)
                            cout << "ATTENTION: User " << u.name << " has different status(" << accountStatusToString(u.status) << ") than in database(" << accountStatusToString(uDb->status) << ")" << endl;
                        if(u.password.compare(uDb->password) != 0 && u.status == NORMAL) {
                            User uDbNew(*uDb);
                            uDbNew.password = u.password;
                            dbUsers->erase(uDb);
                            dbUsers->insert(uDbNew);
                            passwordChanges++;
                        }
                    } else
                        cout << "ATTENTION: User " << u.name << " not in database!" << endl;
                }
            }
            writeDatabase();
            cout << "Updated " << passwordChanges << " passwords!" << endl;
        } else
            cout << "Cannot continue without dbUsers information!" << endl;
    }
    closeSystem();
}
