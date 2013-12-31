#ifndef BASE_GLOBAL_DECLARATION_H
#define BASE_GLOBAL_DECLARATION_H 1

#ifdef WIN32

#define PATH_SYMBOL '\\'

// stdint declarations
typedef unsigned char    uint8_t;
typedef signed char      int8_t;
typedef unsigned short   uint16_t;
typedef signed short     int16_t;
typedef unsigned long    uint32_t;
typedef signed long      int32_t;
typedef __int64          int64_t;
typedef unsigned __int64 uint64_t;
#else

#include <stdint.h>
#include <stddef.h>
#define PATH_SYMBOL '/'
#define __cdecl

#endif


#ifndef align32
#define align32(value) ((((value) + 3) >> 2) << 2)
#endif

#ifndef QT_NO_DEBUG
//lint -save -e1771 -e818

//#define MEMLEAK_DETECTION	// uncomment this define to use our mem-leak-detection

#ifdef MEMLEAK_DETECTION

#include "Global/Include/MemLeakDetection.h"
#include <new>

#pragma warning(push)
#pragma warning (disable: 4290) 


void* operator new(std::size_t size) throw (std::bad_alloc)
{
   return MemLeakDetection::getInstance().allocate(size);
}

void* operator new[](std::size_t size) throw (std::bad_alloc)
{
   return MemLeakDetection::getInstance().allocate(size);
}

void operator delete(void *p) throw ()
{
   if (p != NULL)
   {
      MemLeakDetection::getInstance().deallocate (p);
   }
}

void operator delete[](void *p) throw ()
{
   if (p != NULL)
   {
      MemLeakDetection::getInstance().deallocate (p);
   }
}


void* operator new(std::size_t size, const std::nothrow_t&) throw()
{
    return MemLeakDetection::getInstance().allocate(size);
}

void* operator new[](std::size_t size, const std::nothrow_t&) throw()
{
    return MemLeakDetection::getInstance().allocate(size);
}

void operator delete(void* p, const std::nothrow_t&) throw()
{
    if (p != NULL)
    {
       MemLeakDetection::getInstance().deallocate (p);
    }
}

void operator delete[](void* p, const std::nothrow_t&) throw()
{
    if (p != NULL)
    {
       MemLeakDetection::getInstance().deallocate (p);
    }
}

//void* operator new(std::size_t size, const char* , int) throw (std::bad_alloc)
//{
//   return MemLeakDetection::getInstance().allocate(size);
//}

//void* operator new[](std::size_t nSize, const char* , int ) throw (std::bad_alloc)
//{
//   return MemLeakDetection::getInstance().allocate(nSize);
//}


#pragma warning(pop)

//lint -restore


#endif MEMLEAK_DETECTION

#endif // #ifdef _DEBUG

#endif
