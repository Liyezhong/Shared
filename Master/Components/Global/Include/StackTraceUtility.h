///////////////////////////////////////////////////////////
//  StackTraceUtility.h
//  Implementation of the Class StackTraceUtility
//  Created on:      30-Sep-2009 18:02:43
//  Original author: Hans-Joerg Gruenthal
///////////////////////////////////////////////////////////

#if !defined(EA_B72370F1_CA51_4b80_A03B_4C1980918E44__INCLUDED_)
#define EA_B72370F1_CA51_4b80_A03B_4C1980918E44__INCLUDED_

#include <string>

/**
 * \brief a class utility for determining call stack information
 */
class StackTraceUtility
{

public:
	/**
	 * \rief returns current base pointer EBP
	 */
    static unsigned int getCurrentEBP   ();
    /**
	 * \rief symbol table initialization; must be called on system start by global deploy method only
	 */
	static void         initSymbolTable ();
	/**
	 * \rief determine call stack given by base pointer; memory for instruction pointers must be provided
	 */
	static void         getStackTrace   (unsigned int ebp, unsigned int* instructionPointers, size_t vectorSize );
	/**
	 * \rief determine current call stack; memory for instruction pointers must be provided
	 */
	static void         getStackTrace   (unsigned int* instructionPointers, size_t vectorSize);
	/**
	 * \rief returns symbol information concerning given instruction; symbol name space must be provided
	 */
	static const char*  getSymbolName   (unsigned int instruction, char* symbolName, size_t bufferSize);
	/**
	 * \brief returns a path i.e for temporary files; looks for $TMP environment variable; if
	 * $TMP is not set, "." will be returned
	 */
	static std::string getTempPath();
};
#endif // !defined(EA_B72370F1_CA51_4b80_A03B_4C1980918E44__INCLUDED_)
