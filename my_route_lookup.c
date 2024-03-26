#include "io.h"
#include "my_route_lookup.h"


int bitIPAddress(uint32_t *IPAddress, struct Node *node) {
    uint32_t mask = 1u << (31 - (*(node->bitPosition - 1)));
    return (*IPAddress & mask) != 0;
}


int bitStringComparison(uint32_t *IPAddress, struct Node *node) {
	uint32_t mask = 1u << 31;
	printf("IP address: %d.%d.%d.%d\n", *(IPAddress) >> 24, (*(IPAddress) << 8) >> 24, (*(IPAddress) << 16) >> 24, (*(IPAddress) << 24) >> 24);
	printf("Prefix: %d.%d.%d.%d/%d\n", *(node->bitString) >> 24, (*(node->bitString) << 8) >> 24, (*(node->bitString) << 16) >> 24, (*(node->bitString) << 24) >> 24, *(node->bitPosition));

	for (int i = 0; i < *(node->bitPosition); i++) {
		int bitIPAddress = (*IPAddress & mask) != 0;
		int bitPrefix = (*(node->bitString) & mask) != 0;
		printf("IP address: %d\n", bitIPAddress);
		printf("Prefix: %d\n", bitPrefix);
		
		if (bitIPAddress != bitPrefix) return 0;

		mask = mask >> 1;
	}
	return 1;
}


void compressTrie(struct Node *root) {
    if (root != NULL) {
        if (root->outInterface == NULL) {
            struct Node *child = NULL;

            if (root->leftSon != NULL && root->rightSon == NULL) {
                child = root->leftSon;
            } else if (root->leftSon == NULL && root->rightSon != NULL) {
                child = root->rightSon;
            }

            if (child != NULL) {
                root->bitPosition = child->bitPosition;
                root->bitString = child->bitString;
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
			int bitPosition = i + 1; //i + 2;
			
			if (bit == 0) {
				if (i != prefixLength - 1) {
					if (currentNode->leftSon == NULL) currentNode->leftSon = createNode(&bitPosition, NULL, NULL, root);
				} else {
					if (currentNode->leftSon == NULL) currentNode->leftSon = createNode(&bitPosition, &prefix, &outInterface, root);
					else {
						*(currentNode->leftSon->bitString) = prefix;
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
						*(currentNode->rightSon->bitString) = prefix;
						*(currentNode->rightSon->outInterface) = outInterface;
					}
				}
				
				currentNode = currentNode->rightSon;
			}
		}

		currentNode = root;
	}
}


void lookup(uint32_t *IPAddress, int *numberOfAccesses, int *outInterface, struct Node *root) {
	struct Node *currentNode = root;
	//uint32_t mask = 1u << 31;

	/*for (int i = 0; i < 32; i++) {
		
		if (currentNode->outInterface != NULL) {
			*outInterface = *(currentNode->outInterface);
		}

		if (currentNode->leftSon != NULL || currentNode->rightSon != NULL) {
			if (currentNode->leftSon != NULL && currentNode->rightSon != NULL) {
				if (currentNode->leftSon->bitString != NULL && currentNode->rightSon->bitString != NULL) {
					if (bitStringComparison(IPAddress, currentNode->leftSon) == 1) {
						*(numberOfAccesses)++;
						currentNode = currentNode->leftSon;
					} else if (bitStringComparison(IPAddress, currentNode->rightSon) == 1) {
						*(numberOfAccesses)++;
						currentNode = currentNode->rightSon;
					}
				} else if (currentNode->leftSon->bitString != NULL || currentNode->rightSon->bitString != NULL) {
					if (currentNode->leftSon->bitString != NULL) {
						if (bitStringComparison(IPAddress, currentNode->leftSon) == 1) {
							*(numberOfAccesses)++;
							currentNode = currentNode->leftSon;
						} else if (bitComparison(IPAddress, currentNode->rightSon) == 1) {
							*(numberOfAccesses)++;
							currentNode = currentNode->leftSon;
						}
					} else {
						if (bitStringComparison(IPAddress, currentNode->rightSon) == 1) {
							*(numberOfAccesses)++;
							currentNode = currentNode->rightSon;
						} else if (bitComparison(IPAddress, currentNode->leftSon) == 0) {
							*(numberOfAccesses)++;
							currentNode = currentNode->rightSon;
						}
					}
				} else {
					if (currentNode->leftSon->bitPosition <= currentNode->rightSon->bitPosition) {
						if (bitComparison(IPAddress, currentNode->leftSon) == 0) {
							*(numberOfAccesses)++;
							currentNode = currentNode->leftSon;
						} else if (bitComparison(IPAddress, currentNode->rightSon) == 1) {
							*(numberOfAccesses)++;
							currentNode = currentNode->rightSon;
						}
					} else {
						if (bitComparison(IPAddress, currentNode->rightSon) == 1) {
							*(numberOfAccesses)++;
							currentNode = currentNode->rightSon;
						} else if (bitComparison(IPAddress, currentNode->leftSon) == 0) {
							*(numberOfAccesses)++;
							currentNode = currentNode->leftSon;
						}
					}
				}
			} else {
				if (currentNode->leftSon != NULL) {
					if (currentNode->leftSon->bitString != NULL) {
						if (bitStringComparison(IPAddress, currentNode->leftSon) == 1) {
							*(numberOfAccesses)++;
							currentNode = currentNode->leftSon;
						}
					} else {
						if (bitComparison(IPAddress, currentNode->leftSon) == 0) {
							*(numberOfAccesses)++;
							currentNode = currentNode->leftSon;
						}
					}
				} else {
					if (currentNode->rightSon->bitString != NULL) {
						if (bitStringComparison(IPAddress, currentNode->rightSon) == 1) {
							*(numberOfAccesses)++;
							currentNode = currentNode->rightSon;
						}
					} else {
						if (bitComparison(IPAddress, currentNode->rightSon) == 1) {
							*(numberOfAccesses)++;
							currentNode = currentNode->rightSon;
						}
					}
				}
			}
		} else break;
	}*/

	if (currentNode->outInterface != NULL) *outInterface = *(currentNode->outInterface);

	if (currentNode->leftSon != NULL && currentNode->rightSon != NULL) {
		if (*(currentNode->leftSon->bitPosition) <= *(currentNode->leftSon->bitPosition)) {
			if (bitIPAddress(IPAddress, currentNode->leftSon) == 0) {
				if (currentNode->leftSon->bitString != NULL) {
					if (bitStringComparison(IPAddress, currentNode->leftSon) == 1) {
						(*numberOfAccesses)++;
						lookup(IPAddress, numberOfAccesses, outInterface, currentNode->leftSon);
					}
				} else {
					(*numberOfAccesses)++;
					lookup(IPAddress, numberOfAccesses, outInterface, currentNode->leftSon);
				}
			}

			if (bitIPAddress(IPAddress, currentNode->rightSon) == 1) {
				if (currentNode->rightSon->bitString != NULL) {
					if (bitStringComparison(IPAddress, currentNode->rightSon) == 1) {
						(*numberOfAccesses)++;
						lookup(IPAddress, numberOfAccesses, outInterface, currentNode->rightSon);
					}
				} else {
					(*numberOfAccesses)++;
					lookup(IPAddress, numberOfAccesses, outInterface, currentNode->rightSon);
				}
			}
		} else {
			if (bitIPAddress(IPAddress, currentNode->rightSon) == 1) {
				if (currentNode->rightSon->bitString != NULL) {
					if (bitStringComparison(IPAddress, currentNode->rightSon) == 1) {
						(*numberOfAccesses)++;
						lookup(IPAddress, numberOfAccesses, outInterface, currentNode->rightSon);
					}
				} else {
					(*numberOfAccesses)++;
					lookup(IPAddress, numberOfAccesses, outInterface, currentNode->rightSon);
				}
			}

			if (bitIPAddress(IPAddress, currentNode->leftSon) == 0) {
				if (currentNode->leftSon->bitString != NULL) {
					if (bitStringComparison(IPAddress, currentNode->leftSon) == 1) {
						(*numberOfAccesses)++;
						lookup(IPAddress, numberOfAccesses, outInterface, currentNode->leftSon);
					}
				} else {
					(*numberOfAccesses)++;
					lookup(IPAddress, numberOfAccesses, outInterface, currentNode->leftSon);
				}
			}
		}
	} else if (currentNode->leftSon != NULL && currentNode->rightSon == NULL) {
		if (bitIPAddress(IPAddress, currentNode->leftSon) == 0) {
			if (currentNode->leftSon->bitString != NULL) {
				if (bitStringComparison(IPAddress, currentNode->leftSon) == 1) {
					(*numberOfAccesses)++;
					lookup(IPAddress, numberOfAccesses, outInterface, currentNode->leftSon);
				}
			} else {
				(*numberOfAccesses)++;
				lookup(IPAddress, numberOfAccesses, outInterface, currentNode->leftSon);
			}
		}
	} else if (currentNode->leftSon == NULL && currentNode->rightSon != NULL) {
		if (bitIPAddress(IPAddress, currentNode->rightSon) == 1) {
			if (currentNode->rightSon->bitString != NULL) {
				if (bitStringComparison(IPAddress, currentNode->rightSon) == 1) {
					(*numberOfAccesses)++;
					lookup(IPAddress, numberOfAccesses, outInterface, currentNode->rightSon);
				}
			} else {
				(*numberOfAccesses)++;
				lookup(IPAddress, numberOfAccesses, outInterface, currentNode->rightSon);
			}
		}
	} else return;
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


/*int traverseTrie(struct Node *root) {
    if (root == NULL) {
        return 0;
    }

    int leftNodes = traverseTrie(root->leftSon);
    int rightNodes = traverseTrie(root->rightSon);
    
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

	int bitPosition = 0; //1
	struct Node *root = createNode(&bitPosition, NULL, NULL, NULL);
	generateTrie(root);
	compressTrie(root);

	int NumberOfNodesInTrie = 0;
	traverseTrie(&NumberOfNodesInTrie, root);

	struct timespec initialTime, finalTime;
	uint32_t IPAddress;
	int numberOfAccesses = 1;
	int outInterface = 0;
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
		numberOfAccesses = 1;
		outInterface = 0;
	}
	
	printSummary(NumberOfNodesInTrie, processedPackets, (double) (totalNodeAccesses / processedPackets), (double) (totalPacketProcessingTime / processedPackets));
	freeIO();
	freeTrie(root);
	exit(0);
}
