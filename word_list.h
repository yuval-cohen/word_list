#ifndef _WORD_LIST_H_
#define _WORD_LIST_H_

#include <stdio.h>

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

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordList_BuildCharTree                                                                                    *
 *                                                                                                                     *
 * DESCRIPTION: Build character tree for word list from a word list input file                                         *
 *                                                                                                                     *
 * PARAMETERS: word_list (in/out) - pointer to word list to build its character tree                                   *
 *             file - (in) pointer to (already open) input file to read word list from                                 *
 *                          convernsion: <word><LF><word><LF><word><LF>...                                             *
 *                                                                                                                     *
 * RETURN:    RC_BAD_FORMAT - file format error                                                                        *
 *            RC_NO_MEM - no memory                                                                                    *
 *            RC_EOF - no error (end of file)                                                                          *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
RETURN_CODE WordList_BuildCharTree (WordList *word_list, FILE *file);

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordList_FindWord                                                                                         *
 *                                                                                                                     *
 * DESCRIPTION: search to find a word in a word list's character tree                                                  *
 *                                                                                                                     *
 * PARAMETERS: char_tree (in) - pointer to character tree of word list to search the word from                         *
 *             word (in) - word to search/find                                                                         *
 *                                                                                                                     *
 * RETURN: WORD_FOUND - word is found                                                                                  *
 *         PREFIX_FOUND - word is found as a prefix of another word (e.g. "aband" as a prefix of "abandon")            *
 *         NOT_FOUND - word not found (also not found as a prefix)                                                     *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
int WordList_FindWord (CharNode *char_tree, char *word);

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordList_FreeCharTree                                                                                     *
 *                                                                                                                     *
 * DESCRIPTION: free memory dynamically allocated for word list's character tree                                       *
 *                                                                                                                     *
 * PARAMETERS: char_tree (in) - pointer to character tree                                                              *
 *                                                                                                                     *
 * RETURN: none                                                                                                        *
 *                                                                                                                     *
 * NOTES: must be called to free memory at the end of word list life cycle                                             *
 *                                                                                                                     *
 ***********************************************************************************************************************/
size_t WordList_FreeCharTree (CharNode *char_tree);

#endif // _WORD_LIST_H_
