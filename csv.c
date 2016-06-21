#include "csv.h"

bool FixedFields = 0;
bool NamedFields = 1;
bool ContinueOnError = 0;
unsigned int SkipLines = 0;

bool Flush = 0;
bool Verbose = 0;
bool GotIndexes = 0;
char* Indexes [MAXFIELDS];
FILE* in;


int main (int argc, char** argv) {
	int linenum = 0;
	int i, fieldpos = 0; // feldzähler für jede zeile
	char c, *tok, *line;

	init();

	while( (c = getopt(argc, argv, "Ffnes:vhV")) != -1 )
		switch (c) {
			case 'h': Help(); return 0;
			case 'V': Version(); return 0;
			case 'v': Verbose++; break;
			case 'e': ContinueOnError=1; break;
			case 'f': FixedFields=1; NamedFields=0; break;
			case 'n': NamedFields=1; FixedFields=0; break;
			case 's': SkipLines = atoi(optarg); break;
			case 'F': Flush=1; break;
		}


	while(SkipLines > 0) {
		getLine(1);
		SkipLines--;
	}


	if (NamedFields) {

		// Die Reihenfolge der Felder ist noch nicht bekannt.
		// Daher werden dem Programm einige Spaltennamen mitgegeben, die ausgelesen werden sollen
		// und in der ersten Zeile der CSV-Datei definiert werden müssen.
		// Trennzeichen "."
		// Alternativennennung möglich:
		//   vorname firstname first_name . nachname lastname last_name .
		if (optind >= argc)
			die(PROGNAME ": no field names given");

		while(1) {
			char c, *t;

			linenum++;
			t = line = getLine(1);

			while( ((c = *t)) && (IsWhitespace(c) || IsSeparator(c)) )
				t++;
			if ( ! (c=='\0' || c=='\r' || c=='\n' || IsSeparator(c)) ) {
				if (Verbose)  fprintf(stderr, "Got field names in line %i\n", linenum);
				break;
			}
			// Zeile mit echtem Content gefunden? Dann gehts jetzt richtig los.
			// Ansonsten die nächste Zeile betrachten.
			if (Verbose)  fprintf(stderr, "Skipped junk line %i\n", linenum);
		}



		while(( tok = nextToken(&line) )) {
			
			int lk; // argv-index des letzten haupt-keywords

			for(i=optind,lk=0; i<argc; i++)
				if (lk && argv[i][0]=='.' && argv[i][1]=='\0')
					lk = 0;
				else {
					if (! lk)
						lk = i;
					if (strIEqual(argv[i], tok)) {
						Indexes[ fieldpos ] = argv[lk];
						GotIndexes = 1;
						if (Verbose)  fprintf(stderr, "Got field argv[%i]=%s: %s (position=%i)\n", lk, argv[lk], tok, fieldpos);
						break;
					}
				}

			if (++fieldpos >= MAXFIELDS) 
				die(PROGNAME ": too many fields in input (max %i)", MAXFIELDS);
		}

	} else if (FixedFields) {

		// Die Reihenfolge der Felder ist bereits bekannt und wird übergeben.
		// Nicht auszulesende Felder werden durch ein @ gekennzeichnet:
		//   vorname lastname @ phone
		if (optind >= argc)
			die(PROGNAME ": no field names given");

		for(i=optind; i<argc; i++) {
			if (! (argv[i][0]=='@' && argv[i][1]=='\0')) {
				Indexes[ fieldpos ] = argv[i];
				GotIndexes = 1;
				if (Verbose)  fprintf(stderr, "Got field %s (position=%i)\n", argv[i], fieldpos);
			}
			fieldpos++;
		}

	}


	if (! GotIndexes)
		die(PROGNAME ": no field names given");

	if (Flush || Verbose)
		fflush(stderr);


	// Hier beginnt das eigentliche CSV-Parsing:

	while(( line = getLine(0) )) {
		bool chk = 0;
		fieldpos = 0;
		linenum++;
		while(( tok = nextToken(&line) )) {
			if (Indexes[fieldpos]) {
				printf("%s: %s\n", Indexes[fieldpos], tok);
				chk = 1;
			}
			if (++fieldpos >= MAXFIELDS) {
				fprintf(stderr, "%s: too many fields in input line %i (max %i)\n", PROGNAME, linenum, MAXFIELDS);
				if (! ContinueOnError)
					exit(1);
			}
		}

		if (chk) {
			if (Verbose)  fprintf(stderr, "This was line %i\n", linenum);
			printf("-\n");
		} else if (Verbose)  fprintf(stderr, "Nothing found on line %i\n", linenum);

		if (Flush) {
			if (Verbose)
				fflush(stderr);
			fflush(stdout);
		}
	}

	return 0;
}

void init (void) {
	register unsigned int i = MAXFIELDS;
	while(i)
		Indexes[i--] = NULL;
	in = fdopen(0, "r");
}


void Help (void) { printf(
	M1 PROGNAME M0 " reads a CSV file from standard input and filters it.\n"
	"Usage: " M1 PROGNAME M0 " [" M1 "OPTIONS" M0 "] " M1 "FIELDNAME" M0 " [" M1 "..." M0 "]\n"
	"Options:\n"
	M1 "  -n    " M0 "Non-fixed, named fields:\n"
	   "        "    "The program reads field names from the argument list and compares them\n"
	   "        "    "with the first line of the CSV file, then reads in all lines\n"
	   "        "    "and prints these fields. Separate field names with a single \"" M1 "." M0 "\".\n"
	   "        "    "The name order is irrelevant.\n"
	M1 "  -f    " M0 "Fixed names for field positions:\n"
	   "        "    "The program reads field names from the argument list applies them\n"
	   "        "    "to all lines. Field positions which are to be ignored may be marked\n"
	   "        "    "with a single \"" M1 "@" M0 "\".\n"
	   "        "    "The argument order represents the field order in the CSV input.\n"
	M1 "  -e    " M0 "Don't quit on parse errors.\n"
	M1 "  -s N  " M0 "Skips the first " M1 "N" M0 " input lines.\n"
	   "        "    "(Skipped lines are NOT counted for line numbers.)\n"
	M1 "  -F    " M0 "Flush stdout after every parsed input line.\n"
	M1 "  -h    " M0 "This help\n"
	M1 "  -V    " M0 "Program version\n"
	M1 "  -v    " M0 "Verbose parsing on stderr\n"
	"Default is " M1 "-n" M0 ". Options " M1 "-nf" M0 " are mutually exclusive.\n"
	"\n"
); exit(0); }

void Version (void) { printf(
	PROGNAME " v" VERSION "\n"
	"Written by Maximilian Eul <mle@multinion.de>, November 2007.\n"
	"\n"
); exit(0); }
