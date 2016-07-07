#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdbool.h>


typedef enum outmode {
	OM_SIMPLE,
	OM_JSON,
	OM_JSON_NUMBERED,
	OM_JSON_COMPACT,
} outmode_t;


void set_output (outmode_t _mode, bool do_flush);

void output_begin (void);
void output_end   (void);
void output_line_begin (void);
void output_line_end   (void);

void output_kv (const char* key, const char* value);


#endif  // OUTPUT_H
