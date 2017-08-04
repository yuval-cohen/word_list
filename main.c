#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "gen_defs.h"
#include "word_list.h"

#include "main.h"

//#define _MY_DEBUG_

extern LetterNode *WordList;

/* grid related local functions */
static void init_grid_ctrl (char grid_ctrl[GRID_X_LEN][GRID_Y_LEN]);
static void copy_grid_ctrl (char grid_ctrl1[GRID_X_LEN][GRID_Y_LEN], const char grid_ctrl2[GRID_X_LEN][GRID_Y_LEN]);
static void print_all_found_words_in_grid (char grid[GRID_X_LEN][GRID_Y_LEN]);
static void print_all_found_words_from_prefix (char grid[GRID_X_LEN][GRID_Y_LEN], char *word, int i, int j, char grid_ctrl[GRID_X_LEN][GRID_Y_LEN]);
static int get_next_adjacent_unused_cell (const char grid_ctrl[GRID_X_LEN][GRID_Y_LEN], int i, int j, int *x, int *y);
static void string_to_grid (const char *string, char grid[GRID_X_LEN][GRID_Y_LEN]);

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

/*
* description: print all found words that start with the prefix word and that exists on the grid
*
* parameters:
*
* grid: - letters grid, e.g.         
*                           +-------+ 
*                           |a|b|a|n|
*                           +-------+
*                           |s|d|f|d|
*                           +-------+
*                           |g|h|j|o|
*                           +-------+
*                           |k|l|n|z|
*                           +-------+
*
* word: prefix string, e.g.: "aband"
*
* i,j: the x,y indices of word last letter, e.g.: (1,3)
*
* grid_ctrl: grid control that marks which letters are used (as part of word) and which aren't e.g. (x=used; -=unused):
*                           +-------+ 
*                           |x|x|x|x|
*                           +-------+
*                           |-|-|-|x|
*                           +-------+
*                           |-|-|-|-|
*                           +-------+
*                           |-|-|-|-|
*                           +-------+
*/
static void print_all_found_words_from_prefix (char grid[GRID_X_LEN][GRID_Y_LEN], char *word, int i, int j, char grid_ctrl[GRID_X_LEN][GRID_Y_LEN])
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

/*
* description: get next adjecent unused cell
*
* parameters:
*
* grid_ctrl: (input) grid control that marks which letters are used and which aren't e.g. (x=used; -=unused):
*                           +-------+ 
*                           |x|x|x|x|
*                           +-------+
*                           |-|-|-|x|
*                           +-------+
*                           |-|-|-|-|
*                           +-------+
*                           |-|-|-|-|
*                           +-------+
*
* i,j: (input) the x,y indices of the letter (to find the next adjacent cell that is unused)
*
* x,y: (output) the indices of next unused adjacent letter
*
*/
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

/*
* description: convert string (length GRID_X_LEN x GRID_Y_LEN) to character 2D matrix  grid
*
* parameters:
*
* string: (input) string (length GRID_X_LEN x GRID_Y_LEN) e.g. (3 x 4): "abcdefghijkl"
*
* grid: (output) charater 2D (GRID_X_LEN x GRID_Y_LEN) grid, e.g:
*                                                                         +-------+ 
*                                                                         |a|b|c|d|
*                                                                         +-------+
*                                                                         |e|f|g|h|
*                                                                         +-------+
*                                                                         |i|j|k|l|
*                                                                         +-------+
*
*/
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
		printf("Usage: word_list.exe <word-list-file> <gird-as-%d-chars-string>\n", GRID_X_LEN*GRID_Y_LEN);
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
