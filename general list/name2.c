#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

// 함수 원형 선언

// 연도별 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함 (qsort 함수 사용하지 않음)
// 이미 등장한 이름인지 검사하기 위해 bsearch 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 2배씩 증가
void load_names( FILE *fp, int start_year, tNames *names);

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year);

// bsearch를 위한 비교 함수
int compare( const void *n1, const void *n2);

// 이진탐색 함수
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// 함수 정의

// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}
	
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
	FILE *fp;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n\n", argv[0]);
		return 1;
	}

	// 이름 구조체 초기화
	names = create_names();
	
	fp = fopen( argv[1], "r");
	if (!fp)
	{
		fprintf( stderr, "cannot open file : %s\n", argv[1]);
		return 1;
	}

	fprintf( stderr, "Processing [%s]..\n", argv[1]);
		
	// 연도별 입력 파일(이름 정보)을 구조체에 저장
	load_names( fp, 2009, names);
	
	fclose( fp);
	
	// 이름 구조체를 화면에 출력
	print_names( names, MAX_YEAR_DURATION);

	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}

void load_names(FILE* fp, int start_year, tNames* names) {

	const int max = 1024;
	char line[max];
	char line1[max];
	int prev_len = names->len;

	tName* key; //bssearch key값으로 구조체 선언
	key = (tName*)malloc(sizeof(tName));
	int index; //binary_search return 값 //
	int year;

	while (!feof(fp)) {

		if ((names->capacity) == (names->len)) {
			names->capacity *= 2;
			names->data = (tName*)realloc(names->data, sizeof(tName) * names->capacity);
		}

		fgets(line, max, fp);


		if (feof(fp))break;

		sscanf(line,"%d\t%s\t%c\t%d",&year,key->name,&key->sex,&key->freq[0]);
		year = year - start_year;

		tName* find;
		find = (tName*)bsearch(key, names->data, names->len, sizeof(tName), compare);

		//찾았으면
		if (find != NULL) {

			find->freq[year] = key->freq[0];
		}

		//새로 등장한 이름이면 구조체에 추가해줘야함 (ordered list 유지한 채로)
		else {

			//binary_search : 키값,볼 배열,엘리먼트 개수,배열 사이즈,compare (bsearch와 동일하게)
			index = binary_search(key, names->data, names->len, sizeof(tName), compare);

			//memmove를 통해 새로 저장할 이름의 메모리 공간을 확보해준다. 찾은 index를 바탕으로 (한칸 뒤로 땡겨준다)
			memmove(&(names->data[index + 1]), &(names->data[index]), ((names->len) - index) * sizeof(tName));
			
			//하나 땡기고 빈 곳 채워주기
			strcpy(names->data[index].name, key->name);
			names->data[index].sex = key->sex;
			memset(names->data[index].freq, 0, sizeof(int) * MAX_YEAR_DURATION);
			names->data[index].freq[year] = key->freq[0];
			names->len++;

		}
		//free(key);
	}
	free(key);
}

void print_names(tNames* names, int num_year) {

	for (int i = 0; i < names->len; i++) {
		printf("%s\t%c\t", names->data[i].name, names->data[i].sex);
		for (int j = 0; j < num_year; j++) {
			printf("%d\t", names->data[i].freq[j]);
		}
		printf("\n");
	}

}

int compare(const void* n1, const void* n2) {

	tName name1 = *(tName*)n1;
	tName name2 = *(tName*)n2;
	if (strcmp(name1.name, name2.name) == 0) {
		if (name1.sex > name2.sex) return 1;
		else if (name1.sex == name2.sex) return 0;
		else return -1;
	}

	else {
		return strcmp(name1.name, name2.name);
	}
}

int binary_search(const void* key, const void* base, size_t nmemb, size_t size, int (*compare)(const void*, const void*)) {

	int left = 0;
	int right = nmemb - 1;
	int mid;
	int order;

	//교차되면 탈출
	while (left <= right) {

		mid = (left + right) / 2;
		//key값과 배열을 비교해야하기때문에 compare가 필요
		//base는 names->data 배열, nmemb는 names->len, size는 sizeof(tName)

		//포인터 덧셈에서 void포인터 때문에 size를 곱해주어야한다.
		order = compare(key,base + size* mid);

		if (order < 0) {
			right = mid - 1;
		}
		else if(order > 0){
			left = mid + 1;
		}


	}
	return left;

}









