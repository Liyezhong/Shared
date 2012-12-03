#include "DeviceControl/Include/Devices/StateMachine/Device.h"

namespace DeviceControl
{

CDevice::CDevice(const DeviceProcessing &DeviceProcessing, const DeviceFctModList &SlaveModuleList, DevInstanceID_t InstanceId) :
    m_DeviceProcessing(DeviceProcessing), m_SlaveModuleList(SlaveModuleList), m_InstanceId(InstanceId)
{

}

quint32 CDevice::GetSlaveModuleInstanceIdFromKey(const QString &Key)
{
    quint32 InstanceID = 0;

    if(m_SlaveModuleList.contains(Key)) {
        InstanceID = m_SlaveModuleList.value(Key);
    }

    return InstanceID;
}

} //namespace
