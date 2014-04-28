/******************************************************************/
/*! \file ConfigurationService.cpp
 *
 *  \brief Implementation file for class CConfigurationService.
 *
 *   Version: $ 0.1
 *   Date:    $ 08.07.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This file contains the configuration service class implementation
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

#include <QTextStream>
#include <QDomNode>
#include <QDomElement>
#include <QtDebug>

#include "DeviceControl/Include/DeviceProcessing/ConfigurationService.h"
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/RotaryValveDevice.h"
#include "DeviceControl/Include/Devices/AirLiquidDevice.h"
#include "DeviceControl/Include/Devices/RetortDevice.h"
#include "DeviceControl/Include/Devices/OvenDevice.h"
#include "DeviceControl/Include/Devices/PeripheryDevice.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/Rfid15693.h"
#include "DeviceControl/Include/SlaveModules/Uart.h"
#include "DeviceControl/Include/SlaveModules/Joystick.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/PressureControl.h"
#include "DeviceControl/Include/Configuration/HardwareConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"
#include "Global/Include/Utils.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CConfigurationService
 *
 *  \iparam pDeviceProcessing = Pointer to DeviceProcessing
 *  \iparam pCANCommunicator = Pointer to communication interface
 *  \iparam pObjectTree = Pointer to CAN-object container
 */
/****************************************************************************/
CConfigurationService::CConfigurationService(DeviceProcessing* pDeviceProcessing,
                                             CANCommunicator* pCANCommunicator) :
    m_pDeviceProcessing(pDeviceProcessing),
    m_pCANCommunicator(pCANCommunicator),
    m_lastErrorHdlInfo(DCL_ERR_FCT_CALL_SUCCESS),
    m_lastErrorGroup(0),
    m_lastErrorCode(0),
    m_lastErrorData(0),
    m_stateTimespan(0),
    m_ConfigurationComplete(false)
{
    m_MainState = CS_MAIN_STATE_INIT;
    m_ErrSubState = CS_SUBSTATE_ERR_UNDEF;
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CConfigurationService
 */
/****************************************************************************/
CConfigurationService::~CConfigurationService()
{
    m_pDeviceProcessing = 0;
    m_pCANCommunicator = 0;
}

/****************************************************************************/
/*!
 *  \brief  Throws an error signal
 *
 *  \iparam usNodeID = Node ID
 *  \iparam ulModulInfo = Module ID
 *  \iparam usErrorGroup = Error group
 *  \iparam usErrorID = Error ID
 *  \iparam sErrorData = Error data
 */
/****************************************************************************/
void CConfigurationService::ThrowErrorSignal(quint16 usNodeID, quint32 ulModulInfo, quint16 usErrorGroup, quint16 usErrorID, qint16 sErrorData)
{
    Q_UNUSED(usNodeID);
    Q_UNUSED(ulModulInfo);
    Q_UNUSED(usErrorGroup);
    Q_UNUSED(usErrorID);
    Q_UNUSED(sErrorData);
}

/****************************************************************************/
/*!
 *  \brief  Handles the tasks of the configuration service
 */
/****************************************************************************/
void CConfigurationService::HandleTasks()
{
    if(m_MainState == CS_MAIN_STATE_INIT)
    {
        m_MainState = CS_MAIN_STATE_CREATE_CANOBJECTS;
    }
    else if(m_MainState == CS_MAIN_STATE_CREATE_CANOBJECTS)
    {
        ReturnCode_t RetVal;

        //create all CANNodes, function modules and devices
        RetVal = CreateDeviceComponents();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_pDeviceProcessing->LogObjectTree(0);
            m_MainState = CS_MAIN_STATE_CONFIG_WAIT_CANOBJECTS;
        }
        else
        {
            FILE_LOG_L(laINIT, llINFO) << " CreateDeviceComponents returns error: " << (int) RetVal;
            m_lastErrorHdlInfo = RetVal;
            m_MainState = CS_MAIN_STATE_ERROR;
        }
    }
    else if(m_MainState == CS_MAIN_STATE_CONFIG_WAIT_CANOBJECTS)
    {
        // While this state is active, the CAN nodes configure its function modules.
        // when finished, they switch to idle state
        // Check if all CAN nodes already have switched to IDLE state
        ReturnCode_t RetVal = IsCANNodesStateIdle();
        if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
            m_MainState = CS_MAIN_STATE_CREATE_DEVICES;
            m_ConfigurationComplete = true;
        }
        else if (RetVal == DCL_ERR_TIMEOUT) {
            m_MainState = CS_MAIN_STATE_CREATE_DEVICES;
            m_ConfigurationComplete = false;
        }
        else if (RetVal == DCL_ERR_INTERNAL_ERR) {
            // At least one node is in error mode
            FILE_LOG_L(laINIT, llINFO) << " Set ConfigurationService main state to ERROR";
            m_lastErrorHdlInfo = RetVal;
            m_MainState = CS_MAIN_STATE_ERROR;
            m_ErrSubState = CS_SUBSTATE_ERR_INIT;
            m_ConfigurationComplete = false;
        }
    }
    else if(m_MainState == CS_MAIN_STATE_CREATE_DEVICES)
    {
        m_MainState = CS_MAIN_STATE_FORWARD_CONFIGURATION;
    }
    else if(m_MainState == CS_MAIN_STATE_FORWARD_CONFIGURATION)
    {
        ReturnCode_t rcHWConfig;

        FILE_LOG_L(laINIT, llINFO) << " CheckCANNodeStates returns OK, goto next state";
        rcHWConfig = CreateDiscoveredHWConfiguration();
        if(rcHWConfig == DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laINIT, llINFO) << " CConfigurationService:  m_MainState = CS_MAIN_STATE_IDLE";
            m_MainState = CS_MAIN_STATE_IDLE;
        }
        else
        {
            FILE_LOG_L(laINIT, llERROR) << " CConfigurationService:  CreateDiscoveredHWConfiguration failed";
            m_MainState = CS_MAIN_STATE_ERROR;
        }
    }
    else if(m_MainState == CS_MAIN_STATE_IDLE)
    {
        ; //in this state the device processing finishes the configuration service
    }
    else if(m_MainState == CS_MAIN_STATE_ERROR)
    {
        if(m_ErrSubState == CS_SUBSTATE_ERR_IDLE)
        {
            ;
        }
        else if(m_ErrSubState == CS_SUBSTATE_ERR_INIT)
        {
            //ReportHWConfigurationState();
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Calls all CAN nodes task handling function
 */
/****************************************************************************/
void CConfigurationService::HandleCANNodesTask()
{
    CBaseModule* pCANNode;

    pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(true);
    while (pCANNode)
    {
        pCANNode->HandleTasks();
        pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(false);
    }
}

/****************************************************************************/
/*!
 *  \brief  Create CANObjects and devices according to configuration
 *
 *      The information for object creation based on the class
 *      HardwareConfiguration, which, on her part, reads the hardware
 *      configuration of the full equipped system from xml file.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CConfigurationService::CreateDeviceComponents()
{
    // Read hardware configuration (CAN-objects and devices)
    // The configuration information is stored inside the HardwareConfiguration class
    HardwareConfiguration Configuration;
    ReturnCode_t RetVal = Configuration.ReadHWSpecification(DeviceProcessing::GetHWConfigFile());

    if (RetVal != DCL_ERR_FCT_CALL_SUCCESS) {
        QString strErrorInfo;
        quint16 usErrorID = 0;
        QDateTime errorTimeStamp;

        Configuration.GetLastError(usErrorID, strErrorInfo);
        errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
        m_pDeviceProcessing->ThrowErrorWithInfo(0, EVENT_GRP_DCL_CONFIGURATION, usErrorID, 0, errorTimeStamp, strErrorInfo);
        FILE_LOG_L(laINIT, llERROR) << "   CConfigurationService::ReadHWSpecification() " << strErrorInfo.toStdString();
    }
    else {
        RetVal = CreateObjectTree(&Configuration);
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
            RetVal = CreateDevices(&Configuration);
        }
    }

    FILE_LOG_L(laINIT, llDEBUG) << "   CConfigurationService::ReadHWSpecification() finished.";
    FILE_LOG_L(laINIT, llDEBUG) << "*********************************************************";

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Create all CAN objects and adds them to the object tree
 *
 *  \iparam pHWConfiguration = Pointer to hardware configuration object
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CConfigurationService::CreateObjectTree(HardwareConfiguration* pHWConfiguration)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    CModuleConfig* pCANObjectConfigNode = 0;
    CModuleConfig* pCANObjectConfigFct = 0;
    CBaseModule* pCANNode = 0;

    FILE_LOG_L(laINIT, llDEBUG) << "**********************************************";
    FILE_LOG_L(laINIT, llDEBUG) << " CConfigurationService: create the object tree";

    //determine the first CANNode
    pCANObjectConfigNode = pHWConfiguration->GetCANNode(0);
    while(pCANObjectConfigNode != 0)
    {
        FILE_LOG_L(laINIT, llDEBUG1) << "++++++++++++++++++++++++++++++++++++++++++++";
        //create the CANNode instance and insert it into the device manager's object tree
        pCANNode = CreateAndGetCANNode(pCANObjectConfigNode->m_sCANNodeType, pCANObjectConfigNode->m_sCANNodeIndex);
        pCANNode->SetCANConfiguration(pCANObjectConfigNode);
        RetVal = pCANNode->Initialize();
        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
        {
            quint16 usErrorID = 0;
            QDateTime errorTimeStamp;
            QString strErrorInfo;

            errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
            m_pDeviceProcessing->ThrowErrorWithInfo(0, EVENT_GRP_DCL_CONFIGURATION, usErrorID, 0, errorTimeStamp, strErrorInfo);
            FILE_LOG_L(laINIT, llERROR) << "   CConfigurationService::CANNode-Initialize() failed " << pCANObjectConfigNode->m_sCANNodeIndex;
            break;
        }

        m_pDeviceProcessing->InsertCANNodeToObjectTree(pCANNode);
        FILE_LOG_L(laINIT, llDEBUG1) << "  ConfigService: created node " << pCANNode->GetName().toStdString();

        //determine the first function module assigned to the CANNode
        pCANObjectConfigFct = pHWConfiguration->GetCANFctModule(pCANObjectConfigNode, 0);
        while(0 != pCANObjectConfigFct)
        {
            //create the accordingly function module instance and insert it into the
            //CAN node's function module list
            switch(pCANObjectConfigFct->m_ObjectType)
            {
                case CModuleConfig::CAN_OBJ_TYPE_DIGITAL_IN_PORT:
                {
                    CreateAndAddFunctionModule<CDigitalInput>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_DIGITAL_OUT_PORT:
                {
                    CreateAndAddFunctionModule<CDigitalOutput>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_ANALOG_IN_PORT:
                {
                    CreateAndAddFunctionModule<CAnalogInput>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_ANALOG_OUT_PORT:
                {
                    CreateAndAddFunctionModule<CAnalogOutput>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_STEPPERMOTOR:
                {
                    CreateAndAddFunctionModule<CStepperMotor>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_RFID11785:
                {
                    CreateAndAddFunctionModule<CRfid11785>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_RFID15693:
                {
                    CreateAndAddFunctionModule<CRfid15693>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_TEMPERATURE_CTL:
                {
                    CreateAndAddFunctionModule<CTemperatureControl>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_JOYSTICK:
                {
                    CreateAndAddFunctionModule<CJoystick>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_UART:
                {
                    CreateAndAddFunctionModule<CUart>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                case CModuleConfig::CAN_OBJ_TYPE_PRESSURE_CTL:
                {
                    CreateAndAddFunctionModule<CPressureControl>(pCANNode, pCANObjectConfigFct);
                    break;
                }
                default:
                    //pfui, undefined CAN object type
                    break;
            }
            pCANObjectConfigFct = pHWConfiguration->GetCANFctModule(pCANObjectConfigNode, pCANObjectConfigFct);
        }
        pCANObjectConfigNode = pHWConfiguration->GetCANNode(pCANObjectConfigNode);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Create all devices objects and registrate it
 *
 *  \iparam pHWConfiguration = Pointer to hardware configuration object
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CConfigurationService::CreateDevices(HardwareConfiguration* pHWConfiguration)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    BaseDeviceConfiguration* pBaseDeviceCfg;
    CBaseDevice* pBaseDevice = 0;

    pBaseDeviceCfg = pHWConfiguration->GetDevice(0);
    while(pBaseDeviceCfg != 0)
    {
        ReturnCode_t RetValCANNode = DCL_ERR_FCT_CALL_SUCCESS;

        if(pBaseDeviceCfg->m_Type == "RotaryValveDevice")
        {
            pBaseDevice = CreateAndGetDevice<CRotaryValveDevice>(pBaseDeviceCfg);
        }
        else if(pBaseDeviceCfg->m_Type == "AirLiquidDevice")
        {
            pBaseDevice = CreateAndGetDevice<CAirLiquidDevice>(pBaseDeviceCfg);
        }
        else if(pBaseDeviceCfg->m_Type == "RetortDevice")
        {
            pBaseDevice = CreateAndGetDevice<CRetortDevice>(pBaseDeviceCfg);
        }
        else if(pBaseDeviceCfg->m_Type == "OvenDevice")
        {
            pBaseDevice = CreateAndGetDevice<COvenDevice>(pBaseDeviceCfg);
        }
        else if(pBaseDeviceCfg->m_Type == "PeripheryDevice")
        {
            pBaseDevice = CreateAndGetDevice<CPeripheryDevice>(pBaseDeviceCfg);
        }
        else
        {
            m_MainState = CS_MAIN_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_CONFIGURATION, ERROR_DCL_CONFIG_DEVICE_TYPE_INVALID, (quint16) pBaseDeviceCfg->m_OrderNr);
        }

        if(RetValCANNode != DCL_ERR_FCT_CALL_SUCCESS)
        {
            quint16 usErrorID = 0;
            QDateTime errorTimeStamp;
            QString strErrorInfo;

            errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
            m_pDeviceProcessing->ThrowErrorWithInfo(0, EVENT_GRP_DCL_CONFIGURATION, usErrorID, 0, errorTimeStamp, strErrorInfo);
            //FILE_LOG_L(laINIT, llERROR) << "   CConfigurationService::CANNode-Initialize() failed " << pCANObjectConfigNode->m_sCANNodeIndex;
            break;
        }

        if(pBaseDevice)
        {
            m_pDeviceProcessing->AddDevice(pBaseDevice);
            pBaseDevice->SetTypeKey(pBaseDeviceCfg->m_Type);

            FILE_LOG_L(laINIT, llDEBUG1) << " CConfigurationService - Device " << pBaseDevice->GetType().toStdString() << " created.";
        }

        // get next device from configuration file, if any
        pBaseDeviceCfg = pHWConfiguration->GetDevice(pBaseDeviceCfg);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Create a device object
 *
 *  \iparam pBaseDeviceCfg = Pointer to configuration data container
 *
 *  \return Pointer to the new device instance
 */
/****************************************************************************/
template <class TDevice>
TDevice* CConfigurationService::CreateAndGetDevice(BaseDeviceConfiguration* pBaseDeviceCfg)
{
    TDevice* pDevice;

    pDevice = new TDevice(m_pDeviceProcessing, pBaseDeviceCfg->m_Type);
    pDevice->SetFunctionModuleList(pBaseDeviceCfg->m_DevFctModList);

    return pDevice;
}

/****************************************************************************/
/*!
 *  \brief  Create a device object that requires an index
 *
 *  \iparam pBaseDeviceCfg = Pointer to configuration data container
 *
 *  \return Pointer to the new device instance
 */
/****************************************************************************/
template <class TDevice>
TDevice* CConfigurationService::CreateAndGetIndexedDevice(BaseDeviceConfiguration* pBaseDeviceCfg)
{
    TDevice* pDevice;

    pDevice = new TDevice(m_pDeviceProcessing, pBaseDeviceCfg->m_Type, pBaseDeviceCfg->m_InstanceID);
    pDevice->SetFunctionModuleList(pBaseDeviceCfg->m_DevFctModList);

    return pDevice;
}

/****************************************************************************/
/*!
 *  \brief  Create a CANNode instance
 *
 *  \iparam sCANNodeType = CANNode type
 *  \iparam sCANNodeIndex = CANNode index
 *
 *  \return CANNode instance
 */
/****************************************************************************/
CBaseModule* CConfigurationService::CreateAndGetCANNode(qint16 sCANNodeType, qint16 sCANNodeIndex)
{
    CBaseModule* pCANNode = 0;
    pCANNode = new CBaseModule(m_pDeviceProcessing->GetMessageConfig(), m_pCANCommunicator, sCANNodeType, sCANNodeIndex);

    // Register Signal-Signal forwarding
//    CONNECTSIGNALSLOT(pCANNode, ReportError(quint32, quint16, quint16, quint16, QDateTime),
//                      m_pDeviceProcessing->GetParent(), OnError(quint32, quint16, quint16, quint16, QDateTime));
    CONNECTSIGNALSLOT(pCANNode, ReportError(quint32, quint16, quint16, quint16, QDateTime),
                      m_pDeviceProcessing, OnError(quint32, quint16, quint16, quint16, QDateTime));
    return pCANNode;
}

/****************************************************************************/
/*!
 *  \brief  Create a function module instance
 *
 *  \iparam pCANNode = CANNode instance the function module is assigned to
 *  \iparam pCANObjectConfigFct = Function module configuration data
 *
 *  \return Pointer to function module instance
 */
/****************************************************************************/
template <class TFunctionModule>
void CConfigurationService::CreateAndAddFunctionModule(CBaseModule* pCANNode, CModuleConfig *pCANObjectConfigFct)
{
    ReturnCode_t RetValFctModule;
    TFunctionModule* pFunctionModule = 0;
    pFunctionModule = new TFunctionModule(m_pDeviceProcessing->GetMessageConfig(), m_pCANCommunicator, pCANNode);

    // Register Signal-Signal forwarding
    //CONNECTSIGNALSLOT(pFunctionModule, ReportError(quint32, quint16, quint16, quint16, QDateTime),
    //                  m_pDeviceProcessing->GetParent(), OnError(quint32, quint16, quint16, quint16, QDateTime));
    CONNECTSIGNALSLOT(pFunctionModule, ReportError(quint32, quint16, quint16, quint16, QDateTime),
                      m_pDeviceProcessing, OnError(quint32, quint16, quint16, quint16, QDateTime));

    pFunctionModule->SetCANConfiguration(pCANObjectConfigFct);
    RetValFctModule = pFunctionModule->Initialize();

    if(RetValFctModule == DCL_ERR_FCT_CALL_SUCCESS) {
        FILE_LOG_L(laINIT, llDEBUG1) << "   Function Module: " << pFunctionModule->GetKey().toStdString() <<
                                        //"  Type:  0x" << std::hex << pFunctionModule->m_sCANNodeType <<
                                        //"  Index: 0x" << std::hex << pFunctionModule->m_sCANNodeIndex <<
                                        "  Node:  0x" << std::hex << pCANNode->GetNodeID();
        pCANNode->AddFunctionModule(pFunctionModule);
        FILE_LOG_L(laINIT, llDEBUG1) << "   created fct " << pFunctionModule->GetName().toStdString() << " ModuleID: "  << (int) pFunctionModule->GetChannelNo();
    }
}

/****************************************************************************/
/*!
 *  \brief  Stores the hardware configuration to an xml dom
 *
 *      The hardware configuration of the device, as detected by the
 *      configuration process, is stored to an xml dom.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CConfigurationService::CreateDiscoveredHWConfiguration()
{
    ReturnCode_t result = DCL_ERR_FCT_CALL_SUCCESS;

    CBaseModule* pCANNode;
    CFunctionModule* pFctModuleBase;
    QString strKey, strNodeId;

    QDomDocument DocHwDescr;
    QDomNode ResultNode;
    //pDocHwDescr = new QDomDocument();
    QDomElement root = DocHwDescr.createElement("himalaya_hwconfig");

    ResultNode = DocHwDescr.appendChild(root);
    if(ResultNode.isNull())
    {
        FILE_LOG_L(laCONFIG, llERROR) << " append 'himalaya_hwconfig'' node failed.";
        return  DCL_ERR_FCT_CALL_SUCCESS;
    }

    root.setAttribute("version", "1.0");

    QDomElement slaves = DocHwDescr.createElement("slaves");
    ResultNode = root.appendChild(slaves);
    if(ResultNode.isNull())
    {
        FILE_LOG_L(laCONFIG, llERROR) << " append 'slaves'' node failed.";
        return  DCL_ERR_FCT_CALL_SUCCESS;
    }

    QDomElement slave, fctmodules, fctmodule;

    pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(true);
    while(pCANNode)
    {
        FILE_LOG_L(laCONFIG, llDEBUG1) << "  " <<  pCANNode->GetKey().toStdString() << ": " << (int) pCANNode->GetType() << " " << pCANNode->GetName().toStdString();

        slave = DocHwDescr.createElement("slave");
        ResultNode = slaves.appendChild(slave);
        if(ResultNode.isNull())
        {
            FILE_LOG_L(laCONFIG, llERROR) << " append 'slave' node at " << pCANNode->GetKey().toStdString() << " failed.";
            return  DCL_ERR_FCT_CALL_SUCCESS;
        }

        slave.setAttribute("type", "base");
        slave.setAttribute("key", pCANNode->GetKey());
        slave.setAttribute("name", pCANNode->GetName());

        strNodeId = QString("0x%1").arg(pCANNode->GetNodeID(), 4, 16, QChar('0'));
        FILE_LOG_L(laINIT, llDEBUG1) << " xmldesc: key:" << pCANNode->GetKey().toStdString() << " id:" << strNodeId.toStdString();
        slave.setAttribute("id", strNodeId);

        fctmodules = DocHwDescr.createElement("functionmodules");
        ResultNode = slave.appendChild(fctmodules);
        if(ResultNode.isNull())
        {
            FILE_LOG_L(laCONFIG, llERROR) << " append 'functionmodules' failed.";
            return  DCL_ERR_FCT_CALL_SUCCESS;
        }

        pFctModuleBase = pCANNode->GetFunctionModuleFromList(0);
        while (pFctModuleBase)
        {
            FILE_LOG_L(laCONFIG, llDEBUG1) << "   - " << pFctModuleBase->GetKey().toStdString() << ": " << pFctModuleBase->GetName().toStdString() << " " << (int) pFctModuleBase->GetType();
            fctmodule = DocHwDescr.createElement("functionmodule");
            ResultNode = fctmodules.appendChild(fctmodule);
            if(ResultNode.isNull())
            {
                FILE_LOG_L(laCONFIG, llERROR) << " append 'functionmodule' node at " << pFctModuleBase->GetKey().toStdString() << " failed.";
                return  DCL_ERR_FCT_CALL_SUCCESS;
            }

            fctmodule.setAttribute("type", HardwareConfiguration::GetLabelFromObjectType(pFctModuleBase->GetType()));
            fctmodule.setAttribute("key", pFctModuleBase->GetKey());
            fctmodule.setAttribute("name", pFctModuleBase->GetName());
            fctmodule.setAttribute("channel", QString("%1").arg(pFctModuleBase->GetChannelNo()));
            pFctModuleBase = pCANNode->GetFunctionModuleFromList(pFctModuleBase);
        }
        pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(false);
    }

    m_pDeviceProcessing->SetXML_HWDescription(DocHwDescr);

    // this is for testing and should be removed
    QFile file("text.xml");
    if(file.open(QFile::WriteOnly | QFile::Text | QIODevice::Truncate))
    {
        const int IdentSize = 4;
        QTextStream out(&file);
        DocHwDescr.save(out, IdentSize);
    }

    return result;
}

/****************************************************************************/
/*!
 *  \brief  Checks if all CANNodes from object tree are in idle state
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if all nodes in IDLE state
 *          DCL_ERR_INTERNAL_ERR if at least one node in error state
 *          DCL_ERR_INVALID_STATE if at least one node not in idle state
 *          DCL_ERR_TIMEOUT if not all expected nodes are found
 */
/****************************************************************************/
ReturnCode_t CConfigurationService::IsCANNodesStateIdle()
{
    static unsigned int counter = 0;
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    CBaseModule* pCANNode;

    pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(true);
    while (pCANNode) {
        if (pCANNode->GetMainState() == CBaseModule::CN_MAIN_STATE_ERROR) {
            SetErrorParameter(EVENT_GRP_DCL_CONFIGURATION, ERROR_DCL_CONFIG_CAN_NODE_ERROR,
                              (quint16) pCANNode->GetModuleHandle());
            FILE_LOG_L(laINIT, llINFO) << " CheckCANNodeStates found at least one node in error state: " <<
                                          pCANNode->GetKey().toStdString();
            retCode = DCL_ERR_INTERNAL_ERR;
            break;
        }
        else if (pCANNode->GetMainState() != CBaseModule::CN_MAIN_STATE_IDLE &&
                 pCANNode->GetMainState() != CBaseModule::CN_MAIN_STATE_UPDATE) {
            //there is at least one node not ready, set this node to the exchange variable
            retCode = DCL_ERR_INVALID_STATE;
            break;
        }
        else {

        }
        pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(false);
    }

    counter++;
    if (counter > 1000) {
        counter = 0;
        pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(true);
        while(pCANNode) {
            if (pCANNode->GetMainState() != CBaseModule::CN_MAIN_STATE_IDLE &&
                            pCANNode->GetMainState() != CBaseModule::CN_MAIN_STATE_UPDATE ) {
                SetErrorParameter(EVENT_GRP_DCL_CONFIGURATION, ERROR_DCL_CONFIG_CAN_NODE_IDLE_TIMEOUT,
                                  (quint16) pCANNode->GetModuleHandle());
                FILE_LOG_L(laCONFIG, llWARNING) << pCANNode->GetKey().toStdString() << " not idle, state: " <<
                                                   (int) pCANNode->GetMainState();
                pCANNode->SetStateInactive();
            }
            pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(false);
        }
        retCode = DCL_ERR_TIMEOUT;
    }

    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Set the error parameter of the class, and error time to current time
 *
 *  \iparam errorGroup = Error group
 *  \iparam errorCode = Error code
 *  \iparam errorData = Error data
 */
/****************************************************************************/
void CConfigurationService::SetErrorParameter(quint16 errorGroup, quint16 errorCode, quint16 errorData)
{
    m_lastErrorTime  = Global::AdjustedTime::Instance().GetCurrentDateTime();
    m_lastErrorGroup = errorGroup;
    m_lastErrorCode  = errorCode;
    m_lastErrorData  = errorData;
}

/****************************************************************************/
/*!
 *  \brief  Returns if the expected hardware configuration was fully detected
 *
 *  \return Complete hardware detected (true) or not (false)
 */
/****************************************************************************/
bool CConfigurationService::ConfigurationComplete()
{
    if(m_MainState != CS_MAIN_STATE_IDLE) {
        return false;
    }
    return m_ConfigurationComplete;
}

} //namespace
