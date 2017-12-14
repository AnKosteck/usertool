#include "database.h"
#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <algorithm>

using namespace std;

void printStatistics(int* stats, int sum)
{
    cout << "Statistic +++++++++" << endl
         << "\t#Users not in database: " << stats[0] << endl
         << "\t#Users not in passwd: " << stats[1] << endl
         << "\t#Groups not in database: " << stats[2] << endl
         << "\t#Groups not in group: " << stats[3] << endl
         << "\t#System Users with bad group: " << stats[4] << endl
         << "\t#Database Users with bad group: " << stats[5] << endl
         << "\t#Users not locked on system: " << stats[6] << endl
         << "\t#Users not locked in database: " << stats[7] << endl
         << "\t#FLAWS FOUND::::::: " << sum << endl;
}

void flawSearch(std::string& parameter)
{
    readSystem(parameter);
    multiset<User, UserOrder>* users = getUsers();
    multiset<Group, GroupOrder>* groups = getGroups();

    if(users != NULL && groups != NULL) {
        //Open database files
        readDatabase();
        multiset<User, UserOrder>* dbUsers = getDatabaseUsers();
        multiset<Group, GroupOrder>* dbGroups = getDatabaseGroups();
        int statistic[8] = {};
        int sum = 0;
        if(dbUsers != NULL && dbGroups != NULL) {
            //Now every information is accessible, so search for problems now.
            //Problem 1,2: Users are in the database, but not on the system and vice versa.
            //Probem 3,4: Groups are in the database, but not on the system and vice versa.
            //Problem 5: User has a main system group (gid<1000) or an unexisting group on that system
            //Problem 6: DB User has a main system group or and unexisting group in the database
            //Problem 7+8: A locked/deactivated user is not yet locked/deactivated, either on system or database
            for(auto u: (*users)) {
                if(u.uid >= MIN_ID && u.uid <= MAX_ID) {
                    auto uIt = find(dbUsers->begin(), dbUsers->end(), u);
                    if(uIt == dbUsers->end()) {
                        statistic[0]++;
                        sum++;
                        cout << "USER not in DATABASE: " << u.name << endl;
                    } else if(u.mainGroup->gid < MIN_ID || u.mainGroup->gid > MAX_ID) {
                        statistic[4]++;
                        sum++;
                        cout << "USER " << u.name << " with bad gid " << u.mainGroup->gid << endl;
                    } else {
                        const Group tmp(u.name);
                        auto gIt = find(groups->begin(), groups->end(), tmp);
                        if(gIt == groups->end()) {
                            statistic[4]++;
                            sum++;
                            cout << "USER " << u.name << " has no main group" << endl;
                        }
                    }
                }
            }
            for(auto uDb: (*dbUsers)) {
                auto uIt = find(users->begin(), users->end(), uDb);
                if(uIt == users->end()) {
                    statistic[1]++;
                    sum ++;
                    cout << "USER not in PASSWD: " << uDb.name << endl;
                } else if(uDb.mainGroup->gid < MIN_ID || uDb.mainGroup->gid > MAX_ID) {
                    statistic[5]++;
                    sum++;
                    cout << "DB USER " << uDb.name << " with bad gid " << uDb.mainGroup->gid << endl;
                } else {
                    const Group tmp(uDb.name);
                    auto gIt = find(dbGroups->begin(), dbGroups->end(), tmp);
                    if(gIt == dbGroups->end()) {
                        statistic[5]++;
                        sum++;
                        cout << "DB GROUP " << uDb.name << " does not exist" << endl;
                    }
                }
            }
            for(auto g: (*groups)) {
                auto gIt = find(dbGroups->begin(), dbGroups->end(), g);
                if(g.gid >= MIN_ID && g.gid <= MAX_ID && gIt == dbGroups->end()) {
                    statistic[2]++;
                    sum ++;
                    cout << "GROUP not in DATABASE: " << g.name << endl;
                }
            }
            for(auto gDb: (*dbGroups)) {
                auto gIt = find(groups->begin(), groups->end(), gDb);
                if(gIt == groups->end()) {
                    statistic[3]++;
                    sum ++;
                    cout << "GROUP not in GROUP file: " << gDb.name << endl;
                }
            }
            closeDatabase();
            printStatistics(statistic, sum);
        } else
            cout << "Cannot open database users or groups file! Aborting!" << endl;
    }
    closeSystem();
}
