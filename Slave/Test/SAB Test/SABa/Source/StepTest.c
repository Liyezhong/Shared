/****************************************************************************/
/*! \file halStepTest.c
 * 
 *  \brief  Stepper motor test module
 *
 *  $Version: $ 0.2
 *  $Date:    $ 30.06.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to test the stepper motor using the
 *      hardware abstraction layer. It can be controlled interactively
 *      using the serial debug console.
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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Global.h"
#include "halLayer.h"
#include "halConsole.h"
#include "halConfigure.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define FSTEPS_PER_ROTATION 400        //!< Full steps per rotation
#define HSTEPS_PER_ROTATION 800        //!< Half steps per rotation

#define MIN_STEP_RATE       1          //!< Min. full step rate: 1Hz
#define MAX_STEP_RATE       10000      //!< Max. full step rate: 10kHz
#define MIN_RAMP_LENGTH     1          //!< Min. ramp time = 0
#define MAX_RAMP_LENGTH     10000      //!< Max. ramp time = 10s
#define MIN_MICROSTEP       1          //!< Min. micro steps per step
#define MAX_MICROSTEP       64         //!< Max. micro steps per step
#define MIN_SPEED           25         //!< Minimal speed

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static Int32 MaxSpeed[8] = {0};     //!< Speed at end of each phase
static Int32 MaxAccel[8] = {0};     //!< Acceleration at end of each phase
static Int32 MaxSteps[8] = {0};     //!< Step number at end of each phase
static Int32 Duration[8] = {0};     //!< Duration of each phase [ms]
static Int32 StepIndex = 0;         //!< Step index passed to HAL

static Bool VerboseFlag = FALSE;    //!< Verbose flag
static Handle_t Handle = 0;         //!< Handle of stepper motor controller

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Int32 StepperMove (Int32 Position, 
     UInt32 TargetSpeed, UInt32 TargetAccel, UInt32 Jerk, Int32 Resolution);

static void DelayInMicroSeconds  (UInt32 MicroSeconds);
static void PrintPhaseInfos (void);
static Int32 SingleStep (Int32 Microsteps);


static void LoopTest (void)
{

    UInt32 MicroSteps = 32;
    UInt32 Position   = 1000;
    UInt32 Speed      = 2  * HSTEPS_PER_ROTATION;
    UInt32 Accel      = 40 * HSTEPS_PER_ROTATION;
    UInt32 Jerk       = 40 * HSTEPS_PER_ROTATION;
    UInt32 Home       = 0;
	Int32  Status     = NO_ERROR;

    for (;;)
	{
        Status = StepperMove (
            Position+Home, Speed, Accel, Jerk, MicroSteps);
        DelayInMicroSeconds (100000);

        Status = StepperMove (
            Home, Speed, Accel, Jerk, MicroSteps);
        DelayInMicroSeconds (100000);

    }
}


/*****************************************************************************/
/*!
 *  \brief   Stepper motor positioning test
 *
 *      This function allows the stepper motor to be moved to certain
 *      positions interactively. The user can control the motor using
 *      single key commands, with additional numeric parameters. Each
 *      command must be terminated with a newline character. 
 *      The following command are available:
 *
 *      - Goto position:     g <step position>
 *      - Move relative:     m <steps>
 *      - Loop position:     l <end position>
 *      - Set move profile:  p <speed> <acceleration> <jerk>
 *      - Set microsteps:    u <microsteps>
 *      - Set register:      r <value>
 *      - Show move profile: ?
 *      - Set home position: z 
 *
 *      The positioning is based on half steps.
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static void StepperPositionTest (void) {

    UInt32 MicroSteps = 32;
    UInt32 Position   = 0;
    UInt32 Speed      = 2  * HSTEPS_PER_ROTATION;
    UInt32 Accel      = 40 * HSTEPS_PER_ROTATION;
    UInt32 Jerk       = 40 * HSTEPS_PER_ROTATION;
    UInt32 Home       = 0;
    UInt32 ParaCount  = 0;
    Int32  Status     = NO_ERROR;
    UInt32 Value;
    char Buffer[80];

    printf (">");

    for (;;) {
        if (halConsoleGets (Buffer, sizeof(Buffer))) {
            
            char *Command = strtok (Buffer, " "); 
            char *Parameters[10];
            char *Token;

            ParaCount = 0;
            do {
                Token = strtok (NULL, " "); 
                if (Token != NULL) {
                    Parameters[ParaCount++] = Token;
                }
            } while (Token != NULL && ParaCount < ELEMENTS(Parameters));

            switch (Command[0]) {
                //********************************************************
                // Move stepper relative to actual position (MOVE)
                //********************************************************
                case 'm':  {
                    if (ParaCount >= 1) {
                        Position += atol(Parameters[0]);
                        printf ("Move to position %d\n", Position);
                        Status = StepperMove (
                            Position+Home, Speed, Accel, Jerk, MicroSteps);
                    }
                    break;
                }
                //********************************************************
                // Move stepper to absolute position (GOTO)
                //********************************************************
                case 'g':  {
                    if (ParaCount >= 1) {
                        Position = atol(Parameters[0]);
                        printf ("Move to position %d\n", Position);
                        Status = StepperMove (
                            Position+Home, Speed, Accel, Jerk, MicroSteps);
                    }
                    break;
                }
                //********************************************************
                // Loop stepper between two positions (LOOP)
                //********************************************************
                case 'l':  {
                    if (ParaCount >= 1) {
                        Position = atol(Parameters[0]);
                        printf ("Loop between position 0 and %d\n", Position);
                        printf ("(Press <ESC> to exit)\n");
                        do {
                            Status = StepperMove (
                                Position+Home, Speed, Accel, Jerk, MicroSteps);
                            DelayInMicroSeconds (100000);

                            Status = StepperMove (
                                Home, Speed, Accel, Jerk, MicroSteps);
                            DelayInMicroSeconds (100000);

                        } while (halConsoleRead() != 0x1b && Status >= 0);
                        Position = 0;
                    }
                    break;
                }
                //********************************************************
                // Set micro-step resolution (USTEP)
                //********************************************************
                case 'u':  {
                    if (ParaCount >= 1) {
                        MicroSteps = atol(Parameters[0]);
                        if (MicroSteps > MAX_MICROSTEP) {
                            MicroSteps = MAX_MICROSTEP;
                        }
                        if (MicroSteps < MIN_MICROSTEP) {
                            MicroSteps = MIN_MICROSTEP;
                        }
                    }
                    printf ("- Resolution = %d uSteps\n", MicroSteps);
                    break;
                }
                //********************************************************
                // Set profile parameters
                //********************************************************
                case 'p': {
                    if (ParaCount >= 1) {
                        Int32 Value = atol(Parameters[0]);
                        if (Value > 0) {
                            Speed = Value;
                        }
                    }
                    if (ParaCount >= 2) {
                        Value = atol(Parameters[1]);
                        if (Value > 0) {
                            Accel = Value;
                        }
                    }
                    if (ParaCount >= 3) {
                        Value = atol(Parameters[2]);
                        if (Value > 0) {
                            Jerk = Value;
                        }
                    }
                }
                // fall thru to print parameters

                //********************************************************
                // Print profile parameters
                //********************************************************
                case '?':  {
                    printf ("Actual profile:\n");
                    printf ("- Step speed = %d Halfsteps/s\n", Speed);
                    printf ("- Accelerate = %d Halfsteps/s²\n", Accel);
                    printf ("- Jerk       = %d Halfsteps/s³\n", Jerk);
                    printf ("- Resolution = %d uSteps\n", MicroSteps);
                    printf ("- Position   = %d\n", Position);
                    break;
                }
                //********************************************************
                // Set TMC internal register
                //********************************************************
                case 'r':  {
                    if (ParaCount >= 1) {
                        UInt32 Mode = strtol(Parameters[0], NULL, 0);                    

                        Status = halStepperSetup(Handle, &Mode, 1);
                        if (Status == NO_ERROR) {
                            printf ("TMC26x register set to 0x%x\n", Mode);
                        }
                    }
                    break;
                }
                //********************************************************
                // Set home position (zero)
                //********************************************************
                case 'z':  {
                    printf ("Home position set\n");
                    Home += Position;
                    break;
                }
                //********************************************************
                // Print profile parameters
                //********************************************************
                case '#': 
                    PrintPhaseInfos();
                    break;
            }
            if (Status < 0) {
                printf ("*** ERROR 0x%x\n", Status);
                Status = NO_ERROR;
            }
            printf (">");
        }
    }
}


/*****************************************************************************/
/*!
 *  \brief   Move stepper to absolute position
 *
 *      Moves the stepper motor to the absolute 'Position', using the micro
 *      step resolution defined by Resolution and the given rotation speed.
 *      Resolution can be in the range 1-64. 
 *
 *      An S-curve ramp is used, which can be configured using the maximal
 *      acceleration and the jerk. Alle parameters are scaled on a per
 *      second base (rotation/s, rotations/s², rotations/s³).
 *
 *  \iparam  Position    = Absolute step number 
 *  \iparam  TargetSpeed = Max. rotation speed [r/s]
 *  \iparam  TargetAccel = Max. acceleration [r/s²]
 *  \iparam  Jerk        = Acceleration jerk [r/s³]
 *  \iparam  Resolution  = Micro steps per step (1...64)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Int32 StepperMove (Int32 Position, 
        UInt32 TargetSpeed, UInt32 TargetAccel, UInt32 Jerk, Int32 Resolution) {

    UInt32 StartTime = 0;
    UInt32 OldPhase  = 0;
    UInt32 ActPhase  = 1;
    Int32  ActSpeed  = 0; //MIN_SPEED;
    Int32  ActAccel  = 0;
    UInt32 ActTime   = 0; 
    UInt32 OldTime   = 0;
    UInt32 StepStart = 0;
    UInt32 StepTime  = 0;
    UInt32 StepNo    = 0;
    Int32  Increment;
    Int32  Distance;

    if (2000000L / (TargetSpeed * Resolution) < 10) {
        printf (" - WARNING: Speed out of range\n");
        return (E_INVALID_PARAMETER);
    }     
    Position = Position * MAX_MICROSTEP / 2;

    if (Position > StepIndex) {
        Increment = MAX_MICROSTEP / Resolution;
    }
    else {
        Increment = -MAX_MICROSTEP / Resolution;
    }
    Distance  = abs(Position - StepIndex);
    StepStart = halGetFastTick();

    while (StepNo < Distance) {

        if (ActPhase != OldPhase) {
            StartTime = halSysTickRead();   
            MaxSpeed[OldPhase] = ActSpeed;
            MaxAccel[OldPhase] = ActAccel;
            MaxSteps[OldPhase] = StepNo;    
            Duration[OldPhase] = ActTime;
            OldPhase = ActPhase;
        }
        ActTime = halTimeExpired(StartTime);
        StepNo += abs(Increment);

        switch (ActPhase) {    
           //------------------------------------------------------------
            case 1: // Phase 1: Increasing acceleration
           //------------------------------------------------------------
                if (ActTime != OldTime) {
                    ActAccel = MaxAccel[0] + (Jerk * ActTime) / 1000;
                    ActSpeed = MaxSpeed[0] + (ActTime * (MaxAccel[0] + (Jerk * ActTime) / 2000)) / 1000;
                }        
                if (ActAccel >= TargetAccel || ActSpeed >= TargetSpeed/2 || StepNo >= Distance/4) {
                    if (ActAccel > TargetAccel) {
                        ActAccel = TargetAccel;
                    }
                    if (ActSpeed > TargetSpeed/2) {
                        ActSpeed = TargetSpeed/2;
                    }
                    ActPhase = 2;
                } 
                break;
           //------------------------------------------------------------
            case 2: // Phase 2: Constant acceleration
           //------------------------------------------------------------
                if (ActTime != OldTime) {
                    ActAccel = MaxAccel[1];
                    ActSpeed = MaxSpeed[1] + (ActAccel * ActTime) / 1000;
                }        
                if (ActSpeed >= TargetSpeed - MaxSpeed[1] || StepNo >= Distance/4) {
                    if (ActSpeed > TargetSpeed - MaxSpeed[1]) {
                        ActSpeed = TargetSpeed - MaxSpeed[1];
                    }
                    ActPhase = 3;
                } 
                break;
           //------------------------------------------------------------
            case 3: // Phase 3: Decreasing acceleration
           //------------------------------------------------------------
                if (ActTime != OldTime) {
                    ActAccel = MaxAccel[2] - (Jerk * ActTime) / 1000;
                    ActSpeed = MaxSpeed[2] + (ActTime * (MaxAccel[2] - (Jerk * ActTime) / 2000)) / 1000;
                }        
                if (ActAccel <= 0 || ActSpeed >= TargetSpeed || StepNo >= Distance/2) {
                    if (ActSpeed > TargetSpeed) {
                        ActSpeed = TargetSpeed;
                    }
                    ActPhase = 4;
                } 
                break;
           //------------------------------------------------------------
            case 4: // Phase 4: Constant speed, no acceleration
           //------------------------------------------------------------
                ActAccel = 0;
                ActSpeed = MaxSpeed[3];
        
                if (StepNo >= Distance - MaxSteps[3]) {
                    ActPhase = 5;
                } 
                break;
           //------------------------------------------------------------
            case 5: // Phase 5: Increasing deceleration
           //------------------------------------------------------------
                if (ActTime != OldTime) {
                    ActAccel = MaxAccel[4] - (Jerk * ActTime) / 1000;
                    ActSpeed = MaxSpeed[4] - (ActTime * (MaxAccel[3] + (Jerk * ActTime) / 2000)) / 1000;
                }        
                if (StepNo >= Distance - MaxSteps[2]) {
                //if (ActTime >= Duration[3]) {
                    ActPhase = 6;
                }
                break;
           //------------------------------------------------------------
            case 6: // Phase 6: Constant deceleration    
           //------------------------------------------------------------
                if (ActTime != OldTime && ActSpeed > MIN_SPEED) {  
                    ActAccel = MaxAccel[2];
                    ActAccel = MaxAccel[5];
                    ActSpeed = MaxSpeed[2] - (ActAccel * ActTime) / 1000;
                }        
                if (StepNo >= Distance - MaxSteps[1]) {
                    ActPhase = 7;
                } 
                break;
           //------------------------------------------------------------
            case 7:  // Phase 7: Decreasing deceleration
           //------------------------------------------------------------
                if (ActTime != OldTime) {
                    ActAccel = MaxAccel[1] - (Jerk * ActTime) / 1000;
                    ActSpeed = MaxSpeed[1] - (ActTime * (MaxAccel[2] - (Jerk * ActTime) / 2000)) / 1000;
                }        
                if (StepNo >= Distance) {
                    ActSpeed = ActAccel = 0;
                    ActPhase = 0;
                } 
                break;
        }
        // Wait for the previous step to finish
        while (halGetFastTick() - StepStart < StepTime) {}

        StepStart += StepTime;
        if (ActSpeed < MIN_SPEED) {              
            StepTime = (1000000L * abs(Increment)) / (MIN_SPEED * MAX_MICROSTEP/2);
        }
        else {
            StepTime = (1000000L * abs(Increment)) / (ActSpeed * MAX_MICROSTEP/2);
        }
        StepIndex += Increment;

        SingleStep (StepIndex); 
        OldTime = ActTime;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Perform microsteps
 *
 *      Performs the number of Microsteps in the direction given by the
 *      sign of Microsteps. Microsteps can be 1,2,4,8,16,32, or 64.
 *
 *  \iparam  Microsteps = Number of microsteps
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Int32 SingleStep (Int32 SinIndex) {

    static Error_t LastError = NO_ERROR;
    Error_t Status;

   // Do next microstep, handle stepper errors
    Status = halStepperWrite (Handle, SinIndex);
    if (Status != NO_ERROR) {
        if (Status != LastError) {
            if (VerboseFlag) {
                Int32 ErrFlags = halStepperStatus(Handle, STP_STAT_ERRORFLAGS); 
                printf ("- ErrFlags=0x%x\n", ErrFlags);
            }
            LastError = Status;
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Print moivement phase informations
 *
 *      Prints the speed, acceleration, step counts and duration of the
 *      various movement phases.
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static void PrintPhaseInfos (void) {

    UInt32 Speed = 0;
    UInt32 i;

    for (i=0; i < 8; i++) {
        printf ("P=%d V=%-5d A=%-5d N=%-5d t=%d\n", i, 
            MaxSpeed[i], MaxAccel[i], MaxSteps[i], Duration[i]);
    }
    if (Duration[4]) {
        Speed = (MaxSteps[4] - MaxSteps[3]) * 1000 / Duration[4] / MAX_MICROSTEP;
    }
    printf ("Speed is %d HS/s\n", Speed);
}


/*****************************************************************************/
/*!
 *  \brief   Delays for a certain time
 *
 *      Delays for a certain number of micro seconds.
 *
 *  \iparam  MicroSeconds = Delay time [micro seconds]
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static void DelayInMicroSeconds (UInt32 MicroSeconds) {

    volatile UInt32 Loops = MicroSeconds / 500;

    while (Loops--) {
        halShortDelay (500);
    }
    halShortDelay (MicroSeconds % 500);
}


/*****************************************************************************/
/*!
 *  \brief   Main function of stepper motor test module
 *
 *      Main function. Initializes the HAL, opens the stepper motor controller
 *      and calls the test functions. 
 *
 *  \return  Exit status
 *
 ****************************************************************************/

int main (void) {

    if (halInitializeLayer() != NO_ERROR) {
        printf ("HAL initialization failed\n");
        return (-1);
    }
    Handle = halStepperOpen (HAL_STEPPER_MOTOR_0);
    if (Handle < 0) {
        printf ("Opening Stepper X2 failed\n");
        return (-1);
    }

	LoopTest();

    printf ("\n\n=========================================\n");								
    printf (" Stepper positioning test");
    printf ("\n=========================================\n");
    StepperPositionTest();

    halStepperClose (Handle);

    return (0);
}

//****************************************************************************/
