Goal:

Write a program in C to find all words from a word list file (such as /usr/share/dict/words or
/usr/dict/words) that can be found in a 4x4 grid of letters.

The grid should be given as input to the program. Specify the grid as one 16-character
argument or 16 1-character arguments to the program or in some other reasonable way.

Problem Details:
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

Implementation Details:

Your program should not print any word more than once. Your program can assume that the
word list file contains no duplicated words and that the input arguments are valid.

Your program should be tolerant of word files that contain a mixture of upper and lower case
words and of words that contain punctuation. Your program can deal with such words in some
reasonable way. Note that different word list files can be alphabetized differently.

Please specify the grid on the command line as the last argument or arguments
and print the results to the standard output. An example program run might be:
a.out /usr/share/dict/words abanzqzdrrorrnrr
...
abandon
...
Your output should be a list of matching words separated by newlines. Don't print out anything
else unless your program is reporting an error condition.
