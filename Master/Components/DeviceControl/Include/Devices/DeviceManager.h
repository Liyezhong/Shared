#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H
#include <QString>
#include <QSharedPointer>
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/hwconfig/hwconfig.hpp"

namespace DeviceControl {

typedef struct AbstractDevice
{
    QString Type;
    quint32 InstanceId;
    CBaseDevice* pDevice;
    quint8 Locked;
}AbstractDevice_t;

class DeviceManager
{
public:
    DeviceManager(hwconfigType* , DeviceProcessing*);

    CBaseDevice* Allocate(const QString& type);
    void Free(CBaseDevice*);

    bool IsEmpty() const {return m_callerDeviceMap.count() == 0;}

    void SetSender(const QString& sender){m_Sender = sender;}

private:
    DeviceManager();
    DeviceManager(const DeviceManager&);
    const DeviceManager& operator =(const DeviceManager& );

private:
    QMap<QString, QList<QSharedPointer<AbstractDevice_t>>> m_callerDeviceMap;
    QString m_Sender;
};

}
#endif // DEVICEMANAGER_H
