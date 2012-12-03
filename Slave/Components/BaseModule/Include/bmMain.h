/****************************************************************************/
/*! \file bmMain.h
 *
 *  \brief Base modules main functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module initializes the base module and all function modules by
 *       calling their init functions.
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

#ifndef BM_MAIN_H
#define BM_MAIN_H

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Module initialization function structure
typedef struct {
    UInt16 ModuleID;                //!< Modules identifier
    UInt16 NumberOfInstances;       //!< Number of instances of a module
    bmModuleInit_t InitFunction;    //!< Modules initialization function
} bmModuleParameters_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t bmStartBaseModule (
                const bmModuleParameters_t *ModuleInitTable, UInt16 TableSize);

//****************************************************************************/

#endif /*BM_MAIN_H*/
