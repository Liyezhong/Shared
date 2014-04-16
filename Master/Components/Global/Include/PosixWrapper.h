#ifndef POSIX_WRAPPER_H
#define POSIX_WRAPPER_H 1

#ifdef WIN32
#pragma warning (disable: 4996) 

#include "Base/GlobalDeclaration.h"
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <io.h>
#include <string.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 512
#endif
#ifndef snprintf
//lint -save -e1924
inline int snprintf(char *str, size_t count, const char *fmt, ...)
{
   int len;
   va_list ap;

   va_start(ap, fmt);
   len = vsnprintf_s(str, count,_TRUNCATE, fmt, ap);
   va_end(ap);
   return len;
//lint -restore
}
#endif

#ifndef strerror_r
#define strerror_r(errnum,buf,bufflen)  strerror_s(buf,bufflen,errnum)
#endif

#ifndef open
//lint -save -e652
#define open _open
//lint -restore
#endif

#ifndef ssize_t
#define ssize_t int
#endif

#ifndef localtime_r
inline struct tm *localtime_r (const time_t *tick, struct tm *brokenTime)
{
   errno = localtime_s (brokenTime,
                        tick);
   if (errno == 0)
   {
      return brokenTime;
   }
   return NULL;
}
#endif

#endif // WIN32
#endif // POSIX_WRAPPER_H
