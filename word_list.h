#ifndef _WORD_LIST_H_
#define _WORD_LIST_H_

#define NOT_FOUND		((int)0)
#define WORD_FOUND		((int)1)
#define PREFIX_FOUND	((int)2)

#define READ_BUF_SIZE	256
#define READ_BLOCK_LEN	100

#define MAX_WORD_LEN	50

/* single letter node */
typedef struct _LetterNode 
{
   char letter;
   unsigned char is_word;

   struct _LetterNode *adjacent;
   struct _LetterNode *next;

} LetterNode;

/* word list struct */
typedef struct _WordList
{
	/* word list letter tree */
	LetterNode *letter_tree;

	size_t no_of_words;
	size_t allocated_nodes;
	size_t freed_nodes;

} WordList;

RETURN_CODE build_wordlist (WordList *word_list, FILE *file);
int find_word (LetterNode *letter_tree, char *word);
size_t free_letter_tree (LetterNode *letter_tree);

#endif // _WORD_LIST_H_
