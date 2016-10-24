#include "output.h"
#include "global.h"
#include "const.h"
#include "escape.h"
#include <stdio.h>
#include <stdbool.h>

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
	shvar_prefix = (_shvar_prefix) ? _shvar_prefix : "";

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
	case OM_SHELL_VARS_NUMBERED:
		// don't print column names!
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
		case OM_SHELL_VARS_NUMBERED:
			printf("%s%s" SHVAR_N_RECORDS "=%s%zu\n",
					pp_sym,
					shvar_prefix,
					pp_rst,
					records);
			break;
	}
}

void output_empty (void) {
	output_begin();
	output_end();
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
		case OM_SHELL_VARS_NUMBERED:
			break;
	}
	first_line = false;
	first_kv   = true;
}

void output_line_end (void) {
	switch (mode) {
		case OM_SIMPLE:
		case OM_SHELL_VARS:
		case OM_SHELL_VARS_NUMBERED:
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
			escape_nobr(value, pp_esc, (is_colname) ? pp_key : pp_rst);
			printc('\n');
			break;

		case OM_JSON:
			if (! first_kv)
				prints(pretty ? (PP_SYM "," PP_RST) : ",");
			if (pretty)
				prints("\"" PP_KEY);
			else	printc( '"'      );
			escape_json(key, pp_esc, pp_key);
			if (pretty)
				prints(PP_RST "\":\"");
			else	prints(       "\":\"");
			escape_json(value, pp_esc, pp_rst);
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
			escape_json(value, pp_esc, (is_colname) ? pp_key : pp_rst);
			if (pretty && is_colname)
				prints(PP_RST "\"");
			else	printc(        '"');
			break;

		case OM_SHELL_VARS:
		case OM_SHELL_VARS_NUMBERED:
			if (is_colname) {
				printf("%s%s" SHVAR_COLNAME "=",
						pp_sym,
						shvar_prefix,
						fields);
				escape_shvar(value, pp_esc, pp_key);
			} else {
				printf("%s%s" SHVAR_CELL "=%s",
						pp_sym,
						shvar_prefix,
						records,
						fields,
						pp_rst);
				escape_shvar(value, pp_esc, pp_rst);
			}
			printc('\n');
			break;
	}

	first_kv = false;
	fields++;
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


