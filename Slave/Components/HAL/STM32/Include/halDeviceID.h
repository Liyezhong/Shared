/****************************************************************************/
/*! \file halDeviceID.h
 *
 *  \brief  HAL logical device identifiers
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module defines logical device IDs used by the HAL hardware
 *      abstraction layer to identify the various hardware devices.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef HAL_DEVICES_H
#define HAL_DEVICES_H


//@{**************************************************************************/
//! Logical Device IDs (not function module related)
//****************************************************************************/

// Device IDs for serial busses (I2C, SPI)
#define HAL_BUS_I2C             DEVICEiD(DEVICE_CLASS_I2C, 0)
#define HAL_BUS_I2C_A           HAL_BUS_I2C
#define HAL_BUS_I2C_B           HAL_BUS_I2C+16

#define HAL_BUS_SPI             DEVICEiD(DEVICE_CLASS_SPI, 0)
#define HAL_BUS_SPI_A           HAL_BUS_SPI
#define HAL_BUS_SPI_B           HAL_BUS_SPI+16

// Device IDs for hardware timers
#define HAL_TIMERS              DEVICEiD(DEVICE_CLASS_TIMER, 0)
#define HAL_TIMER_1             HAL_TIMERS
#define HAL_TIMER_2             HAL_TIMERS+1
#define HAL_TIMER_3             HAL_TIMERS+2
#define HAL_TIMER_4             HAL_TIMERS+3
#define HAL_TIMER_5             HAL_TIMERS+4
#define HAL_TIMER_6             HAL_TIMERS+5
#define HAL_TIMER_7             HAL_TIMERS+6
#define HAL_TIMER_8             HAL_TIMERS+7
#define HAL_TIMER_9             HAL_TIMERS+8

// Device IDs for quadrature encoders
#define HAL_ENCODERS            DEVICEiD(DEVICE_CLASS_TIMER, 10)
#define HAL_ENCODER_0           HAL_ENCODERS
#define HAL_ENCODER_1           HAL_ENCODERS+1
#define HAL_ENCODER_2           HAL_ENCODERS+2
#define HAL_ENCODER_3           HAL_ENCODERS+3
#define HAL_ENCODER_4           HAL_ENCODERS+4
#define HAL_ENCODER_5           HAL_ENCODERS+5
#define HAL_ENCODER_6           HAL_ENCODERS+6
#define HAL_ENCODER_7           HAL_ENCODERS+7
#define HAL_ENCODER_8           HAL_ENCODERS+8
#define HAL_ENCODER_9           HAL_ENCODERS+9

// Device IDs for PWM modulators
#define HAL_PWMS                DEVICEiD(DEVICE_CLASS_TIMER, 20)
#define HAL_PWM_0               HAL_PWMS
#define HAL_PWM_1               HAL_PWMS+1
#define HAL_PWM_2               HAL_PWMS+2
#define HAL_PWM_3               HAL_PWMS+3
#define HAL_PWM_4               HAL_PWMS+4
#define HAL_PWM_5               HAL_PWMS+5
#define HAL_PWM_6               HAL_PWMS+6
#define HAL_PWM_7               HAL_PWMS+7
#define HAL_PWM_8               HAL_PWMS+8
#define HAL_PWM_9               HAL_PWMS+9

// Device IDs for external interrupt inputs
#define HAL_INTERRUPTS          DEVICEiD(DEVICE_CLASS_IRQ, 0)
#define HAL_INTERRUPT_0         HAL_INTERRUPTS
#define HAL_INTERRUPT_1         HAL_INTERRUPTS+1
#define HAL_INTERRUPT_2         HAL_INTERRUPTS+2
#define HAL_INTERRUPT_3         HAL_INTERRUPTS+3
#define HAL_INTERRUPT_4         HAL_INTERRUPTS+4
#define HAL_INTERRUPT_5         HAL_INTERRUPTS+5
#define HAL_INTERRUPT_6         HAL_INTERRUPTS+6
#define HAL_INTERRUPT_7         HAL_INTERRUPTS+7
#define HAL_INTERRUPT_8         HAL_INTERRUPTS+8
#define HAL_INTERRUPT_9         HAL_INTERRUPTS+9
//@} End of doxygen group

//@{**************************************************************************/
//! Logical Device IDs for Base Module
//****************************************************************************/

#define HAL_CAN_SYSTEM          DEVICEiD(DEVICE_CLASS_CAN, 0)
#define HAL_CAN_SECONDARY       DEVICEiD(DEVICE_CLASS_CAN, 1)
#define HAL_CAN_NODE_INDEX      DEVICEiD(DEVICE_CLASS_DIGIN, 0)
#define HAL_CAN_LOOPBACK        DEVICEiD(DEVICE_CLASS_DIGOUT, 1)

#define HAL_FRAM_PROTECT        DEVICEiD(DEVICE_CLASS_DIGOUT, 2)
#define HAL_STATUS_LED1         DEVICEiD(DEVICE_CLASS_DIGOUT, 3)
#define HAL_STATUS_LED2         DEVICEiD(DEVICE_CLASS_DIGOUT, 4)
#define HAL_STATUS_LED3         DEVICEiD(DEVICE_CLASS_DIGOUT, 5)

#define HAL_SUPPLY_CURRENT      DEVICEiD(DEVICE_CLASS_ANALOG, 0)
#define HAL_SUPPLY_VOLTAGE      DEVICEiD(DEVICE_CLASS_ANALOG, 5)
#define HAL_SUPPLY_VOLTAGE_1    HAL_SUPPLY_VOLTAGE
#define HAL_SUPPLY_VOLTAGE_2    HAL_SUPPLY_VOLTAGE+1

#define HAL_STORAGE_FRAM        DEVICEiD(DEVICE_CLASS_STORAGE, 0)
#define HAL_STORAGE_OTP         DEVICEiD(DEVICE_CLASS_STORAGE, 1)
#define HAL_STORAGE_FLASH       DEVICEiD(DEVICE_CLASS_STORAGE, 2)
//@} End of doxygen group

//@{**************************************************************************/
//! Logical Device IDs for Digital Input/Output Function Module
//****************************************************************************/

#define HAL_DIGITAL_OUTPUTS     DEVICEiD(DEVICE_CLASS_DIGOUT, 10)
#define HAL_DIGITAL_OUTPUT_0    HAL_DIGITAL_OUTPUTS
#define HAL_DIGITAL_OUTPUT_1    HAL_DIGITAL_OUTPUTS+1
#define HAL_DIGITAL_OUTPUT_2    HAL_DIGITAL_OUTPUTS+2
#define HAL_DIGITAL_OUTPUT_3    HAL_DIGITAL_OUTPUTS+3
#define HAL_DIGITAL_OUTPUT_4    HAL_DIGITAL_OUTPUTS+4
#define HAL_DIGITAL_OUTPUT_5    HAL_DIGITAL_OUTPUTS+5
#define HAL_DIGITAL_OUTPUT_6    HAL_DIGITAL_OUTPUTS+6
#define HAL_DIGITAL_OUTPUT_7    HAL_DIGITAL_OUTPUTS+7
#define HAL_DIGITAL_OUTPUT_8    HAL_DIGITAL_OUTPUTS+8
#define HAL_DIGITAL_OUTPUT_9    HAL_DIGITAL_OUTPUTS+9

#define HAL_DIGITAL_INPUTS      DEVICEiD(DEVICE_CLASS_DIGIN, 10)
#define HAL_DIGITAL_INPUT_0     HAL_DIGITAL_INPUTS
#define HAL_DIGITAL_INPUT_1     HAL_DIGITAL_INPUTS+1
#define HAL_DIGITAL_INPUT_2     HAL_DIGITAL_INPUTS+2
#define HAL_DIGITAL_INPUT_3     HAL_DIGITAL_INPUTS+3
#define HAL_DIGITAL_INPUT_4     HAL_DIGITAL_INPUTS+4
#define HAL_DIGITAL_INPUT_5     HAL_DIGITAL_INPUTS+5
#define HAL_DIGITAL_INPUT_6     HAL_DIGITAL_INPUTS+6
#define HAL_DIGITAL_INPUT_7     HAL_DIGITAL_INPUTS+7
#define HAL_DIGITAL_INPUT_8     HAL_DIGITAL_INPUTS+8
#define HAL_DIGITAL_INPUT_9     HAL_DIGITAL_INPUTS+9
//@} End of doxygen group

//@{**************************************************************************/
//! Logical Device IDs for Analog Input/Output Function Module
//****************************************************************************/

#define HAL_ANALOG_OUTPUTS      DEVICEiD(DEVICE_CLASS_ANALOG, 10)
#define HAL_ANALOG_OUTPUT_0     HAL_ANALOG_OUTPUTS
#define HAL_ANALOG_OUTPUT_1     HAL_ANALOG_OUTPUTS+1
#define HAL_ANALOG_OUTPUT_2     HAL_ANALOG_OUTPUTS+2
#define HAL_ANALOG_OUTPUT_3     HAL_ANALOG_OUTPUTS+3
#define HAL_ANALOG_OUTPUT_4     HAL_ANALOG_OUTPUTS+4
#define HAL_ANALOG_OUTPUT_5     HAL_ANALOG_OUTPUTS+5
#define HAL_ANALOG_OUTPUT_6     HAL_ANALOG_OUTPUTS+6
#define HAL_ANALOG_OUTPUT_7     HAL_ANALOG_OUTPUTS+7
#define HAL_ANALOG_OUTPUT_8     HAL_ANALOG_OUTPUTS+8
#define HAL_ANALOG_OUTPUT_9     HAL_ANALOG_OUTPUTS+9

#define HAL_ANALOG_INPUTS       DEVICEiD(DEVICE_CLASS_ANALOG, 20)
#define HAL_ANALOG_INPUT_0      HAL_ANALOG_INPUTS
#define HAL_ANALOG_INPUT_1      HAL_ANALOG_INPUTS+1
#define HAL_ANALOG_INPUT_2      HAL_ANALOG_INPUTS+2
#define HAL_ANALOG_INPUT_3      HAL_ANALOG_INPUTS+3
#define HAL_ANALOG_INPUT_4      HAL_ANALOG_INPUTS+4
#define HAL_ANALOG_INPUT_5      HAL_ANALOG_INPUTS+5
#define HAL_ANALOG_INPUT_6      HAL_ANALOG_INPUTS+6
#define HAL_ANALOG_INPUT_7      HAL_ANALOG_INPUTS+7
#define HAL_ANALOG_INPUT_8      HAL_ANALOG_INPUTS+8
#define HAL_ANALOG_INPUT_9      HAL_ANALOG_INPUTS+9
//@} End of doxygen group

//@{**************************************************************************/
//! Logical Device IDs for Stepper Motor Function Module
//****************************************************************************/

// Devices used by stepper motor function module
#define HAL_STEPPER_MOTOR       DEVICEiD(DEVICE_CLASS_STEPPER, 0)
#define HAL_STEPPER_MOTOR_0     HAL_STEPPER_MOTOR
#define HAL_STEPPER_MOTOR_1     HAL_STEPPER_MOTOR+1
#define HAL_STEPPER_MOTOR_2     HAL_STEPPER_MOTOR+2

// Timers used by stepper module as time reference
#define HAL_STEPPER_TIMER       HAL_TIMERS+30
#define HAL_STEPPER1_TIMER      HAL_STEPPER_TIMER
#define HAL_STEPPER2_TIMER      HAL_STEPPER_TIMER+1
#define HAL_STEPPER3_TIMER      HAL_STEPPER_TIMER+2
#define HAL_STEPPER4_TIMER      HAL_STEPPER_TIMER+3

// Timers used by stepper module as quadrature encoders
#define HAL_STEPPER_ENCODER     HAL_TIMERS+40
#define HAL_STEPPER1_ENCODER    HAL_STEPPER_ENCODER
#define HAL_STEPPER2_ENCODER    HAL_STEPPER_ENCODER+1
#define HAL_STEPPER3_ENCODER    HAL_STEPPER_ENCODER+2
#define HAL_STEPPER4_ENCODER    HAL_STEPPER_ENCODER+3

// Digital inputs used by HAL stepper module (Limit switch 1)
#define HAL_STEPPER_LIMIT1      HAL_DIGITAL_INPUTS+50
#define HAL_STEPPER1_LIMIT1     HAL_STEPPER_LIMIT1
#define HAL_STEPPER2_LIMIT1     HAL_STEPPER_LIMIT1+1
#define HAL_STEPPER3_LIMIT1     HAL_STEPPER_LIMIT1+2
#define HAL_STEPPER4_LIMIT1     HAL_STEPPER_LIMIT1+3

// Digital inputs used by HAL stepper module (Limit switch 2)
#define HAL_STEPPER_LIMIT2      HAL_DIGITAL_INPUTS+60
#define HAL_STEPPER1_LIMIT2     HAL_STEPPER_LIMIT2
#define HAL_STEPPER2_LIMIT2     HAL_STEPPER_LIMIT2+1
#define HAL_STEPPER3_LIMIT2     HAL_STEPPER_LIMIT2+2
#define HAL_STEPPER4_LIMIT2     HAL_STEPPER_LIMIT2+3

// Digital outputs used by stepper module (driver enable)
#define HAL_STEPPER_ENABLE      HAL_DIGITAL_OUTPUTS+30
#define HAL_STEPPER1_ENABLE     HAL_STEPPER_ENABLE
#define HAL_STEPPER2_ENABLE     HAL_STEPPER_ENABLE+1
#define HAL_STEPPER3_ENABLE     HAL_STEPPER_ENABLE+2
#define HAL_STEPPER4_ENABLE     HAL_STEPPER_ENABLE+3

// Digital outputs used by stepper module (driver chip selects)
#define HAL_STEPPER_SELECT      HAL_DIGITAL_OUTPUTS+40
#define HAL_STEPPER1_SELECT     HAL_STEPPER_SELECT
#define HAL_STEPPER2_SELECT     HAL_STEPPER_SELECT+1
#define HAL_STEPPER3_SELECT     HAL_STEPPER_SELECT+2
#define HAL_STEPPER4_SELECT     HAL_STEPPER_SELECT+3
//@} End of doxygen group

//@{**************************************************************************/
//! Logical Device IDs for UART Function Module
//****************************************************************************/

#define HAL_SERIAL_PORT         DEVICEiD(DEVICE_CLASS_SERIAL, 0)
#define HAL_SERIAL_PORT_0       HAL_SERIAL_PORT
#define HAL_SERIAL_PORT_1       HAL_SERIAL_PORT+1
#define HAL_SERIAL_PORT_2       HAL_SERIAL_PORT+2
#define HAL_SERIAL_PORT_3       HAL_SERIAL_PORT+3
#define HAL_SERIAL_PORT_4       HAL_SERIAL_PORT+4
//@} End of doxygen group

//@{**************************************************************************/
//! Logical Device IDs for SPI Function Module
//****************************************************************************/

#define HAL_SPI_SELECT          HAL_DIGITAL_OUTPUTS+100
#define HAL_SPI_SELECT_0        HAL_SPI_SELECT
#define HAL_SPI_SELECT_1        HAL_SPI_SELECT+1
#define HAL_SPI_SELECT_2        HAL_SPI_SELECT+2
//@} End of doxygen group

//@{**************************************************************************/
//! Logical Device IDs for RFID Function Module
//****************************************************************************/

// Channels used by the RFID ISO 11785 function module (shutdowns)
#define HAL_RFID11785_SHD       HAL_DIGITAL_OUTPUTS+70
#define HAL_RFID11785_SHD_0     HAL_RFID11785_SHD
#define HAL_RFID11785_SHD_1     HAL_RFID11785_SHD+1
#define HAL_RFID11785_SHD_2     HAL_RFID11785_SHD+2
#define HAL_RFID11785_SHD_3     HAL_RFID11785_SHD+3

// Channels used by the RFID ISO 11785 function module (modulating data)
#define HAL_RFID11785_MOD       HAL_DIGITAL_OUTPUTS+74
#define HAL_RFID11785_MOD_0     HAL_RFID11785_MOD
#define HAL_RFID11785_MOD_1     HAL_RFID11785_MOD+1
#define HAL_RFID11785_MOD_2     HAL_RFID11785_MOD+2
#define HAL_RFID11785_MOD_3     HAL_RFID11785_MOD+3

// Channels used by the RFID ISO 11785 function module (antenna select)
#define HAL_RFID11785_ANT       HAL_DIGITAL_OUTPUTS+78
#define HAL_RFID11785_ANT_0     HAL_RFID11785_ANT
#define HAL_RFID11785_ANT_1     HAL_RFID11785_ANT+1
#define HAL_RFID11785_ANT_2     HAL_RFID11785_ANT+2
#define HAL_RFID11785_ANT_3     HAL_RFID11785_ANT+3

// Channels used by the RFID ISO 11785 function module (capture/compare timer)
#define HAL_RFID11785_TIM       HAL_TIMERS+50
#define HAL_RFID11785_TIM_0     HAL_RFID11785_TIM
#define HAL_RFID11785_TIM_1     HAL_RFID11785_TIM+1
#define HAL_RFID11785_TIM_2     HAL_RFID11785_TIM+2
#define HAL_RFID11785_TIM_3     HAL_RFID11785_TIM+3

// Channels used by the RFID ISO/IEC 15693 function module (enable)
#define HAL_RFID15693_EN        HAL_DIGITAL_OUTPUTS+82
#define HAL_RFID15693_EN_0      HAL_RFID15693_EN
#define HAL_RFID15693_EN_1      HAL_RFID15693_EN+1
#define HAL_RFID15693_EN_2      HAL_RFID15693_EN+2
#define HAL_RFID15693_EN_3      HAL_RFID15693_EN+3

// Channels used by the RFID ISO/IEC 15693 function module (SPI data / field modulation)
#define HAL_RFID15693_OUT       HAL_DIGITAL_OUTPUTS+86
#define HAL_RFID15693_OUT_0     HAL_RFID15693_OUT
#define HAL_RFID15693_OUT_1     HAL_RFID15693_OUT+1
#define HAL_RFID15693_OUT_2     HAL_RFID15693_OUT+2
#define HAL_RFID15693_OUT_3     HAL_RFID15693_OUT+3

// Channels used by the RFID ISO/IEC 15693 function module (capture/compare timer)
#define HAL_RFID15693_TIM       HAL_TIMERS+60
#define HAL_RFID15693_TIM_0     HAL_RFID15693_TIM
#define HAL_RFID15693_TIM_1     HAL_RFID15693_TIM+1
#define HAL_RFID15693_TIM_2     HAL_RFID15693_TIM+2
#define HAL_RFID15693_TIM_3     HAL_RFID11785_TIM+3
//@} End of doxygen group

//@{**************************************************************************/
//! Logical Device IDs for Temperature Function Module
//****************************************************************************/
// Channels used by the temperature function module (fan controls)
#define HAL_TEMP_FANCONTROL     HAL_DIGITAL_OUTPUTS+80
#define HAL_TEMP_FANCONTROL_0   HAL_TEMP_FANCONTROL
#define HAL_TEMP_FANCONTROL_1   HAL_TEMP_FANCONTROL+1
#define HAL_TEMP_FANCONTROL_2   HAL_TEMP_FANCONTROL+2
#define HAL_TEMP_FANCONTROL_3   HAL_TEMP_FANCONTROL+3

// Channels used by the temperature function module (main voltage switch)
#define HAL_TEMP_MAINVOLTAGE    HAL_DIGITAL_OUTPUTS+84
#define HAL_TEMP_MAINVOLTAGE_0  HAL_TEMP_MAINVOLTAGE
#define HAL_TEMP_MAINVOLTAGE_1  HAL_TEMP_MAINVOLTAGE+1
#define HAL_TEMP_MAINVOLTAGE_2  HAL_TEMP_MAINVOLTAGE+2
#define HAL_TEMP_MAINVOLTAGE_3  HAL_TEMP_MAINVOLTAGE+3

// Channels used by the temperature function module (fan speed sensor)
#define HAL_TEMP_FANSPEED       HAL_TIMERS+70
#define HAL_TEMP_FANSPEED_0     HAL_TEMP_FANSPEED
#define HAL_TEMP_FANSPEED_1     HAL_TEMP_FANSPEED+1
#define HAL_TEMP_FANSPEED_2     HAL_TEMP_FANSPEED+2
#define HAL_TEMP_FANSPEED_3     HAL_TEMP_FANSPEED+3

// Channels used by the temperature function module (control heating element)
#define HAL_TEMP_CTRLHEATING    HAL_DIGITAL_OUTPUTS+88
#define HAL_TEMP_CTRLHEATING_0  HAL_TEMP_CTRLHEATING
#define HAL_TEMP_CTRLHEATING_1  HAL_TEMP_CTRLHEATING+1
#define HAL_TEMP_CTRLHEATING_2  HAL_TEMP_CTRLHEATING+2
#define HAL_TEMP_CTRLHEATING_3  HAL_TEMP_CTRLHEATING+3

// Channels used by the temperature function module (temperature sensors)
#define HAL_TEMP_SENSOR         HAL_ANALOG_INPUTS+10
#define HAL_TEMP_SENSOR_0       HAL_TEMP_SENSOR
#define HAL_TEMP_SENSOR_1       HAL_TEMP_SENSOR+1
#define HAL_TEMP_SENSOR_2       HAL_TEMP_SENSOR+2
#define HAL_TEMP_SENSOR_3       HAL_TEMP_SENSOR+3
#define HAL_TEMP_SENSOR_4       HAL_TEMP_SENSOR+4
#define HAL_TEMP_SENSOR_5       HAL_TEMP_SENSOR+5
#define HAL_TEMP_SENSOR_6       HAL_TEMP_SENSOR+6
#define HAL_TEMP_SENSOR_7       HAL_TEMP_SENSOR+7

// Channels used by the temperature function module (current measurement)
#define HAL_TEMP_CURRENT        HAL_ANALOG_INPUTS+18
#define HAL_TEMP_CURRENT_0      HAL_TEMP_CURRENT
#define HAL_TEMP_CURRENT_1      HAL_TEMP_CURRENT+1
#define HAL_TEMP_CURRENT_2      HAL_TEMP_CURRENT+2
#define HAL_TEMP_CURRENT_3      HAL_TEMP_CURRENT+3

// Channels used by the temperature function module (temperature compensation)
#define HAL_TEMP_COMPENSATE     HAL_ANALOG_INPUTS+22
#define HAL_TEMP_COMPENSATE_0   HAL_TEMP_COMPENSATE
#define HAL_TEMP_COMPENSATE_1   HAL_TEMP_COMPENSATE+1
#define HAL_TEMP_COMPENSATE_2   HAL_TEMP_COMPENSATE+2
#define HAL_TEMP_COMPENSATE_3   HAL_TEMP_COMPENSATE+3
//@} End of doxygen group

//@{**************************************************************************/
//! Logical Device IDs for Joystick Function Module
//****************************************************************************/
// Channels used by the joystick function module (X-axis)
#define HAL_JOYSTICK_XAXIS      HAL_ANALOG_INPUTS+26
#define HAL_JOYSTICK_XAXIS_0    HAL_JOYSTICK_XAXIS
#define HAL_JOYSTICK_XAXIS_1    HAL_JOYSTICK_XAXIS+1
#define HAL_JOYSTICK_XAXIS_2    HAL_JOYSTICK_XAXIS+2
#define HAL_JOYSTICK_XAXIS_3    HAL_JOYSTICK_XAXIS+3

// Channels used by the joystick function module (Y-axis)
#define HAL_JOYSTICK_YAXIS      HAL_ANALOG_INPUTS+30
#define HAL_JOYSTICK_YAXIS_0    HAL_JOYSTICK_YAXIS
#define HAL_JOYSTICK_YAXIS_1    HAL_JOYSTICK_YAXIS+1
#define HAL_JOYSTICK_YAXIS_2    HAL_JOYSTICK_YAXIS+2
#define HAL_JOYSTICK_YAXIS_3    HAL_JOYSTICK_YAXIS+3
//@} End of doxygen group


//@{**************************************************************************/
//! Logical Device IDs for Pressure Function Module
//****************************************************************************/
// Channels used by the pressure function module (fan controls)
#define HAL_PRESS_FANCONTROL     HAL_DIGITAL_OUTPUTS+92
#define HAL_PRESS_FANCONTROL_0   HAL_PRESS_FANCONTROL
#define HAL_PRESS_FANCONTROL_1   HAL_PRESS_FANCONTROL+1
#define HAL_PRESS_FANCONTROL_2   HAL_PRESS_FANCONTROL+2
#define HAL_PRESS_FANCONTROL_3   HAL_PRESS_FANCONTROL+3

// Channels used by the pressure function module (main voltage switch)
#define HAL_PRESS_MAINVOLTAGE    HAL_DIGITAL_OUTPUTS+96
#define HAL_PRESS_MAINVOLTAGE_0  HAL_PRESS_MAINVOLTAGE
#define HAL_PRESS_MAINVOLTAGE_1  HAL_PRESS_MAINVOLTAGE+1
#define HAL_PRESS_MAINVOLTAGE_2  HAL_PRESS_MAINVOLTAGE+2
#define HAL_PRESS_MAINVOLTAGE_3  HAL_PRESS_MAINVOLTAGE+3

// Channels used by the pressure function module (fan speed sensor)
#define HAL_PRESS_FANSPEED       HAL_TIMERS+74
#define HAL_PRESS_FANSPEED_0     HAL_PRESS_FANSPEED
#define HAL_PRESS_FANSPEED_1     HAL_PRESS_FANSPEED+1
#define HAL_PRESS_FANSPEED_2     HAL_PRESS_FANSPEED+2
#define HAL_PRESS_FANSPEED_3     HAL_PRESS_FANSPEED+3

// Channels used by the pressure function module (control pumping element)
#define HAL_PRESS_CTRLPUMPING    HAL_DIGITAL_OUTPUTS+100
#define HAL_PRESS_CTRLPUMPING_0  HAL_PRESS_CTRLPUMPING
#define HAL_PRESS_CTRLPUMPING_1  HAL_PRESS_CTRLPUMPING+1
#define HAL_PRESS_CTRLPUMPING_2  HAL_PRESS_CTRLPUMPING+2
#define HAL_PRESS_CTRLPUMPING_3  HAL_PRESS_CTRLPUMPING+3

// Channels used by the pressure function module (pressure sensors)
#define HAL_PRESS_SENSOR         HAL_ANALOG_INPUTS+34
#define HAL_PRESS_SENSOR_0       HAL_PRESS_SENSOR
#define HAL_PRESS_SENSOR_1       HAL_PRESS_SENSOR+1
#define HAL_PRESS_SENSOR_2       HAL_PRESS_SENSOR+2
#define HAL_PRESS_SENSOR_3       HAL_PRESS_SENSOR+3
#define HAL_PRESS_SENSOR_4       HAL_PRESS_SENSOR+4
#define HAL_PRESS_SENSOR_5       HAL_PRESS_SENSOR+5
#define HAL_PRESS_SENSOR_6       HAL_PRESS_SENSOR+6
#define HAL_PRESS_SENSOR_7       HAL_PRESS_SENSOR+7

// Channels used by the pressure function module (current measurement)
#define HAL_PRESS_CURRENT        HAL_ANALOG_INPUTS+42
#define HAL_PRESS_CURRENT_0      HAL_PRESS_CURRENT
#define HAL_PRESS_CURRENT_1      HAL_PRESS_CURRENT+1
#define HAL_PRESS_CURRENT_2      HAL_PRESS_CURRENT+2
#define HAL_PRESS_CURRENT_3      HAL_PRESS_CURRENT+3

// Channels used by the pressure function module (current measurement)
#define HAL_PRESS_FANCURRENT    HAL_ANALOG_INPUTS+46
#define HAL_PRESS_FANCURRENT_0  HAL_PRESS_FANCURRENT
#define HAL_PRESS_FANCURRENT_1  HAL_PRESS_FANCURRENT+1
#define HAL_PRESS_FANCURRENT_2  HAL_PRESS_FANCURRENT+2
#define HAL_PRESS_FANCURRENT_3  HAL_PRESS_FANCURRENT+3

// Channels used by the pressure function module (control valve element)
#define HAL_PRESS_CTRLVALVE      HAL_DIGITAL_OUTPUTS+104
#define HAL_PRESS_CTRLVALVE_0    HAL_PRESS_CTRLVALVE
#define HAL_PRESS_CTRLVALVE_1    HAL_PRESS_CTRLVALVE+1
#define HAL_PRESS_CTRLVALVE_2    HAL_PRESS_CTRLVALVE+2

// Channels used by the pressure function module (PWM output control)
#define HAL_PRESS_PWM_CTRL       HAL_ANALOG_OUTPUTS+10
#define HAL_PRESS_PWM_CTRL_0     HAL_PRESS_PWM_CTRL
#define HAL_PRESS_PWM_CTRL_1     HAL_PRESS_PWM_CTRL+1
#define HAL_PRESS_PWM_CTRL_2     HAL_PRESS_PWM_CTRL+2

//****************************************************************************/

#endif /*HAL_DEVICES_H*/
