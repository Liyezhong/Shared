/****************************************************************************/
/*! \file DigitalInput.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CDigitalInput
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

#ifndef DEVICECONTROL_DIGITALINPUTPRIVATE_H
#define DEVICECONTROL_DIGITALINPUTPRIVATE_H

#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "Global/Include/MonotonicTime.h"

namespace DeviceControl
{

class CANCommunicator;

/****************************************************************************/
/*!
*   \brief This class implements the functionality to configure and control a
*          slave's 'digital input' function module
*/
/****************************************************************************/
class CDigitalInput : public CFunctionModule
{
    Q_OBJECT

public:
    CDigitalInput(const CANMessageConfiguration* p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                  CBaseModule* pParentNode);
    ~CDigitalInput();

    //! Module initialisation
    ReturnCode_t Initialize();

    //! Task handling function
    void HandleTasks();
    //! CAN-message handling function
    void HandleCanMessage(can_frame* pCANframe);

    //! Request actual input
    ReturnCode_t ReqActInputValue();

signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report the actual input value
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam InputValue = Actual input value
     */
    /****************************************************************************/
    void ReportActInputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue);

private:
    //! CAN message ID initialization
    ReturnCode_t InitializeCANMessages();
    //! registers the CAN messages to communication layer
    ReturnCode_t RegisterCANMessages();

    //! Idle taks handling function
    void HandleIdleState();

    //! sends the configuration CAN messages
    ReturnCode_t SendCANMessageConfiguration();
    //! sends the CAN message 'InputStateReq'
    ReturnCode_t SendCANMsgDigInputValueReq();

    //! handles the receipt of can message 'InputState'
    void HandleCANMsgDigInputState(can_frame* pCANframe);

    //! command handling function
    void HandleCommandRequestTask();

    /*! configuration sub states */
    typedef enum {
        FM_DINP_SUB_STATE_CONFIG_INIT   = 0x00,  //!< start up
        FM_DINP_SUB_STATE_CONFIG_START  = 0x01,  //!< start, configuration is pending
        FM_DINP_SUB_STATE_CONFIG_SEND   = 0x02,  //!< configuration will be send now
        FM_DINP_SUB_STATE_CONFIG_FINISH = 0x03,  //!< configuration sending has finished
        FM_DINP_SUB_STATE_CONFIG_ERROR  = 0x04   //!< configuration sending has finished with an error
    } CANDigitalInputSubStateConfig_t;

    CANDigitalInputSubStateConfig_t m_SubStateConfig;  //!< configuration sub state

    quint32 m_unCanIDDigInputConfigInput;   //!< CAN-ID 'ConfigInput' message
    quint32 m_unCanIDDigInputConfigLimits;  //!< CAN-ID 'ConfigLimits' message
    quint32 m_unCanIDDigInputStateReq;      //!< CAN-ID 'InputStateReq' message
    quint32 m_unCanIDDigInputState;         //!< CAN-ID 'InputState' message

    /*! digital input command type definitions */
    typedef enum {
        FM_DI_CMD_TYPE_UNDEF            = 0x00,  //!< undefined movement type
        FM_DI_CMD_TYPE_ACTVALUE_REQ     = 0x01   //!< actual value request
    } CANDigitalInputModuleCmdType_t;

    /*! Module command data, used for internal data transfer*/
    typedef struct {
        CANDigitalInputModuleCmdType_t Type;    //!< command type
        ModuleCmdState_t State;                 //!< command state
        Global::MonotonicTime ReqSendTime;      //!< time the command was executed
        qint32 Timeout;                         //!< timeout in ms
    } ModuleCommand_t;

    QList<ModuleCommand_t *> m_ModuleCommand;   //!< Queue of module commands for simultaneous execution

    //! Adds the module command type to the transmit queue
    ModuleCommand_t *SetModuleTask(CANDigitalInputModuleCmdType_t CommandType);
    //! Clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANDigitalInputModuleCmdType_t CommandType);
};

} //namespace

#endif /* DEVICECONTROL_DIGITALINPUTPRIVATE_H */
