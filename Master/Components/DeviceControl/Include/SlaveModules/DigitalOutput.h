/****************************************************************************/
/*! \file DigitalOutput.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class DigitalOutput
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

#ifndef DEVICECONTROL_DIGITALOUTPUTPRIVATE_H
#define DEVICECONTROL_DIGITALOUTPUTPRIVATE_H

#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "Global/Include/MonotonicTime.h"

namespace DeviceControl
{

class CANCommunicator;

#define MAX_DOUTP_CMD_IDX  2   ///< up to 2 module commands can be handled simultaneously

/****************************************************************************/
/*!
*   \brief This class implements the functionality to configure and control a
*          slave's 'digital output' function module
*/
/****************************************************************************/
class CDigitalOutput : public CFunctionModule
{
    Q_OBJECT

public:
    CDigitalOutput(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                   CBaseModule* pParentNode);
    ~CDigitalOutput();

    //!< Module initialisation
    ReturnCode_t Initialize();

    //!< Task handling function
    void HandleTasks();
    //!< CAN-message handling function
    void HandleCanMessage(can_frame* pCANframe);

    //!< Set output
    ReturnCode_t SetOutputValue(quint16 OutputValue, quint16 Duration = 0, quint16 Delay = 0);
    //!< Request actual output
    ReturnCode_t ReqOutputValue();
    //!< Request life time data
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
     *  \brief    This signal is emitted to report the  value
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
    //!< send the 'OutputStateSet' CAN messages
    ReturnCode_t SendCANMsgDigOutputDataSet(quint16 OutputValue, quint16 Duration, quint16 Delay);
    //!< send the 'OutputStateReq' CAN messages
    ReturnCode_t SendCANMsgDigOutputDataReq();
    //!< send the 'LivetimeDataReq' CAN messages
    ReturnCode_t SendCANMsgLifeTimeDataReq();

    //!< handles the receipt of can message 'OutputState'
    void HandleCANMsgOutputState(can_frame* pCANframe);
    //!< handles the receipt of can message 'LivetimeData'
    void HandleCANMsgLifeTimeData(can_frame* pCANframe);

    //!< command handling function
    void HandleCommandRequestTask();

    /*! configuration sub states */
    typedef enum {
        FM_DOUTP_SUB_STATE_CONFIG_INIT   = 0x00,  //!< start up
        FM_DOUTP_SUB_STATE_CONFIG_START  = 0x01,  //!< start, configuration is pending
        FM_DOUTP_SUB_STATE_CONFIG_SEND   = 0x02,  //!< configuration will be send now
        FM_DOUTP_SUB_STATE_CONFIG_FINISH = 0x03,  //!< configuration sending has finished
        FM_DOUTP_SUB_STATE_CONFIG_ERROR  = 0x04   //!< configuration sending has finished with an error
    } CANDigitalOutputSubStateConfig_t;

    CANDigitalOutputSubStateConfig_t  m_SubStateConfig;  //!< configuration sub state

    quint32 m_unCanIDDigOutputConfigOutput;   //!< CAN-ID 'ConfigInput' message
    quint32 m_unCanIDDigOutputConfigBlinking; //!< CAN-ID 'ConfigBlinking' message
    quint32 m_unCanIDDigOutputStateSet;       //!< CAN-ID 'OutputStateSet' message
    quint32 m_unCanIDDigOutputStateReq;       //!< CAN-ID 'OutputStateReq' message
    quint32 m_unCanIDDigOutputState;          //!< CAN-ID 'OutputState' message
    quint32 m_unCanIDDigOutputLifetimeReq;    //!< CAN-ID 'LivetimeDataReq' message
    quint32 m_unCanIDDigOutputLifetime;       //!< CAN-ID 'LivetimeData' message

    /*! digital output command type definitions */
    typedef enum {
        FM_DO_CMD_TYPE_UNDEF          = 0x00,  //!< undefined command type
        FM_DO_CMD_TYPE_SET_OUTP       = 0x02,  //!< set output value
        FM_DO_CMD_TYPE_REQ_ACTVALUE   = 0x03,  //!< actual value request
        FM_DO_CMD_TYPE_REQ_LIFECYCLE  = 0x04,  //!< request life cycle data
        FM_DO_CMD_TYPE_REQ_DATA_RESET = 0x05   //!< data reset
    } CANDigitalOutputModuleCmdType_t;

    /*! module command data, used for internal data transfer*/
    typedef struct {
        CANDigitalOutputModuleCmdType_t  m_Type;    //!< command type
        ModuleCmdState_t m_State;                   //!< command state
        Global::MonotonicTime m_ReqSendTime;        //!< time the command was executed
        qint32 m_Timeout;                           //!< timeout in ms
        quint8 m_TimeoutRetry;                      //!< timeout retry time
        quint16 m_OutputValue;                      //!< requested output value
        quint16 m_Duration;                         //!< Time the ouptput is on
        quint16 m_Delay;                            //!< Time before the output is on

    } ModuleCommand_t;

    ModuleCommand_t m_ModuleCommand[MAX_DOUTP_CMD_IDX];  //!<  array of module commands for simultaneously execution

    //!< set the module command type to free entry within array
    bool SetModuleTask(CANDigitalOutputModuleCmdType_t CommandType, quint8* pCmdIndex = 0);
    //!< clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANDigitalOutputModuleCmdType_t CommandType);
};

} //namespace

#endif /* DEVICECONTROL_DIGITALOUTPUTPRIVATE_H */
