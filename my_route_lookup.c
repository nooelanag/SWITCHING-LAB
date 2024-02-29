#include "io.h"
#include "my_route_lookup.h"
#include "utils.h"

void decToBin(uint32_t prefix, int **binaryArray) {
	for (int i = 31; i >= 0; i--) {
		binaryArray[i] = prefix & 1;
        prefix >>= 1;
	}
}

Node * createNode(int *outInterface, int *result) {
	Node *node = (Node *)malloc(sizeof(Node));
	if (node == NULL) {
        result = MEMORY_ALLOCATION_FAILED;
		return;
    }
	node->outInterface = outInterface;
	node->leftSon = NULL;
	node->rightSon = NULL;
	result = OK;
	return node;
}

int generateTrie(Node *root, int **binaryArray) {
	Node *currentNode = root;
	uint32_t *prefix;
	int *prefixLength;
	int *outInterface;
	int result;
	while((result = readFIBLine(prefix, prefixLength, outInterface)) != EOF) {
		if (result != OK) return result;
		decToBin(prefix, binaryArray);
		for (int i = 0; i < prefixLength; i++) {
			if (binaryArray[i] == 0) {
				if (i != prefixLength - 1) {
					if (currentNode->leftSon == NULL) {
						currentNode->leftSon = createNode(NULL, &result);
						if (result != OK) return result;
					}
				} else {
					if (currentNode->leftSon == NULL) {
						currentNode->leftSon = createNode(outInterface);
						if (result != OK) return result;
					} else {
						currentNode->leftSon->outInterface = outInterface;
					}
				}
				currentNode = currentNode->leftSon
			} else {
				if (i != prefixLength - 1) {
					if (currentNode->rightSon == NULL) {
						currentNode->rightSon = createNode(NULL);
						if (result != OK) return result;
					}
				} else {
					if (currentNode->rightSon == NULL) {
						currentNode->rightSon = createNode(outInterface);
						if (result != OK) return result;
					} else {
						currentNode->rightSon->outInterface = outInterface;
					}
				}
			}
		}
	}
	free(binaryArray);
	return OK;
}

void traverseTrie(Node *node, int *NumberOfNodesInTrie) {
	if (node != NULL) {
		NumberOfNodesInTrie++;
		traverseTrie(node->leftSon, NumberOfNodesInTrie);
		traverseTrie(node->rightSon, NumberOfNodesInTrie);
	}
}

void compressTrie(Node *node) {
	if (node != NULL) {
		if (node->leftSon != NULL & node->rightSon == NULL) {
			Node *temp = node->leftSon;
			node->outInterface = node->leftSon->outInterface;
			node->leftSon = node->leftSon->leftSon;
			node->rightSon = node->leftSon->rightSon;
			free(temp);
		} else if (node->leftSon == NULL & node->rightSon != NULL) {
			Node *temp = node->rightSon;
			node->outInterface = node->rightSon->outInterface;
			node->leftSon = node->rightSon->leftSon;
			node->rightSon = node->rightSon->rightSon;
			free(temp);
		}
		compressTrie(node->leftSon);
		compressTrie(node->rightSon);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 3) exit(1);
	if ((int result = initializeIO(argv[1], argv[2])) != OK) {
		printIOExplanationError(result);
		exit(1);
	}
	int *result;
	Node *root = createNode(NULL, result);
	if (result != OK) {
        printf("Memory allocation failed\n");
		freeIO();
        exit(1);
    }
	int *binaryArray = (int *)malloc(32 * sizeof(int));
	if (binaryArray == NULL) {
        printf("Memory allocation failed\n");
		freeIO();
        exit(1);
    }
	if ((result = generateTrie(root, &binaryArray)) != OK) {
		printf("Error\n");
		freeIO();
		exit(1);
	}
	compressTrie(root);
	uint32_t *IPAddress; //Puntero a la IP address
	struct timespec initialTime;
	struct timespec finalTime;
	double searchingTime;
	int NumberOfNodesInTrie = 0;
	int processedPackets = 0;
	int nodeAccesses = 0;
	int totalNodeAccesses = 0;
	double totalPacketProcessingTime = 0;
	while ((result = readInputPacketFileLine(IPAddress)) != EOF) {
		if (result != OK) {
			printIOExplanationError(result);
			freeIO();
			exit(1);
		}
		clock_gettime(CLOCK_MONOTONIC_RAW, &initialTime);
		//lookup function
		clock_gettime(CLOCK_MONOTONIC_RAW, &finalTime);
		printOutputLine(IPAddress, /*outInterface*/, &initialTime, &finalTime, &searchingTime, /*numberOfAccesses*/);
		totalNodeAccesses += nodeAccesses;
		processedPackets++;
		totalPacketProcessingTime += searchingTime;
	}
	traverseTrie(&NumberOfNodesInTrie);
	printSummary(NumberOfNodesInTrie, processedPackets, (double)totalNodeAccesses / processedPackets, (double)totalPacketProcessingTime / processedPackets);
	freeIO();
	exit(0);
}
