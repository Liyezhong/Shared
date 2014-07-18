/******************************************************************/
/*! \file ConfigurationService.h
 *
 *  \brief Definition file for class CConfigurationService.
 *
 *   Version: $ 0.1
 *   Date:    $ 08.07.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CConfigurationService
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

#ifndef DEVICECONTROL_CONFIGURATIONSERVICE_H
#define DEVICECONTROL_CONFIGURATIONSERVICE_H

#include "Global/Include/MonotonicTime.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceLifeCycleRecord.h"

#include <QHash>

class QDomElement;

namespace DeviceControl
{

class HardwareConfiguration;
class CCANMsgCommunicationBase;
class DeviceProcessing;
class CDrawer;
class CBaseModule;
class CANCommunicator;
class CDigitalInput;
class CDigitalOutput;
class CAnalogInput;
class CAnalogOutput;
class CStepperMotor;
class CJoystick;
class CRfid11785;
class CRfid15693;
class CUart;
class CTemperatureControl;
class CPressureControl;
/****************************************************************************/
/*!
 *  \brief This class implements the functionality to configure the complete
 *         object tree of the device control layer
 */
/****************************************************************************/
class CConfigurationService
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CConfigurationService
     *
     *  \param pDeviceProcessing = DeviceProcessing type parameter
     *  \param pCANCommunicator =  CANCommunicator type parameter
     *
     *  \return from CConfigurationService
     */
    /****************************************************************************/
    CConfigurationService(DeviceProcessing* pDeviceProcessing, CANCommunicator* pCANCommunicator);
    virtual ~CConfigurationService();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleTasks
     *
     *  \return from HandleTasks
     */
    /****************************************************************************/
    void HandleTasks();

    /****************************************************************************/
    /*!
     *  \brief  Restarts the configuration service
     */
    /****************************************************************************/
    void Restart() { m_MainState = CS_MAIN_STATE_CONFIG_WAIT_CANOBJECTS; }

    /// Configuration service main state definitions
    typedef enum  {
        CS_MAIN_STATE_UNDEF                  = 0x00,    ///< undefined
        CS_MAIN_STATE_INIT                   = 0x01,    ///< initialisation state
        CS_MAIN_STATE_CREATE_CANOBJECTS      = 0x02,    ///< create CAN objects
        CS_MAIN_STATE_CONFIG_WAIT_CANOBJECTS = 0x03,    ///< call all CAN objects task handling function and wait for them to finish their configuration
        CS_MAIN_STATE_CREATE_DEVICES         = 0x04,    ///< create the devices
        CS_MAIN_STATE_FORWARD_CONFIGURATION  = 0x05,    ///< forward the detected hardware configuration to other software layers
        CS_MAIN_STATE_IDLE                   = 0x06,    ///< idle state, all configuration stuff has been done
        CS_MAIN_STATE_ERROR                  = 0x07     ///< error state
    } ConfigServiceMainState_t;

    /****************************************************************************/
    /*!
     *  \brief  Returns the main state of the configuration service
     *
     *  \return Main state
     */
    /****************************************************************************/
    ConfigServiceMainState_t GetState() { return m_MainState; }

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ThrowErrorSignal
     *
     *  \param usNodeID = quint16 type parameter
     *  \param ulModulInfo =  quint32 type parameter
     *  \param usErrorGroup =  quint16 type parameter
     *  \param usErrorID =  quint16 type parameter
     *  \param sErrorData =  qint16 type parameter
     *
     *  \return from ThrowErrorSignal
     */
    /****************************************************************************/
    void ThrowErrorSignal(quint16 usNodeID, quint32 ulModulInfo, quint16 usErrorGroup, quint16 usErrorID, qint16 sErrorData);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ConfigurationComplete
     *
     *  \return from ConfigurationComplete
     */
    /****************************************************************************/
    bool ConfigurationComplete();
    void WriteDeviceLifeCycle();

private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateDeviceComponents
     *
     *  \return from CreateDeviceComponents
     */
    /****************************************************************************/
    ReturnCode_t CreateDeviceComponents();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateObjectTree
     *
     *  \param pHWConfiguration = HardwareConfiguration type parameter
     *
     *  \return from CreateObjectTree
     */
    /****************************************************************************/
    ReturnCode_t CreateObjectTree(HardwareConfiguration* pHWConfiguration);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateDevices
     *
     *  \param pHWConfiguration = HardwareConfiguration type parameter
     *
     *  \return from CreateDevices
     */
    /****************************************************************************/
    ReturnCode_t CreateDevices(HardwareConfiguration* pHWConfiguration);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCANNodesTask
     *
     *  \return from HandleCANNodesTask
     */
    /****************************************************************************/
    void HandleCANNodesTask();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsCANNodesStateIdle
     *
     *  \return from IsCANNodesStateIdle
     */
    /****************************************************************************/
    ReturnCode_t IsCANNodesStateIdle();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateDiscoveredHWConfiguration
     *
     *  \return from CreateDiscoveredHWConfiguration
     */
    /****************************************************************************/
    ReturnCode_t CreateDiscoveredHWConfiguration();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetErrorParameter
     *
     *  \param errorGroup = quint16 type parameter
     *  \param errorCode =  quint16 type parameter
     *  \param errorData =  quint16 type parameter
     *
     *  \return from SetErrorParameter
     */
    /****************************************************************************/
    void SetErrorParameter(quint16 errorGroup, quint16 errorCode, quint16 errorData);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateAndGetCANNode
     *
     *  \param sCANNodeType = qint16 type parameter
     *  \param sCANNodeIndex =  qint16 type parameter
     *
     *  \return from CreateAndGetCANNode
     */
    /****************************************************************************/
    CBaseModule* CreateAndGetCANNode(qint16 sCANNodeType, qint16 sCANNodeIndex, bool isVirtual);
    template <class TFunctionModule>
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateAndAddFunctionModule
     *
     *  \param pCANNode = CBaseModule type parameter
     *  \param pCANObjectConfigFct =  CModuleConfig type parameter
     *
     *  \return from CreateAndAddFunctionModule
     */
    /****************************************************************************/
    void CreateAndAddFunctionModule(CBaseModule* pCANNode, CModuleConfig* pCANObjectConfigFct);

    template <class TDevice>
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateAndGetDevice
     *
     *  \param pBaseDeviceCfg = BaseDeviceConfiguration type parameter
     *
     *  \return from CreateAndGetDevice
     */
    /****************************************************************************/
    TDevice* CreateAndGetDevice(BaseDeviceConfiguration* pBaseDeviceCfg);
    template <class TDevice>
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateAndGetIndexedDevice
     *
     *  \param pBaseDeviceCfg = BaseDeviceConfiguration type parameter
     *
     *  \return from CreateAndGetIndexedDevice
     */
    /****************************************************************************/
    TDevice* CreateAndGetIndexedDevice(BaseDeviceConfiguration* pBaseDeviceCfg);

    /// Configuration error sub state definitions
    typedef enum  {
        CS_SUBSTATE_ERR_UNDEF    = 0x00,    ///< undefined
        CS_SUBSTATE_ERR_INIT     = 0x01,    ///< init, the error state was set but not logged
        CS_SUBSTATE_ERR_IDLE     = 0x02     ///< idle, all error handling stuff was done, now just wait
    } ConfigServiceErrSubState_t;

    ConfigServiceMainState_t   m_MainState;     ///< main state
    ConfigServiceErrSubState_t m_ErrSubState;   ///< error sub state

    DeviceProcessing* m_pDeviceProcessing;  ///< pointer to the device processing instance
    CANCommunicator*  m_pCANCommunicator;   ///< pointer to the CAN communicator

    ReturnCode_t m_lastErrorHdlInfo;    ///< return value after a failed function call
    quint16      m_lastErrorGroup;      ///< error group of the error causing functionality
    quint16      m_lastErrorCode;       ///< error code of the error causing functionality
    quint16      m_lastErrorData;       ///< additional error data, filled by the error causing functionality
    QDateTime    m_lastErrorTime;       ///< time of error detection

    Global::MonotonicTime m_stateTimer; ///< timer for timeout observation
    qint16 m_stateTimespan;             ///< max. time delay of current active timeout observation

    bool m_ConfigurationComplete;   //!< Indicates if the expected hardware is found
    DeviceLifeCycleRecord* m_pDeviceLifeCycleRecord;     ///< device's lifeCycle record;
};

}  //namespace

#endif /* DEVICECONTROL_CONFIGURATIONSERVICE_H */
