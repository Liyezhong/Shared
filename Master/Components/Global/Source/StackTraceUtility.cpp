///////////////////////////////////////////////////////////
//  StackTraceUtility.cpp
//  Implementation of the Class StackTraceUtility
//  Created on:      30-Sep-2009 18:02:43
//  Original author: Hans-Joerg Gruenthal
///////////////////////////////////////////////////////////

#include "Global/Include/StackTraceUtility.h"
#include "Global/Include/PosixWrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#include <dbghelp.h>
#include <Tlhelp32.h>
#endif
static const unsigned int SystemPathMaxLen = 4096U;

#ifdef WIN32
enum
{
   STACKWALK_MAX_NAMELEN = 1024, // max name length for found symbols
   SYMBOL_LENGTH = STACKWALK_MAX_NAMELEN + sizeof (IMAGEHLP_SYMBOL64)
};

#ifdef _DEBUG

static void loadModules (DWORD pid, HANDLE processHandle)
{
   MODULEENTRY32 moduleEntry;
   const HANDLE  hSnap = CreateToolhelp32Snapshot (TH32CS_SNAPMODULE,
                                                   pid);
   if (hSnap == INVALID_HANDLE_VALUE)
   {
      return;
   }
   moduleEntry.dwSize = sizeof(MODULEENTRY32);
   BOOL result = Module32First ( hSnap,
                                &moduleEntry);
   while (result)
   {
      const DWORD64 loadresult = SymLoadModule64 (processHandle,
                                                  NULL,
                                                  moduleEntry.szExePath,
                                                  moduleEntry.szModule,
                                                  reinterpret_cast<DWORD64>(moduleEntry.modBaseAddr),
                                                  moduleEntry.modBaseSize);
      if ( (loadresult == 0ULL)
           &&
           (GetLastError () != ERROR_SUCCESS) )
      {
         // no exception here
         return;
      }

      result = Module32Next ( hSnap,
                             &moduleEntry);
   }
   CloseHandle (hSnap);
}

static HANDLE retrieveProcessHandle ()
{
   char   szSymPath[SystemPathMaxLen];
   const DWORD  currentProcessId = GetCurrentProcessId ();
   const HANDLE processHandle    = OpenProcess (PROCESS_QUERY_INFORMATION,
                                                FALSE,
                                                currentProcessId);

   unsigned int usedBuffer = 0U;
   int printed = snprintf (szSymPath,
                           SystemPathMaxLen,
                           ".;");
   if (printed >= 0)
   {
      usedBuffer = static_cast<unsigned int>(printed);
   }

   DWORD result;
   if ((result = GetCurrentDirectory (SystemPathMaxLen - usedBuffer,
                                      szSymPath        + usedBuffer)) != 0)
   {
      usedBuffer += result;
      printed  = snprintf (szSymPath        + usedBuffer,
                           SystemPathMaxLen - usedBuffer,
                           ";");
      if (printed >= 0)
      {
         usedBuffer += static_cast<unsigned int>(printed);
      }
   }

   // path for main module
   if ((result = GetModuleFileName (NULL,
                                    szSymPath        + usedBuffer,
                                    SystemPathMaxLen - usedBuffer)) != 0)
   {
      const unsigned int oldPrinted = usedBuffer;
      usedBuffer += result;

      for (char *p = (szSymPath + (usedBuffer - 1));
           p != szSymPath + oldPrinted;
           --p)
      {
         // locate the rightmost path separator
         if ( (*p == '\\')
              ||
              (*p == '/')
              ||
              (*p == ':') )
         {
            *p = 0;
            usedBuffer--;
            break;
         }
      }  // for (search for path separator...)

      printed = snprintf (szSymPath        + usedBuffer,
                          SystemPathMaxLen - usedBuffer,
                          ";");
      if (printed >= 0)
      {
         usedBuffer += static_cast<unsigned int>(printed);
      }
   }

   if ((result = GetEnvironmentVariable ("_NT_SYMBOL_PATH",
                                         szSymPath        + usedBuffer,
                                         SystemPathMaxLen - usedBuffer)) != 0)
   {
      usedBuffer += result;
      printed  = snprintf (szSymPath        + usedBuffer,
                           SystemPathMaxLen - usedBuffer,
                           ";");
      if (printed >= 0)
      {
         usedBuffer += static_cast<unsigned int>(printed);
      }
   }

   if ((result = GetEnvironmentVariable ("_NT_ALTERNATE_SYMBOL_PATH",
                                         szSymPath        + usedBuffer,
                                         SystemPathMaxLen - usedBuffer)) != 0)
   {
      usedBuffer += result;
      printed  = snprintf (szSymPath        + usedBuffer,
                           SystemPathMaxLen - usedBuffer,
                           ";");
      if (printed >= 0)
      {
         usedBuffer += static_cast<unsigned int>(printed);
      }
   }

   char szTemp[MAX_PATH];
   if ((result = GetEnvironmentVariable ("SYSTEMROOT",
                                         szTemp,
                                         MAX_PATH)) > 0)
   {
      printed  = snprintf (szSymPath        + usedBuffer,
                           SystemPathMaxLen - usedBuffer,
                           "%s;%s\\system32;",
                           szTemp,
                           szTemp);
      if (printed >= 0)
      {
         usedBuffer += static_cast<unsigned int>(printed);
      }
   }

   if ((result = GetEnvironmentVariable ("SYSTEMDRIVE",
                                         szTemp,
                                         MAX_PATH)) > 0)
   {
      snprintf (szSymPath        + usedBuffer,
                SystemPathMaxLen - usedBuffer,
                "SRV*%s;%s\\websymbols*http://msdl.microsoft.com/download/symbols;",
                szTemp,
                szTemp);
   }

   if (SymInitialize (processHandle,
                      szSymPath,
                      FALSE) == FALSE)
   {
      return reinterpret_cast<HANDLE>(0XFFFFFFFF);
   }
   DWORD symOptions = SymGetOptions();
   symOptions |= SYMOPT_LOAD_LINES;
   symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
   static_cast<void>(SymSetOptions (symOptions));

   loadModules (currentProcessId,
                processHandle);

   return processHandle;
}
#endif

static HANDLE s_thisProcess = reinterpret_cast<HANDLE>(0XFFFFFFFF);


#endif // WIN32


//__attribute__((always_inline)) uintptr_t *get_ebp_inline(void)
//{
//    uintptr_t *r;
//    unsigned int currentEBP = 0U;
//    __asm__ volatile ("movl %%ebp, %0" : /* output */ "=r"(currentEBP));
//    return r;
//}

unsigned int StackTraceUtility::getCurrentEBP ()
{
   //lint -save -e953
   unsigned int currentEBP = 0U;
#ifdef __GNUC__
//   __asm__ volatile ("movl %%ebp, %0" : /* output */ "=r"(currentEBP));
//   __asm__ __volatile__ ("movl %%ebp %0" : "=r"(currentEBP));
#elif defined WIN32
   __asm
   {
      mov currentEBP, ebp
   }
#endif
   return currentEBP;
   //lint -restore
}

void StackTraceUtility::getStackTrace(unsigned int ebp, unsigned int* instructionPointers, size_t vectorSize )
{
   memset (instructionPointers,
           0,
           vectorSize * sizeof (unsigned int));
   unsigned int ebpIterator = ebp;

   unsigned int count = 0U;
   while ( (ebpIterator != 0)
           &&
           (count < vectorSize) )
   {
      const unsigned int &oldEbp (*reinterpret_cast<unsigned int*>(ebpIterator));
      const unsigned int &oldEip (*reinterpret_cast<unsigned int*>(ebpIterator + 4));
      instructionPointers[count] = oldEip;
      count++;
      ebpIterator = oldEbp;
   }
}

void StackTraceUtility::initSymbolTable ()
{
#ifdef WIN32
#ifdef _DEBUG
   s_thisProcess = retrieveProcessHandle ();
#endif
#endif
}

const char* StackTraceUtility::getSymbolName(unsigned int instruction, char* symbolName, size_t bufferSize)
{
#ifdef WIN32
#ifdef _DEBUG

   char symbolChar[SYMBOL_LENGTH];
   memset ( symbolChar,
            0,
            SYMBOL_LENGTH);
   
   IMAGEHLP_SYMBOL64 &symbol (*reinterpret_cast<IMAGEHLP_SYMBOL64*>(symbolChar));

   symbol.SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL64);
   symbol.MaxNameLength = STACKWALK_MAX_NAMELEN;
   DWORD64 offsetFromSmybol = 0ULL;

   if (SymGetSymFromAddr64 ( s_thisProcess,
                             DWORD64(instruction),
                            &offsetFromSmybol,
                            &symbol) != FALSE)
   {
      const DWORD printed = UnDecorateSymbolName ( symbol.Name,
                                                   symbolName,
                                                   bufferSize,
                                                   UNDNAME_COMPLETE);
      snprintf (symbolName + printed,
                bufferSize - printed,
                " + %u bytes; ",
                offsetFromSmybol);
   }
   else
#endif //_DEBUG
#endif //WIN32
   {
      snprintf (symbolName,
                bufferSize,
                "unknown symbol 0X%.8X",
                instruction);
   }

   return symbolName;
}

std::string StackTraceUtility::getTempPath()
{
    //lint -save -e953
   const char *tmp  = getenv ("TMP");

   if (tmp != NULL)
   {
      return tmp;
   }

   const char *temp = getenv ("TEMP");
   if (temp != NULL)
   {
      return temp;
   }

   return ".";
}
//lint -restore

void StackTraceUtility::getStackTrace(unsigned int* instructionPointers, size_t vectorSize)
{
   //lint -save -e613 -e681
   memset (instructionPointers,
           0,
           vectorSize * sizeof (unsigned int));
   unsigned int ebpIterator = 0U;

#ifdef __GNUC__
//   __asm__ __volatile__ ("movl %%ebp %0" : "=r"(ebpIterator));
#elif defined WIN32
   __asm
   {
      mov ebpIterator, ebp
   }
#endif

   unsigned int count = 0U;
   while ( (ebpIterator != 0)
           &&
           (count < vectorSize) )
   {
      const unsigned int &oldEbp (*reinterpret_cast<unsigned int*>(ebpIterator));
      const unsigned int &oldEip (*reinterpret_cast<unsigned int*>(ebpIterator + 4));
      instructionPointers[count] = oldEip;
      count++;
      ebpIterator = oldEbp;
   }
   //lint -restore
}
