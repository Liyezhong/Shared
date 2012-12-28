/****************************************************************************/
/*! \file bmPermData.h
 *
 *  \brief Permanent data handling module
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to handle permanent (persistent)
 *       data. It implements a level above the storage module and offers
 *       a more comfortable interface to the non-volatile storage.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef BM_PERMDATA_H
#define BM_PERMDATA_H


//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//! Macro to build a non-volatile storage parameter identifier
#define BUILD_PARAMETER(addr,size)  (((addr) << 4) | ((size) & 0x0F))

//! Macro to extract address from non-volatile storage parameter identifier
#define GET_PARAM_ADDRESS(id)       ((id) >> 4)

//! Macro to extract size from non-volatile storage parameter identifier
#define GET_PARAM_SIZE(id)          ((id) & 15)


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

 //! storage parameter descriptor to be used for verifying/reseting
typedef struct {
    UInt16 Address;             //!< Storage address identifier
    UInt32 MinValue;            //!< Minimal valid value of this parameter
    UInt32 MaxValue;            //!< Maximal valid value of this parameter
    UInt32 Default;             //!< Default value of this parameter
} bmParamRange_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Handle_t bmOpenPermStorage  (UInt16 ModuleID, UInt16 Instance, UInt16 Size);
Handle_t bmClosePermStorage (Handle_t Handle);

Error_t bmResetStorageItems (
              Handle_t Handle, const bmParamRange_t *Ranges, UInt16 Size);

Bool bmVerifyStorageItems (
              Handle_t Handle, const bmParamRange_t *Ranges, UInt16 Size);

UInt32  bmGetStorageItem (Handle_t Handle, UInt32 ParamID, UInt32 Default);
Error_t bmSetStorageItem (Handle_t Handle, UInt32 ParamID, UInt32 Value);
Error_t bmIncStorageItem (Handle_t Handle, UInt32 ParamID, UInt32 Value);
Error_t bmDecStorageItem (Handle_t Handle, UInt32 ParamID, UInt32 Count);

//****************************************************************************/

#endif /*BM_PERMDATA_H*/
