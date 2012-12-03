/******************************************************************/
/*! \file AnalogInput.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CAnalogInput
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

#ifndef DEVICECONTROL_ANALOGINPUTPRIVATE_H
#define DEVICECONTROL_ANALOGINPUTPRIVATE_H

#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "Global/Include/MonotonicTime.h"

namespace DeviceControl
{

class CANCommunicator;

/****************************************************************************/
/*!
 *  \brief  Analog input function module class
 *
 *      This class implements the functionality to configure and control a
 *      slave's 'analog input' function module.
 */
/****************************************************************************/
class CAnalogInput : public CFunctionModule
{
    Q_OBJECT

public:
    CAnalogInput(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                 CBaseModule* pParentNode);
    ~CAnalogInput();

    //! Module initialisation
    ReturnCode_t Initialize();

    //! Task handling function
    void HandleTasks();
    //! CAN-message handling function
    void HandleCanMessage(can_frame* pCANframe);

    ReturnCode_t SetState(bool Enable);
    //! Request actual input
    ReturnCode_t ReqActInputValue();

signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report the actual input value
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam InputValue = Actual input value
     */
    /****************************************************************************/
    void ReportActInputValue(quint32 InstanceID, ReturnCode_t HdlInfo, qint16 InputValue);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to acknowledge a set state request
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportSetState(quint32 InstanceID, ReturnCode_t HdlInfo);

private:
    //! CAN message ID initialization
    ReturnCode_t InitializeCANMessages();
    //! registers the CAN messages to communication layer
    ReturnCode_t RegisterCANMessages();

    //! configuration task handling function
    void SendConfiguration();

    //! idle task handling function
    void HandleIdleState();

    //! sends the configuration can messages
    ReturnCode_t SendCANMessageConfiguration(bool Enable);
    //! sends the configuration can messages
    ReturnCode_t SendCANMessageConfiguration();
    //! sends the can message 'InputStateReq'
    ReturnCode_t SendCANMsgAnalogInputStateReq();

    //! handles the receipt of can message 'InputState'
    void HandleCANMsgAnalogInputState(can_frame* pCANframe);

    //! command handling function
    void HandleCommandRequestTask();

    /*! configuration sub state definitions */
    typedef enum {
        FM_AINP_SUB_STATE_CONFIG_INIT     = 0x00,   //!< start up
        FM_AINP_SUB_STATE_CONFIG_START    = 0x01,   //!< start, configuration is pending
        FM_AINP_SUB_STATE_CONFIG_SEND     = 0x02,   //!< configuration will be send now
        FM_AINP_SUB_STATE_CONFIG_FINISHED = 0x03,   //!< configuration sending has finished
        FM_AINP_SUB_STATE_CONFIG_ERROR    = 0x04    //!< configuration sending has finished with an error
    } CANAnalogInputSubStateConfig_t;

    CANAnalogInputSubStateConfig_t m_SubStateConfig;    //!< configuration sub state

    // can message ids
    quint32 m_unCanIDAnaInputConfigInput;   //!< CAN-ID 'ConfigInput' message
    quint32 m_unCanIDAnaInputConfigLimits;  //!< CAN-ID 'ConfigLimits' message
    quint32 m_unCanIDAnaInputStateReq;      //!< CAN-ID 'InputStateReq' message
    quint32 m_unCanIDAnaInputState;         //!< CAN-ID 'InputState' message

    /*! analog input command type definitions */
    typedef enum {
        FM_AI_CMD_TYPE_UNDEF        = 0x00, //!< undefined movement type
        FM_AI_CMD_TYPE_CONFIG_INPUT = 0x01, //!< configuration message
        FM_AI_CMD_TYPE_ACTVALUE_REQ = 0x02  //!< actual value request
    } CANAnalogInputModuleCmdType_t;

    /*! Command data, used for internal data transfer*/
    typedef struct {
        CANAnalogInputModuleCmdType_t Type; //!< command type
        ModuleCmdState_t State;             //!< command state
        Global::MonotonicTime ReqSendTime;  //!< time the command was executed
        qint32 Timeout;                     //!< timeout in ms
        bool Enable;                        //!< Module enable flag
    } ModuleCommand_t;

    QList<ModuleCommand_t *> m_ModuleCommand;   //!< Queue of module commands for simultaneous execution

    //! Adds the module command type to the transmit queue
    ModuleCommand_t *SetModuleTask(CANAnalogInputModuleCmdType_t CommandType);
    //! Clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANAnalogInputModuleCmdType_t CommandType);
};

} //namespace

#endif /* DEVICECONTROL_ANALOGINPUTPRIVATE_H */
