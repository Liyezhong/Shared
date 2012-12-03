/****************************************************************************/
/*! \file halTest.c
 * 
 *  \brief  Hardware abstraction layer test module
 *
 *  $Version: $ 0.1
 *  $Date:    $ 04.12.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to test the hardware abstraction
 *      layer. It is not part of the HAL.
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

#include <stdio.h>
#include "Global.h"
#include "halCommon.h"
#include "halDeviceID.h"
#include "halError.h"
#include "halInterrupt.h"
#include "halUtilities.h"
#include "halStorage.h"
#include "halFlash.h"
#include "halPorts.h"
#include "halAnalog.h"
#include "halConsole.h"
#include "halSysTick.h"
#include "halRealTime.h"
#include "halWatchdog.h"
#include "halTimer.h"
#include "halExtIntr.h"
#include "halSerial.h"
#include "halStepper.h"
#include "halClocks.h"
#include "halCan.h"
#include "halPwm.h"
#include "halI2cBus.h"
#include "halSpiBus.h"
#include "halMain.h"
#include "halEncoder.h"
#include "halConfigure.h"

#pragma import(__use_no_semihosting)

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static volatile UInt32 InterruptCounter = 0;
static volatile UInt32 CapComInterval   = 0;
static volatile UInt32 CapComErrors     = 0;
static Handle_t Handle;


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static void PrintHeader (char *String) {

    printf ("\n\n=========================================\n");
    printf (String);
    printf ("\n=========================================\n");
}

//****************************************************************************/

static void halLongDelay (UInt32 Milliseconds) {

    UInt32 i;

    for (i=0; i < Milliseconds * 2; i++) {
        halShortDelay (500);
    }
}
//****************************************************************************/

static void DelayInMicroSeconds (UInt32 MicroSeconds) {

    UInt32 Loops = MicroSeconds / 500;

    while (Loops--) {
        halShortDelay (500);
    }
    halShortDelay (MicroSeconds % 500);
}

//****************************************************************************/

static void ShowResetReason (void) {

    ResetReason_t ResetReason = halGetResetReason();

    PrintHeader ("Request reason of last reset");

    switch (ResetReason) {
        case RESET_CAUSE_UNKNOWN:
            printf ("Unknown reset cause"); 
            break;

        case RESET_CAUSED_BY_POWER_ON:
            printf ("Reset caused by normal power up"); 
            break;

        case RESET_CAUSED_BY_HARDWARE:
            printf ("Reset caused by hardware pin"); 
            break;

        case RESET_CAUSED_BY_SOFTWARE:
            printf ("Reset caused by software"); 
            break;

        case RESET_CAUSED_BY_WATCHDOG:
            printf ("Reset caused by watchdog timeout"); 
            break;

	    case RESET_CAUSED_BY_STANDBY:
            printf ("Reset caused by entering standby/stop"); 
            break;

        default:
            printf ("ERROR: Unexpected value"); 
            break;
    }
}

//****************************************************************************/

static void TestDigitalOutput (void) {

    Handle_t Handle;
    Error_t Status;
    UInt32 i;

    PrintHeader ("Testing digital outputs (LEDs)");

    Handle = halPortOpen (HAL_DIGITAL_OUTPUT_0, HAL_OPEN_RW);
    if (Handle > 0) {
        for (i=0; i < 64; i++) {
            Status = halPortWrite (Handle, i);
            if (Status < 0) {
                printf ("halDigitalWrite() failed: %d\n", Status);
            }            
            halLongDelay (100);
        }
        Status = halPortClose (Handle);
        if (Status < 0) {
            printf ("halDigitalClose() failed: %d\n", Status);
        }
    }
    else {
        printf ("halDigitalOpen() failed: %d\n", Handle);
    }
}

//****************************************************************************/

static void TestDigitalInput (void) {

    const char *Keynames[] = {
        "Enter", "Up", "Right", "Down", "Left", "User", "Wakeup", "Tamper" };
    UInt16 newState[8], oldState[8];
    Handle_t Handle[8];
    Error_t errCode;
    UInt32 i;

    PrintHeader ("Testing digital inputs");

    for (i=0; i < ELEMENTS(Handle); i++) {
        Handle[i] = halPortOpen (HAL_DIGITAL_INPUTS+i, HAL_OPEN_READ);
        if (Handle[i] < 0) {
            printf ("halPortOpen(%d) failed\n", i);
        }
        newState[i] = oldState[i] = 0;
    }
    do {
        for (i=0; i < ELEMENTS(Handle); i++) {
            errCode = halPortRead (Handle[i], &newState[i]);
            if (errCode < 0) {
                printf ("halPortRead(%d) failed: %d\n", i, errCode);
            }
            if (newState[i] != oldState[i]) {
                printf ("- %s key %s\n", Keynames[i], newState[i] ? "pressed" : "released");
                oldState[i] = newState[i];
            }
        }

    } while (halConsoleRead() != 0x1b);

    for (i=0; i < ELEMENTS(Handle); i++) {
        errCode = halPortClose (Handle[i]);
        if (errCode < 0) {
            printf ("halPortClose(%d) failed: %d\n", i, errCode);
        }
    }
}

//****************************************************************************/

static void TestAnalogInput (void) {

    UInt16 newData = 0, oldData = 0;
    Handle_t Handle;

    PrintHeader ("Testing analog input (poti)");

    Handle = halAnalogOpen (HAL_ANALOG_INPUT_0, HAL_OPEN_READ);

    if (Handle > 0) {
        if (halAnalogWrite (Handle, newData) == NO_ERROR) {
            printf ("Writing to an input should fail\n");
        }
        do {
            // read analog input from poti
            if (halAnalogRead (Handle, &newData) != NO_ERROR) {
                printf ("Reading analog input failed\n");
            }
            newData = (newData + 5) / 10;

            if (newData != oldData) {
                printf ("Poti = %d%%  \r", newData);
            }
            oldData = newData;
            halLongDelay (100);

        } while (halConsoleRead() != 0x1b);

        halAnalogClose (Handle);
    }
    else {
        printf ("halAnalogOpen() failed: %d\n", Handle);
    }
}

//****************************************************************************/

static void OutputSawtooth (Device_t DeviceID, UInt32 Frequency, UInt32 Duration) {

    UInt32 StepTime = 1000000 / (Frequency * 1000 / 50);
    Handle_t Handle;
    int i;

    Handle = halAnalogOpen (DeviceID, HAL_OPEN_RW);
    if (Handle > 0) {
        UInt32 StartTime = halSysTickRead();

        while (halSysTickRead() - StartTime < Duration) {

            for (i=0; i <= 1000; i+=50) {
                halAnalogWrite (Handle, i);
                halShortDelay (StepTime);
            }
        }
        halAnalogWrite (Handle, 0);
        halAnalogClose (Handle);
     }
}

//****************************************************************************/

static void TestAnalogOutput (Device_t DeviceID, UInt16 Retries) {

    Handle_t Handle;
    UInt16 Data = 0;
    UInt16 i, k;

    PrintHeader ("Testing analog output");

    for (k=0; k < Retries; k++) {
        for (i=200; i < 600; i+=100) {
            OutputSawtooth (DeviceID, i, 200);
        }
    }
    Handle = halAnalogOpen (DeviceID, HAL_OPEN_RW);
    if (Handle > 0) {
        for (i=0; i <= 100; i+=10) {
            halAnalogWrite (Handle, i);
            halAnalogRead (Handle, &Data);
            if (Data != i) {
                printf ("Reading output value failed\n");
            }
        }
        halAnalogClose (Handle);
    }
}

//****************************************************************************/

static void TestShortDelay (void) {

    UInt32 Tick1, Tick2;
    UInt32 i;

    PrintHeader ("Testing short delay");

    Tick1 = halSysTickRead();

    for (i=0; i < 10000; i++) {
        halShortDelay (500);
    }
    Tick2 = halSysTickRead();

    if (Tick2 - Tick1 - (500 * 10000/1000) > 2) {
        printf ("ShortDelay not exact\n");
    }
    halShortDelay (500);
}

//****************************************************************************/

static void TestCoreVoltage (void) {

    Bool State, oldState = FALSE;
    UInt32 Counter = 0;

    PrintHeader ("Testing core voltage monitor");
    do {
        State = halPowerGood();
        if (State != oldState) {
            if (State) 
                printf ("- Power is good\n");
            else
                printf ("- Power is too low\n");

            oldState = State;
            Counter++;
        }
    } while (halConsoleRead() != 0x1b);
}

//****************************************************************************/

static void TestProcessorInfo (void) {

    char *Processor, *Density;

    PrintHeader ("Processor Info");

    switch (halProcessorInfo.Processor) {
        case DEVICE_FAMILY_F100:
            Processor = "STM32F100"; break;
        case DEVICE_FAMILY_F101:
            Processor = "STM32F101"; break;
        case DEVICE_FAMILY_F102:
            Processor = "STM32F102"; break;
        case DEVICE_FAMILY_F103:
            Processor = "STM32F103"; break;
        case DEVICE_FAMILY_F105:
            Processor = "STM32F105"; break;
        default:
            Processor = "STM32F???"; break;
    }
    switch (halProcessorInfo.DensityID) {
        case DEVICE_DENSITY_LOW:
            Density = "Low"; break;
        case DEVICE_DENSITY_MEDIUM:
            Density = "Medium"; break;
        case DEVICE_DENSITY_HIGH:
            Density = "High"; break;
        case DEVICE_DENSITY_XL:
            Density = "XL"; break;
        default:
            Density = "??"; break;
    }
    printf ("Processor:\t%s %s Density\n", Processor, Density);
    printf ("Clockrate:\t%dMHz\n", halProcessorInfo.ClockRate);
    printf ("GPIO Count:\t%d\n", halProcessorInfo.CountGPIO);
    printf ("Port Count:\t%d\n", halProcessorInfo.CountPort);
    printf ("ADC Channels:\t%d\n", halProcessorInfo.CountADC);
    printf ("DAC Channels:\t%d\n", halProcessorInfo.CountDAC);
    printf ("CAN Count:\t%d\n", halProcessorInfo.CountCAN);

    printf ("DMA Count:\t%d\n", halProcessorInfo.CountDMA);
    printf ("Timer Count:\t%d\n", halProcessorInfo.CountTimer);
    printf ("UART Count:\t%d\n", halProcessorInfo.CountUART);
    printf ("SPI Count:\t%d\n", halProcessorInfo.CountSPI);
    printf ("I2C Count:\t%d\n", halProcessorInfo.CountI2C);
    printf ("FLASH Size:\t%dkByte\n", halProcessorInfo.SizeFLASH);
    printf ("RAM Size:\t%dkByte\n", halProcessorInfo.SizeRAM);
}


//****************************************************************************/

static void TestWatchdog (void) {

    int i;

    PrintHeader ("Testing watchdog");

    if (halWatchdogEnable() < 0) {
        printf ("halWatchdogEnable() failed\n");
    }
    if (halWatchdogDisable() == NO_ERROR) {
        printf ("halWatchdogDisble() should fail, but succeeds\n");
    }
    for (i=0; i < 10; i++) {
        if (halWatchdogTrigger() != NO_ERROR) {
            printf ("halWatchdogTrigger() failed\n");
        }
        halLongDelay (1000);
    }
    // Watchdog reset should come when i=5
    for (i=1; i < 10; i++) {
        if (halWatchdogTrigger() != NO_ERROR) {
            printf ("halWatchdogTrigger() failed\n");
        }
        halLongDelay (i * 500);
    }
}

//****************************************************************************/

static void TestHardwareInfos (void) {

    const UInt8 *Buffer; 
    UInt16 Revision;
    char *Family;
    UInt32 Size;
    int i;

    PrintHeader ("Reading hardware infos");

    switch (halGetDeviceID(&Revision)) {
        case DEVICE_F100_MEDIUM:
            Family = "F100 Medium Density"; break;
        case DEVICE_F100_HIGH:
            Family = "F100 High Density"; break;
        case DEVICE_F10x_MEDIUM:
            Family = "F10x Medium Density"; break;
        case DEVICE_F10x_HIGH:
            Family = "F10x High Density"; break;
        case DEVICE_F10x_LOW:
            Family = "F10x Low Density"; break;
        case DEVICE_F10x_XL:
            Family = "F10x XL Density"; break;
        case DEVICE_F105:
            Family = "Connectivity line"; break;
        default:
            Family = "Unknown family"; break;
    }
    printf ("- Processor type:\t %s device\n", Family);
    printf ("- Processor revision:\t %x\n", Revision);

    Size = halGetUniqueID(&Buffer);
    if (Size != SIZEOF_UNIQUE_DEVICE_ID/8) {
        printf ("halGetUniqueID() returns unexpected length\n");
    }
    printf ("- Unique device ID:\t 0x");
    for (i=0; i < Size; i++) {
        printf ("%02x", Buffer[i]);
    }
    printf ("\n");

    Size = halGetMemorySize(MEMORY_SIZE_FLASH);
    printf ("- Size of FLASH memory:\t %d Bytes\n", Size);
    Size = halGetMemorySize(MEMORY_SIZE_RAM);
    printf ("- Size of RAM memory:\t %d Bytes\n", Size);
}


//****************************************************************************/

static UInt32 GetTimerCountPerTime (Handle_t Handle, UInt32 Time) {

    UInt32 Count1, Count2;

    if (halTimerRead (Handle, TIM_REG_COUNTER, &Count1) != NO_ERROR) {
        printf ("halTimerRead() failed\n");
    }
    halLongDelay (Time);

    if (halTimerRead (Handle, TIM_REG_COUNTER, &Count2) != NO_ERROR) {
        printf ("halTimerRead() failed\n");
    }
    return (Count2 - Count1);
}

//****************************************************************************/

static void PrintTimerStatus (Handle_t Handle) {

    Error_t Status;
    UInt32 Value;

    Status = halTimerStatus (Handle, TIM_STAT_COUNTRATE);
    printf ("Timer count rate:\t %d\n", Status);

    Status = halTimerStatus (Handle, TIM_STAT_CLOCKRATE);
    printf ("Timer clock rate:\t %d\n", Status);

    Status = halTimerRead (Handle, TIM_REG_RELOAD, &Value);
    printf ("Timer interval: \t %d\n", Value);

    Status = halTimerRead (Handle, TIM_REG_PRESCALER, &Value);
    printf ("Timer prescaler:\t %d\n", Value);

    Status = halTimerStatus (Handle, TIM_STAT_MAXCOUNT);
    printf ("Timer max count:\t %d\n", Status);

    Status = halTimerStatus (Handle, TIM_STAT_UNITS);
    printf ("Capture/Compare Units:\t %d\n", Status);
}

//****************************************************************************/

static void TimerInterruptHandler (UInt32 UserTag, UInt32 IntrFlags) {

    if (UserTag == 0x99) {
        InterruptCounter++;
    }
}

//****************************************************************************/

static void TestTimer (void) {

    TimerMode_t TimerMode = {
        TIM_MODE_COUNT_DOWN, TIM_MODE_INTERVAL, TIM_MODE_INTERNAL, 0
    };
    const UInt16 Prescale = 36000;

    PrintHeader ("Testing Timer");

    Handle = halTimerOpen (HAL_STEPPER1_TIMER, 0x99, TimerInterruptHandler);
    if (Handle >= 0) {
        InterruptCounter = 0;

        if (halTimerSetup (Handle, &TimerMode, Prescale) < 0) {
            printf ("halTimerSetup() failed\n");
        }
        if (halTimerWrite (Handle, TIM_REG_RELOAD, 1000) != NO_ERROR) {
            printf ("halTimerWrite() failed\n");
        }
        if (GetTimerCountPerTime(Handle, 500) != 0) {
            printf ("Timer running w/o being started\n");
        }
        if (halTimerControl (Handle, TIM_INTR_ENABLE) < 0) {
            printf ("halTimerControl() failed\n");
        }
        if (halTimerControl (Handle, TIM_CTRL_START) != NO_ERROR) {
            printf ("halTimerControl() failed\n");
        }
        if (GetTimerCountPerTime(Handle, 20) == 0) {
            printf ("Timer not running after being started\n");
        }
        PrintTimerStatus (Handle);
        halLongDelay (5000);
        PrintTimerStatus (Handle);

        if (halTimerControl (Handle, TIM_CTRL_STOP) != NO_ERROR) {
            printf ("halTimerControl() failed\n");
        }
        if (GetTimerCountPerTime(Handle, 500) > 0) {
            printf ("Timer running after being stopped\n");
        }
        if (InterruptCounter < 10) {
            printf ("Not enough Timer interrupt detected\n");
        }
        halLongDelay (500);
        halTimerClose (Handle);
    }
    else {
        printf ("halTimerOpen() failed: %d\n", Handle);
    }
}

//****************************************************************************/

static void TestTimerExternalClock (void) {

    TimerMode_t TimerMode = {
        TIM_MODE_COUNT_UP, TIM_MODE_INTERVAL, TIM_MODE_EXTERNAL, 7
    };
    UInt32 Count1, Count2;

    PrintHeader ("Testing Timer (external clock)");

    Handle = halTimerOpen (HAL_TIMER_1, 0x99, TimerInterruptHandler);
    if (Handle >= 0) {
        if (halTimerSetup (Handle, &TimerMode, 2) < 0) {
            printf ("halTimerSetup() failed\n");
        }
        if (halTimerControl (Handle, TIM_INTR_ENABLE) < 0) {
            printf ("halTimerControl() failed\n");
        }
        if (halTimerWrite (Handle, TIM_REG_RELOAD, 0xFFF0) != NO_ERROR) {
            printf ("halTimerWrite() failed\n");
        }
        if (halTimerControl (Handle, TIM_CTRL_START) != NO_ERROR) {
            printf ("halTimerControl() failed\n");
        }
        do {
            if (halTimerRead (Handle, TIM_REG_COUNTER, &Count1) != NO_ERROR) {
                printf ("halTimerRead() failed\n");
            }
            halLongDelay (1000);
    
            if (halTimerRead (Handle, TIM_REG_COUNTER, &Count2) != NO_ERROR) {
                printf ("halTimerRead() failed\n");
            }
            Count1 = (Count2 - Count1) * 60;
            printf ("FAN Speed = %d U/min\n", Count1);

        } while (halConsoleRead() != 0x1b);

        halTimerClose (Handle);
    }
    else {
        printf ("halTimerOpen() failed: %d\n", Handle);
    }
}

//****************************************************************************/

static void TimerCaptureInterrupt (UInt32 UserTag, UInt32 IntrFlags) {

    static UInt32 newCount = 0, oldCount = 0;

    if (halCapComRead (Handle, 0, &newCount) == 1) {
        CapComInterval = newCount - oldCount;
    }
    else {
        CapComErrors++;
    }
    oldCount = newCount;
}

//****************************************************************************/

static void TestTimerMeasureFrequency (void) {

    TimerMode_t TimerMode = {
        TIM_MODE_COUNT_UP, TIM_MODE_INTERVAL, TIM_MODE_INTERNAL, 0
    };
    UInt32 oldCapComInterval = 0;

    PrintHeader ("Testing Capture Unit (Fan sensor on PE9)");
    CapComInterval = CapComErrors = 0;

    Handle = halTimerOpen (HAL_TIMER_1, Handle, TimerCaptureInterrupt);
    if (Handle >= 0) {
        if (halTimerSetup (Handle, &TimerMode, 2) < 0) {
            printf ("halTimerSetup() failed\n");
        } 
        if (halCapComControl (Handle, 0, TIM_INTR_ENABLE) < 0) {
            printf ("halTimerIntrCtrl() failed\n");
        }
        if (halTimerWrite (Handle, TIM_REG_RELOAD, 0xFFFF) != NO_ERROR) {
            printf ("halTimerWrite() failed\n");
        }
        if (halTimerControl (Handle, TIM_CTRL_START) != NO_ERROR) {
            printf ("halTimerControl() failed\n");
        }
        do {
            //halLongDelay (1000);            
            if (CapComInterval != oldCapComInterval) {
                printf ("FAN Speed = %u U/min (%u, %u)\n", 
                    (600000UL / CapComInterval), CapComInterval, CapComErrors);
                oldCapComInterval = CapComInterval;
            }
            CapComErrors = 0;

        } while (halConsoleRead() != 0x1b);

        halTimerClose (Handle);
    }
    else {
        printf ("halTimerOpen() failed: %d\n", Handle);
    }
}

//****************************************************************************/

static void TestTimerPwmOutput (void) {

    TimerMode_t TimerMode = {
        TIM_MODE_COUNT_UP, TIM_MODE_INTERVAL, TIM_MODE_INTERNAL, 0
    };
    const UInt32 Increments = 10;
    UInt16 PwmValue = 0;
    int i;

    PrintHeader ("Testing PWM Output (4 channels)");

    Handle = halTimerOpen (HAL_TIMER_3, 0, NULL);
    if (Handle >= 0) {
        if (halTimerSetup (Handle, &TimerMode, 1) != NO_ERROR) {
            printf ("halTimerSetup() failed\n");
        }
        if (halTimerWrite (Handle, TIM_REG_RELOAD, 0xFFFF) != NO_ERROR) {
            printf ("halTimerWrite() failed\n");
        }
        if (halTimerControl (Handle, TIM_CTRL_START) != NO_ERROR) {
            printf ("halTimerControl() failed\n");
        }
        // Set the 4 PWM channels to 20%, 40%, 60%, 80% PWM
        for (i=0; i < 4; i++) {
            PwmValue = (i + 1) * MAX_UINT16 * 20 / 100;
            if (halCapComWrite (Handle, i, PwmValue) != NO_ERROR) {
                printf ("halCapComWrite() failed\n");
            }
        }
        halLongDelay (1000);                

        // Increment PWM on channel 0 from 0 to max until ESC is presssed
        do {
            for (i=0; i < Increments; i++) {
                PwmValue = i * MAX_UINT16 / Increments;
    
                if (halCapComWrite (Handle, 0, PwmValue) != NO_ERROR) {
                    printf ("halCapComWrite() failed\n");
                }
                halLongDelay (1000);                
            }
        } while (halConsoleRead() != 0x1b);

        halTimerClose (Handle);
    }
    else {
        printf ("halTimerOpen() failed: %d\n", Handle);
    }
}

//****************************************************************************/

static void EncoderInterrupt (UInt32 UserTag, UInt32 IntrFlags) {

    UInt32 Position = 0;

    if (IntrFlags & ENC_IFLAG_LIMIT1) {
        printf ("Reference position 1 reached\n");
    }
    if (halEncoderStatus (Handle, ENC_STAT_LIMITS) > 1) {
        if (IntrFlags & ENC_IFLAG_LIMIT2) {
            printf ("Reference position 2 reached\n");
        }
    }
    if (halEncoderStatus (Handle, ENC_STAT_INDEX)) {
        if (IntrFlags & ENC_IFLAG_INDEX) {
            halEncoderRead (Handle, ENC_REG_INDEX, &Position);
            printf ("Encoder index pulse: %d\n", Position);
        }
    }
    if (IntrFlags & ENC_IFLAG_OVERFLOW) {
        printf ("Encoder underflow\n");
    }
}

//****************************************************************************/

static void TestEncoder (void) {

    UInt32 Position, oldPosition = 0;
    UInt32 newDirection, Direction = 0;
    Error_t Index, Limits;
    UInt8 inChar;

    PrintHeader ("Quadrature Encoder Test");

    Handle = halEncoderOpen (HAL_STEPPER1_ENCODER, 0, EncoderInterrupt);
    if (Handle > 0) {
        Index  = halEncoderStatus (Handle, ENC_STAT_INDEX);
        Limits = halEncoderStatus (Handle, ENC_STAT_LIMITS);
        printf ("Encode Options: Index=%s Limits=%d\n", Index ? "Yes":"No", Limits);

        // Define max. position 
        if (halEncoderWrite (Handle, ENC_REG_ENDPOSITION, 2000) != NO_ERROR) {
            printf ("halEncoderWrite(END) failed\n");
        }
        if (halEncoderWrite (Handle, ENC_REG_LIMIT1, 1500) != NO_ERROR) {
            printf ("halEncoderWrite(REF1) failed\n");
        }
        // Define reference position 2
        if (halEncoderWrite (Handle, ENC_REG_LIMIT2, 1000) != NO_ERROR) {
            printf ("halEncoderWrite(REF2) failed\n");
        }
        if (halEncoderControl (Handle, ENC_INTR_LIMIT1,   ON) != NO_ERROR ||
            halEncoderControl (Handle, ENC_INTR_LIMIT2,   ON) != NO_ERROR ||
            halEncoderControl (Handle, ENC_INTR_OVERFLOW, ON) != NO_ERROR) {
            printf ("halEncoderControl() failed\n");
        }
        do {
            if (halEncoderRead (Handle, ENC_REG_POSITION, &Position) != NO_ERROR) {
                printf ("halEncoderRead() failed\n");
            }
            if (Position != oldPosition) {
                newDirection = halEncoderStatus(Handle, ENC_STAT_DIRECTION);
                if (newDirection != Direction) {
                    //printf ("Drehrichtung geändert: %s\n", Direction ? "CW":"CCW");
                    Direction = newDirection;
                }
                printf ("Position = %d %s\n", Position, Direction ? "<-<" : ">->");
                oldPosition = Position;
            }
            inChar = halConsoleRead();
            if (inChar == 'c') {
                if (halEncoderWrite (Handle, ENC_REG_POSITION, 0) < 0)
                    printf ("Setting Encoder position failed\n");
            }
            if (inChar == 's') {
                if (halEncoderWrite (Handle, ENC_REG_POSITION, 50) < 0)
                    printf ("Setting Encoder position failed\n");
            }
             if (inChar == '?') {
                if (halEncoderRead (Handle, ENC_REG_ENDPOSITION, &Position) < 0) 
                    printf ("Reading Encoder end-position failed\n");
                else 
                    printf ("Encoder Endposition = %d\n", Position);
                
                if (halEncoderRead (Handle, ENC_REG_LIMIT1, &Position) < 0) 
                    printf ("Reading Encoder reference 1 failed\n");
                else 
                    printf ("Encoder Reference 1 = %d\n", Position);

                if (halEncoderRead (Handle, ENC_REG_LIMIT2, &Position) < 0) 
                    printf ("Reading Encoder reference 2 failed\n");
                else 
                    printf ("Encoder Reference 2 = %d\n", Position);
            }

        } while (inChar != 0x1b);

        halEncoderClose (Handle);
    }    
    else {
        printf ("halTimerOpen() failed: %d\n", Handle);
    }
}

//****************************************************************************/

static void TestSerialLoopback (void) {

    SciFormat_t Format = { 8, 1, SCI_PARITY_NONE, SCI_FLOW_NONE, 9600 };
    const char Buffer[] = "Dies ist ein Test";
    Handle_t Handle;
    UInt16 Result;
    int i;

    PrintHeader ("Testing Serial Channel (Loopback)");

    Handle = halSerialOpen (HAL_SERIAL_PORT_1, 0, NULL);
    if (Handle > 0) {
        if (halSerialSetup (Handle, &Format) < 0) {
            printf ("halSerialSetup() failed\n");
        }
        for (i=0; i < ELEMENTS(Buffer); i++) {
            if (halSerialWrite (Handle, Buffer[i]) < 0) {
                printf ("SerialWrite() failed\n");
            }
            else if (halSerialRead (Handle, &Result) < 0) {
                printf ("SerialRead() failed\n");
            }
            else if (Result != Buffer[i]) {
                printf ("Unexpected data received: %c\n", Result);
            }
            else {
                printf ("%c", Result);
            }
        }
        while (!halSerialStatus (Handle, SCI_STAT_TxEMPTY));
        halSerialClose(Handle);
    }
}

//****************************************************************************/

static void TestSerialPolled (void) {

    SciFormat_t Format = { 7, 2, SCI_PARITY_EVEN, SCI_FLOW_NONE, 9600 };
    UInt8  oBuffer[] = "Dies ist ein Test!";
    Error_t iCount = 0, oCount = 0, Status;
    Handle_t Handle;
    UInt16 Result;

    PrintHeader ("Testing Serial Channel (Polling)");

    Handle = halSerialOpen (HAL_SERIAL_PORT_1, 0, NULL);
    if (Handle > 0) {
        if (halSerialSetup (Handle, &Format) < 0) {
            printf ("halSerialSetup() failed\n");
        }
        do {
            if (oCount < ELEMENTS(oBuffer)) {
                Status = halSerialWrite (Handle, oBuffer[oCount]);
                if (Status < 0) {
                    printf ("SerialWrite() failed (0x%x)\n", Status);
                }
                else if (Status > 0) {
                    oCount++;
                }
            }
            Status = halSerialRead (Handle, &Result);
            if (Status < 0) {
                printf ("SerialRead() failed (0x%x)\n", Status);
            }
            else if (Status > 0) {
                printf ("%c", Result);
                iCount++;
            }
        } while (iCount == 0 || Result != '!');

        while (!halSerialStatus (Handle, SCI_STAT_TxEMPTY));
        halSerialClose(Handle);
    }
}

//****************************************************************************/

#define CRC32_POLYNOMIAL     0x04C11DB7u   //!< CRC32 polynomial
#define CRC32_INITIAL_VALUE  MAX_UINT32    //!< CRC32 start value

static UInt32 bmCalculateCrc32 (void *Datablock, int Length) {

    UInt8 *DataPtr = (UInt8*)Datablock;
    UInt32 Crc32 = CRC32_INITIAL_VALUE;
    UInt32 i, k;	

    // Perform modulo-2 division for each byte
    for (i=0; i < Length; i++) {

        Crc32 ^= DataPtr[i] << (32 - 8);

        // Perform modulo-2 division for each bit in byte
        for (k=0; k < 8; k++) {
            if (Crc32 & BIT(31)) {
                Crc32 = (Crc32 << 1) ^ CRC32_POLYNOMIAL;
            }
            else {
                Crc32 = (Crc32 << 1);
            }
        }
    }
    return (Crc32 ^ MAX_UINT32);
} 

//****************************************************************************/

static void TestCrcDevice (void) {

    UInt32 Datablock1[] = { 0x00000001 };
    UInt32 Datablock2[] = { 0x01000000 };
    UInt32 Sum1, Sum2;

    Sum1 = bmCalculateCrc32  (Datablock1, sizeof(Datablock1));
    Sum2 = halCalculateCrc32 (Datablock2, ELEMENTS(Datablock2));
    if (Sum1 != Sum2) {
        printf ("Checksum mismatch: 0x%08x 0x%08x\n", Sum1, Sum2);
    }
    else {
        printf ("Checksum is OK\n");
    }
}

//****************************************************************************/

static void TestCanDevice (void) {

    CanIdFilter_t Filters[] = { { 0x12345678, 0x00000000 } };
    CanMessage_t iMessage, oMessage;
    UInt32 StartTime = 0;
    Handle_t Handle;
    Error_t Status;
    
    PrintHeader ("Testing CAN Controller (Polling)");

    Handle = halCanOpen (HAL_CAN_SYSTEM, 0, NULL);
    if (Handle > 0) {
        
        Status = halCanSetup (Handle, Filters, ELEMENTS(Filters));
        if (Status < 0) {
            printf ("halCanSetup failed\n");
        }
        oMessage.CanID   = 0x12345678;
        oMessage.Length  = 1;
        oMessage.Data[0] = 0;

        do {
            if (halSysTickRead() - StartTime > 2000) {

                Status = halCanWrite (Handle, &oMessage);
                if (Status == 1) {
                    printf ("Message written: 0x%0x (%d)\n", oMessage.CanID, oMessage.Data[0]);
                    oMessage.Data[0]++;
                }
                else if (Status < 0) {
                    printf ("Send message failed\n");
                }
                StartTime = halSysTickRead();
            }
            Status = halCanRead (Handle, &iMessage);
            if (Status == 1) {
                printf ("Message read: 0x%0x (%d)\n", iMessage.CanID, iMessage.Data[0]);
            }
            else if (Status < 0) {
                printf ("Read message failed\n");
            }
            halLongDelay (10);                

        } while (halConsoleRead() != 0x1b);

        halCanClose (Handle);
    }
    else {
        printf ("Opening CAN failed\n");
    }
}

/*****************************************************************************/

void halStepperOutput (UInt16 PhaseA, UInt16 PhaseB) {

    static Handle_t Handle1, Handle2;

    if (!Handle1) {
        Handle1 = halAnalogOpen(HAL_ANALOG_OUTPUT_0, HAL_OPEN_RW);
    }
    if (Handle1) {
        if (PhaseA & 0x100) {
            PhaseA = 256 + (PhaseA & 0xFF);
        }
        else {
            PhaseA = 256 - (PhaseA & 0xFF);
        }
        halAnalogWrite (Handle1, PhaseA & 0x1FF);
    }

    if (!Handle2) {
        Handle2 = halAnalogOpen(HAL_ANALOG_OUTPUT_1, HAL_OPEN_RW);
    }
    if (Handle2) {
        if (PhaseB & 0x100) {
            PhaseB = 256 + (PhaseB & 0xFF);
        }
        else {
            PhaseB = 256 - (PhaseB & 0xFF);
        }
        halAnalogWrite (Handle2, PhaseB & 0x1FF);
    }
}

//****************************************************************************/

static void ExternalInterrupt (UInt32 UserTag, UInt32 LineNo) {

    printf ("External Interrupt %d\n", LineNo);
}

//****************************************************************************/

static void TestExternalInterrupt (void) {

    Handle_t Handle1 = halExtIntOpen (HAL_INTERRUPT_1, 0, ExternalInterrupt);
    Handle_t Handle2 = halExtIntOpen (HAL_INTERRUPT_2, 1, ExternalInterrupt);
    Handle_t Handle3 = halExtIntOpen (HAL_INTERRUPT_3, 2, ExternalInterrupt);
    UInt32 inKey;

    printf ("Testing External Interrupts\n");

    if (Handle1 < 0 || Handle2 < 0 || Handle3 < 0) {
        printf ("Opening ExtInt failed\n");
        return;
    }
    if (halExtIntControl (Handle1, EXT_CTRL_ENABLE) < 0) {
        printf ("halExtIntControl() failed\n");
    }
    if (halExtIntControl (Handle2, EXT_CTRL_ENABLE) < 0) {
        printf ("halExtIntControl() failed\n");
    }
    if (halExtIntControl (Handle3, EXT_CTRL_ENABLE) < 0) {
        printf ("halExtIntControl() failed\n");
    }
    do {
        inKey = halConsoleRead();
        if (inKey == '1') {
           halExtIntControl (Handle1, EXT_CTRL_TRIGGER);
        }
        else if (inKey == '2') {
           halExtIntControl (Handle2, EXT_CTRL_TRIGGER);
        }
        else if (inKey == '3') {
           halExtIntControl (Handle3, EXT_CTRL_TRIGGER);
        }    
    } while (inKey != 0x1b);

    halExtIntClose (Handle1);
    halExtIntClose (Handle2);
    halExtIntClose (Handle3);
}


//****************************************************************************/

static void TestRealTimeClock (void) {

    const char *wd[] = { "MO", "DI", "MI", "DO", "FR", "SA", "SO" };
    DateTime_t Date;
    UInt32 inKey;

    printf ("Testing Real Time Clock\n");

    do {
        inKey = halConsoleRead();

        if (inKey == 's') {
            DateTime_t newDate = { 2011, 3, 31, 0, 18, 18, 0, 999 };
            
            if (halRtcWriteDate (&newDate) < 0) {
                printf ("halRtcWriteDate() failed\n");
            }
            halRtcReadDate(&Date);
            printf ("New Date Set\n");
        }
        if (halRtcReadDate(&Date) < 0) {
            printf ("halRtcReadDate() failed\n");
        }
        printf ("%s %02d.%02d.%04d - %02d:%02d:%02d (%d)\n", wd[Date.WeekDay], 
            Date.Day, Date.Month, Date.Year, 
            Date.Hours, Date.Minutes, Date.Seconds, Date.Millis);

        halLongDelay (1010);

    } while (inKey != 0x1b);
}

//****************************************************************************/

static void PwmInterrupt (UInt32 Handle, UInt32 LineNo) {

    printf ("PWM Interrupt %d\n", LineNo);
}

//****************************************************************************/

static void TestPwmOutputs (void) {

    const PwmMode_t PwmMode = { 
        TIM_MODE_COUNT_UP, PWM_MODE_INTERVAL, PWM_MODE_INTERNAL, 0};
    const UInt16 Unit = 0;

    UInt32 Period = 100000000;
    UInt32 DutyCycle = 0; 
    UInt32 newWidth = 0;
    Handle_t Handle;
    UInt16 Result;
    UInt32 inKey;

    printf ("Testing PWM Outputs\n");

    Handle = halPwmOpen (HAL_TIMER_2, 0, PwmInterrupt);
    if (Handle < 0) {
        printf ("Opening PWM channel failed\n");
        return;
    }
    if (halPwmSetup (Handle, PwmMode, Period) < 0) {
        printf ("halPwmSetup failed\n");
    }
    do {
        switch (inKey = halConsoleRead()) {
            case '+': newWidth = DutyCycle + 1; break;
            case '-': newWidth = DutyCycle - 1; break;
            case '>': newWidth = DutyCycle + 10; break;
            case '<': newWidth = DutyCycle - 10; break;
        }
        if (inKey == 'u') {
            Period /= 5;
            if (halPwmSetup (Handle, PwmMode, Period) < 0) {
                printf ("halPwmSetup failed\n");
            }
        }
        if (inKey == 'd') {
            Period *= 5;
            if (halPwmSetup (Handle, PwmMode, Period) < 0) {
                printf ("halPwmSetup failed\n");
            }
        }
        if (inKey == 'i') {
            halPwmControl (Handle, Unit, PWM_INTR_ENABLE);
        }
        if (inKey == 'n') {
            halPwmControl (Handle, Unit, PWM_INTR_DISABLE);
        }
        if (newWidth <= 100 && newWidth != DutyCycle) {
            if (halPwmWrite (Handle, Unit, newWidth * 0xFFFF / 100) < 0) {
                printf ("halPwmWrite failed\n");
            }
            if (halPwmRead (Handle, Unit, &Result) < 0) {
                printf ("halPwmRead failed\n");
            }
            printf ("- Pulse Width = %d%%\n", Result * 100 / 0xFFFF);
            DutyCycle = newWidth;
        }        
    } while (inKey != 0x1b);

    halPwmClose (Handle);
}


//****************************************************************************/

static void TestFlashMemory (void) {

    UInt32 Address = 0x10000;                       //MAX: 0x80000
    UInt8 Buffer[2048];
    Error_t Status;
    UInt32 inKey;
    UInt32 i;

    PrintHeader ("Flash memory programming");

    do {
        inKey = halConsoleRead();

        if (inKey == 'r') {
            Status = halFlashRead (Address, &Buffer, 1024);
            if (Status < 0) {
                printf ("Flash Read failed\n");
            }
        }
        if (inKey == 'w') {
            for (i=0; i < 512; i++) {
                Buffer[i] = i;
            }        
            Status = halFlashWrite (Address, Buffer, 512);
            if (Status < 0) {
                printf ("Flash Write failed\n");
            }
        }
        if (inKey == 'u') {
            Status = halFlashProtect(OFF);
            if (Status < 0) {
                printf ("halFlashProtect failed\n");
            }
        }
        if (inKey == 'p') {
            Status = halFlashProtect(ON);
            if (Status < 0) {
                printf ("halFlashWriteProtect failed\n");
            }
        }
        if (inKey == 'e') {
            Status = halFlashErase (Address, 2048);
            if (Status < 0) {
                printf ("Flash Erase failed\n");
            }
        }
    } while (inKey != 0x1b);
}

//****************************************************************************/

static void TestLogicalMemory (void) {

    UInt32 Address = 0;
    UInt8 Buffer[2048];
    Handle_t Handle;
    Error_t Status;
    UInt32 inKey;
    UInt32 i;

    PrintHeader ("Logical memory test");

    Handle = halStorageOpen (HAL_STORAGE_FRAM, HAL_OPEN_RWE);
    if (Handle < 0) {
        printf ("halStorageOpen failed\n");
        return;
    }

    do {
        inKey = halConsoleRead();

        if (inKey == 'r') {
            Status = halStorageRead (Handle, Address, &Buffer, 0x100);
            if (Status < 0) {
                printf ("halStorageRead failed\n");
            }
        }
        if (inKey == 'w') {
            for (i=0; i < 512; i++) {
                Buffer[i] = i;
            }        
            Status = halStorageWrite (Handle, Address, Buffer, 0x200);
            if (Status < 0) {
                printf ("halStorageWrite failed\n");
            }
        }
        if (inKey == 'u') {
            Status = halStorageProtect(Handle, OFF);
            if (Status < 0) {
                printf ("halStorageProtect failed\n");
            }
        }
        if (inKey == 'p') {
            Status = halStorageProtect(Handle, ON);
            if (Status < 0) {
                printf ("halStorageProtect failed\n");
            }
        }
        if (inKey == 'e') {
            Status = halStorageErase (Handle, Address, 0x200);
            if (Status < 0) {
                printf ("halStorageErase failed\n");
            }
        }
    } while (inKey != 0x1b);

    halStorageClose(Handle);
}

//****************************************************************************/

static void TestExceptions (void) {

    typedef void (*I2cCallback_t) (int);
    UInt32 Zero = 0;
    UInt32 Result;
    UInt32 inKey;

    PrintHeader ("Exception handling test");

    do {
        inKey = halConsoleRead();

        if (inKey == '0') {
            Result = MAX_UINT32;
            Result = 300 / Zero;
            printf ("Divide by zero not detected (%d)\n", Result);
        }
        if (inKey == 'p') {
            volatile UInt32 *Pointer = NULL;
            *Pointer = 0x55;
            printf ("NULL pointer access not detected (%d)\n", *Pointer);
        }
        if (inKey == 'u') {
            volatile UInt32 *Pointer = (volatile UInt32*) 1;
            Result = *Pointer;
            printf ("Unaligned access not detected (%d)\n", Result);
        }
        if (inKey == 'c') {
            I2cCallback_t Pointer = (I2cCallback_t) 0xE000ED00;
            Pointer(3);
            printf ("Code exception not detected (%d)\n", Result);
        }
    } while (inKey != 0x1b);
}

//****************************************************************************/

typedef unsigned long long UInt64;

typedef struct {
    UInt32 minTime;
    UInt32 maxTime;
    UInt32 avgTime;
    UInt32 Count;
} Statistics_t;

//****************************************************************************/

static void PrintStatistic (Statistics_t *Statistics) {

    printf ("Statistik: min=%uns max=%uns avg=%uns count=%u\n",
        Statistics->minTime * 14, 
        Statistics->maxTime * 14, 
        Statistics->avgTime * 14, 
        Statistics->Count);
}

//****************************************************************************/

static void UpdateStatistic (Statistics_t *Statistics, UInt32 StartTime, UInt32 EndTime) {

    const UInt32 Duration = EndTime - StartTime;

    if ((Duration & 0x80000000) == 0) {
        if (Duration < Statistics->minTime) {
            Statistics->minTime = Duration;
        }
        if (Duration > Statistics->maxTime) {
            Statistics->maxTime = Duration;
        }
        if (Duration != Statistics->avgTime) {
            Statistics->avgTime = 
                ((Statistics->avgTime * Statistics->Count) + Duration) / (Statistics->Count + 1);
        }
        Statistics->Count++;

        if ((Statistics->Count % 100000) == 0) {
            PrintStatistic (Statistics);
        }
    }
}

//****************************************************************************/

static Error_t PrintStepperError (Error_t ErrCode) {

    switch (ErrCode) {
        case E_STEPPER_BUSY:
            printf ("halStepperWrite: Driver busy\n");
            break;
        case E_STEPPER_SHORT_CIRCUIT:
            printf ("halStepperWrite: Short circuit\n");
            break;
        case E_STEPPER_TEMPERATURE:
            printf ("halStepperWrite: Overtemperature\n");
            break;
        case E_STEPPER_OPEN_LOAD:
            printf ("halStepperWrite: Open Load\n");
            break;
        case E_STEPPER_MOTOR_STALLED:
            printf ("halStepperWrite: Motor stalled\n");
            break;
        case NO_ERROR:
            printf ("No (more) errors\n");
            break;
        default:
            printf ("Unknown stepper error (0x%x)\n", ErrCode);
            break;
    }
    return (ErrCode);
}

//****************************************************************************/

static void TestStepperDevice (void) {

    Statistics_t Statistics = {0xFFFFFFFF, 0, 0, 0 };
    UInt32 StartTime, EndTime;
    Error_t Status, LastError;
    Bool Direction = TRUE;
    UInt32 MicroSteps = 1;
    UInt32 StepDuration = 500;
    UInt32 StepIndex = 32;
    UInt32 inKey;
    Handle_t Handle;

    PrintHeader ("Stepper motor test");

    Handle = halStepperOpen (HAL_STEPPER_MOTOR_0);
    if (Handle < 0) {
        printf ("Opening Stepper failed\n");
        return;
    }
    halShortDelay (100);

    do {    
//        StartTime = halGetSystemTimer();
        Status = halStepperWrite(Handle, StepIndex);
//        EndTime = halGetSystemTimer();

        if (Status != LastError) {
            LastError = PrintStepperError(Status);
        }
        if (Status == NO_ERROR) {
            UpdateStatistic (&Statistics, StartTime, EndTime);
        }
        if (Direction) {
            StepIndex += MicroSteps;
        }
        else {
            StepIndex -= MicroSteps;
        }
        inKey = halConsoleRead();
        if (inKey == '-') {
            if (MicroSteps < 64) {
                MicroSteps <<= 1;
            }
            printf ("%d MicroSteps\n", 64/MicroSteps);
        }
        else if (inKey == '+') {
            if (MicroSteps > 1) {
                MicroSteps >>= 1;
            }
            printf ("%d MicroSteps\n", 64/MicroSteps);
        }
        else if (inKey == 'd') {
            Direction = !Direction;
        }
        else if (inKey >= '0' && inKey <= '9') {
            StepDuration = 5 << (inKey - '0');
        }
        else if (inKey == '?') {
            PrintStatistic (&Statistics);
        }
        DelayInMicroSeconds (StepDuration*MicroSteps);

    } while (inKey != 0x1b);

    halStepperClose (Handle);
}


//****************************************************************************/

static UInt32 CallbackCounter = 0;

static void halI2CCallback (UInt16 JobID, Error_t Status) {

    char *Message;

    if (Status != NO_ERROR) {
        switch (Status) {
            case E_BUS_ERROR:
                Message = "Bus error"; 
                break;
            case E_BUS_ACKNOWLEDGE:
                Message = "No Acknowledge"; 
                break;
            case E_BUS_ARBITRATION:
                Message = "Arbitration lost"; 
                break;
            default:
                Message = "Unspecifiy error"; 
        }
        printf ("Transaction %d failed: %s\n", JobID, Message);
    }
    CallbackCounter++;
}

//****************************************************************************/

static void TestI2cInterface (void) {

    UInt8 *TxBuffer = "\x55\xCC\x55\xCC\x55\xCC\x55\xCC\x55\xCC";
    const UInt32 SlaveID = 0x0A;
    Bool EnableWrite = TRUE;
    Bool EnableRead = TRUE;
    UInt8 RxBuffer[10];
    Handle_t Handle;
    Error_t Status;
    UInt32 inKey;
    UInt32 Count = 4;

    PrintHeader ("I2C interface test");

    Handle = halI2cOpen (HAL_BUS_I2C_A, 0, halI2CCallback);
    if (Handle < 0) {
        printf ("halI2cOpen failed\n");
        return;
    }
    do {
        UInt32 Address = 0x70 + (SlaveID << 11);

        inKey = halConsoleRead();
        if (inKey >= '0' && inKey <= '9') {
            Count = inKey - '0';
        }
        else if (inKey == 'r') {
            EnableRead = !EnableRead;
        }
        else if (inKey == 'w') {
            EnableWrite = !EnableWrite;
        }

        if (EnableWrite) {

            Status = halI2cExchange(Handle, Address, TxBuffer, Count, FALSE);
            if (Status < 0) {
                printf ("halI2cWrite failed\n");        
            }
            else {
                halI2cWait (Handle, 0);
            }
        }
        if (EnableRead) {
            Status = halI2cExchange (Handle, Address, RxBuffer, Count, TRUE);
            if (Status < 0) {
                printf ("halI2cRead failed (%d)\n", CallbackCounter);        
            }
            else {
                halI2cWait (Handle, 0);
            }
        }
        halLongDelay (10);        

    } while (inKey != 0x1b);

    halI2cClose (Handle);
}

//****************************************************************************/

static void TestI2cInterface2 (void) {

    UInt8 TxBuffer[10] = "\x55\xCC\x55\xCC\x55\xCC\x55\xCC\x55\xCC";
    UInt8 RxBuffer[10];

    const UInt32 SlaveID = 0x0A;
    Bool EnableWrite = TRUE;
    Bool EnableRead = TRUE;
    Handle_t Handle;
    Error_t Status;
    UInt32 inKey;
    UInt32 Count = 2;
    int i;

    PrintHeader ("I2C interface test");

    Handle = halI2cOpen (HAL_BUS_I2C_A, 0, halI2CCallback);
    if (Handle < 0) {
        printf ("halI2cOpen failed\n");
        return;
    }
    do {
        UInt32 Address = 0x70 + (SlaveID << 11);

        inKey = halConsoleRead();
        if (inKey >= '0' && inKey <= '9') {
            Count = inKey - '0';
        }
        else if (inKey == 'r') {
            EnableRead = !EnableRead;
        }
        else if (inKey == 'w') {
            EnableWrite = !EnableWrite;
        }
        else if (inKey == 'c') {
            for (i=0; i < sizeof(RxBuffer); i++) {
                RxBuffer[i] = 0;
            }
        }

        if (EnableWrite) {
            for (i=0; i < 11; i++) {
                Status = halI2cExchange(Handle, Address, TxBuffer, Count, FALSE);
                if (Status < 0) {
                    printf ("halI2cWrite failed\n");        
                }
            }
            EnableWrite = FALSE;
        }
        if (EnableRead) {
            for (i=0; i < 5; i++) {
                Status = halI2cExchange(Handle, Address, RxBuffer, Count, TRUE);
                if (Status < 0) {
                    printf ("halI2cRead failed\n");        
                }
                halI2cWait (Handle, 0);
            }

            for (i=0; i < Count; i++) {
                if (RxBuffer[i] != TxBuffer[i]) {
                    printf ("Mismatch\n");
                }
            }
            EnableRead = FALSE;
        }
        halLongDelay (10);        

    } while (inKey != 0x1b);

    halI2cClose (Handle);
}

//****************************************************************************/

static void TestSpiDevice (void) {

    UInt16 Format = SPI_FORMAT_MSB1ST | SPI_FORMAT_CLK_PHASE1 | SPI_FORMAT_CLK_IDLE_1;
    UInt8 inChar = 0;
    UInt8 Buffer[10];
    Error_t Status;
    UInt32 Count;

//    { HAL_STEPPER_MOTOR_0, 0, { 0x901B4, 0xD4006, 0xE02D0, 0xA8202 }},

    PrintHeader ("SPI interface test");

    Handle = halSpiOpen (HAL_BUS_SPI_A);
    if (Handle < 0) {
        printf ("halSpiOpen failed\n");
        return;
    }
    Status = halSpiSetup (Handle, 1000000, Format);
    if (Status < 0) {
        printf ("halSpiSetup failed\n");
        return;
    }
    do {
        Buffer[0] = 0x00;
        Buffer[1] = 0x09;
        Buffer[2] = 0x01;
        Buffer[3] = 0xB4;
        Count = 4;

        Status = halSpiTransfer (Handle, Buffer, Count);
        if (Status != NO_ERROR) {
            printf ("halSpiTransfer failed\n");
        }
        inChar = halConsoleRead();

    } while (inChar != 0x1b);

    halSpiClose (Handle);
}


//****************************************************************************/

int main (void) {

    if (halInitializeLayer() == NO_ERROR) {
        
        for (;;) {
            printf ("\n\nPlease choose test case [a..l]: ");
            switch (getchar()) {
                case 'a':
                    TestHardwareInfos();
                    break;
                case 'b':
                    TestAnalogOutput (HAL_ANALOG_OUTPUT_0, 1);
                    break;          
                case 'c':
                    TestAnalogInput();                     
                    break;
                case 'd':
                    TestDigitalOutput();
                    break;
                case 'e':
                    TestDigitalInput();
                    break;
                case 'f':
                    TestSerialPolled();
                    break;
                case 'g':
                    TestSerialLoopback();
                    break;
                case 'h':
                    TestTimer();
                    break;
                case 'i':
                    TestI2cInterface2();
                    break;
                case 'j':
                    TestTimerMeasureFrequency();
                    break;
                case 'k':
                    TestTimerPwmOutput();
                    break;
                case 'l':
                    TestSpiDevice();
                    break;
                case 'm':
                    TestShortDelay();
                    break;    
                case 'n':
                    TestCoreVoltage();
                    break;    
                case 'o':
                    ShowResetReason();
                    break;
                case 'p':
                    TestWatchdog();
                    break;
                case 'q':
                    puts("Reseting device");
                    halHardwareReset();
                    break;
                case 'r':
                    TestCanDevice();
                    break;
                case 's':
                    TestStepperDevice();
                    break;
                case 't':
                    TestRealTimeClock();
                    break;
                case 'u':
                    TestEncoder();
                    break;
                case 'v':
                    TestExternalInterrupt();
                    break;
                case 'w':
                    TestPwmOutputs();
                    break;
                case 'x':
                    TestLogicalMemory();
                    break;
                case 'E':
                    TestTimerExternalClock();
                    break;
                case 'X':
                    TestFlashMemory();
                    break;
                case 'Y':
                    TestCrcDevice();
                    break;
                case '?':
                    TestProcessorInfo();
                    break;
                case '!':
                    TestExceptions();
                    break;
            }
        }
    }
    puts("HAL initialization failed\n");
    for(;;);
}

//****************************************************************************/
