/****************************************************************************/
/*! \file   MoveAck.cpp
 *
 *  \brief  Implementation file for class CMoveAck.
 *
 *  \version  0.1
 *  \date     2012-11-19
 *  \author   Stalin
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

#include "DeviceControl/Include/Devices/MoveAck.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CMoveXYZ
 */
/****************************************************************************/
typedef CSignalTransition<CMoveAck> CMoveAckTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CMoveAck. To create states and register
 *              transition
 *
 *  \iparam     StepperMotor    Stepper motor from which movement acknosledgement
 *                              would be received
 *  \iparam     Name            Name of this State
 *  \iparam     p_Parent        Parent State
 */
/****************************************************************************/
CMoveAck::CMoveAck(CStepperMotor &StepperMotor, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent),
    m_StepperMotor(StepperMotor)
{
    CState *p_MoveWait = new CState("Wait", this);
    QFinalState *p_MoveEnd = new QFinalState(this);
    setInitialState(p_MoveWait);

    p_MoveWait->addTransition(new CMoveAckTransition(&m_StepperMotor, SIGNAL(ReportMovementAckn(quint32,ReturnCode_t,qint32,qint16)),
                                                     *this, &CMoveAck::CheckMovementAckn, p_MoveEnd));
}

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceXyz. To creates states and register
 *              transition
 *
 *  \iparam     p_Event Refers to the arguements list sent by signal.
 *              Arg 0 - Instance ID
 *              Arg 1 - Return Code
 *              Arg 2 - Current Position
 *              Arg 3 - Speed
 *
 *  \return     true, always
 */
/****************************************************************************/
bool CMoveAck::CheckMovementAckn(QEvent *p_Event)
{
    // ReportMovementAckn(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position, qint16 Speed)
    quint32 Position;

    ReturnCode_t ReturnCode = CMoveAckTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit MoveError(ReturnCode);
    }

    if (CMoveAckTransition::GetEventValue(p_Event, 2, Position))
    {
        emit ReportPosition(Position);
    }

    return true;
}

}
