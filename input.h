#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>
#include <stdio.h>


/* Use this for set_input(separator) to auto-detect the separator on the first input line.  */
#define SEP_AUTO '\0'
/* Use this for set_input(separator) to use no separator, i.e. read the file as single-column.
 * This works because linebreaks are always read as record separator (fgets), never as cell separator.  */
#define SEP_NONE '\n'


void set_input (FILE* file, char separator, bool allow_breaks, bool remove_bom, bool skip_after_header, size_t skip_lines, size_t limit_lines);

size_t lineno (void);

bool next_line (void);
const char* next_field (void);


#endif  // INPUT_H
