# Route Lookup Program

This program is designed to perform IP route lookup using a binary trie data structure. It reads routing table information from a file and performs lookups for given IP addresses. The program is equipped with several functionalities including trie generation, trie compression, IP address lookup, error handling, and performance evaluation.

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

## Dependencies

This program requires the following files:

- `assets/routing_table.txt`: Routing table information.
- `assets/prueba1.txt`, `assets/prueba2.txt`, `assets/prueba3.txt`: Test files containing IP addresses for lookup.

## Cleaning Up

To remove the compiled executable, run:

```bash
make clean
```

## Authors

Noel Andolz Aguado y Daniel Lozano Uceda
