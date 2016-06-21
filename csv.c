#include "csv.h"

enum opmode Opmode      = OP_PresetFields;
bool AllowBreaks        = false;
bool ContinueOnError    = false;
bool ArbitrarySeparator = false;
unsigned int SkipLines  = 0;
signed int   LimitLines = -1;

bool Flush      = false;
bool Verbose    = false;
bool GotIndexes = false;
char* Indexes [MAXFIELDS];
FILE* in;

char sepchar = '\0';


int main (int argc, char** argv) {
	init();

	char c;
	while( (c = getopt(argc, argv, "Ffnpabes:l:vhV")) != -1 )
		switch (c) {
			case 'h': Help(); return 0;
			case 'V': Version(); return 0;
			case 'v': Verbose=true; break;
			case 'a': ArbitrarySeparator=true; break;
			case 'b': AllowBreaks=true; break;
			case 'e': ContinueOnError=true; break;
			case 'f': Opmode = OP_FixedFields; break;
			case 'n': Opmode = OP_NamedFields; break;
			case 'p': Opmode = OP_PresetFields; break;
			case 's': SkipLines = atoi(optarg); break;
			case 'l': LimitLines = atoi(optarg); break;
			case 'F': Flush=true; break;
		}


	while(SkipLines > 0) {
		getLine(true);
		SkipLines--;
	}
	linenum = 0;


	int i, fieldpos = 0;
	char *tok,
	     *line;

	if (OP(NamedFields) || OP(PresetFields)) {

		if (OP(NamedFields) && optind >= argc) {
			fprintf(stderr, PROGNAME": no field names given\n");
			exit(1);
		}

		while(1) {
			char c, *t;

			t = line = getLine(true);

			while( ((c = *t)) && (isblank(c) || IsSeparator_(c)) )
				t++;
			if ( ! (c=='\0' || c=='\r' || c=='\n' || IsSeparator_(c)) ) {
				if (Verbose)  fprintf(stderr, "Got field names in line %i\n", linenum);
				if (ArbitrarySeparator) {
					// Trennzeichen finden...
					t = line;
					while( ((c = *t)) && !IsSeparator_(c) )  t++;
					if (c)	sepchar = c;
					else	ArbitrarySeparator = false;
				}
				break;
			}
			// Zeile mit echtem Content gefunden? Dann gehts jetzt richtig los.
			// Ansonsten die nächste Zeile betrachten.
			if (Verbose)  fprintf(stderr, "Skipped junk line %i\n", linenum);
		}


		if (OP(NamedFields))
			// Die Reihenfolge der Felder ist noch nicht bekannt.
			// Daher werden dem Programm einige Spaltennamen mitgegeben, die ausgelesen werden sollen
			// und in der ersten Zeile der CSV-Datei definiert werden müssen.
			// Trennzeichen "."
			// Alternativennennung möglich:
			//   vorname firstname first_name . nachname lastname last_name .
			//
			// Erste Zeile auslesen und mit den gegebenen (gewünschten) Feldnamen vergleichen:
			while(( tok = nextToken(&line) )) {
				
				int lk; // argv-index des letzten haupt-keywords

				for(i=optind,lk=0; i<argc; i++)
					if (lk && argv[i][0]=='.' && argv[i][1]=='\0')
						lk = 0;
					else {
						if (! lk)
							lk = i;
						if (strcasecmp(argv[i], tok) == 0) {
							Indexes[ fieldpos ] = argv[lk];
							GotIndexes = true;
							if (Verbose)  fprintf(stderr, "Got field argv[%i]=%s: %s (position=%i)\n", lk, argv[lk], tok, fieldpos);
							break;
						}
					}

				if (++fieldpos >= MAXFIELDS) {
					fprintf(stderr, PROGNAME": too many fields in input (max %i)\n", MAXFIELDS);
					if (! ContinueOnError)
						exit(1);
					break;
				}
			}
		else if (OP(PresetFields))
			// Erste Zeile auslesen und die dort gefundenen Feldnamen übernehmen:
			while(( tok = nextToken(&line) )) {

				Indexes[ fieldpos ] = strdup(tok);
				GotIndexes = true;
				if (Verbose)  fprintf(stderr, "Got field %s (position=%i)\n", tok, fieldpos);

				if (++fieldpos >= MAXFIELDS) {
					fprintf(stderr, PROGNAME": too many fields in input (max %i)\n", MAXFIELDS);
					if (! ContinueOnError)
						exit(1);
					break;
				}
			}

	} else if (OP(FixedFields)) {

		// Die Reihenfolge der Felder ist bereits bekannt und wird übergeben.
		// Nicht auszulesende Felder werden durch ein @ gekennzeichnet:
		//   firstname lastname @ phone

		if (optind >= argc) {
			fprintf(stderr, PROGNAME": no field names given\n");
			exit(1);
		}

		for(i=optind; i<argc; i++) {
			if (! (argv[i][0]=='@' && argv[i][1]=='\0')) {
				Indexes[ fieldpos ] = argv[i];
				GotIndexes = true;
				if (Verbose)  fprintf(stderr, "Got field %s (position=%i)\n", argv[i], fieldpos);
			}
			fieldpos++;
		}

	}


	if (! GotIndexes) {
		fprintf(stderr, PROGNAME": no field names given\n");
		exit(1);
	}

	if (Flush || Verbose)
		fflush(stderr);

	// Hier beginnt das eigentliche CSV-Parsing:
	parse();

	return 0;
}


void init (void) {
	register unsigned int i = 0;
	while( i < MAXFIELDS )
		Indexes[i++] = NULL;

	in = fdopen(fileno(stdin), "r");
}


void parse () {
	
	char* line;
	char* tok;

	bool chk;
	bool firstline = true; // erste zeile mit echtem inhalt?

	unsigned int fieldpos;


	while(( line = getLine(false) )) {

		fieldpos = 0;
		chk      = false;

		if (firstline && OP(FixedFields) && ArbitrarySeparator) {
			// Trennzeichen finden:
			char c, *t=line;
			while( ((c = *t)) && !IsSeparator_(c) )  t++;
			if (c)	sepchar = c;
			else	ArbitrarySeparator = false;
		}
		
		while(( tok = nextToken(&line) )) {
			if (Indexes[fieldpos]) {
				printf("%s: %s\n", Indexes[fieldpos], tok);
				chk = true;
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

		if (LimitLines >= 0 && linenum >= LimitLines) {
			if (Verbose)  fprintf(stderr, "Line limit hit, terminating.\n");
			break;
		}

		firstline = false;
	}
}
