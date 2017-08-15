#ifndef _WORDS_GRID_H_
#define _WORDS_GRID_H_

#define GRID_X_LEN		4
#define GRID_Y_LEN		4

#define CELL_NOT_USED	((char)0)
#define CELL_USED		((char)1)

/* words grid struct */
typedef struct _WordsGrid
{
	/* word list to search words from */
	WordList word_list;
	
	/* letters grid */
	char grid[GRID_X_LEN][GRID_Y_LEN];
	
	/* output function to be used for all found words in grid */
	void (*output_func) (char *word);
	
} WordsGrid;

void init_grid_from_string (const char *string, char grid[GRID_X_LEN][GRID_Y_LEN]);
void set_output_func (WordsGrid *words_grid, void (*output_func) (char *word));
void output_found_words_in_grid (WordsGrid *words_grid);

#endif // _WORDS_GRID_H_
