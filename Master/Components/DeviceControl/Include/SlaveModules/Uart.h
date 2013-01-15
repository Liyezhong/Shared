/******************************************************************/
/*! \file Uart.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CUart
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

#ifndef DEVICECONTROL_UARTPRIVATE_H
#define DEVICECONTROL_UARTPRIVATE_H

#include "Global/Include/MonotonicTime.h"
#include "DeviceControl/Include/SlaveModules/FunctionModule.h"

namespace DeviceControl
{

class CANCommunicator;

/****************************************************************************/
/*!
 *  \brief UART function module class
 *
 *      This class implements the functionality to configure and control a
 *      slave's 'UART' function module
 */
/****************************************************************************/
class CUart : public CFunctionModule
{
    Q_OBJECT

public:
    CUart(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
          CBaseModule* pParentNode);
    ~CUart();

    /// function module initialization, can communication initialisation
    ReturnCode_t Initialize();

    /// task handling function
    void HandleTasks();

    /// recall function to receive CAN messages from communication layer
    void HandleCanMessage(can_frame* pCANframe);

    ReturnCode_t SendData(QByteArray sendData);
    ReturnCode_t ReqReceivedData();

signals:
    //! report data
    void ReportReadData(quint32 InstanceID, ReturnCode_t hdlInfo, quint32 rfidDataMSB, quint32 rfidDataLSB);
    //! report a write operation acknowledge
    void ReportWriteAckn(quint32 InstanceID, ReturnCode_t hdlInfo);

private:
    /// CAN message ID initialization
    ReturnCode_t InitializeCANMessages();
    /// Register receive CAN messages
    ReturnCode_t RegisterCANMessages();

    /// Handle the configuration state
    void HandleConfigurationState();

    /// Handle the idle state
    void HandleIdleState();

    /// Send CAN message 'request error'
    ReturnCode_t SendCANMsgErrorReqest();
    /// Send CAN message 'Configuration'
    ReturnCode_t SendCANMessageConfiguration();
    /// Send CAN message 'set send data'
    ReturnCode_t SendCANMsgSetSendData();
    /// Send CAN message 'receive data request'
    ReturnCode_t SendCANMsgReceiveDataRequest();

    /// Handle the receiption of CAN message 'send data acknowledge'
    void HandleCANMsgSendDataAckn(can_frame* pCANframe);
    /// Handle the receiption of CAN message 'recieve data'
    void HandleCANMsgReceiveData(can_frame* pCANframe);

    /// Handle the task to send data via CAN to fct-module on slave
    void HandleSendDataTask();
    /// Handle the task to request received data via CAN from fct-module on slave
    void HandleReqReceiveDataTask();
    /// Reset the task state machine
    void ResetTask();

    //! configuration state defines
    typedef enum {
        FM_UART_SUB_STATE_CONFIG_INIT   = 0x00, //!< Configuration initial state
        FM_UART_SUB_STATE_CONFIG_START  = 0x01, //!< Start state, used to trigger the configuration transmission
        FM_UART_SUB_STATE_CONFIG_SEND   = 0x02, //!< Configuration is actually send
        FM_UART_SUB_STATE_CONFIG_FINISH = 0x03, //!< Configuration finished
        FM_UART_SUB_STATE_CONFIG_ERROR  = 0x04  //!< Error occurred while configuration procedure was done
    } CANNodeSubStateConfig_t;

    CANNodeSubStateConfig_t m_subStateConfig;   //!< Sub state of the configuration task
    ModuleCmdState_t m_taskState;               //!< State of the currently active task

    quint32  m_unCanIDError;        //!< CAN-message id of 'error' message
    quint32  m_unCanIDErrorReq;     //!< CAN-message id of 'request error' message
    quint32  m_unCanIDUARTConfig;   //!< CAN-message id of 'TBD' message
    quint32  m_unCanIDUARTDataSet;  //!< CAN-message id of 'TBD' message
    quint32  m_unCanIDUARTDataAckn; //!< CAN-message id of 'TBD' message
    quint32  m_unCanIDUARTDataReq;  //!< CAN-message id of 'TBD' message
    quint32  m_unCanIDUARTData;     //!< CAN-message id of 'TBD' message

    Global::MonotonicTime m_timeAction;     //!< start time used to check for several timeouts
    qint16 m_aktionTimespan;                //!< time span value used to check for several timeouts
};

} //namespace

#endif // DEVICECONTROL_UARTPRIVATE_H
