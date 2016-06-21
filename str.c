#include "str.h"

unsigned int linenum  = 0;
size_t       linelen = 0;


static char*  linebuffer = NULL;
static size_t buffersize = 0;

static void initLinebuffer (size_t needed) {
	if (! needed)
		needed = MAXLINELEN;
	if (needed > buffersize) {
		buffersize = needed;
		linebuffer = realloc(linebuffer, buffersize + 1);

		if (! linebuffer) {
			perror(PROGNAME": malloc()");
			exit(1);
		}
	}
}


char* getLine (bool needLine) {
	initLinebuffer(0);

	if (! fgets(linebuffer, MAXLINELEN, in)) {
		if (needLine) exit(1);
		else          return NULL;
	}

	linenum++;


	off_t woff =
	skipWhitespace( linebuffer);
	chopWhitespace((linebuffer + woff), &linelen);

	return (linebuffer + woff);
}

char* appendLine (char* oldline) {
	char*  lastline    = strdup(oldline);
	size_t lastlinelen = strlen(oldline);
//	size_t lastlinelen = linelen;

	const char between = (lastlinelen > 0) ? '\n' : '\0';
	const off_t boff   = (between ? 1 : 0);

	char* nextline = getLine(false);
	if (! nextline)  return lastline;

	initLinebuffer(lastlinelen + linelen + boff);
//	initLinebuffer(buffersize + MAXLINELEN + boff);
	
	memmove(linebuffer + lastlinelen + boff, nextline, linelen);
	memmove(linebuffer,                      lastline, lastlinelen);

	if (between)
	linebuffer[lastlinelen]                  = between;
	linebuffer[lastlinelen + boff + linelen] = '\0';

	free(lastline);

	linelen += lastlinelen + boff;
	return linebuffer;
}


off_t skipWhitespace (const char* s) {
	register const char* t = s;
	while(isblank(*t))
		t++;
	return (t - s);
}

void chopWhitespace (char* s, size_t *saveLen) {
	register unsigned int len = 0;
	while(s[len])
		len++;
	if (saveLen)  *saveLen = len;

	for(len--; len>0; len--)
		if (isspace(s[len])) {
			if (! isspace(s[len-1])) {
				s[len] = '\0';
				if (saveLen)  *saveLen = len;
				return;
			}
		} else return;

	s[0] = '\0';
}

char* breaksToBlank (char* s) {
	if (!s || !s[0])
		return s;
	for (register char* p = s; *p; p++)
		if (*p == '\n' /*|| *p == '\r'*/)
			*p = ' ';
	return s;
}

void outputJsonString (char* s) {
	if (!s || !*s)
		return;
	for (; *s; s++)
	switch (*s) {
		case 0x07: fputs("\\b", stdout); break;
		case 0x09: fputs("\\t", stdout); break;
		case 0x0a: fputs("\\n", stdout); break;
		case 0x0c: fputs("\\f", stdout); break;
		case 0x0d: fputs("\\r", stdout); break;
		case '/':  fputs("\\/", stdout); break;
		case '\\': fputs("\\\\", stdout); break;
		case '"':  fputs("\\\"", stdout); break;
		default:
			if (*s >= 0 && *s <= 31)
				printf("\\u%04X", (unsigned int)*s);
			else if (s[0]==0xe2 && s[1]==0x80 && s[2]==0xa8)
				// JS does not like these.
				// http://timelessrepo.com/json-isnt-a-javascript-subset
				puts("\\u2028"), s += 2;
			else if (s[0]==0xe2 && s[1]==0x80 && s[2]==0xa9)
				puts("\\u2029"), s += 2;
			else
				putchar(*s);
	}
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


continue_line:
	while(( c = **s )) {
		if (c==strdelim && c && *(1+*s)==strdelim /* && !IsSeparator(*(2+*s)) */ ) {
			// doppeltes string-begrenzungszeichen: kein string-ende, sondern escaping.
			// --> "richtig" escapen und weiter.
//			**s = '\\';
			(*s) += 2;
			continue;
		}
		if ( (IsSeparator(c) && !strdelim) || (c==strdelim && IsSeparator( *(1+*s) )) ) {
			**s = '\0';
			*s += ((c && c==strdelim && (*s)[1]) ? 2 : 1);
				
			return tokstart;
		}
		(*s)++;
	}

	if (AllowBreaks && strdelim) {
		// linebreak inside field!
		off_t diff = (*s - tokstart);
		tokstart = appendLine(tokstart);
		*s = tokstart + diff;
		goto continue_line;
	}

	return (*s==tokstart ? NULL : tokstart);
}
