#include "string_methods.h"
#include "system.h"
#include "usertool.h"

#include <fstream>

using namespace std;

void bsd2Linux(string& parameter)
{
    std::string systemType("b");
    readSystem(systemType);

    multiset<Group, GroupOrder>* groups = getGroups();
    multiset<User, UserOrder>* users = getUsers();

    if(users != NULL && groups != NULL) {

        ofstream passwd("passwd");
        ofstream shadow("shadow");
        ofstream group("group");
        ofstream gshadow("gshadow");

        if(!gshadow || !group || !passwd || !shadow) {
            cout << "Cannot write to one or more of the files gshadow/group/passwd/shadow" << endl;
        } else {
            for(auto it: (*users)) {
                if(it.uid >= MIN_ID && it.uid <= MAX_ID) {
                    it.printPasswd(passwd);
                    it.printShadow(shadow);
                }
            }
            for(auto it: (*groups)) {
                if(it.gid >= MIN_ID && it.gid <= MAX_ID) {
                    it.printGroupLinux(group);
                    it.printGShadow(gshadow);
                }
            }
        }

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
    closeSystem();
}
