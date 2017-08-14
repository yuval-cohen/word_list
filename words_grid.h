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
	
	/* control (used/unused) grid */
	char grid_ctrl[GRID_X_LEN][GRID_Y_LEN];
	
	/* current cell (last letter of current prefix/word) x and y indices  */
	int cur_cell_x;
	int cur_cell_y;
	
	/* next adjacent unused cell (next letter from the current prefix/word) x and y indices  */
	int next_adj_x;
	int next_adj_y;

} WordsGrid;

void init_grid_from_string (const char *string, char grid[GRID_X_LEN][GRID_Y_LEN]);
void output_found_words_in_grid (WordList *word_list, char grid[GRID_X_LEN][GRID_Y_LEN]);

#endif // _WORDS_GRID_H_
