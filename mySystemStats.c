#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <unistd.h>

void generateSystemUsage() {
    printf("---------------------------------------\n");
    printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN)); // display number of cores
    
}

void generateUserUsage() {
    printf("User\n");
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
    

    //int samples;
    //int tdelay;
    // Checking if samples is indicated by user
    

    // Checking if tdelay is indicated by user


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
