#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gen_defs.h"

#include "word_list.h"

/*
 CONCEPT: word list as a tree of character nodes

 e.g. word list with the following words (. means IS_WORD is true; -> mean NEXT pointer; | (n times) means ADJACENT pointer): 
 aa
 aah
 aahed
 aahing
 aahs
 aal
 aalii
 aaliis

 Head->a->a.->h.->e->d.
			  |   |
			  |   i->n->g.
			  |   |
			  |   s.
			  |
			  l.->i->i.->s.    
 */

/*******************************************************************************************************************************************************/
/************************************************************ internal functions declation *************************************************************/
/*******************************************************************************************************************************************************/
static RETURN_CODE readNextWordFromFile (FILE *file, char read_buffer[READ_BUF_SIZE], char *word);
static RETURN_CODE addNewWordToCharTree (CharNode **char_tree, char *word, size_t *allocated_nodes);
static CharNode* newCharNode (CharNode **char_node, char ch, unsigned char is_word);

/*******************************************************************************************************************************************************/
/************************************************************ internal functions definition ************************************************************/
/*******************************************************************************************************************************************************/
/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: readNextWordFromFile                                                                                      *
 *                                                                                                                     *
 * DESCRIPTION: Called in a loop to read the next word from a word list file while                                     *
 *              buffering the read content into a read buffer                                                          *
 *                                                                                                                     *
 * PARAMETERS:  file - (in) pointer to (already open) word list input file                                             *
 *              read_buffer - (in/out) buffer to read the file content to                                              *
 *              word - (out) next word read                                                                            *
 *                                                                                                                     *
 * RETURN: RC_BAD_FORMAT - file format error                                                                           *
 *         RC_NO_ERROR - word found (no error)                                                                         *
 *         RC_EOF - end of file (no more words)                                                                        *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
 static RETURN_CODE readNextWordFromFile (FILE *file, char read_buffer[READ_BUF_SIZE], char *word)
{
	char *new_line_ptr;
	size_t read_bytes, cur_length, word_len;

	new_line_ptr = strstr(read_buffer, "\n");
	if (new_line_ptr == NULL)
	{
		/* read another block from file into read_buffer */
		cur_length = strlen(read_buffer);
		if ((cur_length +1 + READ_BLOCK_LEN) > READ_BUF_SIZE)
		{
			return RC_BAD_FORMAT;
		}
		read_bytes = fread(read_buffer+strlen(read_buffer), 1, READ_BLOCK_LEN, file);
		new_line_ptr = strstr(read_buffer, "\n");
		if ((read_bytes == 0) || (new_line_ptr == NULL))
		{
			return RC_EOF;
		}
		*(read_buffer+cur_length+read_bytes) = '\0';
   }

	/* check word length does not exceed maximum */
	word_len = new_line_ptr - read_buffer;
	if (word_len > MAX_WORD_LEN)
	{
		return RC_BAD_FORMAT;
	}
   
	/* copy next line into from read_buffer into line */
	strncpy(word, read_buffer, new_line_ptr-read_buffer);
	word[new_line_ptr-read_buffer] = '\0';

	/* erase it from read_buffer - can be optimized */
	memmove(read_buffer, new_line_ptr+1, strlen(new_line_ptr+1)+1);
	
	return RC_NO_ERROR;
}

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: newCharNode                                                                                               *
 *                                                                                                                     *
 * DESCRIPTION: memory allocate and initialise new character node                                                      *
 *              (used for every char node for building the char tree)                                                  *
 *                                                                                                                     *
 * PARAMETERS: char_node (in/out) - pointer to character node pointer                                                  *
 *             ch (in) - charactre node char value                                                                     *
 *             is_word (in) - yes/no if this character node is a word within a character tree                          *
 *                                                                                                                     *
 * RETURN: pointer to the new char node (or NULL if could not be allocated)                                            *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
static CharNode* newCharNode (CharNode **char_node, char ch, unsigned char is_word)
{
   (*char_node) = (CharNode*)malloc(sizeof(CharNode));
   if ((*char_node) != NULL)
   {
	  (*char_node)->ch = ch;
	  (*char_node)->is_word = is_word;
	  (*char_node)->adjacent = NULL;
	  (*char_node)->next = NULL;
   }

   return (*char_node);
}

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: addNewWordToCharTree                                                                                      *
 *                                                                                                                     *
 * DESCRIPTION: Add new word to character tree                                                                         *
 *              (called in a loop for every word read from word list file to be added to the character tree)           *
 *                                                                                                                     *
 * PARAMETERS: char_tree - (in/out) pointer to character tree pointer                                                  *
 *             word  - (in) word to be added                                                                           *
 *             allocated_nodes - (out) number of new nodes allocated as a result                                       *
 *                                                                                                                     *
 * RETURN: RC_NO_ERROR - no error                                                                                      *
 *         RC_NO_MEM - error, no memory (nodes could not be allocated)                                                 *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
static RETURN_CODE addNewWordToCharTree (CharNode **char_tree, char *word, size_t *allocated_nodes)
{
   CharNode *nxt_search = (*char_tree);
   CharNode *nxt_search_prev = NULL;
   CharNode *adj_search;
   CharNode *adj_adj_search;
   CharNode *char_node;
   size_t i, word_len;
   unsigned char is_word, char_added_to_adjacent;

   word_len = strlen(word);
   for (i = 0; i < word_len; i++)
   {
	  is_word = (i == (word_len - 1))? 1: 0;
			   
	  /* no charaters exist in this depth */
	  if (nxt_search == NULL)
	  {
		 char_node = newCharNode(&char_node, word[i], is_word);
		 if (char_node == NULL)
		 {
			return RC_NO_MEM;
		 }
		(*allocated_nodes)++;

		nxt_search = char_node;
		 
		 /* initialise word list if the very first character */
         if ((*char_tree) == NULL)
		 {
            (*char_tree) = char_node;
		 }
		 else
		 {
			assert(nxt_search_prev);

			nxt_search_prev->next = nxt_search;
		 }

		 nxt_search_prev = nxt_search;
		 nxt_search = nxt_search->next;
	  }
	  
	  /* characters do exist in this depth */
	  else
	  {
		 char_added_to_adjacent = 0;
		 
		 /* search ADJACENT list to find the correct spot to add character to list (if character does not exist) - ADJACENT list is kept sorted by character value */
		 for (adj_search = nxt_search; !char_added_to_adjacent; adj_search = adj_search->adjacent)
		 {
			assert(adj_search);
			adj_adj_search = adj_search->adjacent;
			
			if (adj_search->ch == (word[i]))
			{
			   nxt_search_prev = adj_search;
			   nxt_search = nxt_search_prev->next;
			   char_added_to_adjacent = 1; /* character already exists - do nothing */
			}
			
			else if (adj_search->ch < (word[i]))
			{
			   if (adj_adj_search == NULL)
			   {
				  /* add character to the end of ADJACENT list */
				  char_node = newCharNode(&char_node, word[i], is_word);
				  if (char_node == NULL)
				  {
					 return RC_NO_MEM;
				  }
				  (*allocated_nodes)++;

				  adj_search->adjacent = char_node;

				  nxt_search_prev = char_node;
				  nxt_search = nxt_search_prev->next;
				  char_added_to_adjacent = 1;

			   }
			   else if (adj_adj_search->ch > (word[i]))
			   {
				  /* add character between adj_search and adj_adj_search */
				  char_node = newCharNode(&char_node, word[i], is_word);
				  if (char_node == NULL)
				  {
					 return RC_NO_MEM;
				  }
				  (*allocated_nodes)++;

				  adj_search->adjacent = char_node;
				  char_node->adjacent = adj_adj_search;

				  nxt_search_prev = char_node;
				  nxt_search = nxt_search_prev->next;
				  char_added_to_adjacent = 1;
			   }
			   /* else (adj_adj_search->ch <= (word[i])) - to be handled in the next loop iteration */
			}
			else /* (adj_search->ch > (word[i])) */
			{
			   /* add character at the start of ADJACENT list */
			   char_node = newCharNode(&char_node, word[i], is_word);
			   if (char_node == NULL)
			   {
				  return RC_NO_MEM;
			   }
			   (*allocated_nodes)++;

			   nxt_search_prev->next = char_node;
			   char_node->adjacent = nxt_search;

			   nxt_search_prev = char_node;
			   nxt_search = nxt_search_prev->next;
			   char_added_to_adjacent = 1;
			}
		 }
	  }

   }

   return RC_NO_ERROR;
}

/*******************************************************************************************************************************************************/
/************************************************************ external functions definition ************************************************************/
/*******************************************************************************************************************************************************/
RETURN_CODE WordList_BuildCharTree (WordList *word_list, FILE *file)
{
	char read_buffer[READ_BUF_SIZE] = {0};
	char word[MAX_WORD_LEN+1];
	RETURN_CODE ret_code;

	/* init word_list */ 
	word_list->char_tree = NULL;
	word_list->no_of_words = 0;
	word_list->allocated_nodes = 0;
	word_list->freed_nodes = 0;

	/* build word_list from file */
	while ((ret_code = readNextWordFromFile(file, read_buffer, word)) == RC_NO_ERROR)
	{
	  ret_code = addNewWordToCharTree(&(word_list->char_tree), word, &(word_list->allocated_nodes));
	  if (ret_code != RC_NO_ERROR)
	  {
		 break;
	  }
	  word_list->no_of_words++;
	}

	return ret_code;
}

int WordList_FindWord (CharNode *char_tree, char *word)
{
   size_t word_len;
   CharNode *search;
   
   word_len = strlen(word);

   assert(word_len);
   
   for (search = char_tree; search != NULL; search = search->adjacent)
   {
	  if (search->ch == word[0])
	  {
		 if (word_len == 1)
		 {
			 if (search->is_word)
			 {
				 return WORD_FOUND;
			 }
			 else 
			 {
				 return PREFIX_FOUND;
			 }
		 }
		 else
		 {
			return WordList_FindWord(search->next, word+1);
		 }
	  }
	  else if (search->ch > word[0])
	  {
		 return NOT_FOUND;
	  }
	  /* else - search->ch < word[0] - continue */
   }

   return NOT_FOUND;
}

size_t WordList_FreeCharTree (CharNode *char_tree)
{
	size_t freed_nodes = 0;
	
	if (char_tree->next != NULL)
	{
		freed_nodes += WordList_FreeCharTree(char_tree->next);
	}

	if (char_tree->adjacent != NULL)
	{
		freed_nodes += WordList_FreeCharTree(char_tree->adjacent);
	}

	char_tree->next = NULL;
	char_tree->adjacent = NULL;
	free(char_tree);

	return freed_nodes+1;
}
