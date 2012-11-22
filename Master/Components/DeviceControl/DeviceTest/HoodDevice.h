#ifndef HOODDEVICE_H
#define HOODDEVICE_H

#include "Device.h"

namespace DeviceControl
{

class CHoodDevice : public CDevice
{
    Q_OBJECT

public:
    CHoodDevice(CMockFm *fm);
    ~CHoodDevice();

signals:
    // interface to DCP
    void ReadHoodStatus();
    void HoodError(ReturnCode_t Result);
    void ReportHoodStatus(HoodState_t HoodState);

private:
    bool Trans_Idle(QEvent *p_Event);
    bool Trans_Read(QEvent *p_Event);

    CState *mp_Idle;
    CState *mp_Read;
};

} //namespace

#endif // HOODDEVICE_H
