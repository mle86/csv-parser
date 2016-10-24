#ifndef _ESCAPE_H
#define _ESCAPE_H
#include "nstr.h"


// Takes a raw input nstr, escapes it and prints it to stdout.
typedef void(escape_fn_t)(const nstr* raw, const char* pp_esc, const char* pp_rst);


escape_fn_t escape_nobr;
escape_fn_t escape_json;
escape_fn_t escape_shvar;


#endif  //  _ESCAPE_H
