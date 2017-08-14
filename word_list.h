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
//typedef struct _WordList
//{
//	LetterNode *list;
//   
//	size_t allocated_bytes;
//	size_t allocated_Blocks;
//	size_t freed_Blocks;
//
//} WordList;

int find_word (LetterNode *wordlist, char *word);
RETURN_CODE build_wordlist (LetterNode **wordlist, FILE *file);
void free_wordlist (LetterNode *wordlist);

#endif // _WORD_LIST_H_
