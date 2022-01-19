#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>

/**
 * Prints memory utilization report
 **/
void generateMemoryUsage() {
    printf(" Memory usage: ______ kilobytes\n");
    printf("---------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
}

/**
 * Prints CPU utilization report
 **/
void generateCPUUsage() {
    printf("---------------------------------------\n");
    printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN)); // display number of cores
    
}

/**
 * Prints users connected in a given time and how many sessions each user is connected to
 **/
void generateUserUsage() {
    printf("---------------------------------------\n");
    printf("### Sessions/users ###\n");

    struct utmp *ut;

    setutent(); // Go to start of user information
    ut = getutent(); // Reads line from current position (i.e., user information)

    while (ut != NULL) {
        if (ut -> ut_type == USER_PROCESS) {
            printf("%s\t%s (%s)\n", ut -> ut_user, ut -> ut_line, ut -> ut_host); // displays user login name, device name, and host name
        }
        ut = getutent(); // Reads next line
    }

}

/**
 * Prints System Information
 **/
void displaySystemInfo() {
    struct utsname uts;
    uname(&uts);
    printf("---------------------------------------\n");
    printf("### System Information ###\n");
    printf("System Name = %s\n", uts.sysname); // display System Name
    printf("Machine Name = %s\n", uts.nodename); // display Machine Name
    printf("Version = %s\n", uts.version); // display Version
    printf("Release = %s\n", uts.release); // display Release
    printf("Architecture = %s\n", uts.machine); // display Architecture
    printf("---------------------------------------\n");
}

/**
 * Returns true iff the string s is an integer
 **/
bool isInteger(char *s) {
    int i = 0;
    while (s[i] != '\0') {
        if (isdigit(s[i]) == 0) {
            return false;
        }
        i++;
    }
    return true;
}


/**
 * Parses through arguments entered by user in command line.
 * Returns true iff arguments are entered in correct format.
 **/
bool parseArguments(int argc, char **argv, int *samples, int *tdelay, bool *systemFlagPresent, 
                    bool *userFlagPresent, bool *graphicsFlagPresent) {
    // If flags are specified by user
    if (argc > 1) {

        int i = 1;
        while (i < argc) {
            char *token = strtok(argv[i], "="); // split each argument at "="
            if (strcmp(token, "--samples") == 0) {
                *samples = atoi(strtok(NULL, "")); // store specified # of samples in samples
            }
            else if (strcmp(token, "--tdelay") == 0) {
                *tdelay = atoi(strtok(NULL, "")); // store specified delay in tdelay
            }
            else if (strcmp(argv[i], "--system") == 0) { // if system flag indicated
                *systemFlagPresent = true;
            }
            else if (strcmp(argv[i], "--user") == 0) { // if user flag indicated
                *userFlagPresent = true;
            }
            else if (strcmp(argv[i], "--graphics") == 0) { // if graphics flag indicated
                *graphicsFlagPresent = true;
            }
            // treating as positional argument
            else if (isInteger(argv[i]) && i+1 < argc && isInteger(argv[i+1])) {
                *samples = atoi(argv[i]);
                *tdelay = atoi(argv[i+1]);
                i++;
            }
            else {
                printf("Invalid argument entered!\n");
                return false;
            }

            i++;
        }
    }

    return true;
}

/**
 * Prints final output
 **/
void printReport(int samples, int tdelay, bool systemFlagPresent, 
                 bool userFlagPresent, bool graphicsFlagPresent) {
    printf("Nbr of samples: %d -- every %d secs\n", samples, tdelay); // Display number of samples and delay

    if (systemFlagPresent) { // if system flag indicated
        generateMemoryUsage();
        generateCPUUsage();  
    }
    if (userFlagPresent) { // if user flag indicated
        generateUserUsage();
    }
    if (graphicsFlagPresent) { // if graphics flag indicated
        printf("Graphics\n");   
    }
    if (!systemFlagPresent && !userFlagPresent && !graphicsFlagPresent) { // if no flag indicated
        generateMemoryUsage();
        generateUserUsage();
        generateCPUUsage();
    }
    displaySystemInfo(); // display System Information
}


int main(int argc, char **argv) {
    
    int samples = 10; // default number of samples set to 10
    int tdelay = 1; // default number of seconds set to 1

    bool systemFlagPresent = false; // boolean value holding if --system flag is specified
    bool userFlagPresent = false; // boolean value holding if --user flag is specified
    bool graphicsFlagPresent = false; // boolean value holding if --graphics flag is specified

    bool properArgs = parseArguments(argc, argv, &samples, &tdelay, &systemFlagPresent, &userFlagPresent, &graphicsFlagPresent);
    if (!properArgs) // if arguments incorrectly formatted
        return 1;
    
    printReport(samples, tdelay, systemFlagPresent, userFlagPresent, graphicsFlagPresent);

    return 0;

}
