#ifndef _WORDS_GRID_H_
#define _WORDS_GRID_H_

#define GRID_X_LEN		4
#define GRID_Y_LEN		4

#define CELL_NOT_USED	((char)0)
#define CELL_USED		((char)1)

void string_to_grid (const char *string, char grid[GRID_X_LEN][GRID_Y_LEN]);
void print_all_found_words_in_grid (WordList *word_list, char grid[GRID_X_LEN][GRID_Y_LEN]);

#endif // _WORDS_GRID_H_
