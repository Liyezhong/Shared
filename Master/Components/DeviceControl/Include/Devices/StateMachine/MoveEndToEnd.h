#ifndef DEVICECONTROL_MOVEENDTOEND_H
#define DEVICECONTROL_MOVEENDTOEND_H

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include <QDateTime>

namespace DeviceControl
{

class CMoveEndToEndState;
class CStepperMotor;

/****************************************************************************/
class CMoveEndToEnd : public QObject
{
    Q_OBJECT

    typedef enum {
        Position1,
        Position2
    } Position_t;

public:
    CMoveEndToEnd(CStepperMotor *p_StepperMotor);
    void DoReferenceRun();
    void MoveToPosition1();
    void MoveToPosition2();

public slots:
    void OnReportReferenceMovementAckn(quint32 InstanceId, ReturnCode_t HandleInfo, qint32 Position);
    void OnReportMovementAckn(quint32 InstanceId, ReturnCode_t HandleInfo, qint32 Position, qint16 Speed);

signals:
    void ReportReferenceMovementAckn(quint32 InstanceId, ReturnCode_t HandleInfo, qint32 Position);
    void ReportMovementAckn(quint32 InstanceId, ReturnCode_t HandleInfo, qint32 Position, qint16 Speed);

private:
    friend class CMoveEndToEndState;
    void ChangeState(CMoveEndToEndState *p_State);
    CMoveEndToEndState *mp_State;
    CStepperMotor *mp_StepperMotor;
};

} //namespace

#endif /* DEVICECONTROL_MOVEENDTOEND_H */
