#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert

#define MAX_STACK_SIZE	50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char		data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE* createTree(void) {
	TREE* p = (TREE*)malloc(sizeof(TREE));
	if (p == NULL)return NULL;
	else return p;
}

/* Deletes all data in tree and recycles memory
*/

static void _destroy(NODE* root) {

	if (root != NULL) {
		if ((root->left) != NULL) {
			_destroy(root->left);
		}
		if ((root->right) != NULL) {
			_destroy(root->right);
		}
		free(root);
	}
	else return;
	
}

void destroyTree(TREE* pTree);



/*  Allocates dynamic memory for a node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
static NODE* _makeNode(char ch) {

	NODE* pq = (NODE*)malloc(sizeof(NODE));
	if (pq == NULL)return NULL;
	pq->left = NULL;
	pq->right = NULL;
	pq->data = ch;

	return pq;
}

/* converts postfix expression to binary tree
	return	1 success
			0 invalid postfix expression
*/
int postfix2tree( char *expr, TREE *pTree){
	int i = 0;
	int top = -1;

	NODE* si[MAX_STACK_SIZE]; //NODE* 스택배열 생성

	while (expr[i] != '\0') {

		//연산자일때
		if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
			if (top < 1) { //invalid한 경우 메모리 잘 해제해야함

				if (top == 0) {
					_destroy(si[top]);
					top--;
				}
				break;
			}
			else if (top >= 1) {
				NODE* heun = _makeNode(expr[i]);
				heun->right = si[top];
				top--;
				heun->left = si[top];
				top--;
				top++;
				si[top] = heun;
			}
		}

		//피연산자일때
		else {
			top++;
			si[top] = _makeNode(expr[i]);
		}
		i++;
	}

	if (top == 0) {
		pTree->root = si[top];
		return 1;
	}
	else if (top!=0){ //invalid한 경우 메모리 잘 해제 해야함


		while (top > -1) {
			_destroy(si[top]);
			top--;
		}
		pTree->root = NULL;
	}
	return 0;
}

/* Print node in tree using inorder traversal
*/
/*
void traverseTree( TREE *pTree);
*/

/* internal traversal function
	an implementation of ALGORITHM 6-6
*/
static void _traverse(NODE* root) {

	if (root != NULL) {
		if (root->data != '+' && root->data != '-' && root->data != '*' && root->data != '/') { //피연산자면
			printf("%c", root->data);
		}
		else {
			printf("(");
			if (root->left != NULL) {
				_traverse(root->left);
			}
			printf("%c", root->data);
			if (root->right != NULL) {
				_traverse(root->right);
			}
			printf(")");
		}
	}
}


/* internal traversal function
*/
static void _infix_print(NODE* root, int level) {

	if (root != NULL) {
		if (root->right != NULL) {
			_infix_print(root->right, level + 1);

		}
		for (int i = 0; i < level; i++)printf("\t");
		printf("%c", root->data);
		printf("\n");

		if (root->left != NULL) {
			_infix_print(root->left, level + 1);
		}
		return;
	}
	else return;
}


/* Print node in tree using inorder traversal
*/
void traverseTree(TREE* pTree);

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE* pTree);


/* evaluate postfix expression
	return	value of expression
*/
float evalPostfix(char* expr) {

	int top = -1;
	float stacking[1024] = { 0, };
	char ss[1024] = { 0, };
	
	int i = 0;
	float value1 = 0;
	float value2 = 0;

	while (i<=1023) {
		
		if (expr[i] == 0)break;

		if (expr[i] == '+') {
			value1 = stacking[top];
			top--;
			value2 = stacking[top];
			top--;
			top++;
			stacking[top] = value1 + value2;
		}
		else if (expr[i] == '-') {
			value1 = stacking[top];
			top--;
			value2 = stacking[top];
			top--;
			top++;
			stacking[top] = value2 - value1;
		}
		else if (expr[i] == '*') {
			value1 = stacking[top];
			top--;
			value2 = stacking[top];
			top--;
			top++;
			stacking[top] = value1 * value2;
		}
		else if (expr[i] == '/') {
			value1 = stacking[top];
			top--;
			value2 = stacking[top];
			top--;
			top++;
			stacking[top] = value2 / value1;
		}
		else {
			top++;
			stacking[top] = expr[i] - '0';
		}
		i++;
	}
	return stacking[top];
}

////////////////////////////////////////////////////////////////////////////////
void destroyTree( TREE *pTree)
{
	if (pTree)
	{
		_destroy( pTree->root);
	}
		
	free( pTree);
}

////////////////////////////////////////////////////////////////////////////////
void printTree( TREE *pTree)
{
	_infix_print(pTree->root, 0);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
void traverseTree( TREE *pTree)
{
	_traverse(pTree->root);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	char expr[1024];
	
	fprintf( stdout, "\nInput an expression (postfix): ");
	
	while (fscanf( stdin, "%s", expr) == 1)
	{
		// creates a null tree
		tree = createTree();
		
		if (!tree)
		{
		  printf( "Cannot create tree\n");
		  return 100;
		}
		
		// postfix expression -> expression tree
		int ret = postfix2tree( expr, tree); //여기서 invalid한 경우에 대해 메모리 해제를 해줘야함
		if (!ret)
		{
		  	fprintf( stdout, "invalid expression!\n");
		  	destroyTree( tree); 
		  	fprintf( stdout, "\nInput an expression (postfix): ");
		  	continue;
		}
		
		// expression tree -> infix expression
		fprintf( stdout, "\nInfix expression : ");
		traverseTree( tree);
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "\n\nTree representation:\n");
		printTree(tree);
		
		// evaluate postfix expression
		float val = evalPostfix( expr);
		fprintf( stdout, "\nValue = %f\n", val);
		
		// destroy tree
		destroyTree( tree);
		
		fprintf( stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}
