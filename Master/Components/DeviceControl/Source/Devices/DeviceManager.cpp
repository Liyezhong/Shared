#include "DeviceControl/Include/Devices/DeviceManager.h"
#include "DeviceControl/Include/Devices/AirLiquidDevice.h"
#include "DeviceControl/Include/Devices/OvenDevice.h"
#include "DeviceControl/Include/Devices/RetortDevice.h"
#include "DeviceControl/Include/Devices/OtherDevice.h"
#include "DeviceControl/Include/Devices/RotaryValveDevice.h"
#include "DeviceControl/Include/Devices/PeripheryDevice.h"


namespace DeviceControl {

DeviceControl::DeviceManager::DeviceManager(hwconfigType* config, DeviceProcessing* deviceProcessing)
{    
    if(config == nullptr || deviceProcessing == nullptr)
        return;

    for(auto retort = config->parameter_master().retorts().retort().begin(); retort != config->parameter_master().retorts().retort().end(); retort++)
    {
        qDebug() << "Retort Name: " + QString::fromStdString((*retort).name());
        QList<QSharedPointer<AbstractDevice_t>> devcieList;
        for(auto device = (*retort).devices().device().begin(); device != (*retort).devices().device().end(); device++)
        {
            bool ok = true;
            auto instanceId = QString::fromStdString((*device).id()).toUInt(&ok, 16);
            auto type = QString::fromStdString((*device).name());


            CBaseDevice *pDevice = deviceProcessing->GetDevice(instanceId);

            if (pDevice != NULL && pDevice->GetMainState() != CBaseDevice::DEVICE_MAIN_STATE_IDLE)
                continue;

            devcieList.append(QSharedPointer<AbstractDevice_t>(new AbstractDevice_t{type, instanceId, pDevice, 0}));
            qDebug() << "Device Name: " + QString::fromStdString((*device).name()) + " InstanceId: " + QString::fromStdString((*device).id());
        }
        m_callerDeviceMap.insert(QString::fromStdString((*retort).name()), devcieList);
    }
}

CBaseDevice* DeviceManager::Allocate(const QString &type)
{
    CBaseDevice* pDevice = nullptr;

    // find the device & mark it as locked
    for(auto item = m_callerDeviceMap.begin(); item != m_callerDeviceMap.end(); item++)
    {
        foreach(auto device, item.value())
        {
            if(device->Type == type && !device->Locked && m_Sender == item.key())
            {
                device->Locked = 0x01;
                pDevice = device->pDevice;
                break;
            }
        }
    }

    if(pDevice == nullptr)
        return pDevice;

    // mark the same device allocated to another AbstractDevice as locked
    for(auto item = m_callerDeviceMap.begin(); item != m_callerDeviceMap.end(); item++)
    {
        if(m_Sender == item.key())
            continue;

        foreach(auto device, item.value())
        {
            if(pDevice == device->pDevice)
            {
                device->Locked = 0x01;
            }
        }
    }

    return pDevice;
}

void DeviceManager::Free(CBaseDevice* pDevice)
{
    // find the device & mark it as unlocked
    for(auto item = m_callerDeviceMap.begin(); item != m_callerDeviceMap.end(); item++)
    {
        foreach(auto device, item.value())
        {
            if(device->pDevice == pDevice)
            {
                device->Locked = 0;
            }
        }
    }
}

}
