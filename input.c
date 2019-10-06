/*
 * This file is part of the 'csv-parser' project
 * (see https://github.com/mle86/csv-parser).
 *
 * SPDX-License-Identifier: MIT
 * Copyright © 2017-2019 Maximilian Eul
 */

#define _POSIX_C_SOURCE 200809L
#include "input.h"
#include "const.h"
#include "global.h"
#include "chars.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Number of the last-read line, indexed by 1.
 * Increased by get_line() after EOL.
 * This includes skipped lines.
 */
static size_t line_number;

/**
 * Number of the last complete record, indexed by 1.
 * Increased by next_field() after EOR.
 * Does NOT include skipped records.
 */
static size_t record_number;

static char         separator;
static char         enclosure;
static bool         first_line;
static bool         do_skip;
static bool         remove_bom;
static bool         allow_breaks;
static size_t       limit_records;
static size_t       skip_records;
static FILE*        input;
static trimmode_t   trim;
static filtermode_t filter;

/**
 * This is where get_line() will store its results.
 * set_input() will always clear it.
 */
static char*  cur_line = NULL;
static size_t cur_line_len = 0;
static size_t cur_line_bufsz = 0;

/**
 * While reading cur_line, we need a pointer to increase which keeps its position between next_field() calls.
 * We cannot simply increase the cur_line pointer itself, because getline(3) might call realloc() on it;
 * we use this pointer instead.  Our get_line() always must reset lp to &cur_line[0] if successful.
 * Our next_field() always sets this to NULL if there are no more fields on the current line.
 */
static const char* lp = NULL;

/** recognized trimmable characters */
#define istrim(c) (c != separator && (c == ' ' || c == '\t'))

/**
 * Reads one input line into 'cur_line', stores the line length in 'cur_line_len'
 *  (including the trailing linebreak but excluding the trailing NUL byte),
 *  and returns TRUE.
 * 'line_number' will be increased by 1.
 * Always returns FALSE (and does not read from input)
 *  if 'record_number' has already reached 'limit_records' (assuming it's set).
 */
static bool get_line (void);

/**
 * Returns a pointer to an nstr containing the next field,
 * or NULL if there was no next field on the current line (EOL or EOF).
 * The pointer is only valid until the next call!
 */
static const nstr* get_field (void);

/**
 * Reads and discards 'n' input records, updating 'line_number'.
 * Does not change 'cur_line' or 'cur_line_len' in any way.
 */
static void skip (size_t n);

/**
 * Tries to find a UTF-x BOM on the 's' line and return the length of the found BOM.
 * If no BOM is found, zero is returned.
 */
static unsigned short check_bom (const char* s);

/**
 * Determines whether s currently points to the line-end of cur_line.buffer.
 * This can mean an LF, CRLF, or just the end of the buffer.
 */
static bool is_lineend (const char* s);

/**
 * Tries to find a separator character on the current line and store it in 'separator'.
 * If nothing is found, 'separator' won't be changed.
 */
static void find_separator (void);

/**
 * Used by next_line()
 * to determine if the cur_line should be returned or skipped.
 */
static bool is_filterable_line (void);

/**
 * Used by is_filterable_line()
 * to determine if the current field is empty.
 */
static bool is_filterable_field (const char* field);

static bool isspaces (const char* s);
inline bool iszero (const char* s);


void set_input (FILE* file, char _separator, char _enclosure, bool _allow_breaks, bool _remove_bom, bool skip_after_header, size_t _skip_records, size_t _limit_records, trimmode_t _trim, filtermode_t _filter) {
	input         = file;
	line_number   = 0;
	record_number = 0;
	first_line    = true;
	do_skip       = false;
	allow_breaks  = _allow_breaks;
	separator     = _separator;
	enclosure     = _enclosure;
	remove_bom    = _remove_bom;
	limit_records = _limit_records;
	trim          = _trim;
	filter        = _filter;
	skip_records  = 0;

	lp            = NULL;
	cur_line_len  = 0;

	if (_skip_records) {
		// Skip some records.

		if (skip_after_header) {
			// Don't skip the initial header line!
			// get_line() will do the skipping after reading the header line.
			skip_records = _skip_records;
		} else {
			// Skip first N lines right now,
			// including the header line (if there is one).
			skip(_skip_records);
		}
	}
}

inline size_t lineno (void) {
	return line_number;
}

inline void skip (size_t n) {
	char* buf = NULL;
	size_t bufsz = 0;

	if (!allow_breaks) {
		// Each line is one record.
		// We can easily skip N records by skipping N lines:
		while (n-- > 0 && getline(&buf, &bufsz, input))
			line_number++;

	} else {
		// Records may span multiple lines.
		// We have to read and process them correctly:
		while (n-- > 0) {
			if (!get_line()) {
				return;  // eof
			}
			while (get_field()) ;
		}
	}

	if (ferror(input)) {
		ERR(EXIT_INTERNAL, "read error");
		clearerr(input);
	}

	free(buf);
}

bool get_line (void) {
	if (limit_records > 0 && record_number >= limit_records)
		// line limit reached, don't read any more
		return false;

	if (do_skip && skip_records > 0) {
		// This flag has been set by next_field()
		// after it read the entire first record (header).
		do_skip = false;
		skip(skip_records);
	}

	ssize_t len = getline(&cur_line, &cur_line_bufsz, input);
	if (len < 0) {
		if (ferror(input)) {
			ERR(EXIT_INTERNAL, "read error");
			clearerr(input);
		}
		cur_line_len = 0;
		return false;
	} else {
		cur_line_len = len;
	}

	line_number++;
	lp = cur_line;  // !

	if (first_line) {
		first_line = false;

		if (separator == SEP_AUTO)
			find_separator();
		if (remove_bom)
			// skip bom in returned input:
			lp += check_bom(cur_line);
	}

	if (trim & TRIM_LINES_L) {
		while (istrim(*lp))
			lp++;
	}
	if (trim & TRIM_LINES_R) {
		char* endptr = cur_line + cur_line_len - 1;
		char* ep = endptr;
		if (*ep == '\n')  ep--;
		if (*ep == '\r')  ep--;
		while (ep >= lp && istrim(*ep))
			ep--;
		if (ep != endptr) {
			ep[1] = '\0';
			cur_line_len -= (endptr - ep);
		}
	}

	return true;
}

unsigned short check_bom (const char* s) {
	#define chkbom(bom) do{ if (0 == strncmp(s, bom, sizeof(bom) - 1)) { return sizeof(bom) - 1; } }while(0)

	chkbom("\xEF\xBB\xBF");  // UTF-8
	chkbom("\xFE\xFF");  // UTF-16 BE
	chkbom("\xFF\xFE");  // UTF-16 LE

	return 0;
}

bool is_lineend (const char* s) {
	return (
//	/* eol */	    s[0] == '\0'
	/* eol */	   (s >= cur_line + cur_line_len)
	/* CR */	|| (s[0] == '\n' && s[1] == '\0')
	/* CRLF */	|| (s[0] == '\n' && s[1] == '\r' && s[2] == '\0')
	/* LF */	|| (s[0] == '\r' && s[1] == '\0')
	/* LFCR */	|| (s[0] == '\r' && s[1] == '\n' && s[2] == '\0')
	);
}

void find_separator (void) {
	register char* p = cur_line;
	for (char q = '\0'; p < cur_line + cur_line_len; p++) {
		// this will stop at the first valid separator char,
		// or at the trailing NUL if there are no sepchars.
		// quoted fields will be skipped.
		if (!q && isq(*p))
			q = *p;
		else if (q && *p == q)
			q = '\0';
		else if (!q && issep(*p))
			break;
	}

	if (*p) {
		separator = *p;
		VERBOSE("found separator \"%c\" on line %zu\n", separator, lineno());
	} else {
		ERR(EXIT_NOSEP, "no separator found on line %zu\n", lineno());
		separator = '\0';
	}
}

/**
 * Wrapper around get_line().
 * Silently skips over records matching the filtermode,
 *  up to and including skipping over the entire input if it's all filterable records.
 */
bool next_line (void) {
	if (filter == FILTER_NONE) {
		// Simple case: no filtering.
		return get_line();
	}

	bool line_status;
	while ((line_status = get_line()) && is_filterable_line()) {
		// Keep reading lines until we either hit EOF or find a non-empty line.
	}

	return line_status;
}

/**
 * Wrapper around get_field().
 * Increases 'record_number' after there are no more fields on the current line,
 * i.e. whenever it returns the final field and 'lp' has been re-set to NULL.
 */
const nstr* next_field (void) {
	const size_t original_record = record_number;

	const nstr* f = get_field();

	if (f && !lp) {
		record_number++;

		if (record_number == 1 && skip_records > 0) {
			// This was the header record and now we're supposed to skip some records.
			// We cannot do it right here, or we'll overwrite our 'field' buffer and return garbage.
			// But we can leave a flag for the next get_line() call:
			do_skip = true;
		}
	}

	if (f && !lp) {
		VERBOSE("line %zu, record %zu (orig %zu), field \"%s\"\n", line_number, record_number, original_record, f&&f->buffer ? f->buffer : "//");
	}

	return f;
}

const nstr* get_field (void) {
	static nstr* field = NULL;
	static size_t bsz = 0;
	#define BSZ_INITIAL 4095
	#define BSZ_ADD 1024

	if (field == NULL) {
		// Initialize the field buffer
		bsz   = BSZ_INITIAL;
		field = nstr_init(bsz);
	}

	if (!cur_line || !lp || lp > cur_line + cur_line_len)
		// EOL/EOF
		return NULL;

	char quote = '\0';

	if (enclosure == ENC_AUTO || enclosure == ENC_MIXED) {
		if (isq(lp[0])) {
			// quoted field
			quote = lp[0];
			lp++;

			if (enclosure == ENC_AUTO) {
				enclosure = quote;
				VERBOSE("found field enclosure character %c on line %zu\n", enclosure, lineno());
			}
		}
	} else if (enclosure == ENC_NONE) {
		// field cannot be enclosed, ignore all potential enclosure characters
	} else if (lp[0] == enclosure) {
		// quoted field
		quote = lp[0];
		lp++;
	}

	nstr_truncate(field);

	#define quoted (quote != '\0')  /* if(quoted) looks better than if(quote) */

	/**
	 * This addf() macro appends one character to the end of the 'field' nstr.
	 * Resizing and NUL-terminating are handled automatically.  */
//	#define addf(c) do{ nstr_appendc_a(&field, (c)); }while(0)
	/**
	 * This addf() macro appends one character to the end of the 'field' nstr,
	 * assuming that it is long enough.  That's why there is a resize check
	 * in the read loop.  It also does NOT nul-terminate the buffer --
	 * the fin() macro will do it.  */
	#define addf(c) do{ field->buffer[ field->length++ ] = (c); }while(0)

        /* fin(nextptr)
         *  This macro will set lp to a suitable position for the following next_field() call,
         *  i.e. to the start of the next field (if there is one on the current line)
         *       or to NULL if cur_line's end is reached.
         *  After that, it will terminate the field buffer by writing a NUL to the current position
	 *  (addf() might not have done that yet) and return the string.  */
        #define fin(nextptr) (					\
                        (lp = (nextptr)),			\
			(field->buffer[field->length] = '\0'),	\
			field					\
                )

	while (true) {

		if (field->length + 1 >= bsz) {
			// Resize the input buffer.
			// The nstr_append*_a() functions also do that, so if addf() uses them, this block is unreachable.
			// But if addf() does the append operation itself, this block is needed for long fields!
			nstr_resize(&field, (bsz += 1 + BSZ_ADD));
		}

		if (quoted && lp[0] == quote) {
			// We just stumbled upon the quote char.
			if (lp[1] == quote) {
				// double quote means escaped quote.
				// skip this one, only write second one to fieldbuffer:
				lp++;
			} else if (lp[1] == separator) {
				// field end after the quote. place cur_line on next field start
				return fin(lp + 2);
			} else if (is_lineend(lp + 1)) {
				// line end after the quote. clear cur_line so the following next_field() call will fail
				break;
			} else {
				ERR(EXIT_FORMAT, "unexpected quote on line %zu\n", lineno());
			}

		} else if (!quoted && is_lineend(lp)) {
			// line end. clear cur_line so the following next_field() call will fail
			break;

		} else if (!quoted && lp[0] == separator) {
			// field end. place cur_line on next field start
			return fin(lp + 1);

		} else if (quoted && is_lineend(lp)) {
			// line end in quoted string!

			if (! allow_breaks) {
				ERR(EXIT_FORMAT, "unexpected end of line on line %zu\n", lineno());
				break;  // end quoted field here
			}

			addf('\n');

			if (! get_line()) {
				ERR(EXIT_FORMAT, "unexpected end of file on line %zu\n", lineno());
				break;  // end quoted field here
			}
			continue;  // re-enter loop on next line
		}

		// append the current char to the fieldbuffer and continue.
		addf(*(lp++));
	}

	// Record end.

	// Return the last field and clear cur_line:
	return fin(NULL);
 #undef fin
 #undef addf
 #undef quoted
}

bool is_filterable_line (void) {
	/* To determine whether the current line/record should be dropped,
	 * we'll walk all of its fields (using next_field)
	 * and test them against is_filterable_field().
	 * If any one of them is non-empty, we can instantly return false (= do not drop this record),
	 * we'll just have to reset the 'lp' pointer to the line start
	 * so that the caller can walk the line with next_field again.  */

	const size_t original_line = line_number;  // for better verbose msg in case of multi-line records

	const nstr* field;
	while ((field = get_field())) {
		if (!is_filterable_field(field->buffer)) {
			// non-empty record --> non-empty line!
			lp = cur_line;  // reset lp so that the caller can re-read the line with next_field:
			return false;
		}
	}

	// We found only empty/filterable fields on that line, or possibly no fields at all:
	VERBOSE("skipped filtered line %zu\n", original_line);
	return true;
}

inline bool is_filterable_field (const char* field) {
	switch (filter) {
		case FILTER_EMPTY:
			return (field[0] == '\0');
		case FILTER_ZEROES:
			return (iszero(field));
		case FILTER_EMPTY_OR_ZEROES:
			return (field[0] == '\0' || iszero(field));
		case FILTER_BLANK_OR_ZEROES:
			return (field[0] == '\0' || iszero(field) || isspaces(field));
		case FILTER_BLANK:
			return (field[0] == '\0' || isspaces(field));
		default:
			return false;
	}
}

inline bool isspaces (const char* s) {
	while (*s) {
		if (!isspace(*(s++)))
			return false;
	}
	return true;
}

inline bool iszero (const char* s) {
	/* Recognized "zero" formats:
	 *  0		S0,S1
	 *  000		S0,S1+
	 *  .000	S0,S2+
	 *  0.		S0,S1,S3
	 *  0.000	S0,S1,S3+
	 *  000.	S0,S1+,S3
	 *  000.000	S0,S1+,S3+
	 */

	#define NS(nextState) do{ s++; goto nextState; }while(0)
	#define skipZeroes() do{ while(*s == '0') { s++; } }while(0)
	#define acceptEot() do{ return (*s == '\0'); }while(0)

// S0:
	if (*s == '.')  NS(S2);
	if (*s != '0')  return false;  // first char neither dot nor zero
	NS(S1);
 S1:
	skipZeroes();
	if (*s == '.')  NS(S3);
	acceptEot();  // zeroes only
 S2:
	if (!*s)  return false;  // dot without any zeroes
 S3:
	skipZeroes();
	acceptEot();

 #undef NS
 #undef skipZeroes
 #undef acceptEot
}

