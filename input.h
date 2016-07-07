#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>
#include <stdio.h>


void set_input (FILE* file, char separator, bool allow_breaks, bool remove_bom, bool skip_after_header, size_t skip_lines, size_t limit_lines);

size_t lineno (void);

bool next_line (void);
const char* next_field (void);


#endif  // INPUT_H