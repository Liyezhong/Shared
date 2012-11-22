/****************************************************************************/
/*! \file main.c
 * 
 *  \brief Main function module
 *
 *  $Version: $ 0.1
 *  $Date:    $ 04.08.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains the "main" function of the slave software,
 *       which is called after startup of the hardware. The main task
 *       of this module is, to specify all function modules required
 *       by this firmware and to call the base module start routine.
 *       All the rest is done by the base module in combination with
 *       the selected function modules.  
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

#include <string.h>
#include <stdlib.h>
#include "Global.h"
#include "bmCommon.h"
#include "Basemodule.h"
#include "bmMain.h"
#include "fmAnaInput.h"
#include "fmAnaOutput.h"
#include "fmDigiInput.h"
#include "fmDigiOutput.h"
#include "fmSimulate.h"
#include "fmJoystick.h"
#include "fmRfid.h"
#include "fmTemperature.h"
#include "fmUart.h"
#include "ModuleIDs.h"

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static const UInt32 TestOptionList[] = {
    BASEMODULE_MODULE_ID, 3, DEFAULT_BOARD_OPTIONS,  0x0BB805DC, 0x03000150
};

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

ERROR_t tstInitializeTestPatterns (UInt16 TestID);


/*****************************************************************************/
/*! 
 *  \brief   Function module descriptor table 
 *
 *      ModuleInitTable must contain a function pointer to the initialization 
 *      function of each function module used. Beside this information, the
 *      number of instances (incarnations) and the unique module identifier 
 *      of each module must be specified. If the number of instances is zero,
 *      the board options are checked for the number of module instances.
 *  
 ****************************************************************************/

static const MODULE_INITIALIZE_t ModuleInitTable[] = { 
    { MODULE_ID_DIGITAL_IN,  1, diInitializeModule  },
    { MODULE_ID_DIGITAL_OUT, 1, doInitializeModule  },
    { MODULE_ID_ANALOG_IN,   1, aiInitializeModule  },
    { MODULE_ID_ANALOG_OUT,  1, aoInitializeModule  }, 
    { MODULE_ID_SIMUALTION,  1, simInitializeModule },
    { MODULE_ID_JOYSTICK,    1, joysInitializeModule },
    { MODULE_ID_RFID,        1, rfidInitializeModule },
    { MODULE_ID_TEMPERATURE, 1, tempInitializeModule },
    { MODULE_ID_UART,        1, uartInitializeModule },
};
static const int NumberOfModules = ELEMENTS(ModuleInitTable);


/*****************************************************************************/
/*! 
 *  \brief   Define board option list
 *
 *      Defines a debug board option list. Normally, the board options
 *      are programmed during manufacturing external to this firmware.
 *      This is for debug only
 *  
 *  \return  Should never return
 *
 ****************************************************************************/

static ERROR_t InitTestBoardOptionBlock (void) {
    
    UInt16 Elements = ELEMENTS(TestOptionList);
    UInt16 i;
    
    UInt32 *BoardOptions = calloc (Elements+3, sizeof(UInt32));
    
    if (!isPtrNull(BoardOptions)) {
        
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


//****************************************************************************/

static ERROR_t InitTestBoardInfoBlock (void) {
    
    static BOARD_INFO_BLOCK_t InfoBlock;

    InfoBlock.Signature       = INFOBLOCK_SIGNATURE;
    InfoBlock.NodeType        = 0x12;
    InfoBlock.NodeClass       = 0;

    strcpy (InfoBlock.BoardName, "ASB99");
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
        &InfoBlock, sizeof(BOARD_INFO_BLOCK_t) - sizeof(UInt32));

    return (halSetAddress (ADDRESS_BOARD_HARDWARE_INFO, &InfoBlock));
}

//****************************************************************************/

static ERROR_t InitTestBootloaderInfoBlock (void) {
    
    static BOOT_INFO_BLOCK_t BootBlock;
    
    BootBlock.Signature       = INFOBLOCK_SIGNATURE;
    BootBlock.VersionMajor    = 1;
    BootBlock.VersionMinor    = 1;
    BootBlock.CreationYear    = 10;
    BootBlock.CreationMonth   = 7;
    BootBlock.CreationDay     = 12;    
    
    BootBlock.Checksum = bmCalculateCrc (
        &BootBlock, sizeof(BOOT_INFO_BLOCK_t) - sizeof(UInt32));
        
    return (halSetAddress (ADDRESS_BOOTLOADER_INFO, &BootBlock));        
}


/*****************************************************************************/
/*! 
 *  \brief   Firmware's main function
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
 *  \return  Should never return
 *
 ****************************************************************************/

int main (int argc, char **argv)
{
    ERROR_t Status;
    
    // this is for debugging only (tables are later defined externally)
    InitTestBoardInfoBlock();
    InitTestBootloaderInfoBlock();
    InitTestBoardOptionBlock();

    tstInitializeTestPatterns(16);
    
    // this function call should never return
    Status = bmStartBaseModule (ModuleInitTable, NumberOfModules);
    
    if (Status < 0) {
        dbgPrintError (BASEMODULE_CHANNEL, Status, ON, 0);
    }
    dbgPrint ("Firmware stopped");
}

//****************************************************************************/
