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

bool CMoveAck::CheckMovementAckn(QEvent *p_Event)
{
    // ReportMovementAckn(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position, qint16 Speed)
    ReturnCode_t ReturnCode = CMoveAckTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        //            emit NeedInitialize(ReturnCode);
        emit MoveError(ReturnCode);
    }
    return true;
}

}
