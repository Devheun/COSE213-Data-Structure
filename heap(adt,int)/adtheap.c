#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time

#define MAX_ELEM	20

typedef struct
{
	void **heapArr;//heapArr가 가르키는게 void *
	int	last; //heap의 마지막 엘리먼트 index값
	int	capacity;//배열을 얼마나 크게 잡았는지, 이거 초과 불가
	int (*compare) (void *arg1, void *arg2);//reheapup과 reheapdown에서 사용
} HEAP;

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
*/
HEAP *heapCreate( int capacity, int (*compare) (void *arg1, void *arg2));

/* Free memory for heap
*/
void heapDestroy( HEAP *heap);

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heapInsert( HEAP *heap, void *dataPtr); //void *로 int를 포인팅해서 가져와서 내부에 삽입

/* Reestablishes heap by moving data in child up to correct location heap array
*/
//새로운 element가 index부터 추가돼서 거기서부터 내 부모와 비교해서 적절한 자리를 찾아가도록
static void _reheapUp( HEAP *heap, int index); //heapInsert의 내부함수

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heapDelete( HEAP *heap, void **dataOutPtr);//루트에서만 삭제가 일어남. call by reference로 인해 **

/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
//index는 사실 0으로만 사용됨 (루트에서만 삭제되니까)
static void _reheapDown( HEAP *heap, int index);//heapDelete 내부함수

/* user-defined compare function */
int compare(void *arg1, void *arg2)
{
	int *a1 = (int *)arg1;
	int *a2 = (int *)arg2;
	
	return *a1 - *a2;
}

/* Print heap array */
void heapPrint( HEAP *heap)
{
	int i;
	int last = heap->last;
	
	for( i = 0; i <= last; i++)
	{
		int *value = (int *)(heap->heapArr[i]);
		printf("%6d", *value);
	}
	printf( "\n");
}

int main(void)
{
	HEAP *heap;
	int data;
	int *dataPtr;
	int i;
	
	heap = heapCreate(MAX_ELEM, compare);
	
	srand( time(NULL));
	
	for (i = 0; i < MAX_ELEM; i++)
	{
		data = rand() % (MAX_ELEM * 3) + 1; // 1 ~ MAX_ELEM*3 random number
				
		fprintf( stdout, "Inserting %d: ", data);
		
		int *newdata = (int *)malloc( sizeof(int)); //data 만들면 newdata가 가르키게한다음에 newdata가 가르키는 내용에다 data 삽입
		*newdata = data;
		
		// insert function call
		heapInsert( heap, newdata);
		
		heapPrint( heap);
 	}

	while (heap->last >=0)
	{
		// delete function call
		heapDelete( heap, (void **)&dataPtr); //삭제된 data는 dataPtr에 들어감

		printf( "Deleting %d: ", *(int *)dataPtr);

		free(dataPtr);

		heapPrint( heap);
 	} 
	
	heapDestroy( heap);
	
	return 0;
}

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
*/
HEAP* heapCreate(int capacity, int (*compare) (void* arg1, void* arg2)) {

	HEAP* heap = (HEAP*)malloc(sizeof(HEAP));
	if (heap == NULL)return NULL;
	heap->capacity = 20;
	heap->last = -1;
	heap->heapArr = (void**)malloc(capacity * sizeof(void*));
	heap->compare = compare;

	return heap;
}

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heapInsert(HEAP* heap, void* dataPtr) { //void *로 int를 포인팅해서 가져와서 내부에 삽입
	
	(heap->last)++;
	if ((heap->last) >= 20)return 0;
	heap->heapArr[heap->last] = dataPtr;
	_reheapUp(heap, heap->last);

	return 1;

}
/* Reestablishes heap by moving data in child up to correct location heap array
*/
//새로운 element가 index부터 추가돼서 거기서부터 내 부모와 비교해서 적절한 자리를 찾아가도록
static void _reheapUp(HEAP* heap, int index) { //heapInsert의 내부함수

	
	while ((index != 0) && (heap->compare(heap->heapArr[index],heap->heapArr[(index-1)/2])>0)) {
		void* temp;
		temp = (heap->heapArr[index]);
		heap->heapArr[index] = heap->heapArr[(index - 1) / 2];
		heap->heapArr[(index - 1) / 2] = temp;
		index = (index - 1) / 2;
	}
}

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/

//루트에서만 삭제가 일어남. 
//call by reference로 인해 **

int heapDelete(HEAP* heap, void** dataOutPtr) {

	if (heap->last == -1) return 0;
	*dataOutPtr = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	heap->last--;
	_reheapDown(heap, 0);
	return 1;
}


/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
//index는 사실 0으로만 사용됨 (루트에서만 삭제되니까)
static void _reheapDown(HEAP* heap, int index) {//heapDelete 내부함수

	int left = 0;
	int right = 0;
	int large = 0;
	void* rightsub;
	void* leftsub;


	if (index * 2 + 1 <= heap->last) {
		leftsub = heap->heapArr[index * 2 + 1];
		if (index * 2 + 2 <= heap->last) {
			rightsub = heap->heapArr[index * 2 + 2];
			if (heap->compare(leftsub, rightsub) > 0) {
				large = index * 2 + 1;
			}
			else if (heap->compare(leftsub, rightsub) <= 0) {
				large = index * 2 + 2;
			}
		}
		else {
			large = index * 2 + 1;
		}

		if (heap->compare(heap->heapArr[index], heap->heapArr[large]) < 0) {
			void* temp;
			temp = heap->heapArr[index];
			heap->heapArr[index] = heap->heapArr[large];
			heap->heapArr[large] = temp;
			_reheapDown(heap, large);
		}
	}
}

/* Free memory for heap
*/
void heapDestroy(HEAP* heap) {
	/*
	for (int i = 0; i < 20; i++) {
		heap->heapArr[i] = NULL;
		free(heap->heapArr[i]);
	}
	*/

	void* dataOutPtr;


	while(heap->last>=0){
		dataOutPtr = heap->heapArr[0];
		heap->heapArr[0] = heap->heapArr[heap->last];
		heap->last--;
		free(dataOutPtr);
	}
	free(heap->heapArr);
	free(heap);
}