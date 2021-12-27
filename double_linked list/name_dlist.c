#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*name;
	int		freq;
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	void		*dataPtr;	//tName		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
} NODE; //데이터를 담는 노드

typedef struct
{
	int		count;
	NODE	*pos; // unused (여기서 안씀)
	NODE	*head; //list의 첫번째 노드
	NODE	*rear; //list의 마지막 노드
	int		(*compare)(const void *, const void *); // used in _search function
} LIST; //head structure

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST* createList(int (*compare)(const void*, const void*)) {

	LIST* crelist = (LIST*)malloc(sizeof(LIST));  //동적 할당 메모리 누수 주의
	if (crelist == NULL)return NULL;
	crelist->count = 0;
	crelist->head = NULL;
	crelist->rear = NULL;
	crelist->compare = compare;

	return crelist;
}
/* Deletes all data in name structure and recycles memory
*/
void destroyName(void* pNode) { //free(pNode->name); 식으로 해제해야함

	((tName*)pNode)->freq = 0;
	free(((tName*)pNode)->name);
	free(pNode);

}

/* Deletes all data in list and recycles memory
*/
void destroyList(LIST* pList,void (*callback)(void*)) {
	NODE* del = NULL;
	
	while (pList->head != NULL) {
		del = pList->head;
		pList->head = pList->head->rlink;
		callback((tName*)(del->dataPtr));
		free(del);
	}
	free(pList);

}

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search(LIST* pList, NODE** pPre, NODE** pLoc, void* pArgu) {

	*pPre = NULL; //처음에 NULL로 초기화
	*pLoc = pList->head; //head structure가 가리키고 있는 노드


	while ((*pLoc) != NULL && strcmp(((tName*)((*pLoc)->dataPtr))->name, ((tName*)pArgu)->name) <= 0) {
		//같은 값 찾은거
		if (strcmp(((tName*)((*pLoc)->dataPtr))->name, ((tName*)pArgu)->name) == 0) {
			return 1;
		}
		else { //순방향으로 찾기
			*pPre = *pLoc;
			*pLoc = (*pLoc)->rlink;
		}
	}
	return 0;
}

//void *pArgu 는 이름 구조체, 같은 이름을 가진 노드 찾아서 발견이 되면 pLoc이 가르키고, pPre는 선행자노드를 가르킴

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert(LIST* pList, NODE* pPre, void* dataInPtr) { //tName

	NODE* newnode = (NODE*)malloc(sizeof(NODE));
	if (newnode == NULL)return 0;

	//데이터 삽입
	/*
	((tName*)(newnode->dataPtr))->name = strdup(((tName*)dataInPtr)->name);
	//strcpy(((tName*)(newnode->dataPtr))->name, ((tName*)dataInPtr)->name);
	((tName*)(newnode->dataPtr))->freq = ((tName*)dataInPtr)->freq;
	*/
	newnode->dataPtr = (tName*)dataInPtr;
	newnode->llink = pPre;

	
	//처음이나 빈 리스트에 삽입
	if (pPre == NULL) {
		if (pList->head == NULL) pList->rear = newnode; //빈 리스트
		else pList->head->llink = newnode;
		newnode->llink = NULL;
		newnode->rlink = pList->head;
		pList->head = newnode;
	}
	else { //중간이나 끝
		if (pPre->rlink == NULL) { //끝에
			pList->rear = newnode;
		}
		else pPre->rlink->llink = newnode;
		newnode->rlink = pPre->rlink;
		pPre->rlink = newnode;
	}
	
	/*
	if (pPre == NULL) {

		//아무것도 없을 때
		if (pList->head == NULL) {
			pList->rear = newnode;
		}

		newnode->rlink = pList->head;
		pList->head = newnode;
	}
	else if(pPre!=NULL){
		//마지막에 넣을 때
		if (pPre->rlink == NULL) {
			pList->rear = newnode;
		}
		newnode->rlink = pPre->rlink;
		pPre->rlink = newnode;
	}
	*/
	pList->count++;

	return 1;
}
/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete(LIST* pList, NODE* pPre, NODE* pLoc, void** dataOutPtr) {//tName

	*dataOutPtr = pLoc->dataPtr;
	if (pPre == NULL) {
		if (pList->rear == pLoc) {
			pList->head = NULL;
			pList->rear = NULL;
		}
		else {
			pList->head->rlink->llink = NULL;
			pList->head = pLoc->rlink;
		}
	}
	else {
		if (pLoc->rlink == NULL) {
			pPre->rlink = NULL;
			pList->rear = pPre;
		}
		else {
			pLoc->rlink->llink = pPre;
			pPre->rlink = pLoc->rlink;
		}
	}
	free(pLoc);
	pList->count--;
}


/* Inserts data into list
	return	0 if overflow
			1 if successful
			2 if duplicated key
*/

int addNode(LIST* pList, void* dataInPtr, void (*callback)(const void*, const void*)) { //tName
	
	

	NODE* pPre = NULL; //변수 선언
	NODE* pLoc = NULL;

	int check = _search(pList, &pPre, &pLoc, dataInPtr);

	if (check) { //같은 이름이 있으면 빈도 값만 증가시켜주면 된다. count는 안커지고 그대로

		callback(pLoc->dataPtr, dataInPtr);
		return 2;
	}
	
	int a;
	a = _insert(pList, pPre, dataInPtr);
	if (a) {
		return 1;
	}
	else {
		return 0;
	}

}
 
//찾야아 될 이름, 기존의 이름
//리스트에 없으면 추가, 있으면 빈도 +

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode(LIST* pList, void* keyPtr, void** dataOut) { //tName
	NODE* pPre = NULL;
	NODE* pLoc = NULL;

	int del = _search(pList, &pPre, &pLoc, keyPtr);

	if (del) {
		_delete(pList, pPre, pLoc, dataOut);
		return 1;
	}
	else return 0;

}

//있으면 제거, 없으면 제거 못함, 삭제 될 이름을 dataOut에 연결해준다.

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList(LIST* pList, void* pArgu, void** dataOutPtr) {  //tName
	NODE* pPre = NULL;
	NODE* pLoc = NULL;

	int found = _search(pList, &pPre, &pLoc,pArgu);

	if (found) {
		*dataOutPtr = pLoc->dataPtr;
		return 1;
	}
	else return 0;
}

/* returns number of nodes in list 
*/
int countList(LIST* pList) {
	return pList->count;
}

/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList);

/* traverses data from list (forward)
*/
void traverseList(LIST* pList, void (*callback)(const void*)) {
	
	NODE* traverse;
	traverse = pList->head;

	while (traverse != NULL) {
		callback(traverse->dataPtr);
		traverse = traverse->rlink;
	}

}

/* traverses data from list (backward)
*/
void traverseListR(LIST* pList, void (*callback)(const void*)) {

	NODE* traverseb;
	traverseb = pList->rear;

	while (traverseb != NULL) {
		callback(traverseb->dataPtr);
		traverseb = traverseb->llink;
	}
}




////////////////////////////////////////////////////////////////////////////////
/* Allocates dynamic memory for a name structure, initialize fields(name, freq) and returns its address to caller
	return	name structure pointer
			NULL if overflow
*/
tName* createName(char* str, int freq) { //char *name=strdup(str); 이면 str과 동일한 내용으로 새로운 메모리 할당 받아서 주소 넘겨줌

	tName* ss = (tName*)malloc(sizeof(tName));
	if (ss == NULL)return NULL;
	ss->name = strdup(str);
	ss->freq = freq;
	
	return ss;
}


////////////////////////////////////////////////////////////////////////////////
/* gets user's input
*/
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'F':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

//밑에는 call back 함수

// compares two names in name structures
// for createList function
int cmpName( const void* pName1, const void* pName2)
{
	return strcmp( ((tName *)pName1)->name, ((tName *)pName2)->name);
}

// prints name and freq in name structure
// for traverseList and traverseListR functions
void print_name(const void *dataPtr)
{
	printf( "%s\t%d\n", ((tName *)dataPtr)->name, ((tName *)dataPtr)->freq);
}

// increases freq in name structure
// for addNode function                addNode에서 중복된 이름이 발견되었을 때 해야될 일
void increse_freq(const void *dataOutPtr, const void *dataInPtr)
{
	//기존 이름의 빈도에다가 새로운 이름의 빈도를 더한다
	((tName *)dataOutPtr)->freq += ((tName *)dataInPtr)->freq;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	
	char str[1024];
	int freq;
	
	tName *pName;
	int ret;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList( cmpName);
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	void *p;
	
	while(fscanf( fp, "%*d\t%s\t%*c\t%d", str, &freq) != EOF)
	{
		pName = createName( str, freq);
		
		ret = addNode( list, pName, increse_freq); //기존에 있는 이름이면 기존노드에다가 빈도만 더해줌
		
		if (ret == 2) // duplicated
		{
			destroyName( pName);
		}
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		void *p;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list,destroyName);
				return 0;
			
			case FORWARD_PRINT:
				traverseList( list, print_name); //처음부터 끝까지 다 돌아다니고 한 노드, 한 노드 만날 때마다
				break;                           //print_name이란 함수 돌려서 하고자 하는 일을 시킨다 
			 
			case BACKWARD_PRINT:
				traverseListR( list, print_name);
				break;
			
			case SEARCH:
				fprintf( stderr, "Input a string to find: ");
				fscanf( stdin, "%s", str);

				pName = createName( str, 0);

				if (searchList( list, pName, &p))
				{
					print_name( p);
				}
				else fprintf( stdout, "%s not found\n", str);
				
				destroyName( pName);
				break;
				
			case DELETE:
				fprintf( stderr, "Input a string to delete: ");
				fscanf( stdin, "%s", str);
				
				pName = createName( str, 0);

				if (removeNode( list, pName, &p)) // 1 리턴하면 제대로 삭제된 것
				{
					fprintf( stdout, "(%s, %d) deleted\n", ((tName *)p)->name, ((tName *)p)->freq);
					destroyName( (tName *)p);
				}
				else fprintf( stdout, "%s not found\n", str);
				
				destroyName( pName);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}
