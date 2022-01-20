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

/*
 * Returns CPU usage as a percentage
 */
float calculateCPUUsage(int *lastTotal, int *lastIdle) {
    FILE *fp = fopen("/proc/stat", "r"); // open /proc/stat file for reading
    char stat_str[1024]; // string to store file content
    fgets(stat_str, 1024, fp); // store file content in stat_str
    fclose(fp); // close /proc/stat file
    int total_time;
    float cpu_usage;

    char *token = strtok(stat_str, " ");
    token = strtok(NULL, " "); // move to next token
    int user = atoi(token);
    token = strtok(NULL, " "); // move to next token
    int nice = atoi(token);
    token = strtok(NULL, " "); // move to next token
    int system = atoi(token);
    token = strtok(NULL, " "); // move to next token
    int idle = atoi(token);
    token = strtok(NULL, " "); // move to next token
    int iowait = atoi(token);
    token = strtok(NULL, " "); // move to next token
    int irq = atoi(token);
    token = strtok(NULL, " "); // move to next token
    int softirq = atoi(token);
    token = strtok(NULL, " "); // move to next token
    int steal = atoi(token);
    token = strtok(NULL, " "); // move to next token
    int guest = atoi(token);
    
    total_time = user + nice + system + idle + iowait + irq + softirq + steal + guest;

    if (*lastTotal == -1 && *lastIdle == -1) { // if calculation is made for the first time (no previous time point)
        cpu_usage = (1 - (idle / total_time)) * 100.0;
        
    } 
    else { // if previous time point exists
        cpu_usage = 100 - (idle - *lastIdle) * 100.0 / (total_time - *lastTotal);;
    }

    *lastTotal = total_time;
    *lastIdle = idle;

    return cpu_usage;

}

typedef struct LinkedListNode {
    char str[1024];
    struct LinkedListNode *next;
} Node;

/*
 * Deletes the link list with head node head
 */
void deleteList(Node *head) {
    Node *p = NULL;
    Node *q = NULL;
    p = head;
    while (p != NULL) {
        q = p->next;
        free(p);
        p = q;
    }
}

/**
 * Prints System Usage without Graphics
 **/
void generateSystemUsage(int samples, int tdelay) {
    printf("Nbr of samples: %d -- every %d secs\n", samples, tdelay); // Display number of samples and delay
    
    // to get memory usage (kilobytes)
    struct rusage r;
    getrusage(RUSAGE_SELF, &r);

    // to get memory report
    struct sysinfo s;
    
    Node *mem_usage_list_head = NULL; // points to head of linked list of Memory usage string per sample
    Node *mem_usage_list_tail = NULL; // points to tail of linked list of Memory usage string per sample
    float total_ram; // holds total ram (GB) in a sample
    float free_ram; // holds free ram (GB) in a sample
    float total_virtual; // holds total virtual memory (GB) in a sample

    int lastTotal = -1, lastIdle = -1;
    
    int i = 0;
    while (i < samples) {
        
        printf("\x1b%d", 7); // save position
        printf(" Memory usage: %ld kilobytes\n", r.ru_maxrss); // TODO: Remove i counter
        printf("---------------------------------------\n");
        printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
        Node *new_sample_mem = (Node *)calloc(1, sizeof(Node)); // new node in linked list for new sample
        // Add new_sample to tail of linked list
        new_sample_mem->next = NULL;
        if (mem_usage_list_head == NULL) {
            mem_usage_list_head = new_sample_mem;
            mem_usage_list_tail = new_sample_mem;
        }
        else {
            mem_usage_list_tail->next = new_sample_mem;
            mem_usage_list_tail = new_sample_mem;
        }
        // Set str for new_sample_mem
        strcpy(new_sample_mem->str, "");
        sysinfo(&s); // fetch memory information
        total_ram = (float)s.totalram/s.mem_unit/1000000000;
        free_ram = (float)s.freeram/s.mem_unit/1000000000;
        total_virtual =(float)s.totalswap/s.mem_unit/1000000000;

        sprintf(new_sample_mem->str, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB", total_ram - free_ram, total_ram, 
                    total_ram - free_ram, total_ram + total_virtual);
        
        // print linked list
        Node *mp = mem_usage_list_head;
        while (mp != NULL) {
            printf("%s\n", mp->str); // print string for sample
            mp = mp->next;
        }
        
        // add extra lines below
        for (int j = 0; j < samples - i - 1; j++)
            printf("\n");

        printf("---------------------------------------\n");
        printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN)); // display number of cores
        float cpu_usage = calculateCPUUsage(&lastTotal, &lastIdle);
        printf(" total cpu use = %.2f%%\n", cpu_usage); // display CPU usage
    
        if (i+1 < samples) { // if not printing last sample
            sleep(tdelay); // delay
            printf("\x1b%d", 8); // go back to saved position
        }
            
        i++;
    }

    deleteList(mem_usage_list_head); // delete memory usage linked list
}

/**
 * Prints System Usage with Graphics
 **/
void generateSystemUsageGraphics(int samples, int tdelay) {
    printf("Nbr of samples: %d -- every %d secs\n", samples, tdelay); // Display number of samples and delay
    
    // to get memory usage (kilobytes)
    struct rusage r;
    getrusage(RUSAGE_SELF, &r);

    // to get memory report
    struct sysinfo s;
    
    Node *mem_usage_list_head = NULL; // points to head of linked list of Memory usage string per sample
    Node *mem_usage_list_tail = NULL; // points to tail of linked list of Memory usage string per sample
    float total_ram; // holds total ram (GB) in a sample
    float free_ram; // holds free ram (GB) in a sample
    float total_virtual; // holds total virtual memory (GB) in a sample
    //float free_virtual; // holds free virtual memory (GB) in a sample

    Node *cpu_usage_list_head = NULL; // points to head of linked list of CPU Usage bars per sample
    Node *cpu_usage_list_tail = NULL; // points to tail of linked list of CPU Usage bars per sample
    int lastTotal = -1, lastIdle = -1;
    
    int i = 0;
    while (i < samples) {
        
        printf("\x1b%d", 7); // save position
        printf(" Memory usage: %ld kilobytes\n", r.ru_maxrss); // TODO: Remove i counter
        printf("---------------------------------------\n");
        printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
        Node *new_sample_mem = (Node *)calloc(1, sizeof(Node)); // new node in linked list for new sample
        // Add new_sample to tail of linked list
        new_sample_mem->next = NULL;
        if (mem_usage_list_head == NULL) {
            mem_usage_list_head = new_sample_mem;
            mem_usage_list_tail = new_sample_mem;
        }
        else {
            mem_usage_list_tail->next = new_sample_mem;
            mem_usage_list_tail = new_sample_mem;
        }
        // Set str for new_sample_mem
        strcpy(new_sample_mem->str, "");
        sysinfo(&s); // fetch memory information
        total_ram = (float)s.totalram/s.mem_unit/1000000000;
        free_ram = (float)s.freeram/s.mem_unit/1000000000;
        total_virtual =(float)s.totalswap/s.mem_unit/1000000000;
        //free_virtual =(float)s.freeswap/s.mem_unit/1000000000;
        sprintf(new_sample_mem->str, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB", total_ram - free_ram, total_ram, 
                    total_ram - free_ram, total_ram + total_virtual);
        
        // print linked list
        Node *mp = mem_usage_list_head;
        while (mp != NULL) {
            printf("%s\n", mp->str); // print string for sample
            mp = mp->next;
        }
        
        // add extra lines below
        for (int j = 0; j < samples - i - 1; j++)
            printf("\n");

        printf("---------------------------------------\n");
        printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN)); // display number of cores
        float cpu_usage = calculateCPUUsage(&lastTotal, &lastIdle);
        printf(" total cpu use = %.2f%%\n", cpu_usage); // display CPU usage
    
        // Graphics for CPU Usage
        Node *new_sample = (Node *)calloc(1, sizeof(Node)); // new node in linked list for new sample
        // Add new_sample to tail of linked list
        new_sample->next = NULL;
        if (cpu_usage_list_head == NULL) {
            cpu_usage_list_head = new_sample;
            cpu_usage_list_tail = new_sample;
        }
        else {
            cpu_usage_list_tail->next = new_sample;
            cpu_usage_list_tail = new_sample;
        }
        // Set str for new_sample (string of bars)
        strcpy(new_sample->str, "");
        if (cpu_usage < 1)
            strcat(new_sample->str, "*");
        else {
            for (int b = 0; b < cpu_usage; b++) {
                strcat(new_sample->str, "|");
            }
        }
        
        char cpu_usage_str[100];
        sprintf(cpu_usage_str, " %.2f%%", cpu_usage);
        strcat(new_sample->str, cpu_usage_str);

        // print linked list
        Node *p = cpu_usage_list_head;
        while (p != NULL) {
            printf("\t%s\n", p->str); // print bars
            p = p->next;
        }
        
        // add extra lines below
        for (int j = 0; j < samples - i - 1; j++)
            printf("\n");

        if (i+1 < samples) { // if not printing last sample
            sleep(tdelay); // delay
            printf("\x1b%d", 8); // go back to saved position
        }
            
        i++;
    }

    deleteList(mem_usage_list_head); // delete memory usage linked list
    deleteList(cpu_usage_list_head); // delete cpu usage linked list
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
            else if ((strcmp(argv[i], "--graphics") == 0) ||  (strcmp(argv[i], "-g") == 0)) { // if graphics flag indicated
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
    if (systemFlagPresent && !graphicsFlagPresent) { // if system flag indicated without graphics
        generateSystemUsage(samples, tdelay);
    }
    if (systemFlagPresent && graphicsFlagPresent) { // if system and graphics flag indicated
        generateSystemUsage(samples, tdelay);
    }
    if (userFlagPresent) { // if user flag indicated
        generateUserUsage();
    }
    if (!systemFlagPresent && !userFlagPresent && !graphicsFlagPresent) { // if no flag indicated
        generateSystemUsage(samples, tdelay);
        generateUserUsage();
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
