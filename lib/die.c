#ifndef __DIE_C
#define __DIE_C

#ifndef ERRHANDLE
#define ERRHANDLE stderr
#endif

void die (char* errfmt, ...) {
	va_list ap;
	va_start(ap, errfmt);
	vfprintf(ERRHANDLE, errfmt, ap);
	fprintf(ERRHANDLE, "\n");
	fflush(ERRHANDLE);
	va_end(ap);
	exit(1);
}

void err (char * errfmt, ...) {
	va_list ap;
	va_start(ap, errfmt);
	vfprintf(ERRHANDLE, errfmt, ap);
	fprintf(ERRHANDLE, "\n");
	fflush(ERRHANDLE);
	va_end(ap);
}


#endif // __DIE_C
