/******************************************************************/
/*! \file AnalogOutput.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
*       This module contains the declaration of the class CAnalogOutput
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

#ifndef DEVICECONTROL_ANALOGOUTPUTPRIVATE_H
#define DEVICECONTROL_ANALOGOUTPUTPRIVATE_H

#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "Global/Include/MonotonicTime.h"

namespace DeviceControl
{

class CANCommunicator;

/****************************************************************************/
/*!
 *  \brief Analog input function module class
 *
 *      This class implements the functionality to configure and control a
 *      slave's 'analog output' function module.
 */
/****************************************************************************/
class CAnalogOutput : public CFunctionModule
{
    Q_OBJECT

public:
    CAnalogOutput(const CANMessageConfiguration* p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                  CBaseModule* pParentNode);
    ~CAnalogOutput();

    //! Module initialisation
    ReturnCode_t Initialize();

    //! Task handling function
    void HandleTasks();
    //! CAN-message handling function
    void HandleCanMessage(can_frame* pCANframe);

    //! Set output
    ReturnCode_t SetOutputValue(quint16 OutputValue, quint16 Duration = 0, quint16 Delay = 0);
    //! Request actual output
    ReturnCode_t ReqOutputValue();
    //! Request life time data
    ReturnCode_t ReqLifeTimeData();
    //! Request data reset
    ReturnCode_t ReqDataReset();

signals:
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to acknowledge the setting of the last requested output value
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   OutputValue = Output value recently set
     *
     ****************************************************************************/
    void ReportOutputValueAckn(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the actual output value
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   OutputValue = Actual output value
     *
     ****************************************************************************/
    void ReportActOutputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the live time data
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   LifeTime   = Total on-times (minutes)
     *  \param   LifeCycles = Number of off-to-on transitions
     *
     ****************************************************************************/
    void ReportLifeTimeData(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 LifeTime, quint32 LifeCycles);

private:
    //! CAN message ID initialization
    ReturnCode_t InitializeCANMessages();
    //! registers the CAN messages to communication layer
    ReturnCode_t RegisterCANMessages();

    //! Idle taks handling function
    void HandleIdleState();

    //! sends the configuration CAN messages
    ReturnCode_t SendCANMessageConfiguration();
    //! send the 'OutputStateSet' CAN messages
    ReturnCode_t SendCANMsgAnalogOutputStateSet(quint16 OutputValue, quint16 Duration, quint16 Delay);
    //! send the 'OutputStateReq' CAN messages
    ReturnCode_t SendCANMsgAnalogOutputStateReq();
    //! send the 'LivetimeDataReq' CAN messages
    ReturnCode_t SendCANMsgLifeTimeDataReq();

    //! handles the receipt of can message 'OutputState'
    void HandleCANMsgAnalogOutputState(can_frame* pCANframe);
    //! handles the receipt of can message 'LivetimeData'
    void HandleCANMsgAnalogOutputLifetime(can_frame* pCANframe);

    //!< command handling function
    void HandleCommandRequestTask();

    /*! configuration sub state declaration */
    typedef enum {
        FM_AOUTP_SUB_STATE_CONFIG_INIT     = 0x00,  ///< initialisation state
        FM_AOUTP_SUB_STATE_CONFIG_START    = 0x01,  ///< start state
        FM_AOUTP_SUB_STATE_CONFIG_SEND     = 0x02,  ///< configuration is actually send
        FM_AOUTP_SUB_STATE_CONFIG_FINISHED = 0x03,  ///< configuration is finished
        FM_AOUTP_SUB_STATE_CONFIG_ERROR    = 0x04   ///< error state
    } CANAnalogOutputSubStateConfig_t;

    CANAnalogOutputSubStateConfig_t m_SubStateConfig;  //!< configuration sub state

    // CAN message IDs
    quint32 m_unCanIDAnaOutputConfigOutput;   //!< CAN-ID 'ConfigOutput' message
    quint32 m_unCanIDAnaOutputStateSet;       //!< CAN-ID 'OutputStateSet' message
    quint32 m_unCanIDAnaOutputStateReq;       //!< CAN-ID 'OutputStateReq' message
    quint32 m_unCanIDAnaOutputState;          //!< CAN-ID 'OutputState' message
    quint32 m_unCanIDAnaOutputLifeTimeReq;    //!< CAN-ID 'LiveTimeDataReq' message
    quint32 m_unCanIDAnaOutputLifeTime;       //!< CAN-ID 'LiveTimeData' message

    /*! analog output command type definitions */
    typedef enum {
        FM_AO_CMD_TYPE_UNDEF          = 0x00,  //!< undefined movement type
        FM_AO_CMD_TYPE_SET_OUTP       = 0x02,  //!< set output value
        FM_AO_CMD_TYPE_REQ_ACTVALUE   = 0x03,  //!< actual value request
        FM_AO_CMD_TYPE_REQ_LIFECYCLE  = 0x04,  //!< request life cycle data
        FM_AO_CMD_TYPE_REQ_DATA_RESET = 0x05   //!< data reset
    } CANAnalogOutputModuleCmdType_t;

    /*! module command data, used for internal data transfer*/
    typedef struct {
        CANAnalogOutputModuleCmdType_t Type;    //!< command type
        ModuleCmdState_t State;                 //!< command state
        Global::MonotonicTime ReqSendTime;      //!< time the command was executed
        qint32 Timeout;                         //!< timeout in ms
        quint16 OutputValue;                    //!< requested output value
        quint16 Duration;                       //!< Time the ouptput is on
        quint16 Delay;                          //!< Time before the output is on
    } ModuleCommand_t;

    QList<ModuleCommand_t *> m_ModuleCommand;   //!< Queue of module commands for simultaneous execution

    //! Adds the module command type to the transmit queue
    ModuleCommand_t *SetModuleTask(CANAnalogOutputModuleCmdType_t CommandType);
    //! Clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANAnalogOutputModuleCmdType_t CommandType);
};

} //namespace

#endif //DEVICECONTROL_CANALOGOUTPUTPRIVATE_H
