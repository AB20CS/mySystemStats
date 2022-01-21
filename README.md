# mySystemStats
### Approach


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
      | Flag                | Description                                                                                                                                |
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
