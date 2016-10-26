#include "global.h"
#include "input.h"
#include "output.h"
#include "nstr.h"
#include "aux.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

// globals:
bool        Verbose      = false;
bool        IgnoreErrors = false;
const nstr* ColumnName   [MAXCOLUMNS] = {NULL};

typedef struct coldef {
	const char* name [MAXCOLALIASES];
	size_t names;
	bool found;
} coldef_t;

static void find_colnames (void);
static void read_colname_assignments (size_t args, const char** fieldname);
static size_t read_coldefs (size_t argc, const char** argv, struct coldef coldefs [], size_t max);
static void match_colnames (size_t argc, const char** argv);
static void autonumber_columns (void);
static void process_csv_input ();



int main (int argc, char** argv) {
	enum {
		MODE_AUTO_COLUMNS,
		MODE_NAMED_COLUMNS,
		MODE_ASSIGNED_NAMES,
		MODE_NUMBERED_COLUMNS,
	} mode = MODE_AUTO_COLUMNS;

	outmode_t outmode = OM_SIMPLE;

	colormode_t colormode = COLOR_AUTO;

	size_t skip_lines  = 0;
	size_t limit_lines = 0;
	char   separator   = '\0';
	bool   remove_bom  = true;
	bool   allow_breaks = false;
	bool   do_flush     = false;

	const char* options = "gnaihVmjJXd:bes:l:FM";
	const struct option long_options [] = {
		{ "assigned-names",	0, NULL, 'g' },
		{ "named-columns",	0, NULL, 'n' },
		{ "auto-columns",	0, NULL, 'a' },
		{ "numbered-columns",	0, NULL, 'i' },

		{ "help",		0, NULL, 'h' },
		{ "version",		0, NULL, 'V' },
		{ "color",		2, NULL,   1 },
		{ "colour",		2, NULL,   1 },
//		{ "verbose",		0, NULL, 'v' },

		{ "simple",		0, NULL, 'm' },
		{ "compact-json",	0, NULL, 'j' },
		{ "json",		0, NULL, 'J' },
		{ "shell-vars",		0, NULL, 'X' },

		{ "separator",		0, NULL, 'd' },
		{ "allow-breaks",	0, NULL, 'b' },
		{ "ignore-errors",	0, NULL, 'e' },
		{ "skip",		1, NULL, 's' },
		{ "limit",		1, NULL, 'l' },
		{ "flush",		0, NULL, 'F' },
		{ "keep-bom",		0, NULL, 'M' },

		{ 0, 0, 0, 0 }};
	signed char c;
	while ((c = getopt_long(argc, argv, options, long_options, NULL)) != -1)
	switch (c) {
		case 'a': mode = MODE_AUTO_COLUMNS;     break;
		case 'g': mode = MODE_ASSIGNED_NAMES;   break;
		case 'n': mode = MODE_NAMED_COLUMNS;    break;
		case 'i': mode = MODE_NUMBERED_COLUMNS; break;

		case 'h': Help();    return EXIT_HELP;
		case 'V': Version(); return EXIT_HELP;

		case 'd': sep_arg(&separator, "-d", optarg); break;
		case   1: color_arg(&colormode, "--color", optarg); break;

		case 'm': outmode = OM_SIMPLE; break;
		case 'j': outmode = OM_JSON; break;
		case 'J': outmode = OM_JSON_COMPACT; break;
		case 'X': outmode = OM_SHELL_VARS; break;

		case 'b': allow_breaks = true; break;
		case 'e': IgnoreErrors = true; break;
		case 'F': do_flush = true; break;
//		case 'v': Verbose = true; break;
		case 'M': remove_bom = false; break;

		case 's': int_arg(&skip_lines, "-s", optarg); break;
		case 'l': int_arg(&limit_lines, "-l", optarg); break;
	}

	if (mode == MODE_NUMBERED_COLUMNS) {
		if (outmode == OM_JSON_COMPACT)
			FAIL(EXIT_SYNTAX, "input mode -i is not compatible with output mode -J\n");
		else if (outmode == OM_SHELL_VARS)
			outmode = OM_SHELL_VARS_NUMBERED;
		else if (outmode == OM_JSON)
			outmode = OM_JSON_NUMBERED;
	}

	bool pretty_print = (colormode == COLOR_ON) || (colormode == COLOR_AUTO && isatty(fileno(stdout)));
	bool file_has_header = (mode == MODE_NAMED_COLUMNS || mode == MODE_AUTO_COLUMNS);
	bool skip_after_header = skip_lines && file_has_header;

	if (limit_lines && file_has_header)
		limit_lines += 1;

	set_input(stdin, separator, allow_breaks, remove_bom, skip_after_header, skip_lines, limit_lines);
	set_output(outmode, do_flush, pretty_print, "");

	if (! next_line()) {
		if (file_has_header) {
			FAIL(EXIT_EMPTY, "empty file, no header line found\n");
		} else {
			// Ok, this is MODE_ASSIGNED_NAMES or MODE_NUMBERED_COLUMNS,
			// it's okay if the input is empty.
			output_empty();
			return 0;
		}
	}

	if (mode == MODE_NAMED_COLUMNS) {
		/* Read actual column names from first line,
		 * compare with cmdline arguments and reword them.  */
		match_colnames(argc - optind, (const char**)(argv + optind));

	} else if (mode == MODE_ASSIGNED_NAMES) {
		/* Read column names from cmdline arguments.
		 * Columns to be omitted can be marked with OMIT_COLUMN.  */
		read_colname_assignments(argc - optind, (const char**)(argv + optind));

	} else if (mode == MODE_AUTO_COLUMNS) {
		/* Get column names from first line.  */
		find_colnames();

	} else if (mode == MODE_NUMBERED_COLUMNS) {
		/* Auto-numbered columns, no header line.  */
		autonumber_columns();
	}

	if (file_has_header) {
		if (! next_line()) {
			VERBOSE("no content lines\n");
			output_empty();
			return 0;
		}
	}

	process_csv_input();
}


void process_csv_input () {
	output_begin();

	do {
		output_line_begin();

		size_t col = 0;
		const nstr* field;
		while ((field = next_field())) {
			if (ColumnName[col])
				output_kv(ColumnName[col], field);
			col++;
		}

		output_line_end();

	} while (next_line());

	output_end();
}


// reading column names:

void match_colnames (size_t argc, const char** argv) {
	coldef_t coldefs [MAXCOLDEFS];
	size_t n_coldefs = read_coldefs(argc, argv, coldefs, MAXCOLDEFS);
	if (! n_coldefs)
		FAIL(EXIT_SYNTAX, "no column names\n");

	size_t found = 0;
	size_t c = 0;
	const nstr* s;
	while ((s = next_field())) {
		if (c >= MAXCOLUMNS) {
			ERR(EXIT_FORMAT, "too many columns in header line\n");
			break;
		}

		// got one actual input colname, now compare it against all known colname aliases
		for (size_t cd = 0; cd < n_coldefs; cd++) {
			#define has_name(ni) (ni < coldefs[cd].names)
			#define name_equal(ni, s) (nstr_cmpsz(s, coldefs[cd].name[ni]))
			for (size_t ni = 0; has_name(ni); ni++) {
				if (!coldefs[cd].found && name_equal(ni, s)) {
					// this coldef had the name!
					// assign the coldef's basename:
					ColumnName[c] = nstr_fromsz(coldefs[cd].name[0]);
					coldefs[cd].found = true;
					VERBOSE("found column \"%s\" (%zu)\n", ColumnName[c]->buffer, c);
					found++;
					goto next_col;
				}
			}
			#undef name_equal
			#undef has_name
		}

		VERBOSE("unknown column \"%s\" (%zu)\n", s->buffer, c);

		next_col:
		c++;
	}

	if (found <= 0)
		FAIL(EXIT_FORMAT, "could not match any column names\n");
}

size_t read_coldefs (size_t argc, const char** argv, struct coldef coldefs [], size_t max) {
	if (argc > max) {
		FAIL(EXIT_SYNTAX, "too many column definitions\n");
	}

	const char* basename = NULL;
	size_t n = 0;

	for (size_t a = 0; a < argc; a++) {
		#define arg argv[a]
		if (streq(arg, NAME_SEPARATOR)) {
			// this is a separator argument, starting the next basename.
			if (basename) {
				basename = NULL;
				n++;
			}
			continue;
		}

		if (! basename) {
			// this is a new basename argument, initialize new coldef_t struct:
			basename = arg;
			coldefs[n].name[0] = basename;
			coldefs[n].names = 1;
			coldefs[n].found = false;
		} else {
			// this is an alias argument for the last basename
			const size_t ai = coldefs[n].names++;
			if (ai >= MAXCOLALIASES)
				FAIL(EXIT_SYNTAX, "too many aliases for column name \"%s\"\n", coldefs[n].name[0]);
			coldefs[n].name[ ai ] = arg;
		}
		#undef arg
	}

	if (basename) {
		// last NAME_SEPARATOR argument missing, we'll accept it
		n++;
		if (n == 1)
			fprintf(stderr,
				PROGNAME": warning: "
				"no \"%s\" separator arguments, using only one column name\n",
				NAME_SEPARATOR);
	}

	return n;
}

void find_colnames (void) {
	size_t c = 0;
	const nstr* s;
	while ((s = next_field())) {
		if (c >= MAXCOLUMNS) {
			ERR(EXIT_FORMAT, "too many columns\n");
			break;
		}

		ColumnName[c] = nstr_dup(s);

		VERBOSE("found column name %zu: \"%s\"\n", c, s->buffer);

		c++;
	}

	if (c <= 0) {
		// This should never happen -- next_field() should at least return a single empty string.
		FAIL(EXIT_INTERNAL, "first line contains no column names\n");
	}
}

void read_colname_assignments (size_t args, const char** argv) {
	if (args <= 0)
		FAIL(EXIT_SYNTAX, "no column names\n");

	for (size_t c = 0; c < args; c++) {
		if (streq(argv[c], OMIT_COLUMN)) {
			// continue
		} else {
			ColumnName[c] = nstr_fromsz(argv[c]);
			VERBOSE("got column name \"%s\" (%zu)\n", ColumnName[c]->buffer, c);
		}
	}
}

void autonumber_columns (void) {
	/* This function fills the ColumnName[] array with the string representation
	 * of column indexes.
	 *
	 * Because we don't know yet how many columns the longest input line has,
	 * we'll fill up the entire array (indexes 0 .. MAXCOLUMNS-1).
	 *
	 * Instead of calling strdup() hundreds of times, we use one big buffer
	 * and build nstr structures in there.
	 * The buffer length is the max column count
	 * multiplied with the memory size of the longest possible field index.
	 * A lot of that buffer will be wasted because of the shorter numbers
	 * and due to struct padding, but who cares.  */

	#define str_(s) #s
	#define str(s) str_(s)

	// length of longest possible field index,
	// including terminating NUL byte:
//	const unsigned int nlen = 1 + ceil(log10(MAXCOLUMNS - 1));
	const size_t nlen = sizeof(str(MAXCOLUMNS));

	// memory size of longest possible field name,
	// including terminating NUL byte:
	const size_t nslen = nlen + sizeof(nstr);

	char* buf = Malloc(nslen * MAXCOLUMNS);

	for (size_t c = 0; c < MAXCOLUMNS; c++) {
		nstr* s = (nstr*)(buf + (nslen * c));
		s->length = snprintf(s->buffer, nlen, "%zu", c);
		ColumnName[c] = s;
	}
}

