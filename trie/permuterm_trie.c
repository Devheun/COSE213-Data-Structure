#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE* trieCreateNode(void) { //빈 노드 하나 만드는거, index 처음에 -1초기화, subtree부분들은 다 NULL로 초기화
	
	TRIE* newTrie = (TRIE*)malloc(sizeof(TRIE));
	if (newTrie == NULL)return NULL;
	newTrie->index = -1;
	for (int i = 0; i < MAX_DEGREE; i++) {
		newTrie->subtrees[i] = NULL;
	}
	return newTrie;

}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함(이미 삽입되었으면 삽입 못하게)
// 대소문자를 소문자로 통일하여 삽입 (isupper 대문자인지 확인, tolower 소문자로 변환)
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index);

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
//str에 들어있는 앞글자부터 한문자한문자 root로부터 경로를 따라감
//a가 들어오면 0번째로 내려가서 노드접근, 중간에 안끝났는데 NULL 만나면 없는거고
// 끝까지 내려가서 노드 찾았는데 index가 -1이 아닌 값이면 사전에 있는 문자열이 저장된 노드
int trieSearch( TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]);

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function uses trieList function
*/
//str까지는 따라 내려감,abb밑에 있는 모든 서브트리를 다 순회.
void triePrefixList( TRIE *root, char *str, char *dic[]);

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size);

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *trie;
	TRIE *permute_trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p;
	int index = 0;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	trie = trieCreateNode(); // original trie
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	while (fscanf( fp, "%s", str) != EOF)
	{	
		ret = trieInsert( trie, str, index);
		
		if (ret)
		{
			num_p = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_p; i++)
				trieInsert( permute_trie, permuterms[i], index);
			
			clear_permuterms( permuterms, num_p);
			
			dic[index++] = strdup( str);
		}
	}
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search term
		if (strchr( str, '*')) 
		{
			trieSearchWildcard( permute_trie, str, dic);
		}
		// keyword search
		else 
		{
			ret = trieSearch( trie, str);
			if (ret == -1) printf( "[%s] not found!\n", str);
			else printf( "[%s] found!\n", dic[ret]);
		}
		printf( "\nQuery: ");
	}

	for (int i = 0; i < index; i++)
		free( dic[i]);
	
	trieDestroy( trie);
	trieDestroy( permute_trie);
	
	return 0;
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함(이미 삽입되었으면 삽입 못하게)
// 대소문자를 소문자로 통일하여 삽입 (isupper 대문자인지 확인, tolower 소문자로 변환)
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE* root, char* str, int dic_index) {

	TRIE* first = root;

	while (*str) {


		//영문자, EOW 일때만
		if ((*str == EOW) || ((*str >= 65) && (*str <= 90)) || ((*str >= 97) && (*str <= 122))) {

			if (*str == EOW) {
				if (first->subtrees[getIndex(EOW)] == NULL) {
					first->subtrees[getIndex(EOW)] = trieCreateNode();
				}
			}

			//대문자일때
			else if (isupper(*str)) {
				*str = tolower(*str);
				if (first->subtrees[getIndex(*str)] == NULL) {
					first->subtrees[getIndex(*str)] = trieCreateNode();
				}
			}
			//소문자일때
			else {
				if (first->subtrees[getIndex(*str)] == NULL) {
					first->subtrees[getIndex(*str)] = trieCreateNode();
				}
			}
			first = first->subtrees[getIndex(*str)];
			str++;
		}
		else return 0;
	}

	//단어의 끝부분

	if (first->index == -1) {

		first->index = dic_index;
		return 1;

	}
	else return 0;
}


/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms(char* str, char* permuterms[]) {

	

	int cnt = 0; //return 값
	int length = 0;
	if (str) {
		length = strlen(str);
	}
	else return 0;

	int len = length;

	str[len++] = '$'; //abc$ 만들어준것
	str[len] = '\0';
	char* ptr = str;

	char* s;

	int i = 0;
	while (i <= length) {

		str[len++] = *ptr;
		str[len] = '\0';
		ptr++;
		s = ptr;
		permuterms[cnt++] = strdup(s);
		i++;
	}
	str[length] = '\0';

	return cnt;
}

/* recycles memory for permuterms
*/
void clear_permuterms(char* permuterms[], int size) {

	for (int i = 0; i < size; i++) {
		if (permuterms[i]) {
			free(permuterms[i]);
		}
	}
}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
//str에 들어있는 앞글자부터 한문자한문자 root로부터 경로를 따라감
//a가 들어오면 0번째로 내려가서 노드접근, 중간에 안끝났는데 NULL 만나면 없는거고
// 끝까지 내려가서 노드 찾았는데 index가 -1이 아닌 값이면 사전에 있는 문자열이 저장된 노드
int trieSearch(TRIE* root, char* str) {

	TRIE* cur2 = root;

	while (*str) {

		if (*str == '*')return -1;

		if ((*str == EOW) || ((*str >= 65) && (*str <= 90)) || ((*str >= 97) && (*str <= 122))) {

			if (cur2->subtrees[getIndex(*str)] != NULL) {
				cur2 = cur2->subtrees[getIndex(*str)];
				str++;
			}
			else return -1;
		}

		else return -1;
	}


	if (cur2->index != -1) {
		return cur2->index;
	}
	return -1;
}

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE* root, char* dic[]) {

	if (root->index != -1) {
		printf("%s\n", dic[root->index]);
	}

	for (int i = 0; i < MAX_DEGREE; i++) {
		if (root->subtrees[i] != NULL) {
			trieList(root->subtrees[i], dic);
		}
	}
}

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function uses trieList function
*/
//str까지는 따라 내려감,abb밑에 있는 모든 서브트리를 다 순회.
void triePrefixList(TRIE* root, char* str, char* dic[]) {

	TRIE* cur = root;
	while (*str) {

		if (*str == '*') {
			trieList(cur, dic);
			return;
		}

		if (cur->subtrees[getIndex(*str)] != NULL) {
			cur = cur->subtrees[getIndex(*str)];
			str++;
		}
		else return;
	}

}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
//root- permute_trie, str - str, dic-dic
void trieSearchWildcard(TRIE* root, char* str, char* dic[]) { //*가 맨뒤에 오도록 rotation 해서 검색하는 것.

	TRIE* cur = root;
	char* p = str;
	int count = 0;

	while (*p) {
		if ((*p == '*') || ((*p >= 65) && (*p <= 90)) || ((*p >= 97) && (*p <= 122))) {
			if (*p == '*')count++;
			p++;
		}
		else return;
	}

	if (count == 1) {
		if (str[strlen(str) - 1] == '*' && str[0] != '*') { //X*의 경우
			if (cur->subtrees[getIndex(EOW)]) {
				cur = cur->subtrees[getIndex(EOW)];
				triePrefixList(cur, str, dic);
			}
			else return;
		}
	}

	if (count == 1) {
		 if (str[0] == '*' && str[strlen(str) - 1] != '*') { //*X의 경우

			str++;

			while (*str) {

				if (cur->subtrees[getIndex(*str)]) {
					cur = cur->subtrees[getIndex(*str)];
					str++;
				}
				else return;
			}

			if (cur->subtrees[getIndex(EOW)]) {
				cur = cur->subtrees[getIndex(EOW)];
				trieList(cur, dic);
			}
			else return;
		}
	}


	if (count == 2) {
		if (str[0] == '*' && str[strlen(str) - 1] == '*') { //*X*의 경우
			str++;
			triePrefixList(cur, str, dic);
		}
	}

	if (count == 1) {
		 if (str[0] != '*' && str[strlen(str) - 1] != '*') { // X*Y의 경우

			char* ptr = str;
			int cnt = 0;

			while (*str) {
				str++;
				if (*str == '*') {
					cnt++;
					break;
				}
			}

			if (cnt == 1) {
				str++;

				while (*str) {

					if (*str == '*')return;

					if (cur->subtrees[getIndex(*str)]) {
						cur = cur->subtrees[getIndex(*str)];
						str++;
					}
					else return;
				}

				if (cur->subtrees[getIndex(EOW)]) {
					cur = cur->subtrees[getIndex(EOW)];
				}
				else return;


				triePrefixList(cur, ptr, dic);
			}

		}
	}
	
	return;

}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE* root) {

	for (int i = 0; i < MAX_DEGREE; i++) {
		if (root->subtrees[i]) {
			trieDestroy(root->subtrees[i]);
		}
	}
	free(root);
}