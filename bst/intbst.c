#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> // time

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
    int         data;
    struct node* left;
    struct node* right;
} NODE;

typedef struct
{
    NODE* root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
   return   head node pointer
         NULL if overflow
*/
TREE* BST_Create(void) {

    TREE* newtree = (TREE*)malloc(sizeof(TREE));
    newtree->root = NULL;
    if (newtree == NULL)return NULL;
    else return newtree;
}

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy(TREE* pTree);

/* internal function (not mandatory)
*/
static void _destroy(NODE* root);

/* Inserts new data into the tree
   return   1 success
         0 overflow
*/
int BST_Insert(TREE* pTree, int data); //data 값을 가지도록 node 하나 만들고, 

/* internal function (not mandatory)
*/
static void _insert(NODE* root, NODE* newPtr); //만들어진 node : newPtr을 기존 tree에다가 삽입시키는것

NODE* _makeNode(int data); //노드 하나 만들기

/* Deletes a node with dltKey from the tree
   return   1 success
         0 not found
*/
int BST_Delete(TREE* pTree, int dltKey); //Tree와 어떤 key를 삭제할거냐를 받는다.

/* internal function
   success is 1 if deleted; 0 if not
   return   pointer to root
*/
static NODE* _delete(NODE* root, int dltKey, int* success); //root가 (call by value)바뀔수 있으니 함수 밖에서 알도록 return 값으로 NODE* 사용

/* Retrieve tree for the node containing the requested key
   return   address of data of the node containing the key
         NULL not found
*/
int* BST_Retrieve(TREE* pTree, int key);

/* internal function
   Retrieve node containing the requested key
   return   address of the node containing the key
         NULL not found
*/
static NODE* _retrieve(NODE* root, int key);

/* prints tree using inorder traversal
*/
void BST_Traverse(TREE* pTree);
static void _traverse(NODE* root);

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE* pTree);
/* internal traversal function
*/
static void _infix_print(NODE* root, int level);

/*
   return 1 if the tree is empty; 0 if not
*/
int BST_Empty(TREE* pTree);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    TREE* tree;
    int data;

    // creates a null tree
    tree = BST_Create();

    if (!tree)
    {
        printf("Cannot create a tree!\n");
        return 100;
    }

    fprintf(stdout, "How many numbers will you insert into a BST: ");

    int numbers;
    scanf("%d", &numbers);

    fprintf(stdout, "Inserting: ");

    srand(time(NULL));
    for (int i = 0; i < numbers; i++)
    {
        data = rand() % (numbers * 3) + 1; // random number (1 ~ numbers * 3)

        fprintf(stdout, "%d ", data);

        // insert funtion call
        int ret = BST_Insert(tree, data);
        if (!ret) break;
    }
    fprintf(stdout, "\n");

    // inorder traversal
    fprintf(stdout, "Inorder traversal: ");
    BST_Traverse(tree);
    fprintf(stdout, "\n");

    // print tree with right-to-left infix traversal
    fprintf(stdout, "Tree representation:\n");
    printTree(tree);

    while (1)
    {
        fprintf(stdout, "Input a number to delete: ");
        int num;
        int ret = scanf("%d", &num);
        if (ret != 1) break;

        ret = BST_Delete(tree, num);
        if (!ret)
        {
            fprintf(stdout, "%d not found\n", num);
            continue;
        }

        // print tree with right-to-left infix traversal
        fprintf(stdout, "Tree representation:\n");
        printTree(tree);

        if (BST_Empty(tree))
        {
            fprintf(stdout, "Empty tree!\n");
            break;
        }
    }

    BST_Destroy(tree);

    return 0;
}

int BST_Insert(TREE* pTree, int data) {     //data 값을 가지도록 node 하나 만들고, 

    NODE* newNode = _makeNode(data);
    if (newNode == NULL)return 0;
    if (pTree->root == NULL) pTree->root = newNode;
    else _insert(pTree->root, newNode);


    return 1;
}

static void _insert(NODE* root, NODE* newPtr) { //만들어진 node : newPtr을 기존 tree에다가 삽입시키는것
    NODE* current = root;

    while (current != NULL) {
        if (current->data <= newPtr->data) {
            if (current->right == NULL) {
                current->right = newPtr;
                break;
            }
            else current = current->right;
        }
        if (current->data > newPtr->data) {
            if (current->left == NULL) {
                current->left = newPtr;
                break;
            }
            else current = current->left;
        }
    }

}

NODE* _makeNode(int data) { //노드 하나 만들기

    NODE* makeNode = (NODE*)malloc(sizeof(NODE));
    makeNode->data = data;
    makeNode->left = NULL;
    makeNode->right = NULL;

    return makeNode;
}

/* prints tree using inorder traversal
*/
void BST_Traverse(TREE* pTree) {

    _traverse(pTree->root);
    return;
}

static void _traverse(NODE* root) {

    if (root != NULL) {
        if (root->left != NULL) {
            _traverse(root->left);
        }
        printf("%d ", root->data);
        if (root->right != NULL) {
            _traverse(root->right);
        }
    }

}

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE* pTree) {

    _infix_print(pTree->root, 0);
    return;
}

/* internal traversal function
*/
static void _infix_print(NODE* root, int level) {

    if (root != NULL) {
        if (root->right != NULL) {
            _infix_print(root->right, level + 1);

        }
        for (int i = 0; i < level; i++)printf("\t");
        printf("%d", root->data);
        printf("\n");

        if (root->left != NULL) {
            _infix_print(root->left, level + 1);
        }
        return;
    }
    else return;
}

/* Deletes a node with dltKey from the tree
   return   1 success
         0 not found
*/
int BST_Delete(TREE* pTree, int dltKey) { //Tree와 어떤 key를 삭제할거냐를 받는다.

    int success;
    int* reikey = BST_Retrieve(pTree, dltKey);
    if (reikey == NULL)return 0; //값 못찾았을 때
    else {
        pTree->root = _delete(pTree->root, dltKey, &success);
    }
    if (success == 0)return 0;
    else return 1;

}

/* internal function
   success is 1 if deleted; 0 if not
   return   pointer to root
*/
static NODE* _delete(NODE* root, int dltKey, int* success) { //root가 (call by value)바뀔수 있으니 함수 밖에서 알도록 return 값으로 NODE* 사용

    NODE* parent = NULL;
    NODE* cur = root;

    while (cur->data != dltKey) { //cur로 삭제할 노드 찾기
        if (cur->data > dltKey) {
            parent = cur;
            cur = cur->left;
            if (cur == NULL)break;
        }
        else {
            parent = cur;
            cur = cur->right;
            if (cur == NULL)break;
        }
    }

    if (cur == NULL) { //못찾았으면
        *success = 0;
        return root;
    }

    else if (cur->left == NULL && cur->right == NULL) { //자식 X
        if (root == cur) root = NULL;
        else {
            if (parent->left == cur) parent->left = NULL;
            if (parent->right == cur) parent->right = NULL;
        }
        free(cur);
    }
    else if (cur->right == NULL) { //왼쪽 자식만
        if (root == cur) root = cur->left;
        else {
            if (parent->left == cur) {
                parent->left = cur->left;
            }
            if (parent->right == cur) {
                parent->right = cur->left;
            }
        }
        free(cur);
    }
    else if (cur->left == NULL) { //오른쪽 자식만
        if (root == cur) root = cur->right;
        else {
            if (parent->left == cur) {
                parent->left = cur->right;
            }
            if (parent->right == cur) {
                parent->right = cur->right;
            }
        }
        free(cur);
    }

    else { //자식 두개
        NODE* a = cur->right;
        NODE* aparent = cur;

        while (a->left != NULL) {
            aparent = a;
            a = a->left;
        }

        cur->data = a->data;

        if (cur == aparent) {
            cur->right = a->right;
            aparent->right = a->right;
            free(a);
        }
        else {
            aparent->left = a->right;
            free(a);
        }
    }

    *success = 1;
    return root;
}

/* Retrieve tree for the node containing the requested key
   return   address of data of the node containing the key
         NULL not found
*/
int* BST_Retrieve(TREE* pTree, int key) {

    NODE* retri = _retrieve(pTree->root, key);
    if (retri == NULL)return NULL;
    return &(retri->data);
}

/* internal function
   Retrieve node containing the requested key
   return   address of the node containing the key
         NULL not found
*/
static NODE* _retrieve(NODE* root, int key) {

    if (root == NULL)return NULL;
    else {
        if (key < root->data) {
            return _retrieve(root->left, key);
        }
        else if (key > root->data) {
            return _retrieve(root->right, key);
        }
        else { //키 값 찾았을 때
            return root;
        }
    }
}
/*
   return 1 if the tree is empty; 0 if not
*/
int BST_Empty(TREE* pTree) {
    if (pTree->root == NULL)return 1;
    else return 0;
}


/* Deletes all data in tree and recycles memory
*/
void BST_Destroy(TREE* pTree) {

    if (pTree) {
        _destroy(pTree->root);
    }
    free(pTree);
}

/* internal function (not mandatory)
*/
static void _destroy(NODE* root) {

    if (root != NULL) {
        if (root->left != NULL) {
            _destroy(root->left);
        }
        if (root->right != NULL) {
            _destroy(root->right);
        }
        free(root);
    }
    else return;
}