#ifndef AUX_H
#define AUX_H

/**
 * Miscellaneous helper functions.
 *
 * This file is part of the 'csv-parser' project
 * (see https://github.com/mle86/csv-parser).
 *
 * SPDX-License-Identifier: MIT
 * Copyright © 2017-2019 Maximilian Eul
 */

#include <stdlib.h>
#include "global.h"
#include "input.h"


void  Help    (void);
void  Version (void);

void* Malloc  (size_t size);
void* Realloc (void* origptr, size_t newsize);

void sep_arg    (char *separator,    const char* option, const char* value);
void enc_arg    (char *enclosure,    const char* option, const char* value);
void int_arg    (size_t *var,        const char* option, const char* value);
void chr_arg    (char *var,          const char* option, const char* value);
void color_arg  (colormode_t *mode,  const char* option, const char* value);
void trim_arg   (trimmode_t *mode,   const char* option, const char* value);
void filter_arg (filtermode_t *mode, const char* option, const char* value);

#endif // AUX_H
