#ifndef __MACROS_H
#define __MACROS_H
//#include <string.h>


char _mhc;

#define IsWhitespace(c)     ( (_mhc=c), ((_mhc == ' ') || (_mhc == '\t')) )
#define IsExtWhitespace(c) \
	( (_mhc=c), (IsWhitespace(_mhc) || (_mhc == '\r') || (_mhc == '\n')) )
#define IsAlpha(c)    ( (_mhc=c), (IsUAlpha(c) || IsLAlpha(c)) )
#define IsUAlpha(c)   ( (_mhc=c), ((_mhc >= 'A') && (_mhc <= 'Z')) )
#define IsLAlpha(c)   ( (_mhc=c), ((_mhc >= 'a') && (_mhc <= 'z')) )

#define uc(c) ( IsLAlpha(c) ? (_mhc ^ 32) : _mhc )
#define lc(c) ( IsUAlpha(c) ? (_mhc | 32) : _mhc )


#endif // __MACROS_H
