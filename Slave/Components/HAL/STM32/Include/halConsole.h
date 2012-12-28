/****************************************************************************/
/*! \file halConsole.h
 *
 *  \brief  Console I/O (for debug)
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to read and write to the console.
 *      The console is connected via cortex internal ITM hardware debug
 *      module. The emulator (i.e. ULINK2) must be configured to SW mode
 *      and ITM trace port 0 must be enabled.
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

#ifndef HAL_CONSOLE_H
#define HAL_CONSOLE_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

#define DEVICE_F100_MEDIUM   0x420   //!< Low/medium density value line
#define DEVICE_F100_HIGH     0x428   //!< High density value line
#define DEVICE_F10x_MEDIUM   0x410   //!< Medium density performance line
#define DEVICE_F10x_HIGH     0x414   //!< High density performance line
#define DEVICE_F10x_LOW      0x412   //!< Low density performance line
#define DEVICE_F10x_XL       0x430   //!< XL density performance line
#define DEVICE_F105          0x418   //!< Connectivity line devices

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halGetDeviceID  (UInt16 *Revision);
UInt32  halConsoleGets  (char *Buffer, UInt32 Size);
UInt32  halConsolePuts  (char *Buffer);
Error_t halConsoleWrite (UInt8 Data);
Error_t halConsoleRead  (void);
Error_t halConsoleInit  (void);

//****************************************************************************/

#endif
