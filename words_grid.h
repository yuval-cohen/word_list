#ifndef _WORDS_GRID_H_
#define _WORDS_GRID_H_

#define GRID_X_LEN		4
#define GRID_Y_LEN		4

#define CELL_NOT_USED	((char)0)
#define CELL_USED		((char)1)

#include "word_list.h"

/* words grid struct */
typedef struct _WordsGrid
{
	/* word list to search words from */
	WordList word_list;
	
	/* characters grid */
	char grid[GRID_X_LEN][GRID_Y_LEN];
	
	/* output function to be used for all found words in grid */
	void (*output_func) (char *word);
	
} WordsGrid;

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordsGrid_InitWordList                                                                                    *
 *                                                                                                                     *
 * DESCRIPTION: initialise word list in words grid from a file                                                         *
 *                                                                                                                     *
 * PARAMETERS: WordsGrid* - (in/out) pointer to words grid to initialise its word list                                 *
 *             FILE* - (in) pointer to (already open) input file to read word list from                                *
 *                          convernsion: <word><LF><word><LF><word><LF>...                                             *
 *                                                                                                                     *
 * RETURN:    RC_BAD_FORMAT - file format error                                                                        *
 *            RC_NO_MEM - no memory                                                                                    *
 *            RC_EOF - no error (end of file)                                                                          *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
RETURN_CODE WordsGrid_InitWordList (WordsGrid*, FILE*);

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordsGrid_SetGrid                                                                                         *
 *                                                                                                                     *
 * DESCRIPTION: set grid from grid string length [GRID_X_LEN x GRID_Y_LEN] (convert to character 2D matrix)            *
 *                                                                                                                     *
 * PARAMETERS: WordsGrid* - (in/out) pointer to words grid to set its 2D character grid                                *
 *             const char* - (in) string (length GRID_X_LEN x GRID_Y_LEN) e.g. (3 x 4): "abcdefghijkl"                 *
 *                                                                                                                     *
 * RETURN: none                                                                                                        *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
void WordsGrid_SetGrid (WordsGrid*, const char*);

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordsGrid_SetOutputFunc                                                                                   *
 *                                                                                                                     *
 * DESCRIPTION: set output function in words grid                                                                      *
 *                                                                                                                     *
 * PARAMETERS: WordsGrid* - (in/out) pointer to words grid to set its output_func                                      *
 *             void (*output_func)(char*) - (in) pointer to output function                                            *
 *                                                                                                                     *
 * RETURN: none                                                                                                        *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
void WordsGrid_SetOutputFunc (WordsGrid*, void (*output_func)(char*));

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordsGrid_OutputFoundWords                                                                                *
 *                                                                                                                     *
 * DESCRIPTION: output (using output function) found words from word list in grid                                      *
 *                                                                                                                     *
 * PARAMETERS: WordsGrid* - (in/out) pointer to words grid                                                             *
 *                                                                                                                     *
 * RETURN: none                                                                                                        *
 *                                                                                                                     *
 * NOTES: must be called only after WordsGrid_SetGrid, WordsGrid_SetGrid and WordsGrid_SetOutputFunc                   *
 *                                                                                                                     *
 ***********************************************************************************************************************/
void WordsGrid_OutputFoundWords (WordsGrid*);

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordsGrid_Free                                                                                            *
 *                                                                                                                     *
 * DESCRIPTION: free memory dynamically allocated for words grid                                                       *
 *                                                                                                                     *
 * PARAMETERS: WordsGrid* - (in/out) pointer to words grid                                                             *
 *                                                                                                                     *
 * RETURN: none                                                                                                        *
 *                                                                                                                     *
 * NOTES: must be called to free words grid memory at the end                                                          *
 *                                                                                                                     *
 ***********************************************************************************************************************/
void WordsGrid_Free (WordsGrid*);

#endif // _WORDS_GRID_H_
