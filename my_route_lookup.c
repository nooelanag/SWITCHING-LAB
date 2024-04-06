#include "io.h"
#include "my_route_lookup.h"


int bitIPAddress(uint32_t *IPAddress, struct Node *node) {
    uint32_t mask = 1u << (32 - *(node->bitPosition));
    return (*IPAddress & mask) != 0;
}


int bitStringComparison(uint32_t *IPAddress, struct Node *node) {
	uint32_t mask = 1u << 31;

	if (*(node->bitPosition) == 1) return 1;

	for (int i = 0; i < *(node->bitPosition) - 1; i++) {
		int bitIPAddress = (*IPAddress & mask) != 0;
		int bitPrefix = (*(node->bitString) & mask) != 0;
		
		if (bitIPAddress != bitPrefix) return 0;

		mask = mask >> 1;
	}

	return 1;
}


void compressTrie(struct Node *node) {
	if (node != NULL) {
		struct Node *child = NULL;

		if (node->leftSon != NULL && node->rightSon == NULL) child = node->leftSon;
		else if (node->leftSon == NULL && node->rightSon != NULL) child = node->rightSon;

		if (child != NULL) {
			if (node->bitString == NULL && node->outInterface == NULL) {
				free(node->bitPosition);
				node->bitPosition = child->bitPosition;
				node->bitString = child->bitString;
				node->outInterface = child->outInterface;
				node->leftSon = child->leftSon;
				node->rightSon = child->rightSon;
				free(child);
				compressTrie(node);
			} else if (child->bitString == NULL && child->outInterface == NULL) {
				free(node->bitPosition);
				node->bitPosition = child->bitPosition;
				node->leftSon = child->leftSon;
				node->rightSon = child->rightSon;
				free(child->bitString);
				free(child->outInterface);
				free(child);
				compressTrie(node);
			}
		}

		compressTrie(node->leftSon);
		compressTrie(node->rightSon);
	}
}


struct Node *createNode(int *bitPosition, uint32_t *bitString, int *outInterface, struct Node *root) {
    struct Node *node = malloc(sizeof(struct Node));

    if (node == NULL) {
		freeIO();
		freeTrie(root);
        printErrors(MEMORY_ALLOCATION_FAILED);
        exit(1);
    }

	if (bitPosition != NULL) {
		node->bitPosition = malloc(sizeof(int));
		
		if (node->bitPosition == NULL) {
			freeIO();
            freeNode(node);
			freeTrie(root);
            printErrors(MEMORY_ALLOCATION_FAILED);
        	exit(1);
        }

		*(node->bitPosition) = *bitPosition;
	} else node->bitPosition = NULL;

	if (bitString != NULL) {
		node->bitString = malloc(sizeof(uint32_t));
		
		if (node->bitString == NULL) {
			freeIO();
            freeNode(node);
			freeTrie(root);
            printErrors(MEMORY_ALLOCATION_FAILED);
        	exit(1);
        }

		*(node->bitString) = *bitString;
	} else node->bitString = NULL;

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
		if (node->bitPosition != NULL) free(node->bitPosition);
		if (node->bitString != NULL) free(node->bitString);
		if (node->outInterface != NULL) free(node->outInterface);

		free(node);
	}
}


void freeTrie(struct Node *root) {
    if (root == NULL) return;

    freeTrie(root->leftSon);
    freeTrie(root->rightSon);
    freeNode(root);
}


void generateTrie(struct Node *root) {
	struct Node *currentNode;
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

		currentNode = root;
		
		if (prefixLength == 0) {
			if (root->bitString == NULL) {
				root->bitString = malloc(sizeof(uint32_t));

				if (root->bitString == NULL) {
					freeIO();
            		freeTrie(root);
            		printErrors(MEMORY_ALLOCATION_FAILED);
        			exit(1);
        		}
			}

			if (root->outInterface == NULL) {
				root->outInterface = malloc(sizeof(int));

				if (root->outInterface == NULL) {
					freeIO();
            		freeTrie(root);
            		printErrors(MEMORY_ALLOCATION_FAILED);
        			exit(1);
        		}
			}

			*(root->outInterface) = outInterface;
		}

		uint32_t mask = 1u << 31;
		
		for (int i = 0; i < prefixLength; i++) {
			int bit = (prefix & mask) != 0;
			mask = mask >> 1;
			int bitPosition = i + 2;
			
			if (bit == 0) {
				if (i != prefixLength - 1) {
					if (currentNode->leftSon == NULL) currentNode->leftSon = createNode(&bitPosition, NULL, NULL, root);
				} else {
					if (currentNode->leftSon == NULL) currentNode->leftSon = createNode(&bitPosition, &prefix, &outInterface, root);
					else {
						currentNode->leftSon->bitString = malloc(sizeof(uint32_t));

						if (currentNode->leftSon->bitString == NULL) {
							freeIO();
							freeTrie(root);
            				printErrors(MEMORY_ALLOCATION_FAILED);
        					exit(1);
        				}

						*(currentNode->leftSon->bitString) = prefix;

						currentNode->leftSon->outInterface = malloc(sizeof(int));

						if (currentNode->leftSon->outInterface == NULL) {
							freeIO();
							freeTrie(root);
            				printErrors(MEMORY_ALLOCATION_FAILED);
        					exit(1);
        				}

						*(currentNode->leftSon->outInterface) = outInterface;
					}
				}
				
				currentNode = currentNode->leftSon;
			} else {
				if (i != prefixLength - 1) {
					if (currentNode->rightSon == NULL) currentNode->rightSon = createNode(&bitPosition, NULL, NULL, root);
				} else {
					if (currentNode->rightSon == NULL) currentNode->rightSon = createNode(&bitPosition, &prefix, &outInterface, root);
					else {
						currentNode->rightSon->bitString = malloc(sizeof(uint32_t));

						if (currentNode->rightSon->bitString == NULL) {
							freeIO();
							freeTrie(root);
            				printErrors(MEMORY_ALLOCATION_FAILED);
        					exit(1);
        				}

						*(currentNode->rightSon->bitString) = prefix;

						currentNode->rightSon->outInterface = malloc(sizeof(int));

						if (currentNode->rightSon->outInterface == NULL) {
							freeIO();
							freeTrie(root);
            				printErrors(MEMORY_ALLOCATION_FAILED);
        					exit(1);
        				}

						*(currentNode->rightSon->outInterface) = outInterface;
					}
				}
				
				currentNode = currentNode->rightSon;
			}
		}
	}
}


void lookup(uint32_t *IPAddress, int *numberOfAccesses, int *outInterface, struct Node *root) {
	struct Node *currentNode = root;

	if (currentNode->bitString != NULL && bitStringComparison(IPAddress, currentNode) == 1) {
		if (currentNode->outInterface != NULL) *outInterface = *(currentNode->outInterface);
	}

	if (currentNode->leftSon != NULL && bitIPAddress(IPAddress, currentNode) == 0) {
		currentNode = currentNode->leftSon;
		(*numberOfAccesses)++;
		lookup(IPAddress, numberOfAccesses, outInterface, currentNode);
	} else if (currentNode->rightSon != NULL && bitIPAddress(IPAddress, currentNode) == 1) {
		currentNode = currentNode->rightSon;
		(*numberOfAccesses)++;
		lookup(IPAddress, numberOfAccesses, outInterface, currentNode);
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
		printf("Memory allocation failed\n");
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


/*int traverseTrie2(struct Node *root) {
    if (root == NULL) {
        return 0;
    }

    int leftNodes = traverseTrie2(root->leftSon);
    int rightNodes = traverseTrie2(root->rightSon);
    
    return 1 + leftNodes + rightNodes;
}*/


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

	int bitPosition = 1;
	struct Node *root = createNode(&bitPosition, NULL, NULL, NULL);
	generateTrie(root);
	compressTrie(root);

	int NumberOfNodesInTrie = 0;
	traverseTrie(&NumberOfNodesInTrie, root);
	/*int NumberOfNodesInTrie2 = traverseTrie2(root);
	printf("Number of nodes in trie 2: %d\n", NumberOfNodesInTrie2);*/

	struct timespec initialTime, finalTime;
	uint32_t IPAddress;
	int numberOfAccesses;
	int outInterface;
	int processedPackets = 0;
	double searchingTime = 0;
	int totalNodeAccesses = 0;
	double totalPacketProcessingTime = 0;

	while ((result = readInputPacketFileLine(&IPAddress)) != REACHED_EOF) {

		if (result != OK) {
			freeIO();
			freeTrie(root);
			printIOExplanationError(result);
			exit(1);
		}

		numberOfAccesses = 1;
		outInterface = 0;
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
