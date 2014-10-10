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
     * \brief returns current base pointer EBP
     * \return current EBP
	 */
    static unsigned int getCurrentEBP();

    /**
     * \brief symbol table initialization; must be called on system start by global deploy method only
	 */
    static void initSymbolTable ();
	/**
     * \brief determine call stack given by base pointer; memory for instruction pointers must be provided
     * \iparam ebp ebp
     * \iparam instructionPointers instructionPointers
     * \iparam vectorSize vectorSize
	 */
    static void getStackTrace   (unsigned int ebp, unsigned int* instructionPointers, size_t vectorSize );

    /**
     * \brief determine current call stack; memory for instruction pointers must be provided
     * \iparam instructionPointers instructionPointers
     * \iparam vectorSize vectorSize
	 */
    static void getStackTrace(unsigned int* instructionPointers, size_t vectorSize);

    /**
     * \brief returns symbol information concerning given instruction; symbol name space must be provided
     * \iparam instruction instruction
     * \iparam symbolName symbolName
     * \iparam bufferSize bufferSize
     * \return symbol name
	 */
    static const char*  getSymbolName   (unsigned int instruction, char* symbolName, size_t bufferSize);
	/**
	 * \brief returns a path i.e for temporary files; looks for $TMP environment variable; if
	 * $TMP is not set, "." will be returned
     * \return temp path
	 */
	static std::string getTempPath();
};
#endif // !defined(EA_B72370F1_CA51_4b80_A03B_4C1980918E44__INCLUDED_)
