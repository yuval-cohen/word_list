#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "gen_defs.h"
#include "word_list.h"

#include "words_grid.h"

/*******************************************************************************************************************************************************/
/************************************************************ internal functions declation *************************************************************/
/*******************************************************************************************************************************************************/
static void initGridCtrl (char grid_ctrl[][GRID_Y_LEN], int size);
static void gridCopy (char grid_dest[][GRID_Y_LEN], char grid_src[][GRID_Y_LEN], int size);
static void outputFoundWordsFromPrefix (WordsGrid *words_grid, char *word, int i, int j, char grid_ctrl[][GRID_Y_LEN], int size);
static int getNextAdjacentUnusedCell (char grid_ctrl[][GRID_Y_LEN], int size, int i, int j, int *x, int *y);

/*******************************************************************************************************************************************************/
/************************************************************ internal functions definition ************************************************************/
/*******************************************************************************************************************************************************/

/**********************************************************************************************************************
 * FUNCTION: initGridCtrl                                                                                             *
 *                                                                                                                    *
 * DESCRIPTION: initialise grid control to cell-unused                                                                *
 *                                                                                                                    *
 * PARAMETERS: (in/out) grid_ctrl                                                                                     *
 *                                                                                                                    *
 * RETURN: none                                                                                                       *
 *                                                                                                                    *
 * NOTES:                                                                                                             *
 *                                                                                                                    *
 **********************************************************************************************************************/
static void initGridCtrl (char grid_ctrl[][GRID_Y_LEN], int size)
{
	int i, j;
   
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < GRID_Y_LEN; j++)
		{
			grid_ctrl[i][j] = CELL_NOT_USED;
		}
	}
}

/**********************************************************************************************************************
 * FUNCTION: gridCopy                                                                                                 *
 *                                                                                                                    *
 * DESCRIPTION: copy one grid to another                                                                              *
 *                                                                                                                    *
 * PARAMETERS: (out) grid_dest - destination grid                                                                     *
 *             (in)  grid_src - source grid                                                                           *
 *                                                                                                                    *
 * RETURN: none                                                                                                       *
 *                                                                                                                    *
 * NOTES:                                                                                                             *
 *                                                                                                                    *
 *********************************************************************************************************************/
static void gridCopy (char grid_dest[][GRID_Y_LEN], char grid_src[][GRID_Y_LEN], int size)
{
	int i, j;
   
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < GRID_Y_LEN; j++)
		{
			grid_dest[i][j] = grid_src[i][j];
		}
	}
}

 /**********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: outputFoundWordsFromPrefix                                                                                *
 *                                                                                                                     *
 * DESCRIPTION: output (calls output_func) all found words from grid that start with a prefix                          *
 *                                                                                                                     *
 * PARAMETERS: words_grid - (in) pointer to words grid w/ word list and a grid e.g.:                                   *
 *                                                                                  +-------+                          *
 *                                                                                  |a|b|a|n|                          *
 *                                                                                  +-------+                          *
 *                                                                                  |s|d|f|d|                          *
 *                                                                                  +-------+                          *
 *                                                                                  |g|h|j|o|                          *
 *                                                                                  +-------+                          *
 *                                                                                  |k|l|n|z|                          *
 *                                                                                  +-------+                          *
 *                                                                                                                     *
 *             word - (in) prefix string, e.g.: "aband"                                                                *
 *                                                                                                                     *
 *             i,j - (in) the x,y indices of the last character of prefix, e.g.: (1,3)                                 *
 *                                                                                                                     *
 *             grid_ctrl - (in) grid control that marks which characters are used (part of prefix) and which aren't    *
 *                                                                                  e.g.: (x = used; o = unused)       *
 *                                                                                  +-------+                          *
 *                                                                                  |x|x|x|x|                          *
 *                                                                                  +-------+                          *
 *                                                                                  |o|o|o|x|                          *
 *                                                                                  +-------+                          *
 *                                                                                  |o|o|o|o|                          *
 *                                                                                  +-------+                          *
 *                                                                                  |o|o|o|o|                          *
 *                                                                                  +-------+                          *
 *                                                                                                                     *
 * RETURN: none                                                                                                        *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
static void outputFoundWordsFromPrefix (WordsGrid *words_grid, char *word, int i, int j, char grid_ctrl[][GRID_Y_LEN], int size)
{
	char grid_ctrl_next[GRID_X_LEN][GRID_Y_LEN];
	char word_next[(GRID_X_LEN*GRID_Y_LEN)+1];
	int word_found;
	size_t word_len;
	int x, y;
	
	word_found = WordList_FindWord(words_grid->word_list.char_tree, word);
	if ((word_found == WORD_FOUND) || (word_found == PREFIX_FOUND))
	{
		if (word_found == WORD_FOUND)
		{
			words_grid->output_func(word);
		}
		
		/* init x and y to i and j */
		x = i;
		y = j;

		/* in a loop: 1) find next adjacent cell 2) recursive call to outputFoundWordsFromPrefix */
		while (getNextAdjacentUnusedCell(grid_ctrl,GRID_X_LEN,i,j,&x,&y))
		{
			/* prepare the next work */
			word_len = strlen(word);
			strcpy(word_next, word);
			word_next[word_len] = words_grid->grid[x][y];
			word_next[word_len+1] = '\0';
			
			/* prepare the next grid ctrl */
			gridCopy(grid_ctrl_next, grid_ctrl, GRID_X_LEN);
			grid_ctrl_next[x][y] = CELL_USED;
			
			outputFoundWordsFromPrefix(words_grid, word_next, x, y, grid_ctrl_next, GRID_X_LEN);
		}
	}
	/* else: NOT_FOUND - no need to check further this prefix */
}

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: getNextAdjacentUnusedCell                                                                                 *
 *                                                                                                                     *
 * DESCRIPTION: get next adjecent unused cell                                                                          *
 *                                                                                                                     *
 * PARAMETERS: grid_ctrl - (in) grid control that marks which characters are used and which aren't e.g.:               *
 *                           (x = used; o = unused):                                                                   *
 *                           +-------+                                                                                 *
 *                           |x|x|x|x|                                                                                 *
 *                           +-------+                                                                                 *
 *                           |o|o|o|x|                                                                                 *
 *                           +-------+                                                                                 *
 *                           |o|o|o|o|                                                                                 *
 *                           +-------+                                                                                 *
 *                           |o|o|o|o|                                                                                 *
 *                           +-------+                                                                                 *
 *                                                                                                                     *
 *             i,j - (in) the x,y indices of the character to find the next adjacent unused cell from                  *
 *                                                                                                                     *
 *             x,y - (out) the indices of next unused adjacent character                                               *
 *                                                                                                                     *
 * RETURN: 0 - cell not found                                                                                          *
 *         1 - cell found                                                                                              *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
static int getNextAdjacentUnusedCell (char grid_ctrl[][GRID_Y_LEN], int size, int i, int j, int *x, int *y)
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
		
		if (((*x) >= 0) && ((*x) < size) && ((*y) >= 0) && ((*y) < GRID_Y_LEN) && (grid_ctrl[*x][*y] == CELL_NOT_USED))
		{
			/* cell exists and unused */
			return 1;
		}
	}
	
	return 0;
}

/*******************************************************************************************************************************************************/
/************************************************************ external functions definition ************************************************************/
/*******************************************************************************************************************************************************/
RETURN_CODE WordsGrid_InitWordList (WordsGrid *words_grid, FILE *file)
{
	return WordList_BuildCharTree(&(words_grid->word_list), file);
}

void WordsGrid_SetGrid (WordsGrid *words_grid, const char *string)
{
	int i, j, k = 0;
	
	assert(strlen(string) == (GRID_X_LEN*GRID_Y_LEN));
	
	for (i = 0; i < GRID_X_LEN; i++)
	{
		for (j = 0; j < GRID_Y_LEN; j++)
		{
			words_grid->grid[i][j] = string[k++];
		}
	}
}

void WordsGrid_SetOutputFunc (WordsGrid *words_grid, void (*output_func) (char *word))
{
	words_grid->output_func = output_func;
}

void WordsGrid_OutputFoundWords (WordsGrid *words_grid)
{
   char grid_ctrl[GRID_X_LEN][GRID_Y_LEN];
   char word[(GRID_X_LEN*GRID_Y_LEN)+1];
   int i, j;
   
   for (i = 0; i < GRID_X_LEN; i++)
   {
	   for (j = 0; j < GRID_Y_LEN; j++)
	   {
		   initGridCtrl(grid_ctrl, GRID_X_LEN);
		   grid_ctrl[i][j] = CELL_USED;
		   
		   word[0] = words_grid->grid[i][j];
		   word[1] = '\0';
		   
		   outputFoundWordsFromPrefix(words_grid, word, i, j, grid_ctrl, GRID_X_LEN);
	   }
   }
}

void WordsGrid_Free (WordsGrid *words_grid)
{
	words_grid->word_list.freed_nodes = WordList_FreeCharTree(words_grid->word_list.char_tree);
}
