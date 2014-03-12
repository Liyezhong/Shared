/****************************************************************************/
/*! \file HardwareConfiguration.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class HardwareConfiguration
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
/****************************************************************************/

#include "DeviceControl/Include/Configuration/HardwareConfiguration.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/SystemPaths.h"

#include <QtXml>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor
 */
/****************************************************************************/
HardwareConfiguration::HardwareConfiguration()
{
    m_strErrorInfo = "";
    m_usErrorID = 0;
}

/****************************************************************************/
/*!
 *  \brief  Destructor
 */
/****************************************************************************/
HardwareConfiguration::~HardwareConfiguration()
{
    try
    {
        while (!m_DeviceCfgList.isEmpty()) {
            delete m_DeviceCfgList.takeFirst();
        }
    }
    catch (...)
    {
        // and exit
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  Returns the last error ID and information string
 *
 *  \oparam usErrorID = error ID
 *  \oparam strErrorInfo = error information string
 */
/****************************************************************************/
void HardwareConfiguration::GetLastError(quint16& usErrorID, QString& strErrorInfo)
{
      strErrorInfo = m_strErrorInfo;
      usErrorID = m_usErrorID;
}

/****************************************************************************/
/*!
 *  \brief  Read the hardware specification file where the configuration procedure based on
 *
 *  \iparam HWConfigFileName = name of the hardware specification file
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the configuration was read successful,
 *          otherwise DCL_ERR_FCT_CALL_FAILED (call GetLastError(..) for further error information)
 */
/****************************************************************************/
ReturnCode_t HardwareConfiguration::ReadHWSpecification(QString HWConfigFileName)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    QString errorStr;
    int errorLine;
    int errorColumn;
    int sOrderNrNode = 0;

    QDomDocument domDocument;
    QDomElement root;
    QDomElement child;

    HWConfigFileName = Global::SystemPaths::Instance().GetSettingsPath() + "/" + HWConfigFileName;
    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing: use config file: " << HWConfigFileName.toStdString();

    QFile file(HWConfigFileName);

    FILE_LOG_L(laINIT, llDEBUG2) << "*******************************************";
    FILE_LOG_L(laINIT, llINFO) << " HardwareConfiguration: ReadHWSpecification";

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        m_strErrorInfo = QObject::tr("Cannot read file %1:\n%2.")
                               .arg(HWConfigFileName)
                               .arg(file.errorString());
            FILE_LOG_L(laINIT, llERROR) << m_strErrorInfo.toStdString();
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_OPEN_FAILED;

            return DCL_ERR_FCT_CALL_FAILED;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,
                                &errorColumn))
    {
        m_strErrorInfo = QObject::tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr);
           FILE_LOG_L(laINIT, llERROR) << m_strErrorInfo.toStdString();
           m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_PARSE_ERROR;

           return DCL_ERR_FCT_CALL_FAILED;
    }

    root = domDocument.documentElement();
    if (root.tagName() != "hwconfig")
    {
        m_strErrorInfo = QObject::tr("The file is not a hardware config file. Tag 'hwconfig' missed");
           FILE_LOG_L(laINIT, llERROR) << m_strErrorInfo.toStdString();
           m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR;

        return DCL_ERR_FCT_CALL_FAILED;
    }
    else if (root.hasAttribute("version")  /// \todo adapt version
               && root.attribute("version") != "1.0")
    {
        m_strErrorInfo = QObject::tr("The hardware config files' version is not valid.");
           FILE_LOG_L(laINIT, llERROR) << m_strErrorInfo.toStdString();
           m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_VERSION_ERROR;

        return DCL_ERR_FCT_CALL_FAILED;
    }

    //-------------------------------------------
    //   slaves
    //-------------------------------------------
    child = root.firstChildElement("parameter_slaves");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    child = child.firstChildElement("slave");
    while (!child.isNull())
    {
        retval = ParseSlaveElement(child, sOrderNrNode);
        if(retval != DCL_ERR_FCT_CALL_SUCCESS)
        {
            break;
        }
        child = child.nextSiblingElement("slave");
        sOrderNrNode++;
    }

    //-------------------------------------------
    //   devices
    //-------------------------------------------
    child = root.firstChildElement("devices");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_DEV;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    BaseDeviceConfiguration* pDevConfig;
    quint8 OrderNrDevice = 0;

    child = child.firstChildElement("device");
    while (!child.isNull())
    {
        pDevConfig = ParseDeviceElement(child, OrderNrDevice);
        if(pDevConfig != NULL)
        {
            OrderNrDevice++;
            m_DeviceCfgList.insert(m_DeviceCfgList.size(), pDevConfig);
        }
        child = child.nextSiblingElement("device");
    }

    FILE_LOG_L(laINIT, llDEBUG2) << " HardwareConfiguration: ReadHWSpecification finished.";
    FILE_LOG_L(laINIT, llDEBUG2) << "*****************************************************";

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Parse device element from xml
 *
 *  \iparam element = Contains the device desription
 *  \iparam orderNrDevice = Order number
 *
 *  \return Pointer to BaseDeviceConfiguration
 */
/****************************************************************************/
BaseDeviceConfiguration* HardwareConfiguration::ParseDeviceElement(const QDomElement &element, quint8 orderNrDevice)
{
    BaseDeviceConfiguration* pDevConfig = 0;
    QDomElement child;
    QString strType, strInstanceID, strOptional;
    QString strKey, strFctInstanceID;
    quint32 InstanceID, FctInstanceID;
    bool ok = true;

    strType = element.attribute("type");
    strInstanceID = element.attribute("dev_instanceID");
    strOptional = element.attribute("optional");

    pDevConfig = new BaseDeviceConfiguration();
    pDevConfig->m_Type = strType;

    InstanceID = strInstanceID.toUInt(&ok, 16);
    pDevConfig->m_InstanceID = GetDeviceIDFromValue(InstanceID);
    if(pDevConfig->m_InstanceID == DEVICE_INSTANCE_ID_UNDEFINED)
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_DEV;
        delete pDevConfig;
        return NULL;
    }

    pDevConfig->m_Optional = (bool) strOptional.toShort(&ok, 10);
    pDevConfig->m_OrderNr = orderNrDevice;

    //-------------------------------------------
    //   dependencies
    //-------------------------------------------
    child = element.firstChildElement("functionmodules");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_DEV;
        return pDevConfig;
    }

    FILE_LOG_L(laINIT, llDEBUG1) << " HardwareConfiguration: Device found:" << strType.toStdString() << ", 0x" << std::hex << InstanceID;

    child = child.firstChildElement("functionmodule");
    while (!child.isNull())
    {
        strKey = child.attribute("key");
        strFctInstanceID = child.attribute("fct_instanceID");

        FctInstanceID = strFctInstanceID.toUInt(&ok, 16);

/*lint -save -e534 */
        //ignoring return value of insert
        pDevConfig->m_DevFctModList.insert(strKey, FctInstanceID);
/*lint -restore */

        FILE_LOG_L(laINIT, llDEBUG1) << " HardwareConfiguration: DeviceDependency found: " << strKey.toStdString() << " 0x" << std::hex << FctInstanceID;

        child = child.nextSiblingElement("functionmodule");
    }
    return pDevConfig;
}

/****************************************************************************/
/*!
 *  \brief  Parse slave element from xml
 *
 *  \iparam element = Contains the slave desription
 *  \iparam sOrderNrNode = Order number
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the configuration was read successful,
 *          otherwise DCL_ERR_FCT_CALL_FAILED (call GetLastError(..) for further error information)
 */
/****************************************************************************/
ReturnCode_t HardwareConfiguration::ParseSlaveElement(const QDomElement &element, short sOrderNrNode)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    QDomElement child;
    CModuleConfig* pCANObjCfgEntry;
    short sOrderNrFct = 0;

    pCANObjCfgEntry = ParseCANNode(element, sOrderNrNode);

    if (m_CANObjectCfgList.contains(pCANObjCfgEntry->m_strKey))
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return DCL_ERR_FCT_CALL_FAILED;
    }
/*lint -save -e534 */
    //ignoring return value of insert
    m_CANObjectCfgList.insert(pCANObjCfgEntry->m_strKey, pCANObjCfgEntry);
/*lint -restore */

    FILE_LOG_L(laINIT, llDEBUG2) << "   slave added:" << pCANObjCfgEntry->m_strKey.toStdString() << ", Order:" << pCANObjCfgEntry->m_sOrderNr;

    child = element.firstChildElement("functionmodules");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    child = child.firstChildElement("functionmodule");
    while (!child.isNull())
    {
        retval = ParseFunctionModule(child, pCANObjCfgEntry, sOrderNrFct);
        if(retval != DCL_ERR_FCT_CALL_SUCCESS)
        {
            // error inside ParseFunctionModule, forward the retval
            break;
        }
        child = child.nextSiblingElement("functionmodule");
        sOrderNrFct++;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Parse CAN node element from xml
 *
 *  \iparam element = Contains the can node desription
 *  \iparam sOrderNrNode = Order number
 *
 *  \return CAN node configuration
 */
/****************************************************************************/
CModuleConfig* HardwareConfiguration::ParseCANNode(const QDomElement &element, const short sOrderNrNode)
{
    CModuleConfig* pCANObjCfgEntry = 0;
    bool bOK = false;

    QDomElement child;
    QString strCANNodeKey;
    QString strCANNodeName;
    QString strCANNodeType;

    int nCANNodeIndex = 0, nCANNodeType = 0;

    strCANNodeKey = element.attribute("key");
    strCANNodeName = element.attribute("name");
    strCANNodeType = element.attribute("type");

    child = element.firstChildElement("nodetype");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjCfgEntry;
    }
    else
    {
        nCANNodeType = child.text().toInt(&bOK, 10);
    }

    child = element.firstChildElement("nodeindex");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjCfgEntry;
    }
    else
    {
        nCANNodeIndex = child.text().toInt(&bOK, 10);
    }

    pCANObjCfgEntry = new CModuleConfig();
    pCANObjCfgEntry->m_ObjectType = CModuleConfig::CAN_OBJ_TYPE_NODE;
    pCANObjCfgEntry->m_strKey        = strCANNodeKey;
    pCANObjCfgEntry->m_strName       = strCANNodeName;
    pCANObjCfgEntry->m_sCANNodeIndex = nCANNodeIndex;
    pCANObjCfgEntry->m_sCANNodeType  = nCANNodeType;
    pCANObjCfgEntry->m_sOrderNr      = sOrderNrNode;

    return pCANObjCfgEntry;
}

/****************************************************************************/
/*!
 *  \brief  Parse function module element from xml
 *
 *  \iparam element = Contains the function module desription
 *  \iparam pCANObjCfgNode = CAN node of the function modules
 *  \iparam sOrderNrFct = Order number
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the configuration was read successful,
 *          otherwise DCL_ERR_FCT_CALL_FAILED (call GetLastError(..) for further error information)
 */
/****************************************************************************/
ReturnCode_t HardwareConfiguration::ParseFunctionModule(const QDomElement &element,
                                                        const CModuleConfig* pCANObjCfgNode,
                                                        short sOrderNrFct)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    QString strCANFctModuleType, strCANFctModuleKey, strCANFctModuleName, strCANFctModuleIface;
    CModuleConfig::CANObjectType_t sObjectType;
    short sIface;
    bool bOK = true;

    strCANFctModuleType = element.attribute("type"); // the type of the function module, e.g. Dig. IO port or steppermotor
    strCANFctModuleKey = element.attribute("key");   // the unique key, must be unique within the application
    strCANFctModuleName = element.attribute("name"); // the name as shown in user interface
    strCANFctModuleIface = element.attribute("interface");

    if (m_CANObjectCfgList.contains(strCANFctModuleKey))
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    sObjectType = GetObjectTypeFromString(strCANFctModuleType);

    sIface = strCANFctModuleIface.toShort(&bOK, 10);
    if(!bOK)
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(CModuleConfig::CAN_OBJ_TYPE_STEPPERMOTOR == sObjectType)
    {
        CANFctModuleStepperMotor* pCANObjFctMotorEntry;

        pCANObjFctMotorEntry = ParseStepperMotor(element);
        if(pCANObjFctMotorEntry)
        {
            pCANObjFctMotorEntry->m_ObjectType = sObjectType;
            pCANObjFctMotorEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctMotorEntry->m_strName = strCANFctModuleName;
            pCANObjFctMotorEntry->pParent = pCANObjCfgNode;
            pCANObjFctMotorEntry->m_sChannel = sIface;
            pCANObjFctMotorEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctMotorEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctMotorEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;

            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctMotorEntry;
            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctMotorEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctMotorEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctMotorEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else if(CModuleConfig::CAN_OBJ_TYPE_DIGITAL_IN_PORT == sObjectType)
    {
        CANFctModuleDigitInput* pCANObjFctDigitInpEntry;

        pCANObjFctDigitInpEntry = ParseDigitalInPort(element);
        if(pCANObjFctDigitInpEntry)
        {
            pCANObjFctDigitInpEntry->m_ObjectType = sObjectType;
            pCANObjFctDigitInpEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctDigitInpEntry->m_strName = strCANFctModuleName;
            pCANObjFctDigitInpEntry->pParent = pCANObjCfgNode;
            pCANObjFctDigitInpEntry->m_sChannel = sIface;
            pCANObjFctDigitInpEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctDigitInpEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctDigitInpEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;
            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctDigitInpEntry;

            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctDigitInpEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctDigitInpEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctDigitInpEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else if(CModuleConfig::CAN_OBJ_TYPE_DIGITAL_OUT_PORT == sObjectType)
    {
        CANFctModuleDigitOutput* pCANObjFctDigitOutEntry;

        pCANObjFctDigitOutEntry = ParseDigitalOutPort(element);
        if(pCANObjFctDigitOutEntry)
        {
            pCANObjFctDigitOutEntry->m_ObjectType = sObjectType;
            pCANObjFctDigitOutEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctDigitOutEntry->m_strName = strCANFctModuleName;
            pCANObjFctDigitOutEntry->pParent = pCANObjCfgNode;
            pCANObjFctDigitOutEntry->m_sChannel = sIface;
            pCANObjFctDigitOutEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctDigitOutEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctDigitOutEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;

            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctDigitOutEntry;

            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctDigitOutEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctDigitOutEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctDigitOutEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else if(CModuleConfig::CAN_OBJ_TYPE_ANALOG_IN_PORT == sObjectType)
    {
        CANFctModuleAnalogInput* pCANObjFctAnalogInEntry;

        pCANObjFctAnalogInEntry = ParseAnalogInPort(element);
        if(pCANObjFctAnalogInEntry)
        {
            pCANObjFctAnalogInEntry->m_ObjectType = sObjectType;
            pCANObjFctAnalogInEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctAnalogInEntry->m_strName = strCANFctModuleName;
            pCANObjFctAnalogInEntry->pParent = pCANObjCfgNode;
            pCANObjFctAnalogInEntry->m_sChannel = sIface;
            pCANObjFctAnalogInEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctAnalogInEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctAnalogInEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;

            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctAnalogInEntry;
            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctAnalogInEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctAnalogInEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctAnalogInEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else if(CModuleConfig::CAN_OBJ_TYPE_ANALOG_OUT_PORT == sObjectType)
    {
        CANFctModuleAnalogOutput* pCANObjFctAnalogOutEntry;

        pCANObjFctAnalogOutEntry = ParseAnalogOutPort(element);
        if(pCANObjFctAnalogOutEntry)
        {
            pCANObjFctAnalogOutEntry->m_ObjectType = sObjectType;
            pCANObjFctAnalogOutEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctAnalogOutEntry->m_strName = strCANFctModuleName;
            pCANObjFctAnalogOutEntry->pParent = pCANObjCfgNode;
            pCANObjFctAnalogOutEntry->m_sChannel = sIface;
            pCANObjFctAnalogOutEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctAnalogOutEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctAnalogOutEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;
            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctAnalogOutEntry;
            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctAnalogOutEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctAnalogOutEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctAnalogOutEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else if (sObjectType == CModuleConfig::CAN_OBJ_TYPE_RFID11785)
    {
        CANFctModuleRFID11785* pCANObjFctRFIDEntry;

        pCANObjFctRFIDEntry = ParseRFID11785(element);
        if(pCANObjFctRFIDEntry)
        {
            pCANObjFctRFIDEntry->m_ObjectType = sObjectType;
            pCANObjFctRFIDEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctRFIDEntry->m_strName = strCANFctModuleName;
            pCANObjFctRFIDEntry->pParent = pCANObjCfgNode;
            pCANObjFctRFIDEntry->m_sChannel = sIface;
            pCANObjFctRFIDEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctRFIDEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctRFIDEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;

            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctRFIDEntry;

            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctRFIDEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctRFIDEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctRFIDEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else if(sObjectType == CModuleConfig::CAN_OBJ_TYPE_RFID15693)
    {
        CANFctModuleRFID15693* pCANObjFctRFIDEntry;

        pCANObjFctRFIDEntry = ParseRFID15693(element);
        if(pCANObjFctRFIDEntry)
        {
            pCANObjFctRFIDEntry->m_ObjectType = sObjectType;
            pCANObjFctRFIDEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctRFIDEntry->m_strName = strCANFctModuleName;
            pCANObjFctRFIDEntry->pParent = pCANObjCfgNode;
            pCANObjFctRFIDEntry->m_sChannel = sIface;
            pCANObjFctRFIDEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctRFIDEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctRFIDEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;

            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctRFIDEntry;

            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctRFIDEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctRFIDEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctRFIDEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else if(sObjectType == CModuleConfig::CAN_OBJ_TYPE_TEMPERATURE_CTL)
    {
        CANFctModuleTempCtrl* pCANObjFctTempEntry;

        pCANObjFctTempEntry = ParseTempCtrl(element);
        if(pCANObjFctTempEntry)
        {
            pCANObjFctTempEntry->m_ObjectType = sObjectType;
            pCANObjFctTempEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctTempEntry->m_strName = strCANFctModuleName;
            pCANObjFctTempEntry->pParent = pCANObjCfgNode;
            pCANObjFctTempEntry->m_sChannel = sIface;
            pCANObjFctTempEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctTempEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctTempEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;

            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctTempEntry;
            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctTempEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctTempEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctTempEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else if(CModuleConfig::CAN_OBJ_TYPE_JOYSTICK == sObjectType)
    {
        CANFctModuleJoystick* pCANObjFctInclEntry;

        pCANObjFctInclEntry = ParseJoystick(element);
        if(pCANObjFctInclEntry)
        {
            pCANObjFctInclEntry->m_ObjectType = sObjectType;
            pCANObjFctInclEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctInclEntry->m_strName = strCANFctModuleName;
            pCANObjFctInclEntry->pParent = pCANObjCfgNode;
            pCANObjFctInclEntry->m_sChannel = sIface;
            pCANObjFctInclEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctInclEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctInclEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;

            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctInclEntry;

            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctInclEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctInclEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctInclEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else if(CModuleConfig::CAN_OBJ_TYPE_UART == sObjectType)
    {
        CANFctModuleUART* pCANObjFctInclEntry;

        pCANObjFctInclEntry = ParseUART(element);
        if(pCANObjFctInclEntry)
        {
            pCANObjFctInclEntry->m_ObjectType = sObjectType;
            pCANObjFctInclEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctInclEntry->m_strName = strCANFctModuleName;
            pCANObjFctInclEntry->pParent = pCANObjCfgNode;
            pCANObjFctInclEntry->m_sChannel = sIface;
            pCANObjFctInclEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctInclEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctInclEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;

            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctInclEntry;

            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctInclEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctInclEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctInclEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
#ifdef PRE_ALFA_TEST
     else if(CModuleConfig::CAN_OBJ_TYPE_PRESSURE_CTL == sObjectType)
    {
        CANFctModulePressureCtrl* pCANObjFctPressureEntry;

        pCANObjFctPressureEntry = ParsePressureCtrl(element);
        if(pCANObjFctPressureEntry)
        {
            pCANObjFctPressureEntry->m_ObjectType = sObjectType;
            pCANObjFctPressureEntry->m_strKey = strCANFctModuleKey;
            pCANObjFctPressureEntry->m_strName = strCANFctModuleName;
            pCANObjFctPressureEntry->pParent = pCANObjCfgNode;
            pCANObjFctPressureEntry->m_sChannel = sIface;
            pCANObjFctPressureEntry->m_sOrderNr = sOrderNrFct;
            pCANObjFctPressureEntry->m_sCANNodeType = pCANObjCfgNode->m_sCANNodeType;
            pCANObjFctPressureEntry->m_sCANNodeIndex = pCANObjCfgNode->m_sCANNodeIndex;

            m_CANObjectCfgList[strCANFctModuleKey] = pCANObjFctPressureEntry;
            FILE_LOG_L(laINIT, llDEBUG1) << "    - fct-mod: " << pCANObjFctPressureEntry->m_strKey.toStdString() <<
                                            ", Order:" << pCANObjFctPressureEntry->m_sOrderNr <<
                                            " Type: " << strCANFctModuleType.toStdString() << ", " << (int) pCANObjFctPressureEntry->m_ObjectType;
        }
        else
        {
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
#endif
    return retCode;

}

/****************************************************************************/
/*!
 *  \brief  Parse digital input port element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleDigitInput* HardwareConfiguration::ParseDigitalInPort(const QDomElement &element)
{
    CANFctModuleDigitInput* pCANObjFctDigitInEntry = 0;

    QDomElement child;
    QString strEnabled, strTimestamp, strPolarity, strThreshold, strInterval, strDebounce;
    bool ok;

    child = element.firstChildElement("configuration");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjFctDigitInEntry;
    }
    strEnabled = child.attribute("enabled");
    strTimestamp = child.attribute("timestamp");
    strPolarity = child.attribute("polarity");
    strThreshold = child.attribute("threshold");
    strInterval = child.attribute("interval");
    strDebounce = child.attribute("debounce");

    pCANObjFctDigitInEntry = new CANFctModuleDigitInput();
    pCANObjFctDigitInEntry->m_bEnabled = strEnabled.toShort(&ok, 10);
#ifdef PRE_ALFA_TEST //Issac request for CV test on 2014.3.11
    if((pCANObjFctDigitInEntry->m_bEnabled < 0)||(pCANObjFctDigitInEntry->m_bEnabled >65535)) {
        delete pCANObjFctDigitInEntry;
        pCANObjFctDigitInEntry = 0;
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return pCANObjFctDigitInEntry;
    }
#endif
    pCANObjFctDigitInEntry->m_bTimeStamp = strTimestamp.toShort(&ok, 10);
    pCANObjFctDigitInEntry->m_sPolarity = strPolarity.toShort(&ok, 10);
#ifdef PRE_ALFA_TEST //Issac request for CV test on 2014.3.11
    if((pCANObjFctDigitInEntry->m_sPolarity < 0)||(pCANObjFctDigitInEntry->m_sPolarity >65535)) {
        delete pCANObjFctDigitInEntry;
        pCANObjFctDigitInEntry = 0;
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return pCANObjFctDigitInEntry;
    }
#endif
    pCANObjFctDigitInEntry->m_sThreshold = strThreshold.toShort(&ok, 10);
#ifdef PRE_ALFA_TEST //Issac request for CV test on 2014.3.11
    if((pCANObjFctDigitInEntry->m_sThreshold < 0)||(pCANObjFctDigitInEntry->m_sThreshold >65535)) {
        delete pCANObjFctDigitInEntry;
        pCANObjFctDigitInEntry = 0;
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return pCANObjFctDigitInEntry;
    }
#endif
    pCANObjFctDigitInEntry->m_bInterval = strInterval.toShort(&ok, 10);
    pCANObjFctDigitInEntry->m_bDebounce = strDebounce.toShort(&ok, 10);

    return pCANObjFctDigitInEntry;

}

/****************************************************************************/
/*!
 *  \brief  Parse digital output port element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleDigitOutput* HardwareConfiguration::ParseDigitalOutPort(const QDomElement &element)
{
    CANFctModuleDigitOutput* pCANObjFctDigitOutEntry = 0;
    QDomElement child;
    QString strEnabled, strInactivShdw, strInactivEmcy, strInactivStby, strPolarity, strOutvalInactiv, strLivetimeLimit;
    bool ok;

    child = element.firstChildElement("configuration");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjFctDigitOutEntry;
    }

    strEnabled       = child.attribute("enabled");
    strInactivShdw   = child.attribute("inactiv_shutdown");
    strInactivEmcy   = child.attribute("inactiv_emcy");
    strPolarity      = child.attribute("polarity");
    strOutvalInactiv = child.attribute("outval_inactiv");
    strLivetimeLimit = child.attribute("livetime_limit");

    pCANObjFctDigitOutEntry = new CANFctModuleDigitOutput();

    pCANObjFctDigitOutEntry->m_bEnabled           = strEnabled.toShort(&ok, 10);
#ifdef PRE_ALFA_TEST //Issac request for CV test on 2014.3.11
    if((pCANObjFctDigitOutEntry->m_bEnabled < 0)||(pCANObjFctDigitOutEntry->m_bEnabled >65535)) {
        delete pCANObjFctDigitOutEntry;
        pCANObjFctDigitOutEntry = 0;
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return pCANObjFctDigitOutEntry;
    }
#endif
    pCANObjFctDigitOutEntry->m_bInaktivAtShutdown = strInactivShdw.toShort(&ok, 10);
#ifdef PRE_ALFA_TEST //Issac request for CV test on 2014.3.11
    if((pCANObjFctDigitOutEntry->m_bInaktivAtShutdown < 0)||(pCANObjFctDigitOutEntry->m_bInaktivAtShutdown >65535)) {
        delete pCANObjFctDigitOutEntry;
        pCANObjFctDigitOutEntry = 0;
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return pCANObjFctDigitOutEntry;
    }
#endif
    pCANObjFctDigitOutEntry->m_bInaktivAtEmgyStop = strInactivEmcy.toShort(&ok, 10);
#ifdef PRE_ALFA_TEST //Issac request for CV test on 2014.3.11
    if((pCANObjFctDigitOutEntry->m_bInaktivAtEmgyStop < 0)||(pCANObjFctDigitOutEntry->m_bInaktivAtEmgyStop >65535)) {
        delete pCANObjFctDigitOutEntry;
        pCANObjFctDigitOutEntry = 0;
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return pCANObjFctDigitOutEntry;
    }
#endif
    pCANObjFctDigitOutEntry->m_sPolarity          = strPolarity.toShort(&ok, 10);
    pCANObjFctDigitOutEntry->m_sOutvalInactiv     = strOutvalInactiv.toShort(&ok, 10);
#ifdef PRE_ALFA_TEST //Issac request for CV test on 2014.3.11
    if((pCANObjFctDigitOutEntry->m_sOutvalInactiv < 0)||(pCANObjFctDigitOutEntry->m_sOutvalInactiv >65535)) {
        delete pCANObjFctDigitOutEntry;
        pCANObjFctDigitOutEntry = 0;
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return pCANObjFctDigitOutEntry;
    }
#endif
    pCANObjFctDigitOutEntry->m_sLivetimeLimit     = strLivetimeLimit.toShort(&ok, 10);
#ifdef PRE_ALFA_TEST //Issac request for CV test on 2014.3.11
    if((pCANObjFctDigitOutEntry->m_sLivetimeLimit < 0)||(pCANObjFctDigitOutEntry->m_sLivetimeLimit >65535)) {
        delete pCANObjFctDigitOutEntry;
        pCANObjFctDigitOutEntry = 0;
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
        return pCANObjFctDigitOutEntry;
    }
#endif

    return pCANObjFctDigitOutEntry;
}

/****************************************************************************/
/*!
 *  \brief  Parse analog input port element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleAnalogInput* HardwareConfiguration::ParseAnalogInPort(const QDomElement &element)
{
    CANFctModuleAnalogInput* pCANObjFctAnalogInEntry = 0;
    QDomElement child;
    QString strEnabled, strTimestamp, strFastSampling, strSupervision, strLimitAutoSend, strInterval, strDebounce;
    QString strValue1SendExceed, strValue1SendBelow, strValue1SendWarnMsg, strValue1SendDataMsg, strValue1;
    QString strValue2SendExceed, strValue2SendBelow, strValue2SendWarnMsg, strValue2SendDataMsg, strValue2;
    QString strHysteresis;
    bool ok;

    child = element.firstChildElement("configuration");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjFctAnalogInEntry;
    }

    strEnabled = child.attribute("enabled");
    strTimestamp = child.attribute("timestamp");
    strFastSampling = child.attribute("fast_sampling");
    strSupervision = child.attribute("limit_supervision");
    strLimitAutoSend = child.attribute("limit_autosend");
    strInterval = child.attribute("interval");
    strDebounce = child.attribute("debounce");

    child = element.firstChildElement("limit_supervision");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjFctAnalogInEntry;
    }

    strValue1SendExceed  = child.attribute("value1_send_at_exceed");
    strValue1SendBelow   = child.attribute("value1_send_at_fall_below");
    strValue1SendWarnMsg = child.attribute("value1_send_warn_message");
    strValue1SendDataMsg = child.attribute("value1_send_data_message");
    strValue1            = child.attribute("value1");

    strValue2SendExceed  = child.attribute("value2_send_at_exceed");
    strValue2SendBelow   = child.attribute("value2_send_at_fall_below");
    strValue2SendWarnMsg = child.attribute("value2_send_warn_message");
    strValue2SendDataMsg = child.attribute("value2_send_data_message");
    strValue2            = child.attribute("value2");

    strHysteresis        = child.attribute("hysteresis");

    pCANObjFctAnalogInEntry = new CANFctModuleAnalogInput();

    pCANObjFctAnalogInEntry->m_bEnabled = strEnabled.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_bTimeStamp = strTimestamp.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_bFastSampling = strFastSampling.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_sLimitAutoSend = strLimitAutoSend.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_sInterval = strInterval.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_sDebounce = strDebounce.toShort(&ok, 10);

    pCANObjFctAnalogInEntry->m_bLimitValue1SendExceed  = strValue1SendExceed.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_bLimitValue1SendBelow   = strValue1SendBelow.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_bLimitValue1SendWarnMsg = strValue1SendWarnMsg.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_bLimitValue1SendDataMsg = strValue1SendDataMsg.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_sLimitValue1            = strValue1.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_bLimitValue2SendExceed  = strValue2SendExceed.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_bLimitValue2SendBelow   = strValue2SendBelow.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_bLimitValue2SendWarnMsg = strValue2SendWarnMsg.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_bLimitValue2SendDataMsg = strValue2SendDataMsg.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_sLimitValue2            = strValue2.toShort(&ok, 10);
    pCANObjFctAnalogInEntry->m_sHysteresis             = strHysteresis.toShort(&ok, 10);

    return pCANObjFctAnalogInEntry;
}

/****************************************************************************/
/*!
 *  \brief  Parse analog output port element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleAnalogOutput* HardwareConfiguration::ParseAnalogOutPort(const QDomElement &element)
{
    CANFctModuleAnalogOutput* pCANObjFctAnalogOutEntry = 0;
    QDomElement child;
    QString strEnabled, strInactivShdw, strInactivEmcy, strResolution, strOutvalInactiv, strLivetimeLimit;
    bool ok;

    child = element.firstChildElement("configuration");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjFctAnalogOutEntry;
    }

    strEnabled       = child.attribute("enabled");
    strInactivShdw   = child.attribute("inactiv_shutdown");
    strInactivEmcy   = child.attribute("inactiv_emcy");
    strResolution    = child.attribute("resolution");
    strOutvalInactiv = child.attribute("outval_inactiv");
    strLivetimeLimit = child.attribute("livetime_limit");

    pCANObjFctAnalogOutEntry = new CANFctModuleAnalogOutput();

    pCANObjFctAnalogOutEntry->m_bEnabled           = strEnabled.toShort(&ok, 10);
    pCANObjFctAnalogOutEntry->m_bInaktivAtShutdown = strInactivShdw.toShort(&ok, 10);
    pCANObjFctAnalogOutEntry->m_bInaktivAtEmgyStop = strInactivEmcy.toShort(&ok, 10);
    pCANObjFctAnalogOutEntry->m_sMode              = 0x01; //enable
    pCANObjFctAnalogOutEntry->m_sBitCount          = strResolution.toShort(&ok, 10);
    pCANObjFctAnalogOutEntry->m_sOutvalInactiv     = strOutvalInactiv.toShort(&ok, 10);
    pCANObjFctAnalogOutEntry->m_sLivetimeLimit     = strLivetimeLimit.toShort(&ok, 10);

    return pCANObjFctAnalogOutEntry;
}

/****************************************************************************/
/*!
 *  \brief  Clean up the stepper motor configuration
 *
 *  \iparam pCANFctModuleStepperMotor = Stepper motor configuration
 */
/****************************************************************************/
void HardwareConfiguration::ErrorCleanUp(CANFctModuleStepperMotor* pCANFctModuleStepperMotor)
{
    m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
    delete pCANFctModuleStepperMotor;
}

/****************************************************************************/
/*!
 *  \brief  Parse stepper motor element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleStepperMotor* HardwareConfiguration::ParseStepperMotor(const QDomElement &element)
{
    //ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    CANFctModuleStepperMotor* pCANFctModuleStepperMotor = 0;
    bool ok;
    QDomElement child;
    QDomElement childPosCoverage;
    QDomElement childLimitSwitches;
    QDomElement childLimitSwitch;
    QDomElement childPosCode;
    QDomElement childMotionProfiles;
    QDomElement childMotionProfile;
    QString strRotationType, strStepsRev, strDirection, strPositionReset, strPositionMin, strPositionMax, strSpeedMin, strSpeedMax, strRunCurrent, strStopCurrent, strStopCurrentDelay;
    QString strRefRunRefPos, strRefRunMaxDistance, strRefRunTimeout, strRefRunReverseDist, strRefRunSlowSpeed, strRefRunHighSpeed, strRefPosOffset;
    QString strEncoderType, strEncoderRes, strEncoderRot;
    QString strLSOrientation, strLSIndex;
    QString strPosCodeIndex;
    QString strProfileSpeedMin, strProfileSpeedMax, strProfileMicroSteps, strProfileRampSteps;
    QString strProfileAcc, strProfileDec, strProfileAccTime, strProfileDecTime;
    quint8  sProfileIdx;
/*
#ifdef PRE_ALFA_TEST
    QString strRefRunRefPosSkip;
#endif
*/

    //  motor resolution
    child = element.firstChildElement("rotation");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANFctModuleStepperMotor;
    }

    strRotationType     = child.attribute("type");
    strStepsRev         = child.attribute("steps_revolution");
    strDirection        = child.attribute("direction");
    strPositionReset    = child.attribute("position_reset");
    strPositionMin      = child.attribute("position_min");
    strPositionMax      = child.attribute("position_max");
    strSpeedMin         = child.attribute("speed_min");
    strSpeedMax         = child.attribute("speed_max");
    strRunCurrent       = child.attribute("run_cs");
    strStopCurrent      = child.attribute("stop_cs");
    strStopCurrentDelay = child.attribute("stop_cs_delay");

    pCANFctModuleStepperMotor = new CANFctModuleStepperMotor();

    if(strRotationType == "rot")
    {
        pCANFctModuleStepperMotor->rotationType = CANFctModuleStepperMotor::ROTATION_TYPE_ROT;
    }
    else
    {
        pCANFctModuleStepperMotor->rotationType = CANFctModuleStepperMotor::ROTATION_TYPE_LINEAR;
    }

    pCANFctModuleStepperMotor->sResolution = strStepsRev.toShort(&ok, 10);
    pCANFctModuleStepperMotor->bDirection = GetRotationFromString(strDirection);

    pCANFctModuleStepperMotor->sResetPosition = strPositionReset.toShort(&ok, 10);
    pCANFctModuleStepperMotor->lMinPosition = strPositionMin.toLong(&ok, 10);
    pCANFctModuleStepperMotor->lMaxPosition = strPositionMax.toLong(&ok, 10);

    pCANFctModuleStepperMotor->sMinSpeed = strSpeedMin.toShort(&ok, 10);
    pCANFctModuleStepperMotor->sMaxSpeed = strSpeedMax.toShort(&ok, 10);

    pCANFctModuleStepperMotor->runCurrentScale = strRunCurrent.toShort(&ok, 10);
    pCANFctModuleStepperMotor->stopCurrentScale = strStopCurrent.toShort(&ok, 10);
    pCANFctModuleStepperMotor->stopCurrentDelay = strStopCurrentDelay.toShort(&ok, 10);

#ifdef PRE_ALFA_TEST //added for V&V CV test, requested by Brandon, 2013.7.16
        if((pCANFctModuleStepperMotor->runCurrentScale < 0)||(pCANFctModuleStepperMotor->runCurrentScale >31)) {
            delete pCANFctModuleStepperMotor;
            pCANFctModuleStepperMotor = 0;
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return pCANFctModuleStepperMotor;
        }
        if((pCANFctModuleStepperMotor->stopCurrentScale < 0)||(pCANFctModuleStepperMotor->stopCurrentScale >31)) {
            delete pCANFctModuleStepperMotor;
            pCANFctModuleStepperMotor = 0;
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return pCANFctModuleStepperMotor;
        }
        quint32 stopCurrentDelay = strStopCurrentDelay.toUInt(&ok, 10);
        if((stopCurrentDelay < 0)||(stopCurrentDelay >65535)) {
            delete pCANFctModuleStepperMotor;
            pCANFctModuleStepperMotor = 0;
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return pCANFctModuleStepperMotor;
        }
#endif

    FILE_LOG_L(laINIT, llDEBUG4) << " Motorconfiguration: " << strPositionMin.toStdString() << " - " << strPositionMax.toStdString() <<
            " position: " << pCANFctModuleStepperMotor->lMinPosition << "- " << pCANFctModuleStepperMotor->lMaxPosition;

    //  motor reference run
    child = element.firstChildElement("reference_run");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;

        delete pCANFctModuleStepperMotor;
        pCANFctModuleStepperMotor = 0;
        return pCANFctModuleStepperMotor;
    }
    strRefRunRefPos      = child.attribute("ref_position");
    strRefRunMaxDistance = child.attribute("max_distance");
    strRefRunTimeout     = child.attribute("timeout");
    strRefRunReverseDist = child.attribute("reverse_distance");
    strRefRunSlowSpeed   = child.attribute("slow_speed");
    strRefRunHighSpeed   = child.attribute("high_speed");
    strRefPosOffset      = child.attribute("refpos_offset");
/*
#ifdef PRE_ALFA_TEST
    strRefRunRefPosSkip  = child.attribute("refpos_skip");
#endif
*/

    pCANFctModuleStepperMotor->refRunRefPos           = strRefRunRefPos.toLong(&ok, 10);
    pCANFctModuleStepperMotor->lRefRunMaxDistance     = strRefRunMaxDistance.toLong(&ok, 10);
#ifndef PRE_ALFA_TEST
    pCANFctModuleStepperMotor->sRefRunTimeout         = strRefRunTimeout.toShort(&ok, 10);
#else
    pCANFctModuleStepperMotor->sRefRunTimeout         = strRefRunTimeout.toUShort(&ok, 10);
#endif
    pCANFctModuleStepperMotor->lRefRunReverseDistance = strRefRunReverseDist.toLong(&ok, 10);
    pCANFctModuleStepperMotor->sRefRunSlowSpeed       = strRefRunSlowSpeed.toShort(&ok, 10);
    pCANFctModuleStepperMotor->sRefRunHighSpeed       = strRefRunHighSpeed.toShort(&ok, 10);
    pCANFctModuleStepperMotor->lRefPosOffset          = strRefPosOffset.toLong(&ok, 10);
/*
#ifdef PRE_ALFA_TEST
    if(strRefRunRefPosSkip.isEmpty()) {
        pCANFctModuleStepperMotor->refRunRefPosSkip   = 0;
    }
    else {
        pCANFctModuleStepperMotor->refRunRefPosSkip   = strRefRunRefPosSkip.toShort(&ok, 10);
    }
#endif
*/
    //############################
    // position coverage
    childPosCoverage = element.firstChildElement("position_coverage");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        delete pCANFctModuleStepperMotor;
        pCANFctModuleStepperMotor = 0;
        return pCANFctModuleStepperMotor;
    }

    //  encoder
    child = childPosCoverage.firstChildElement("encoder");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        delete pCANFctModuleStepperMotor;
        pCANFctModuleStepperMotor = 0;
        return pCANFctModuleStepperMotor;
    }

    strEncoderType = child.attribute("type");
    pCANFctModuleStepperMotor->bEncoderType = strEncoderType.toShort(&ok, 10);
    if(pCANFctModuleStepperMotor->bEncoderType)
    {
        //if encoder exists, read resolutoin and rotation direction
        strEncoderRes = child.attribute("resolution");
        strEncoderRot = child.attribute("rotation");
        pCANFctModuleStepperMotor->sEncoderResolution = strEncoderRes.toShort(&ok, 10);
        pCANFctModuleStepperMotor->bEncoderDir = GetRotationFromString(strEncoderRot);
    }

    //##########################
    //  limit switches
    childLimitSwitches = childPosCoverage.firstChildElement("limitswitches");
    childLimitSwitch = childLimitSwitches.firstChildElement("limitswitch");
    while (!childLimitSwitch.isNull()) {
        CANFctModuleLimitSwitch LimitSwitch = ParseLimitSwitch(childLimitSwitch);

        strLSOrientation = childLimitSwitch.attribute("orientation");
        strLSIndex = childLimitSwitch.attribute("index");

        if(strLSOrientation == "cw") {
            LimitSwitch.bOrientation = 1;
        }
        else if(strLSOrientation == "ccw") {
            LimitSwitch.bOrientation = 2;
        }

        LimitSwitch.bIndex = strLSIndex.toShort(&ok, 10);
        if(LimitSwitch.bIndex == 0) {
            pCANFctModuleStepperMotor->LimitSwitch1 = LimitSwitch;
        }
        else if(LimitSwitch.bIndex == 1) {
            pCANFctModuleStepperMotor->LimitSwitch2 = LimitSwitch;
        }
#ifdef PRE_ALFA_TEST //added for V&V CV test, requested by Brandon, 2013.7.16
        else if((LimitSwitch.bIndex != 0)&&(LimitSwitch.bIndex != 1)) {
            delete pCANFctModuleStepperMotor;
            pCANFctModuleStepperMotor = 0;
            m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
            return pCANFctModuleStepperMotor;
        }
#endif

        childLimitSwitch = childLimitSwitch.nextSiblingElement("limitswitch");
    }

    //##########################
    //  limit switches position code
    childPosCode = childLimitSwitches.firstChildElement("position_code");
    while (!childPosCode.isNull()) {
        strPosCodeIndex = childPosCode.attribute("value");
        quint8 posCodeIndex = strPosCodeIndex.toShort(&ok, 10);

        if ((posCodeIndex <= 0) || (posCodeIndex >= 4)) {
            ErrorCleanUp(pCANFctModuleStepperMotor);
            return 0;
        }

        if(posCodeIndex == 1) {
            pCANFctModuleStepperMotor->PosCode1 = ParsePosCode(childPosCode);
        }
        else if(posCodeIndex == 2) {
            pCANFctModuleStepperMotor->PosCode2 = ParsePosCode(childPosCode);
        }
        else if(posCodeIndex == 3) {
            pCANFctModuleStepperMotor->PosCode3 = ParsePosCode(childPosCode);
        }

        childPosCode = childPosCode.nextSiblingElement("position_code");
    }

    //##########################
    //supervision
    QString strStepLossWarnLimit, strStepLossErrorLimit, strCurrentLimit;

    child = element.firstChildElement("supervision");
    if(child.isNull())
    {
        ErrorCleanUp(pCANFctModuleStepperMotor);
        return 0;
    }
    strStepLossWarnLimit  = child.attribute("steploss_warn_limit");
    strStepLossErrorLimit = child.attribute("steploss_error_limit");
    strCurrentLimit       = child.attribute("current_limit");

    pCANFctModuleStepperMotor->sStepLossWarnLimit  = strStepLossWarnLimit.toShort(&ok, 10);
    pCANFctModuleStepperMotor->sStepLossErrorLimit = strStepLossErrorLimit.toShort(&ok, 10);
    pCANFctModuleStepperMotor->sCurrentLimit       = strCurrentLimit.toShort(&ok, 10);

    //############################
    // motor driver configuration
    pCANFctModuleStepperMotor->driverType = CANFctModuleStepperMotor::DRIVER_DEFAULT;

    child = element.firstChildElement("driver");
    if (!child.isNull())
    {
        QString strDriverType = child.attribute("type");

        if ("TMC26X" == strDriverType.toUpper())
        {
            pCANFctModuleStepperMotor->driverType = CANFctModuleStepperMotor::DRIVER_TMC26X;

            QString strTMC26XdrvConf   = child.attribute("reg_drvConf");
            QString strTMC26XsgcConf   = child.attribute("reg_sgcsConf");
            QString strTMC26XsmartEn   = child.attribute("reg_smartEn");
            QString strTMC26XchopConf  = child.attribute("reg_chopConf");
#ifdef PRE_ALFA_TEST //added for V&V CV test, requested by Brandon, 2013.7.16
            quint32 chopConf =  strTMC26XchopConf.toUInt(&ok, 16);
            if((chopConf & 0xF) == 0)
            {
                delete pCANFctModuleStepperMotor;
                pCANFctModuleStepperMotor = 0;
                m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT;
                return pCANFctModuleStepperMotor;
            }
#endif
            pCANFctModuleStepperMotor->tmc26x.drvConf  = strTMC26XdrvConf.toUInt(&ok, 16);
            if (ok)
                pCANFctModuleStepperMotor->tmc26x.sgcsConf  = strTMC26XsgcConf.toUInt(&ok, 16);
            if (ok)
                pCANFctModuleStepperMotor->tmc26x.smartEn  = strTMC26XsmartEn.toUInt(&ok, 16);
            if (ok)
                pCANFctModuleStepperMotor->tmc26x.chopConf  = strTMC26XchopConf.toUInt(&ok, 16);

            if (!ok)
                pCANFctModuleStepperMotor->driverType = CANFctModuleStepperMotor::DRIVER_DEFAULT;
        }
    }

    //############################
    // motion profiles
    childMotionProfiles = element.firstChildElement("motion_profiles");
    if(childMotionProfiles.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        delete pCANFctModuleStepperMotor;
        pCANFctModuleStepperMotor = 0;

        return pCANFctModuleStepperMotor;
    }

    // loop thru the motion profiles, create a data instace for storage and place it into the list
    sProfileIdx = 0;
    childMotionProfile = childMotionProfiles.firstChildElement("motion_profile");
    while (!childMotionProfile.isNull())
    {
        CANFctModuleMotionProfile MotionProfile;

        // speed_min="25" speed="600" acc="200" dec="200" micro_steps="64" ramp="1" />
        strProfileSpeedMin = childMotionProfile.attribute("speed_min");
        strProfileSpeedMax = childMotionProfile.attribute("speed_max");
        strProfileAcc = childMotionProfile.attribute("acc");
        strProfileDec = childMotionProfile.attribute("dec");
        strProfileAccTime = childMotionProfile.attribute("acc_time");
        strProfileDecTime = childMotionProfile.attribute("dec_time");
        strProfileMicroSteps = childMotionProfile.attribute("micro_steps");
        strProfileRampSteps  = childMotionProfile.attribute("ramp_type");

        MotionProfile.bIndex = sProfileIdx;
        MotionProfile.sSpeedMin = strProfileSpeedMin.toShort(&ok, 10);
        MotionProfile.sSpeedMax = strProfileSpeedMax.toShort(&ok, 10);
        MotionProfile.sAcc = strProfileAcc.toShort(&ok, 10);
        MotionProfile.sDec = strProfileDec.toShort(&ok, 10);;
        MotionProfile.sAccTime = strProfileAccTime.toShort(&ok, 10);
        MotionProfile.sDecTime = strProfileDecTime.toShort(&ok, 10);
        MotionProfile.bMicroSteps = strProfileMicroSteps.toShort(&ok, 10);
        MotionProfile.bRampType = strProfileRampSteps.toShort(&ok, 10);

/*lint -save -e534 */
        //ignoring return value of insert
        pCANFctModuleStepperMotor->listMotionProfiles.insert(MotionProfile.bIndex, MotionProfile);
/*lint -restore */
        sProfileIdx++;

        childMotionProfile = childMotionProfile.nextSiblingElement("motion_profile");
    }

    return pCANFctModuleStepperMotor;
}

/****************************************************************************/
/*!
 *  \brief  Parses the stepper motor limit switch description
 *
 *  \iparam element = Contains the limit switch desription
 *
 *  \return Stepper motor limit switch configuration
 */
/****************************************************************************/
CANFctModuleLimitSwitch HardwareConfiguration::ParseLimitSwitch(const QDomElement &element)
{
    CANFctModuleLimitSwitch LimitSwitch;
    QDomElement child;
    QString strPolarity, strSampleRate, strDebounce;
    bool ok;

    child = element.firstChildElement("configuration");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return LimitSwitch;
    }

    strPolarity = child.attribute("polarity");
    strSampleRate = child.attribute("sample_rate");
    strDebounce = child.attribute("debounce");

    LimitSwitch.bExists = 1;
    LimitSwitch.bPolarity = strPolarity.toShort(&ok, 10);
    LimitSwitch.bSampleRate = strSampleRate.toShort(&ok, 10);
    LimitSwitch.bDebounce = strDebounce.toShort(&ok, 10);

    return LimitSwitch;

}

/****************************************************************************/
/*!
 *  \brief  Parses the stepper motor position code description
 *
 *  \iparam element = Contains the position code desription
 *
 *  \return Stepper motor position code configuration
 */
/****************************************************************************/
CANFctModulePosCode HardwareConfiguration::ParsePosCode(const QDomElement &element)
{
    CANFctModulePosCode PositionCode;
    QString strStop, strStopDir, strPosition, strWidth, strDeviation;
#ifdef PRE_ALFA_TEST
    QString strRotDirCheck, strHitSkip;
#endif
    bool ok;


    strStop = element.attribute("stop");
    strStopDir = element.attribute("stop_dir");
    strPosition = element.attribute("position");
    strWidth = element.attribute("width");
    strDeviation = element.attribute("deviation");
#ifdef PRE_ALFA_TEST
    strRotDirCheck = element.attribute("dir_check");
    strHitSkip = element.attribute("hit_skip");
#endif

    PositionCode.bStop = strStop.toShort(&ok, 10);
    if(strStopDir == "cw") {
        PositionCode.bStopDir = 1;
    }
    else if(strStopDir == "ccw") {
        PositionCode.bStopDir = 2;
    }
    else {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return PositionCode;
    }
    PositionCode.bValid = true;
    PositionCode.position = strPosition.toShort(&ok, 10);
    PositionCode.width = strWidth.toShort(&ok, 10);
    PositionCode.deviation = strDeviation.toShort(&ok, 10);
#ifdef PRE_ALFA_TEST
    if (strRotDirCheck.isEmpty()) {
        PositionCode.bRotDirCheck = 1;
    }
    else {
        PositionCode.bRotDirCheck = strRotDirCheck.toShort(&ok, 10);
    }

    if (strHitSkip.isEmpty()) {
        PositionCode.hitSkip = 0;
    }
    else {
        PositionCode.hitSkip = strHitSkip.toShort(&ok, 10);
    }
#endif

    return PositionCode;
}

/****************************************************************************/
/*!
 *  \brief  Parse RFID ISO 11785 element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleRFID11785* HardwareConfiguration::ParseRFID11785(const QDomElement &element)
{
    Q_UNUSED(element)
    return new CANFctModuleRFID11785();
}

/****************************************************************************/
/*!
 *  \brief  Parse RFID ISO 15693 element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleRFID15693* HardwareConfiguration::ParseRFID15693(const QDomElement &element)
{
    Q_UNUSED(element)
    return new CANFctModuleRFID15693();
}

/****************************************************************************/
/*!
 *  \brief  Parse temperature control element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleTempCtrl* HardwareConfiguration::ParseTempCtrl(const QDomElement &element)
{
    CANFctModuleTempCtrl* pCANObjFctTempCtrl = 0;
    QDomElement child;
    QDomElement childPidControllers;
    QDomElement childPidController;
    QString strTempTolerance, strSamplingPeriod, strFanSpeed, strFanThreshold,
            strCurrentGain, strHeaterCurrent, strHeaterThreshold, strCurrentDeviation;
    QString strMaxTemperature, strControllerGain, strResetTime, strDerivativeTime;
    QString strCurrentMin230_Serial, strCurrentMax230_Serial, strCurrentMin100_Serial, strCurrentMax100_Serial, strCurrentMin100_Parallel, strCurrentMax100_Parallel;
    bool ok;

    child = element.firstChildElement("configuration");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjFctTempCtrl;
    } 

    strTempTolerance = child.attribute("temp_tolerance");
    strSamplingPeriod = child.attribute("sampling_period");
    strFanSpeed = child.attribute("fan_speed");
    strFanThreshold = child.attribute("fan_threshold");
    strCurrentGain = child.attribute("current_gain");
    strHeaterCurrent = child.attribute("heater_current");
    strHeaterThreshold = child.attribute("heater_threshold");
    strCurrentDeviation = child.attribute("current_deviation");
    strCurrentMin230_Serial = child.attribute("current_min_230_serial");
    strCurrentMax230_Serial = child.attribute("current_max_230_serial");
    strCurrentMin100_Serial = child.attribute("current_min_100_serial");
    strCurrentMax100_Serial = child.attribute("current_max_100_serial");
    strCurrentMin100_Parallel = child.attribute("current_min_100_parallel");
    strCurrentMax100_Parallel = child.attribute("current_max_100_parallel");

    pCANObjFctTempCtrl = new CANFctModuleTempCtrl();
    pCANObjFctTempCtrl->bTempTolerance = strTempTolerance.toShort(&ok, 10);
    pCANObjFctTempCtrl->sSamplingPeriod = strSamplingPeriod.toShort(&ok, 10);
    pCANObjFctTempCtrl->sFanSpeed = strFanSpeed.toShort(&ok, 10);
    pCANObjFctTempCtrl->sFanThreshold = strFanThreshold.toShort(&ok, 10);
    pCANObjFctTempCtrl->sCurrentGain = strCurrentGain.toShort(&ok, 10);
    pCANObjFctTempCtrl->sHeaterCurrent = strHeaterCurrent.toShort(&ok, 10);
    pCANObjFctTempCtrl->sHeaterThreshold = strHeaterThreshold.toShort(&ok, 10);
    pCANObjFctTempCtrl->sCurrentDeviation = strCurrentDeviation.toShort(&ok, 10);
    pCANObjFctTempCtrl->sCurrentMin230_Serial = strCurrentMin230_Serial.toShort(&ok, 10);
    pCANObjFctTempCtrl->sCurrentMax230_Serial = strCurrentMax230_Serial.toShort(&ok, 10);
    pCANObjFctTempCtrl->sCurrentMin100_Serial = strCurrentMin100_Serial.toShort(&ok, 10);
    pCANObjFctTempCtrl->sCurrentMax100_Serial = strCurrentMax100_Serial.toShort(&ok, 10);
    pCANObjFctTempCtrl->sCurrentMin100_Parallel = strCurrentMin100_Parallel.toShort(&ok, 10);
    pCANObjFctTempCtrl->sCurrentMax100_Parallel = strCurrentMax100_Parallel.toShort(&ok, 10);

    //############################
    // PID controller parameters
    CANFctPidController PidController;

    childPidControllers = element.firstChildElement("pid_controllers");
    if(childPidControllers.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        delete pCANObjFctTempCtrl;
        pCANObjFctTempCtrl = 0;

        return pCANObjFctTempCtrl;
    }

    // loop thru the pid controllers, create a data instace for storage and place it into the list
    childPidController = childPidControllers.firstChildElement("pid_controller");
    while (!childPidController.isNull())
    {
        strMaxTemperature = childPidController.attribute("max_temperature");
        strControllerGain = childPidController.attribute("controller_gain");
        strResetTime = childPidController.attribute("reset_time");
        strDerivativeTime = childPidController.attribute("derivative_time");

        PidController.sMaxTemperature = strMaxTemperature.toShort(&ok, 10);
        PidController.sControllerGain = strControllerGain.toShort(&ok, 10);
        PidController.sResetTime = strResetTime.toShort(&ok, 10);
        PidController.sDerivativeTime = strDerivativeTime.toShort(&ok, 10);

        pCANObjFctTempCtrl->listPidControllers.append(PidController);

        childPidController = childPidController.nextSiblingElement("pid_controller");
    }

    return pCANObjFctTempCtrl;
}
#ifdef PRE_ALFA_TEST
/****************************************************************************/
/*!
 *  \brief  Parse pressure control element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModulePressureCtrl* HardwareConfiguration::ParsePressureCtrl(const QDomElement &element)
{
    CANFctModulePressureCtrl* pCANObjFctPressureCtrl = 0;
    QDomElement child;
    QDomElement childPidControllers;
    QDomElement childPidController;
    QDomElement childPwmController;
    QString strPressureTolerance, strSamplingPeriod, strFanCurrent, strFanCurrentGain, strFanThreshold,
            strCurrentGain, strPumpCurrent, strPumpThreshold;
    QString strMaxPressure, strMinPressure, strControllerGain, strResetTime, strDerivativeTime;
    QString strMaxActuatingValue, strMinActuatingValue, strMaxPwmDuty, strMinPwmDuty;
    bool ok;

    child = element.firstChildElement("configuration");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjFctPressureCtrl;
    }

    strPressureTolerance = child.attribute("pressure_tolerance");
    strSamplingPeriod = child.attribute("sampling_period");
    //strFanSpeed = child.attribute("fan_speed");
    strFanThreshold = child.attribute("fan_threshold");
    strFanCurrent = child.attribute("fan_current");
    strFanCurrentGain = child.attribute("fan_current_gain");
    strCurrentGain = child.attribute("current_gain");
    strPumpCurrent = child.attribute("pump_current");
    strPumpThreshold = child.attribute("pump_threshold");

    pCANObjFctPressureCtrl = new CANFctModulePressureCtrl();
    pCANObjFctPressureCtrl->bPressureTolerance = strPressureTolerance.toShort(&ok, 10);
    pCANObjFctPressureCtrl->sSamplingPeriod = strSamplingPeriod.toShort(&ok, 10);
    //pCANObjFctPressureCtrl->sFanSpeed = strFanSpeed.toShort(&ok, 10);
    pCANObjFctPressureCtrl->sFanThreshold = strFanThreshold.toShort(&ok, 10);
    pCANObjFctPressureCtrl->sCurrentGain = strCurrentGain.toShort(&ok, 10);
    pCANObjFctPressureCtrl->sPumpCurrent = strPumpCurrent.toShort(&ok, 10);
    pCANObjFctPressureCtrl->sPumpThreshold = strPumpThreshold.toShort(&ok, 10);
    pCANObjFctPressureCtrl->sFanCurrent = strFanCurrent.toShort(&ok, 10);
    pCANObjFctPressureCtrl->sFanCurrentGain = strFanCurrentGain.toShort(&ok, 10);

    //############################
    // PID controller parameters
    CANPressureFctPidController PidController;

    childPidControllers = element.firstChildElement("pid_controllers");
    if(childPidControllers.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        delete pCANObjFctPressureCtrl;
        pCANObjFctPressureCtrl = 0;

        return pCANObjFctPressureCtrl;
    }

    // loop thru the pid controllers, create a data instace for storage and place it into the list
    childPidController = childPidControllers.firstChildElement("pid_controller");
    while (!childPidController.isNull())
    {
        strMaxPressure = childPidController.attribute("max_pressure");
        strMinPressure = childPidController.attribute("min_pressure");
        strControllerGain = childPidController.attribute("controller_gain");
        strResetTime = childPidController.attribute("reset_time");
        strDerivativeTime = childPidController.attribute("derivative_time");

        PidController.sMaxPressure = strMaxPressure.toShort(&ok, 10);
        PidController.sMinPressure = strMinPressure.toShort(&ok, 10);
        PidController.sControllerGain = strControllerGain.toShort(&ok, 10);
        PidController.sResetTime = strResetTime.toShort(&ok, 10);
        PidController.sDerivativeTime = strDerivativeTime.toInt(&ok, 10);

        pCANObjFctPressureCtrl->listPidControllers.append(PidController);

        childPidController = childPidController.nextSiblingElement("pid_controller");
    }

    childPwmController = element.firstChildElement("pwm_controller");
    if(childPwmController.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        delete pCANObjFctPressureCtrl;
        pCANObjFctPressureCtrl = 0;

        return pCANObjFctPressureCtrl;

    }
    strMaxActuatingValue = childPwmController.attribute("max_actuating_value");
    strMinActuatingValue = childPwmController.attribute("min_actuating_value");
    strMaxPwmDuty = childPwmController.attribute("max_pwm_duty");
    strMinPwmDuty = childPwmController.attribute("min_pwm_duty");

    pCANObjFctPressureCtrl->pwmController.sMaxActuatingValue = strMaxActuatingValue.toShort(&ok, 10);
    pCANObjFctPressureCtrl->pwmController.sMinActuatingValue = strMinActuatingValue.toShort(&ok, 10);
    pCANObjFctPressureCtrl->pwmController.sMaxPwmDuty = strMaxPwmDuty.toShort(&ok, 10);
    pCANObjFctPressureCtrl->pwmController.sMinPwmDuty = strMinPwmDuty.toShort(&ok, 10);

    return pCANObjFctPressureCtrl;
}

#endif
/****************************************************************************/
/*!
 *  \brief  Parse joystick element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleJoystick* HardwareConfiguration::ParseJoystick(const QDomElement &element)
{
    CANFctModuleJoystick* pCANObjFctJoystick = 0;
    QDomElement child;
    QString strType, strProtocol;

    child = element.firstChildElement("configuration");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjFctJoystick;
    }

    strType = child.attribute("type");

    pCANObjFctJoystick = new CANFctModuleJoystick();
    //pCANObjFctJoystick->m_bType = strType.toShort(&ok, 10);
    pCANObjFctJoystick->m_bCommModeThresHold = 0;
    pCANObjFctJoystick->m_sSampleRate = 0;
    pCANObjFctJoystick->m_sUpperThreshold = 0;
    pCANObjFctJoystick->m_sLowerThreshold = 0;

    return pCANObjFctJoystick;
}

/****************************************************************************/
/*!
 *  \brief  Parse UART element from xml
 *
 *  \iparam element = Contains the function module's desription
 *
 *  \return The function module's configuration
 */
/****************************************************************************/
CANFctModuleUART* HardwareConfiguration::ParseUART(const QDomElement &element)
{
    CANFctModuleUART* pCANObjFctUART = 0;
    QDomElement child;
    QString strType, strProtocol;

    child = element.firstChildElement("configuration");
    if(child.isNull())
    {
        m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return pCANObjFctUART;
    }

    pCANObjFctUART = new CANFctModuleUART();

    return pCANObjFctUART;
}

/****************************************************************************/
/*!
 *  \brief  Finds an available device configuration using the order number
 *
 *  \iparam xpBaseDeviceConfig = Device configuration containing order number
 *
 *  \return Device configuration
 */
/****************************************************************************/
BaseDeviceConfiguration* HardwareConfiguration::GetDevice(const BaseDeviceConfiguration* xpBaseDeviceConfig)
{
    BaseDeviceConfiguration* pBaseDeviceConfig = 0;
    BaseDeviceConfiguration* pBaseDeviceConfigRet = 0;

    QListIterator<BaseDeviceConfiguration*> iter(m_DeviceCfgList);

    FILE_LOG_L(laINIT, llDEBUG1) << " HardwareConfiguration, " << m_DeviceCfgList.count() << " devices.";

    while (iter.hasNext())
    {
        pBaseDeviceConfig = (BaseDeviceConfiguration*) iter.next();

        FILE_LOG_L(laINIT, llDEBUG1) << " HardwareConfiguration, GetDevice";

        if(xpBaseDeviceConfig != 0)  // search the next device, identified by order number
        {
            FILE_LOG_L(laINIT, llDEBUG1) << " HardwareConfiguration, " << (int) xpBaseDeviceConfig->m_OrderNr << " == " << (int) pBaseDeviceConfig->m_OrderNr << " ?";

            if(pBaseDeviceConfig->m_OrderNr == (xpBaseDeviceConfig->m_OrderNr + 1))
            {
                pBaseDeviceConfigRet = pBaseDeviceConfig;
                break;
            }
        }
        else  // search the first node, with order number '0'
        {
            if(0 == pBaseDeviceConfig->m_OrderNr)
            {
                pBaseDeviceConfigRet = pBaseDeviceConfig;
                break;
            }
        }
    }

    FILE_LOG_L(laINIT, llDEBUG1) << " HardwareConfiguration, GetDevice returns " << pBaseDeviceConfigRet;

    return pBaseDeviceConfigRet;
}

/****************************************************************************/
/*!
 *  \brief  Finds an available CAN node configuration using the order number
 *
 *  \iparam xpCANObjectNode = CAN node containing order number
 *
 *  \return CAN node configuration
 */
/****************************************************************************/
CModuleConfig* HardwareConfiguration::GetCANNode(const CModuleConfig* xpCANObjectNode)
{
    CModuleConfig* pCANObjectNode = 0;
    CModuleConfig* pCANObjectNodeRet = 0;

    QMap<QString, CModuleConfig*>::const_iterator iter;

    iter = m_CANObjectCfgList.constBegin();
    while (iter != m_CANObjectCfgList.constEnd())
    {
        pCANObjectNode = iter.value();
        // search for nodes, no function modules
        if(CModuleConfig::CAN_OBJ_TYPE_NODE == pCANObjectNode->m_ObjectType)
        {
            if(0 != xpCANObjectNode)  // search the next node, identified by order number
            {
                if(pCANObjectNode->m_sOrderNr == (xpCANObjectNode->m_sOrderNr + 1))
                {
                    pCANObjectNodeRet = pCANObjectNode;
                    break;
                }
            }
            else  // search the first node, with order number '0'
            {
                if(0 == pCANObjectNode->m_sOrderNr)
                {
                    pCANObjectNodeRet = pCANObjectNode;
                    break;
                }
            }
        }

        ++iter;
    }

    return pCANObjectNodeRet;
}

/****************************************************************************/
/*!
 *  \brief  Finds a function module configuration using the order number
 *
 *  \iparam xpCANObjectNodeParent = CAN node containing order number
 *  \iparam xpCANObjectFct = Function module containing order number
 *
 *  \return Function module configuration
 */
/****************************************************************************/
CModuleConfig* HardwareConfiguration::GetCANFctModule(const CModuleConfig* xpCANObjectNodeParent,
                                                      const CModuleConfig* xpCANObjectFct)
{
    CModuleConfig* pCANObjectFct = 0;
    CModuleConfig* pCANObjectFctRet = 0;

    QMap<QString, CModuleConfig*>::const_iterator iter;

    iter = m_CANObjectCfgList.constBegin();
    while (iter != m_CANObjectCfgList.constEnd())
    {
        pCANObjectFct = iter.value();
        // search for function modules, exclude nodes
        if((pCANObjectFct->m_ObjectType != CModuleConfig::CAN_OBJ_TYPE_NODE) &&
                (pCANObjectFct->m_ObjectType != CModuleConfig::CAN_OBJ_TYPE_UNDEF))
        {
            if(0 != xpCANObjectFct)  // search the next node, identified by order number
            {
                if((pCANObjectFct->pParent == xpCANObjectNodeParent) &&
                   (pCANObjectFct->m_sOrderNr == (xpCANObjectFct->m_sOrderNr + 1)))
                {
                    pCANObjectFctRet = pCANObjectFct;
                    break;
                }
            }
            else  // search the first node, with order number '0'
            {
                if((xpCANObjectNodeParent == pCANObjectFct->pParent) &&
                   (0 == pCANObjectFct->m_sOrderNr))
                {
                    pCANObjectFctRet = pCANObjectFct;
                    break;
                }
            }
        }
        ++iter;
    }

    return pCANObjectFctRet;
}

/****************************************************************************/
/*!
 *  \brief  Returns the object type from a function modul key string
 *
 *  \iparam strCANObjectType = Function module key string
 *
 *  \return CAN object type
 */
/****************************************************************************/
CModuleConfig::CANObjectType_t HardwareConfiguration::GetObjectTypeFromString(const QString strCANObjectType)
{
    CModuleConfig::CANObjectType_t eObjectType;

    if(strCANObjectType == "steppermotor")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_STEPPERMOTOR;
    }
    else if(strCANObjectType == "digital_in")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_DIGITAL_IN_PORT;
    }
    else if(strCANObjectType == "digital_out")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_DIGITAL_OUT_PORT;
    }
    else if(strCANObjectType == "analog_in")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_ANALOG_IN_PORT;
    }
    else if(strCANObjectType == "analog_out")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_ANALOG_OUT_PORT;
    }
    else if(strCANObjectType == "rfid11785")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_RFID11785;
    }
    else if(strCANObjectType == "rfid15693")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_RFID15693;
    }
    else if(strCANObjectType == "temperature_control")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_TEMPERATURE_CTL;
    }
    else if(strCANObjectType == "joystick")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_JOYSTICK;
    }
    else if(strCANObjectType == "uart")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_UART;
    }
#ifdef PRE_ALFA_TEST
    else if(strCANObjectType == "pressure_control")
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_PRESSURE_CTL;
    }
#endif
    else
    {
        eObjectType = CModuleConfig::CAN_OBJ_TYPE_UNDEF;
    }

    return eObjectType;

}

/****************************************************************************/
/*!
 *  \brief  Returns the function modul key string from an object type
 *
 *  \iparam eObjectType = The object type of a CAN object
 *
 *  \return Function module key string
 */
/****************************************************************************/
QString HardwareConfiguration::GetLabelFromObjectType(const CModuleConfig::CANObjectType_t eObjectType)
{
    QString strCANObjectType;

    switch (eObjectType)
    {
        case CModuleConfig::CAN_OBJ_TYPE_DIGITAL_IN_PORT:
            strCANObjectType = "digital_in";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_DIGITAL_OUT_PORT:
            strCANObjectType = "digital_out";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_ANALOG_IN_PORT:
            strCANObjectType = "analog_in";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_ANALOG_OUT_PORT:
            strCANObjectType = "analog_out";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_STEPPERMOTOR:
            strCANObjectType = "steppermotor";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_RFID11785:
            strCANObjectType = "rfid11785";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_RFID15693:
            strCANObjectType = "rfid15693";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_UART:
            strCANObjectType = "uart";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_JOYSTICK:
            strCANObjectType = "joystick";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_TEMPERATURE_CTL:
            strCANObjectType = "temperature_control";
            break;
        case CModuleConfig::CAN_OBJ_TYPE_PRESSURE_CTL:
            strCANObjectType = "pressure_control";
            break;
        default:
            strCANObjectType = "undef";
            FILE_LOG_L(laINIT, llERROR) << "ERROR: unknown object type";
            break;
    }

    return strCANObjectType;

}

/****************************************************************************/
/*!
 *  \brief  Returns the Device instance ID from a quint32 value
 *
 *  \iparam DeviceValue = Device instance identifier as quint32
 *
 *  \return Device instance ID as DevInstanceID_t
 */
/****************************************************************************/
DevInstanceID_t HardwareConfiguration::GetDeviceIDFromValue(quint32 DeviceValue)
{
    DevInstanceID_t DevInstanceID = DEVICE_INSTANCE_ID_UNDEFINED;

    switch (DeviceValue)
    {
        case 0x000080C0:
            DevInstanceID = DEVICE_INSTANCE_ID_ROTARY_VALVE;
            break;
        case 0x000080C1:
            DevInstanceID = DEVICE_INSTANCE_ID_AIR_LIQUID;
        break;
        case 0x000080C2:
            DevInstanceID = DEVICE_INSTANCE_ID_OVEN;   //!< Oven
        break;
        case 0x000080C3:
            DevInstanceID = DEVICE_INSTANCE_ID_RETORT; //!< Retort
        break;
        case 0x000080C4:
            DevInstanceID = DEVICE_INSTANCE_ID_MAIN_CONTROL; //!< Main contorl
        break;
        default:
            DevInstanceID = DEVICE_INSTANCE_ID_UNDEFINED;
            break;
    }

    return DevInstanceID;
}

/*****************************************************************************/
/*!
 *  \brief  Convert the rotation string to rotation type
 *
 *  \iparam strRotDir = Rotation string
 *
 *  \return Rotation type
 */
/*****************************************************************************/
CANFctModuleStepperMotor::RotationDir_t HardwareConfiguration::GetRotationFromString(const QString &strRotDir)
{
    if("cw" == strRotDir) {
        return CANFctModuleStepperMotor::ROTATION_DIR_CW;
    }
    else if("ccw" == strRotDir) {
        return CANFctModuleStepperMotor::ROTATION_DIR_CCW;
    }
    else {
        return CANFctModuleStepperMotor::ROTATION_DIR_UNDEF;
    }
}

} //namespace
