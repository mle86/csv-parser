#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdbool.h>
#include "const.h"
#include "nstr.h"


extern bool Verbose;
extern bool IgnoreErrors;
extern const nstr* ColumnName [];


/* Prints an error message.
 * If IgnoreErrors is not set, the program ends here with 'status'.  */
#define ERR(status, ...) do {				\
	fprintf(stderr, PROGNAME": "__VA_ARGS__);	\
	if (! IgnoreErrors) {				\
		exit(status); 				\
	}						\
  }while(0)

/* Prints an error message and ends the program with 'status'.  */
#define FAIL(status, ...) do {				\
	fprintf(stderr, PROGNAME": "__VA_ARGS__);	\
	exit(status);					\
  }while(0)

#define VERBOSE(...) do {					\
	if (Verbose) {						\
		fprintf(stderr, PROGNAME": " __VA_ARGS__);	\
	}							\
  }while(0)


#define streq(a, b) (0 == strcmp(a, b))


#ifdef __GNUC__
  #define likely(c)   __builtin_expect(!!(c), 1)
  #define unlikely(c) __builtin_expect(!!(c), 0)
#else
  #define likely(c)   (c)
  #define unlikely(c) (c)
#endif


#endif  // GLOBAL_H
