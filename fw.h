#ifndef FW_H
#define FW_H

/**
 * Specialized functions and types
 * for fixed-width input.
 *
 * This file is part of the 'csv-parser' project
 * (see https://github.com/mle86/csv-parser).
 *
 * SPDX-License-Identifier: MIT
 * Copyright © 2019 Maximilian Eul
 */

#include <stdbool.h>
#include "nstr.h"


typedef struct fixed_width_column {
	/**
	 * Index of first character of this field.
	 * Counting starts at 1.  Must be positive!
	 */
	size_t start;

	/**
	 * Index of last character of this field.
	 * Counting starts at 1.  Can be set to zero to indicate “until end of line”.
	 */
	size_t end;
} fw_col_t;


/**
 * Initializes the fixed-width input.
 *
 * This needs to be called once
 * before calling next_fw_field() for the first time!
 *
 * column_definitions[] may be unsorted but may not be empty.
 * n_columns must reflect the number of records in column_definitions[].
 */
void set_fw (const fw_col_t column_definitions[], size_t n_columns);

/**
 * Returns true if set_fw() was previously called with n_columns > 0;
 * returns false otherwise.
 */
bool is_fw (void);

/**
 * Reads and returns the next fixed-width field from the line.
 * Assumes that 'lp' is a pointer to the next position on the line
 * or to the start of the line in case of a new, not previously-read line.
 *
 * On success, the function will return the field as-is
 * and set *lp to the starting position of the next fixed-width field
 * or to NULL if the line's last field has been returned.
 * If there are no more fields on the line, the function will return NULL
 * and not change *lp.
 *
 * The returned pointer is valid until the next call.
 */
nstr* next_fw_field (const char* line, size_t linelen, const char* *lp);


#endif  // FW_H
