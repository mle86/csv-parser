char* getLine (bool needLine) {
	static char* lp = NULL;
	if (! lp) {
		lp = malloc(1 + MAXLINELEN);
		if (! lp) {
			perror(PROGNAME": malloc()");
			exit(1);
		}

		lp[0] = '\0';
	}

	if (! fgets(lp, MAXLINELEN, in))
		exit(needLine ? 1 : 0);

	skipWhitespace(&lp);
	chopWhitespace(lp);
	return lp;
}

void skipWhitespace (char* *s) {
	while(**s==' ' || **s=='\t')
		(*s)++;
}

void chopWhitespace (char* s) {
	register unsigned int len = 0;
	while(s[len])
		len++;
	for(len--; len>0; len--)
		if (IsExtWhitespace(s[len])) {
			if (! IsExtWhitespace(s[len-1])) {
				s[len] = '\0';
				return;
			}
		} else return;
	s[0] = '\0';
}

char* nextToken (char* *s) {
	char c;
	char strdelim = '\0';
	char* tokstart = *s;

	if (! (c = **s))
		return NULL;
	if (c=='"' || c=='\'' || c=='`') {
		strdelim = c;
		tokstart++;
		(*s)++;
	}

	while(( c = **s )) {
		if (c==strdelim && c && *(1+*s)==strdelim && !IsSeparator(*(2+*s))) {
			// doppeltes string-begrenzungszeichen: kein string-ende, sondern escaping.
			// --> "richtig" escapen und weiter.
//			**s = '\\';
			(*s) += 2;
			continue;
		}
		if ( (IsSeparator(c) && !strdelim) || (c==strdelim && IsSeparator( *(1+*s) )) ) {
			**s = '\0';
			*s += (c==strdelim && c ? 2 : 1);
			return tokstart;
		}
		(*s)++;
	}

	return (*s==tokstart ? NULL : tokstart);
}

bool strIEqual (char* a, char* b) {
	if (a == b) return true;
	if (! a)    return false;
	if (! b)    return false;
	do {
		if ( lc(*a) != lc(*b) )
			return false; // not equal
	} while( *a++ && *b++ );
	return true; // equal
}
