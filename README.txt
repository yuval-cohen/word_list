Word Search:

A program in C that finds all words from a word list file (such as .\word.list) that can be found in a 4x4 grid of letters.

The grid should be given as input to the program. Specify the grid as one 16-character
argumen.

Details:
A word can be found in a grid by starting on any letter, then moving to an adjacent letter and so
on.

Example 1:

a b a n
z q z d
r r o r
r n r r

The word "abandon" fits in the grid first horizontally, then diagonally down and to the left. The
word "ran" does NOT fit in the grid because 'r' and 'a' are not adjacent.

A word cannot use a specific letter in the grid more than once.

Example 2:

The grid
z z z z
z z b z
h z e z
a z z z

contains the words "be", "ha", and "ah". It should not contain "bee" or "ebb" or "he" or "hah".

Your program prints any word found to the standard output.

Your program assumes that the word list file contains no duplicated words and that the input arguments are valid.

Your program is not tolerant to word files that contain a mixture of upper and lower case
words and of words that contain punctuation.

The program expects the grid on the command line as the last argument and print the results to the standard output. 
An example program run might be:
word_search.exe word.list abanzqzdrrorrnrr
...
abandon
...

The output is a list of matching words separated by newlines.
The program doesn't print out anything else unless your program is reporting an error.
