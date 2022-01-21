# mySystemStats
### Approach
###### Step 1
Reading through the documentation for the auxiliary C libraries listed in the assignment handout, I determined which functions I can use to gather all the required information.
 - To get the total memory usage (in kB), I used the `ru_maxrss` field from the `rusage` struct defined in `sys/resource.h'. Getting this information required using the `getrusage()` function.
 - To get the amount of memory used (physical and virtual), I used the `sysinfo` struct defined in `sys/sysinfo.h`. Getting this information required using the `sysinfo()` function. The following fields from the `sysinfo` struct were useful for the purposes of this program:
    - `totalram`: the total usable main memory size
    - `freeram`: the available memory space
    - `totalswap`: the total swap space size
    - `freeswap`: the size of the swap space still available
    - `mem_unit`: the memory unit size in bytes
- To get the number of CPU cores, I called the `sysconf(_SC_NPROCESSORS_ONLN)`function from `unistd.h`.
- To get the total CPU usage, I read through the `/proc/stat` file and performed the calculations outlined in Step 2.
- To get the user usage information, I used the `utmp` struct defined in `utmp.h`. Getting this information required using the `getutent()` function. The following fields in the struct were useful for this program:
    - `ut_user`: the user log-in name
    - `ut_line`: the device name
    - `host`: the host name
- To get the system usage information, I used the `utsname` struct defined in `<sys/utsname.h>`. Getting this information required using the `uname()` function. The following fields in the struct were useful for this program:
    - `sysname`: the system name
    - `nodename`: the machine name
    - `version`: the version of the operating system
    - `release`: the release of the operating system
    - `machine`: the machine's architecture

###### Step 2
The next step involved understanding how to calculate the CPU and memory usage.

**CPU Usage:**

Calculating CPU usage requires two time points. 

<img src="http://www.sciweavers.org/upload/Tex2Img_1642781254/render.png" align="center" border="0" alt="\text{CPU usage} = (1 - \frac{\text{idle}_2 - \text{idle}_1}{\text{total time}_2 - \text{total time}_1}) \times 100\%" width="431" height="46" />
Note: the subscript 1 denotes the value at the first time point and the subscript 2 denotes the value at the second time point.
The total time can be calculated be summing all the values in the first line of the `/proc/stat` file and the idle time is the fourth number on the first line of the `/proc/stat` file.

**Memory Usage:**

The physical memory consists of RAM and the virtual memory consists of the RAM and the swap space. The values are reported based on this distinction.

###### Step 3


### Function Overview
| Function | Description|
| --- | --- |
| `void printReport(int samples, int tdelay, bool systemFlagPresent, bool userFlagPresent, bool graphicsFlagPresent)` | Prints final output. `samples` is the number times statistics will be collected and `tdelay` is the frequency of statistic collection. `systemFlagPresent`, `userFlagPresent` and `graphicsFlagPresent` hold true iff the `system` flag, `user` flag and `graphics` flag are inputted by the user, respectively. |
| `bool parseArguments(int argc, char **argv, int *samples, int *tdelay, bool *systemFlagPresent, bool *userFlagPresent, bool *graphicsFlagPresent)` | Parses through command line arguments to determine which flags have been entered. Returns true iff arguments are entered in correct format. `argc` is the number of command line arguments entered and `argv` is an array of strings that holds the command line arguments entered. `samples` points to the number times statistics will be collected and `tdelay` points to the frequency of statistic collection. `systemFlagPresent`, `userFlagPresent` and `graphicsFlagPresent` each point to a boolean variables that holds true iff the `system` flag, `user` flag and `graphics` flag are inputted by the user, respectively.|
| `void generateSystemUsage(int samples, int tdelay, UsageInfoLL *usageInfo, int i)` | Displays system usage (i.e., memory and CPU usage) without graphics. `samples` is the number times statistics will be collected and `tdelay` is the frequency of statistic collection. `usageInfo` is a struct that holds pointers to the linked lists holding memory/CPU usage information. `i` indicates the number times statistics will have been collected by the end of the current cycle. |
| `void generateSystemUsageGraphics(int samples, int tdelay, UsageInfoLL *usageInfo, int i)` | Displays system usage (i.e., memory and CPU usage) with graphics. `samples` is the number times statistics will be collected and `tdelay` is the frequency of statistic collection. `usageInfo` is a struct that holds pointers to the linked lists holding memory/CPU usage information. `i` indicates the number times statistics will have been collected by the end of the current cycle. |
| `void generateUserUsage()` | Display users usage. |
| `float calculateCPUUsage(int *lastTotal, int *lastIdle)` | Returns CPU usage as a percentage. `lastTotal` and `lastIdle` point to the total uptime and total idle time at the previous time point, respectively. |
| `bool isInteger(char *s)` | Returns true iff the string s is an integer. |
| `void deleteList(Node *head)` | Deletes a linked list whose head node is pointed to by `head`. |

### Running the Program
  1. Navigate to the directory (i.e., `cd`) in which `mySystemStats.c` is saved on your machine.
  2. In the terminal, enter `gcc -Wall -Werror mySystemStats.c -o mySystemStats` to compile the program.
  3. To execute the program, enter `./mySystemStats` into the terminal. You may also use the following flags when executing:
      | Flag                | Description                                                                                                                             |
      | ------------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
      | `--system`          | to print system usage report                                                                                                            |
      | `--user`            | to print users usage report                                                                                                             |
      | `--graphics`, `-g`  | to include graphical output in system usage report                                                                                      |
      | `--samples=N`       | to indicate that the system statistics will be collected `N` times, where `N` is a positive integer _(if this flag is not used, the default value is                               10)_.               |
      | `--tdelay=T`        | to indicate that the system statistics will be collected every `T` seconds, where `T` is a positive integer _(if this flag is not used, the default                                 value is 1 second)_. |
      
      
      - For example, `./mySystemStats --system -g --samples=5 --tdelay=2` will print the system usage report with graphics and will collect statistics every 2 seconds for a total         of 5 time points.
      - The number of samples and frequency can also be inputted as positional arguments (i.e., as two adjacent numbers separated by a space, where the first number is the             number of samples and the second number is the frequency).
          - These positional arguments can be located anywhere along the input as long as the two numbers are adjacent.
          - In this implementation, having just one of the numbers (e.g., `./mySystemStats 5`) is invalid.
          - For example, `./mySystemStats --user 5 2`  and `./mySystemStats 5 2 --system` will both print the system usage report and will collect statistics every 2                           seconds for a total of 5 time points.
      - `./mySystemStats` is equivalent to `./mySystemStats --system --user --samples=10 --tdelay=1`.
  4. If `Invalid argument entered!` is printed on the screen after executing, refer back to flags outline in _Step 3_ and repeat the above steps.
