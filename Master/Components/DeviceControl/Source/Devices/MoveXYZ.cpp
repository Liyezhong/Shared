/****************************************************************************/
/*! \file   MoveXyz.cpp
 *
 *  \brief  Implementation file for class CMoveXYZ. To handle 3 axis movement
 *          & acknowledgement for XYZ Transfer System
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

#include "DeviceControl/Include/Devices/MoveXYZ.h"
#include "DeviceControl/Include/Devices/MoveAck.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CMoveXYZ
 */
/****************************************************************************/
typedef CSignalTransition<CMoveXYZ> CMoveXyzTransition;


/****************************************************************************/
/*!
 *  \brief      Constructor of class CMoveXYZ. To create states and register
 *              transition. Connects appropriate signals (Request & response interface)
 *
 *  \iparam     XAxisMotor  Reference of X axis stepper motor (FM)
 *  \iparam     YAxisMotor  Reference of Y axis stepper motor (FM)
 *  \iparam     ZAxisMotor  Reference of Z axis stepper motor (FM)
 *  \iparam     Name        Name of thie state
 *  \iparam     p_Parent    Parent of this state
 */
/****************************************************************************/
CMoveXYZ::CMoveXYZ(CStepperMotor &XAxisMotor, CStepperMotor &YAxisMotor, CStepperMotor &ZAxisMotor,
                   const QString &Name, QState *p_Parent) : CState(Name, p_Parent),
    m_XAxisMotor(XAxisMotor),m_YAxisMotor(YAxisMotor), m_ZAxisMotor(ZAxisMotor)
{
    CState *p_Idle = new CState("Idle", this);
    CState *p_Moving = new CState("Moving", this);
    CState *p_Aborting = new CState("Aborting", this);

    p_Moving->setChildMode(QState::ParallelStates);
    CMoveAck* p_MovingAckX = new CMoveAck(m_XAxisMotor, "X", p_Moving);
    CMoveAck* p_MovingAckY = new CMoveAck(m_YAxisMotor, "Y", p_Moving);
    CMoveAck* p_MovingAckZ = new CMoveAck(m_ZAxisMotor, "Z", p_Moving);

    p_Aborting->setChildMode(QState::ParallelStates);
    CMoveAck* p_AbortingAckX = new CMoveAck(m_XAxisMotor, "X", p_Aborting);
    CMoveAck* p_AbortingAckY = new CMoveAck(m_YAxisMotor, "Y", p_Aborting);
    CMoveAck* p_AbortingAckZ = new CMoveAck(m_ZAxisMotor, "Z", p_Aborting);

    // Signal Connections
    connect(p_MovingAckX, SIGNAL(ReportPosition(quint32)), this, SLOT(SetPositionX(quint32)));
    connect(p_MovingAckY, SIGNAL(ReportPosition(quint32)), this, SLOT(SetPositionY(quint32)));
    connect(p_MovingAckZ, SIGNAL(ReportPosition(quint32)), this, SLOT(SetPositionZ(quint32)));

    connect(p_AbortingAckX, SIGNAL(ReportPosition(quint32)), this, SLOT(SetPositionX(quint32)));
    connect(p_AbortingAckY, SIGNAL(ReportPosition(quint32)), this, SLOT(SetPositionY(quint32)));
    connect(p_AbortingAckZ, SIGNAL(ReportPosition(quint32)), this, SLOT(SetPositionZ(quint32)));

    // Connect Error signal
    connect(p_MovingAckX, SIGNAL(MoveError(ReturnCode_t)), this, SLOT(MoveError(ReturnCode_t)));
    connect(p_MovingAckY, SIGNAL(MoveError(ReturnCode_t)), this, SLOT(MoveError(ReturnCode_t)));
    connect(p_MovingAckZ, SIGNAL(MoveError(ReturnCode_t)), this, SLOT(MoveError(ReturnCode_t)));

    connect(p_AbortingAckX, SIGNAL(MoveError(ReturnCode_t)), this, SLOT(MoveError(ReturnCode_t)));
    connect(p_AbortingAckY, SIGNAL(MoveError(ReturnCode_t)), this, SLOT(MoveError(ReturnCode_t)));
    connect(p_AbortingAckZ, SIGNAL(MoveError(ReturnCode_t)), this, SLOT(MoveError(ReturnCode_t)));

    // State Transitions
    // Idle -> Moving
    p_Idle->addTransition(new CMoveXyzTransition(this, SIGNAL(Move(quint32,quint8,quint32,quint8,quint32,quint8)),
                                                 *this, &CMoveXYZ::Trans_Idle_Moving, p_Moving));

    // Moving -> Idle
    p_Moving->addTransition(new CMoveXyzTransition(p_Moving, SIGNAL(finished()),
                                                   *this, &CMoveXYZ::Trans_Moving_Idle, p_Idle));

    // Moving -> Aborting
    p_Moving->addTransition(new CMoveXyzTransition(this, SIGNAL(Abort()),
                                                   *this, &CMoveXYZ::Trans_Moving_Aborting, p_Aborting));

    // Aborting -> Idle
    p_Aborting->addTransition(new CMoveXyzTransition(p_Aborting, SIGNAL(finished()),
                                                     *this, &CMoveXYZ::Trans_Aborting_Idle, p_Idle));

    setInitialState(p_Idle);

    m_CurrentPositionX = m_CurrentPositionY = m_CurrentPositionZ = 0;
}

/****************************************************************************/
/*!
 *  \brief      To initiate all axis movement when triggered
 *
 *  \iparam     p_Event Refers to the arguements list sent by signal.
 *              Arg 0 - X axis target position
 *              Arg 1 - X axis motion profile
 *              Arg 2 - Y axis target position
 *              Arg 3 - Y axis motion profile
 *              Arg 4 - Z axis target position
 *              Arg 5 - Z axis motion profile
 *
 *  \return     true, if sucessful, else false
 */
/****************************************************************************/
bool CMoveXYZ::Trans_Idle_Moving(QEvent *p_Event)
{
    quint32 PositionX;
    quint32 PositionY;
    quint32 PositionZ;

    quint8 ProfileX;
    quint8 ProfileY;
    quint8 ProfileZ;

    if (!CMoveXyzTransition::GetEventValue(p_Event, 0, PositionX)) {
        return false;
    }
    if (!CMoveXyzTransition::GetEventValue(p_Event, 1, ProfileX)) {
        return false;
    }
    if (!CMoveXyzTransition::GetEventValue(p_Event, 2, PositionY)) {
        return false;
    }
    if (!CMoveXyzTransition::GetEventValue(p_Event, 3, ProfileY)) {
        return false;
    }
    if (!CMoveXyzTransition::GetEventValue(p_Event, 4, PositionZ)) {
        return false;
    }
    if (!CMoveXyzTransition::GetEventValue(p_Event, 5, ProfileZ)) {
        return false;
    }

    if (PositionX == NO_CHANGE)
    {
        PositionX = m_CurrentPositionX;
    }

    if (PositionY == NO_CHANGE)
    {
        PositionY = m_CurrentPositionY;
    }

    if (PositionZ == NO_CHANGE)
    {
        PositionZ = m_CurrentPositionZ;
    }

    qDebug() << "Target:" << PositionX << PositionY << PositionZ;

    ReturnCode_t ReturnCode;

    ReturnCode = m_XAxisMotor.DriveToPosition(PositionX, ProfileX);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportMove(ReturnCode);
        return false;
    }

    ReturnCode = m_YAxisMotor.DriveToPosition(PositionY, ProfileY);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportMove(ReturnCode);
        return false;
    }

    ReturnCode = m_ZAxisMotor.DriveToPosition(PositionZ, ProfileZ);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportMove(ReturnCode);
        return false;
    }

    m_ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    return true;
}

/****************************************************************************/
/*!
 *  \brief      To report movement when axis axis movements are completed
 *
 *  \iparam     p_Event not used
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveXYZ::Trans_Moving_Idle(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    qDebug() << "Current:" << m_CurrentPositionX << m_CurrentPositionY << m_CurrentPositionZ;

    emit ReportMove(m_ReturnCode);

    return true;
}

/****************************************************************************/
/*!
 *  \brief      To abort movement in progress
 *
 *  \iparam     p_Event not used
 *
 *  \return     true, if sucessful, else false
 */
/****************************************************************************/
bool CMoveXYZ::Trans_Moving_Aborting(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode;

    ReturnCode = m_XAxisMotor.DriveSpeed(0, 0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportAbort(ReturnCode);
        return false;
    }

    ReturnCode = m_YAxisMotor.DriveSpeed(0, 0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportAbort(ReturnCode);
        return false;
    }

    ReturnCode = m_ZAxisMotor.DriveSpeed(0, 0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportAbort(ReturnCode);
        return false;
    }

    m_ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    return true;
}

/****************************************************************************/
/*!
 *  \brief      To report movement aborted
 *
 *  \iparam     p_Event not used
 *
 *  \return     true, else
 */
/****************************************************************************/
bool CMoveXYZ::Trans_Aborting_Idle(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReportAbort(m_ReturnCode);

    return true;
}

/****************************************************************************/
/*!
 *  \brief      To set current position of X Axis
 *
 *  \iparam     Position Current position
 */
/****************************************************************************/
void CMoveXYZ::SetPositionX(quint32 Positon)
{
    m_CurrentPositionX = Positon;
}

/****************************************************************************/
/*!
 *  \brief      To set current position of Y Axis
 *
 *  \iparam     Position Current position
 */
/****************************************************************************/
void CMoveXYZ::SetPositionY(quint32 Positon)
{
    m_CurrentPositionY = Positon;
}

/****************************************************************************/
/*!
 *  \brief      To set current position of Z Axis
 *
 *  \iparam     Position Current position
 */
/****************************************************************************/
void CMoveXYZ::SetPositionZ(quint32 Positon)
{
    m_CurrentPositionZ = Positon;
}

//! \todo should check if Mutex is necessary, as its used within a thread.
/****************************************************************************/
/*!
 *  \brief      To get current position of X Axis
 *
 *  \return     Current position
 */
/****************************************************************************/
quint32 CMoveXYZ::GetPositionX()
{
    return m_CurrentPositionX;
}

/****************************************************************************/
/*!
 *  \brief      To get current position of Y Axis
 *
 *  \return     Current position
 */
/****************************************************************************/
quint32 CMoveXYZ::GetPositionY()
{
    return m_CurrentPositionY;
}

/****************************************************************************/
/*!
 *  \brief      To get current position of Z Axis
 *
 *  \return     Current position
 */
/****************************************************************************/
quint32 CMoveXYZ::GetPositionZ()
{
    return m_CurrentPositionZ;
}

/****************************************************************************/
/*!
 *  \brief      To capture error code
 *
 *  \iparam     ReturnCode Error Code
 */
/****************************************************************************/
void CMoveXYZ::MoveError(ReturnCode_t ReturnCode)
{
    m_ReturnCode = ReturnCode;
}

}
