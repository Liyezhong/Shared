/***********************************************************************************/
/*! \file halConfigure.c
 *
 *  \brief  HAL configuration file
 *
 *  $Version: $ 0.1
 *  $Date:    $ 20.08.2012
 *  $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *      This module is used to configure the hardware abstraction layer.
 *      It must be adopted for each hardware board. The following points
 *      must be configurated:
 *
 *      - Pin multiplexing (GPIO, special functions)
 *      - Logical port (digital in-/outputs) descriptor
 *      - Logical analog input/output descriptor
 *      - Logical timer descriptors
 *      - Logical memory descriptors
 *      - Logical serial busses (I2C, SPI)
 *      - External interrupt inputs
 *      - Logical motor decriptors
 *
 *      This file works in conjunction with the halConfigure.h header
 *      file, which must also be adopted to the boards needs.
 *
 *      ATTENTION: Configuration must match the real hardware, else
 *      damage of hardware is possible.
 *
 *  \b Company:
 *
 *      Leica Biosystems Shanghai R&D Center.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/***********************************************************************************/

#include "Global.h"
#include "halCommon.h"
#include "halDeviceID.h"
#include "halConfigure.h"
#include "halProcessor.h"


/***********************************************************************************/
// Processor Frequency Settings
/***********************************************************************************/

const UInt32 halSystemFrequency   = 72000000; //!< System clock frequency [Hz]
const UInt32 halCrystalFrequency  = 16000000; //!< Processor crystal frequency [Hz]
const UInt32 halRtcClockFrequency = 0;        //!< Real time crystal frequency [Hz]

const UInt32 halProcessorVoltage  = 3300;     //!< Nominal processor voltage [mV]
const UInt32 halWatchdogInterval  = 2000;     //!< Watchdog interval time [ms]

const UInt32 halNonVolatileSize   = 512;      //!< Non volatile memory size
#ifdef ASB_FCT
const UInt32 halNonVolatileCache  = 0;        //!< Non volatile memory cache size
#else
const UInt32 halNonVolatileCache  = 512;      //!< Non volatile memory cache size
#endif

const UInt32 halCanDefaultBitrate = 500000;   //!< CAN bus baudrate [Hz]


/***********************************************************************************/
/*!
 *  \brief   Processor pin descriptor table
 *
 *      The pin descriptor table must contain one entry for each processor
 *      pin to configure. The following informations must be specified for
 *      each single pin:
 *
 *      - Physical port number
 *      - Physical port bit number
 *      - Pin multiplexer setting (GPIO / special function)
 *      - Pin type (direction, attributes)
 *      - Input filtering / output max. speed
 *
 *      Which peripheral i/o signal is mapped to which pin is fixed and can
 *      be influenced only in a limited manner by configuring the peripheral
 *      pin mapping (see halPeripheralMapping).
 *
 ***********************************************************************************/

const halPinDesciptor_t halPinDescriptors[] = {
    { PORT_A,  0, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN0/FanCurrentMeasurement
    { PORT_A,  3, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // Lid status    
    { PORT_A,  4, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN4/TempMeas1
    { PORT_A,  5, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN5/TempMeas2
    { PORT_A,  6, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN6/TempMeas3
    { PORT_A,  7, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN7/TempMeas4
#ifdef ASB15_VER_A    
    //{ PORT_A,  8, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN7/TempMeas5
#endif

#ifdef ASB15_VER_B
    //{ PORT_A,  2, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN4/TempMeas5
#endif

    { PORT_A, 11, PIN_MUX_SFIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN1 RX
    { PORT_A, 12, PIN_MUX_SFIO, PIN_TYP_DOUT10, PIN_OPT_NONE }, // CAN1 TX

    { PORT_B,  6, PIN_MUX_SFIO, PIN_TYP_DOUT10, PIN_OPT_OPENDR }, // I2C1/SCL
    { PORT_B,  7, PIN_MUX_SFIO, PIN_TYP_DOUT10, PIN_OPT_OPENDR }, // I2C1/SDA

    { PORT_C,  1, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // ADC12_IN11/5V_VOLTAGE_CURRENT_MEAS
    { PORT_C,  2, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // ADC12_IN12/HeatCurrent_Measurement
    { PORT_C,  3, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // ADC12_IN13/PumpValveCurrent_Measurement
    { PORT_C,  4, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // ADC12_IN14/UnderVoltageDetection
    { PORT_C,  5, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // ADC12_IN15/PressureValve

    { PORT_C,  6, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // HeatElementControl1
    { PORT_C,  7, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // HeatElementControl2
    { PORT_C,  8, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // HeatElementControl3
#ifdef ASB15_VER_A       
    { PORT_C,  9, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // HeatElementControl4
#endif

#ifdef ASB15_VER_B       
    { PORT_C,  9, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // MainRelayControl
#endif

    { PORT_C, 10, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // LidControl

    { PORT_D,  0, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // CAN_LBKA
    { PORT_D,  1, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN_ID4
    { PORT_D,  2, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN_ID3
    { PORT_D,  3, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN_ID2
    { PORT_D,  4, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN_ID1

    { PORT_D,  5, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // FanControl
    { PORT_D,  6, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // ExhaustFanControl
    { PORT_D,  9, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // AirValveControl1
    { PORT_D, 10, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // AirValveControl2
    { PORT_D, 11, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // AirValveControl3
#ifdef ASB15_VER_A
    { PORT_D, 12, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // VacuumPumpControl
#endif

    { PORT_C, 13, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // LED1
    { PORT_C, 15, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // LED2
    { PORT_C, 14, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // LED3

#ifdef ASB15_VER_A
    { PORT_E, 10, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // Remote Alarm Control
    { PORT_E, 11, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // Remote Alarm Set
    { PORT_E, 12, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // Remote Alarm Clear   
    { PORT_B,  9, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // Local Alarm Control
    { PORT_B, 10, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // Local Alarm Set
    { PORT_B, 11, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // Local Alarm Clear
#endif

#ifdef ASB15_VER_B
    { PORT_E, 10, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // Remote Alarm Control
    { PORT_E, 14, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // Remote Alarm Set Status
    { PORT_E, 15, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // Remote Alarm Connection Status
    { PORT_B,  9, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // Local Alarm Control
    { PORT_B, 13, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // Local Alarm Set Status
    { PORT_B, 14, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // Local Alarm Connection Status    
#endif    
    
#ifdef ASB15_VER_B
    { PORT_B, 12, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // VacuumPumpControl
    { PORT_E,  7, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // LimitSwitch1/LidSensor
    { PORT_E,  8, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // LimitSwitch1/OvenDoorSensor
#ifndef ASB_FCT    
    { PORT_B, 15, PIN_MUX_SFIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // PWM
#else    
    { PORT_B, 15, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // GPIO
#endif    
    //{ PORT_A, 10, PIN_MUX_SFIO, PIN_TYP_DOUT2,  PIN_OPT_NONE },
#endif    

};

const UInt32 halPinDescriptorCount = ELEMENTS(halPinDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Logical port descriptor table
 *
 *      The hardware abstraction layer offers logical ports. Logical ports
 *      are mapped to a configurable number of bits on physical ports and
 *      can be accessed without influencing other bits on a physical port.
 *      Following parameters must be configured for each logical port:
 *
 *      - Logical port identifier (Device ID)
 *      - Port data direction (input, output)
 *      - Interface (intern, serial, I2C, SPI)
 *      - Physical port number (GPIO)
 *      - Bit position inside physical port
 *      - Port width (number of bits)
 *      - Polarity bitmask (inversion bitmask)
 *      - Initial output value
 *
 *      The physical port bits must be configured as GPIOs to be useable for
 *      logical ports (see pin descriptor table).
 *
 ***********************************************************************************/

const halPortDescriptor_t halPortDescriptors[] = {
#ifndef ASB_FCT
    { HAL_CAN_NODE_INDEX,      DIR_INPUT,  BUS_TYPE_INTERN, PORT_D,  1, 4, 0, 0 },
    { HAL_CAN_LOOPBACK,        DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D,  0, 1, 0, 0 },

    { HAL_STATUS_LED1,         DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C, 13, 1, 0, 0 },
    { HAL_STATUS_LED2,         DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C, 15, 1, 0, 0 },
    { HAL_STATUS_LED3,         DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C, 14, 1, 0, 0 },

    //{ HAL_TEMP_MAINVOLTAGE_0,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D,  9, 1, 0, 0 },
    
    //{ HAL_FANCONTROL_SIGNAL,   DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D,  6, 1, 0, 0 },
    { HAL_LIDCONTROL_SIGNAL,   DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C, 10, 1, 0, 0 },
    { HAL_HEATER_RELAY,        DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C,  9, 1, 0, 0 },
#ifdef ASB15_VER_A     
    { HAL_REMOTE_ALARM_CONTROL,DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E, 10, 1, 0, 0 },
    { HAL_REMOTE_ALARM_SET,    DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E, 11, 1, 0, 0 },
    { HAL_REMOTE_ALARM_CLEAR,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E, 12, 1, 0, 0 }, 
    { HAL_LOCAL_ALARM_CONTROL, DIR_OUTPUT, BUS_TYPE_INTERN, PORT_B,  9, 1, 0, 0 },
    { HAL_LOCAL_ALARM_SET,     DIR_OUTPUT, BUS_TYPE_INTERN, PORT_B, 10, 1, 0, 0 },
    { HAL_LOCAL_ALARM_CLEAR,   DIR_OUTPUT, BUS_TYPE_INTERN, PORT_B, 11, 1, 0, 0 },
#endif

#ifdef ASB15_VER_B
    { HAL_REMOTE_ALARM_CONTROL,DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E, 10, 1, 0, 0 },
    { HAL_LOCAL_ALARM_CONTROL, DIR_OUTPUT, BUS_TYPE_INTERN, PORT_B,  9, 1, 0, 0 },
#endif

#ifdef ASB15_VER_A            
    { HAL_LIDSTATUS_SIGNAL,    DIR_INPUT,  BUS_TYPE_INTERN, PORT_A,  3, 1, 0, 0 },
#endif    
    
#ifdef ASB15_VER_B
    { HAL_LIDSTATUS_SIGNAL,    DIR_INPUT,  BUS_TYPE_INTERN, PORT_E,  7, 1, 0, 0 },
    { HAL_OVENDOOR_SIGNAL,     DIR_INPUT,  BUS_TYPE_INTERN, PORT_E,  8, 1, 0, 0 },
    { HAL_LOCAL_ALARM_CONNECT, DIR_INPUT,  BUS_TYPE_INTERN, PORT_B, 14, 1, 0, 0 },
    { HAL_REMOTE_ALARM_CONNECT,DIR_INPUT,  BUS_TYPE_INTERN, PORT_E, 15, 1, 0, 0 },
#endif    

    { HAL_TEMP_CTRLHEATING_0,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C,  8, 1, 0, 0 },
    { HAL_TEMP_CTRLHEATING_1,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C,  6, 1, 0, 0 },
    { HAL_TEMP_CTRLHEATING_2,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C,  7, 1, 0, 0 },

//
//    { HAL_TEMP_CTRLHEATING_0,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D,  9, 1, 0, 0 },
//    { HAL_TEMP_CTRLHEATING_1,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 10, 1, 0, 0 },
//    { HAL_TEMP_CTRLHEATING_2,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 11, 1, 0, 0 },
//    { HAL_TEMP_CTRLHEATING_3,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 12, 1, 0, 0 },

#ifdef ASB15_VER_A
    { HAL_PRESS_CTRLPUMPING_0, DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 12, 1, 0, 0 },
#endif

#ifdef ASB15_VER_B
    { HAL_PRESS_CTRLPUMPING_0, DIR_OUTPUT, BUS_TYPE_INTERN, PORT_B, 12, 1, 0, 0 },
#endif

    { HAL_PRESS_FANCONTROL_0,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D,  6, 1, 0, 0 },  //Fan
    { HAL_PRESS_CTRLVALVE_0,   DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 10, 1, 0, 0 },  //Pressure
    { HAL_PRESS_CTRLVALVE_1,   DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 11, 1, 0, 0 }    //Vacuum
#else//FCT configuration
    { HAL_CAN_NODE_INDEX,      DIR_INPUT,  BUS_TYPE_INTERN, PORT_D,  1, 4, 0, 0 },
    { HAL_CAN_LOOPBACK,        DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D,  0, 1, 0, 0 },

    { HAL_STATUS_LED1,         DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C, 13, 1, 0, 0 },
    { HAL_STATUS_LED2,         DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C, 15, 1, 0, 0 },
    { HAL_STATUS_LED3,         DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C, 14, 1, 0, 0 },

    { HAL_LOCAL_ALARM_CONTROL, DIR_OUTPUT, BUS_TYPE_INTERN, PORT_B,  9, 1, 0, 0 },
    { HAL_VACUUM_PUMP_CONTROL, DIR_OUTPUT, BUS_TYPE_INTERN, PORT_B, 12, 1, 0, 0 },
    { HAL_LOCAL_ALARM_CONNECT, DIR_INPUT,  BUS_TYPE_INTERN, PORT_B, 14, 1, 0, 0 },
    { HAL_PUMP_PWM_CONTROL,    DIR_OUTPUT, BUS_TYPE_INTERN, PORT_B, 15, 1, 0, 0 },

    { HAL_HEAT_ELEMENT_CONTROL1,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C,  6, 1, 0, 0 },
    { HAL_HEAT_ELEMENT_CONTROL2,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C,  7, 1, 0, 0 },
    { HAL_HEAT_ELEMENT_CONTROL3,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C,  8, 1, 0, 0 },
    { HAL_HEATER_RELAY_CONTROL,   DIR_OUTPUT, BUS_TYPE_INTERN, PORT_C,  9, 1, 0, 0 },

    { HAL_EXHAUST_FAN_CONTROL,   DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D,  6, 1, 0, 0 }, 
    { HAL_AIR_VALVE_CONTROL_1,   DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 10, 1, 0, 0 },  
    { HAL_AIR_VALVE_CONTROL_2,   DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 11, 1, 0, 0 },

    { HAL_LIDSTATUS_SIGNAL,     DIR_INPUT,  BUS_TYPE_INTERN, PORT_E,  7, 1, 0, 0 },
    { HAL_OVENDOOR_SIGNAL,      DIR_INPUT,  BUS_TYPE_INTERN, PORT_E,  8, 1, 0, 0 },
    { HAL_REMOTE_ALARM_CONTROL, DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E, 10, 1, 0, 0 },
    { HAL_REMOTE_ALARM_CONNECT, DIR_INPUT,  BUS_TYPE_INTERN, PORT_E, 15, 1, 0, 0 },
#endif
};
const UInt32 halPortDescriptorCount = ELEMENTS(halPortDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Logical analog channel descriptor table
 *
 *      The hardware abstraction layer offers logical analog channels,
 *      which can be inputs or outputs and can use processor internal
 *      converters or external converters. 
 *
 *      Following parameters must be configured for each logical port:
 *
 *      - Logical channel ID (device ID)
 *      - Data direction (input, output)
 *      - Interface (intern, serial, I2C, SPI)
 *      - Converter resolution (number of bits)
 *      - Physical channel number(ADC channel)
 *      - Conversion time
 *      - Scaling facor (value for full scale)
 *      - Initial output value
 *
 ***********************************************************************************/

const halAnalogDescriptor_t halAnalogDescriptors[] = {
#ifndef ASB_FCT
    { HAL_SUPPLY_VOLTAGE,    DIR_INPUT, BUS_TYPE_INTERN, 12, 14, 6, 33000, 0 },
    { HAL_SUPPLY_CURRENT,    DIR_INPUT, BUS_TYPE_INTERN, 12, 11, 6,   750, 0 },
      
    { HAL_TEMP_CURRENT_0,    DIR_INPUT, BUS_TYPE_INTERN, 12, 12, 6,  3000, 0 },
    
    { HAL_PRESS_CURRENT_0,   DIR_INPUT, BUS_TYPE_INTERN, 12, 13, 6,  3000, 0 },
    
    { HAL_PRESS_FANCURRENT_0,DIR_INPUT, BUS_TYPE_INTERN, 12,  0, 6,  3000, 0 },


    { HAL_TEMP_SENSOR_0,     DIR_INPUT, BUS_TYPE_INTERN, 12,  6, 6,  3000, 0 },
    { HAL_TEMP_SENSOR_1,     DIR_INPUT, BUS_TYPE_INTERN, 12,  4, 6,  3000, 0 },
    { HAL_TEMP_SENSOR_2,     DIR_INPUT, BUS_TYPE_INTERN, 12,  5, 6,  3000, 0 },
    { HAL_TEMP_SENSOR_3,     DIR_INPUT, BUS_TYPE_INTERN, 12,  7, 6,  3000, 0 },
    //{ HAL_TEMP_SENSOR_4,     DIR_INPUT, BUS_TYPE_INTERN, 12,  4, 6,  3000, 0 },

    { HAL_PRESS_SENSOR_0,    DIR_INPUT, BUS_TYPE_INTERN, 12, 15, 6,  3000, 0 },

#ifdef ASB15_VER_B
    // TIM1_CH3N, 2 ==> CH3, 10ns*10000 ==> 10 KHz 
    { HAL_PRESS_PWM_CTRL_0,  DIR_OUTPUT,BUS_TYPE_PWM,    16,  2, 0, 10000, 0, HAL_PWM_0}
#endif

     //HAL_PRESS_CURRENT, HAL_PRESS_MAINVOLTAGE, HAL_PRESS_CTRLPUMPING, Instances);

#else                                                                                            
    { HAL_TUBE_SENSOR_1_VOLTAGE,     DIR_INPUT, BUS_TYPE_INTERN, 12,  4, 6,  3000, 0 },
    { HAL_TUBE_SENSOR_2_VOLTAGE,     DIR_INPUT, BUS_TYPE_INTERN, 12,  5, 6,  3000, 0 },
    { HAL_LEVEL_SENSOR_VOLTAGE,      DIR_INPUT, BUS_TYPE_INTERN, 12,  6, 6,  3000, 0 },
    { HAL_PRESSURE_SENSOR_VOLTAGE,   DIR_INPUT, BUS_TYPE_INTERN, 12, 15, 6,  3000, 0 },
    { HAL_VCC5V_SENSOR_CURRENT,      DIR_INPUT, BUS_TYPE_INTERN, 12, 11, 6,  3000, 0 },
    { HAL_TUBE_LEVEL_SENSOR_CURRENT, DIR_INPUT, BUS_TYPE_INTERN, 12, 12, 6,  3000, 0 },
    { HAL_PUMP_VALVE_SENSOR_CURRENT, DIR_INPUT, BUS_TYPE_INTERN, 12, 13, 6,  3000, 0 },
    { HAL_EXHAUSTFAN_SENSOR_CURRENT, DIR_INPUT, BUS_TYPE_INTERN, 12,  0, 6,  3000, 0 },
    { HAL_UNDER_VOLTAGE_DETECTION,   DIR_INPUT, BUS_TYPE_INTERN, 12, 14, 6,  33000, 0 }
#endif
};
const UInt32 halAnalogDescriptorCount = ELEMENTS(halAnalogDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Logical timers descriptor table
 *
 *      The timer descriptor table must contain one entry for each timer and
 *      for each capture/compare unit used. The following informations must be
 *      specified for each single pin:
 *
 *      - Logical timer ID (device ID)
 *      - Physical timer number
 *      - Physical capture/compare unit number
 *      - Priority of interrupt (1=highest, 15=lowest, 0=default)
 *      - Operation of capture/compare unit (capture, compare, disabled)
 *      - Mode of timer resp. capture/compare channel
 *
 *      The interrupt priority must be specified only for the counter, the
 *      priorities specified for the capture/compare units are not evaluated.
 *
 *      Capture/compare units that are not used needs not to be specified.
 *
 ***********************************************************************************/
#ifdef ASB15_VER_A
const halTimerDescriptor_t halTimerDescriptors[] = {
0
/*
    // Timer 3 programmed for the control of the heating elements
    { HAL_TEMP_CTRLHEATING, 2, 0, 0, TIM_SEL_COUNTER, TIM3_MODE_MASTER  },
    { HAL_TEMP_CTRLHEATING, 2, 0, 0, TIM_SEL_COMPARE, TIM3_MODE_CAPCOM1 },
    { HAL_TEMP_CTRLHEATING, 2, 1, 0, TIM_SEL_COMPARE, TIM3_MODE_CAPCOM2 },
    { HAL_TEMP_CTRLHEATING, 2, 2, 0, TIM_SEL_COMPARE, TIM3_MODE_CAPCOM3 },
    { HAL_TEMP_CTRLHEATING, 2, 3, 0, TIM_SEL_COMPARE, TIM3_MODE_CAPCOM4 }
*/
};
//const UInt32 halTimerDescriptorCount = ELEMENTS(halTimerDescriptors);
const UInt32 halTimerDescriptorCount = 0;
#endif

#ifdef ASB15_VER_B
const halTimerDescriptor_t halTimerDescriptors[] = {
    { HAL_PWM_0, 0, 0, 0, TIM_SEL_COUNTER, 0 },
    { HAL_PWM_0, 0, 0, 0, TIM_SEL_DISABLE, TIM3_MODE_CAPCOM1 },
    { HAL_PWM_0, 0, 1, 0, TIM_SEL_DISABLE, TIM3_MODE_CAPCOM2 },
    { HAL_PWM_0, 0, 2, 0, TIM_SEL_COMPARE, TIM3_MODE_CAPCOM3 },
    { HAL_PWM_0, 0, 3, 0, TIM_SEL_DISABLE, TIM3_MODE_CAPCOM4 }
};
const UInt32 halTimerDescriptorCount = ELEMENTS(halTimerDescriptors);
#endif

/***********************************************************************************/
/*!
 *  \brief   Stepper motor descriptor table
 *
 *      The stepper motor descriptor table must contain one entry for each motor.
 *      The following informations must be specified for each single motor:
 *
 *      - Logical stepper motor device ID
 *      - SPI number the driver is connected to
 *      - Stepper controller configuration words
 *
 *      Multiple motors can be maintained using one SPI port, as long as the
 *      slave select pin NSS differs. Nevertheless, only one of those motors
 *      can be open at the same time.
 *
 ***********************************************************************************/

const halStepperDescriptor_t halStepperDescriptors[] = {0};

const UInt32 halStepperDescriptorCount = 0;


/***********************************************************************************/
/*!
 *  \brief   Memory descriptor table
 *
 *      The memory descriptor table must contain one entry for each logical
 *      memory. The following informations must be configured for each
 *      logical memory:
 *
 *      - Logical device ID
 *      - Storage class (FRAM, EEPROM, FLASH)
 *      - Access mode (Read/Write/Erase)
 *      - Base address
 *      - Size of logical memory
 *
 *      In case a memory device is connected via a serial bus (I2C, SPI) the
 *      upper bits contain the bus number and chip (slave) select bits. See
 *      in the individual device modules for more informations.
 *
 *      A physical memory can contain multiple logical memories by
 *      specifying different different start addresses.
 *
 ***********************************************************************************/

const halStorageDescriptor_t halStorageDescriptors[] = {
   { HAL_STORAGE_FLASH, MEM_CLASS_FLASH, HAL_OPEN_RWE, 0x08000000, 0x20000 },
   { HAL_STORAGE_OTP,   MEM_CLASS_FLASH, HAL_OPEN_RWE, 0x1FFFF000, 0x00800 },
   { HAL_STORAGE_FRAM,  MEM_CLASS_FRAM,  HAL_OPEN_RWE, 0x00000000, 512 }
};
const UInt32 halStorageDescriptorCount = ELEMENTS(halStorageDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Serial devices descriptor table
 *
 *      The serial descriptor table must contain one entry for each logical serial
 *      device (I2C, SPI, CAN, UART). The following informations must be configured
 *      for each device:
 *
 *      - Logical device ID
 *      - Logical output port for chip select (SPI only)
 *      - Bus class (I2C, SPI, CAN)
 *      - Number of bus members (I2C only)
 *      - Physical device/interface number
 *      - Baud rate
 *
 *      The number of bus members is applicable only for I2C busses to specify the
 *      max. number of slaves connected to the bus. In case of SPI, each slave
 *      device connected to the SPI bus must have it's own entry into this table.
 *
 ***********************************************************************************/

const halSerialDescriptor_t halSerialDescriptors[] = {
    { HAL_CAN_SYSTEM, 0, SIO_TYPE_CAN, 1, 0, 500000 },
    { HAL_BUS_I2C_A,  0, SIO_TYPE_I2C, 1, 0, 400000 }
};
const UInt32 halSerialDescriptorCount = ELEMENTS(halSerialDescriptors);


/***********************************************************************************/
/*!
 *  \brief   ADC779x analog input descriptor table
 *
 *      The ADC779x descriptor table must contain one entry for each channel of the
 *      ADC. The following informations must be configured for each device:
 *
 *      - Slave device ID (SPI)
 *      - Input channel number (logical)
 *      - Input scan relative frequency
 *      - AD779x i/o register setting
 *      - AD779x mode register setting
 *      - AD779x configuration register setting
 *
 ***********************************************************************************/

const halAd779xDescriptor_t halAd779xDescriptors[] = {0};

const UInt32 halAd779xDescriptorCount = 0;


/***********************************************************************************/
/*!
 *  \brief   Processor peripheral pin mappings
 *
 *      The peripheral pin mapping descriptor contains the informations
 *      for the various processor internal peripherals and to which pins
 *      the i/o signals of those periperhals are routed.
 *
 *      The defines used in this structure are defined in halConfigure.h
 *      and should be adopted there.
 *
 ***********************************************************************************/

const halPinMapping_t halPinMappings = {
    MAPPING_SPI1,       //!< SPI1 mapping
    MAPPING_I2C1,       //!< I2C1 mapping
    MAPPING_USART1,     //!< USART1 mapping
    MAPPING_USART2,     //!< USART2 mapping
    MAPPING_USART3,     //!< USART3 mapping
    MAPPING_TIMER1,     //!< TIMER1 mapping
    MAPPING_TIMER2,     //!< TIMER2 mapping
    MAPPING_TIMER3,     //!< TIMER3 mapping
    MAPPING_TIMER4,     //!< TIMER4 mapping
    MAPPING_CAN1,       //!< CAN1 alternate function mapping
    MAPPING_DEFAULT,    //!< Port D0/D1 remap to OSC IN/OUT
    MAPPING_TIMER5,     //!< TIMER5 mapping
    MAPPING_DEFAULT,    //!< ADC1 ext. trigger injected mapping
    MAPPING_DEFAULT,    //!< ADC1 ext. trigger regular mapping
    MAPPING_DEFAULT,    //!< ADC2 ext. trigger injected mapping
    MAPPING_DEFAULT,    //!< ADC2 ext. trigger regular mapping
    MAPPING_DEFAULT,    //!< Ethernet MAC I/O mapping
    MAPPING_CAN2,       //!< CAN2 I/O mapping
    MAPPING_DEFAULT,    //!< Ethernet MII/RMII selection
    MAPPING_SWJCFG,     //!< Serial wire JTAG configuration
    MAPPING_DEFAULT,    //!< Reserved
    MAPPING_SPI3,       //!< SPI3 mapping
    MAPPING_DEFAULT,    //!< TIMER2 internal trigger 1 mapping
    MAPPING_DEFAULT,    //!< Ethernet PTP PPS mapping
    MAPPING_DEFAULT,    //!< Reserved
    MAPPING_TIMER15,    //!< TIMER15 mapping
    MAPPING_TIMER16,    //!< TIMER16 mapping
    MAPPING_TIMER17,    //!< TIMER17 mapping
    MAPPING_DEFAULT,    //!< CEC mapping
    MAPPING_DEFAULT,    //!< TIMER1 CH1 mapping
    MAPPING_TIMER9,     //!< TIMER9 mapping
    MAPPING_TIMER10,    //!< TIMER10 mapping
    MAPPING_TIMER11,    //!< TIMER11 mapping
    MAPPING_TIMER13,    //!< TIMER13 mapping
    MAPPING_TIMER14,    //!< TIMER14 mapping
    MAPPING_DEFAULT,    //!< FSMC mapping
    MAPPING_DEFAULT,    //!< TIMER6/7 DMA mapping
    MAPPING_TIMER12,    //!< TIMER12 mapping
    MAPPING_DEFAULT,    //!< Miscellaneous mapping
    MAPPING_DEFAULT     //!< Reserved
};


/***********************************************************************************/
/*!
 *  \brief   Processor information block
 *
 *      The Processor information block contains information about the selected
 *      processor. This includes:
 *
 *      - Processor family and density
 *      - Processor clock rate (max)
 *      - Size of RAM and FLASH memory
 *      - Number of instances of each peripheral
 *      - Timers selection bitmap
 *
 *      The timer selection bitmap indicates which timers are available on
 *      the selected processor (Bit 0..16 are related to TIM1..T17).
 *
 *      The defines used in this structure are defined in halProcessor.h.
 *      They depend on the value of the PROCESSOR define, which should
 *      select one of the supported members of the STM32F10xxx family.
 *      See halProcessor.h for more details.
 *
 ***********************************************************************************/

const halProcessorInfo_t halProcessorInfo = {
    DEVICE_FAMILY,      //!< Processor family ID
    DEVICE_DENSITY,     //!< Processor density ID
    DEVICE_SYSCLOCK,    //!< Processor clock frequency
    NUMBER_OF_GPIOs,    //!< Number of general purpose I/O's
    NUMBER_OF_PORTs,    //!< Number of processor ports
    NUMBER_OF_ADCs,     //!< Number of analog inputs
    NUMBER_OF_DACs,     //!< Number of analog outputs
    NUMBER_OF_CANs,     //!< Number of CAN controllers
    NUMBER_OF_DMAs,     //!< Number of DMA channels
    NUMBER_OF_TIMERS,   //!< Number of hardware timers
    NUMBER_OF_UARTs,    //!< Number of USARTs
    NUMBER_OF_SPIs,     //!< Number of SPI devices
    NUMBER_OF_I2Cs,     //!< Number of I2C devices
    TIMERS_BITMASK,     //!< Timer selection bitmask
    SIZEOF_FLASH,       //!< Size of FLASH memory (MB)
    SIZEOF_RAM          //!< Size of RAM memory (KB)
};

/***********************************************************************************/
