#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$'	// end of word

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
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
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
void triePrefixList( TRIE *root, char *str, char *dic[]);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
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
	
	trie = trieCreateNode();
	
	while (fscanf( fp, "%s", str) != EOF)
	{
		ret = trieInsert( trie, str, index);

		if (ret) 
			dic[index++] = strdup( str);
	}
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search 전방일치
		if (str[strlen(str)-1] == '*')
		{
			str[strlen(str)-1] = 0;
			triePrefixList( trie, str, dic); //그 문자열로 시작하는 모든 entry를 찾는다, str을 prefix로 삼아서 trie에서 노드를 따라 내려간 다음
			//거기서 부터는 trieList 호출해서 걔로부터 밑에 있는 모든 노드들을 방문하도록 시켜서 화면에 출력
		}
		// keyword search 주어진 문자열 그대로가 사전에 저장되어있는지
		else
		{
			ret = trieSearch( trie, str); //딱 해당되는 문자열만 찾는거
			if (ret == -1) printf( "[%s] not found!\n", str);
			else printf( "[%s] found!\n", dic[ret]);
		}
		
		printf( "\nQuery: ");
	}
	
	for (int i = 0; i < index; i++)
		free( dic[i]);
	
	trieDestroy( trie);
	
	return 0;
}

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
			//대문자일때
			if (isupper(*str)) {
				*str = tolower(*str);
				if (first->subtrees[getIndex(*str)] == NULL) {
					first->subtrees[getIndex(*str)] = trieCreateNode();
				}
			}
			//대문자 아닐때
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

	if (first->index==-1) {

		first->index = dic_index;
		return 1;
	
	}
	else return 0;
}

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function uses trieList function
*/
void triePrefixList(TRIE* root, char* str, char* dic[]) {//그 문자열로 시작하는 모든 entry를 찾는다, str을 prefix로 삼아서 trie에서 노드를 따라 내려간 다음
			//거기서 부터는 trieList 호출해서 걔로부터 밑에 있는 모든 노드들을 방문하도록 시켜서 화면에 출력

	TRIE* cur = root;
	while (*str) {

		if (*str == '*')return;

		if (cur->subtrees[getIndex(*str)] != NULL) {
			cur = cur->subtrees[getIndex(*str)];
			str++;
		}
		else return;
	}

	//while 탈출했을 때
	trieList(cur, dic);

}

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE* root, char* dic[]) {

	if (root->index !=-1) {
		printf("%s\n", dic[root->index]);
	}

	for (int i = 0; i < MAX_DEGREE; i++) {
		if (root->subtrees[i] != NULL) {
			trieList(root->subtrees[i],dic);
		}
	}

}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
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

	if (cur2->index!=-1) {
		return cur2->index;
	}
	return -1;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE* root) {

	for (int i = 0; i < MAX_DEGREE; i++) {
		if (root->subtrees[i] != NULL) {
			trieDestroy(root->subtrees[i]);
		}
	}
	free(root);
}