#include <stdio.h>
#include <ctype.h>
#include "escape.h"
#include "nstr.h"
#include "global.h"


static bool is_shvar_safe (const nstr* str);

#define in_range(c, r0, r1) \
	(((unsigned char)(c)) >= ((unsigned char)(r0)) && ((unsigned char)(c)) <= ((unsigned char)(r1)))

/**
 * This macro begins the main loop for all escape_fn_t functions.
 * The loop body should include the ruleset file.
 */
#define apply_rules(nstrp) \
	for (register const char* raw = (nstrp)->buffer; raw < (nstrp)->buffer + (nstrp)->length; )

/**
 * This macro expands to a byte-for-byte string comparison:
 * If 'raw' currently starts with the 'rs' string,
 *  the 'es' string is printed (without its trailing NUL)
 *  and 'raw' is incremented past the 'rs' occurrence.
 * If 'rs' does not match the string at 'raw', nothing happens.
 * Both 'rs' and 'es' must be string literals so that sizeof() works on them.
 */
#define escape(rs, es)  {  \
		size_t p;				\
		for (p = 0; p < sizeof(rs) - 1; p++)	\
			if (rs[p] != raw[p]) {		\
				/* mismatch */		\
				break;			\
			}				\
		if (p == sizeof(rs) - 1) {		\
			/* it's a match, output 'es' */	\
			output_escape_sequence(es);	\
			raw += sizeof(rs) - 1;		\
			/* skip the other rules: */	\
			continue;			\
		}					\
	}

/**
 * This macro expands to a character check of raw[0]:
 * If the character value is between r0 and r1, it will be skipped
 *  (i.e. raw will be increased by 1),
 *  and printf(el, ef, raw[0]) will be called
 *  to print a replacement escape sequence.
 * If raw[0] is not within r0..r1, nothing happens.
 */
#define escape_other(a0, a1, el, ef)  \
	if (in_range(raw[0], a0, a1)) {			\
		/* it's a range match, output 'ef' */	\
		output_fmt_escape_sequence(el, ef);	\
		raw++;					\
		/* skip the other rules: */		\
		continue;				\
	}						\

/**
 * This macro outputs the current raw[0] character unchanged and increases raw by 1.
 * This should be called for every input character
 * which does not match any of the escape() and escape_other() rules.
 * Since the escape() and escape_other() macros both call "continue" after a match,
 * it's safe to simply call this at the end of the loop.
 */
#define cpyraw()  output_chr(*(raw++))


////////////////////////////////////////////////////////////////////////////////
// Preparations for the escape_fn_t functions,
// which write directly to stdout:

#define output_chr(c)  fputc((c), stdout)

#define output_escape_sequence(es)  do{  \
		if (pp_esc != NULL) { fputs(pp_esc, stdout); }			\
		fputs(es, stdout);						\
		if (pp_rst != NULL) { fputs(pp_rst, stdout); }			\
	}while(0)

#define output_fmt_escape_sequence(el, ef)  do{  \
		if (pp_esc != NULL) { fputs(pp_esc, stdout); }			\
		printf(ef, raw[0]);						\
		if (pp_rst != NULL) { fputs(pp_rst, stdout); }			\
	}while(0)


void escape_shvar (const nstr* input, const char* pp_esc, const char* pp_rst) {
	if (is_shvar_safe(input)) {
		// If a value consists of safe chars only, it does not need to be quoted or escaped
		fputs(input->buffer, stdout);
		return;
	}

	output_chr('"');
	apply_rules(input) {
		#include "def/shvar-escape-rules.def"
		cpyraw();
	}
	output_chr('"');
}

void escape_nobr (const nstr* input, const char* pp_esc, const char* pp_rst) {
	apply_rules(input) {
		#include "def/nobr-escape-rules.def"
		cpyraw();
	}
}

void escape_json (const nstr* input, const char* pp_esc, const char* pp_rst) {
	apply_rules(input) {
		#include "def/json-escape-rules.def"
		cpyraw();
	}
}


inline bool is_shvar_safe (const nstr* str) {
	#define is_safe_chr(c) (									\
			isalnum(c) ||									\
			c == '_' || c == '-' || c == '+' || c == ',' || c == '.' || c == ':' ||		\
			c == '@' || c == '/' || c == '[' || c == ']' || c == '{' || c == '}' )
	for (register const char* s = str->buffer; s < str->buffer + str->length; s++)
		if (!is_safe_chr(*s))
			return false;
	return true;
}

