/****************************************************************************/
/*! \file bmService.c
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
 *       - Hardware informations (version number, production date)
 *       - Software informations (version number, creation date)
 *       - Bootloader informations (version number, creation date)
 *       - End-test results (date and test result)
 *       - Life cycle counters
 *
 *       Additionally, the following actions can be initiated by the
 *       master using special service commands:
 *
 *       - Format non-volatile storage
 *       - Reset non-volatile data to defaults
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
#include "Global.h"
#include "halLayer.h"
#include "bmCommon.h"
#include "bmError.h"
#include "bmTime.h"
#include "bmScheduler.h"
#include "bmUtilities.h"
#include "bmCan.h"
#include "bmDispatch.h"
#include "bmPermData.h"
#include "bmSystem.h"
#include "bmStorage.h"
#include "bmDebug.h"
#include "bmService.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

//@{ Start of doxygen group
//!  Definition of non-volatile parameter used by basemodule
#define PARAM_LAYOUT_VERSION    BUILD_PARAMETER(0,2)
#define PARAM_LAUNCH_STATE      BUILD_PARAMETER(2,2)
#define PARAM_LAUNCH_YEAR       BUILD_PARAMETER(4,2)
#define PARAM_LAUNCH_MONTH      BUILD_PARAMETER(6,2)
#define PARAM_LAUNCH_DAY        BUILD_PARAMETER(8,2)
#define PARAM_OPERATION_TIME    BUILD_PARAMETER(10,4)
#define PARAM_STARTUP_CYCLES    BUILD_PARAMETER(14,2)
#define PARAM_RESERVE_SOME      BUILD_PARAMETER(16,16)
#define PARAM_TOTAL_SIZE        32
//@} End of doxygen group

#define PARTITION_VERSION       1   //!< Partition layout version number

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Contains all variables to maintain life time data
typedef struct {
    UInt32 Duration;            //!< Accumulated life time
    UInt32 LastUpdate;          //!< Last storage update time
    UInt16 ErrCount;            //!< Update counter failures
} bmLifeTimeData_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Non-volatile parameters descriptor (used for verification)
static const bmParamRange_t ParamTable[] = {
    { PARAM_LAYOUT_VERSION, 1, PARTITION_VERSION, PARTITION_VERSION },
    { PARAM_LAUNCH_STATE,   0,  1,  0 },
    { PARAM_LAUNCH_YEAR,    0, 99,  0 },
    { PARAM_LAUNCH_MONTH,   1, 12,  1 },
    { PARAM_LAUNCH_DAY,     1, 31,  1 },
    { PARAM_OPERATION_TIME, 0,  0,  0 },
    { PARAM_STARTUP_CYCLES, 0,  0,  0 }
};

static bmBoardInfoBlock_t *InfoBlock;   //!< Pointer to board info block
static bmLifeTimeData_t LifeTime = {0}; //!< Life time counter structure
static Bool LaunchDateSaved;            //!< Launch date saved flag
static Handle_t bmStorage;              //!< Storage partition handle


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmFormatPermStorage (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendSerialNumber  (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendEndTestResult (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendHardwareInfo  (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendSoftwareInfo  (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendLoaderInfo    (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendLifeCycleInfo (UInt16 Channel, CanMessage_t *Message);
static Error_t bmResetPermData     (UInt16 Channel, CanMessage_t *Message);
static Bool    bmUpdateLaunchDate  (void);
static Error_t bmVerifyPartition   (void);
static Error_t bmUpgradePartition  (void);


/*****************************************************************************/
/*!
 *  \brief   Update life time counter
 *
 *      This function is called by the task scheduler on a regular base.
 *      It increments the life time counter in non-volatile storage by
 *      the number of seconds expired since last updating the counter.
 *      Beside this, the launch info is updated.
 *
 *      Time measurement is done in milliseconds, but converted to
 *      seconds before saving it into non-volatile memory.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmUpdateLifeTimeCounter (void) {

    UInt32 TimeExpired = bmTimeExpired(LifeTime.LastUpdate);
    Error_t Status;

    LifeTime.Duration += TimeExpired;

    Status = bmIncStorageItem (
        bmStorage, PARAM_OPERATION_TIME, LifeTime.Duration / 1000);

    if (Status == NO_ERROR) {
        //dbgPrint ("Life time = %lds",
        //    bmGetStorageItem(bmStorage, PARAM_OPERATION_TIME, 0));
        LifeTime.Duration = TimeExpired % 1000;
        LifeTime.ErrCount = 0;
    }
    else if (LifeTime.ErrCount++ == 10) {
        bmSignalEvent (0, E_LIFE_TIME_UPDATE, 1, 0);
    }
    LifeTime.LastUpdate = bmGetTime();

    if (!LaunchDateSaved) {
        LaunchDateSaved = bmUpdateLaunchDate();
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Update board launch date
 *
 *      Checks if the launch date is already stored in non-volatile memory.
 *      If not, the actual date is request and saved in non-volatile memory.
 *      If actual date is not available, because the master hasn't send a
 *      time message yet, checking is repeated each time this function is
 *      called. After a valid date is available, that date is stored in
 *      non-volatile memory as the device launch date.
 *
 *      Launch means the 1st start of the board after delivery. It can be
 *      read by the master using a service command.
 *
 *  \return  TRUE = update done, FALSE = not done
 *
 ****************************************************************************/

static Bool bmUpdateLaunchDate (void) {

    DateTime_t Date;

    if (bmReadDateTime(&Date) == NO_ERROR) {

        bmSetStorageItem (bmStorage, PARAM_LAUNCH_YEAR, Date.Year - 2000);
        bmSetStorageItem (bmStorage, PARAM_LAUNCH_MONTH, Date.Month);
        bmSetStorageItem (bmStorage, PARAM_LAUNCH_DAY, Date.Day);
        bmSetStorageItem (bmStorage, PARAM_LAUNCH_STATE, 1);
        return (TRUE);
    }
    return (FALSE);
}


/*****************************************************************************/
/*!
 *  \brief   Format non volatile memory
 *
 *      Reformats the non-volatile memory. The message contains the
 *      maximum number of partitions to be creatable on the storage.
 *      After formatting, all data and all partitions are lost.
 *
 *      This function is intended to be uses during manufacturing only.
 *      To prevent unauthorized access to this critical function, it is
 *      protected with a numerical password, contained in the data of
 *      the message. If the password doesn't match, an error message is
 *      signalled to the master.
 *
 *      After formatting is finished, an acknowledge message containing
 *      a success/failed flag is returned to the master. After that,
 *      the node is automatically reset.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request storage format" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmFormatPermStorage (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;
    UInt16 PartitionTableSize;
    UInt16 Password;
    Error_t Status;
    Bool Success;

    if (Message->Length >= 1) {

        Password = bmGetMessageItem (Message, 0, 2);
        if (!bmCheckPassword (Password)) {
            return (E_PROTECTED_COMMAND);
        }
        PartitionTableSize = bmGetMessageItem (Message, 2, 1);

        if (bmFormatStorage(PartitionTableSize) == NO_ERROR) {
//        if (bmInvalidateStorage(PartitionTableSize) == NO_ERROR) {        
            Success = TRUE;
        }
        else {
            Success = FALSE;
        }
        Response.CanID = MSG_SRV_ACK_MEMORY_FORMAT;
        bmSetMessageItem (&Response, Success, 0, 1);
        Response.Length = 1;

        Status = canWriteMessage(Channel, &Response);
        if (Success) {
            UInt32 StartTime = bmGetTime();

            // Reset node when all messages are send
            canFlushMessages(2000);

            while (bmTimeExpired(StartTime) < 2000);
            halHardwareReset();
        }
        return (Status);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Reset non volatile data to defaults
 *
 *      Resets all data in non-volatile memory to defaults. If Channel is
 *      not zero, only the data in the partition belonging to the module
 *      instance associated with Channel is reset. If Channel is zero,
 *      data in all partitions (including base module) are reset.
 *
 *      This function is intended to be uses during manufacturing only.
 *      To prevent unauthorized access to this critical function, it is
 *      protected with a numerical password, contained in the data of
 *      the message. If the password doesn't match, an error message is
 *      signalled to the master.
 *
 *      After reseting the data, an acknowledge message containing
 *      a success/failed flag is returned to the master.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "reset permanent data" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmResetPermData (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;
    Error_t Status;
    UInt16 Password;

    if (Message->Length >= 1) {

        Password = bmGetMessageItem (Message, 0, 2);
        if (!bmCheckPassword (Password)) {
            return (E_PROTECTED_COMMAND);
        }
        if (Channel) {
            Status = bmSetModuleControl(Channel, MODULE_CONTROL_RESET_DATA);
        }
        else {
            Status = bmSetGlobalControl(MODULE_CONTROL_RESET_DATA);
        }
        if (Status == NO_ERROR) {
            bmSetMessageItem (&Response, 1, 0, 1);
        }
        else {
            bmSetMessageItem (&Response, 0, 0, 1);
        }
        Response.CanID  = MSG_SRV_ACK_RESET_DATA;
        Response.Length = 1;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Send serial number of board
 *
 *      Reads the serial number from the external info block and sends it
 *      via CAN to the master. The info block must be written to a fixed
 *      memory location during manufacturing of the board. The format of
 *      the serial number is not defined; therefore it is coded and send
 *      as a formatless string.
 *
 *      Since the length of the serial number exceeds the capacity of a
 *      single CAN message, multiple messages are used. Each message
 *      contains a  sequence number byte and 7 serial number bytes. The
 *      end of the serial number is indicated by a null character.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request serial number" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendSerialNumber (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;
    Error_t Status;
    UInt16 i;

    if (InfoBlock != NULL) {

        const char *Serial = InfoBlock->SerialNum;
        UInt16 Length = strlen(Serial) + 1;
        UInt16 MsgNum = 0;

        Response.CanID = MSG_SRV_SERIAL_NUMBER;

        if (Serial == NULL) {
            return (E_SERIAL_NUMBER_MISSING);
        }
        while (Length) {
            UInt16 Count = MIN(Length, 7);

            for (i=1; i <= Count; i++) {
                bmSetMessageItem (&Response, *Serial++, i, 1);
            }
            while (i < 8) {
                bmSetMessageItem (&Response, 0, i++, 1);
            }
            bmSetMessageItem (&Response, MsgNum++, 0, 1);
            Response.Length = 8;

            if ((Status = canWriteMessage(Channel, &Response)) < 0) {
                return (Status);
            }
            Length -= Count;
        }
        return (NO_ERROR);
    }
    return (E_INFOBLOCK_MISSING);
}


/*****************************************************************************/
/*!
 *  \brief   Send unique number
 *
 *      Sends the unique number of the hardware. This is not a serial
 *      number, but a unique number delivered by the hardware. If the
 *      hardware doesn't supports this feature, an error is returned. 
 *      Since the unique number may exceed the capacity of a single 
 *      CAN message, it may use several messages. The STM32 processor
 *      delivers a 96 bit number.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request unique number" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendUniqueNumber (UInt16 Channel, CanMessage_t *Message) {

    const UInt8 *UniqueID;
    CanMessage_t Response;     
    UInt32 Offset = 0;
    UInt32 i;

    UInt16 Length = halGetUniqueID(&UniqueID);

    if (Length > 0) {
        while (Length > Offset) {
    
            UInt32 Count = MIN(Length, 6);
            Error_t Status;
    
            Response.CanID = MSG_SRV_UNIQUE_NUMBER;
            for (i=0; i < Count; i++) {
                bmSetMessageItem (&Response, UniqueID[Offset+i], i+2, 1);
            }
            bmSetMessageItem (&Response, Length*8, 0, 1);
            bmSetMessageItem (&Response, Offset/6, 1, 1);
            Response.Length = Count + 2;
    
            if ((Status = canWriteMessage(Channel, &Response)) < 0) {
                return (Status);
            }
            Offset += Count;
        }
        return (NO_ERROR);
    }
    return (E_UNSUPPORTED_FUNCTION);
}


/*****************************************************************************/
/*!
 *  \brief   Send board name
 *
 *      Reads the board name stored in the external info block and sends
 *      it via CAN to the master. The info block must be written to a
 *      fixed memory location during manufacturing of the board. The
 *      board name can be from 0 to 8 bytes in length.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request board name" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendBoardName (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;
    UInt16 Length;
    UInt16 i;

    if (InfoBlock != NULL) {

        char *BoardName = InfoBlock->BoardName;

        if (BoardName == NULL) {
            BoardName = "unnamed";
        }
        Length = strlen(BoardName);
        if (Length > ELEMENTS(Response.Data)) {
            Length = ELEMENTS(Response.Data);
        }
        for (i=0; i < Length; i++) {
            bmSetMessageItem (&Response, BoardName[i], i, 1);
        }
        while (i < 8) {
            bmSetMessageItem (&Response, 0, i++, 1);
        }
        Response.CanID  = MSG_SRV_BOARD_NAME;
        Response.Length = 8;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_INFOBLOCK_MISSING);
}


/*****************************************************************************/
/*!
 *  \brief   Send end-test date and status
 *
 *      Reads the end-test informations from the external info block and
 *      sends it via CAN to the master. The info block must be written to
 *      a fixed memory location during manufacturing of the board. The
 *      end-test information block includes:
 *
 *      - End-test result (OK, FAILED, NOT TESTED)
 *      - Date of end-test (Year, month, day)
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request end-test results" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendEndTestResult (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;

    if (InfoBlock != NULL) {

        Response.CanID = MSG_SRV_ENDTEST_RESULT;
        bmSetMessageItem (&Response, InfoBlock->EndTestResult, 0, 1);
        bmSetMessageItem (&Response, InfoBlock->EndTestYear,  1, 1);
        bmSetMessageItem (&Response, InfoBlock->EndTestMonth, 2, 1);
        bmSetMessageItem (&Response, InfoBlock->EndTestDay, 3, 1);
        Response.Length = 4;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_INFOBLOCK_MISSING);
}


/*****************************************************************************/
/*!
 *  \brief   Send launch date and status
 *
 *      Reads the launch informations from non-volatile memory and sends
 *      it via CAN to the master. The launch information is written to
 *      non-volatile memory on first start of the slave. The launch
 *      information contains:
 *
 *      - Launch status (launched, not launched)
 *      - Date of first start (Year, month, day)
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request launch date" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendLaunchDate (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;

    if (bmStorage >= 0) {

        bmSetMessageItem (&Response,
            bmGetStorageItem (bmStorage, PARAM_LAUNCH_STATE, 0), 0, 1);
        bmSetMessageItem (&Response,
            bmGetStorageItem (bmStorage, PARAM_LAUNCH_YEAR, 0),  1, 1);
        bmSetMessageItem (&Response,
            bmGetStorageItem (bmStorage, PARAM_LAUNCH_MONTH, 0), 2, 1);
        bmSetMessageItem (&Response,
            bmGetStorageItem (bmStorage, PARAM_LAUNCH_DAY, 0), 3, 1);

        Response.CanID  = MSG_SRV_LAUNCH_DATE;
        Response.Length = 4;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_STORAGE_READ_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Send hardware informations
 *
 *      Reads the hardware informations from the external info block and
 *      sends it via CAN to the master. The info block must be written to
 *      a fixed memory location during manufacturing of the board. The
 *      hardware informations consists of:
 *
 *      - Hardware version number (major, minor)
 *      - Date of production (Year, month, day)
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request hardware informations" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendHardwareInfo (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;

    if (InfoBlock != NULL) {

        Response.CanID = MSG_SRV_HARDWARE_INFO;
        bmSetMessageItem (&Response, InfoBlock->VersionMajor, 0, 1);
        bmSetMessageItem (&Response, InfoBlock->VersionMinor, 1, 1);
        bmSetMessageItem (&Response, InfoBlock->ProductionYear, 2, 1);
        bmSetMessageItem (&Response, InfoBlock->ProductionMonth, 3, 1);
        bmSetMessageItem (&Response, InfoBlock->ProductionDay,  4, 1);
        Response.Length = 5;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_INFOBLOCK_MISSING);
}


/*****************************************************************************/
/*!
 *  \brief   Send software informations
 *
 *      Sends the software informations via CAN to the master. Software
 *      informations consists of:
 *
 *      - Software version number (major, minor)
 *      - Date of software creation (Year, month, day)
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request software informations" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendSoftwareInfo (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;

    Response.CanID = MSG_SRV_SOFTWARE_INFO;
    bmSetMessageItem (&Response, SOFTWARE_VERSION, 0, 2);
    bmSetMessageItem (&Response, SOFTWARE_CREATION_YEAR - 2000, 2, 1);
    bmSetMessageItem (&Response, SOFTWARE_CREATION_MONTH, 3, 1);
    bmSetMessageItem (&Response, SOFTWARE_CREATION_DAY,  4, 1);
    Response.Length = 5;

    return (canWriteMessage(Channel, &Response));
}


/*****************************************************************************/
/*!
 *  \brief   Send bootloader informations
 *
 *      Reads the bootloader informations from the external info block and
 *      sends it via CAN to the master. The info block must be written to
 *      a fixed memory location during manufacturing of the board. The
 *      bootloader informations consists of:
 *
 *      - Bootloader software version number (major, minor)
 *      - Bootloader software creation date (year, month, day)
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request bootloader informations" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendLoaderInfo (UInt16 Channel, CanMessage_t *Message) {

    bmBootInfoBlock_t *BootBlock = bmGetLoaderInfoBlock();

    CanMessage_t Response;

    if (BootBlock != NULL) {

        Response.CanID = MSG_SRV_LOADER_INFO;
        bmSetMessageItem (&Response, BootBlock->VersionMajor,  0, 1);
        bmSetMessageItem (&Response, BootBlock->VersionMinor,  1, 1);
        bmSetMessageItem (&Response, BootBlock->CreationYear,  2, 1);
        bmSetMessageItem (&Response, BootBlock->CreationMonth, 3, 1);
        bmSetMessageItem (&Response, BootBlock->CreationDay,   4, 1);
        Response.Length = 5;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_BOOTLOADER_MISSING);
}


/*****************************************************************************/
/*!
 *  \brief   Send life cycle counters
 *
 *      Sends the life cycle counter(s) via CAN to the master. Currently,
 *      only the total time of operation of the board is returned. The
 *      sent operation time is in minutes.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request life cycle counters" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendLifeCycleInfo (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;
    UInt32 OperationTime;
    UInt32 StartupCycles;

    if (bmStorage >= 0) {
        OperationTime = bmGetStorageItem (bmStorage, PARAM_OPERATION_TIME, 0);
        StartupCycles = bmGetStorageItem (bmStorage, PARAM_STARTUP_CYCLES, 0);

        Response.CanID = MSG_SRV_LIFECYCLE_DATA;
        bmSetMessageItem (&Response, OperationTime / 60, 0, 4);
        bmSetMessageItem (&Response, StartupCycles, 4, 2);
        Response.Length = 6;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_STORAGE_READ_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Send board option
 *
 *      Sends the board option bits of a certain module instance to the
 *      master. The request message must contain the module ID and the
 *      module instance number. The response message send back contains
 *      the same data, and also the number of instances of this module
 *      and the 32 board option bits of the requested module instance.
 *
 *      If the requested module or instance doesn't exists, an error is
 *      returned to the master.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request board options" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendBoardOptions (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;
    UInt32 Count;

    if (Message->Length  >= 3) {

        UInt32 ModuleID = bmGetMessageItem (Message, 0, 2);
        UInt32 Instance = bmGetMessageItem (Message, 2, 1);
        UInt32 Options  = bmGetBoardOptions (ModuleID, Instance, 0);

        if (ModuleID == BASEMODULE_MODULE_ID) {
            Count = bmGetBoardOptions (ModuleID, MODUL_INSTANCES, 0);
        }
        else {
            Count = bmGetModuleInstances (ModuleID);
        }
        if (Instance < Count) {
            Response.CanID = MSG_SRV_BOARD_OPTIONS;
            bmSetMessageItem (&Response, ModuleID, 0, 2);
            bmSetMessageItem (&Response, Instance, 2, 1);
            bmSetMessageItem (&Response, Count, 3, 1);
            bmSetMessageItem (&Response, Options, 4, 4);
            Response.Length = 8;

            return (canWriteMessage(Channel, &Response));
        }
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Verify base module's partition
 *
 *      Verifies the base module's data partition in non-volatile storage
 *      for plausibility. Each data item must be inside the range defined
 *      for that item. If a single data item is outside it's range, ALL
 *      data items are reset to defaults. Both, the ranges (from/to) and
 *      the defaults are taken from a descriptor table.
 *
 *      If the partition is ok, the partition layout version is checked.
 *      If the layout version in the partition is older than the one
 *      required by the actual software (due to a firmware update),
 *      data in the partition has to be converted.
 *
 *      If the layout version is zero, all data items in the partition
 *      are set to defaults as well. This may happen, if the partition
 *      was erased due to a checksum error or during manufacturing.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmVerifyPartition (void) {

    UInt32 LayoutVersion;

    if (bmStorage >= 0) {

        if (!bmVerifyStorageItems (
                bmStorage, ParamTable, ELEMENTS(ParamTable))) {

            bmSignalEvent(0, E_PERSISTENTS_INVALID, 1, 0);
            bmResetPartition();
        }
        LayoutVersion = bmGetStorageItem(bmStorage, PARAM_LAYOUT_VERSION, 0);

        // if required, upgrade partition to new layout format
        if (LayoutVersion < PARTITION_VERSION) {
            bmUpgradePartition();
        }
    }
    return (NO_ERROR);
}


/******************************************************************************/
/*!
 *  \brief   Reset non-volatile data to default
 *
 *      Resets all data in base module's data partition to defaults and
 *      sends an info message about that fact to the master for logging.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

Error_t bmResetPartition (void) {

    if (bmStorage >= 0) {

        bmSignalEvent(0, I_PARTITION_RESET, 1, 0);
        LifeTime.ErrCount = LifeTime.Duration = 0;

        return (bmResetStorageItems (
            bmStorage, ParamTable, ELEMENTS(ParamTable)));
    }
    return (E_STORAGE_OPEN_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Upgrade partition data layout
 *
 *      This function is called, when the data layout version number of this
 *      software release is higher than the layout version stored in the
 *      data partition. Its purpose is to convert (upgrade) an outdated
 *      data layout into the format requirered by the actual software.
 *
 *      Since this is the 1st release of the software, no conversion is
 *      necessary. This function is just a placeholder for the future.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

static Error_t bmUpgradePartition (void) {

    UInt32 LayoutVersion = bmGetStorageItem(bmStorage, PARAM_LAYOUT_VERSION, 0);

    if (LayoutVersion < PARTITION_VERSION) {
        // <insert conversion code here>
        bmSignalEvent(0, I_PARTITION_CONVERTED, 1, LayoutVersion);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes this module
 *
 *      Initializes this module by registering all service messages to the
 *      message dispatcher. Furthermore, the data partition belonging to
 *      the base module is created/opened and the number of startup cycles
 *      in that partition gets incremented.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitServiceModule (void) {

    static bmCallbackEntry_t Commands[] = {
        { MSG_SRV_REQ_LIFECYCLE_DATA, bmSendLifeCycleInfo },
        { MSG_SRV_REQ_SERIAL_NUMBER,  bmSendSerialNumber  },
        { MSG_SRV_REQ_HARDWARE_INFO,  bmSendHardwareInfo  },
        { MSG_SRV_REQ_SOFTWARE_INFO,  bmSendSoftwareInfo  },
        { MSG_SRV_REQ_BOARD_OPTIONS,  bmSendBoardOptions  },
        { MSG_SRV_REQ_ENDTEST_RESULT, bmSendEndTestResult },
        { MSG_SRV_REQ_LOADER_INFO,    bmSendLoaderInfo    },
        { MSG_SRV_REQ_LAUNCH_DATE,    bmSendLaunchDate    },
        { MSG_SRV_REQ_MEMORY_FORMAT,  bmFormatPermStorage },
        { MSG_SRV_REQ_BOARD_NAME,     bmSendBoardName     },
        { MSG_SRV_REQ_UNIQUE_NUMBER,  bmSendUniqueNumber  }
    };
    static bmCallbackEntry_t Broadcasts[] = {
        { MSG_SRV_REQ_RESET_DATA, bmResetPermData }
    };
    Error_t Status;

    // open and verify data partition in non-volatile storage
    bmStorage = bmOpenPermStorage (BASEMODULE_MODULE_ID, 0, PARAM_TOTAL_SIZE);
    if (bmStorage >= 0) {
        bmVerifyPartition();
        bmIncStorageItem (bmStorage, PARAM_STARTUP_CYCLES, 1);
    }
    LifeTime.LastUpdate = LifeTime.Duration = LifeTime.ErrCount = 0;
    LaunchDateSaved = bmGetStorageItem (bmStorage, PARAM_LAUNCH_STATE, 0);

    // register CAN message to handle by this module
    Status = canRegisterMessages (
        BASEMODULE_MODULE_ID, Commands, ELEMENTS(Commands));

    if (Status == NO_ERROR) {
      Status = canRegisterBroadcast (Broadcasts, ELEMENTS(Broadcasts));
    }
    InfoBlock = bmGetBoardInfoBlock();

    return (Status);
}

//****************************************************************************/
