#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <utmp.h>
#include <unistd.h>

void generateSystemUsage() {
    printf("---------------------------------------\n");
    printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN)); // display number of cores

}

void generateUserUsage() {
    printf("---------------------------------------\n");
    printf("### Sessions/users ### \n");

    struct utmp *ut;

    setutent(); // Go to start of user information
    ut = getutent(); // Reads line from current position (i.e., user information)

    while (ut) {
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
    
    // Checking if samples is indicated by user
    

    // Checking if tdelay is indicated by user


    printf("Nbr of samples: %d -- every %d secs\n", samples, tdelay); // Display number of samples and delay

    // Choosing type of output
    if (strcmp(argv[1], "--system") == 0) {
        generateSystemUsage();
        
    }
    else if (strcmp(argv[1], "--user") == 0) {
        generateUserUsage();
        
    }
    else if (strcmp(argv[1], "--graphics") == 0) {
        printf("Graphics\n");
        
    }

    displaySystemInfo(); // display System Information
    return 0;

}
