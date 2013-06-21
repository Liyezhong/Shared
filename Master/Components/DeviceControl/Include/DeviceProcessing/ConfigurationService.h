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

/****************************************************************************/
/*!
 *  \brief This class implements the functionality to configure the complete
 *         object tree of the device control layer
 */
/****************************************************************************/
class CConfigurationService
{
public:
    CConfigurationService(DeviceProcessing &Processing, CANCommunicator &Communicator);
    virtual ~CConfigurationService();

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

    bool ConfigurationComplete();

private:
    ReturnCode_t CreateDeviceComponents();
    ReturnCode_t CreateObjectTree(HardwareConfiguration* pHWConfiguration);
    ReturnCode_t CreateDevices(HardwareConfiguration* pHWConfiguration);

    void HandleCANNodesTask();
    ReturnCode_t IsCANNodesStateIdle();
    ReturnCode_t CreateDiscoveredHWConfiguration();

    CBaseModule* CreateAndGetCANNode(qint16 sCANNodeType, qint16 sCANNodeIndex);
    template <class TFunctionModule>
    void CreateAndAddFunctionModule(CBaseModule* pCANNode, CModuleConfig* pCANObjectConfigFct);

    /// Configuration error sub state definitions
    typedef enum  {
        CS_SUBSTATE_ERR_UNDEF    = 0x00,    ///< undefined
        CS_SUBSTATE_ERR_INIT     = 0x01,    ///< init, the error state was set but not logged
        CS_SUBSTATE_ERR_IDLE     = 0x02     ///< idle, all error handling stuff was done, now just wait
    } ConfigServiceErrSubState_t;

    ConfigServiceMainState_t   m_MainState;     ///< main state
    ConfigServiceErrSubState_t m_ErrSubState;   ///< error sub state

    DeviceProcessing &m_DeviceProcessing;   ///< Device processing instance
    CANCommunicator &m_CANCommunicator;     ///< CAN communicator

    Global::MonotonicTime m_stateTimer; ///< timer for timeout observation
    qint16 m_stateTimespan;             ///< max. time delay of current active timeout observation

    bool m_ConfigurationComplete;   //!< Indicates if the expected hardware is found
};

}  //namespace

#endif /* DEVICECONTROL_CONFIGURATIONSERVICE_H */
