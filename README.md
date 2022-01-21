# mySystemStats
### Approach


### Function Overview
| Function | Description|
| --- | --- |
| `void printReport(int samples, int tdelay, bool systemFlagPresent, bool userFlagPresent, bool graphicsFlagPresent)` | Prints final output |
| `bool parseArguments(int argc, char **argv, int *samples, int *tdelay, bool *systemFlagPresent, bool *userFlagPresent, bool *graphicsFlagPresent)` | Parses through command line arguments to determine which flags have been entered. Returns true iff arguments are entered in correct format.|
| `void generateSystemUsage(int samples, int tdelay, UsageInfoLL *usageInfo, int i)` | Display system usage (i.e., memory and CPU usage) without graphics |
| `void generateSystemUsageGraphics(int samples, int tdelay, UsageInfoLL *usageInfo, int i)` | Display system usage (i.e., memory and CPU usage) with graphics |
| `void generateUserUsage()` | Display users usage |

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
