#ifndef ESCAPE_H
#define ESCAPE_H

/**
 * String escaping functions.
 *
 * This file is part of the 'csv-parser' project
 * (see https://github.com/mle86/csv-parser).
 *
 * SPDX-License-Identifier: MIT
 * Copyright © 2017-2019 Maximilian Eul
 */

#include "nstr.h"


// Takes a raw input nstr, escapes it and prints it to stdout.
typedef void(escape_fn_t)(const nstr* raw, const char* pp_esc, const char* pp_rst);

// Takes a raw input nstr, escapes it and stores the result in a new nstr.
typedef nstr*(reformat_fn_t)(const nstr* raw, const char* pp_esc, const char* pp_rst);


escape_fn_t escape_nobr;
escape_fn_t escape_json;
escape_fn_t escape_shvar;
escape_fn_t escape_csv;

reformat_fn_t reformat_nobr;
reformat_fn_t reformat_json;
reformat_fn_t reformat_shvar;
reformat_fn_t reformat_csv;


#endif  //  ESCAPE_H
