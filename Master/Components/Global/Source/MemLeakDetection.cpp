///////////////////////////////////////////////////////////
//  MemLeakDetection.cpp
//  Implementation of the Class MemLeakDetection
//  Created on:      29-Sep-2009 07:08:44
//  Original author: Hans-Joerg Gruenthal
///////////////////////////////////////////////////////////

#include "Global/Include/MemLeakDetection.h"
#include "Global/Include/PosixWrapper.h"
#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/Exception.h"
#include "Global/Include/StackTraceUtility.h"
#include "Global/Include/GlobalDeclaration.h"

#include <QRegExp>
#include <QStringList>
#include <cxxabi.h>

#include <stdio.h>
#include <exception>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#else
#define CRITICAL_SECTION void*
#define EnterCriticalSection(x)
#define LeaveCriticalSection(x)
#define InitializeCriticalSection(x)
#define DeleteCriticalSection(x)
#endif

#ifdef MEMLEAK_DETECTION
static const unsigned int TextSize = 256U;

MemLeakDetection::MemLog* MemLeakDetection::MemLog::s_instance = NULL;

MemLeakDetection* MemLeakDetection::s_memLeakDetectionP = NULL;

// Critical Section - Windows vs Linux in C++
// http://thompsonng.blogspot.de/2011/06/critical-section-windows-vs-linux-in-c.html

#ifdef WIN32
static CRITICAL_SECTION csData[MemLeakDetection::ElementCount];
#else
#include <pthread.h>
static pthread_mutex_t csData[MemLeakDetection::ElementCount];
#endif

#ifdef WIN32
    class ScopedInternalMemoryLock
    {
    public:
       explicit ScopedInternalMemoryLock (CRITICAL_SECTION *handle) : m_handle (handle)
       {
          if (m_handle != NULL)
          {
             EnterCriticalSection(m_handle);
          }
       }
       ~ScopedInternalMemoryLock ()
       {
          if (m_handle != NULL)
          {
             LeaveCriticalSection (m_handle);
          }
          m_handle = NULL;
       }
    private:
       CRITICAL_SECTION *m_handle;
    };
#else
    class ScopedInternalMemoryLock
    {
    public:
       explicit ScopedInternalMemoryLock (pthread_mutex_t *handle) : m_handle (handle)
       {
          if (m_handle != NULL)
          {
             pthread_mutex_lock (m_handle);
          }
       }
       ~ScopedInternalMemoryLock ()
       {
          if (m_handle != NULL)
          {
             pthread_mutex_unlock (m_handle);
          }
          m_handle = NULL;
       }
    private:
       pthread_mutex_t *m_handle;
    };
#endif


//static void
//getCallStack (MemLeakDetection::StackEntry *addressVector)
//{
//   memset (addressVector,
//           0,
//           MemLeakDetection::StackEntries * sizeof (MemLeakDetection::StackEntry));

//   unsigned int ebpAtAllocation = StackTraceUtility::getCurrentEBP ();

//   unsigned int count = 0U;
//   while ( (ebpAtAllocation != 0U)
//           &&
//           (count < MemLeakDetection::StackEntries) )
//   {
//      const unsigned int &oldEbp (*reinterpret_cast<unsigned int*>(ebpAtAllocation));
//      const unsigned int &oldEip (*reinterpret_cast<unsigned int*>(ebpAtAllocation + 4));
//      addressVector[count].instruction = oldEip;
//      count++;
//      ebpAtAllocation = oldEbp;
//   }
//}


#include <execinfo.h>

static void
getCallStack (MemLeakDetection::StackEntry *addressVector)
{
    memset (addressVector,
            0,
            MemLeakDetection::StackEntries * sizeof (MemLeakDetection::StackEntry));

    void *array[MemLeakDetection::StackEntries];
    size_t nfuncs;

    nfuncs = backtrace(array, MemLeakDetection::StackEntries);

    for (int i = 0; i<nfuncs; i++) {
#ifndef __x86_64
        addressVector[i].instruction = (unsigned int)array[i];
#endif
    }
}


MemLeakDetection::MemLeakDetection()
   : m_log ()
{
   unsigned int index = 0U;
   const unsigned int elementSizes[] = 
   {
      ElementSize0Byte,
      ElementSize4Byte,
      ElementSize8Byte,
      ElementSize16Byte,
      ElementSize32Byte,
      ElementSize64Byte,
      ElementSize128Byte,
      ElementSize256Byte,
      ElementSize512Byte,
      ElementSize1024Byte,
      ElementSize2048Byte,
      ElementSize4096Byte,
      ElementSize8192Byte,
      ElementSize16384Byte,
      ElementSize32768Byte,
      ElementSize65536Byte,
      ElementSize131072Byte,
      ElementSize262144Byte,
      ElementSize524288Byte,
      ElementSize1048576Byte,
      ElementSize5242880Byte
   };
   const unsigned int elementNo[] = 
   {
      ElementNo0Byte,
      ElementNo4Byte,
      ElementNo8Byte,
      ElementNo16Byte,
      ElementNo32Byte,
      ElementNo64Byte,
      ElementNo128Byte,
      ElementNo256Byte,
      ElementNo512Byte,
      ElementNo1024Byte,
      ElementNo2048Byte,
      ElementNo4096Byte,
      ElementNo8192Byte,
      ElementNo16384Byte,
      ElementNo32768Byte,
      ElementNo65536Byte,
      ElementNo131072Byte,
      ElementNo262144Byte,
      ElementNo524288Byte,
      ElementNo1048576Byte,
      ElementNo5242880Byte
   };
   const unsigned int memSize[] = 
   {
      MemSize0,
      MemSize4,
      MemSize8,
      MemSize16,
      MemSize32,
      MemSize64,
      MemSize128,
      MemSize256,
      MemSize512,
      MemSize1024,
      MemSize2048,
      MemSize4096,
      MemSize8192,
      MemSize16384,
      MemSize32768,
      MemSize65536,
      MemSize131072,
      MemSize262144,
      MemSize524288,
      MemSize1048576,
      MemSize5242880
   };

   int iCount;
   for (iCount = 0;
        iCount < ElementCount;
        iCount++)
   {
#ifdef WIN32
      InitializeCriticalSection (&csData[iCount]);
#else
       //create mutex attribute variable
       pthread_mutexattr_t mAttr;

       // setup recursive mutex for mutex attribute
       pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP);

       // Use the mutex attribute to create the mutex
       pthread_mutex_init(&csData[iCount], &mAttr);

       // Mutex attribute can be destroy after initializing the mutex variable
       pthread_mutexattr_destroy(&mAttr);
#endif
      m_counters[iCount] = Counter ( elementSizes[iCount],
                                     elementNo[iCount],
                                     m_memoryArea + index,
                                    &csData[iCount]);
      index += memSize[iCount];
   }
}

MemLeakDetection::~MemLeakDetection()
{
   //lint -save -e1551
   int iCount;
   char fileName[MAX_PATH];
   memset (fileName,
           0,
           MAX_PATH);
   snprintf (fileName,
             MAX_PATH,
             "%s%cMemLeakDetection_%p.txt",
             StackTraceUtility::getTempPath ().c_str (),
             PATH_SYMBOL,
             this);

   FILE *outFile = fopen (fileName, "w");
   if (outFile != NULL)
   {
      for (iCount = 0;
           iCount < ElementCount;
           iCount++)
      {
         char statisticText[TextSize];
         fprintf (outFile,
                  "%s\n",
                  m_counters[iCount].toString (statisticText,
                                               TextSize));
      }

      for (iCount = 0;
           iCount < ElementCount;
           iCount++)
      {
         char statisticText[TextSize * 1024 * 8];
         fprintf (outFile,
                  "%s",
                  m_counters[iCount].printLeakText (statisticText,
                                                    TextSize * 1024 * 8));
      }
      fclose (outFile);
      outFile = NULL;
   }
   for (iCount = 0;
        iCount < ElementCount;
        iCount++)
   {
#ifdef WIN32
      DeleteCriticalSection (&csData[iCount]);
#else
    pthread_mutex_destroy (&csData[iCount]);
#endif
   }
   //lint -restore
}


void* MemLeakDetection::allocate(size_t size, void* ptr)
{
// 'placement new'
//	if (ptr)
//		return m_counters[0].allocate (ptr);

   int iCount = 1;
   while ( (iCount < ElementCount)
		   &&
		   (size > m_counters[iCount].getElementSize ()) )
   {
	  iCount++;
   }

   if (iCount >= ElementCount)
   {
      char exceptionText[TextSize];
      snprintf (exceptionText,
                TextSize,
                "MemLeakDetection::allocate(%u): out of memory",
                size);
      Q_ASSERT(false);
      THROWARG (Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, exceptionText);
//      THROW_Exception (exceptionText);
   }

   void* ccc = (void*)0xc85584;
   void* ppp = m_counters[iCount].allocate (NULL);
   if (ccc == ppp) {
       return ppp;
   }
   return ppp;

   return m_counters[iCount].allocate (NULL);
}

void MemLeakDetection::deallocate(const void* ptr)
{
   if (ptr == NULL)
   {
      return;
   }

// 'placement delete'
//    void* pElementBuffer = m_counters[0].search (ptr);
//    if (pElementBuffer)
//    {
//        m_counters[0].free (pElementBuffer);
//        return;
//    }

   if (ptr < m_counters[1].getFirstPtr ())
   {
       Q_ASSERT(false);
       THROWARG (Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, "this memory was not allocated by new/delete");
//      THROW_Exception ("this memory was not allocated by new/delete");
      //::free (ptr)
      //return;
   }

   int iCount = 1;
   while ( (iCount < ElementCount)
           &&
           (ptr > m_counters[iCount].getLastPtr ()) )
   {
      iCount++;
   }

   if (iCount >= ElementCount)
   {
       Q_ASSERT(false);
//       THROWARG (Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, "this memory was not allocated by new/delete");
//      THROW_Exception ("this memory was not allocated by new/delete");
//      ::free (ptr)
      return;
   }

   void* ccc = (void*)0xc85584;
   if (ccc == (ptr)) {
       ccc = NULL;
   }

   m_counters[iCount].free (ptr);
}

MemLeakDetection& MemLeakDetection::getInstance()
{
   if (s_memLeakDetectionP == NULL)
   {
      static MemLeakDetection s_memLeakDetection;
      s_memLeakDetectionP = &s_memLeakDetection;
   }
   return *s_memLeakDetectionP;
}

const char* MemLeakDetection::Counter::toString(char* buffer, size_t bufferSize) const
{
   snprintf (buffer,
             bufferSize,
             "elementSize: %.6u, highWaterMark: %.6u of %.6u, rest %u",
             m_elementSize,
             m_usageMax,
             m_elementNo,
             m_usage);
   return buffer;
}

const char* MemLeakDetection::Counter::printLeakText(char* buffer, size_t bufferSize) const
{
    QRegExp regexp("([^(]+)\\(([^)^+]+)(\\+[^)]+)\\)\\s(\\[[^]]+\\])");

   //lint -save -e953
   memset (buffer,
           0,
           bufferSize);

   unsigned int printed = 0U;
   unsigned int iCount;
   for (iCount = 0U;
        iCount < m_elementNo;
        iCount++)
   {
      const unsigned int   offset    = iCount * (ElementHeaderSize + m_elementSize);
      //lint -e{613} -e{826}
      const ElementStruct *elemntPtr = reinterpret_cast<ElementStruct *>(m_elements + offset);

      if (elemntPtr->inUse != 0)
      {
         int printResult = snprintf (buffer     + printed,
                                     bufferSize - printed,
                                     "\nMemoryLeak: memory %p, a %u bytes segment,  not deallocated, callStack:\n",
                                     &(elemntPtr->elementBuffer),
                                     m_elementSize);
         if (printResult < 0)
         {
            return buffer;
         }

         char** symbols = backtrace_symbols((void**)&(elemntPtr->stackTrace), StackEntries);

         printed += static_cast<unsigned int>(printResult);
         unsigned int iStack;
         for (iStack = 0U;
              (iStack < StackEntries)
              &&
              (elemntPtr->stackTrace[iStack].instruction != 0U);
              iStack++)
         {
            char functionText[TextSize];

#define GETSYMBOL_NEW
#ifdef GETSYMBOL_NEW
            QString fctText;
            if (regexp.indexIn(symbols[iStack]) != -1)
            {
                //get the library or app that contains this symbol
                QString unit = regexp.cap(1);
                //get the symbol
                QString symbol = regexp.cap(2);
                //get the offset
                QString offset = regexp.cap(3);
                //get the address
                QString address = regexp.cap(4);

                //now try and demangle the symbol
                int stat;
                char *demangled =
                        abi::__cxa_demangle(qPrintable(symbol),0,0,&stat);

                if (demangled)
                {
                    symbol = demangled;
                    ::free(demangled);
                }

                //put this all together
                fctText = QString("(%1) %2 (%3 +%4)\n  -- %5\n")
                                    .arg(QString::number(iStack), 3)
                                    .arg(unit).arg(address,offset)
                                    .arg(symbol);
            }
            else
            {
                //split line into words
                QStringList words = QString(symbols[iStack])
                                        .split(" ", QString::SkipEmptyParts);

                if (words.count() == 6 && words[4] == "+")
                {
                    //get the library or app that contains this symbol
                    QString unit = words[1];
                    //get the symbol
                    QString symbol = words[3];
                    //get the offset
                    QString offset = words[5];
                    //get the address
                    QString address = words[2];

                    //now try and demangle the symbol
                    int stat;
                    char *demangled =
                            abi::__cxa_demangle(qPrintable(symbol),0,0,&stat);

                    if (demangled)
                    {
                        symbol = demangled;
                        ::free(demangled);
                    }

                    //put this all together
                    fctText = QString("(%1) %2 (%3 +%4)\n  -- %5\n")
                                        .arg(QString::number(iStack), 3)
                                        .arg(unit).arg(address,offset)
                                        .arg(symbol);
                }
                else
                    //I don't recognise this string - just add the raw
                    //string to the backtrace
                    fctText = symbols[iStack];
            }


            printResult = snprintf (buffer     + printed,
                                    bufferSize - printed,
                                    "%p (%s)\n",
                                    reinterpret_cast<void*>(elemntPtr->stackTrace[iStack].instruction),
                                    fctText.toStdString().c_str()
                                    );
#else
            printResult = snprintf (buffer     + printed,
                                    bufferSize - printed,
                                    "%p (%s)\n",
                                    reinterpret_cast<void*>(elemntPtr->stackTrace[iStack].instruction),
                                    StackTraceUtility::getSymbolName (elemntPtr->stackTrace[iStack].instruction,
                                                                      functionText,
                                                                      TextSize)
                                    );
#endif

            if (printResult < 0)
            {
               return buffer;
            }
            printed += static_cast<unsigned int>(printResult);
         }
      }
   }
   return buffer;
}//lint -restore

void* MemLeakDetection::Counter::allocate (void* ptr)
{
#ifdef WIN32
   ScopedInternalMemoryLock lock (reinterpret_cast<CRITICAL_SECTION*>(m_section));
#else
    ScopedInternalMemoryLock lock (reinterpret_cast<pthread_mutex_t*>(m_section));
#endif
   if (m_usage + 1 > m_elementNo)
   {
      char exceptionText[TextSize];
      snprintf (exceptionText,
                TextSize,
                "MemLeakDetection::Counter::allocate () segment %ubyte: out of memory",
                m_elementSize);
      MemLeakDetection::MemLog::doLogConflict (NULL,
                                               m_elementSize,
                                               exceptionText);
      Q_ASSERT(false);
      THROWARG (Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, exceptionText);
//      THROW_Exception (exceptionText);
   }

   unsigned int iCount = 0;
   //lint -e{826}
   ElementStruct *elemntPtr = reinterpret_cast<ElementStruct *>(m_elements);
   while ( (iCount < m_elementNo)
           &&
           //lint -e{613}
           (elemntPtr->inUse != 0) )
   {
      iCount++;
      //lint -e{826}
      elemntPtr = reinterpret_cast<ElementStruct *>(m_elements + iCount * (ElementHeaderSize + m_elementSize));
   }

   if (iCount >= m_elementNo)
   {
       Q_ASSERT(false);
       THROWARG (Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, "that must not occure");
//      THROW_Exception ("that must not occure");
   }

   //lint -e{794}
   getCallStack (elemntPtr->stackTrace);

   if (ptr)
		*reinterpret_cast<void**>(&(elemntPtr->elementBuffer)) = ptr;
   else
   {
	   //lint -e{794}
       memset (&(elemntPtr->elementBuffer),
               0X00,
//               0XFF,
               m_elementSize);
   }

   m_usage++;
   if (m_usage > m_usageMax)
   {
      m_usageMax = m_usage;
   }
   //lint -e{794}
   elemntPtr->inUse = 1;
   //lint -e{794}
   MemLeakDetection::MemLog::doLog (ptr ? ptr : &(elemntPtr->elementBuffer),
                                    m_elementSize,
                                    true);
   //lint -e{794}
   if (ptr)
	   return ptr;

   return &(elemntPtr->elementBuffer);
}

//lint -save -e613 -e953
void MemLeakDetection::Counter::free (const void* buffer)
{
   if (buffer == NULL)
   {
      return;
   }

   MemLeakDetection::MemLog::doLog (buffer,
                                    m_elementSize,
                                    false);

   if (buffer < m_elements)
   {
      MemLeakDetection::MemLog::doLogConflict (buffer,
                                               m_elementSize,
                                               "wrong pointer in deallocation");
      Q_ASSERT(false);
      THROWARG (Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, "wrong pointer in deallocation");
//      THROW_Exception ("wrong pointer in deallocation");
   }

   unsigned int offset = static_cast<unsigned int>(reinterpret_cast<const char*>(buffer) - m_elements);
   offset -= ElementHeaderSize;

   if ((offset % (m_elementSize + ElementHeaderSize)) != 0)
   {
      MemLeakDetection::MemLog::doLogConflict (buffer,
                                               m_elementSize,
                                               "wrong pointer in deallocation");
      return;
      Q_ASSERT(false);
      THROWARG (Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, "wrong pointer in deallocation");
      //THROW_Exception ("wrong pointer in deallocation");
   }

   //lint -e{826}
   ElementStruct *elemntPtr = reinterpret_cast<ElementStruct *>(m_elements + offset);

   if (elemntPtr->inUse == 0)
   {
      MemLeakDetection::MemLog::doLogConflict (buffer,
                                               m_elementSize,
                                               "pointer is not allocated");
      Q_ASSERT(false);
      THROWARG (Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, "pointer is not allocated");
      //THROW_Exception ("pointer is not allocated");
   }
  
#ifdef WIN32
   ScopedInternalMemoryLock lock (reinterpret_cast<CRITICAL_SECTION*>(m_section));
#else
    ScopedInternalMemoryLock lock (reinterpret_cast<pthread_mutex_t*>(m_section));
#endif

   elemntPtr->inUse = 0;
   m_usage--;
}

void* MemLeakDetection::Counter::search (const void* ptr)
{
	if (ptr == NULL)
	{
		return NULL;
	}

	//lint -e{826}
	ElementStruct* elemntPtr;
	for (unsigned int iCount = 0; iCount < m_elementNo; iCount++)
	{
        elemntPtr = reinterpret_cast<ElementStruct*>(m_elements + iCount * (ElementHeaderSize + m_elementSize));
        if (ptr == *reinterpret_cast<void**>(&(elemntPtr->elementBuffer)))
            return &(elemntPtr->elementBuffer);
	}
	return NULL;
}

//lint -restore

MemLeakDetection::MemLog::MemLog ()
   : m_position        (0U),
     m_criticalSection (NULL)
{
   memset (m_fileName,
           0,
           MAX_PATH);
//   memcpy(m_fileName, "/home/colorado/git/Colorado/ColoradoMain/Master/Components/Main/Build/bin_dbg/memleak.log", 89);
   memset (m_buffer,
           0,
           ElementSize32768Byte);
//   snprintf (m_fileName,
//             TextSize,
//             "%s%cMemLeakDetection_%p.log",
//             StackTraceUtility::getTempPath ().c_str (),
//             PATH_SYMBOL,
//             this);
   snprintf (m_fileName,
             TextSize,
             "./MemLeakDetection_%p.log",
             this);
#ifdef WIN32
   static CRITICAL_SECTION internalMutex;
   InitializeCriticalSection (&internalMutex);
   m_criticalSection = static_cast<void*>(&internalMutex);
#else
   static pthread_mutex_t internalMutex;
   //create mutex attribute variable
   pthread_mutexattr_t mAttr;

   // setup recursive mutex for mutex attribute
   pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP);

   // Use the mutex attribute to create the mutex
   pthread_mutex_init(&internalMutex, &mAttr);

   // Mutex attribute can be destroy after initializing the mutex variable
   pthread_mutexattr_destroy(&mAttr);

   m_criticalSection = static_cast<void*>(&internalMutex);
#endif
   s_instance = this;
}

MemLeakDetection::MemLog::~MemLog ()
{
   //lint -save -e1551
   flush ();
   s_instance = NULL;

#ifdef WIN32
   DeleteCriticalSection (static_cast<CRITICAL_SECTION*>(m_criticalSection));
#else
   pthread_mutex_destroy (static_cast<pthread_mutex_t*>(m_criticalSection));
#endif
   m_criticalSection = NULL;
   //lint -restore
}

void MemLeakDetection::MemLog::doLog (const void *ptr, size_t size, bool allocated)
{
   if (s_instance != NULL)
   {
      s_instance->log (ptr,
                       size,
                       allocated);
   }
}

void MemLeakDetection::MemLog::log (const void *ptr, size_t size, bool allocated)
{
#ifdef WIN32
    ScopedInternalMemoryLock lock (static_cast<CRITICAL_SECTION*>(m_criticalSection));
#else
    ScopedInternalMemoryLock lock (reinterpret_cast<pthread_mutex_t*>(m_criticalSection));
#endif

   const int printed = snprintf (m_buffer             + m_position,
                                 ElementSize32768Byte - m_position,
                                 "%s %p %u\n",
                                 allocated ? "+++" : "---",
                                 ptr,
                                 size);
   if (printed > 0)
   {
      m_position += static_cast<unsigned int>(printed);
   }
   if (ElementSize32768Byte - m_position < 128)
   {
      flush ();
   }
}

void MemLeakDetection::MemLog::doLogConflict (const void *ptr, size_t size, const char *comment)
{
   if (s_instance != NULL)
   {
      s_instance->logConflict (ptr,
                               size,
                               comment);
   }
}

void MemLeakDetection::MemLog::logConflict (const void *ptr, size_t size, const char *comment)
{
   const int printed = snprintf (m_buffer             + m_position,
                                 ElementSize32768Byte - m_position,
                                 "!!!!!!!! %p %u \"%s\"\n",
                                 ptr,
                                 size,
                                 comment);
   if (printed > 0)
   {
      m_position += static_cast<unsigned int>(printed);
      flush ();
   }
}

void MemLeakDetection::MemLog::flush ()
{
   FILE *logFile (fopen (m_fileName,
                         "a"));
   if (logFile != NULL)
   {
      fprintf (logFile,
               m_buffer);
      fclose (logFile);
   }
   m_position = 0U;
   memset (m_buffer,
           0,
           ElementSize32768Byte);
}
#endif
