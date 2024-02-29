#include "io.h"
#include "my_route_lookup.h"
#include "utils.h"

typedef struct Node {
	int *outInterface;
	struct Node *leftSon;
	struct Node *rightSon;
}

int *decToBin(uint32_t prefix, int *binaryArray) {
	for(int i = 31; i >= 0; i--) {
		binaryArray[i] = prefix & 1;
        prefix >>= 1;
	}
	return binaryArray;
}

Node * createNode(int *outInterface) {
	Node* node = (Node *) malloc(sizeof(Node));
	if(node == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
	node -> outInterface = outInterface;
	node -> leftSon = NULL;
	node -> leftSon = NULL;
	return node;
}

void generateTree(uint32_t *prefix, int *prefixLength, int *outInterface, Node root) {
	Node currentNode = root;
	int *binaryArray = (int *) malloc(32 * sizeof(int));
	if(binaryArray == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
	while((int result = readFIBLine(prefix, prefixLength, outInterface)) != EOF) {
		if(result != OK) {
			printIOExplanationError(result);
			break;
		}
		for(int i = 0; i < prefixLength; i++) {
			if(binaryArray[i] == 0) {
				if(i != prefixLength - 1) {
					if(currentNode -> leftSon == NULL) {
						currentNode -> leftSon = createNode(NULL);
					}
				} else {
					if(currentNode -> leftSon == NULL) {
						currentNode -> leftSon = createNode(outInterface);
					} else {
						currentNode -> leftSon -> outInterface = outInterface;
					}
				}
				currentNode = currentNode -> leftSon
			} else {
				if(i != prefixLength - 1) {
					if(currentNode -> rightSon == NULL) {
						currentNode -> rightSon = createNode(NULL);
					}
				} else {
					if(currentNode -> rightSon == NULL) {
						currentNode -> rightSon = createNode(outInterface);
					} else {
						currentNode -> rightSon -> outInterface = outInterface;
					}
				}
			}
		}
	}
	free(binaryArray);
}

int main(int argc, char *argv[]) {
	if(argc != 3) return -1;
	if((int result = initializeIO(argv[1], argv[2])) != OK) {
		printIOExplanationError(result);
		exit(1);
	}
	struct node *root = (Node *) malloc(sizeof(Node));
	if(root == NULL) {
        printf("Memory allocation failed\n");
		freeIO();
        exit(1);
    }
	freeIO();
}
