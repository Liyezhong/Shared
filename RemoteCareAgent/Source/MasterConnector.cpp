/****************************************************************************/
/*! \file MasterConnector.cpp
 *
 *  \brief Implementation file for class MasterController. This class handles
 *  connection to master and forwards all incoming commands as well as logging
 *  events to the Master.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 14.05.2013
 *  $Author:    $ B.Stach
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

// Internal
#include "MasterConnector.h"
#include "RemoteCareEventCodes.h"

#include <QMetaType>
#include <QSharedMemory>
#include <Global/Include/SystemPaths.h>

// Global
#include <Global/Include/Utils.h>
#include <Global/Include/SharedPointer.h>
#include <Global/Include/Commands/Command.h>
#include <Global/Include/Commands/AckOKNOK.h>

// Network
#include <NetworkComponents/Include/MasterLinkDevice.h>

// Commands to Master (Request, Set)
#include <NetCommands/Include/CmdRCSetTag.h>
#include <NetCommands/Include/CmdRCSetLogEvent.h>

// Commands from Master (Notify, Report)
#include <NetCommands/Include/CmdRCReportEvent.h>
#include <NetCommands/Include/CmdRCNotifyAssetComplete.h>
#include <NetCommands/Include/CmdRCNotifyShutdown.h>
#include <NetCommands/Include/CmdRCNotifyReconnection.h>
#include <NetCommands/Include/CmdRCNotifyDataItem.h>

namespace RCAgentNamespace {

const int COMMAND_TIME_OUT = 15000;                    //!< Command Timeout

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam  ip    = IP adress
 *  \iparam  port  = Port number
 */
/****************************************************************************/
MasterConnector::MasterConnector(const QString &ip, const QString &port)
{
    qRegisterMetaType<Global::AlternateEventStringUsage>("Global::AlternateEventStringUsage");
    qRegisterMetaType<RemoteCare::RCDataItemType_t>("RemoteCare::RCDataItemType_t");
    qRegisterMetaType<RemoteCare::RCDataItemQuality_t>("RemoteCare::RCDataItemQuality_t");
    qRegisterMetaType<Global::CommandShPtr_t>("Global::CommandShPtr_t");

    connectorMaster = new NetworkBase::CMasterLinkDevice(ip, port, NetworkBase::NCE_TYPE_AXEDA);
}

/****************************************************************************/
/*!
 *  \brief Destructor.
 */
/****************************************************************************/
MasterConnector::~MasterConnector()
{
    try {
        delete connectorMaster;
        connectorMaster = NULL;
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief   Initialize Network Layer for connection to Master.
 *
 *  \return  true if link is successfully initialized, false otherwise
 */
/****************************************************************************/
bool MasterConnector::Initialize()
{
    try {

        if ( !connectorMaster->NetworkInit() ) {
            qDebug() << "Failed to initialize Master network";
            return false;
        }

        CONNECTSIGNALSLOT(connectorMaster, SigCmdTimeout(Global::tRefType, const QString &),
                          this, CommandTimeOut(Global::tRefType, const QString &));
        /// Master signals
        CONNECTSIGNALSLOT(connectorMaster,SigMasterConnected(const QString &), this, ConnectedToMaster(const QString &));
        CONNECTSIGNALSLOT(connectorMaster,SigMasterDisconnected(const QString &), this, DisconnectedFromMaster(const QString &));

        CONNECTSIGNALSLOT(&Global::EventObject::Instance(),
                          ForwardEvent(const quint32, const Global::tTranslatableStringList &,
                                       const bool, quint32 , const Global::AlternateEventStringUsage), this,
                          ForwardEventReportCommand(const quint32, const Global::tTranslatableStringList &,
                                       const bool, quint32 , const Global::AlternateEventStringUsage))

        // Notify from Master
        connectorMaster->RegisterNetMessage<NetCommands::CmdRCNotifyAssetComplete>(&MasterConnector::OnNotifyAssetComplete, this);
        connectorMaster->RegisterNetMessage<NetCommands::CmdRCNotifyReconnection>(&MasterConnector::OnReconnection, this);
        connectorMaster->RegisterNetMessage<NetCommands::CmdRCNotifyShutdown>(&MasterConnector::OnShutdownNotification, this);
        connectorMaster->RegisterNetMessage<NetCommands::CmdRCNotifyDataItem>(&MasterConnector::OnDataItemNotification, this);
        // Report from Master
        connectorMaster->RegisterNetMessage<NetCommands::CmdRCReportEvent>(&MasterConnector::OnReportEvent, this);
    }
    catch (const std::bad_alloc &) {
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief This slot function is called when connection to master gets a timeout
 *
 *  \iparam Ref         = Command reference
 *  \iparam CommandName = Command name
 */
/****************************************************************************/
void MasterConnector::CommandTimeOut(Global::tRefType Ref, const QString &CommandName)
{
    DEBUGWHEREAMI;
    qDebug() << "Ref = " << Ref << " Name = " << CommandName;
}

/****************************************************************************/
/*!
 *  \brief   This slot is called when connection to Master SW is established
 *
 *  \iparam name = name of the client
 */
/****************************************************************************/
void MasterConnector::ConnectedToMaster(const QString &name)
{
    qDebug() << "MasterConnector: ConnectedToMaster slot is called." << name;
}

/****************************************************************************/
/*!
 *  \brief   This slot is called when connection to Master SW is lost.
 *           Req: 10815 informing about failed connection to master
 *
 *  \iparam name = name of the client
 */
/****************************************************************************/
void MasterConnector::DisconnectedFromMaster(const QString &name)
{
    qDebug() << "MasterConnector: DisconnectedFromMaster slot is called." << name;
    QString message  = " Disconnected from Master";
    QString severity = "300";
    QString empty    = "";

    emit SigReportEventWithoutAck(name, message, severity, empty);
}

/****************************************************************************/
/*!
 *  \brief Send event report command to Master
 *
 *  \iparam EventCode       = Event Code
 *  \iparam EventStringList = EventString List
 *  \iparam EventKey        = Event Key
 *  \iparam AltStringUsage  = Alternate String
 *  \iparam EventStatus     = Event Status
 *
 */
/****************************************************************************/
void MasterConnector::ForwardEventReportCommand(    const quint32                           EventCode,
                                                    const Global::tTranslatableStringList   &EventStringList,
                                                    const bool                              EventStatus,
                                                    quint32                                 EventKey,
                                                    const Global::AlternateEventStringUsage AltStringUsage)
{
    DEBUGWHEREAMI;
    Q_UNUSED(EventKey);
    Q_UNUSED(EventStatus);
    NetCommands::RCLogEventReportStruct EventStructure;
    EventStructure.EventCode = EventCode;
    EventStructure.AltStringUsage = AltStringUsage;

    for (qint32 Counter = 0; Counter < EventStringList.count(); Counter++) {
        Global::TranslatableString TranslateString = EventStringList.value(Counter);
        EventStructure.EventStringList << TranslateString.GetString();
    }
    NetCommands::CmdRCSetLogEvent Cmd(COMMAND_TIME_OUT, EventStructure);

    (void)connectorMaster->SendCmdToMaster(Cmd, &MasterConnector::OnAck, this);
}

/****************************************************************************/
/*!
 * \brief Acknowledge received
 *
 * \iparam Ref = Acknowledge reference.
 * \iparam Ack = The acknowledge.
 */
/****************************************************************************/
void MasterConnector::OnAck(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    qDebug() << "MasterConnector: OnAck is called " << Ref << "Ack " << Ack.GetStatus();
}

/****************************************************************************/
/*!
 *  \brief   This slot is called when RE connector acknowledges the Master command.
 *
 *  \iparam    ref    = reference of the request
 *  \iparam    result = true if ok, false otherwise
 */
/****************************************************************************/
void MasterConnector::OnAckCommand(const Global::tRefType ref, const bool &result)
{
    qDebug() << "MasterConnector: OnAckCommand " << ref << " on RemoteCareAgent reported status : " << result;
    connectorMaster->SendAckToMaster(ref, Global::AckOKNOK(result));
}

/****************************************************************************/
/*!
 *  \brief This function notifies about shutdown of the controller.
 *  \iparam Ref = Reference type
 *  \iparam Cmd = Command status parameter
 */
/****************************************************************************/
void MasterConnector::OnShutdownNotification(Global::tRefType Ref, const NetCommands::CmdRCNotifyShutdown &Cmd)
{
    Q_UNUSED(Cmd);
    qDebug() << "MasterConnector:Received shutdown notification.";

    connectorMaster->SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit SigShutdownController();
}

/****************************************************************************/
/*!
 *  \brief Master notifies about reconnection to Remote Enterprise Server
 *  \iparam Ref = Reference type
 *  \iparam Cmd = Command status parameter
 */
/****************************************************************************/
void MasterConnector::OnReconnection(Global::tRefType Ref, const NetCommands::CmdRCNotifyReconnection &Cmd)
{
    Q_UNUSED(Cmd);
    qDebug() << "MasterConnector:Received reconnection notification.";

    emit SigReconnect(Ref);
}

/****************************************************************************/
/*!
 *  \brief This function reports an event to RE Server.
 *  \iparam Ref = Reference type
 *  \iparam Cmd = Command status parameter
 */
/****************************************************************************/
void MasterConnector::OnReportEvent(Global::tRefType Ref, const NetCommands::CmdRCReportEvent &Cmd)
{
    qDebug() << "MasterConnector:Received report event.";

    NetCommands::RCEventReportDataStruct eventData = Cmd.GetRCEventReportDataStruct();
    emit SigReportEvent(Ref, eventData.m_EventName, eventData.m_EventMessage, eventData.m_EventSeverity, eventData.m_TimeStamp);
}

/****************************************************************************/
/*!
 *  \brief This sends a notification about complete asset package for upload.
 *  \iparam Ref = Reference type
 *  \iparam Cmd = Command status parameter
 */
/****************************************************************************/
void MasterConnector::OnNotifyAssetComplete(Global::tRefType Ref, const NetCommands::CmdRCNotifyAssetComplete &Cmd)
{
    qDebug() << "MasterConnector:Received asset complete.";
    emit SigNotifyUpload(Ref, Cmd.GetFilePath().toUtf8());
}

/****************************************************************************/
/*!
 *  \brief This forwards the data item information to RE Server.
 *  \iparam Ref  = Reference type
 *  \iparam Cmd  = Command status parameter
 */
/****************************************************************************/
void MasterConnector::OnDataItemNotification(Global::tRefType Ref, const NetCommands::CmdRCNotifyDataItem &Cmd)
{
    qDebug() << "MasterConnector:Received data item notification.";
    emit SigReportDataItem(Ref, Cmd.GetDataItemName(), Cmd.GetDataItemType(), Cmd.GetDataItemQuality(), Cmd.GetDataItemValue(), Cmd.GetDataItemTimestamp());
}

/****************************************************************************/
/*!
 *  \brief   This slot is called when Agent receives a command from the
 *           Remote Care Server.
 *
 *  \iparam  ptr = shared pointer to a command
 */
/*****************************************************************************/
void MasterConnector::OnAgentIncomingCommand(const Global::CommandShPtr_t &ptr)
{
    qDebug() << "*****************************************";
    qDebug() << "MasterConnector: OnAgentIncomingCommand " ;
    qDebug() << "*****************************************";

    const NetCommands::CmdRCSetTag *Cmd = dynamic_cast<const NetCommands::CmdRCSetTag*>(ptr.GetPointerToUserData());
    CHECKPTR(Cmd);

    (void)connectorMaster->SendCmdToMaster(*Cmd, &MasterConnector::OnAck, this);
}

} // end namespace
