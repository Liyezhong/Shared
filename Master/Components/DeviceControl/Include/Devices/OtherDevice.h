#ifndef OTHERDEVICE_H
#define OTHERDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"


namespace DeviceControl
{
class COtherModule;

/****************************************************************************/
/*!
*   \brief This class implements the functionality to configure and control a
*          'Retort' device
*/
/****************************************************************************/
class COtherDevice : public CBaseDevice
{
    Q_OBJECT
public:

    //! constructor
    COtherDevice(DeviceProcessing* pDeviceProcessing, QString& Type,
                  const DeviceModuleList_t &ModuleList,
                  quint32 InstanceID);
    //! destructor
    virtual ~COtherDevice();

    //! general task handling function
    void HandleTasks();

    

private slots:
    ReturnCode_t HandleInitializationState();

private:
    ReturnCode_t InitFunctionModuleLifeTime(const QString& FctModKey, const QString& modName);
    //Function modules
    COtherModule* m_pOtherModules[OD_OtherModule_NUM];              //!< Temperature control modules of the device
    QMap<QString, OtherDeviceModuleType_t> m_mapModules;
};
}

#endif // RETORTDEVICE_H
