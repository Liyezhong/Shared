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

#define MAX_DINP_CMD_IDX  2   ///< up to 2 module commands can be handled simultaneously

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

    //!< Module initialisation
    ReturnCode_t Initialize();

    //!< Task handling function
    void HandleTasks();
    //!< CAN-message handling function
    void HandleCanMessage(can_frame* pCANframe);

    //!< Request actual input
    ReturnCode_t ReqActInputValue();

signals:
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the actual input value
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   InputValue = Actual input value
     *
     ****************************************************************************/
    void ReportActInputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue);

private:
    //!< CAN message ID initialization
    ReturnCode_t InitializeCANMessages();
    //!< registers the CAN messages to communication layer
    ReturnCode_t RegisterCANMessages();

    //!< configuration task handling function
    void SendConfiguration();

    //!< Idle taks handling function
    void HandleIdleState();

    //!< sends the configuration CAN messages
    ReturnCode_t SendCANMessageConfiguration();
    //!< sends the CAN message 'InputStateReq'
    ReturnCode_t SendCANMsgDigInputValueReq();

    //!< handles the receipt of can message 'InputState'
    void HandleCANMsgDigInputState(can_frame* pCANframe);

    //!< command handling function
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

    /*! motor command data, used for internal data transfer*/
    typedef struct {
        CANDigitalInputModuleCmdType_t m_Type;  //!< command type
        ModuleCmdState_t m_State;               //!< command state
        Global::MonotonicTime m_ReqSendTime;    //!< time the command was executed
        qint32 m_Timeout;                       //!< timeout in ms
        quint8 m_TimeoutRetry;                  //!< timeout retry time
    } ModuleCommand_t;

    ModuleCommand_t m_ModuleCommand[MAX_DINP_CMD_IDX];  //!< // array of module commands for simultaneously execution

    //!< set the module command type to free entry within array
    bool SetModuleTask(CANDigitalInputModuleCmdType_t CommandType, quint8* pCmdIndex = 0);
    //!< clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANDigitalInputModuleCmdType_t CommandType);
};

} //namespace

#endif /* DEVICECONTROL_DIGITALINPUTPRIVATE_H */
