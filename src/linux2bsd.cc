#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <fstream>

using namespace std;

void linux2BSD(std::string& parameter)
{
    std::string systemType("l");
    readSystem(systemType);
    multiset<Group, GroupOrder>* groups = getGroups();
    multiset<User, UserOrder>* users = getUsers();

    if(groups != NULL && users != NULL) {
        ofstream mpasswd("master.passwd");
        ofstream group("group");
        if(!group || !mpasswd) {
            cout << "Cannot write to master.passwd or group" << endl;
        } else {

            for(auto it: (*users)) {
                if(it.uid >= MIN_ID && it.uid <= MAX_ID)
                    it.printMasterPasswd(mpasswd);
            }
            for(auto it: (*groups)) {
                if(it.gid >= MIN_ID && it.gid <= MAX_ID)
                    it.printGroupBSD(group);
            }
        }
        try {
            mpasswd.close();
        } catch (exception e) {}
        try {
            group.close();
        } catch (exception e) {}
    }
    closeSystem();
}
