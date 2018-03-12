#ifndef SYSTEM_H
#define SYSTEM_H

#define DEFAULT_PERMISSIONS "0700"

#include "group.h"
#include "user.h"

#include <set>
#include <string>

std::multiset<Group, GroupOrder>* getGroups();
std::multiset<User, UserOrder>* getUsers();

bool isBSD();

void closeSystem();
void readSystem(std::string& parameter);
void writeSystem();

#endif
