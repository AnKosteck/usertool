#include "string_methods.h"
#include "usertool.h"

#include <cerrno>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include <vector>

using namespace std;

void log(std::ostream& logStream, const std::string& message)
{
    std::time_t result = std::time(nullptr);
    logStream << std::asctime(std::localtime(&result)) << "\t\t" << message << std::endl;
}

void log(std::ostream& logStream, const char* message)
{
    std::time_t result = std::time(nullptr);
    logStream << std::asctime(std::localtime(&result)) << "\t\t" << message << std::endl;
}

void daemon(std::string& parameter)
{
    vector<string> params;
    split(parameter,params,",");
    char system = convertStringToChar(params[0]);
    if((system != 'b' && system != 'l') || params.size() <= 1) {
        printUsage();
        return;
    }
    char daemonType = convertStringToChar(params[1]);
    if(daemonType != 'c' && daemonType != 's') {
        cout << "Specify the daemon type: ./usertool -d b|l,c|s where c means client and s stands for server." << endl;
        return;
    }

    /******************
    ******PARENT*******
    ******************/
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        cout << "Could not fork process, exiting!" << endl;
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    /******************
    *******CHILD*******
    ******************/

    cout << "Child is starting!" << endl;
    cout << "Child knows the files " << getUsersFile() << " and " << getGroupsFile() << endl;

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */
    ofstream logStream("/var/log/usertool", ofstream::app);
    if(!logStream) {
        cout << "Could not open log file! Aborting!" << endl;
        exit(EXIT_FAILURE);
    }

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }
    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Daemon-specific initialization goes here */

    /* The Big Loop */
    // while (1) {
    /* Do some task here ... */

    //  sleep(30); /* wait 30 seconds */
    //}

    if(daemonType == 's') {
        log(logStream,"Server has started!");
    } else {
        while(1) {
            systemUpdate(parameter);		//remove users, add users, update account status
            passwordUpdate(parameter);	//NOW look at password changes
            sleep(60*5);				//sleep 5 minutes
        }
    }

    exit(EXIT_SUCCESS);
}
