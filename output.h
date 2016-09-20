#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdbool.h>
#include "const.h"


#define SIMPLE_LINESEP "-\n"
#define SIMPLE_KVSEP ": "

// pretty-printing colors:
#define PP_KEY Mgreen
#define PP_SYM Myellow
#define PP_ESC Mcyan
#define PP_RST M0

// variable name templates:
#define SHVAR_COLNAME	"CSV_COLNAME_%zu"
#define SHVAR_CELL	"CSV_%zu_%zu"
#define SHVAR_N_RECORDS	"CSV_RECORDS"

typedef enum outmode {
	OM_SIMPLE,
	OM_JSON,
	OM_JSON_NUMBERED,
	OM_JSON_COMPACT,
	OM_SHELL_VARS,
} outmode_t;


void set_output (outmode_t mode, bool do_flush, bool pretty, const char* shvar_prefix);

void output_begin (void);
void output_end   (void);
void output_empty (void);
void output_line_begin (void);
void output_line_end   (void);

void output_kv (const char* key, const char* value);


#endif  // OUTPUT_H
