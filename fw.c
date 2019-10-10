/*
 * This file is part of the 'csv-parser' project
 * (see https://github.com/mle86/csv-parser).
 *
 * SPDX-License-Identifier: MIT
 * Copyright © 2019 Maximilian Eul
 */

#include <stdlib.h>
#include "fw.h"
#include "input.h"
#include "global.h"
#include "const.h"
#include "aux.h"

static size_t           n_columns          = 0;
static const fw_col_t*  column_definitions = NULL;
static const fw_col_t*  prev_column        = NULL;

/** Pointer to the last record in colum_definitions[]. Undefined if !is_fw. */
#define FINAL_COLUMN (&(column_definitions[n_columns - 1]))

static inline unsigned short count_lineend_len (const char* line, size_t linelen);
static inline const fw_col_t* find_next_column_def (const char* cur_line, size_t cur_line_len, const char* *lp);


void set_fw (const fw_col_t coldef[], size_t ncol) {
	n_columns          = ncol;
	column_definitions = (ncol > 0) ? structcpy(coldef, sizeof(fw_col_t) * ncol) : NULL;
	prev_column        = NULL;
}


inline bool is_fw (void) {
	return (n_columns > 0);
}


nstr* next_fw_field (const char* line, size_t linelen, const char* *lp) {
	static nstr* field = NULL;
	static size_t bsz = 0;
	#define BSZ_INITIAL 4095
	#define BSZ_ADD 1024

	if (field == NULL) {
		// Initialize the field buffer
		bsz   = BSZ_INITIAL;
		field = nstr_init(bsz);
	}

	// Find definition of next column:
	const fw_col_t* col = find_next_column_def(line, linelen, lp);
	if (!col) {
		// This indicates that there are no more columns to process
		// or that the line was not long enough to read the following column:
		prev_column = NULL;
		return NULL;
	}

	// Find the exact field length to copy:
	size_t n;
	if (col->end) {
		n = 1 + col->end - col->start;
	} else {
		// Copy the rest of the line, without the trailing CRLF:
		n = &line[linelen] - *lp;
		n -= count_lineend_len(line, linelen);
	}

	// Ensure there is enough letters left on the line:
	if (*lp + n > line + linelen) {
		ERR(EXIT_FORMAT, "unexpected end of line on line %zu\n", lineno());
		n = (line + linelen) - *lp;
	}

	// Copy field contents to return buffer:
	nstr_truncate(field);
	nstr_appends_a(&field, *lp, n, &bsz);

	prev_column = col;  // !

	if (col == FINAL_COLUMN) {
		*lp = NULL;
	}

	return field;
}

const fw_col_t* find_next_column_def (const char* line, size_t linelen, const char* *lp) {
	const fw_col_t* col = prev_column;
	if (col && col >= FINAL_COLUMN) {
		// There are no more columns
		return NULL;
	} else if (col) {
		// Go to next:
		col++;
	} else {
		// New line, use first column definition:
		col = &column_definitions[0];
	}

	const size_t start_pos = col->start - 1;  // start/end are 1-indexed
	if (start_pos >= linelen) {
		ERR(EXIT_FORMAT, "unexpected end of line on line %zu\n", lineno());
		return NULL;
	}

	// Advance *lp to next column start pos, then return that column definition:
	*lp = &line[start_pos];
	return col;
}

unsigned short count_lineend_len (const char* line, size_t linelen) {
	if (linelen >= 2 && line[linelen - 2] == '\r' && line[linelen - 1] == '\n')
		return 2;  // CR LF
	else if (linelen >= 2 && line[linelen - 2] == '\n' && line[linelen - 1] == '\r')
		return 2;  // LF CR
	else if (linelen >= 1 && line[linelen - 1] == '\n')
		return 1;  // LF
	else if (linelen >= 1 && line[linelen - 1] == '\r')
		return 1;  // CR
	else
		return 0;
}

