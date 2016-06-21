#ifndef __CSV_H
#define __CSV_H

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "const.h"
#include "aux.h"
#include "str.h"


int   main (int argc, char** argv);
void  init (void);
void  parse (void);


extern FILE* in;
extern char sepchar;
extern bool ArbitrarySeparator;
extern bool AllowBreaks;

extern enum opmode {
	OP_PresetFields,
	OP_FixedFields,
	OP_NamedFields
} Opmode;
#define  OP(o)  (Opmode == OP_ ## o)

#define  IsSeparator_(c)  ( c==',' || c==';' || c==':' || c=='\0' )
#define  IsSeparator(c)  ( ArbitrarySeparator ? (c==sepchar || c=='\0') : IsSeparator_(c) )


#endif // __CSV_H
