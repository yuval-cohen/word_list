#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "gen_defs.h"
#include "word_list.h"
#include "words_grid.h"


#include "main.h"

//#define _MY_DEBUG_

int main (int argc, char* argv[])
{
	WordsGrid words_grid;
	RETURN_CODE ret_code;
	FILE *input_file;

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
			ret_code = build_wordlist(&(words_grid.word_list), input_file);

			if (ret_code == RC_EOF)
			{
#ifndef _MY_DEBUG_
				init_grid_from_string(argv[2], words_grid.grid);
#else
				init_grid_from_string("mikayuvaaahlmich", words_grid.grid); // aahebcidbengmika, abombanilenesess, mikayuvaaahlmich
#endif
			 
				output_found_words_in_grid(&(words_grid.word_list), words_grid.grid);
			 
				ret_code = RC_NO_ERROR;
			}
		 
			words_grid.word_list.freed_nodes = free_letter_tree(words_grid.word_list.letter_tree);

			fclose(input_file);
		}

		if (ret_code != RC_NO_ERROR)
		{
			printf("ERROR: error code #%d\n", ret_code);
		}
	}

	return 0;
}
