# mySystemStats
### Approach
###### Step 1: Gathering Information
Reading through the documentation for the auxiliary C libraries listed in the assignment handout, I determined which functions I can use to gather all the required information.
 - To get the total memory usage (in kB), I used the `ru_maxrss` field from the `rusage` struct defined in `sys/resource.h`. Getting this information required using the `getrusage()` function.
 - To get the amount of memory used (physical and virtual), I used the `sysinfo` struct defined in `sys/sysinfo.h`. Getting this information required using the `sysinfo()` function. The following fields from the `sysinfo` struct were useful for the purposes of this program:
    - `totalram`: the total usable main memory size
    - `freeram`: the available memory space
    - `totalswap`: the total swap space size
    - `freeswap`: the size of the swap space still available
    - `mem_unit`: the memory unit size in bytes
- To get the number of CPU cores, I called the `sysconf(_SC_NPROCESSORS_ONLN)`function from `unistd.h`.
- To get the total CPU usage, I read through the `/proc/stat` file and performed the calculations outlined in [Step 2](https://github.com/AB20CS/mySystemStats/blob/main/README.md#step-2-calculations).
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

###### Step 2: Calculations
The next step involved understanding how to calculate the CPU and memory usage.

**CPU Usage:**

Calculating CPU usage requires two time points. 

![image](http://www.sciweavers.org/upload/Tex2Img_1642781254/render.png)

Note: the subscript 1 denotes the value at the first time point and the subscript 2 denotes the value at the second time point.
The total time can be calculated be summing all the values in the first line of the `/proc/stat` file and the idle time is the fourth number on the first line of the `/proc/stat` file.

**Memory Usage:**

The physical memory consists of RAM and the virtual memory consists of the RAM and the swap space. The values are reported based on this distinction.

###### Step 3: Data Storage
I decided to store the data collected at each time point using linked lists - one linked list for memory usage and another for CPU usage. Each timepoint is represented as a node in the linked list and each node contains a string containing all the relevant information for that time point. I had also created a struct called `UsageInfoLL` which stores pointers to the head and tail of each linked list.

###### Step 4: Parsing Through User Input
I iterated through `argv` to get access to the command line arguments entered by the user. Since some flags have a `=` sign in the middle, I used `strtok()` from `string.h` to split each argument at `=`. This way, we can read the flag name and the value inputted (if applicable separately). If the the string after the `=` cannot be converted to an integer, an error message will appear and the program will terminate. To decide which flags have been inputted by the user, I used `strcmp()` from `string.h` and used boolean variables to store whether or not each flag has been inputted. Depending on which combination of flags have been inputted, the program prints the relevant information (through a series of `if`/`else` statements.

###### Step 5: Printing the Report
To ensure that the output is refreshed at every time point, before taking each sample, I saved the position of the cursor using the `\x1b7` escape code. After printing out the relevant information using the functions described [below](https://github.com/AB20CS/mySystemStats#function-overview), I used the `\x1b8` escape code to allow for the cursor to return to previously saved position. In the next iteration, the previous output is overwritten. In this way, the output refreshes at every time point.

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
 
 
### Understanding Graphics
This section applies if the user inputs the graphics flag (i.e., `--graphics` or `-g`). 
 
**Memory Usage Graphics:**
If the amount of virtual memory used is positive, the `#` symbol will be printed for every 0.01 GB used. Beside this graphic is an expression of the form `swapSpaceUsed (totalMemoryUsed)`, where both values are expressed in GB.
 
_Sample Graphics:_
```
### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)
9.25 GB / 16.50 GB -- 9.25 GB / 17.53 GB        |* 0.00 (9.25)
9.25 GB / 16.50 GB -- 9.25 GB / 17.53 GB        |#* 0.01 (9.25)
9.25 GB / 16.50 GB -- 9.25 GB / 17.53 GB        |###* 0.03 (9.25)
9.25 GB / 16.50 GB -- 9.25 GB / 17.53 GB        |* 0.00 (9.25)
9.25 GB / 16.50 GB -- 9.25 GB / 17.53 GB        |* 0.00 (9.25)
```
 
**CPU Usage Graphics:**
If the CPU usage is 0%, the `*` symbol is printed. If the CPU usage is positive, a `|` is printed for every percent of usage (rounded up to the nearest percent). The CPU usage is printed beside the graphic. 
 
 _Sample Graphics:_
 ```
 Number of cores: 4
 total cpu use = 20.91%
        * 0.00%
        |||||||||||||||||||| 19.44%
        |||||||||||||||||||||||||| 25.38%
        |||||||||||||||||||||||||||||||||||||||| 39.70%
        |||||||||||||||||||||||||||||||||||||||| 39.90%
        |||||||||||||||||||||||||||||||||||| 35.34%
        ||||||||||||||||||||||||||||||||||| 34.41%
        ||||||||||||||||||| 18.16%
        |||||||||||||||||||||| 22.00%
        ||||||||||||||||||||| 20.91%
 ```
 
 
 
 
 
