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
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.112
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
#include <QDateTime>

#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "DeviceControl/Include/CanCommunication/CANInterface.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

class CANReceiveThread;
class CANTransmitThread;
class CModule;

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
class CANCommunicator : public QObject
{
    Q_OBJECT

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CANCommunicator)

public:
    CANCommunicator(QObject *pParent);
    virtual ~CANCommunicator();

    ReturnCode_t StartComm(const char* Interface);
    void StopComm();

    /****************************************************************************/
    /*!
     *  \brief  Report CAN error
     *  \iparam ErrorID index the specific error
     *  \return void
     */
    /****************************************************************************/
    void ReportCANError(quint32 ErrorID = 0);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetCANInterface
     *
     *  \return from GetCANInterface
     */
    /****************************************************************************/
    const DeviceControl::CANInterface &GetCANInterface();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RegisterCOB
     *
     *  \param unCanID = unsigned int type parameter
     *  \param pCallbackModule =  CModule type parameter
     *
     *  \return from RegisterCOB
     */
    /****************************************************************************/
    ReturnCode_t RegisterCOB(unsigned int unCanID, CModule* pCallbackModule);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendCOB
     *
     *  \param canmsg = can_frame type parameter
     *
     *  \return from SendCOB
     */
    /****************************************************************************/
    ReturnCode_t SendCOB(can_frame& canmsg);
	
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsOutMessagePending
     *
     *  \return from IsOutMessagePending
     */
    /****************************************************************************/
    bool IsOutMessagePending();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PopPendingOutMessage
     *
     *  \return from PopPendingOutMessage
     */
    /****************************************************************************/
    can_frame PopPendingOutMessage();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function DispatchMessage
     *
     *  \param canmsg = can_frame type parameter
     *
     *  \return from DispatchMessage
     */
    /****************************************************************************/
    void DispatchMessage(can_frame& canmsg);

signals:
    /****************************************************************************/
    /*!
     *  \brief  Forward error information
     *
     *  \iparam EventCode = Event code
     *  \iparam EventData = Event data
     */
    /****************************************************************************/
    void ReportEvent(quint32 EventCode, quint16 EventData);
    /****************************************************************************/
    /*!
     *  \brief  Forward error information
     *
     *  \iparam instanceID = Instance identifier of this function module instance
     *  \iparam usErrorGroup = Error group
     *  \iparam usErrorID = Error ID
     *  \iparam usErrorData = Additional error information
     *  \iparam timeStamp = Error time
     */
    /****************************************************************************/
    void ReportError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp);


private slots:
    void OnReportEvent(quint32 EventCode, quint16 EventData);

private:
    CANReceiveThread*  m_pCANReceiveThread;     //!< receive thread
    CANTransmitThread* m_pCANTransmitThread;    //!< transmit thread

    CANInterface m_CANInterface;    //!< CAN interface class

    COBConsumerMap m_cobMap;    //!< map containing the registered CAN-message IDs of receivable messages

    COBDeque m_SendQueue;   //!< the send queue for outgoing CAN messages, type tCanMsgStruct

    //internal error situation
    quint16 m_nErrorState;          ///< error state (new, transmitted, changed,...
    qint16  m_nErrorCode;           ///< the error code used to log it
    qint16  m_nErrorAdditionalInfo; ///< additional error information

    quint8 m_errorUntransmitted;  ///< flag for error forwarding

    QMutex m_mutexCOB;  ///< Access serialisation object

};

} //namespace

#endif /* CANCOMMUNICATOR_H */
