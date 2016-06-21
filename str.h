#ifndef __STR_H
#define __STR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "csv.h"


extern size_t       linelen;
extern unsigned int linenum;


char* getLine (bool needLine);
char* appendLine (char* oldline);

char* nextToken (char* *s);

off_t skipWhitespace (const char* s);
void  chopWhitespace (char* s, size_t *saveLen);
char* breaksToBlank  (char* s);
void outputJsonString (char* s);


#endif // __STR_H
