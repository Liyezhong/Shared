/****************************************************************************/
/*! \file   FmStepperInit.cpp
 *
 *  \brief  Implementation file for class CFmStepperInit
 *          To enable and perform reference run for the stepper motor
 *
 *  \version  0.1
 *  \date     2012-09-26
 *  \author   Torsten Scheck
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include <QFinalState>
#include <QDebug>

#include "DeviceControl/Include/Devices/FmStepperInit.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CFmStepperInit
 */
/****************************************************************************/
typedef CSignalTransition<CFmStepperInit> CFmSmInitTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CFmStepperInit
 *
 *  \iparam     p_StepperMotor  Stepper motor function module
 *
 *  \iparam     Name            Name of the state
 *
 *  \iparam     p_Parent        Parent of this state
 */
/****************************************************************************/
CFmStepperInit::CFmStepperInit(
    CStepperMotor *p_StepperMotor,
    const QString &Name, 
    QState *p_Parent) :
        CState(Name, p_Parent), 
        mp_StepperMotor(p_StepperMotor)
{
    CState *p_Start = new CState("StepperInitStart", this);
    CState *p_SetState = new CState("StepperInitSetState", this);
    CState *p_RefRun = new CState("StepperInitRefRun", this);
    QFinalState *p_End = new QFinalState(this);
    setInitialState(p_Start);

    p_Start->addTransition( new CFmSmInitTransition(
        p_Start, SIGNAL(entered()),
        *this, &CFmStepperInit::Trans_Start_SetState,
        p_SetState) );

    p_SetState->addTransition( new CFmSmInitTransition(
        mp_StepperMotor, SIGNAL(ReportSetStateAckn(quint32,ReturnCode_t)),
        *this, &CFmStepperInit::Trans_SetState_RefRun,
        p_RefRun) );

    p_RefRun->addTransition( new CFmSmInitTransition(
        mp_StepperMotor, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)),
        *this, &CFmStepperInit::Trans_RefRun_End,
        p_End) );
}


/****************************************************************************/
/*!
 *  \brief      To enable the stepper motor
 *
 *  \iparam     p_Event unused
 *
 *  \return     false, if function modules returns error, else true
 */
/****************************************************************************/
bool CFmStepperInit::Trans_Start_SetState(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_StepperMotor->SetMotorState(true);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief      Validate SetState() acknowledgement & initiate stepper motor
 *              reference run
 *
 *  \iparam     Contains ReturnCode passed by the signal
 *
 *  \return     false, if function modules acknowledges error, else true
 */
/****************************************************************************/
bool CFmStepperInit::Trans_SetState_RefRun(QEvent *p_Event)
{
    // ReportSetStateAckn(quint32, ReturnCode_t)
    ReturnCode_t ReturnCode = CFmSmInitTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    ReturnCode = mp_StepperMotor->ExecReferenceMovement();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief      Validate reference run acknowledgement
 *
 *  \iparam     Contains ReturnCode passed by the signal
 *
 *  \return     false, if function modules acknowledges error, else true
 */
/****************************************************************************/
bool CFmStepperInit::Trans_RefRun_End(QEvent *p_Event)
{
    // ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)
    ReturnCode_t ReturnCode = CFmSmInitTransition::GetEventValue(p_Event, 1);

    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    return true;
}


} //namespace

// vi: set ts=4 sw=4 et:
