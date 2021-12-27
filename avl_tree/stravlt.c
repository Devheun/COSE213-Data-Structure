#define SHOW_STEP 0
#define BALANCING 1

#include <stdlib.h> // malloc, rand
#include <stdio.h>
#include <time.h> // time
#include <string.h> //strcmp, strdup

//양쪽 자식 중에 더 큰 값 + 1 = height
#define max(x, y)	(((x) > (y)) ? (x) : (y)) //height 계산할 때 필요 

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	char		*data;
	struct node	*left;
	struct node	*right;
	int			height; //나를 root로 삼을 때 내 subtree들의 높이, 노드 하나짜리 height=1
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void);

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree);
static void _destroy( NODE *root); //각 노드를 따라다니면서 노드구조체 삭제, string도 삭제

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data);

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr); //char* data 받아서 내부에서 노드하나 만들고 그 노드를 삽입해라

static NODE *_makeNode( char *data); //노드 만드는 함수, 노드를 위한 메모리할당,포인터들은 NULL로 초기화
//문자열 받은거 (char *data) 메모리 할당해서 그 문자열 주소를 포인팅하도록 -> strdup, 처음 노드 하나 만들땐 height 1로

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key); //키 값 비교 -> strcmp

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree);
static void _traverse( NODE *root);

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight(NODE* root) {
	
	if (root == NULL) {
		return 0;
	}
	return root->height;
}

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root); //포인터의 위치를 바꾸어줌, 노드들의 height값 바뀜

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	char str[1024];
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 0;
	}
	
	// creates a null tree
	tree = AVL_Create();
	
	 if (!tree)
	 {
		fprintf( stderr, "Cannot create tree!\n");
		return 100;
	 } 

	FILE *fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Cannot open file! [%s]\n", argv[1]);
		return 200;
	}

	while(fscanf( fp, "%s", str) != EOF)
	{

#if SHOW_STEP
		fprintf( stdout, "Insert %s>\n", str);
#endif	
		// insert function call
		AVL_Insert( tree, str);

#if SHOW_STEP
		fprintf( stdout, "Tree representation:\n");
		printTree( tree);
#endif
	}
	
	fclose( fp);
	
#if SHOW_STEP
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	AVL_Traverse( tree);
	fprintf( stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
#endif
	fprintf( stdout, "Height of tree: %d\n", tree->root->height);
	fprintf( stdout, "# of nodes: %d\n", tree->count);

	// retrieval
	char *key;
	fprintf( stdout, "Query: ");
	while( fscanf( stdin, "%s", str) != EOF)
	{
		key = AVL_Retrieve( tree, str);
		
		if (key) fprintf( stdout, "%s found!\n", key);
		else fprintf( stdout, "%s NOT found!\n", str);
		
		fprintf( stdout, "Query: ");
	}
	
	// destroy tree
	AVL_Destroy( tree);

	return 0;
}

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE* AVL_Create(void) {

	AVL_TREE* newtree = (AVL_TREE*)malloc(sizeof(AVL_TREE));
	if (newtree == NULL)return NULL;
	newtree->count = 0;
	newtree->root = NULL;
	return newtree;
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert(AVL_TREE* pTree, char* data) {

	NODE* newNode = _makeNode(data);
	if (newNode == NULL)return 0;
	if (pTree->root == NULL) {
		pTree->root = newNode;
		pTree->count++;
	}
	else {
		pTree->root=_insert(pTree->root, newNode);
		pTree->count++;
	}

	return 1;
}

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE* _insert(NODE* root, NODE* newPtr) { //char* data 받아서 내부에서 노드하나 만들고 그 노드를 삽입해라

	if (root == NULL) {
		return newPtr;
	}

	//insert 과정
	if (strcmp(root->data, newPtr->data) > 0) {
		root->left = _insert(root->left, newPtr);
		root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	}
	else {
		root->right = _insert(root->right, newPtr);
		root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	}


	if (BALANCING) {
		//balance를 맞춰줘야함
		int factor = getHeight(root->left) - getHeight(root->right);

		//unbalance할때 회전시키기

		if (factor > 1) {
			//LL
			if (getHeight(root->left->left) - getHeight(root->left->right) > 0) {
				root = rotateRight(root);
			}
			//LR
			else {
				root->left = rotateLeft(root->left);
				root = rotateRight(root);
			}
		}
		else if (factor < -1) {
			//RR
			if (getHeight(root->right->left) - getHeight(root->right->right) < 0) {
				root = rotateLeft(root);
			}
			//RL
			else {
				root->right = rotateRight(root->right);
				root = rotateLeft(root);
			}

		}
	}

	return root;
}

static NODE* _makeNode(char* data) { //노드 만드는 함수, 노드를 위한 메모리할당,포인터들은 NULL로 초기화
//문자열 받은거 (char *data) 메모리 할당해서 그 문자열 주소를 포인팅하도록 -> strdup, 처음 노드 하나 만들땐 height 1로

	NODE* makeNode = (NODE*)malloc(sizeof(NODE));
	makeNode->left = NULL;
	makeNode->right = NULL;
	makeNode->height = 1;
	makeNode->data = strdup(data);

	return makeNode;
}

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE* rotateRight(NODE* root) { //포인터의 위치를 바꾸어줌, 노드들의 height값 바뀜

	NODE* node = root->left;
	if (node == NULL)return root;
	NODE* node2 = node->right;

	//exchange
	node->right = root;
	root->left = node2;
	//height 바뀌는 애들 : rotate 당사자랑, 자식 1개

	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

	
	return node;

}

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE* rotateLeft(NODE* root) {

	NODE* node = root->right;
	if (node == NULL)return root;
	NODE* node2 = node->left;
	
	node->left = root;
	root->right = node2;

	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

	return node;
}

/* internal traversal function
*/
static void _infix_print(NODE* root, int level) {

	if (root != NULL) {
		if (root->right != NULL) {
			_infix_print(root->right, level + 1);
		}
		for (int i = 0; i < level; i++)printf("\t");
		printf("%s", root->data);
		printf("\n");

		if (root->left != NULL) {
			_infix_print(root->left, level + 1);
		}
		return;
	}
	else return;
}

/* Prints tree using inorder right-to-left traversal
*/
void printTree(AVL_TREE* pTree) {
	_infix_print(pTree->root, 0);
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char* AVL_Retrieve(AVL_TREE* pTree, char* key) {

	NODE* retri = _retrieve(pTree->root, key);
	if (retri == NULL)return NULL;
	return (retri->data);

}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE* _retrieve(NODE* root, char* key) { //키 값 비교 -> strcmp

	if (root == NULL)return NULL;
	else {
		if (strcmp(key, root->data) < 0) {
			return _retrieve(root->left, key);
		}
		else if (strcmp(key, root->data) > 0) {
			return _retrieve(root->right, key);
		}
		else return root;
	}
}

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy(AVL_TREE* pTree) {
	if (pTree) {
		_destroy(pTree->root);
	}
	free(pTree);
}
static void _destroy(NODE* root) { //각 노드를 따라다니면서 노드구조체 삭제, string도 삭제

	if (root != NULL) {
		if (root->left != NULL) {
			_destroy(root->left);
		}
		if (root->right != NULL) {
			_destroy(root->right);
		}

		free(root->data);
		free(root);
	}

}

/* Prints tree using inorder traversal
*/
void AVL_Traverse(AVL_TREE* pTree) {
	_traverse(pTree->root);
}

static void _traverse(NODE* root) {
	
	if (root != NULL) {
		if (root->left != NULL) {
			_traverse(root->left);
		}
		printf("%s ", root->data);
		if (root->right != NULL) {
			_traverse(root->right);
		}
	}
}