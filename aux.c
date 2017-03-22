#include "aux.h"
#include "const.h"
#include "input.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>

static void invalid_arg (const char* option, const char* value);


void Help (void) { printf(
	"Reads a CSV file from standard input and outputs a cleaner format.\n"
	"Usage: "M1 PROGNAME M0" ["M1"MODE"M0"] ["M1"OPTIONS"M0"] [--] ["Mu "COLUMNNAME"M0"...]\n"
	"Input modes:\n"
	" "M1"-n"M0", "M1"--named-columns"M0"  "
	"Non-fixed, named columns:\n"
	"            The program reads column names from the argument list\n"
	"            and compares them to the first line of the CSV input,\n"
	"            printing only the named columns.\n"
	"            Separate column names with a single \""M1 NAME_SEPARATOR M0"\" argument!\n"
	"            The name order is irrelevant, as is case.\n"
	" "M1"-g"M0", "M1"--assigned-names"M0"  "
	"Names assigned to column positions:\n"
	"            The program reads column names from the argument list and\n"
	"            applies them to all lines.  The special name \""M1 OMIT_COLUMN M0"\"\n"
	"            causes columns to be ignored.  The argument order represents\n"
	"            the column order in the CSV input.\n"
	" "M1"-a"M0", "M1"--auto-columns"M0"  "
	"Automatic column names:\n"
	"            Column names will be read automatically from the first CSV line.\n"
	"            In this mode, "Mu "COLUMNNAME"M0" specifications are\n"
	"            neither needed nor possible.\n"
	"            (This is the default mode.)\n"
	" "M1"-i"M0", "M1"--numbered-columns"M0"  "
	"Numbered columns:\n"
	"            In this mode, the columns will be automatically numbered,\n"
	"            starting from zero.  Similar to \""M1"-g 0 1 2 3"M0"...\",\n"
	"            but will force the "M1"-j"M0" output mode to print each line\n"
	"            as an array instead of an object.  Not compatible with "M1"-J"M0" mode.\n"
	"            Causes the "M1"-X"M0" mode not to output any header names.\n"
	"\n"
	"Options:\n"
	"  "M1"-m"M0", "M1"--simple"M0"  "
	"Output in simple format. (This is the default format.)\n"
	"  "M1"-j"M0", "M1"--json"M0"    "
	"Output in JSON format.\n"
	"  "M1"-J"M0", "M1"--compact-json"M0"  "
	                      "Output in compact JSON format\n"
	"                      (an object with 'columns' and 'lines' keys).\n"
	"  "M1"-X"M0", "M1"--shell-vars"M0"  "
	                      "Output in shell variable assignment format.\n"
	"  "M1"-d"M0", "M1"--separator"M0" "Mu "C"M0"  "
	                     "Use field separator character "Mu "C"M0".\n"
	"                     The special value \""M1"auto"M0"\" causes auto-detection\n"
	"                     on the first input line (this is the default mode).\n"
	"  "M1"-b"M0", "M1"--allow-breaks"M0"  "
	                      "Allow linebreaks within doublequoted fields.\n"
	"  "M1"-s"M0", "M1"--skip "M0 Mu "N"M0"  "
	                "Skip the first "Mu "N"M0" input lines.\n"
	"                In modes "M1"-n"M0" and "M1"-a"M0", skip the first "Mu "N"M0" input lines\n"
	"                after the header line.\n"
	"                Setting "Mu "N"M0" to zero means no skip (this is the default).\n"
	"  "M1"-l"M0", "M1"--limit "M0 Mu "N"M0"  "
	                 "Stop after the "Mu "N"M0"th input line.\n"
	"                 In modes "M1"-n"M0" and "M1"-a"M0", the header line is not counted.\n"
	"                 Setting "Mu "N"M0" to zero removes the limit (this is the default).\n"
	"\n"
	"  "M1"-e"M0", "M1"--ignore-errors"M0"  "
	                  "Don't stop on encountering malformed CSV input.\n"
	"  "M1"--color"M0"[="Mu "WHEN"M0"]  "
	                   "Colorize output: "M1"always"M0" (default if "Mu "WHEN"M0" is missing),\n"
	"                  "M1"auto"M0" (default), or "M1"never"M0".\n"
	"  "M1"--trim"M0"[="Mu "MODE"M0"]   "
	                   "Trim whitespace from input:\n"
	"                  "M1"lines"M0" (default if "Mu "MODE"M0" is missing) or "M1"none"M0" (default).\n"
	"  "M1"-F"M0", "M1"--flush"M0"     "
	                   "Flush the output after every input line.\n"
	"  "M1"-M"M0", "M1"--keep-bom"M0"  "
	                  "Don't try to remove UTF-8/16 BOMs from the first line.\n"
	"  "M1"-u"M0", "M1"--unknowns"M0"  "
	                  "Also output unknown columns in "M1"-n"M0" mode.\n"
	"  "M1"-h"M0", "M1"--help"M0"      "
	                  "This help\n"
	"  "M1"-V"M0", "M1"--version"M0"   "
	                  "Program version\n"
	"\n"
); }

void Version (void) { printf(
	PROGNAME " v" VERSION "\n"
	"Written by Maximilian Eul <maximilian@eul.cc>, " VERDATE ".\n"
	"MIT License <https://opensource.org/licenses/MIT>.\n"
	"\n"
); }


inline void* Malloc (size_t size) {
	void* ptr = malloc(size);
	if (ptr == NULL && size > 0) {
		FAIL(EXIT_INTERNAL, "out of memory\n");
	}
	return ptr;
}

inline void* Realloc (void* origptr, size_t newsize) {
	void* ptr = realloc(origptr, newsize);
	if (ptr == NULL && newsize > 0) {
		FAIL(EXIT_INTERNAL, "out of memory\n");
	}
	return ptr;
}


// cmdline argument handling:

void invalid_arg (const char* option, const char* value) {
	FAIL(EXIT_SYNTAX, "invalid argument for option %s: %s\n", option, value);
}

void chr_arg (char *var, const char* option, const char* value) {
	if (value && value[0] && !value[1])
		// single character
		*var = value[0];
	else if (value && value[0] && value[1] == value[0] && !value[2])
		// single character, repeated once
		*var = value[0];
	else invalid_arg(option, value);
}

void int_arg (size_t *var, const char* option, const char* value) {
	char* endptr = NULL;
	long long int out = strtoll(value, &endptr, 10);

	if ((*value && !*endptr) && out >= 0)
		*var = out;
	else invalid_arg(option, value);
}

void sep_arg (char *separator, const char* option, const char* value) {
	  if (streq(value, "auto"))
		  *separator = SEP_AUTO;
	  else if (streq(value, "none"))
		  *separator = SEP_NONE;
	  else
		  chr_arg(separator, option, value);
}

void color_arg (colormode_t *mode, const char* option, const char* value) {
	  if (value == NULL || streq(value, "on") || streq(value, "force") || streq(value, "always"))
		  *mode = COLOR_ON;
	  else if (streq(value, "off") || streq(value, "never"))
		  *mode = COLOR_OFF;
	  else if (streq(value, "auto"))
		  *mode = COLOR_AUTO;
	  else invalid_arg(option, value);
}

void trim_arg (trimmode_t *mode, const char* option, const char* value) {
	if (value == NULL || streq(value, "lines") || streq(value, "line") || streq(value, "records") || streq(value, "record"))
		*mode = TRIM_LINES;
	else if (streq(value, "none") || streq(value, "no") || streq(value, "off"))
		*mode = TRIM_NONE;
	else invalid_arg(option, value);
}

