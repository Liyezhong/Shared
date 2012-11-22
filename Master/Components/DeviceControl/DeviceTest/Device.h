#ifndef DEVICECONTROL_DEVICE_H
#define DEVICECONTROL_DEVICE_H

#include <QStateMachine>
#include <QStringList>
//#include <QDateTime>
#include "State.h"
#include "MockFm.h"

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

class CDevice : public QObject
{
    Q_OBJECT

public:
    //CDevice(const DeviceProcessing &DeviceProcessing, 
    //	const DeviceFctModList &SlaveModuleList, 
    //	DevInstanceID_t InstanceId);
    CDevice(CMockFm *fm);
    QStringList GetStates();

signals:
    // command request interface to DCP
    void Configure();
    void Initialize();

    // command response interface to DCP
    void ReportConfigure(ReturnCode_t ReturnCode);
    void ReportInitialize(ReturnCode_t ReturnCode);

    void ReportError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime);

    // to this
    void NeedInitialize(ReturnCode_t ReturnCode);

protected slots:
    void OnMachineErrorEntered();

protected:
    bool Trans_Initializing_Configured(QEvent *p_Event);
    bool Trans_Initializing_Working(QEvent *p_Event);
    bool Initialize_Nack(QEvent *p_Event);

    CMockFm *mp_fm;

    QStateMachine m_machine;
    CState *mp_All;
    CState *mp_MachineError;
    CState *mp_Init;
    CState *mp_Start;
    CState *mp_Configuring;
    CState *mp_Configured;
    CState *mp_Initializing;
    CState *mp_Working;
};

} //namespace

#endif /* DEVICECONTROL_DEVICE_H */

// vi: set ts=4 sw=4 et:

