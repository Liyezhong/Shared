/****************************************************************************/
/*! \file fmSimulate.h
 * 
 *  \brief Function module to simulate hardware input patterns
 *
 *  $Version: $ 0.1
 *  $Date:    $ 06.08.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *         This module simulates, in combination with the HAL simulation,
 *         data input from hardware. It can simulate CAN receive messages
 *         and digital/analog data input. Data patterns can be defined
 *         including data/messages over time. Several test cases are
 *         defined, to test several aspects of the slave software. 
 *         More tests can be added.
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

#ifndef FM_SIMULATE_H
#define FM_SIMULATE_H

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

typedef struct {
    UInt16 Delay;
    UInt16 Channel;
    CanMessage_t Message;
} CanLoopbackData_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

UInt16  simImportCanTestPatterns (char *Pathname);
UInt16  simWriteTestPatterns (const CanLoopbackData_t *Patterns, UInt16 Count);
Error_t simInitializeModule (UInt16 ModuleID, UInt16 NumberOfInstances);
Error_t simCanRegisterTestMessages (
          const CanLoopbackData_t *TestDataTable, UInt16 TableSize);

//****************************************************************************/

#endif /*FM_SIMULATE_H*/
