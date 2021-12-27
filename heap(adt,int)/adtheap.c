#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time

#define MAX_ELEM	20

typedef struct
{
	void **heapArr;//heapArr�� ����Ű�°� void *
	int	last; //heap�� ������ ������Ʈ index��
	int	capacity;//�迭�� �󸶳� ũ�� ��Ҵ���, �̰� �ʰ� �Ұ�
	int (*compare) (void *arg1, void *arg2);//reheapup�� reheapdown���� ���
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
int heapInsert( HEAP *heap, void *dataPtr); //void *�� int�� �������ؼ� �����ͼ� ���ο� ����

/* Reestablishes heap by moving data in child up to correct location heap array
*/
//���ο� element�� index���� �߰��ż� �ű⼭���� �� �θ�� ���ؼ� ������ �ڸ��� ã�ư�����
static void _reheapUp( HEAP *heap, int index); //heapInsert�� �����Լ�

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heapDelete( HEAP *heap, void **dataOutPtr);//��Ʈ������ ������ �Ͼ. call by reference�� ���� **

/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
//index�� ��� 0���θ� ���� (��Ʈ������ �����Ǵϱ�)
static void _reheapDown( HEAP *heap, int index);//heapDelete �����Լ�

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
		
		int *newdata = (int *)malloc( sizeof(int)); //data ����� newdata�� ����Ű���Ѵ����� newdata�� ����Ű�� ���뿡�� data ����
		*newdata = data;
		
		// insert function call
		heapInsert( heap, newdata);
		
		heapPrint( heap);
 	}

	while (heap->last >=0)
	{
		// delete function call
		heapDelete( heap, (void **)&dataPtr); //������ data�� dataPtr�� ��

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
int heapInsert(HEAP* heap, void* dataPtr) { //void *�� int�� �������ؼ� �����ͼ� ���ο� ����
	
	(heap->last)++;
	if ((heap->last) >= 20)return 0;
	heap->heapArr[heap->last] = dataPtr;
	_reheapUp(heap, heap->last);

	return 1;

}
/* Reestablishes heap by moving data in child up to correct location heap array
*/
//���ο� element�� index���� �߰��ż� �ű⼭���� �� �θ�� ���ؼ� ������ �ڸ��� ã�ư�����
static void _reheapUp(HEAP* heap, int index) { //heapInsert�� �����Լ�

	
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

//��Ʈ������ ������ �Ͼ. 
//call by reference�� ���� **

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
//index�� ��� 0���θ� ���� (��Ʈ������ �����Ǵϱ�)
static void _reheapDown(HEAP* heap, int index) {//heapDelete �����Լ�

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