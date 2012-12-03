/****************************************************************************/
/*! \file bmProcData.h
 *
 *  \brief Process data handling module
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions handling process data for all
 *       function modules on this node. The function modules only have to
 *       deliver their process data values via their status function.
 *       The rest is done by this module.
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

#ifndef BM_PROCDATA_H
#define BM_PROCDATA_H


//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t bmMonitorProcessData (UInt16 Channel, bmModuleState_t ModuleState);
Error_t bmInitProcessData    (UInt16 MaxTaskCount);

//****************************************************************************/

#endif /*BM_PROCDATA_H*/
