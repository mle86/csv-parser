#ifndef __CSV_H
#define __CSV_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define VERSION "1.0"

#define MAXLINELEN 4096
#define MAXFIELDLEN 90
#define MAXFIELDS 400

#define M1 "[1m"
#define M0 "[0m"

int   main (int argc, char** argv);
char* getLine (short needLine);
void  skipWhitespace (char** s);
void  chopWhitespace (char* s);
char* nextToken (char** s);
void  init (void);
void  Help (void);
void  Version (void);
short strIEqual (char* a, char* b);

extern FILE* in;

#include "macros.h"
#include "malloc.h"
#include "die.h"

#include "malloc.c"
#include "die.c"
#include "str.c"

#endif // __CSV_H
