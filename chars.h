#ifndef CHARS_H
#define CHARS_H

// Character class test functions.


/** recognized separator characters for "--separator auto" */
#define issep(c) (c == ',' || c == ';' || c == '\t' || c == '|')

/** recognized quoting characters */
#define isq(c) (c == '"' || c == '\'')


#endif  // CHARS_H
