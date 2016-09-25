#include "input.h"
#include "const.h"
#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char   separator;
static size_t line_number;
static bool   first_line;
static bool   remove_bom;
static bool   allow_breaks;
static size_t limit_lines;
static size_t skip_lines;
static FILE*  input;

static char   fieldbuffer [MAXLINELEN];
static char   linebuffer  [MAXLINELEN];
static char*  cur_line;

// recognized separator characters for "--separator auto":
#define issep(c) (c == ',' || c == ';' || c == '\t' || c == '|')
// recognized quoting characters:
#define isq(c) (c == '"' || c == '\'')

static char* get_line (void);
static void skip (size_t n);
static unsigned short check_bom (const char* s);
static bool is_lineend (const char* s);
static void find_separator (const char* s);


void set_input (FILE* file, char _separator, bool _allow_breaks, bool _remove_bom, bool skip_after_header, size_t _skip_lines, size_t _limit_lines) {
	input         = file;
	cur_line      = NULL;
	line_number   = 0;
	first_line    = true;
	allow_breaks  = _allow_breaks;
	separator     = _separator;
	remove_bom    = _remove_bom;
	limit_lines   = _limit_lines;
	skip_lines    = 0;

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
	char buf [MAXLINELEN];
	while (n-- > 0 && fgets(buf, sizeof(buf), input))
		line_number++;
}

char* get_line (void) {
	linebuffer[0] = '\0';

	if (limit_lines > 0 && line_number >= limit_lines)
		// line limit reached, only return NULLs from here on
		return NULL;

	if (! fgets(linebuffer, sizeof(linebuffer), input))
		return NULL;

	line_number++;

	if (first_line) {
		first_line = false;

		if (separator == '\0')
			find_separator(linebuffer);

		if (skip_lines)
			// Looks like we just read the header line,
			// and now we're supposed to skip some content lines.
			skip(skip_lines);

		if (remove_bom)
			// skip bom in returned input:
			return linebuffer + check_bom(linebuffer);
	}

	return linebuffer;
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
	/* eol */	    s[0] == '\0'
	/* CR */	|| (s[0] == '\n' && s[1] == '\0')
	/* CRLF */	|| (s[0] == '\n' && s[1] == '\r' && s[2] == '\0')
	/* LF */	|| (s[0] == '\r' && s[1] == '\0')
	/* LFCR */	|| (s[0] == '\r' && s[1] == '\n' && s[2] == '\0')
	);
}

void find_separator (const char* s) {
	while (*s && !issep(*s))
		// this will stop at the first valid separator char,
		// or at the trailing NUL if there are no sepchars.
		s++;

	if (*s) {
		separator = *s;
		VERBOSE("found separator \"%c\" on line %zu\n", separator, lineno());
	} else {
		ERR(EXIT_NOSEP, "no separator found on line %zu\n", lineno());
		separator = '\0';
	}
}

bool next_line (void) {
	cur_line = get_line();
	return (cur_line != NULL);
}

const char* next_field (void) {
	if (!cur_line || !cur_line[0])
		return NULL;

	fieldbuffer[0] = '\0';
	size_t f = 0;
	#define addf(c) do{ fieldbuffer[f++] = (c); }while(0)

	bool toolong = false;

	#define t cur_line  /* just an abbreviation */
	#define quoted (quote != '\0')  /* if(quoted) looks better than if(quote) */

	char quote = '\0';
	if (isq(t[0])) {
		quote = t[0];
		t++;
	}

	/* fin(nextptr)
	 *  Usually, this macro will set cur_line to a suitable position for the following next_field() call,
	 *  i.e. to the start of the next field (if there is one on the current line)
	 *       or to NULL if cur_line's end is reached.
	 *  After that, it will terminate fieldbuffer by writing a NUL to the current position (f)
	 *  and return that array.
	 *
	 *  However, if toolong==true,
	 *  we must not return the fieldbuffer, as it is an incomplete value.
	 *  cur_line should still be advanced to the next field,
	 *  but the macro will then clear the toolong flag and return an empty string.  */
	#define fin(nextptr) (					\
			(cur_line = (nextptr)),			\
			(fieldbuffer[f] = '\0'),		\
			(toolong ? ((toolong=false), "")	\
			         : fieldbuffer)			\
		)


	while (true) {
		if (f >= sizeof(fieldbuffer) - 1) {
			f = 0;  // this way, we'll never write past fieldbuffer's boundaries
			if (! toolong) {
				toolong = true;
				// Only show this error once
				ERR(EXIT_FORMAT, "field too long on line %zu\n", lineno());
			}
		}

		if (quoted && t[0] == quote) {
			// We just stumbled upon the quote char.
			if (t[1] == quote) {
				// double quote means escaped quote.
				// skip this one, only write second one to fieldbuffer:
				t++;
			} else if (t[1] == separator) {
				// field end after the quote. place cur_line on next field start
				return fin(t + 2);
			} else if (is_lineend(t + 1)) {
				// line end after the quote. clear cur_line so the following next_field() call will fail
				return fin(NULL);
			} else {
				ERR(EXIT_FORMAT, "unexpected quote on line %zu\n", lineno());
			}

		} else if (!quoted && t[0] == separator) {
			// field end. place cur_line on next field start
			return fin(t + 1);

		} else if (quoted && is_lineend(t)) {
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

		} else if (is_lineend(t)) {
			// line end. clear cur_line so the following next_field() call will fail
			return fin(NULL);
		}

		// append the current char to the fieldbuffer and continue.
		addf(*(t++));
	}
}

