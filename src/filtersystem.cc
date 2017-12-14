#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <fstream>

using namespace std;

void filterSystem(string& parameter)
{
    readSystem(parameter);

    multiset<User, UserOrder>* users = getUsers();
    multiset<Group, GroupOrder>* groups = getGroups();
    if(users != NULL && groups != NULL) {

        if(isBSD()) {
            ofstream mpasswd("master.passwd");
            ofstream group("group");
            if(mpasswd) {
                for(auto user: (*users)) {
                    if(user.uid < MIN_ID || user.uid > MAX_ID)
                        user.printMasterPasswd(mpasswd);
                }
            } else
                cout << "Cannot write to master.passwd" << endl;
            if(group) {
                for(auto g: (*groups)) {
                    if(g.gid < MIN_ID || g.gid > MAX_ID)
                        g.printGroupBSD(group);
                }
            }
            try {
                mpasswd.close();
            } catch(exception e) {}
            try {
                group.close();
            } catch(exception e) {}
        } else {
            ofstream passwd("passwd");
            ofstream shadow("shadow");
            ofstream group("group");
            ofstream gshadow("gshadow");
            if(passwd && shadow) {
                for(auto user: (*users)) {
                    if(user.uid < MIN_ID || user.uid > MAX_ID) {
                        user.printPasswd(passwd);
                        user.printShadow(shadow);
                    }
                }
            } else
                cout << "Cannot write to shadow or passwd" << endl;
            if(gshadow && group) {
                for(auto g: (*groups)) {
                    if(g.gid < MIN_ID || g.gid > MAX_ID) {
                        g.printGroupLinux(group);
                        g.printGShadow(gshadow);
                    }
                }
            } else
                cout << "Cannot write to group or gshadow" << endl;
            try {
                passwd.close();
            } catch(exception e) {}
            try {
                group.close();
            } catch(exception e) {}
            try {
                gshadow.close();
            } catch(exception e) {}
            try {
                shadow.close();
            } catch(exception e) {}
        }
    }
    closeSystem();
}
