#ifndef CHARS_H
#define CHARS_H

/**
 * Character class test functions.
 *
 * This file is part of the 'csv-parser' project
 * (see https://github.com/mle86/csv-parser).
 *
 * SPDX-License-Identifier: MIT
 * Copyright © 2017-2019 Maximilian Eul
 */


/** recognized separator characters for "--separator auto" */
#define issep(c) (c == ',' || c == ';' || c == '\t' || c == '|')

/** recognized quoting characters */
#define isq(c) (c == '"' || c == '\'')


#endif  // CHARS_H
