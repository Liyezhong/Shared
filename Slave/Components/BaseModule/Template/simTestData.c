/****************************************************************************/
/*! \file simTestdata.c
 * 
 *  \brief Test patterns for hardware simulation
 *
 *  $Version: $ 0.1
 *  $Date:    $ 24.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *         This module contains test patterns for the hardware simulation
 *         of this project.
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

#include "Global.h"
#include "halLayer.h"
#include "Basemodule.h"
#include "fmAnaInput.h"
#include "fmAnaOutput.h"
#include "fmDigiInput.h"
#include "fmDigiOutput.h"
#include "fmSimulate.h"
#include "fmJoystick.h"
#include "fmTemperature.h"
#include "fmRfid.h"
#include "fmUart.h"

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

typedef void (*TestCaseFunction_t) (void);

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/


//****************************************************************************/
// Implementation 
//****************************************************************************/

static void tstTestConfigurationPhase (void) {

    static const CanLoopbackData_t tstMessages[] = {
        { 1000, 0, { MSG_SYS_SET_NODESTATE,     1, { NODE_STATE_IDENTIFY }}},
        { 5000, 0, { MSG_SYS_ACK_HARDWARE_ID,   0 }},
        { 1000, 0, { MSG_SYS_REQ_CONFIGURATION, 0 }},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}
    
//****************************************************************************/

static void tstTestHeartbeatMessages (void) {

    static const CanLoopbackData_t tstMessages[] = {
        { 1000, 0, { MSG_SYS_CFG_HEARTBEAT,    5, { 0xC0, 0x04, 0x00, 0x0A, 0x00 }}},
        { 1000, 0, { MSG_SYS_MASTER_HEARTBEAT, 0  }},
        { 1000, 0, { MSG_SYS_CFG_HEARTBEAT,    5, { 0xC0, 0x02, 0x00, 0x0A, 0x00 }}},
        { 5000, 0, { MSG_SYS_MASTER_HEARTBEAT, 0  }},
        { 5000, 0, { MSG_SYS_CFG_HEARTBEAT,    5, { 0x00, 0x00, 0x00, 0x00, 0x00 }}},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}

//****************************************************************************/

static void tstTestNodeStateSwitching (void) {

    static const CanLoopbackData_t tstMessages[] = {
        { 1000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_STARTUP   }}},        
        { 1000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_IDENTIFY  }}},
        { 5000, 0, { MSG_SYS_ACK_HARDWARE_ID, 0  }},
        { 1000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_DIAGNOSIS }}},
        { 1000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_DIAGNOSIS }}},
        { 1000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_UPDATE    }}},
        { 1000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_STANDBY   }}},
        { 5000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_SHUTDOWN  }}},
        { 1000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_NORMAL    }}},
        { 1000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_NORMAL    }}},
     // { 1000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_BOOTING   }}},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}
    
//****************************************************************************/

static void tstTestEmergencyStopCommand (void) {

    static const CanLoopbackData_t tstMessages[] = {
        {  500, 1, { MSG_DI_CONFIG_INPUT,    7, { 0xC0, 0, 0, 0, 0, 0, 1 }}},
        { 1000, 1, { MSG_DI_REQ_INPUT_STATE, 2, { 0, 2 }}},
        { 1000, 1, { MSG_DI_REQ_INPUT_STATE, 0  }},
        
        { 1000, 0, { MSG_SYS_NOTSTOP,        1, { 1 }}},
        { 1000, 1, { MSG_DI_REQ_INPUT_STATE, 2, { 0, 2 }}},
        { 1000, 1, { MSG_DI_REQ_INPUT_STATE, 0  }},
        
        { 1000, 0, { MSG_SYS_NOTSTOP,        1, { 0 }}},
        { 1000, 1, { MSG_DI_REQ_INPUT_STATE, 2, { 0, 3 }}},
        { 1000, 1, { MSG_DI_REQ_INPUT_STATE, 0  }},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}
    
//****************************************************************************/

static void tstTestServiceCommands (void) {

    static const CanLoopbackData_t tstMessages[] = {
        { 1000, 0, { MSG_SYS_REALTIME,           6, { 10, 7, 32, 14, 00, 33 }}},
        { 1000, 0, { MSG_SRV_REQ_SERIAL_NUMBER,  0  }},
        { 1000, 0, { MSG_SRV_REQ_ENDTEST_RESULT, 0  }},
        { 1000, 0, { MSG_SRV_REQ_HARDWARE_INFO,  0  }},
        { 1000, 0, { MSG_SRV_REQ_SOFTWARE_INFO,  0  }},
        { 1000, 0, { MSG_SRV_REQ_LOADER_INFO,    0  }},
        { 1000, 0, { MSG_SRV_REQ_LAUNCH_DATE,    0  }},
        { 1000, 0, { MSG_SRV_REQ_LIFECYCLE_DATA, 0  }},
        { 1000, 0, { MSG_SRV_REQ_BOARD_OPTIONS,  3, { 0, 0, 0 }}},
        { 1000, 0, { MSG_SRV_REQ_BOARD_OPTIONS,  3, { 0, 0, 1 }}},
        { 1000, 0, { MSG_SRV_REQ_BOARD_OPTIONS,  3, { 0, 1, 0 }}},
        { 1000, 0, { MSG_SRV_REQ_BOARD_OPTIONS,  3, { 0, 2, 0 }}},
        { 1000, 0, { MSG_SYS_REALTIME,           6, { 10, 7, 24, 14, 00, 33 }}},
        { 1000, 0, { MSG_SRV_REQ_LAUNCH_DATE,    0  }},
        { 1000, 0, { MSG_SRV_REQ_BOARD_NAME,     0  }},
        { 1000, 2, { MSG_SRV_REQ_RESET_DATA,     2, { 0, 0 }}},                        
        { 1000, 2, { MSG_SRV_REQ_RESET_DATA,     2, { 0, 0x99 }}},                        
        { 1000, 0, { MSG_SRV_REQ_MEMORY_FORMAT,  3, { 0, 0, 10 }}},
        { 1000, 0, { MSG_SRV_REQ_MEMORY_FORMAT,  3, { 0, 0x99, 10 }}},                
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}

//****************************************************************************/

static void tstTestProcessData (void) {

    static const CanLoopbackData_t tstMessages[] = {
        {  500, 1, { MSG_DI_CONFIG_INPUT, 7, { 0x80, 0,   0, 0,  0 }}},
        { 1000, 1, { MSG_PROC_CONFIGURE,  5, { 0x02, 0, 250, 0,  0 }}},
        { 5000, 1, { MSG_PROC_CONFIGURE,  5, { 0x01, 0,   0, 0, 10 }}},
        { 5000, 1, { MSG_PROC_CONFIGURE,  5, { 0x01, 2,   0, 0, 10 }}},
        {  500, 1, { MSG_DI_CONFIG_INPUT, 7, { 0x00, 0,   0, 0,  0 }}},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}

//****************************************************************************/

static void tstTestTaskStatistics (void) {

    static const CanLoopbackData_t tstMessages[] = {
        {  500, 0, { MSG_SRV_CFG_STATISTICS, 1, { 0x80 }}},
        { 5000, 0, { MSG_SRV_CFG_STATISTICS, 1, { 0x81 }}},
        {    1, 0, { MSG_SRV_CFG_STATISTICS, 1, { 0x82 }}},
        { 5000, 0, { MSG_SRV_CFG_STATISTICS, 1, { 0x81 }}},
        {    1, 0, { MSG_SRV_CFG_STATISTICS, 1, { 0x82 }}},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}

//****************************************************************************/

static void tstTestDigitalInputFunctionModule (void) {

    static const CanLoopbackData_t tstMessages[] = {
        {  500, 1, { MSG_DI_CONFIG_INPUT,    7, { 0xC0, 0, 0, 0, 1, 0, 1 }}},
        {  500, 1, { MSG_DI_CONFIG_LIMITS,   8, { 0x31, 0, 1, 0x32, 0, 3, 0, 0 }}},
        { 1500, 1, { MSG_DI_REQ_INPUT_STATE, 0  }},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}

//****************************************************************************/
    
static void tstTestAnalogInputFunctionModule (void) {

    static const CanLoopbackData_t tstMessages[] = {
        {  500, 3, { MSG_AI_CONFIG_INPUT,    7, { 0xC0, 0, 0, 1, 0, 0, 1 }}},
        {  500, 3, { MSG_AI_CONFIG_LIMITS,   8, { 0x33, 0, 0, 0x33, 0, 1, 0, 0 }}},
        { 1500, 3, { MSG_AI_REQ_INPUT_STATE, 0  }},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}
    
//****************************************************************************/

static void tstTestDigitalOutputFunctionModule (void) {

    static const CanLoopbackData_t tstMessages[] = {
        {  500, 2, { MSG_DO_CONFIG_OUTPUT,    7, { 0x80, 0, 0, 0, 9, 0, 9 }}},
        {  500, 2, { MSG_DO_CONFIG_BLINKING,  8, { 0x80, 2, 1, 0, 1, 0, 4, 0 }}},
        { 2000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x44, 0, 1, 0, 0, 0, 0 }}},
        
        { 4000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x48, 0, 2, 0, 0, 9, 0 }}},
        { 5000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x40, 0, 3, 8, 0, 0, 0 }}},
        { 5000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x40, 0, 4, 0, 0, 0, 0 }}},
        
        { 5000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x40, 0, 1, 0, 0, 0, 0 }}},
        { 5000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x40, 0, 2, 0, 0, 8, 0 }}},
        { 5000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x40, 0, 3, 8, 0, 0, 0 }}},
        { 5000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x40, 0, 4, 8, 0, 8, 0 }}},
        { 5000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x44, 0, 5, 6, 0, 8, 0 }}},
        { 5000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x40, 0, 6, 0, 0, 0, 0 }}},
        { 6000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x44, 0, 7, 0, 0, 8, 0 }}},
        { 6000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x48, 0, 8, 8, 0, 0, 0 }}},
        { 6000, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x40, 0, 9, 1, 0, 20, 0 }}},
        {  200, 2, { MSG_DO_SET_OUTPUT_STATE, 7, { 0x44, 0, 5, 4, 0, 4, 0 }}},
        
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
        {  200, 2, { MSG_DO_REQ_OUTPUT_STATE, 0, }},      
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}

//****************************************************************************/

static void tstTestAnalogOutputFunctionModule (void) {

    static const CanLoopbackData_t tstMessages[] = {
        {  500, 4, { MSG_AO_CONFIG_OUTPUT,    6, { 0x80, 0, 0xFF, 0xFF, 0, 3 }}},
        { 1000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 8, 10, 0, 10, 0 }}},
        { 3000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_SHUTDOWN }}},
        { 3000, 0, { MSG_SYS_SET_NODESTATE,   1, { NODE_STATE_NORMAL  }}},
        
        { 4000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x41, 0, 12, 0, 0, 0, 0 }}},
        { 4000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 1, 0, 0, 0, 0 }}},
        { 5000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x42, 0, 12, 0, 0, 0, 0 }}},
        { 5000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x43, 0, 4, 0, 0, 0, 0 }}},
        
        { 5000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 1, 0, 0, 0, 0 }}},
        { 5000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 2, 0, 0, 8, 0 }}},
        { 5000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 3, 8, 0, 0, 0 }}},
        { 5000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 4, 8, 0, 8, 0 }}},
        { 5000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 5, 6, 0, 8, 0 }}},
        { 5000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 6, 0, 0, 0, 0 }}},
        { 6000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 7, 0, 0, 8, 0 }}},
        { 6000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 8, 8, 0, 0, 0 }}},
        { 6000, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 9, 1, 0,20, 0 }}},
        {  200, 4, { MSG_AO_SET_OUTPUT_STATE, 7, { 0x40, 0, 5, 4, 0, 4, 0 }}},
        {  400, 4, { MSG_AO_REQ_OUTPUT_STATE, 0, }},      
        {  400, 4, { MSG_AO_REQ_OUTPUT_STATE, 0, }},      
        {  400, 4, { MSG_AO_REQ_OUTPUT_STATE, 0, }},      
        {  400, 4, { MSG_AO_REQ_OUTPUT_STATE, 0, }},      
        {  400, 4, { MSG_AO_REQ_OUTPUT_STATE, 0, }},      
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));    
}

//****************************************************************************/

static void tstTestVoltageMonitoring (void) {

    static const CanLoopbackData_t tstMessages[] = {
        {  500, 0, { MSG_CFG_VOLTAGE_MONITOR,   7, { 0x80, 5, 50, 0x39, 0, 0x19, 0 }}},
        { 1000, 0, { MSG_SRV_REQ_VOLTAGE_STATE, 0  }},
        { 4000, 0, { MSG_SRV_REQ_VOLTAGE_STATE, 0  }},
        { 3000, 0, { MSG_SRV_REQ_VOLTAGE_STATE, 0  }},
        { 9500, 0, { MSG_CFG_VOLTAGE_MONITOR,   7, { 0x00, 5, 50, 0x20, 0, 0x40, 0 }}},
        { 1000, 0, { MSG_SRV_REQ_VOLTAGE_STATE, 0  }},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}

//****************************************************************************/

static void tstTestCurrentMonitoring (void) {

    static const CanLoopbackData_t tstMessages[] = {
        {  500, 0, { MSG_CFG_CURRENT_MONITOR,   7, { 0x80, 5, 50, 0x39, 0, 0x19, 0 }}},
        { 1000, 0, { MSG_SRV_REQ_CURRENT_STATE, 0  }},
        { 4000, 0, { MSG_SRV_REQ_CURRENT_STATE, 0  }},
        { 3000, 0, { MSG_SRV_REQ_CURRENT_STATE, 0  }},
        { 9500, 0, { MSG_CFG_CURRENT_MONITOR,   7, { 0x00, 5, 50, 0x20, 0, 0x40, 0 }}},
        { 1000, 0, { MSG_SRV_REQ_CURRENT_STATE, 0  }},
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}


//****************************************************************************/

static void tstTestJoystickFunctionModule (void) {

    static const CanLoopbackData_t tstMessages[] = {
        { 1000, 6, { MSG_JOYS_SET_CONFIGURATION, 7, { 0x1, 0x3, 0xe8, 0x0, 0x0, 0x0, 0x0 } } },
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}


//****************************************************************************/

static void tstTestRfidFunctionModule (void) {

    static const CanLoopbackData_t tstMessages[] = {
        { 1000, 7, { MSG_RFID_SET_CONFIG, 1, { 0x10 } } },
        { 2000, 7, { MSG_RFID_WRITE_USER_DATA, 5, { 0, 0xde, 0xad, 0xbe, 0xef } } },
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}


//****************************************************************************/

static void tstTestTemperatureFunctionModule (void) {

    static const CanLoopbackData_t tstMessages[] = {
        { 1000, 8, { MSG_TEMP_SET_TEMP, 7, { 1, 50, 5, 0x3, 0xe8, 0, 0 } } },
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}


//****************************************************************************/

static void tstTestUartFunctionModule (void) {

    static const CanLoopbackData_t tstMessages[] = {
        { 1000, 9, { MSG_UART_SET_CONF, 3, { 0xe3, 0x25, 0x80 } } },
        { 2000, 9, { MSG_UART_SET_DATA, 6, { 'd', 'e', 'a', 'd', '\r', '\n' } } },
    };
    simCanRegisterTestMessages (tstMessages, ELEMENTS(tstMessages));
}

/*****************************************************************************/
/*! 
 *  \brief   Test selection function
 *
 *      Selects a test case and calls its setup function. 
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

Error_t tstInitializeTestPatterns (UInt16 TestID) {

    static const TestCaseFunction_t TestCases[] = {
        /*  0 */ NULL,
        /*  1 */ tstTestConfigurationPhase,
        /*  2 */ tstTestHeartbeatMessages,
        /*  3 */ tstTestNodeStateSwitching,
        /*  4 */ tstTestEmergencyStopCommand,        
        /*  5 */ tstTestServiceCommands,
        /*  6 */ tstTestTaskStatistics,
        /*  7 */ tstTestProcessData,
        /*  8 */ tstTestCurrentMonitoring,
        /*  9 */ tstTestVoltageMonitoring,
        /* 10 */ tstTestDigitalInputFunctionModule,
        /* 11 */ tstTestDigitalOutputFunctionModule,
        /* 12 */ tstTestAnalogInputFunctionModule,
        /* 13 */ tstTestAnalogOutputFunctionModule,
        /* 14 */ tstTestJoystickFunctionModule,
        /* 15 */ tstTestRfidFunctionModule,
        /* 16 */ tstTestTemperatureFunctionModule,
        /* 17 */ tstTestUartFunctionModule
    };
    if (TestID < ELEMENTS(TestCases)) {
        TestCases[TestID] ();
    }
    return (NO_ERROR);
}

//****************************************************************************/

