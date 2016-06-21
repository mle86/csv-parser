#ifndef __OUTPUT_H
#define __OUTPUT_H

#include <stdio.h>
#include "csv.h"


void outputKV (char* key, char* value, bool first_kv_of_record, bool first_line);
void outputRecordEnd ();

void outputBegin ();
void outputEnd   ();


#endif // __OUTPUT_H
