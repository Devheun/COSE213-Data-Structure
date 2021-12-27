#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_YEAR_DURATION	10	// 기간
#define LINEAR_SEARCH 0
#define BINARY_SEARCH 1

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName* data;		// 이름 배열의 포인터
} tNames;


////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언(declaration)

// 연도별 입력 파일을 읽어 이름 정보(이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// names->capacity는 2배씩 증가

// 선형탐색(linear search) 버전
void load_names_lsearch(FILE* fp, int year_index, tNames* names);

// 이진탐색(binary search) 버전
void load_names_bsearch(FILE* fp, int year_index, tNames* names);

// 구조체 배열을 화면에 출력
void print_names(tNames* names, int num_year);

// qsort를 위한 비교 함수
// 정렬 기준 : 이름(1순위), 성별(2순위)
int compare(const void* n1, const void* n2);

////////////////////////////////////////////////////////////////////////////////
// 함수 정의 (definition)

// 이름 구조체를 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames* create_names(void)
{
	tNames* pnames = (tNames*)malloc(sizeof(tNames));

	pnames->len = 0;
	pnames->capacity = 1;
	pnames->data = (tName*)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames* pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	tNames* names;
	int mode;

	FILE* fp;
	int num_year = 0;

	if (argc <= 2)
	{
		fprintf(stderr, "Usage: %s mode FILE...\n\n", argv[0]);
		fprintf(stderr, "mode\n\t-l\n\t\twith linear search\n\t-b\n\t\twith binary search\n");
		return 1;
	}

	if (strcmp(argv[1], "-l") == 0) mode = LINEAR_SEARCH;
	else if (strcmp(argv[1], "-b") == 0) mode = BINARY_SEARCH;
	else {
		fprintf(stderr, "unknown mode : %s\n", argv[1]);
		return 1;
	}

	// 이름 구조체 초기화
	names = create_names();

	// 첫 연도 알아내기 "yob2009.txt" -> 2009
	int start_year = atoi(&argv[2][strlen(argv[2]) - 8]); //atoi 함수 : 문자열을 정수로 변환

	for (int i = 2; i < argc; i++)
	{
		num_year++;
		fp = fopen(argv[i], "r");
		if (!fp) {
			fprintf(stderr, "cannot open file : %s\n", argv[i]);
			return 1;
		}

		int year = atoi(&argv[i][strlen(argv[i]) - 8]); // ex) "yob2009.txt" -> 2009

		fprintf(stderr, "Processing [%s]..\n", argv[i]);

		if (mode == LINEAR_SEARCH)
		{
			// 연도별 입력 파일(이름 정보)을 구조체에 저장
			// 선형탐색 모드
			load_names_lsearch(fp, year - start_year, names);

		}
		else // (mode == BINARY_SEARCH)
		{
			// 이진탐색 모드
			load_names_bsearch( fp, year-start_year, names);

			// 정렬 (이름순 (이름이 같은 경우 성별순))
			qsort( names->data, names->len, sizeof(tName), compare);
		}
		fclose(fp);

	}

	// 정렬 (이름순 (이름이 같은 경우 성별순))
	qsort(names->data, names->len, sizeof(tName), compare);

	// 이름 구조체를 화면에 출력
	print_names( names, num_year);

	// 이름 구조체 해제
	destroy_names(names);

	return 0;
}

void load_names_lsearch(FILE* fp, int year_index, tNames* names) {


	int visited = 0;
	const int max = 1024;
	char line[max];
	char line1[max];

	char name1[20];
	char sex1;
	int freq;
	


	while (!feof(fp)) {

		//동적 할당 처리
		if ((names->capacity) == (names->len)) {
             names->capacity *= 2;
             names->data=(tName*)realloc(names->data, sizeof(tName) * names->capacity);
        }

		fgets(line, max, fp); //한줄 한줄씩 읽기
		strcpy(line1,line);

		//char* ptr = strtok(line, ",");
		if(feof(fp))break;

		sscanf(line, "%[^','],%c,%d", name1, &sex1, &freq);
		
		/*
		//이름과 성별이 전부 같은 경우가 있는 경우를 확인.
		for (int i = 0; i < names->len; i++) {

			if (strcmp(names->data[i].name,ptr)== 0) {
				ptr = strtok(NULL, ",");
				if (names->data[i].sex == ptr[0]) {
					ptr = strtok(NULL, ",");
					//이름과 성별이 전부 같은 경우 빈도수만 다르게해서 동일데이터에 삽입
					names->data[i].freq[year_index] = atoi(ptr);
					visited = 1;
					break;
				}

				else {
					char line2[max];
					strcpy(line2, line1);
					ptr = strtok(line2, ",");
					//ptr = strtok(line, ",");
					continue;
				}
			}

			else {
				continue;
			}
		}
		*/
		
		
		for (int i = 0; i < names->len; i++) {
			//이름 같고
			if (strcmp(name1, names->data[i].name) == 0) {
				//성별 같으면
				if (names->data[i].sex==sex1) {
					visited = 1;
					names->data[i].freq[year_index] = freq;
					break;
				}
			}
		}
		


		//이름과 성별이 전부 같은 경우가 없을 경우
		if (!visited) {

			/*
			ptr = strtok(line1, ",");

			int i = 0;
			while (ptr != NULL) {

				if (i == 0) {
					strcpy(names->data[names->len].name,ptr);
				}
				else if (i == 1) {
					names->data[names->len].sex=ptr[0];
					//strcpy(names->data[names->len].sex,ptr);
				}
				else if (i == 2) {
					memset(names->data[names->len].freq, 0, sizeof(int)*MAX_YEAR_DURATION);
					names->data[names->len].freq[year_index] = atoi(ptr);
				}

				ptr = strtok(NULL, ",");
				i++;
			}
			*/
			strcpy(names->data[names->len].name, name1);
			names->data[names->len].sex = sex1;
			memset(names->data[names->len].freq, 0, sizeof(int) * MAX_YEAR_DURATION);
			names->data[names->len].freq[year_index] = freq;
			names->len++;

		}
		visited = 0;
	}


}

void load_names_bsearch(FILE* fp, int year_index, tNames* names) {


	int visited = 0;
	const int max = 1024;
	char line[max];
	char line1[max];
	int prev_len = names->len;
	tName* key; //bssearch key값으로 구조체 선언
	int index; //bsearch해서 결과 찾았을때 index값

	while (!feof(fp)) {

		key = (tName*)malloc(sizeof(tName));


		//동적 할당 처리
		if ((names->capacity) == (names->len)) {
			names->capacity *= 2;
			names->data = (tName*)realloc(names->data, sizeof(tName) * names->capacity);
		}


		fgets(line, max, fp);
		strcpy(line1, line);
		if (feof(fp))break;

		sscanf(line, "%[^','],%c,%d", key->name, &key->sex, &key->freq[0]);
		//      printf("%s %c %d\n",key->name,key->sex,key->freq[0]);
		tName* find;

		//이진 탐색으로 이전데이터들에 같은이름과 성별이 있는지 확인
		find = (tName*)bsearch(key, names->data, prev_len, sizeof(tName), compare);


		//이진 탐색을 통해서 같은 이름과 같은 성별을 찾았을 때
		if (find != NULL) {
			//         printf("%s %c %d\n",key->name,key->sex,key->freq[0]);

			//이름과 성별이 전부 같은 경우 빈도수만 다르게해서 동일데이터에 삽입
			find->freq[year_index] = key->freq[0];

			//index = (char*)find - (char*)names->data;
			//names->data[index].freq[year_index] = atoi(ptr);
		}
		
		//이진 탐색을 통해서 같은 이름을 찾지 못했을 때

		else {

			/*
			char* ptr = strtok(line1, ",");

			int i = 0;
			while (ptr != NULL) {

				if (i == 0) {
					strcpy(names->data[names->len].name, ptr);
				}
				else if (i == 1) {
					names->data[names->len].sex = ptr[0];
					//strcpy(names->data[names->len].sex,ptr);
				}
				else if (i == 2) {
					memset(names->data[names->len].freq, 0, sizeof(int) * MAX_YEAR_DURATION);
					names->data[names->len].freq[year_index] = atoi(ptr);
				}

				ptr = strtok(NULL, ",");
				i++;
			}
			*/
			strcpy(names->data[names->len].name, key->name);
			names->data[names->len].sex = key->sex;
			memset(names->data[names->len].freq, 0, sizeof(int) * MAX_YEAR_DURATION);
			names->data[names->len].freq[year_index] = key->freq[0];
			names->len++;
			//qsort(names->data, names->len, sizeof(tName), compare);
			//free(ptr);
		}
		free(key);
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
/*
// 정렬 (이름순 (이름이 같은 경우 성별순))
int compare(const void* n1, const void* n2) {

	
   tName name1 = *(tName*)n1;
   tName name2 = *(tName*)n2;
   if(strcmp(name1.name,name2.name)==0){
	return name1.sex>name2.sex;
   }   
   
   else{
   return strcmp(name1.name,name2.name);
   }

}
*/

