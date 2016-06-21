#ifndef __DIE_H
#define __DIE_H

#include <stdarg.h>
#include <stdio.h>
#include "macros.h"
#include "compiler.h"


void die (char* errorFormat, ...) __ATTR_format(printf,1,2) __ATTR_noreturn;
void err (char* errorFormat, ...) __ATTR_format(printf,1,2);


#endif // __DIE_H
