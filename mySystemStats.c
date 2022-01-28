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

// struct definition for linked list node
typedef struct LinkedListNode {
    char str[1024];
    struct LinkedListNode *next;
} Node;

// struct definition for struct to hold linked lists with usage information
typedef struct UsageInformationLinkedLists {
    Node *mem_usage_list_head; // points to head of linked list of Memory usage string per sample
    Node *mem_usage_list_tail; // points to tail of linked list of Memory usage string per sample

    Node *cpu_usage_list_head; // points to head of linked list of CPU Usage bars per sample
    Node *cpu_usage_list_tail; // points to tail of linked list of CPU Usage bars per sample

    float last_mem_usage; // only used for memory graphics (holds total memory usage from last sample)

    // for CPU usage calculations
    int lastTotal; // total uptime at last time point
    int lastIdle; // total idle time at last time point
} UsageInfoLL;

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
 * Prints Memory Usage without Graphics
 **/
void generateMemoryUsage(int samples, int tdelay, UsageInfoLL *usageInfo, int i) {
    
    // to get memory usage (kilobytes)
    struct rusage r;

    // to get memory report
    struct sysinfo s;
    
    float total_ram; // holds total ram (GB) in a sample
    float free_ram; // holds free ram (GB) in a sample
    float total_swap; // holds total virtual memory (GB) in a sample
    float free_swap; // holds free virtual memory (GB) in a sample
    float swap_used; // holds swap space used
    float phys_mem_used; // holds physical memory used (GB)
    float virtual_mem_used; // holds virtual memory used (GB)
    float total_memory; // total_ram + total_virtual
    
    getrusage(RUSAGE_SELF, &r); // fetch memory usage
    printf(" Memory usage: %ld kilobytes\n", r.ru_maxrss);
    printf("---------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    Node *new_sample_mem = (Node *)calloc(1, sizeof(Node)); // new node in linked list for new sample
    // Add new_sample to tail of linked list
    new_sample_mem->next = NULL;
    if (usageInfo->mem_usage_list_head == NULL) {
        usageInfo->mem_usage_list_head = new_sample_mem;
        usageInfo->mem_usage_list_tail = new_sample_mem;
    }
    else {
        usageInfo->mem_usage_list_tail->next = new_sample_mem;
        usageInfo->mem_usage_list_tail = new_sample_mem;
    }
    // Set str for new_sample_mem
    strcpy(new_sample_mem->str, "");
    sysinfo(&s); // fetch memory information
    total_ram = (float)s.totalram/s.mem_unit/1000000000;
    free_ram = (float)s.freeram/s.mem_unit/1000000000;
    total_swap =(float)s.totalswap/s.mem_unit/1000000000;
    free_swap =(float)s.freeswap/s.mem_unit/1000000000;
    
    total_memory = total_ram + total_swap;
    phys_mem_used = total_ram - free_ram;
    swap_used = total_swap - free_swap;
    virtual_mem_used = phys_mem_used + swap_used;

    sprintf(new_sample_mem->str, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB", phys_mem_used, total_ram, 
                virtual_mem_used, total_memory);
    
    // print linked list
    Node *mp = usageInfo->mem_usage_list_head;
    while (mp != NULL) {
        printf("%s\n", mp->str); // print string for sample
        mp = mp->next;
    }
    
    // add extra lines below
    for (int j = 0; j < samples - i - 1; j++)
        printf("\n");

}

/**
 * Prints CPU Usage without Graphics
 **/
void generateCPUUsage(int samples, int tdelay, UsageInfoLL *usageInfo, int i) {
    
    printf("---------------------------------------\n");
    printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN)); // display number of cores
    float cpu_usage = calculateCPUUsage(&(usageInfo->lastTotal), &(usageInfo->lastIdle));
    printf(" total cpu use = %.2f%%\n", cpu_usage); // display CPU usage

}

/**
 * Prints Memory Usage with Graphics
 **/
void generateMemoryUsageGraphics(int samples, int tdelay, UsageInfoLL *usageInfo, int i) {
    
    // to get memory usage (kilobytes)
    struct rusage r;

    // to get memory report
    struct sysinfo s;
    
    float total_ram; // holds total ram (GB) in a sample
    float free_ram; // holds free ram (GB) in a sample
    float total_swap; // holds total virtual memory (GB) in a sample
    float free_swap; // holds free virtual memory (GB) in a sample
    float swap_used; // holds swap space used
    float phys_mem_used; // holds physical memory used (GB)
    float virtual_mem_used; // holds virtual memory used (GB)
    float total_memory; // total_ram + total_virtual

    float delta_mem_use; // holds relative change in memory usage from last sample (as a decimal)
    float rounded_delta_mem_use; // holds delta_mem_use rounded to 2 decimal places

    getrusage(RUSAGE_SELF, &r); // fetch memory usage
    printf(" Memory usage: %ld kilobytes\n", r.ru_maxrss);
    printf("---------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    Node *new_sample_mem = (Node *)calloc(1, sizeof(Node)); // new node in linked list for new sample
    
    // Add new_sample to tail of linked list
    new_sample_mem->next = NULL;
    if (usageInfo->mem_usage_list_head == NULL) {
        usageInfo->mem_usage_list_head = new_sample_mem;
        usageInfo->mem_usage_list_tail = new_sample_mem;
    }
    else {
        usageInfo->mem_usage_list_tail->next = new_sample_mem;
        usageInfo->mem_usage_list_tail = new_sample_mem;
    }
    // Set str for new_sample_mem
    strcpy(new_sample_mem->str, "");
    sysinfo(&s); // fetch memory information
    total_ram = (float)s.totalram/s.mem_unit/1000000000;
    free_ram = (float)s.freeram/s.mem_unit/1000000000;
    total_swap =(float)s.totalswap/s.mem_unit/1000000000;
    free_swap =(float)s.freeswap/s.mem_unit/1000000000;
    
    total_memory = total_ram + total_swap;
    phys_mem_used = total_ram - free_ram;
    swap_used = total_swap - free_swap;
    virtual_mem_used = phys_mem_used + swap_used;

    sprintf(new_sample_mem->str, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB\t|", phys_mem_used, total_ram, 
                virtual_mem_used, total_memory);

    // Concatenate graphics
    if (usageInfo->last_mem_usage == -1) { // if no previous sample taken
        delta_mem_use = 0; // zero change (since it is first sample)
        strcat(new_sample_mem->str, "o");
    }
    else { // if previous sample is taken
        delta_mem_use = (virtual_mem_used - usageInfo->last_mem_usage) / (usageInfo->last_mem_usage);
        char temp_rounded[6];
        sprintf(temp_rounded, "%.2f", delta_mem_use);
        rounded_delta_mem_use = atof(temp_rounded);

        // zero+
        if (rounded_delta_mem_use == 0.00 && delta_mem_use > 0) {
            strcat(new_sample_mem->str, "o");
        }
        // zero-
        else if (rounded_delta_mem_use == 0.00 && delta_mem_use < 0) {
            strcat(new_sample_mem->str, "@");
        }
        // positive change in memory usage
        else if (rounded_delta_mem_use > 0) {
            for (float i = 0; i < delta_mem_use; i += 0.01) {
                strcat(new_sample_mem->str, "#");
            }
            strcat(new_sample_mem->str, "*");
        }
        // negative change in memory usage
        else if (rounded_delta_mem_use < 0) {
            for (float i = 0; i < -delta_mem_use; i += 0.01) {
                strcat(new_sample_mem->str, ":");
            }
            strcat(new_sample_mem->str, "@");
        }
        
    }
    char temp_str[100];
    sprintf(temp_str, " %.2f (%.2f)", delta_mem_use, virtual_mem_used);
    strcat(new_sample_mem->str, temp_str);
    
    usageInfo->last_mem_usage = virtual_mem_used; // update last_mem_usage

    // print linked list
    Node *mp = usageInfo->mem_usage_list_head;
    while (mp != NULL) {
        printf("%s\n", mp->str); // print string for sample
        mp = mp->next;
    }
    
    // add extra lines below
    for (int j = 0; j < samples - i - 1; j++)
        printf("\n");
}

/**
 * Prints CPU Usage with Graphics
 **/
void generateCPUUsageGraphics(int samples, int tdelay, UsageInfoLL *usageInfo, int i) {

    printf("---------------------------------------\n");
    printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN)); // display number of cores
    float cpu_usage = calculateCPUUsage(&(usageInfo->lastTotal), &(usageInfo->lastIdle));
    printf(" total cpu use = %.2f%%\n", cpu_usage); // display CPU usage

    // Graphics for CPU Usage
    Node *new_sample = (Node *)calloc(1, sizeof(Node)); // new node in linked list for new sample
    // Add new_sample to tail of linked list
    new_sample->next = NULL;
    if (usageInfo->cpu_usage_list_head == NULL) {
        usageInfo->cpu_usage_list_head = new_sample;
        usageInfo->cpu_usage_list_tail = new_sample;
    }
    else {
        usageInfo->cpu_usage_list_tail->next = new_sample;
        usageInfo->cpu_usage_list_tail = new_sample;
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
    Node *p = usageInfo->cpu_usage_list_head;
    while (p != NULL) {
        printf("\t%s\n", p->str); // print bars
        p = p->next;
    }
    
    // add extra lines below
    for (int j = 0; j < samples - i - 1; j++)
        printf("\n");
        
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
    
    bool samplesSpecified = false; // holds true iff user specifies the number of samples
    bool tdelaySpecified = false; // holds true iff user specifies the value for tdelay
    
    // If flags are specified by user
    if (argc > 1) {

        int i = 1;
        while (i < argc) {
            char *token = strtok(argv[i], "="); // split each argument at "="
            if (strcmp(token, "--samples") == 0) {
                *samples = atoi(strtok(NULL, "")); // store specified # of samples in samples
                samplesSpecified = true;
            }
            else if (strcmp(token, "--tdelay") == 0) {
                *tdelay = atoi(strtok(NULL, "")); // store specified delay in tdelay
                tdelaySpecified = true;
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
            else if (isInteger(argv[i]) && i+1 < argc && isInteger(argv[i+1]) && !samplesSpecified && !tdelaySpecified) {
                *samples = atoi(argv[i]);
                *tdelay = atoi(argv[i+1]);
                samplesSpecified = true;
                tdelaySpecified = true;
                i++;
            }
            else if (isInteger(argv[i]) && !samplesSpecified) { // single numerical value entered
                *samples = atoi(argv[i]);
                samplesSpecified = true;
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
    
    UsageInfoLL *usageInfo = (UsageInfoLL *)calloc(1, sizeof(UsageInfoLL)); // allocate struct to hold lists with usage info

    usageInfo->last_mem_usage = -1; // initialize last_mem_usage to -1 to indicate no previous sample taken

    // Initialize time=0 points for CPU usage calculations
    usageInfo->lastTotal = -1;
    usageInfo->lastIdle = -1;

    int i = 0;
    while (i < samples) {
        printf("\x1b%d", 7); // save position

        if (systemFlagPresent && !graphicsFlagPresent) { // if system flag indicated without graphics
            generateMemoryUsage(samples, tdelay, usageInfo, i);
            generateCPUUsage(samples, tdelay, usageInfo, i);
        }
        if (systemFlagPresent && graphicsFlagPresent) { // if system and graphics flag indicated
            generateMemoryUsageGraphics(samples, tdelay, usageInfo, i);
            generateCPUUsageGraphics(samples, tdelay, usageInfo, i);
        }
        if (userFlagPresent) { // if user flag indicated
            generateUserUsage();
        }
        if (!systemFlagPresent && !userFlagPresent && !graphicsFlagPresent) { // if no flag indicated
            generateMemoryUsage(samples, tdelay, usageInfo, i);
            generateCPUUsage(samples, tdelay, usageInfo, i);
            generateUserUsage();
        }
        if (!systemFlagPresent && !userFlagPresent && graphicsFlagPresent) { // if no flag indicated except graphics
            generateMemoryUsageGraphics(samples, tdelay, usageInfo, i);
            generateCPUUsageGraphics(samples, tdelay, usageInfo, i);
            generateUserUsage();
        }

        if (i+1 < samples) { // if not printing last sample
            sleep(tdelay); // delay
            printf("\x1b%d", 8); // go back to saved position
        }
        i++;
    }
    
    deleteList(usageInfo->mem_usage_list_head); // delete memory usage linked list
    deleteList(usageInfo->cpu_usage_list_head); // delete cpu usage linked list
    free(usageInfo); // free usageInfo

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
