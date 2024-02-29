#ifndef MY_ROUTE_LOOKUP_H
#define MY_ROUTE_LOOKUP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "io.h"
#include "utils.h"

/********************************************************************
 * Constant definitions
 ********************************************************************/
#define MEMORY_ALLOCATION_FAILED -3007

typedef struct Node {
    int *outInterface;
    struct Node *leftSon;
    struct Node *rightSon;
} Node;

void decToBin(uint32_t prefix, int **binaryArray);
Node *createNode(int *outInterface, int *result);
int generateTrie(Node *root, int **binaryArray);
void traverseTrie(Node *node, int *NumberOfNodesInTrie);
void compressTrie(Node *node);

#endif /* MY_ROUTE_LOOKUP_H */

