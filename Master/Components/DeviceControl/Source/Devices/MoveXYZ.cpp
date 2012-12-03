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
    new CMoveAck(m_XAxisMotor, "XAxis", p_Moving);
    new CMoveAck(m_YAxisMotor, "YAxis", p_Moving);
    new CMoveAck(m_ZAxisMotor, "ZAxis", p_Moving);

    p_Aborting->setChildMode(QState::ParallelStates);
    new CMoveAck(m_XAxisMotor, "XAxis", p_Aborting);
    new CMoveAck(m_YAxisMotor, "YAxis", p_Aborting);
    new CMoveAck(m_ZAxisMotor, "ZAxis", p_Aborting);

    // Signal Connections
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

}
