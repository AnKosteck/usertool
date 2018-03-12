#ifndef DATABASE_H
#define DATABASE_H

#include "group.h"
#include "user.h"

#include <set>

std::multiset<Group, GroupOrder>* getDatabaseGroups();
std::multiset<User, UserOrder>* getDatabaseUsers();

//Database operations
void closeDatabase();
void readDatabase(std::multiset<Group, GroupOrder>* systemGroups);
void writeDatabase();

#endif
