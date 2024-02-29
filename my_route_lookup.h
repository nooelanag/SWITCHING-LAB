#ifndef MY_ROUTE_LOOKUP_H
#define MY_ROUTE_LOOKUP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "io.h"
#include "utils.h"

typedef struct Node {
    int *outInterface;
    struct Node *leftSon;
    struct Node *rightSon;
} Node;

void decToBin(uint32_t prefix, int *binaryArray);
Node *createNode(int *outInterface);
void generateTrie(Node *root);
void compressTrie(Node *node);
void lookup(uint32_t *IPAddress, int *nodeAccesses);
void traverseTrie(Node *node, int *NumberOfNodesInTrie);

#endif /* MY_ROUTE_LOOKUP_H */

