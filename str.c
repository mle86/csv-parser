char* getLine (short needLine) {
	static char* lp = NULL;
	if (! lp) {
		lp = strMalloc(MAXLINELEN);
		lp[0] = '\0';
	}
	if (! fgets(lp, MAXLINELEN, in))
		exit(needLine);
	skipWhitespace(&lp);
	chopWhitespace(lp);
	return lp;
}

void skipWhitespace (char** s) {
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

char* nextToken (char** s) {
	char c;
	char strdelim = '\0';
	char* tokstart = *s;

	#define  IsSeparator(c)  ( c==',' || c==';' || c==':' || c=='\0' )
	
	if (! (c = **s))
		return NULL;
	if (c=='"' || c=='\'' || c=='`') {
		strdelim = c;
		tokstart++;
		(*s)++;
	}

	while(( c = **s )) {
		if (c==strdelim && *(1+*s)==strdelim) {
			// doppeltes string-begrenzungszeichen: kein string-ende, sondern escaping.
			// --> "richtig" escapen und weiter.
			**s = '\\';
			(*s) += 2;
			continue;
		}
		if ( IsSeparator(c) || (c==strdelim && IsSeparator( *(1+*s) )) ) {
			**s = '\0';
			*s += (c==strdelim ? 2 : 1);
			return tokstart;
		}
		(*s)++;
	}

	return (*s==tokstart ? NULL : tokstart);
}

short strIEqual (char* a, char* b) {
	if (a == b) return 1;
	if (! a)    return 0;
	if (! b)    return 0;
	do {
		if ( lc(*a) != lc(*b) )
			return 0; // not equal
	} while( *a++ && *b++ );
	return 1; // equal
}
