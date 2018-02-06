# usertool
Usertool allows for user management on BSD or Linux systems and generates Ansible-usable files
## How to build
*usertool* requires a `c++11` able compiler and [yaml-cpp](https://github.com/jbeder/yaml-cpp) version >= 5.2 (as long as cmake can find *yaml-cpp*)
```bash
cd usertool
mkdir build
cd build
cmake ..
make
```
## Command line
```bash
./usertool -m <MODE> -p <PARAMETER> [-o <OUTPUT_DIR>] [-u <PATH_TO_USERSYAML_FILE> -g <PATH_TO_GROUPSYAML_FILE>]
```
## Modes
There are several modes or functionalities *usertool* supports:
-  **BSD2LINUX**: Create shadow,group,passwd,gshadow from master.passwd and group files
-  **FILTERSYSTEM**: Extract system users/groups into locol (current working dir) group and (master.)passwd files
-  **FLAWSEARCH**: Search for mismatches and problems in system's known users/groups and YAML files.
-  **LINUX2BSD**: Create master.passwd and group from shadow,group,passwd
-  **PASSWORDUPDATE**: Updates users YAML file with new passwords from system
-  **REMOVEUSERS**: Remove users and groups from YAML files
-  **SYSTEM2YAML**: Create YAML files for users and groups
-  **SYSTEMUPDATE**: Update user and group information on system from YAML files
-  **UPDATEDATABASE**: Update user and group information from system in YAML files

## Required parameters per mode
-  **BSD2LINUX**: , separated list <PATH_TO_MASTER.PASSWD>,<PATH_TO_GROUP>" << std::endl
-  **FILTERSYSTEM**: System type 'b' (BSD) or 'l' (Linux)" << std::endl
-  **FLAWSEARCH**: System type 'b' (BSD) or 'l' (Linux)" << std::endl
-  **LINUX2BSD**: , separated list <PATH_TO_PASSWD>,<PATH_TO_GROUP>,<PATH_TO_SHADOW>" << std::endl
-  **PASSWORDUPDATE**: System type 'b' (BSD) or 'l' (Linux)" << std::endl
-  **REMOVEUSERS**: , separated list of usernames <USER1>,<USER2>,...,<USERN>" << std::endl
-  **SYSTEM2YAML**: System type 'b' (BSD) or 'l' (Linux)" << std::endl
-  **SYSTEMUPDATE**: System type 'b' (BSD) or 'l' (Linux)" << std::endl
-  **UPDATEDATABASE**: System type 'b' (BSD) or 'l' (Linux)" << std::endl;
