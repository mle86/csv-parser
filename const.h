#ifndef CONST_H
#define CONST_H

/**
 * Common constants.
 *
 * This file is part of the 'csv-parser' project
 * (see https://github.com/mle86/csv-parser).
 *
 * SPDX-License-Identifier: MIT
 * Copyright © 2017-2019 Maximilian Eul
 */


#define VERSION "2.5.2"
#define VERDATE "February 2019"

#define EXIT_HELP	0
#define EXIT_SYNTAX	1
#define EXIT_FORMAT	2
#define EXIT_NOSEP	3
#define EXIT_EMPTY	4
#define EXIT_INTERNAL	10

#define NAME_SEPARATOR	"."
#define OMIT_COLUMN	"@"

#ifndef PROGNAME
  #define PROGNAME "csv"
#endif

#define MAXCOLUMNS	  512
#define MAXCOLDEFS	 1024
#define MAXCOLALIASES	   40


#define M1	"[1m"  /* highlighted */
#define Mu	"[4m"  /* underlined */
#define M0	"[0m"  /* normal */
#define Mcyan	"[36m"
#define Mpurple	"[35m"
#define Myellow	"[33m"
#define Mgreen	"[32m"


#endif  // CONST_H
