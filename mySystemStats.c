#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>

void generateMemoryUsage() {
    printf(" Memory usage: ______ kilobytes\n");
    printf("---------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
}

void generateCPUUsage() {
    printf("---------------------------------------\n");
    printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN)); // display number of cores
    
}

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

int main(int argc, char **argv) {
    
    int samples = 10; // default number of samples set to 10
    int tdelay = 1; // default number of seconds set to 1

    bool systemFlagPresent = false; // boolean value holding if --system flag is specified
    bool userFlagPresent = false; // boolean value holding if --user flag is specified
    bool graphicsFlagPresent = false; // boolean value holding if --graphics flag is specified

    // If flags are specified by user
    if (argc > 1) {

        bool tdelayFlagPresent = false; // holds true iff samples flag has been indicated

        for (int i = 1; i < argc; i++) {
            char *token = strtok(argv[i], "="); // split each argument at "="
            if (strcmp(token, "--samples") == 0) {
                if (tdelayFlagPresent) {
                    printf("Improper formatting of arguments! - samples flag must come before tdelay\n");
                    return 1;
                }
                else {
                    samples = atoi(strtok(NULL, "")); // store specified # of samples in samples
                }
            }
            else if (strcmp(token, "--tdelay") == 0) {
                tdelay = atoi(strtok(NULL, "")); // store specified delay in tdelay
                tdelayFlagPresent = true;
            }
            else if (strcmp(argv[1], "--system") == 0) { // if system flag indicated
                systemFlagPresent = true;
            }
            else if (strcmp(argv[1], "--user") == 0) { // if user flag indicated
                userFlagPresent = true;
            }
            else if (strcmp(argv[1], "--graphics") == 0) { // if graphics flag indicated
                graphicsFlagPresent = true;
            }
            else {
                printf("Invalid argument entered!\n");
                return 1;
            }
        }
    }
    
    printf("Nbr of samples: %d -- every %d secs\n", samples, tdelay); // Display number of samples and delay

    if (systemFlagPresent) { // if system flag indicated
        generateMemoryUsage();
        generateCPUUsage();  
    }
    else if (userFlagPresent) { // if user flag indicated
        generateUserUsage();
    }
    else if (graphicsFlagPresent) { // if graphics flag indicated
        printf("Graphics\n");   
    }
    else { // if no flag indicated
        generateMemoryUsage();
        generateUserUsage();
        generateCPUUsage();
    }
    displaySystemInfo(); // display System Information

    return 0;

}
