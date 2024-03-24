#include <stdio.h>
#include <time.h>
#include "io.h"


/***********************************************************************
 * Constant definitions
 ***********************************************************************/
#define NOT_ENOUGH_ARGUMENTS -3007
#define TOO_MANY_ARGUMENTS -3008
#define MEMORY_ALLOCATION_FAILED -3009



/***********************************************************************
 * Struct for the nodes of the trie
 ***********************************************************************/
struct Node {
	int *bitPosition;
	int *outInterface;
	struct Node *leftSon;
	struct Node *rightSon;
};


/***********************************************************************
 * Compress the trie to get the Patricia trie (or compressed trie) given
 * the root of the trie
 ***********************************************************************/
void compressTrie(struct Node *root);


/***********************************************************************
 * Create the node with the given output interface
 ***********************************************************************/
struct Node *createNode(int *bitPosition, int *outInterface, struct Node *root);



/***********************************************************************
 * Free the memory allocated for a node
 ***********************************************************************/
void freeNode(struct Node *node);


/***********************************************************************
 * Free the memory allocated for the complete trie
 ***********************************************************************/
void freeTrie(struct Node *root);


/***********************************************************************
 * Generate the trie given the root node
 ***********************************************************************/
void generateTrie(struct Node *root);


/***********************************************************************
 * Look up for the output interface of a given IP address
 ***********************************************************************/
void lookup(uint32_t *IPAddress, int *numberOfAccesses, int *outInterface, struct Node *root);


/***********************************************************************
 * Write explanation for error identifier
 ***********************************************************************/
void printErrors(int result);


/***********************************************************************
 * Traverse the trie and count the number of nodes in it
 ***********************************************************************/
void traverseTrie(int *NumberOfNodesInTrie, struct Node *root);


/***********************************************************************
 * Main function
 ***********************************************************************/
void main(int argc, char *argv[]);
