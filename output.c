#include "output.h"
#include "global.h"
#include "const.h"
#include <stdio.h>
#include <stdbool.h>

static outmode_t mode = OM_SIMPLE;
static bool do_flush = false;
static bool pretty = false;

static bool first_line = true;
static bool first_kv   = true;

static void json_print (const char* value);
static void nobr_print (const char* value);

#define PRETTY if(pretty)


inline void set_output (outmode_t _mode, bool _do_flush, bool _pretty) {
	first_line = true;
	first_kv   = true;
	mode       = _mode;
	do_flush   = _do_flush;
	pretty     = _pretty;
}


void output_begin (void) {
  switch (mode) {
	case OM_SIMPLE:
		break;
	case OM_JSON:
	case OM_JSON_NUMBERED:
		fputc('[', stdout);
		PRETTY{ fputs(P_SYM, stdout); }
		break;
	case OM_JSON_COMPACT:
		fputs("{\"columns\": [", stdout);
		for (size_t c = 0; c < MAXCOLUMNS; c++) {
			if (ColumnName[c])
				output_kv(NULL, ColumnName[c]);
		}
		fputs("],\n\"lines\": [\n ", stdout);
		PRETTY{ fputs(P_SYM, stdout); }
		first_kv = true;  // output_kv() has cleared it
		break;
} }

void output_end (void) {
	PRETTY{ fputs(P_RST, stdout); }

	switch (mode) {
		case OM_SIMPLE:
			break;
		case OM_JSON:
		case OM_JSON_NUMBERED:
			fputs(" ]\n", stdout);
			break;
		case OM_JSON_COMPACT:
			fputs(" ]}\n", stdout);
			break;
	}
}

void output_line_begin (void) {
	switch (mode) {
		case OM_SIMPLE:
			if (! first_line) {
				if (pretty)
					fputs(P_SYM "-\n" P_RST, stdout);
				else	fputs(      "-\n"      , stdout);
			}
			break;
		case OM_JSON:
			if (!first_line)
				fputs("\n,", stdout);
			if (pretty)
				fputs(P_SYM "{" P_RST, stdout);
			else	fputc(      '{'      , stdout);
			break;
		case OM_JSON_COMPACT:
		case OM_JSON_NUMBERED:
			if (pretty) {
				if (first_line)
					fputs(      P_SYM "[" P_RST, stdout);
				else	fputs("\n," P_SYM "[" P_RST, stdout);
			} else {
				if (first_line)
					fputc(   '[', stdout);
				else	fputs("\n,[", stdout);
			}
			break;
	}
	first_line = false;
	first_kv   = true;
}

void output_line_end (void) {
	switch (mode) {
		case OM_SIMPLE:
			break;
		case OM_JSON:
			if (pretty)
				fputs(P_SYM "}" P_RST, stdout);
			else	fputc(      '}'      , stdout);
			break;
		case OM_JSON_NUMBERED:
		case OM_JSON_COMPACT:
			if (pretty)
				fputs(P_SYM "]" P_RST, stdout);
			else	fputc(      ']'      , stdout);
			break;
	}

	if (do_flush)
		fflush(stdout);
}

void output_kv (const char* key, const char* value) {
	switch (mode) {
		case OM_SIMPLE:
			PRETTY{ fputs(P_KEY, stdout); }
			fputs(key, stdout);
			PRETTY{ fputs(P_RST, stdout); }
			fputs(", ", stdout);
			nobr_print(value);
			fputc('\n', stdout);
			break;

		case OM_JSON:
			if (! first_kv)
				fputs(pretty ? (P_SYM "," P_RST) : ",", stdout);
			if (pretty)
				fputs("\"" P_KEY, stdout);
			else	fputc( '"'      , stdout);
			json_print(key);
			if (pretty)
				fputs(P_RST "\":\"", stdout);
			else	fputs(      "\":\"", stdout);
			json_print(value);
			fputc('"', stdout);
			break;

		case OM_JSON_NUMBERED:
		case OM_JSON_COMPACT:;
			const bool is_colname = (mode == OM_JSON_COMPACT && !key);
			if (! first_kv) {
				if (pretty && !is_colname)
					fputs(P_SYM "," P_RST, stdout);
				else	fputc(      ','      , stdout);
			}
			if (pretty && is_colname)
				fputs("\"" P_KEY, stdout);
			else	fputc( '"'      , stdout);
			json_print(value);
			if (pretty && is_colname)
				fputs(P_RST "\"", stdout);
			else	fputc(       '"', stdout);
			break;
	}

	first_kv = false;
}


void json_print (const char* value) {
	for (register const unsigned char* c = (const unsigned char*)value; *c; c++)
	switch (*c) {
		case 0x07: fputs("\\b", stdout); break;
		case 0x09: fputs("\\t", stdout); break;
		case 0x0a: fputs("\\n", stdout); break;
		case 0x0c: fputs("\\f", stdout); break;
		case 0x0d: fputs("\\r", stdout); break;
		case '/':  fputs("\\/", stdout); break;
		case '\\': fputs("\\\\", stdout); break;
		case '"':  fputs("\\\"", stdout); break;
		default:
			if (*c <= 31)
				printf("\\u%04X", (unsigned int)*c);
			else if (c[0]==0xe2 && c[1]==0x80 && c[2]==0xa8) {
				// JS does not like these.
				// http://timelessrepo.com/json-isnt-a-javascript-subset
				fputs("\\u2028", stdout);
				c += 2;
			} else if (c[0]==0xe2 && c[1]==0x80 && c[2]==0xa9) {
				fputs("\\u2029", stdout);
				c += 2;
			} else
				fputc(*c, stdout);
	}
}

void nobr_print (const char* value) {
	register const char* c = value;
	while (*c) {
		if (*c == '\\')
			// print an extra backslash
			fputs("\\\\", stdout);
		else if (c[0] == '\r' && c[1] == '\n') {
			// collapse CRLF to single "\n"
			fputs("\\n", stdout);
			c++;
		} else if (c[0] == '\r' || c[0] == '\n') {
			// output single linebreaks as "\n"
			fputs("\\n", stdout);
		} else
			fputc(*c, stdout);
		
		c++;
	}
}

