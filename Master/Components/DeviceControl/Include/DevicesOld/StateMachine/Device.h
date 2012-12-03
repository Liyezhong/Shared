#ifndef DEVICECONTROL_DEVICE_H
#define DEVICECONTROL_DEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include <QDateTime>

namespace DeviceControl
{

/****************************************************************************/
class CDevice : public QObject
{
    Q_OBJECT

public:
    CDevice(const DeviceProcessing &DeviceProcessing, const DeviceFctModList &SlaveModuleList, DevInstanceID_t InstanceId);

protected:
    quint32 GetSlaveModuleInstanceIdFromKey(const QString &Key);

private:
    const DeviceProcessing &m_DeviceProcessing;
    const DeviceFctModList &m_SlaveModuleList;
    DevInstanceID_t m_InstanceId;
};

} //namespace

#endif /* DEVICECONTROL_DEVICE_H */
