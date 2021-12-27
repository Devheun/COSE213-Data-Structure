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
TRIE* trieCreateNode(void) { //�� ��� �ϳ� ����°�, index ó���� -1�ʱ�ȭ, subtree�κе��� �� NULL�� �ʱ�ȭ
	
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
// ����! ��Ʈ���� �ߺ� �������� �ʵ��� üũ�ؾ� ��(�̹� ���ԵǾ����� ���� ���ϰ�)
// ��ҹ��ڸ� �ҹ��ڷ� �����Ͽ� ���� (isupper �빮������ Ȯ��, tolower �ҹ��ڷ� ��ȯ)
// �����ڿ� EOW �� ���ڸ� �����ϴ� ���ڿ��� �������� ����
int trieInsert( TRIE *root, char *str, int dic_index);

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
//str�� ����ִ� �ձ��ں��� �ѹ����ѹ��� root�κ��� ��θ� ����
//a�� ������ 0��°�� �������� �������, �߰��� �ȳ����µ� NULL ������ ���°Ű�
// ������ �������� ��� ã�Ҵµ� index�� -1�� �ƴ� ���̸� ������ �ִ� ���ڿ��� ����� ���
int trieSearch( TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]);

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function uses trieList function
*/
//str������ ���� ������,abb�ؿ� �ִ� ��� ����Ʈ���� �� ��ȸ.
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
// ����! ��Ʈ���� �ߺ� �������� �ʵ��� üũ�ؾ� ��(�̹� ���ԵǾ����� ���� ���ϰ�)
// ��ҹ��ڸ� �ҹ��ڷ� �����Ͽ� ���� (isupper �빮������ Ȯ��, tolower �ҹ��ڷ� ��ȯ)
// �����ڿ� EOW �� ���ڸ� �����ϴ� ���ڿ��� �������� ����
int trieInsert(TRIE* root, char* str, int dic_index) {

	TRIE* first = root;

	while (*str) {


		//������, EOW �϶���
		if ((*str == EOW) || ((*str >= 65) && (*str <= 90)) || ((*str >= 97) && (*str <= 122))) {

			if (*str == EOW) {
				if (first->subtrees[getIndex(EOW)] == NULL) {
					first->subtrees[getIndex(EOW)] = trieCreateNode();
				}
			}

			//�빮���϶�
			else if (isupper(*str)) {
				*str = tolower(*str);
				if (first->subtrees[getIndex(*str)] == NULL) {
					first->subtrees[getIndex(*str)] = trieCreateNode();
				}
			}
			//�ҹ����϶�
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

	//�ܾ��� ���κ�

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

	

	int cnt = 0; //return ��
	int length = 0;
	if (str) {
		length = strlen(str);
	}
	else return 0;

	int len = length;

	str[len++] = '$'; //abc$ ������ذ�
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
//str�� ����ִ� �ձ��ں��� �ѹ����ѹ��� root�κ��� ��θ� ����
//a�� ������ 0��°�� �������� �������, �߰��� �ȳ����µ� NULL ������ ���°Ű�
// ������ �������� ��� ã�Ҵµ� index�� -1�� �ƴ� ���̸� ������ �ִ� ���ڿ��� ����� ���
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
//str������ ���� ������,abb�ؿ� �ִ� ��� ����Ʈ���� �� ��ȸ.
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
void trieSearchWildcard(TRIE* root, char* str, char* dic[]) { //*�� �ǵڿ� ������ rotation �ؼ� �˻��ϴ� ��.

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
		if (str[strlen(str) - 1] == '*' && str[0] != '*') { //X*�� ���
			if (cur->subtrees[getIndex(EOW)]) {
				cur = cur->subtrees[getIndex(EOW)];
				triePrefixList(cur, str, dic);
			}
			else return;
		}
	}

	if (count == 1) {
		 if (str[0] == '*' && str[strlen(str) - 1] != '*') { //*X�� ���

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
		if (str[0] == '*' && str[strlen(str) - 1] == '*') { //*X*�� ���
			str++;
			triePrefixList(cur, str, dic);
		}
	}

	if (count == 1) {
		 if (str[0] != '*' && str[strlen(str) - 1] != '*') { // X*Y�� ���

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