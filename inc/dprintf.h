#ifndef _DPRINTF_H_
#define _DPRINTF_H_

#include "COMMON.H"

#ifndef _DEBUG
#define dprintf	\/\/dprintf
#else
void dprintf( char *fmt, ... );
#endif

#endif // _DPRINTF_H_
