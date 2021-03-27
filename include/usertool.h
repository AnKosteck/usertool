#ifndef USERTOOL_H
#define USERTOOL_H

//Both IDs are inclusive, i.e. valid user IDs, so nobody with 65534 is not included
#define MIN_ID 1000
#define MAX_ID 65533

#include <iostream>
#include <string>

void bsd2Linux(std::string& parameter);
void daemon(std::string& parameter);
void filterSystem(std::string& parameter);
void flawSearch(std::string& parameter);
void linux2BSD(std::string& parameter);
void passwordUpdate(std::string& parameter);
void removeUsers(std::string& parameter);
void system2YAML(std::string& parameter);
void systemUpdate(std::string& parameter);
void updateDatabase(std::string& parameter);
void yamlTest();

inline void printUsage()
{
    std::cout << "./usertool -m <MODE> -p <PARAMETER> [-o <OUTPUT_DIR>] [-u <PATH_TO_USERSYAML_FILE> -g <PATH_TO_GROUPSYAML_FILE>]" << std::endl
              << "-h or -H displays this help" << std::endl
              << "-o specifies the destination path for any additional output the usertool generates (default: current working directory)" << std::endl
              << "-G (groups) -U (users) and -S (shadow) allow for path override, use -U for master.passwd files and always -U and -S together" << std::endl
              << "Modes for '-m':" << std::endl
              << "\tBSD2LINUX: Create shadow,group,passwd,gshadow from master.passwd and group files" << std::endl
              << "\tFILTERSYSTEM: Extract system users/groups into local (current working dir) group and (master.)passwd files" << std::endl
              << "\tFLAWSEARCH: Search for mismatches and problems in system's known users/groups and YAML files." << std::endl
              << "\tLINUX2BSD: Create master.passwd and group from shadow,group,passwd" << std::endl
              << "\tPASSWORDUPDATE: Updates users YAML file with new passwords from system" << std::endl
              << "\tREMOVEUSERS: Remove users and groups from YAML files" << std::endl
              << "\tSYSTEM2YAML: Create YAML files for users and groups" << std::endl
              << "\tSYSTEMUPDATE: Update user and group information on system from YAML files" << std::endl
              << "\tUPDATEDATABASE: Update user and group information from system in YAML files" << std::endl
              << "PARAMETERS for '-p':" << std::endl
              << "\tBSD2LINUX: , separated list <PATH_TO_MASTER.PASSWD>,<PATH_TO_GROUP>" << std::endl
              << "\tFILTERSYSTEM: System type 'b' (BSD) or 'l' (Linux)" << std::endl
              << "\tFLAWSEARCH: System type 'b' (BSD) or 'l' (Linux)" << std::endl
              << "\tLINUX2BSD: , separated list <PATH_TO_PASSWD>,<PATH_TO_GROUP>,<PATH_TO_SHADOW>" << std::endl
              << "\tPASSWORDUPDATE: System type 'b' (BSD) or 'l' (Linux)" << std::endl
              << "\tREMOVEUSERS: , separated list of usernames <USER1>,<USER2>,...,<USERN>" << std::endl
              << "\tSYSTEM2YAML: System type 'b' (BSD) or 'l' (Linux)" << std::endl
              << "\tSYSTEMUPDATE: System type 'b' (BSD) or 'l' (Linux)" << std::endl
              << "\tUPDATEDATABASE: System type 'b' (BSD) or 'l' (Linux)" << std::endl;
}

std::string& getGroupsPath();
std::string& getUsersPath();
std::string& getShadowPath();

std::string& getYAMLGroups();
std::string& getYAMLUsers();
std::string& getWorkingDirectory();

#endif
