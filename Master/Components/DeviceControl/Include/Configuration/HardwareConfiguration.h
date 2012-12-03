/******************************************************************/
/*! \file HardwareConfiguration.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class HardwareConfiguration
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/******************************************************************/

#ifndef HARDWARECONFIGURATION_H
#define HARDWARECONFIGURATION_H

#include <QMap>
#include <QString>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/SlaveModules/ModuleConfig.h"

class QDomElement;

namespace DeviceControl
{

//! Type definition of list of object configuration objects (data container)
typedef QMap<QString, CModuleConfig*> CANObjectCfgList;

//! Type definition of device configuration objects list (data container)
typedef QList<BaseDeviceConfiguration*> DeviceCfgList;

/****************************************************************************/
/*!
 *  \brief This class reads the device's hardware configuration file
 *
 *      This class reads the device's hardware configuration file
 *      (hw_specification.xml). The class creates a QMap, which contains all
 *      the configuration data.
 *
 *      The class is for temporarily use
 */
/****************************************************************************/
class HardwareConfiguration
{
public:
    HardwareConfiguration();
    ~HardwareConfiguration();

    //! Read the hardware configuration from file
    ReturnCode_t ReadHWSpecification(QString HWConfigFileName);

    //! Returns pointer to device configuration
    BaseDeviceConfiguration* GetDevice(const BaseDeviceConfiguration* xpBaseDeviceConfig);
    CModuleConfig* GetCANNode(const CModuleConfig* pCANObjectNode);
    CModuleConfig* GetCANFctModule(const CModuleConfig* pCANObjectNode, const CModuleConfig* pCANObjectFct);

    static CModuleConfig::CANObjectType_t GetObjectTypeFromString(const QString strCANObjectType);
    static QString GetLabelFromObjectType(const CModuleConfig::CANObjectType_t eObjectType);
    static CANFctModuleStepperMotor::RotationDir_t GetRotationFromString(const QString &strRotDir);

    void GetLastError(quint16& usErrorID, QString& strErrorInfo);

private:
    ReturnCode_t ParseSlaveElement(const QDomElement &element, short sOrderNrNode);
    ReturnCode_t ParseFunctionModule(const QDomElement &element, const CModuleConfig* pCANObject, short sOrderNrFct);

    CModuleConfig*            ParseCANNode(const QDomElement &element, const short sOrderNrNode);
    CANFctModuleDigitInput*   ParseDigitalInPort(const QDomElement &element);
    CANFctModuleDigitOutput*  ParseDigitalOutPort(const QDomElement &element);
    CANFctModuleAnalogInput*  ParseAnalogInPort(const QDomElement &element);
    CANFctModuleAnalogOutput* ParseAnalogOutPort(const QDomElement &element);
    CANFctModuleStepperMotor* ParseStepperMotor(const QDomElement &element);
    CANFctModuleLimitSwitch   ParseLimitSwitch(const QDomElement &element);
    CANFctModulePosCode       ParsePosCode(const QDomElement &element);
    CANFctModuleRFID11785*    ParseRFID11785(const QDomElement &element);
    CANFctModuleRFID15693*    ParseRFID15693(const QDomElement &element);
    CANFctModuleTempCtrl*     ParseTempCtrl(const QDomElement &element);
    CANFctModuleUART*         ParseUART(const QDomElement &element);
    CANFctModuleJoystick*     ParseJoystick(const QDomElement &element);

    void ErrorCleanUp(CANFctModuleStepperMotor* pCANFctModuleStepperMotor);

    //! Parse device element from xml
    BaseDeviceConfiguration* ParseDeviceElement(const QDomElement &element, quint8 orderNrDevice);

    // The list contains all the CANObjectConfiguration* instance addresses
    //! List of object configuration objects (data container)
    CANObjectCfgList m_CANObjectCfgList;
    //! List of device configuration objects (data container)
    DeviceCfgList    m_DeviceCfgList;

    QString m_strErrorInfo; //!< Error information string
    quint16 m_usErrorID;    //!< Error ID
};

} //namespace

#endif /* HARDWARECONFIGURATION_H */
