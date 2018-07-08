#include <stdio.h>
#include <string.h>

#include "gen_defs.h"
#include "words_grid.h"

#include "main.h"

//#define _MY_DEBUG_

static words_grid_output_func (char *word);

static words_grid_output_func (char *word)
{
	printf("%s\n", word);
}

int main (int argc, char* argv[])
{
	WordsGrid words_grid;
	RETURN_CODE ret_code;
	FILE *input_file;

#ifndef _MY_DEBUG_
	if ((argc != 3) || (strlen(argv[2]) != (GRID_X_LEN*GRID_Y_LEN)))
	{
		printf("Usage: word_search.exe <word-list-file> <gird-as-%d-chars-string>\n", GRID_X_LEN*GRID_Y_LEN);
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
			ret_code = WordsGrid_InitWordList(&words_grid, input_file);

			if (ret_code == RC_EOF)
			{
#ifndef _MY_DEBUG_
				WordsGrid_SetGrid(&words_grid, argv[2]);
#else
				WordsGrid_SetGrid(&words_grid, "mikayuvaaahlmich"); // more examples: aahebcidbengmika, abombanilenesess, mikayuvaaahlmich
#endif

				WordsGrid_SetOutputFunc(&words_grid, words_grid_output_func);
				WordsGrid_OutputFoundWords(&words_grid);
			 
				ret_code = RC_NO_ERROR;
			}
		 
			WordsGrid_Free(&words_grid);

			fclose(input_file);
		}

		if (ret_code != RC_NO_ERROR)
		{
			printf("ERROR: error code #%d\n", ret_code);
		}
	}

	return 0;
}
