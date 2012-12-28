/****************************************************************************/
/*! \file halConsole.c
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

#include <stdio.h>
#include "Global.h"
#include "halCommon.h"
#include "halInterrupt.h"
#include "halSerial.h"
#include "halError.h"
#include "halMain.h"
#include "halConsole.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define ITM_RXBUFFER_EMPTY  0x5AA55AA5   //!< ITM RxBuffer empty indicator
#define ITM_TCR_ENABLE      0x00000001   //!< Enable ITM stimulus
#define ITM_TER_ENABLE      0x00000001   //!< Enable ITM stimulus

#define MCU_DBG_SLEEP       0x00000001   //!< Debug Sleep mode
#define MCU_DBG_STOP        0x00000002   //!< Debug Stop mode
#define MCU_DBG_STANDBY     0x00000004   //!< Debug Standby mode
#define MCU_DBG_TRACE_IOEN  0x00000020   //!< Trace pin assignment control
#define MCU_DBG_TRACE_MODE  0x000000C0   //!< Trace pin assignment control
#define MCU_DBG_IWDG_STOP   0x00000100   //!< Independent watchdog stop enable
#define MCU_DBG_WWDG_STOP   0x00000200   //!< Windowed watchdog stop enable
#define MCU_DBG_TIM1_STOP   0x00000400   //!< Timer 1 counter stop enable
#define MCU_DBG_TIM2_STOP   0x00000800   //!< Timer 2 counter stop enable
#define MCU_DBG_TIM3_STOP   0x00001000   //!< Timer 3 counter stop enable
#define MCU_DBG_TIM4_STOP   0x00002000   //!< Timer 4 counter stop enable
#define MCU_DBG_TIM5_STOP   0x00040000   //!< Timer 5 counter stop enable
#define MCU_DBG_TIM6_STOP   0x00080000   //!< Timer 6 counter stop enable
#define MCU_DBG_TIM7_STOP   0x00100000   //!< Timer 7 counter stop enable
#define MCU_DBG_TIM8_STOP   0x00020000   //!< Timer 8 counter stop enable
#define MCU_DBG_CAN1_STOP   0x00004000   //!< CAN 1 stop enable
#define MCU_DBG_CAN2_STOP   0x00200000   //!< CAN 2 stop enable
#define MCU_DBG_I2C1_STOP   0x00008000   //!< I2C 1 stop enable
#define MCU_DBG_I2C2_STOP   0x00010000   //!< I2C 2 stop enable

#define MCU_IDCODE_DEVID    0x00000FFF   //!< Device identifier
#define MCU_IDCODE_REVID    0xFFFF0000   //!< Revision identifier

#define DBG_TRC_ENABLE      BIT(24)      //!< DEMCR TRCENA enable
                                         

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Cortex internal ITM trace register layout
typedef volatile struct {
    UInt32 TER;             //!< ITM Trace Enable Register
    UInt32 Reserved1[15];   //!< Reserved
    UInt32 TPR;             //!< ITM Trace Privilege Register
    UInt32 Reserved2[15];   //!< Reserved
    UInt32 TCR;             //!< ITM Trace Control Register
} TrcRegFile_t;

//! Cortex internal core debug register layout
typedef volatile struct {
    UInt32 DHCSR;           //!< Halting Control and Status Register
    UInt32 DCRSR;           //!< Core Register Selector Register
    UInt32 DCRDR;           //!< Core Register Data Register
    UInt32 DEMCR;           //!< Exception and Monitor Control Register
} DbgRegFile_t;

//! Cortex internal ITM stimulus register layout
typedef volatile struct {
    //! ITM port widths
    union Ports {
        UInt8  BYTE;        //!< ITM Stimulus Port 8-bit
        UInt16 WORD;        //!< ITM Stimulus Port 16-bit
        UInt32 DWORD;       //!< ITM Stimulus Port 32-bit
    } PORT [32];            //!< ITM Stimulus Port Registers
} ItmRegFile_t;

//! Cortex internal Debug MCU configuration register
typedef volatile struct {
    UInt32 IDCODE;          //!< MCU device ID code
    UInt32 CR;              //!< Debug MCU configuration register
} McuRegFile_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Register file addresses for core debug registers
static DbgRegFile_t *DBG = (DbgRegFile_t*) 0xE000EDF0;

//! Register file addresses for the ITM stimulus registers
static ItmRegFile_t *ITM = (ItmRegFile_t*) 0xE0000000;

//! Register file addresses for MCU configuration register
static McuRegFile_t *MCU = (McuRegFile_t*) 0xE0042000;

//! This variable must be global and can't be renamed!
volatile UInt32 ITM_RxBuffer = ITM_RXBUFFER_EMPTY;

static Bool AutoEcho = TRUE;    //!< Select auto echo on/off


/*****************************************************************************/
/*!
 *  \brief   Write a character to the console
 *
 *      Outputs a character via the ITM channel 0 to be displayed in the
 *      remote development environements console window. Returns the number
 *      of characters actually written, which can be 0 or 1.
 *
 *      It returns if no debugger is connected that has booked the channel.
 *      It is blocking when a debugger is connected, but the previous
 *      character send is not transmitted.
 *
 *  \iparam  Data = Character to write
 *
 *  \return  Number of characters written [0, 1]
 *
 ****************************************************************************/

Error_t halConsoleWrite (UInt8 Data) {

//    if ((DBG->DEMCR & DBG_TRC_ENABLE) &&
//        (TRC->TCR   & ITM_TCR_ENABLE) &&
//        (TRC->TER   & ITM_TER_ENABLE)) {

    if (DBG->DEMCR & DBG_TRC_ENABLE) {

        while (!ITM->PORT[0].DWORD);

        ITM->PORT[0].BYTE = Data;
        return (1);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Read a character from the console
 *
 *      Reads a character via the ITM channel 0 from the remote development
 *      environements console window. The function is non-blocking. If no 
 *      character is available 0 is returned. 
 *
 *      It uses a global variable for communication. The debugger directly
 *      modifies this variable to deliver a new character.
 *
 *  \return  Read character or 0 if no character available
 *
 ****************************************************************************/

Error_t halConsoleRead (void) {

    if (ITM_RxBuffer != ITM_RXBUFFER_EMPTY) {

        UInt8 InByte = (UInt8) (ITM_RxBuffer & MAX_UINT8);
        ITM_RxBuffer = ITM_RXBUFFER_EMPTY;

        return (InByte);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Read string from the console
 *
 *      Reads a string from the remote development environements console 
 *      window. The null terminated string is returned in Buffer. The 
 *      function returns when a newline character is received or Size 
 *      characters are read.
 *
 *      Attention: This function blocks until the next carriage character
 *      is received via console.
 *
 *  \iparam  Buffer = Buffer to return read data
 *  \iparam  Size   = Size of Buffer
 *
 *  \return  Number of characters read
 *
 ****************************************************************************/

UInt32 halConsoleGets (char *Buffer, UInt32 Size) {

    UInt32 Count = 0;
    UInt32 inKey;

    if (Buffer != NULL && Size > 0) {
        do {
            do {
                inKey = halConsoleRead();
            } while (!inKey);

            if (AutoEcho) {
                if (inKey == '\r') {
                    halConsoleWrite('\n');
                }
                else {
                    halConsoleWrite(inKey);
                }
            }
            if (inKey != '\r') {
                Buffer[Count++] = inKey;
            }
        } while (inKey != '\r' && Count < Size);

        if (Count < Size) {
            Buffer[Count] = '\0';
        }
    }
    return (Count);    
}


/*****************************************************************************/
/*!
 *  \brief   Write string to the console
 *
 *      Writes a string to the remote development environements console 
 *      window. The null terminated string must be given in Buffer. 
 *
 *      Attention: This function blocks until all characters are written.
 *
 *  \iparam  Buffer = Buffer containing data to write
 *
 *  \return  Number of characters written
 *
 ****************************************************************************/

UInt32 halConsolePuts (char *Buffer) {

    UInt32 Count = 0;

    if (Buffer != NULL) {
        while (Buffer[Count] != '\0') {

            if (halConsoleWrite(Buffer[Count])) {
                Count++;
            }
        }
    }
    return (Count);    
}


/*****************************************************************************/
/*!
 *  \brief   Get device ID code and revision
 *
 *      Returns the processor device ID code and the revision. If revision
 *      is not needed, Revision can be NULL.
 *
 *      Attention: According to errata sheet, this function works in debug
 *      mode only. It shouldn't be used therefore in release mode.
 *
 *  \iparam  Revision = Pointer to return the revision
 *
 *  \return  Device ID code
 *
 ****************************************************************************/

Error_t halGetDeviceID (UInt16 *Revision) {

    if (Revision != NULL) {
        *Revision = (MCU->IDCODE & MCU_IDCODE_REVID) >> 16;
    }
    return (MCU->IDCODE & MCU_IDCODE_DEVID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize the debug console
 *
 *      Initializes the debug console, by reopening the standard i/o's.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halConsoleInit (void) {

    if (halGetInitState() == INIT_IN_PROGRESS) {

        MCU->CR |= MCU_DBG_SLEEP | MCU_DBG_STOP | MCU_DBG_STANDBY | MCU_DBG_TRACE_IOEN;
    
        #ifdef DEBUG
        freopen("itm", "w", stdout);
        freopen("itm", "r", stdin);
        #endif
    }
    return (NO_ERROR);
}

//****************************************************************************/
