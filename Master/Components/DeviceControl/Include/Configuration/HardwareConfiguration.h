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

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetObjectTypeFromString
     *
     *  \param strCANObjectType =  QString type parameter
     *
     *  \return from GetObjectTypeFromString
     */
    /****************************************************************************/
    static CModuleConfig::CANObjectType_t GetObjectTypeFromString(const QString strCANObjectType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetLabelFromObjectType
     *
     *  \param eObjectType =  CModuleConfig::CANObjectType_t type parameter
     *
     *  \return from GetLabelFromObjectType
     */
    /****************************************************************************/
    static QString GetLabelFromObjectType(const CModuleConfig::CANObjectType_t eObjectType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetRotationFromString
     *
     *  \param strRotDir =  QString type parameter
     *
     *  \return from GetRotationFromString
     */
    /****************************************************************************/
    static CANFctModuleStepperMotor::RotationDir_t GetRotationFromString(const QString &strRotDir);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetLastError
     *
     *  \param usErrorID = quint16 type parameter
     *  \param strErrorInfo =  QString type parameter
     *
     *  \return from GetLastError
     */
    /****************************************************************************/
    void GetLastError(quint16& usErrorID, QString& strErrorInfo);

private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseSlaveElement
     *
     *  \param element =  QDomElement type parameter
     *  \param sOrderNrNode =  short type parameter
     *
     *  \return from ParseSlaveElement
     */
    /****************************************************************************/
    ReturnCode_t ParseSlaveElement(const QDomElement &element, short sOrderNrNode);
    ReturnCode_t ParseFunctionModule(const QDomElement &element, const CModuleConfig* pCANObject, short sOrderNrFct);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseCANNode
     *
     *  \param element =  QDomElement type parameter
     *  \param sOrderNrNode =  short type parameter
     *
     *  \return from ParseCANNode
     */
    /****************************************************************************/
    CModuleConfig*            ParseCANNode(const QDomElement &element, const short sOrderNrNode);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseDigitalInPort
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseDigitalInPort
     */
    /****************************************************************************/
    CANFctModuleDigitInput*   ParseDigitalInPort(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseDigitalOutPort
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseDigitalOutPort
     */
    /****************************************************************************/
    CANFctModuleDigitOutput*  ParseDigitalOutPort(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseAnalogInPort
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseAnalogInPort
     */
    /****************************************************************************/
    CANFctModuleAnalogInput*  ParseAnalogInPort(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseAnalogOutPort
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseAnalogOutPort
     */
    /****************************************************************************/
    CANFctModuleAnalogOutput* ParseAnalogOutPort(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseStepperMotor
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseStepperMotor
     */
    /****************************************************************************/
    CANFctModuleStepperMotor* ParseStepperMotor(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseLimitSwitch
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseLimitSwitch
     */
    /****************************************************************************/
    CANFctModuleLimitSwitch   ParseLimitSwitch(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParsePosCode
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParsePosCode
     */
    /****************************************************************************/
    CANFctModulePosCode       ParsePosCode(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseRFID11785
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseRFID11785
     */
    /****************************************************************************/
    CANFctModuleRFID11785*    ParseRFID11785(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseRFID15693
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseRFID15693
     */
    /****************************************************************************/
    CANFctModuleRFID15693*    ParseRFID15693(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseTempCtrl
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseTempCtrl
     */
    /****************************************************************************/
    CANFctModuleTempCtrl*     ParseTempCtrl(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseUART
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseUART
     */
    /****************************************************************************/
    CANFctModuleUART*         ParseUART(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParseJoystick
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParseJoystick
     */
    /****************************************************************************/
    CANFctModuleJoystick*     ParseJoystick(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParsePressureCtrl
     *
     *  \param element =  QDomElement type parameter
     *
     *  \return from ParsePressureCtrl
     */
    /****************************************************************************/
    CANFctModulePressureCtrl* ParsePressureCtrl(const QDomElement &element);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ErrorCleanUp
     *
     *  \param pCANFctModuleStepperMotor = CANFctModuleStepperMotor type parameter
     *
     *  \return from ErrorCleanUp
     */
    /****************************************************************************/
    void ErrorCleanUp(CANFctModuleStepperMotor* pCANFctModuleStepperMotor);

    //! Parse device element from xml
    BaseDeviceConfiguration* ParseDeviceElement(const QDomElement &element, quint8 orderNrDevice);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetDeviceIDFromValue
     *
     *  \param DeviceValue = quint32 type parameter
     *
     *  \return from GetDeviceIDFromValue
     */
    /****************************************************************************/
    quint32 GetDeviceIDFromValue(quint32 DeviceValue);

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
