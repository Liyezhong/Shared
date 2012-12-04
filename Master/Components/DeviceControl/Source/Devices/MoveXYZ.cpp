#include <QFinalState>
#include <QDebug>

#include "DeviceControl/Include/Devices/MoveXYZ.h"
#include "DeviceControl/Include/Devices/MoveAck.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CMoveXYZ
 */
/****************************************************************************/
typedef CSignalTransition<CMoveXYZ> CMoveXyzTransition;


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

    return true;
}

bool CMoveXYZ::Trans_Moving_Idle(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    qDebug() << "Current:" << m_CurrentPositionX << m_CurrentPositionY << m_CurrentPositionZ;

    emit ReportMove(DCL_ERR_FCT_CALL_SUCCESS);
    return true;
}

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

    return true;
}

bool CMoveXYZ::Trans_Aborting_Idle(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReportAbort(DCL_ERR_FCT_CALL_SUCCESS);

    return true;
}

void CMoveXYZ::SetPositionX(quint32 Positon)
{
    m_CurrentPositionX = Positon;
}

void CMoveXYZ::SetPositionY(quint32 Positon)
{
    m_CurrentPositionY = Positon;
}

void CMoveXYZ::SetPositionZ(quint32 Positon)
{
    m_CurrentPositionZ = Positon;
}

//! \todo should check if Mutex is necessary, as its used within a thread.
quint32 CMoveXYZ::GetPositionX()
{
    return m_CurrentPositionX;
}

quint32 CMoveXYZ::GetPositionY()
{
    return m_CurrentPositionY;
}

quint32 CMoveXYZ::GetPositionZ()
{
    return m_CurrentPositionZ;
}

}
