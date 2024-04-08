## HOW TO RUN THE CODE:

In order to run this program we created a MAKEFILE.
In the terminal go to the directory where the files have been downloaded.
If "make file" is written, the code will be compiled.
Each of the given tests can be run by executing ```make test1```, ```make test2``` and ```make test3``` respectively.
If it is desired to run a Valgrind analysis, the required commands for test 1, test 2 and test3 are: ```make valgrind1```,```make valgrind2``` and ```make valgrind3``` respectively.
The output files after executing the tests can be found in the folder named "Assets".

# Route Lookup Program

This program, `my_route_lookup`, is designed to perform IP route lookup using a binary trie data structure. It reads routing table information from a file and performs lookups for given IP addresses. The program is equipped with several functionalities including trie generation, trie compression, IP address lookup, error handling, and performance evaluation.

## Usage

To compile the program, simply run:

```bash
make compile
```

This will generate an executable named `my_route_lookup`.

### Running Tests

You can execute predefined tests using the following commands:

- Test 1:
  ```bash
  make test1
  ```
- Test 2:
  ```bash
  make test2
  ```
- Test 3:
  ```bash
  make test3
  ```

### Memory Leak Detection (Valgrind)

To check for memory leaks, you can run tests with Valgrind:

- Test 1:
  ```bash
  make valgrind1
  ```
- Test 2:
  ```bash
  make valgrind2
  ```
- Test 3:
  ```bash
  make valgrind3
  ```

## Files

- `my_route_lookup.c`: Contains the main program logic.
- `my_route_lookup.h`: Header file defining function prototypes and constants.
- `io.c`, `io.h`: Input/output operations for file handling.
- `utils.c`, `utils.h`: Utility functions used within the program.

## Compilation Flags

- `-Wall`: Enables most compiler warnings.
- `-O3`: Optimization level 3 for better performance.
- `-lm`: Links the math library.

## Dependencies

This program requires the following files:

- `assets/routing_table.txt`: Routing table information.
- `assets/prueba1.txt`, `assets/prueba2.txt`, `assets/prueba3.txt`: Test files containing IP addresses for lookup.

## Cleaning Up

To remove the compiled executable, run:

```bash
make clean
```

## Author

This program was developed for the RL Lab 2020 Switching UC3M.

## Disclaimer

This program is provided as-is without any guarantees. Use it at your own risk.


- `my_route_lookup.c`: Contains the main program logic.
- `my_route_lookup.h`: Header file defining function prototypes and constants.
- `io.c`, `io.h`: Input/output operations for file handling.
- `
