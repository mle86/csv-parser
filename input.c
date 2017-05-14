#define _POSIX_C_SOURCE 200809L
#include "input.h"
#include "const.h"
#include "global.h"
#include "chars.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char       separator;
static char       enclosure;
static size_t     line_number;
static bool       first_line;
static bool       remove_bom;
static bool       allow_breaks;
static size_t     limit_lines;
static size_t     skip_lines;
static FILE*      input;
static trimmode_t trim;

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
 *  if 'line_number' has already reached 'limit_lines' (assuming it's set).
 */
static bool get_line (void);

/**
 * Reads and discards 'n' input lines, updating 'line_number'.
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
 * Tried to find a separator character on the current line and store it in 'separator'.
 * If nothing is found, 'separator' won't be changed.
 */
static void find_separator (void);


void set_input (FILE* file, char _separator, char _enclosure, bool _allow_breaks, bool _remove_bom, bool skip_after_header, size_t _skip_lines, size_t _limit_lines, trimmode_t _trim) {
	input         = file;
	line_number   = 0;
	first_line    = true;
	allow_breaks  = _allow_breaks;
	separator     = _separator;
	enclosure     = _enclosure;
	remove_bom    = _remove_bom;
	limit_lines   = _limit_lines;
	trim          = _trim;
	skip_lines    = 0;

	lp            = NULL;
	cur_line_len  = 0;

	if (_skip_lines) {
		// Skip some lines.

		if (limit_lines)
			// skipped lines count towards line_number
			limit_lines += _skip_lines;

		if (skip_after_header) {
			// Don't skip the initial header line!
			// get_line() will do the skipping after reading the header line.
			skip_lines = _skip_lines;
		} else {
			// Skip first N lines right now,
			// including the header line (if there is one).
			skip(_skip_lines);
		}
	}
}

inline size_t lineno (void) {
	return line_number;
}

inline void skip (size_t n) {
	char* buf = NULL;
	size_t bufsz = 0;

	while (n-- > 0 && getline(&buf, &bufsz, input))
		line_number++;

	if (ferror(input)) {
		ERR(EXIT_INTERNAL, "read error");
		clearerr(input);
	}

	free(buf);
}

bool get_line (void) {
	if (limit_lines > 0 && line_number >= limit_lines)
		// line limit reached, don't read any more
		return false;

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

		if (skip_lines)
			// Looks like we just read the header line,
			// and now we're supposed to skip some content lines.
			skip(skip_lines);

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

bool next_line (void) {
	return get_line();
}

const nstr* next_field (void) {
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
				return fin(NULL);
			} else {
				ERR(EXIT_FORMAT, "unexpected quote on line %zu\n", lineno());
			}

		} else if (!quoted && is_lineend(lp)) {
			// line end. clear cur_line so the following next_field() call will fail
			return fin(NULL);

		} else if (!quoted && lp[0] == separator) {
			// field end. place cur_line on next field start
			return fin(lp + 1);

		} else if (quoted && is_lineend(lp)) {
			// line end in quoted string!

			if (! allow_breaks) {
				ERR(EXIT_FORMAT, "unexpected end of line on line %zu\n", lineno());
				return fin(NULL);  // end quoted field here
			}

			addf('\n');

			if (! next_line()) {
				ERR(EXIT_FORMAT, "unexpected end of file on line %zu\n", lineno());
				return fin(NULL);  // end quoted field here
			}
			continue;  // re-enter loop on next line
		}

		// append the current char to the fieldbuffer and continue.
		addf(*(lp++));
	}
}

