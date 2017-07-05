#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

//#define _MY_DEBUG_

#define READ_BUF_SIZE	256
#define READ_BLOCK_LEN	100

#define MAX_WORD_LEN	50

#define GRID_X_LEN		4
#define GRID_Y_LEN		4

#define NOT_FOUND		((int)0)
#define WORD_FOUND		((int)1)
#define PREFIX_FOUND	((int)2)

#define CELL_NOT_USED	((char)0)
#define CELL_USED		((char)1)


/* program's return codes */
typedef enum
{
   /* external */
   RC_NO_ERROR,
   RC_BAD_FORMAT,
   RC_NO_MEM,
   RC_FILE_NOT_FOUND,

   /* internal */
   RC_EOF,

} RETURN_CODE;

/* single letter node */
typedef struct _LetterNode 
{
   char letter;
   unsigned char is_word;

   struct _LetterNode *adjacent;
   struct _LetterNode *next;

} LetterNode;

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
static LetterNode *WordList = NULL;

static RETURN_CODE read_next_word (FILE *file, char *word_text);
static LetterNode* new_letter (LetterNode **letter_node, char letter, unsigned char is_word);
static RETURN_CODE add_new_word_to_wordlist (char *word_text);
static RETURN_CODE build_wordlist (FILE *file);
static int find_word (LetterNode *wordlist, unsigned char *word);
static void free_wordlist (LetterNode *wordlist);
static void init_grid_ctrl (char grid_ctrl[GRID_X_LEN][GRID_Y_LEN]);
static void copy_grid_ctrl (char grid_ctrl1[GRID_X_LEN][GRID_Y_LEN], const char grid_ctrl2[GRID_X_LEN][GRID_Y_LEN]);
static void print_all_found_words_in_grid (char grid[GRID_X_LEN][GRID_Y_LEN]);
static void print_all_found_words_from_prefix(char grid[GRID_X_LEN][GRID_Y_LEN], char *word, int i, int j, char grid_ctrl[GRID_X_LEN][GRID_Y_LEN]);
static int get_next_adjacent_unused_cell (const char grid_ctrl[GRID_X_LEN][GRID_Y_LEN], int i, int j, int *x, int *y);
static void string_to_grid (const char *string, char grid[GRID_X_LEN][GRID_Y_LEN]);

static RETURN_CODE read_next_word (FILE *file, char *word_text)
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
	strncpy(word_text, ReadBuffer, new_line_ptr-ReadBuffer);
	word_text[new_line_ptr-ReadBuffer] = '\0';

	/* erase it from ReadBuffer - can be optimized */
	memmove(ReadBuffer, new_line_ptr+1, strlen(new_line_ptr+1)+1);
	
	return RC_NO_ERROR;
}

static LetterNode* new_letter (LetterNode **letter_node, char letter, unsigned char is_word)
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

static RETURN_CODE add_new_word_to_wordlist (char *word_text)
{
   LetterNode *nxt_search = WordList;
   LetterNode *nxt_search_prev;
   LetterNode *adj_search;
   LetterNode *adj_adj_search;
   LetterNode *letter_node;
   size_t i, word_len;
   unsigned char is_word, letter_added_to_adjacent;

   word_len = strlen(word_text);
   for (i = 0; i < word_len; i++)
   {
      is_word = (i == (word_len - 1))? 1: 0;
               
      /* no letters exist in this depth */
      if (nxt_search == NULL)
      {
         letter_node = new_letter(&letter_node, word_text[i], is_word);
         if (letter_node == NULL)
         {
            return RC_NO_MEM;
         }

         nxt_search = letter_node;
         
         /* initialise WordList if the very first letter */
         if (WordList == NULL)
         {
            WordList = letter_node;
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
            
            if (adj_search->letter == (word_text[i]))
            {
               nxt_search_prev = adj_search;
               nxt_search = nxt_search_prev->next;
               letter_added_to_adjacent = 1; /* letter already exists - do nothing */
            }
            
            else if (adj_search->letter < (word_text[i]))
            {
               if (adj_adj_search == NULL)
               {
                  /* add letter to the end of ADJACENT list */
                  letter_node = new_letter(&letter_node, word_text[i], is_word);
                  if (letter_node == NULL)
                  {
                     return RC_NO_MEM;
                  }

                  adj_search->adjacent = letter_node;

                  nxt_search_prev = letter_node;
                  nxt_search = nxt_search_prev->next;
                  letter_added_to_adjacent = 1;

               }
               else if (adj_adj_search->letter > (word_text[i]))
               {
                  /* add letter between adj_search and adj_adj_search */
                  letter_node = new_letter(&letter_node, word_text[i], is_word);
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
               /* else (adj_adj_search->letter <= (word_text[i])) - to be handled in the next loop iteration */
            }
            else /* (adj_search->letter > (word_text[i])) */
            {
               /* add letter at the start of ADJACENT list */
               letter_node = new_letter(&letter_node, word_text[i], is_word);
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

static RETURN_CODE build_wordlist (FILE *file)
{
   char word_text[MAX_WORD_LEN+1];
   RETURN_CODE ret_code;

   /* build wordlist from file */
   while ((ret_code = read_next_word(file, word_text)) == RC_NO_ERROR)
   {
      ret_code = add_new_word_to_wordlist(word_text);

      if (ret_code != RC_NO_ERROR)
      {
         break;
      }
   }

   return ret_code;
}

static int find_word (LetterNode *wordlist, unsigned char *word)
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

static void init_grid_ctrl (char grid_ctrl[GRID_X_LEN][GRID_Y_LEN])
{
	int i, j;
   
	for (i = 0; i < GRID_X_LEN; i++)
	{
		for (j = 0; j < GRID_Y_LEN; j++)
		{
			grid_ctrl[i][j] = CELL_NOT_USED;
		}
	}
}

static void copy_grid_ctrl (char grid_ctrl1[GRID_X_LEN][GRID_Y_LEN], const char grid_ctrl2[GRID_X_LEN][GRID_Y_LEN])
{
	int i, j;
   
	for (i = 0; i < GRID_X_LEN; i++)
	{
		for (j = 0; j < GRID_Y_LEN; j++)
		{
			grid_ctrl1[i][j] = grid_ctrl2[i][j];
		}
	}
}

static void print_all_found_words_in_grid (char grid[GRID_X_LEN][GRID_Y_LEN])
{
   char grid_ctrl[GRID_X_LEN][GRID_Y_LEN];
   char word[(GRID_X_LEN*GRID_Y_LEN)+1];
   int i, j;
   
   for (i = 0; i < GRID_X_LEN; i++)
   {
	   for (j = 0; j < GRID_Y_LEN; j++)
	   {
		   init_grid_ctrl(grid_ctrl);
		   grid_ctrl[i][j] = CELL_USED;
		   
		   word[0] = grid[i][j];
		   word[1] = '\0';
		   
		   print_all_found_words_from_prefix(grid, word, i, j, grid_ctrl);
	   }
   }
}

static void print_all_found_words_from_prefix(char grid[GRID_X_LEN][GRID_Y_LEN], char *word, int i, int j, char grid_ctrl[GRID_X_LEN][GRID_Y_LEN])
{
	char grid_ctrl_next[GRID_X_LEN][GRID_Y_LEN];
	char word_next[(GRID_X_LEN*GRID_Y_LEN)+1];
	int word_found;
	size_t word_len;
	int x, y;
	
	word_found = find_word(WordList, word);
	if ((word_found == WORD_FOUND) || (word_found == PREFIX_FOUND))
	{
		if (word_found == WORD_FOUND)
		{
			printf("%s\n", word);
		}
		
		/* init x and y to i and j */
		x = i;
		y = j;

		/* in a loop: 1) find next adjacent cell 2) recursive call to print_all_found_words_from_prefix */
		while (get_next_adjacent_unused_cell(grid_ctrl,i,j,&x,&y))
		{
			/* prepare the next work */
			word_len = strlen(word);
			strcpy(word_next, word);
			word_next[word_len] = grid[x][y];
			word_next[word_len+1] = '\0';
			
			/* prepare the next grid ctrl */
			copy_grid_ctrl(grid_ctrl_next, grid_ctrl);
			grid_ctrl_next[x][y] = CELL_USED;
			
			print_all_found_words_from_prefix(grid, word_next, x, y, grid_ctrl_next);
		}
	}
	/* else: NOT_FOUND - no need to check further this prefix */
}

static int get_next_adjacent_unused_cell (const char grid_ctrl[GRID_X_LEN][GRID_Y_LEN], int i, int j, int *x, int *y)
{
	while (((*x) != (i-1)) || ((*y) != (j-1)))
	{
		/*
		check adjacent cell in the following order from X = the current cell.
		(i,j) - current cell
		(x,y) - current/next adjacent cell - initialised to (i,j)
		               -------------
					   | 8 | 1 | 2 |
		               -------------
					   | 7 | X | 3 |
		               -------------
					   | 6 | 5 | 4 |
		               -------------
		*/
		if (((*x) == i) && ((*y) == j))
		{
			/* current -> 1 */
			(*x) = (i-1);
		}
		else if (((*x) == (i-1)) && ((*y) == j))
		{
			/* 1 -> 2 */
			(*y) = (j+1);
		}
		else if (((*x) == (i-1)) && ((*y) == (j+1)))
		{
			/* 2 -> 3 */
			(*x) = i;
		}
		else if (((*x) == i) && ((*y) == (j+1)))
		{
			/* 3 -> 4 */
			(*x) = (i+1);
		}
		else if (((*x) == (i+1)) && ((*y) == (j+1)))
		{
			/* 4 -> 5 */
			(*y) = j;
		}
		else if (((*x) == (i+1)) && ((*y) == j))
		{
			/* 5 -> 6 */
			(*y) = (j-1);
		}
		else if (((*x) == (i+1)) && ((*y) == (j-1)))
		{
			/* 6 -> 7 */
			(*x) = i;
		}
		else /* (((*x) = i) && ((*y) = (j-1))) */
		{
			assert(((*x) == i) && ((*y) == (j-1)));
			
			/* 7 -> 8 */
			(*x) = (i-1);
		}
		
		if (((*x) >= 0) && ((*x) < GRID_X_LEN) && ((*y) >= 0) && ((*y) < GRID_Y_LEN) && (grid_ctrl[*x][*y] == CELL_NOT_USED))
		{
			/* cell exists and unused */
			return 1;
		}
	}
	
	return 0;
}

static void free_wordlist (LetterNode *wordlist)
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

static void string_to_grid (const char *string, char grid[GRID_X_LEN][GRID_Y_LEN])
{
	int i, j, h = 0;
	
	assert(strlen(string) == (GRID_X_LEN*GRID_Y_LEN));
	
	for (i = 0; i < GRID_X_LEN; i++)
	{
		for (j = 0; j < GRID_Y_LEN; j++)
		{
			grid[i][j] = string[h++];
		}
	}
}

int main (int argc, char* argv[])
{
	RETURN_CODE ret_code;
	FILE *input_file;
	char grid[GRID_X_LEN][GRID_Y_LEN];

#ifndef _MY_DEBUG_
	if ((argc != 3) || (strlen(argv[2]) != (GRID_X_LEN*GRID_Y_LEN)))
	{
		printf("Usage: word_list.exe <word-list-file> <gird-as-16-chars-string>\n");
	}
	else
#endif
	{
#ifndef _MY_DEBUG_
		input_file = fopen(argv[1], "r");
#else
		input_file = fopen("word_x.list", "r");
#endif
		if (input_file == NULL)
		{
			/* no file */
			ret_code = RC_FILE_NOT_FOUND;
		}
		else
		{
			ret_code = build_wordlist(input_file);

			if (ret_code == RC_EOF)
			{
#ifndef _MY_DEBUG_
				string_to_grid(argv[2], grid);
#else
				string_to_grid("mikayuvaaahlmich", grid); // aahebcidbengmika, abombanilenesess, mikayuvaaahlmich
#endif
			 
				print_all_found_words_in_grid(grid);
			 
				ret_code = RC_NO_ERROR;
			}
		 
			free_wordlist(WordList);

			fclose(input_file);
		}

		if (ret_code != RC_NO_ERROR)
		{
			printf("ERROR: error code #%d\n", ret_code);
		}
	}

	return 0;
}
