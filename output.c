#include "output.h"
#include "global.h"
#include "const.h"
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

static outmode_t mode = OM_SIMPLE;
static bool do_flush = false;
static bool pretty = false;
static const char* shvar_prefix = "";

static const char
	*pp_key, *pp_sym, *pp_esc, *pp_rst;

static bool first_line = true;
static bool first_kv   = true;

static size_t records = 0;
static size_t fields = 0;

static void json_print  (const char* value, bool is_key);
static void nobr_print  (const char* value, bool is_key);
static void shvar_print (const char* value, bool is_key);

#define PRETTY if(pretty)

#define prints(s) fputs(s, stdout)
#define printc(c) fputc(c, stdout)

// Like prints(), but will pretty-print using PP_ESC.
static void printx (const char* s, bool in_key);
// Like prints(), but will pretty-print using PP_KEY.
static void printk (const char* s);


inline void set_output (outmode_t _mode, bool _do_flush, bool _pretty, const char* _shvar_prefix) {
	first_line = true;
	first_kv   = true;
	records    = 0;
	fields     = 0;
	mode       = _mode;
	do_flush   = _do_flush;
	shvar_prefix = (_shvar_prefix) ? shvar_prefix : "";

	pretty = _pretty;
	if (pretty) {
		pp_sym = PP_SYM;
		pp_key = PP_KEY;
		pp_esc = PP_ESC;
		pp_rst = PP_RST;
	} else {
		pp_sym = "";
		pp_key = "";
		pp_esc = "";
		pp_rst = "";
	}
}


void output_begin (void) {
  switch (mode) {
	case OM_SIMPLE:
		break;
	case OM_JSON:
	case OM_JSON_NUMBERED:
		printc('[');
		prints(pp_sym);
		break;
	case OM_JSON_COMPACT:
		prints("{\"columns\": [");
		for (size_t c = 0; c < MAXCOLUMNS; c++) {
			if (ColumnName[c])
				output_kv(NULL, ColumnName[c]);
		}
		prints("],\n\"lines\": [\n ");
		prints(pp_sym);
		first_kv = true;  // output_kv() has cleared it
		break;
	case OM_SHELL_VARS:
		for (size_t c = 0; c < MAXCOLUMNS; c++) {
			if (ColumnName[c])
				output_kv(NULL, ColumnName[c]);
		}
		break;
} }

void output_end (void) {
	prints(pp_rst);

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
		case OM_SHELL_VARS:
			printf("%s%s" SHVAR_N_RECORDS "=%s%zu\n",
					pp_sym,
					shvar_prefix,
					pp_rst,
					records);
			break;
	}
}

void output_line_begin (void) {
	fields = 0;
	switch (mode) {
		case OM_SIMPLE:
			if (! first_line) {
				if (pretty)
					prints(PP_SYM SIMPLE_LINESEP PP_RST);
				else	prints(       SIMPLE_LINESEP       );
			}
			break;
		case OM_JSON:
			if (!first_line)
				prints("\n,");
			if (pretty)
				prints(PP_SYM "{" PP_RST);
			else	printc(       '{'       );
			break;
		case OM_JSON_COMPACT:
		case OM_JSON_NUMBERED:
			if (pretty) {
				if (first_line)
					prints(      PP_SYM "[" PP_RST);
				else	prints("\n," PP_SYM "[" PP_RST);
			} else {
				if (first_line)
					printc(   '[');
				else	prints("\n,[");
			}
			break;
		case OM_SHELL_VARS:
			break;
	}
	first_line = false;
	first_kv   = true;
}

void output_line_end (void) {
	switch (mode) {
		case OM_SIMPLE:
		case OM_SHELL_VARS:
			break;
		case OM_JSON:
			if (pretty)
				prints(PP_SYM "}" PP_RST);
			else	printc(       '}'       );
			break;
		case OM_JSON_NUMBERED:
		case OM_JSON_COMPACT:
			if (pretty)
				prints(PP_SYM "]" PP_RST);
			else	printc(       ']'       );
			break;
	}

	records++;

	if (do_flush)
		fflush(stdout);
}

void output_kv (const char* key, const char* value) {
	const bool is_colname = (!key && (mode == OM_JSON_COMPACT || mode == OM_SHELL_VARS));
	switch (mode) {
		case OM_SIMPLE:
			printk(key),
			prints(SIMPLE_KVSEP);
			nobr_print(value, is_colname);
			printc('\n');
			break;

		case OM_JSON:
			if (! first_kv)
				prints(pretty ? (PP_SYM "," PP_RST) : ",");
			if (pretty)
				prints("\"" PP_KEY);
			else	printc( '"'      );
			json_print(key, true);
			if (pretty)
				prints(PP_RST "\":\"");
			else	prints(       "\":\"");
			json_print(value, false);
			printc('"');
			break;

		case OM_JSON_NUMBERED:
		case OM_JSON_COMPACT:;
			if (! first_kv) {
				if (pretty && !is_colname)
					prints(PP_SYM "," PP_RST);
				else	printc(       ','       );
			}
			if (pretty && is_colname)
				prints("\"" PP_KEY);
			else	printc( '"'       );
			json_print(value, is_colname);
			if (pretty && is_colname)
				prints(PP_RST "\"");
			else	printc(        '"');
			break;

		case OM_SHELL_VARS:
			if (is_colname) {
				printf("%s%s" SHVAR_COLNAME "=",
						pp_sym,
						shvar_prefix,
						fields);
				shvar_print(value, true);
			} else {
				printf("%s%s" SHVAR_CELL "=%s",
						pp_sym,
						shvar_prefix,
						records,
						fields,
						pp_rst);
				shvar_print(value, false);
			}
			printc('\n');
			break;
	}

	first_kv = false;
	fields++;
}


void json_print (const char* value, bool is_key) {
	for (register const unsigned char* c = (const unsigned char*)value; *c; c++)
	switch (*c) {
		case 0x08: printx("\\b",  is_key); break;
		case 0x09: printx("\\t",  is_key); break;
		case 0x0a: printx("\\n",  is_key); break;
		case 0x0c: printx("\\f",  is_key); break;
		case 0x0d: printx("\\r",  is_key); break;
		case '/':  printx("\\/",  is_key); break;  // http://stackoverflow.com/questions/1580647/json-why-are-forward-slashes-escaped
		case '\\': printx("\\\\", is_key); break;
		case '"':  printx("\\\"", is_key); break;
		default:
			if (*c <= 31) {
				// control character
				char buf [sizeof "\\uFFFF"];
				snprintf(buf, sizeof(buf), "\\u%04X", (unsigned int)*c);
				printx(buf, is_key);
			} else if (c[0]==0xe2 && c[1]==0x80 && c[2]==0xa8) {
				// JS does not like these.
				// http://timelessrepo.com/json-isnt-a-javascript-subset
				printx("\\u2028", is_key);
				c += 2;
			} else if (c[0]==0xe2 && c[1]==0x80 && c[2]==0xa9) {
				printx("\\u2029", is_key);
				c += 2;
			} else
				printc(*c);
	}
}

void nobr_print (const char* value, bool is_key) {
	register const char* c = value;
	while (*c) {
		if (*c == '\\')
			// print an extra backslash
			printx("\\\\", is_key);
		else if (c[0] == '\r' && c[1] == '\n') {
			// collapse CRLF to single "\n"
			printx("\\n", is_key);
			c++;
		} else if (c[0] == '\r' || c[0] == '\n') {
			// output single linebreaks as "\n"
			printx("\\n", is_key);
		} else
			printc(*c);
		
		c++;
	}
}

void shvar_print (const char* value, bool is_key) {
	// If a value consists of safe chars only, it does not need to be quoted or escaped:
	#define is_safe_chr(c) (									\
			isalnum(c) ||									\
			c == '_' || c == '-' || c == '+' || c == ',' || c == '.' || c == ':' ||		\
			c == '@' || c == '/' || c == '[' || c == ']' || c == '{' || c == '}' )

	bool is_safe = true;
	for (register const char* c = value; *c; c++)
		if (!is_safe_chr(*c)) {
			is_safe = false;
			break;
		}

	if (is_safe) {
		// no escaping and quoting necessary, just print the value itself
		if (pretty && is_key)
			prints(PP_KEY);
		prints(value);
		if (pretty && is_key)
			prints(PP_RST);
		return;
	}
	
	// Characters which MUST be backslash-escaped in a quoted string:
	#define is_quotable_chr(c) (c == '"' || c == '`' || c == '$' || c == '\\')

	PRETTY{ prints(PP_SYM); }
	printc('"');
	PRETTY{ prints((is_key) ? PP_KEY : PP_RST); }
	
	char buf [sizeof "\\x"];
	buf[0] = '\\';
	buf[1] = '\0';  // changed in loop
	buf[2] = '\0';

	for (register const char* c = value; *c; c++) {
		if (is_quotable_chr(*c)) {
			buf[1] = *c;
			printx(buf, is_key);
		} else {
			printc(*c);
		}
	}

	PRETTY{ prints(PP_SYM); }
	printc('"');
	PRETTY{ prints(PP_RST); }
}

inline void printx (const char* s, bool in_key_name) {
	PRETTY{ prints(PP_ESC); }
	prints(s);
	PRETTY{ prints((in_key_name) ? PP_KEY : PP_RST); }
}
inline void printk (const char* s) {
	PRETTY{ prints(PP_KEY); }
	prints(s);
	PRETTY{ prints(PP_RST); }
}


