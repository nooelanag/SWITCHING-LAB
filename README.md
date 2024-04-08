# Route Lookup Program

This program is designed to perform IP route lookup using the Patricia trie algorithm. It reads routing table information from a file and performs lookups for given IP addresses. The program is equipped with several functionalities including trie generation, trie compression, IP address lookup, error handling, and performance evaluation.

## Usage

To compile the program, simply run:

```bash
make compile
```

This will generate an executable named `my_route_lookup`.

### Running Tests

You can execute predefined tests using the following commands:

- Prueba 1:
  ```bash
  make test1
  ```
- Prueba 2:
  ```bash
  make test2
  ```
- Prueba 3:
  ```bash
  make test3
  ```

### Memory Leak Detection (Valgrind)

To check for memory leaks, you can run tests with Valgrind:

- Prueba 1:
  ```bash
  make valgrind1
  ```
- Prueba 2:
  ```bash
  make valgrind2
  ```
- Prueba 3:
  ```bash
  make valgrind3
  ```

## Files

- `my_route_lookup.c`: Contains the main program logic.
- `my_route_lookup.h`: Header file defining function prototypes and constants.

## Dependencies

This program requires the following files:

- `io.c`, `io.h`: Input/output operations for file handling.
- `utils.c`, `utils.h`: Utility functions used within the program.
- `assets/routing_table.txt`, `linearSearch/routing_table_simple.txt`: Routing tables information.
- `linearSearch/prueba0.txt`, `assets/prueba1.txt`, `assets/prueba2.txt`, `assets/prueba3.txt`: Test files containing IP addresses for lookup.
- `linear_search/linear_search`: Executable that implements the linear search algorithm and will be used as a reference to
compare with our solution.


## Cleaning Up

To remove the compiled executable, run:

```bash
make clean
```

## Authors

Noel Andolz Aguado (100451366) y Daniel Lozano Uceda (100451550)
