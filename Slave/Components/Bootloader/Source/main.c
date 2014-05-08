/****************************************************************************/
/*! \file main.c
 *
 *  \brief Main function module for the Bootloader
 *
 *  $Version: $ 0.1
 *  $Date:    $ 10.10.2011
 *  $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *       This module contains the "main" function of the bootloader,
 *       which is called after startup of the hardware. The main task
 *       of this module is to call the bootloader start routine.
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

#include <stdlib.h>
#include <string.h>
#include "blMain.h"
#include "bmCan.h"
#include "bmCommon.h"
#include "bmUtilities.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define DEFAULT_NODE_INDEX      0       //!< Node index (if no DIP switch)

#define VOLTAGE_LIMIT_WARNING   21600   //!< Supply voltage warning limit [mV]
#define VOLTAGE_LIMIT_FAILURE   21000   //!< Supply voltage error limit [mV]
#define CURRENT_LIMIT_WARNING   300     //!< Supply current warning limit [mA]
#define CURRENT_LIMIT_FAILURE   400     //!< Supply current error limit [mA]

//****************************************************************************/
//! Board options of the base module
//****************************************************************************/

static const UInt32 BASEMODULE_OPTIONS = (
        OPTION_IDENTIFICATION  |        // Enable identification phase 
        OPTION_NODE_INDEX_DIP  |        // Enable node index DIP switch
        OPTION_STATUS_LED      |        // Enable onboard status/error LEDs
        OPTION_LIFECYCLE_DATA  |        // Enable life cycle data collection
        OPTION_PROCESS_DATA    |        // Enable process data support
        OPTION_TRIGGER_EVENTS  |        // Enable trigger event support
        OPTION_TASK_STATISTICS |        // Enable statistics collection
        OPTION_VOLTAGE_MONITOR |        // Enable supply voltage monitor
        OPTION_CURRENT_MONITOR |        // Enable supply current monitor
        OPTION_INFO_MESSAGES            // Enable info messages on startup
);

//****************************************************************************/
//! Board options list for all modules
//****************************************************************************/

static const UInt32 TestOptionList[] = {
    // Base modules board options
    BASEMODULE_MODULE_ID, 6, 
        BASEMODULE_OPTIONS,             // Base module option bits
        DEFAULT_NODE_INDEX,             // Node index (if no DIP switch)
        VOLTAGE_LIMIT_WARNING,          // Voltage monitor warning level
        VOLTAGE_LIMIT_FAILURE,          // Voltage monitor failure level
        CURRENT_LIMIT_WARNING,          // Current monitor warning level
        CURRENT_LIMIT_FAILURE,          // Current monitor failure level
};


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t InitTestBootloaderInfoBlock (void);
static Error_t InitTestBoardOptionBlock (void);
static Error_t InitTestBoardInfoBlock (void);


/*****************************************************************************/
/*!
 *  \brief   Define board option list
 *
 *      Defines a default board option list. Normally, the board options
 *      are programmed during manufacturing external to this firmware.
 *      This is for fallback only.
 *
 *  \return  NO_ERROR
 *
 ****************************************************************************/

static Error_t InitTestBoardOptionBlock (void) {

    UInt16 Elements = ELEMENTS(TestOptionList);
    UInt16 i;

    //lint -esym(429, BoardOptions)
    UInt32 *BoardOptions = calloc (Elements+3, sizeof(UInt32));

    if (BoardOptions != NULL) {

        BoardOptions[0] = INFOBLOCK_SIGNATURE;
        BoardOptions[1] = Elements + 2;

        for (i=0; i < Elements; i++) {
            BoardOptions[2+i] = TestOptionList[i];
        }
        BoardOptions[2+i] =
            bmCalculateCrc (BoardOptions, sizeof(UInt32) * (Elements+2));

        halSetAddress (ADDRESS_BOARD_OPTION_BLOCK, BoardOptions);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Define board info block
 *
 *      Defines the default board information block. Normally, the board
 *      information block is programmed during manufacturing external to this
 *      firmware. This is for fallback only.
 *
 *  \return  NO_ERROR
 *
 ****************************************************************************/

static Error_t InitTestBoardInfoBlock (void) {

    static bmBoardInfoBlock_t InfoBlock;

    InfoBlock.Signature       = INFOBLOCK_SIGNATURE;
    InfoBlock.NodeType        = 15;
    InfoBlock.NodeClass       = 0;

    strcpy (InfoBlock.BoardName, "ASB0");
    strcpy (InfoBlock.SerialNum, "SN:0307.12345.AB/9-4");

    InfoBlock.VersionMajor    = 0;
    InfoBlock.VersionMinor    = 1;

    InfoBlock.ProductionYear  = 10;
    InfoBlock.ProductionMonth = 7;
    InfoBlock.ProductionDay   = 25;

    InfoBlock.EndTestYear     = 10;
    InfoBlock.EndTestMonth    = 7;
    InfoBlock.EndTestDay      = 1;
    InfoBlock.EndTestResult   = 1;

    InfoBlock.Checksum = bmCalculateCrc (
        &InfoBlock, sizeof(bmBoardInfoBlock_t) - sizeof(UInt32));

    halSetAddress (ADDRESS_BOARD_HARDWARE_INFO, &InfoBlock);
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Define boot loader info block
 *
 *      Defines the default boot loader information block. Normally, the boot
 *      loader information block is programmed during manufacturing external
 *      to this firmware. This is for fallback only.
 *
 *  \return  NO_ERROR
 *
 ****************************************************************************/

static Error_t InitTestBootloaderInfoBlock (void) {

    static bmBootInfoBlock_t BootBlock;

    BootBlock.Signature       = INFOBLOCK_SIGNATURE;
    BootBlock.VersionMajor    = 1;
    BootBlock.VersionMinor    = 1;
    BootBlock.CreationYear    = 10;
    BootBlock.CreationMonth   = 7;
    BootBlock.CreationDay     = 12;

    BootBlock.Checksum = bmCalculateCrc (
        &BootBlock, sizeof(bmBootInfoBlock_t) - sizeof(UInt32));

    halSetAddress (ADDRESS_BOOTLOADER_INFO, &BootBlock);
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Bootloader's main function
 *
 *      This is the main function, which is called by the startup code after
 *      reset of the hardware. Its main purpose is to start the base module,
 *      which is some kind of operating system for the function modules.
 *      The base module does all the initialization of itself, the hardware
 *      abstraction layer, and the function modules. The initialization
 *      function pointers in the ModuleInitTable are used to achieve this.
 *      After initialization the base module calls the task functions of
 *      the function modules in a regular interval.
 *
 *  \iparam  argc = Argument counter
 *  \iparam  argv = Argument vector
 *
 *  \return  Should never return
 *
 ****************************************************************************/

int main (int argc, char **argv)
{
    // this is for debugging only (tables are later defined externally)
    InitTestBoardInfoBlock ();
    InitTestBootloaderInfoBlock ();
    InitTestBoardOptionBlock ();

    // this function call should never return
    blStartBootloader ();
    return (0);
}

//****************************************************************************/
