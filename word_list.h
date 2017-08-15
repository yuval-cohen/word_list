#ifndef _WORD_LIST_H_
#define _WORD_LIST_H_

#define NOT_FOUND		((int)0)
#define WORD_FOUND		((int)1)
#define PREFIX_FOUND	((int)2)

#define READ_BUF_SIZE	256
#define READ_BLOCK_LEN	100

#define MAX_WORD_LEN	50

/* character node struct */
typedef struct _CharNode 
{
   char ch;
   unsigned char is_word;

   struct _CharNode *adjacent;
   struct _CharNode *next;

} CharNode;

/* word list struct */
typedef struct _WordList
{
	/* word list character tree */
	CharNode *char_tree;

	size_t no_of_words;
	size_t allocated_nodes;
	size_t freed_nodes;

} WordList;

RETURN_CODE build_wordlist (WordList *word_list, FILE *file);
int find_word (CharNode *char_tree, char *word);
size_t free_char_tree (CharNode *char_tree);

#endif // _WORD_LIST_H_
