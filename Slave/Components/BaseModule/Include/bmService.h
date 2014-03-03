/****************************************************************************/
/*! \file bmService.h
 *
 *  \brief Service commands handling module
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions handling service commands from
 *       the master. The master can request the following informations:
 *
 *       - Serial number
 *       - End-test informations (date and test result)
 *       - Hardware informations (version number, production date)
 *       - Software informations (version number, creation date)
 *       - Bootloader informations (version number, creation date)
 *       - Life cycle counters
 *
 *       Additionally, the following actions can be initiated by the
 *       master using special service commands:
 *
 *       - Reset all data in non-volatile storage to default
 *       - Format non-volatile storage
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

#ifndef BM_SERVICE_H
#define BM_SERVICE_H

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t bmUpdateLifeTimeCounter (void);
Error_t bmResetPartition        (void);
Error_t bmInitServiceModule     (void);

//****************************************************************************/

#endif /*BM_SERVICE_H*/
