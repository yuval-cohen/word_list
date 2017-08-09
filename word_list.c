#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "gen_defs.h"

#include "word_list.h"

/* buffer for read from input file */
static char ReadBuffer[READ_BUF_SIZE] = {0};

/* statistics */
static size_t MemoryAllocated_Bytes = 0;
static size_t MemoryAllocated_Blocks = 0;
static size_t MemoryFreed_Blocks = 0;

/*

 word list as a tree of letter nodes

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
static RETURN_CODE read_next_word_from_file (FILE *file, char *word);
static RETURN_CODE add_new_word_to_wordlist (LetterNode **wordList, char *word);
static LetterNode* new_letter_node (LetterNode **letter_node, char letter, unsigned char is_word);

static RETURN_CODE read_next_word_from_file (FILE *file, char *word)
{
	char *new_line_ptr;
	size_t read_bytes, cur_length, word_len;

	new_line_ptr = strstr(ReadBuffer, "\n");
	if (new_line_ptr == NULL)
	{
		/* read another block from file into ReadBuffer */
		cur_length = strlen(ReadBuffer);
		if ((cur_length +1 + READ_BLOCK_LEN) > READ_BUF_SIZE)
		{
			return RC_BAD_FORMAT;
		}
		read_bytes = fread(ReadBuffer+strlen(ReadBuffer), 1, READ_BLOCK_LEN, file);
		new_line_ptr = strstr(ReadBuffer, "\n");
		if ((read_bytes == 0) || (new_line_ptr == NULL))
		{
			return RC_EOF;
		}
		*(ReadBuffer+cur_length+read_bytes) = '\0';
   }

	/* check word length does not exceed maximum */
	word_len = new_line_ptr - ReadBuffer;
	if (word_len > MAX_WORD_LEN)
	{
		return RC_BAD_FORMAT;
	}
   
	/* copy next line into from ReadBuffer into line */
	strncpy(word, ReadBuffer, new_line_ptr-ReadBuffer);
	word[new_line_ptr-ReadBuffer] = '\0';

	/* erase it from ReadBuffer - can be optimized */
	memmove(ReadBuffer, new_line_ptr+1, strlen(new_line_ptr+1)+1);
	
	return RC_NO_ERROR;
}

static LetterNode* new_letter_node (LetterNode **letter_node, char letter, unsigned char is_word)
{
   (*letter_node) = (LetterNode*)malloc(sizeof(LetterNode));
   if ((*letter_node) != NULL)
   {
      (*letter_node)->letter = letter;
      (*letter_node)->is_word = is_word;
      (*letter_node)->adjacent = NULL;
      (*letter_node)->next = NULL;

      MemoryAllocated_Bytes += sizeof(LetterNode);
      MemoryAllocated_Blocks++;
   }

   return (*letter_node);
}

static RETURN_CODE add_new_word_to_wordlist (LetterNode **wordList, char *word)
{
   LetterNode *nxt_search = (*wordList);
   LetterNode *nxt_search_prev;
   LetterNode *adj_search;
   LetterNode *adj_adj_search;
   LetterNode *letter_node;
   size_t i, word_len;
   unsigned char is_word, letter_added_to_adjacent;

   word_len = strlen(word);
   for (i = 0; i < word_len; i++)
   {
      is_word = (i == (word_len - 1))? 1: 0;
               
      /* no letters exist in this depth */
      if (nxt_search == NULL)
      {
         letter_node = new_letter_node(&letter_node, word[i], is_word);
         if (letter_node == NULL)
         {
            return RC_NO_MEM;
         }

         nxt_search = letter_node;
         
         /* initialise wordList if the very first letter */
         if ((*wordList) == NULL)
         {
            (*wordList) = letter_node;
         }
         else
         {
            assert(nxt_search_prev);

            nxt_search_prev->next = nxt_search;
         }

         nxt_search_prev = nxt_search;
         nxt_search = nxt_search->next;
      }
      
      /* letters do exist in this depth */
      else
      {
         letter_added_to_adjacent = 0;
         
         /* search ADJACENT list to find the correct spot to add letter to list (if letter does not exist) - ADJACENT list is kept sorted by letter value */
         for (adj_search = nxt_search; !letter_added_to_adjacent; adj_search = adj_search->adjacent)
         {
            assert(adj_search);
            adj_adj_search = adj_search->adjacent;
            
            if (adj_search->letter == (word[i]))
            {
               nxt_search_prev = adj_search;
               nxt_search = nxt_search_prev->next;
               letter_added_to_adjacent = 1; /* letter already exists - do nothing */
            }
            
            else if (adj_search->letter < (word[i]))
            {
               if (adj_adj_search == NULL)
               {
                  /* add letter to the end of ADJACENT list */
                  letter_node = new_letter_node(&letter_node, word[i], is_word);
                  if (letter_node == NULL)
                  {
                     return RC_NO_MEM;
                  }

                  adj_search->adjacent = letter_node;

                  nxt_search_prev = letter_node;
                  nxt_search = nxt_search_prev->next;
                  letter_added_to_adjacent = 1;

               }
               else if (adj_adj_search->letter > (word[i]))
               {
                  /* add letter between adj_search and adj_adj_search */
                  letter_node = new_letter_node(&letter_node, word[i], is_word);
                  if (letter_node == NULL)
                  {
                     return RC_NO_MEM;
                  }

                  adj_search->adjacent = letter_node;
                  letter_node->adjacent = adj_adj_search;

                  nxt_search_prev = letter_node;
                  nxt_search = nxt_search_prev->next;
                  letter_added_to_adjacent = 1;
               }
               /* else (adj_adj_search->letter <= (word[i])) - to be handled in the next loop iteration */
            }
            else /* (adj_search->letter > (word[i])) */
            {
               /* add letter at the start of ADJACENT list */
               letter_node = new_letter_node(&letter_node, word[i], is_word);
               if (letter_node == NULL)
               {
                  return RC_NO_MEM;
               }

               nxt_search_prev->next = letter_node;
               letter_node->adjacent = nxt_search;

               nxt_search_prev = letter_node;
               nxt_search = nxt_search_prev->next;
               letter_added_to_adjacent = 1;
            }
         }
      }

   }

   return RC_NO_ERROR;
}

RETURN_CODE build_wordlist (LetterNode **wordlist, FILE *file)
{
   char word[MAX_WORD_LEN+1];
   RETURN_CODE ret_code;

   /* build wordlist from file */
   while ((ret_code = read_next_word_from_file(file, word)) == RC_NO_ERROR)
   {
      ret_code = add_new_word_to_wordlist(wordlist, word);

      if (ret_code != RC_NO_ERROR)
      {
         break;
      }
   }

   return ret_code;
}

int find_word (LetterNode *wordlist, char *word)
{
   size_t word_len;
   LetterNode *search;
   
   word_len = strlen(word);

   assert(word_len);
   
   for (search = wordlist; search != NULL; search = search->adjacent)
   {
      if (search->letter == word[0])
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
      else if (search->letter > word[0])
      {
         return NOT_FOUND;
      }
      /* else - search->letter < word[0] - continue */
   }

   return NOT_FOUND;
}

void free_wordlist (LetterNode *wordlist)
{
   if (wordlist->next != NULL)
   {
      free_wordlist(wordlist->next);
   }
   
   if (wordlist->adjacent != NULL)
   {
      free_wordlist(wordlist->adjacent);
   }
   
   wordlist->next = NULL;
   wordlist->adjacent = NULL;
   free(wordlist);

   MemoryFreed_Blocks++;
}
