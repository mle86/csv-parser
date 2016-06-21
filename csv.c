#include "csv.h"

short Opmode = OP_PresetFields;
bool ContinueOnError = false;
bool ArbitrarySeparator = false;
unsigned int SkipLines = 0;
signed int   LimitLines = -1;

bool Flush = false;
bool Verbose = false;
bool GotIndexes = false;
char* Indexes [MAXFIELDS];
FILE* in;

char sepchar = '\0';


int main (int argc, char** argv) {
	int linenum = 0;
	bool firstline = true; // erste zeile mit echtem inhalt?
	int i, fieldpos = 0; // feldzähler für jede zeile
	char c, *tok, *line;

	init();

	while( (c = getopt(argc, argv, "Ffnpaes:l:vhV")) != -1 )
		switch (c) {
			case 'h': Help(); return 0;
			case 'V': Version(); return 0;
			case 'v': Verbose=true; break;
			case 'a': ArbitrarySeparator=true; break;
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


	if (OP(NamedFields) || OP(PresetFields)) {

		if (OP(NamedFields) && optind >= argc) {
			fprintf(stderr, PROGNAME": no field names given\n");
			exit(1);
		}

		while(1) {
			char c, *t;

			linenum++;
			t = line = getLine(true);

			while( ((c = *t)) && (IsWhitespace(c) || IsSeparator_(c)) )
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
						if (strIEqual(argv[i], tok)) {
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
		//   vorname lastname @ phone

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

	while(( line = getLine(false) )) {
		bool chk = false;
		fieldpos = 0;

		linenum++;
		
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

	return 0;
}

void init (void) {
	register unsigned int i = MAXFIELDS;
	while(i)
		Indexes[i--] = NULL;
	in = fdopen(fileno(stdin), "r");
}


void Help (void) { printf(
	M1 PROGNAME M0" reads a CSV file from standard input and filters it.\n"
	"Usage: "M1 PROGNAME M0" ["M1"OPTIONS"M0"] ["M1"FIELDNAME ..."M0"]\n"
	"Options:\n"
	M1"  -n    "M0"Non-fixed, named fields:\n"
	  "        "  "The program reads field names from the argument list ("M1"FIELDNAME"M0"S)\n"
	  "        "  "and compares them with the first line of the CSV file, then reads in\n"
	  "        "  "all lines and prints these fields.\n"
	  "        "  "Separate field names with a single \""M1"."M0"\".\n"
	  "        "  "The name order is irrelevant, as is case.\n"
	M1"  -f    "M0"Fixed names for field positions:\n"
	  "        "  "The program reads field names from the argument list ("M1"FIELDNAME"M0"S)\n"
	  "        "  "and applies them to all lines. Field positions which are to be ignored\n"
	  "        "  "may be marked with a single \""M1"@"M0"\".\n"
	  "        "  "The argument order represents the field order in the CSV input.\n"
	M1"  -p    "M0"The programm reads field names from the first line and prints all fields\n"
	  "        "  "with their names from the first line.\n"
	  "        "  "In this mode, "M1"FIELDNAME"M0" specifications are neither needed nor possible.\n"
	"\n"
	M1"  -e    "M0"Don't quit on parse errors.\n"
	M1"  -a    "M0"Only use separator which was found in first line.\n"
	M1"  -s N  "M0"Skips the first "M1"N"M0" input lines.\n"
	  "        "  "(Skipped lines are NOT counted for line numbers.)\n"
	M1"  -l N  "M0"Limit parsing to "M1"N"M0" input lines.\n"
	  "        "  "As soon as the "M1"N"M0"th line is reached (skipped lines not counted),\n"
	  "        "  "the program will terminate. Limiting to zero lines is of course useless,\n"
	  "        "  "except for mode "M1"-p"M0" (preset field names), where no content parsing\n"
	  "        "  "will be done, instead the program will terminate after printing\n"
	  "        "  "all field names as found on first line.\n"
	M1"  -F    "M0"Flush stdout after every parsed input line.\n"
	M1"  -h    "M0"This help\n"
	M1"  -V    "M0"Program version\n"
	M1"  -v    "M0"Verbose parsing on stderr\n"
	"\n"
	"Default is "M1"-p"M0". Options "M1"-nfp"M0" are mutually exclusive.\n"
	"\n"
); exit(0); }

void Version (void) { printf(
	PROGNAME " v" VERSION "\n"
	"Written by Maximilian Eul <mle@multinion.de>, April 2010.\n"
	"\n"
); exit(0); }
