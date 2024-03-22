#include "io.h"
#include "my_route_lookup.h"


void compressTrie(struct Node *root) {
	if (root != NULL) {
		if (root->outInterface == NULL) {
			if (root->leftSon != NULL && root->rightSon == NULL) {
				struct Node *child = root->leftSon;
				root->outInterface = child->outInterface;
        		root->leftSon = child->leftSon;
        		root->rightSon = child->rightSon;
        		free(child);
				compressTrie(root);
			} else if (root->leftSon == NULL && root->rightSon != NULL) {
				struct Node *child = root->rightSon;
				root->outInterface = child->outInterface;
        		root->leftSon = child->leftSon;
        		root->rightSon = child->rightSon;
        		free(child);
				compressTrie(root);
			}
		}
		
		compressTrie(root->leftSon);
		compressTrie(root->rightSon);
	}
}


struct Node *createNode(int *outInterface, struct Node *root) {
    struct Node *node = malloc(sizeof(struct Node));

    if (node == NULL) {
		freeIO();
		freeTrie(root);
        printErrors(MEMORY_ALLOCATION_FAILED);
        exit(1);
    }

    if (outInterface != NULL) {
        node->outInterface = malloc(sizeof(int));

        if (node->outInterface == NULL) {
			freeIO();
            freeNode(node);
			freeTrie(root);
            printErrors(MEMORY_ALLOCATION_FAILED);
        	exit(1);
        }

        *(node->outInterface) = *outInterface;
    } else node->outInterface = NULL;

    node->leftSon = NULL;
    node->rightSon = NULL;
    return node;
}


void freeNode(struct Node *node) {
	if (node != NULL) {
		if (node->outInterface != NULL) free(node->outInterface);
		free(node);
	}
}


void freeTrie(struct Node *root) {
	if (root != NULL) {
		freeTrie(root->leftSon);
		freeTrie(root->rightSon);
		freeNode(root);
	}
}


void generateTrie(struct Node *root) {
	struct Node *currentNode = root;
	int outInterface;
	uint32_t prefix;
	int prefixLength;
	int result;

	while((result = readFIBLine(&prefix, &prefixLength, &outInterface)) != REACHED_EOF) {
		if (result != OK) {
			freeIO();
			freeTrie(root);
			printIOExplanationError(result);
			exit(1);
		}
		
		if (prefixLength == 0) {
			root->outInterface = malloc(sizeof(int));

			if (root->outInterface == NULL) {
				freeIO();
            	freeTrie(root);
            	printErrors(MEMORY_ALLOCATION_FAILED);
        		exit(1);
        	}

			*(root->outInterface) = outInterface;
		}

		uint32_t mask = 1u << 31;
		
		for (int i = 0; i < prefixLength; i++) {
			int bit = (prefix & mask) ? 1 : 0;
			mask = mask >> 1;
			
			if (bit == 0) {
				if (i != prefixLength - 1) {
					if (currentNode->leftSon == NULL) currentNode->leftSon = createNode(NULL, root);
				} else {
					if (currentNode->leftSon == NULL) currentNode->leftSon = createNode(&outInterface, root);
					else *(currentNode->leftSon->outInterface) = outInterface;
				}
				
				currentNode = currentNode->leftSon;
			} else {
				if (i != prefixLength - 1) {
					if (currentNode->rightSon == NULL) currentNode->rightSon = createNode(NULL, root);
				} else {
					if (currentNode->rightSon == NULL) currentNode->rightSon = createNode(&outInterface, root);
					else *(currentNode->rightSon->outInterface) = outInterface;
				}
				
				currentNode = currentNode->rightSon;
			}
		}

		currentNode = root;
	}
}


void lookup(uint32_t *IPAddress, int *numberOfAccesses, int *outInterface, struct Node *root) {
	struct Node *currentNode = root;
	*numberOfAccesses = 0;
	uint32_t mask = 1u << 31;

	for (int i = 0; i < 32; i++) {
		int bit = (*IPAddress & mask) ? 1 : 0;
		mask = mask >> 1;
		(*numberOfAccesses)++;

		if (currentNode->outInterface != NULL) *outInterface = *(currentNode->outInterface);

		if (bit == 0 && currentNode->leftSon != NULL) currentNode = currentNode->leftSon;
		else if (bit == 1 && currentNode->rightSon != NULL) currentNode = currentNode->rightSon;
		else break;
	}
}


void printErrors(int result) {
	switch(result) {
	case NOT_ENOUGH_ARGUMENTS:
		printf("Not enough arguments\n");
		break;
	case TOO_MANY_ARGUMENTS:
		printf("Too many arguments\n");
		break;
	case MEMORY_ALLOCATION_FAILED:
		printf("Memory allocaction failed\n");
	default:
		printf("Unknown error\n");
		break;
	}
}


void traverseTrie(int *NumberOfNodesInTrie, struct Node *root) {
	if (root != NULL) {
		(*NumberOfNodesInTrie)++;
		traverseTrie(NumberOfNodesInTrie, root->leftSon);
		traverseTrie(NumberOfNodesInTrie, root->rightSon);
	}
}


void main(int argc, char *argv[]) {
	if (argc != 3) {
		if (argc < 3) printErrors(NOT_ENOUGH_ARGUMENTS);
		else printErrors(TOO_MANY_ARGUMENTS);
		exit(1);
	}

	char *routingTableName = argv[1];
	char *inputFileName = argv[2];
	int result;

	if ((result = initializeIO(routingTableName, inputFileName)) != OK) {
		printIOExplanationError(result);
		exit(1);
	}

	struct Node *root = createNode(NULL, NULL);
	generateTrie(root);
	int NumberOfNodesInTrie2 = 0;
	traverseTrie(&NumberOfNodesInTrie2, root);
	printf("nodes: %d\n", NumberOfNodesInTrie2);
	compressTrie(root);
	int NumberOfNodesInTrie = 0;
	traverseTrie(&NumberOfNodesInTrie, root);
	printf("compressed nodes: %d\n", NumberOfNodesInTrie);
	struct timespec initialTime;
	struct timespec finalTime;
	uint32_t IPAddress;
	int numberOfAccesses = 0;
	int outInterface;
	int processedPackets = 0;
	double searchingTime;
	int totalNodeAccesses = 0;
	double totalPacketProcessingTime = 0;

	while ((result = readInputPacketFileLine(&IPAddress)) != REACHED_EOF) {
		if (result != OK) {
			freeIO();
			freeTrie(root);
			printIOExplanationError(result);
			exit(1);
		}

		clock_gettime(CLOCK_MONOTONIC_RAW, &initialTime);
		lookup(&IPAddress, &numberOfAccesses, &outInterface, root);
		clock_gettime(CLOCK_MONOTONIC_RAW, &finalTime);
		printOutputLine(IPAddress, outInterface, &initialTime, &finalTime, &searchingTime, numberOfAccesses);
		processedPackets++;
		totalNodeAccesses += numberOfAccesses;
		totalPacketProcessingTime += searchingTime;
	}
	
	printSummary(NumberOfNodesInTrie, processedPackets, (double) (totalNodeAccesses / processedPackets), (double) (totalPacketProcessingTime / processedPackets));
	freeIO();
	freeTrie(root);
	exit(0);
}
