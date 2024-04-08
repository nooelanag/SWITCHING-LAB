#include <stdio.h>
#include <time.h>
#include "io.h"


/***********************************************************************
 * Constant definitions
 ***********************************************************************/
#define NOT_ENOUGH_ARGUMENTS -3007
#define TOO_MANY_ARGUMENTS -3008
#define MEMORY_ALLOCATION_FAILED -3009
#define NOT_VALID_PREFIX_LENGTH -3010
#define NOT_VALID_POSITION -3011
#define MATCH 3000
#define NOT_MATCH 3001


/***********************************************************************
 * Struct for the nodes of the trie
 ***********************************************************************/
struct Node {
	int *bitPosition;
	char *prefixString;
	int *outInterface;
	struct Node *leftSon;
	struct Node *rightSon;
};


int addressPrefixComparison(const uint32_t IPAddress, const char *prefixString, int *result);


int bitIPAddress(const uint32_t IPAddress, const int position, int *result);


void compressTrie(struct Node *node);


struct Node *createNode(const int *bitPosition, char *prefixString, const int *outInterface, int *result);


void freeNode(struct Node *node);


void freeTrie(struct Node *node);


struct Node *generateTrie(int *result);


int lookup(const uint32_t IPAddress, int *outInterface, int *result, struct Node *root);


void printErrors(const int result);


int traverseTrie(struct Node *node);


char *uint32ToString(const uint32_t IPAddress, const int prefixLength, int *result);


int main(int argc, char *argv[]);
