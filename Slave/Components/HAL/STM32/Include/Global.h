/****************************************************************************/
/*! \file Global.h
 *
 *  \brief Global Project Definitions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains global project definitions.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H


//****************************************************************************/
// Error Class Definitions
//****************************************************************************/

#define NO_ERROR             0             //!< No error 

#define ERRCODE_MASK_CLASS   0x70000000L   //!< Bitmask for event class
#define ERRCODE_MASK_MODULE  0x0FFF0000L   //!< Bitmask for module ID
#define ERRCODE_MASK_NUMBER  0x00003FFFL   //!< Bitmask for event number

#define ERRCLASS_INFO        0x80000000L   //!< Event class Info
#define ERRCLASS_ERROR       0x90000000L   //!< Event class Error
#define ERRCLASS_WARNING     0xA0000000L   //!< Event class Warning
#define ERRCLASS_FATAL       0xB0000000L   //!< Event class Fatal Error

//! Macro to construct an error code from ModuleID, ErrorClass, ErrCode
#define  BUILD_ERRCODE(ModuleID,Class,Code) (Error_t)( \
          (((UInt32)(ModuleID) << 16) & ERRCODE_MASK_MODULE) | (Class) | (Code))

//****************************************************************************/
// Global Project Definitions
//****************************************************************************/

#define FALSE                0             //!< Boolean FALSE constant
#define TRUE                 1             //!< Boolean TRUE constant
#ifndef NULL
#define NULL                 (void*)0      //!< NULL constant (pointers)
#endif

typedef unsigned long long   UInt64;       //!< 64 bit unsigned integer type
typedef unsigned long        UInt32;       //!< 32 bit unsigned integer type
typedef unsigned short       UInt16;       //!< 16 bit unsigned integer type
typedef unsigned char        UInt8;        //!< 8 bit unsigned integer type

typedef signed long long     Int64;        //!< 64 bit signed integer type
typedef signed long          Int32;        //!< 32 bit signed integer type
typedef signed short         Int16;        //!< 16 bit signed integer type
typedef signed char          Int8;         //!< 8 bit signed integer type

#define Error_t              Int32         //!< Error code type
typedef Int32                Handle_t;     //!< Handle type
typedef UInt8                Bool;         //!< Boolean type

#define MAX_UINT32           0xFFFFFFFF    //!< Max. value of an UInt32
#define MAX_UINT16           0xFFFF        //!< Max. value of an UInt16
#define MAX_UINT8            0xFF          //!< Max. value of an UInt8

#define MAX_INT32            0x7FFFFFFF    //!< Max. value of an Int32
#define MAX_INT16            0x7FFF        //!< Max. value of an Int16
#define MAX_INT8             0x7F          //!< Max. value of an Int8

#define MIN_INT32            -MAX_INT32-1  //!< Min. value of an Int32
#define MIN_INT16            -MAX_INT16-1  //!< Min. value of an Int16
#define MIN_INT8             -MAX_INT8-1   //!< Min. value of an Int8

#define ON                   TRUE          //!< Alias for TRUE
#define OFF                  FALSE         //!< Alias for FALSE

#ifndef                      inline
#define inline               __inline      //!< Make inline portable
#endif

//****************************************************************************/
// Global Project Macros
//****************************************************************************/

//! Macro to determine the number of elements in an array
#define ELEMENTS(t)          (sizeof(t)/sizeof(*t))

//! Macro to determine the lesser of two values
#define MIN(x,y)             ((x) < (y) ? (x):(y))

//! Macro to determine the greater of two values
#define MAX(x,y)             ((x) > (y) ? (x):(y))

//! Macro to create a bitmask with bit 'n' set
#define BIT(n)               (1UL << (n))

//****************************************************************************/

#endif /*GLOBAL_H*/
