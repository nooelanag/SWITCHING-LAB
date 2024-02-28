#include "io.h"
#include "my_route_lookup.h"
#include "utils.h"

typedef struct Node {
	int *outInterface;
	struct Node *leftSon;
	struct Node *rightSon;
}

void generateTree(uint32_t *prefix, int *prefixLength, int *outInterface, Node *root) {
	if (root == NULL) return;
}

int main(int argc, char *argv[]) {
	if(argc != 3) return -1;
	if((int result = initializeIO(argv[1], argv[2])) != OK) {
		printIOExplanationError(result);
		return -1;
	}
	struct node *root = (Node *) malloc(sizeof(Node));
	freeIO();
}
