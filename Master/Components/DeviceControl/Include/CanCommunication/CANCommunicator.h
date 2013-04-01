/****************************************************************************/
/*! \file CANCommunicator.h
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 07.02.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CANCommunicator
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

#ifndef CANCOMMUNICATOR_H
#define CANCOMMUNICATOR_H

#include <map>
#include <deque>
#include <QMutex>
#include <QObject>

#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

class CANInterface;
class CANReceiveThread;
class CANTransmitThread;
class CModule;
class Can2TcpClient;

//! Type definition of a map containing the CAN-message receiving objects
typedef std::map <__u32, CModule*> COBConsumerMap;
//! CAN message queue type definition, used as receive and transmit queue (double ended queue)
typedef std::deque <can_frame> COBDeque;

/****************************************************************************/
/*!
 *  \brief  This class manages the incoming and outgoing CAN messages
 *
 *      The class provides a send and a receive queue for the CAN messages.
 *      It also provides methods to register receive CAN messages. After
 *      receiving, a registered CAN-message will be forwarded to the assigned
 *      object.
 */
/****************************************************************************/
class CANCommunicator
{
public:
    CANCommunicator();
    virtual ~CANCommunicator();

    int m_nInstanceNo;  //!< Instance counter

    /****************************************************************************/
    /*!
     *  \brief  Set Simulation flag
     *
     *  \iparam UseTcp = Usage of simulation flag
     */
    /****************************************************************************/
    void SetTcpCommunication(bool UseTcp) { m_bUseTcp = UseTcp; }

    /****************************************************************************/
    /*!
     *  \brief  Return the CAN to TCP converter
     *
     *  \return Pointer to 'CAN to TCP converter' object
     */
    /****************************************************************************/
    Can2TcpClient* GetTcpClient() { return m_pClient; }

    qint16 StartComm(const char* ifaceCAN);
    ReturnCode_t StopComm();

    void SetCommunicationError(qint16 nError, qint16 nErrorAdditionalInfo);
    qint16 GetCommunicationError(qint16& nErrorAdditionalInfo);
    int  GetCommunicationStatus();
    CANInterface* GetCANInterface();

    ReturnCode_t RegisterCOB(unsigned int unCanID, CModule* pCallbackModule);
    ReturnCode_t SendCOB(can_frame& canmsg);
    void DispatchMessage(can_frame& canmsg);

    void DispatchPendingInMessage();
    void AppendToReceiveQueue(can_frame& canmsg);
    bool IsOutMessagePending();
    bool IsInMessagePending();
    can_frame PopPendingOutMessage();
    can_frame PopPendingInMessage();

private:
    CANReceiveThread*  m_pCANReceiveThread;     ///< receive thread
    CANTransmitThread* m_pCANTransmitThread;    ///< transmit thread

    qint16 OpenCAN(const char* iface);

    CANInterface* m_pCANInterface;  ///< CAN interface class

    Can2TcpClient* m_pClient;   ///< the tcp client to connect to simulation software
    bool m_bUseTcp;             ///< and a flag to store the usage

    COBConsumerMap m_cobMap;    ///< map containing the registered CAN-message IDs of receivable messages

    COBDeque m_RecvQueue;   ///< the receive queue for incoming CAN messages
    COBDeque m_SendQueue;   ///< the send queue for outgoing CAN messages, type tCanMsgStruct

    //internal error situation
    quint16 m_nErrorState;          ///< error state (new, transmitted, changed,...
    qint16  m_nErrorCode;           ///< the error code used to log it
    qint16  m_nErrorAdditionalInfo; ///< additional error information

    quint8 errorUntransmitted;  ///< flag for error forwarding

    QMutex m_mutexCOB;  ///< Access serialisation object
};

} //namespace

#endif /* CANCOMMUNICATOR_H */
