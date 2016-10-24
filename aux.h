#ifndef AUX_H
#define AUX_H
#include <stdlib.h>


void  Help    (void);
void  Version (void);

void* Malloc  (size_t size);
void* Realloc (void* origptr, size_t newsize);


#endif // AUX_H
