#include "output.h"
#include "global.h"
#include "const.h"
#include "escape.h"
#include "nstr.h"
#include <stdio.h>
#include <stdbool.h>

static outmode_t mode = OM_SIMPLE;
static bool do_flush = false;
static bool pretty = false;
static const char* shvar_prefix = "";

static void reformat_all_colnames (void);

static const char
	*pp_key, *pp_sym, *pp_esc, *pp_rst,
	*pp0_key, *pp0_sym, *pp0_esc, *pp0_rst;

static bool first_line = true;
static bool first_kv   = true;

static size_t records = 0;
static size_t fields = 0;

#define PRETTY if(pretty)

#define prints(s) fputs(s, stdout)
#define printc(c) putchar(c)

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
		pp_sym  = PP_SYM;
		pp_esc  = PP_ESC;
		pp_key  = PP_KEY;
		pp_rst  = PP_RST;
		pp0_sym = PP_SYM;
		pp0_esc = PP_ESC;
		pp0_key = PP_KEY;
		pp0_rst = PP_RST;
	} else {
		pp_sym  = "";
		pp_esc  = "";
		pp_key  = "";
		pp_rst  = "";
		pp0_sym = NULL;
		pp0_esc = NULL;
		pp0_key = NULL;
		pp0_rst = NULL;
	}
}

void output_begin (void) {
  reformat_all_colnames();
  switch (mode) {
	case OM_SIMPLE:
		break;
	case OM_JSON:
	case OM_JSON_NUMBERED:
		printc('[');
		PRETTY{ prints(PP_SYM); }
		break;
	case OM_JSON_COMPACT:
		prints("{\"columns\": [");
		for (size_t c = 0; c < MAXCOLUMNS; c++) {
			if (ColumnName[c])
				output_kv(NULL, ColumnName[c]);
		}
		prints("],\n\"lines\": [\n ");
		PRETTY{ prints(PP_SYM); }
		first_kv = true;  // output_kv() has cleared it
		break;
	case OM_SHELL_VARS:
		for (size_t c = 0; c < MAXCOLUMNS; c++) {
			if (ColumnName[c])
				output_kv(NULL, ColumnName[c]);
		}
		break;
	case OM_CSV:
		for (size_t c = 0; c < MAXCOLUMNS; c++) {
			if (ColumnName[c])
				output_kv(NULL, ColumnName[c]);
		}
		prints(CSV_LINESEP);
		first_kv = true;  // output_kv() has cleared it
		break;
	case OM_SHELL_VARS_NUMBERED:
		// don't print column names!
		break;
} }

void output_end (void) {
	prints(pp_rst);

	switch (mode) {
		case OM_SIMPLE:
		case OM_CSV:
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
			printf("%s%s{%s",
					(first_line) ? "" : "\n,",
					pp_sym, pp_rst);
			break;
		case OM_JSON_COMPACT:
		case OM_JSON_NUMBERED:
			printf("%s%s[%s",
					(first_line) ? "" : "\n,",
					pp_sym, pp_rst);
			break;
		case OM_SHELL_VARS:
		case OM_SHELL_VARS_NUMBERED:
		case OM_CSV:
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
			printf("%s}%s", pp_sym, pp_rst);
			break;
		case OM_JSON_NUMBERED:
		case OM_JSON_COMPACT:
			printf("%s]%s", pp_sym, pp_rst);
			break;
		case OM_CSV:
			prints(CSV_LINESEP);
			break;
	}

	records++;

	if (do_flush)
		fflush(stdout);
}

void output_kv (const nstr* key, const nstr* value) {
	const bool is_colname = (!key && (mode == OM_JSON_COMPACT || mode == OM_SHELL_VARS || mode == OM_CSV));

	switch (mode) {
		case OM_SIMPLE:
			printk(key->buffer);
			prints(SIMPLE_KVSEP);
			escape_nobr(value, pp0_esc, pp0_rst);
			printc('\n');
			break;

		case OM_JSON:
			if (likely(!first_kv))
				prints(pretty ? (PP_SYM "," PP_RST) : ",");
			printc('"');
			printk(key->buffer);
			prints("\":\"");
			escape_json(value, pp0_esc, pp0_rst);
			printc('"');
			break;

		case OM_JSON_NUMBERED:
		case OM_JSON_COMPACT:;
			if (likely(!first_kv)) {
				if (pretty && !is_colname)
					prints(PP_SYM "," PP_RST);
				else	printc(       ','       );
			}
			printc('"');
			if (is_colname) {
				printk(value->buffer);
			} else {
				escape_json(value, pp0_esc, pp0_rst);
			}
			printc('"');
			break;

		case OM_SHELL_VARS:
		case OM_SHELL_VARS_NUMBERED:
			if (unlikely(is_colname)) {
				printf("%s%s" SHVAR_COLNAME "=%s%s%s\n",
						pp_sym,
						shvar_prefix,
						fields,
						pp_key,
						value->buffer,
						pp_rst);
			} else {
				printf("%s%s" SHVAR_CELL "=%s",
						pp_sym,
						shvar_prefix,
						records,
						fields,
						pp_rst);
				escape_shvar(value, pp0_esc, pp0_rst);
				printc('\n');
			}
			break;

		case OM_CSV:
			if (likely(!first_kv)) {
				printf("%s%c%s",
					pp_sym,
					CSV_FIELDSEP,
					pp_rst);
			}
			if (unlikely(is_colname)) {
				printk(value->buffer);
			} else {
				escape_csv(value, pp0_esc, pp0_rst);
			}
			break;
	}

	first_kv = false;
	fields++;
}


inline void printk (const char* s) {
	PRETTY{ prints(PP_KEY); }
	prints(s);
	PRETTY{ prints(PP_RST); }
}


/**
 * Column names are read only once,
 * but will be printed often (at least in output modes -m and -j).
 * We can gain performance by reformatting them only once
 * instead of doing it every time they get printed.
 * All outmodes escape the NUL character.
 * Therefore, the result is a regular NUL-terminated string.
 * This function does NOT prepend pp_key or append pp_rst.
 */
inline void reformat_all_colnames () {
	for (size_t c = 0; c < MAXCOLUMNS; c++) {
		if (ColumnName[c]) {
			const nstr* orig = ColumnName[c];
			switch (mode) {
				case OM_SIMPLE:
					ColumnName[c] = reformat_nobr(orig, pp_esc, pp_key);
					break;
				case OM_JSON:
				case OM_JSON_NUMBERED:
				case OM_JSON_COMPACT:
					ColumnName[c] = reformat_json(orig, pp_esc, pp_key);
					break;
				case OM_SHELL_VARS:
				case OM_SHELL_VARS_NUMBERED:
					ColumnName[c] = reformat_shvar(orig, pp_esc, pp_key);
					break;
				case OM_CSV:
					ColumnName[c] = reformat_csv(orig, pp_esc, pp_key);
					break;
			}
		}
	}
}

