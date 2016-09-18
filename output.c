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

#define prints(s) fputs(s, stdout)
#define printc(c) fputc(c, stdout)

// Prints an escape sequence. Like prints(), but will pretty-print using P_ESC.
static void printx (const char* s);


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
		printc('[');
		PRETTY{ prints(P_SYM); }
		break;
	case OM_JSON_COMPACT:
		prints("{\"columns\": [");
		for (size_t c = 0; c < MAXCOLUMNS; c++) {
			if (ColumnName[c])
				output_kv(NULL, ColumnName[c]);
		}
		prints("],\n\"lines\": [\n ");
		PRETTY{ prints(P_SYM); }
		first_kv = true;  // output_kv() has cleared it
		break;
} }

void output_end (void) {
	PRETTY{ prints(P_RST); }

	switch (mode) {
		case OM_SIMPLE:
			break;
		case OM_JSON:
		case OM_JSON_NUMBERED:
			prints("]\n");
			break;
		case OM_JSON_COMPACT:
			prints("]}\n");
			break;
	}
}

void output_line_begin (void) {
	switch (mode) {
		case OM_SIMPLE:
			if (! first_line) {
				if (pretty)
					prints(P_SYM SIMPLE_LINESEP P_RST);
				else	prints(      SIMPLE_LINESEP      );
			}
			break;
		case OM_JSON:
			if (!first_line)
				prints("\n,");
			if (pretty)
				prints(P_SYM "{" P_RST);
			else	printc(      '{'      );
			break;
		case OM_JSON_COMPACT:
		case OM_JSON_NUMBERED:
			if (pretty) {
				if (first_line)
					prints(      P_SYM "[" P_RST);
				else	prints("\n," P_SYM "[" P_RST);
			} else {
				if (first_line)
					printc(   '[');
				else	prints("\n,[");
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
				prints(P_SYM "}" P_RST);
			else	printc(      '}'      );
			break;
		case OM_JSON_NUMBERED:
		case OM_JSON_COMPACT:
			if (pretty)
				prints(P_SYM "]" P_RST);
			else	printc(      ']'      );
			break;
	}

	if (do_flush)
		fflush(stdout);
}

void output_kv (const char* key, const char* value) {
	switch (mode) {
		case OM_SIMPLE:
			PRETTY{ prints(P_KEY); }
			prints(key);
			PRETTY{ prints(P_RST); }
			prints(SIMPLE_KVSEP);
			nobr_print(value);
			printc('\n');
			break;

		case OM_JSON:
			if (! first_kv)
				prints(pretty ? (P_SYM "," P_RST) : ",");
			if (pretty)
				prints("\"" P_KEY);
			else	printc( '"'      );
			json_print(key);
			if (pretty)
				prints(P_RST "\":\"");
			else	prints(      "\":\"");
			json_print(value);
			printc('"');
			break;

		case OM_JSON_NUMBERED:
		case OM_JSON_COMPACT:;
			const bool is_colname = (mode == OM_JSON_COMPACT && !key);
			if (! first_kv) {
				if (pretty && !is_colname)
					prints(P_SYM "," P_RST);
				else	printc(      ','      );
			}
			if (pretty && is_colname)
				prints("\"" P_KEY);
			else	printc( '"'      );
			json_print(value);
			if (pretty && is_colname)
				prints(P_RST "\"");
			else	printc(       '"');
			break;
	}

	first_kv = false;
}


void json_print (const char* value) {
	for (register const unsigned char* c = (const unsigned char*)value; *c; c++)
	switch (*c) {
		case 0x08: printx("\\b"); break;
		case 0x09: printx("\\t"); break;
		case 0x0a: printx("\\n"); break;
		case 0x0c: printx("\\f"); break;
		case 0x0d: printx("\\r"); break;
		case '/':  printx("\\/"); break;
		case '\\': printx("\\\\"); break;
		case '"':  printx("\\\""); break;
		default:
			if (*c <= 31) {
				PRETTY{ prints(P_ESC); }
				printf("\\u%04X", (unsigned int)*c);
				PRETTY{ prints(P_RST); }
			} else if (c[0]==0xe2 && c[1]==0x80 && c[2]==0xa8) {
				// JS does not like these.
				// http://timelessrepo.com/json-isnt-a-javascript-subset
				printx("\\u2028");
				c += 2;
			} else if (c[0]==0xe2 && c[1]==0x80 && c[2]==0xa9) {
				printx("\\u2029");
				c += 2;
			} else
				printc(*c);
	}
}

void nobr_print (const char* value) {
	register const char* c = value;
	while (*c) {
		if (*c == '\\')
			// print an extra backslash
			printx("\\\\");
		else if (c[0] == '\r' && c[1] == '\n') {
			// collapse CRLF to single "\n"
			printx("\\n");
			c++;
		} else if (c[0] == '\r' || c[0] == '\n') {
			// output single linebreaks as "\n"
			printx("\\n");
		} else
			printc(*c);
		
		c++;
	}
}

inline void printx (const char* s) {
	PRETTY{ prints(P_ESC); }
	prints(s);
	PRETTY{ prints(P_RST); }
}

