#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "gen_defs.h"

#include "word_list.h"

/*

 word list as a tree of character nodes

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

/* word list related local functions */
static RETURN_CODE read_next_word_from_file (FILE *file, char read_buffer[READ_BUF_SIZE], char *word);
static RETURN_CODE add_new_word_to_char_tree (CharNode **char_tree, char *word, size_t *allocated_nodes);
static CharNode* new_char_node (CharNode **char_node, char ch, unsigned char is_word);

static RETURN_CODE read_next_word_from_file (FILE *file, char read_buffer[READ_BUF_SIZE], char *word)
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

static CharNode* new_char_node (CharNode **char_node, char ch, unsigned char is_word)
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

static RETURN_CODE add_new_word_to_char_tree (CharNode **char_tree, char *word, size_t *allocated_nodes)
{
   CharNode *nxt_search = (*char_tree);
   CharNode *nxt_search_prev;
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
		 char_node = new_char_node(&char_node, word[i], is_word);
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
				  char_node = new_char_node(&char_node, word[i], is_word);
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
				  char_node = new_char_node(&char_node, word[i], is_word);
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
			   char_node = new_char_node(&char_node, word[i], is_word);
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

RETURN_CODE build_wordlist (WordList *word_list, FILE *file)
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
	while ((ret_code = read_next_word_from_file(file, read_buffer, word)) == RC_NO_ERROR)
	{
	  ret_code = add_new_word_to_char_tree(&(word_list->char_tree), word, &(word_list->allocated_nodes));
	  if (ret_code != RC_NO_ERROR)
	  {
		 break;
	  }
	  word_list->no_of_words++;
	}

	return ret_code;
}

int find_word (CharNode *char_tree, char *word)
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
			return find_word(search->next, word+1);
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

size_t free_char_tree (CharNode *char_tree)
{
	size_t freed_nodes = 0;
	
	if (char_tree->next != NULL)
	{
		freed_nodes += free_char_tree(char_tree->next);
	}

	if (char_tree->adjacent != NULL)
	{
		freed_nodes += free_char_tree(char_tree->adjacent);
	}

	char_tree->next = NULL;
	char_tree->adjacent = NULL;
	free(char_tree);

	return freed_nodes+1;
}
