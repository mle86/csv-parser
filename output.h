#ifndef OUTPUT_H
#define OUTPUT_H

/**
 * Output functions
 * and output mode-related definitions.
 *
 * Call sequence:
 *
 *   - On program start: set_output().
 *   
 *   - If input is empty: output_empty().
 *   - If input is non-empty:
 *       - output_begin().
 *       - For each input record:
 *           - output_line_begin().
 *           - For each field in the record: output_kv().
 *           - output_line_end().
 *       - output_end().
 *
 * This file is part of the 'csv-parser' project
 * (see https://github.com/mle86/csv-parser).
 *
 * SPDX-License-Identifier: MIT
 * Copyright © 2017-2020 Maximilian Eul
 */

#include <stdbool.h>
#include "nstr.h"
#include "const.h"


#define SIMPLE_LINESEP "-\n"
#define SIMPLE_KVSEP ": "

#define CSV_FIELDSEP ','
#define CSV_FIELDENC '"'
#define CSV_LINESEP "\r\n"

// pretty-printing colors:
#define PP_KEY Mgreen
#define PP_SYM Myellow
#define PP_ESC Mcyan
#define PP_RST M0

// variable name templates:
#define SHVAR_COLNAME	"CSV_COLNAME_%zu"
#define SHVAR_CELL	"CSV_%zu_%zu"
#define SHVAR_N_RECORDS	"CSV_RECORDS"

typedef enum outmode {
	OM_SIMPLE,
	OM_JSON,
	OM_JSON_NUMBERED,  // special mode, only reachable with mode combination -ij.
	OM_JSON_COMPACT,
	OM_SHELL_VARS,
	OM_SHELL_VARS_NUMBERED,  // special mode, only reachable with mode combination -iX.
	OM_CSV,
	OM_CSV_NUMBERED,  // special mode, only reachable with mode combination -iC.
} outmode_t;


/** Call this once (prior to any other output_* calls) to initialize the output module. */
void set_output (outmode_t mode, bool do_flush, bool pretty, const char* shvar_prefix);

/** Call this once to start non-empty output, i.e. shortly before printing the first record using output_line_begin(). */
void output_begin (void);
/** Call this once to terminate non-empty output, i.e. after the last record has been printed. */
void output_end   (void);
/** Call this once (instead of output_begin/output_end) if there are no input records to be printed. */
void output_empty (void);
/** Call this once for every input record before calling output_kv(). */
void output_line_begin (void);
/** Call this once for every input record after the record's last key--value pair has been printed using output_kv(). */
void output_line_end   (void);

void output_kv (const nstr* key, const nstr* value);


#endif  // OUTPUT_H
