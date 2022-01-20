# mySystemStats
### Approach


### Function Overview

### Running the Program
  1. Navigate to the directory (i.e., `cd`) in which `mySystemStats.c` is saved on your machine.
  2. In the terminal, enter `gcc -Wall -Werror mySystemStats.c -o mySystemStats` to compile the program.
  3. To execute the program, enter `./mySystemStats` into the terminal. You may also use the following flags when executing:
      - `--system`: to print system usage report
      - `--user`: to print users usage report
      - `--graphics` or `-g`: to include graphical output in system usage report
      - `--samples=N`: to indicate that the system statistics will be collected `N` times _(if this flag is not used, the default value is 10)_.
      - `--tdelay=T`: to indicate that the system statistics will be collected every `T` seconds _(if this flag is not used, the default value is 1 second)_.
