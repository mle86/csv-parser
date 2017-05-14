#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>
#include <stdio.h>
#include "nstr.h"


/* Use this for set_input(separator) to auto-detect the separator on the first input line.  */
#define SEP_AUTO '\0'
/* Use this for set_input(separator) to use no separator, i.e. read the file as single-column.
 * This works because linebreaks are always read as record separator (fgets/getline), never as field separator.  */
#define SEP_NONE '\n'

/* Use this for set_input(enclosure) to auto-detect the enclosure.  */
#define ENC_AUTO '\0'
/* Use this for set_input(enclosure) if the input may contain mixed enclosure characters.  */
#define ENC_MIXED '\xff'


typedef enum trimmode {
	TRIM_NONE	= 0,

	TRIM_LINES_L	= 1,
	TRIM_LINES_R	= 2,
	TRIM_LINES	= TRIM_LINES_L | TRIM_LINES_R,
} trimmode_t;


void set_input (FILE* file, char separator, char enclosure, bool allow_breaks, bool remove_bom, bool skip_after_header, size_t skip_lines, size_t limit_lines, trimmode_t trim);

size_t lineno (void);

bool next_line (void);

/**
 * Returns a pointer to an nstr containing the next field,
 * or NULL if there was not next field on the current line (EOL or EOF).
 * The pointer is only valid until the next call!  */
const nstr* next_field (void);


#endif  // INPUT_H
