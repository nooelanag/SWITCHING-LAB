#include "io.h"
#include "my_route_lookup.h"


int addressPrefixComparison(const uint32_t IPAddress, const char *prefixString, int *result) {
	if (prefixString[0] == '*') {
		*result = OK;
		return MATCH;
	}

	int prefixLength = strlen(prefixString);
	char *IPAddressString = uint32ToString(IPAddress, prefixLength, result);
	if (*result != OK) return *result;

	int comparison = strcmp(IPAddressString, prefixString);
	free(IPAddressString);
	*result = OK;
	return (comparison == 0) ? MATCH : NOT_MATCH;
}


int bitIPAddress(const uint32_t IPAddress, const int position, int *result) {
	if (position < 1 || position > 32) {
		*result = NOT_VALID_POSITION;
		return *result;
	}

	*result = OK;
	return (IPAddress >> (32 - position)) & 1;
}


void compressTrie(struct Node *node) {
	if (node == NULL) return;

	if ((node->leftSon != NULL) ^ (node->rightSon != NULL)) {
		struct Node *childNode = (node->leftSon != NULL) ? node->leftSon : node->rightSon;

		if (node->prefixString == NULL || childNode->prefixString == NULL) {
			if (node->prefixString == NULL && childNode->prefixString != NULL) {
				node->prefixString = childNode->prefixString;
				node->outInterface = childNode->outInterface;
			}

			free(node->bitPosition);
			node->bitPosition = childNode->bitPosition;
			node->leftSon = childNode->leftSon;
			node->rightSon = childNode->rightSon;
			free(childNode);
			compressTrie(node);
		}
	}

	compressTrie(node->leftSon);
	compressTrie(node->rightSon);
}


struct Node *createNode(const int *bitPosition, char *prefixString, const int *outInterface, int *result) {
	struct Node *node = malloc(sizeof(struct Node));
	if (node == NULL) {
		*result = MEMORY_ALLOCATION_FAILED;
		return NULL;
	}

	node->bitPosition = NULL;
	node->prefixString = prefixString;
	node->outInterface = NULL;
	node->leftSon = NULL;
	node->rightSon = NULL;

	if (bitPosition != NULL) {
		node->bitPosition = malloc(sizeof(int));
		if (node->bitPosition == NULL) {
			freeNode(node);
			*result = MEMORY_ALLOCATION_FAILED;
			return NULL;
		}

		*(node->bitPosition) = *bitPosition;
	}

	if (outInterface != NULL) {
		node->outInterface = malloc(sizeof(int));
		if (node->outInterface == NULL) {
			freeNode(node);
			*result = MEMORY_ALLOCATION_FAILED;
			return NULL;
		}

		*(node->outInterface) = *outInterface;
	}

	*result = OK;
	return node;
}


void freeNode(struct Node *node) {
	if (node == NULL) return;
	if (node->bitPosition != NULL) free(node->bitPosition);
	if (node->prefixString != NULL) free(node->prefixString);
	if (node->outInterface != NULL) free(node->outInterface);
	free(node);
}


void freeTrie(struct Node *node) {
	if (node == NULL) return;
	freeTrie(node->leftSon);
	freeTrie(node->rightSon);
	freeNode(node);
}


struct Node *generateTrie(int *result) {
	int bitPosition = 1;
	struct Node *root = createNode(&bitPosition, NULL, NULL, result);
	if (*result != OK) return NULL;

	struct Node *currentNode;
	int bit, outInterface, prefixLength;
	uint32_t prefix;

	while ((*result = readFIBLine(&prefix, &prefixLength, &outInterface)) != REACHED_EOF) {
		if (*result != OK) {
			freeTrie(root);
			return NULL;
		}

		if (prefixLength == 0) {
			if (root->outInterface == NULL) root->outInterface = malloc(sizeof(int));
			if (root->outInterface == NULL) {
				freeTrie(root);
				return NULL;
			}

			*(root->outInterface) = outInterface;
		}

		bitPosition = 1;
		currentNode = root;

		for (int i = 0; i < prefixLength; i++) {
			bit = bitIPAddress(prefix, bitPosition, result);
			if (*result != OK) {
				freeTrie(root);
				return NULL;
			}

			bitPosition++;
			struct Node **sonPtr = (bit == 0) ? &(currentNode->leftSon) : &(currentNode->rightSon);

			if (*sonPtr == NULL) {
				*sonPtr = createNode((bitPosition <= 32) ? &bitPosition : NULL, NULL, NULL, result);
				if (*result != OK) {
					freeTrie(root);
					return NULL;
				}
			}

			if (i == prefixLength - 1) {
				(*sonPtr)->prefixString = uint32ToString(prefix, prefixLength, result);
				if (*result != OK) {
					freeTrie(root);
					return NULL;
				}

				if ((*sonPtr)->outInterface == NULL) (*sonPtr)->outInterface = malloc(sizeof(int));
				if ((*sonPtr)->outInterface == NULL) {
					freeTrie(root);
					*result = MEMORY_ALLOCATION_FAILED;
					return NULL;
				}
				*((*sonPtr)->outInterface) = outInterface;
			}

			currentNode = *sonPtr;
		}
	}

	*result = OK;
	return root;
}


int lookup(const uint32_t IPAddress, int *outInterface, int *result, struct Node *root) {
	struct Node *currentNode = root;
	int bit, match, numberOfAccesses = 0;
	*outInterface = 0;

	while (currentNode != NULL) {
		numberOfAccesses++;
		if (currentNode->prefixString != NULL) {
			match = addressPrefixComparison(IPAddress, currentNode->prefixString, result);
			if (*result != OK) return *result;
			else if (match == MATCH) *outInterface = *(currentNode->outInterface);
		}

		if (currentNode->bitPosition == NULL) break;
		bit = bitIPAddress(IPAddress, *(currentNode->bitPosition), result);
		if (*result != OK) return *result;
		if (bit == 0) currentNode = currentNode->leftSon;
		else currentNode = currentNode->rightSon;
	}

	return numberOfAccesses;
}


void printErrors(const int result) {
	switch (result) {
		case NOT_ENOUGH_ARGUMENTS:
			printf("Not enough arguments\n");
			break;
		case TOO_MANY_ARGUMENTS:
			printf("Too many arguments\n");
			break;
		case MEMORY_ALLOCATION_FAILED:
			printf("Memory allocation failed\n");
			break;
		case NOT_VALID_PREFIX_LENGTH:
			printf("Not valid prefix length\n");
			break;
		case NOT_VALID_POSITION:
			printf("Not valid position\n");
			break;
		default:
			printf("Unknown error\n");
			break;
	}
}


int traverseTrie(struct Node *node) {
	if (node == NULL) return 0;
	return 1 + traverseTrie(node->leftSon) + traverseTrie(node->rightSon);
}


char *uint32ToString(const uint32_t IPAddress, const int prefixLength, int *result) {
	if (prefixLength < 0 || prefixLength > 32) {
		*result = NOT_VALID_PREFIX_LENGTH;
		return NULL;
	}

	if (prefixLength == 0) {
		char *bitString = malloc(2 * sizeof(char));
		if (bitString == NULL) {
			*result = MEMORY_ALLOCATION_FAILED;
			return NULL;
		}
		bitString[0] = '*';
		bitString[1] = '\0';
		*result = OK;
		return bitString;
	}

	char *bitString = malloc((prefixLength + 1) * sizeof(char));
	if (bitString == NULL) {
		*result = MEMORY_ALLOCATION_FAILED;
		return NULL;
	}

	for (int i = 1; i <= prefixLength; i++) {
		bitString[i - 1] = bitIPAddress(IPAddress, i, result) ? '1' : '0';
		if (*result != OK) {
			free(bitString);
			return NULL;
		}
	}
	bitString[prefixLength] = '\0';
	*result = OK;
	return bitString;
}


int main(int argc, char *argv[]) {
	if (argc != 3) {
		if (argc < 3) printErrors(NOT_ENOUGH_ARGUMENTS);
		else printErrors(TOO_MANY_ARGUMENTS);
		return 1;
	}

	char *routingTableName = argv[1];
	char *inputFileName = argv[2];

	int result = initializeIO(routingTableName, inputFileName);
	if (result != OK) {
		printIOExplanationError(result);
		return 1;
	}

	struct Node *root = generateTrie(&result);
	if (result != OK) {
		freeIO();
		printErrors(result);
		return 1;
	}

	compressTrie(root);
	struct timespec initialTime, finalTime;
	uint32_t IPAddress;
	int numberOfAccesses, outInterface, processedPackets = 0, totalNodeAccesses = 0;
	double searchingTime = 0, totalPacketProcessingTime = 0;

	while ((result = readInputPacketFileLine(&IPAddress)) != REACHED_EOF) {
		if (result != OK) {
			freeIO();
			freeTrie(root);
			printIOExplanationError(result);
			return 1;
		}

		clock_gettime(CLOCK_MONOTONIC_RAW, &initialTime);
		numberOfAccesses = lookup(IPAddress, &outInterface, &result, root);
		clock_gettime(CLOCK_MONOTONIC_RAW, &finalTime);
		if (result != OK) {
			freeIO();
			freeTrie(root);
			printErrors(result);
			return 1;
		}

		printOutputLine(IPAddress, outInterface, &initialTime, &finalTime, &searchingTime, numberOfAccesses);
		processedPackets++;
		totalNodeAccesses += numberOfAccesses;
		totalPacketProcessingTime += searchingTime;
	}

	int numberOfNodesInTrie = traverseTrie(root);
	double averageNodeAccesses = (double)totalNodeAccesses / processedPackets;
	double averagePacketProcessingTime = (double)totalPacketProcessingTime / processedPackets;
	printSummary(numberOfNodesInTrie, processedPackets, averageNodeAccesses, averagePacketProcessingTime);
	freeIO();
	freeTrie(root);
	return 0;
}
