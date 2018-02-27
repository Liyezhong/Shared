/****************************************************************************/
/*! \file MasterConnector.h
 *
 *  \brief Definition file for class MasterConnector.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 30.04.2013
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

#ifndef RCAGENTNAMESPACE_MASTERCONNECTOR_H
#define RCAGENTNAMESPACE_MASTERCONNECTOR_H

// Qt
#include <QtGlobal>
#include <QObject>

// Network Layer
#include <NetworkComponents/Include/MasterLinkDevice.h>

// Global
#include <Global/Include/RemoteCareTypes.h>

namespace NetCommands {
    class CmdRCNotifyShutdown;
    class CmdRCReportEvent;
    class CmdRCNotifyAssetComplete;
    class CmdRCNotifyReconnection;
    class CmdRCNotifyDataItem;
    class CmdRCSetTag;
}

namespace RCAgentNamespace {

/// internal name of the external process
const QString RE_PROCESS_NAME = "RemoteCareAgent";

/****************************************************************************/
/*!
 *  \brief This class setups a connections with master and links commands.
 *
 *   This class handles connection to Master and delivers a interface to the
 *   main controller.
 */
/****************************************************************************/
class MasterConnector : public QObject
{
    Q_OBJECT

    /// test classes are all friends:
    friend class TestMasterConnector;

public:

    MasterConnector(const QString &ip, const QString &port);
    virtual ~MasterConnector();

    bool Initialize();

public slots:

    void OnAgentIncomingCommand(const Global::CommandShPtr_t &ptr);
    void OnAckCommand(const Global::tRefType ref, const bool &result);

signals:
     /****************************************************************************/
     /*!
      *  \brief   This signal is emitted when Master notifies about shutdown of controller.
      */
     /*****************************************************************************/
     void SigShutdownController();
     /****************************************************************************/
     /*!
      *  \brief   This signal is emitted when Master notifies about shutdown of controller.
      *
      *  \iparam  Ref        = request's reference
      */
     /*****************************************************************************/
     void SigReconnect(const Global::tRefType &Ref);
     /****************************************************************************/
     /*!
      *  \brief   This signal is emitted when Master notifies about failure during initialization.
      */
     /*****************************************************************************/
     void SigMasterLinkInitialiizationFailed();
     /****************************************************************************/
     /*!
      *  \brief   This signal is emitted when Master reports a event.
      *
      *  \iparam  ref        = request's reference
      *  \iparam  name       = event's name
      *  \iparam  message    = event's description text
      *  \iparam  severity   = event's severity
      *  \iparam  time       = Master's timestamp value
      */
     /*****************************************************************************/
     void SigReportEvent(
                            const Global::tRefType  &ref,
                            const QString           &name,
                            const QString           &message,
                            const QString           &severity,
                            const QString           &time
                         );
     /****************************************************************************/
     /*!
      *  \brief   This signal is emitted when Master reports a event.
      *
      *  \iparam  name       = event's name
      *  \iparam  message    = event's description text
      *  \iparam  severity   = event's severity
      *  \iparam  time       = Master's timestamp value
      */
     /*****************************************************************************/
     void SigReportEventWithoutAck(
                            const QString           &name,
                            const QString           &message,
                            const QString           &severity,
                            const QString           &time
                         );

     /****************************************************************************/
     /*!
      *  \brief   This signal is emitted when Master reports a dataitem.
      *
      *  \iparam  ref        = request's reference
      *  \iparam  name       = parameter's name
      *  \iparam  type       = type of the parameter (analog/digital/string)
      *  \iparam  quality    = quality of the parameter
      *  \iparam  value      = the value of the parameter
      *  \iparam  time       = Master's timestamp value
      */
     /*****************************************************************************/
     void SigReportDataItem(
                             const Global::tRefType                 &ref,
                             const QString                          &name,
                             const RemoteCare::RCDataItemType_t     &type,
                             const RemoteCare::RCDataItemQuality_t  &quality,
                             const QString                          &value,
                             const QString                          &time
                            );
     /****************************************************************************/
     /*!
      *  \brief   This signal is emitted when Master notifies about upload.
      *
      *  \iparam  ref       = request's reference
      *  \iparam  filename  = the name of the file, which shall be located in "Uploads".
      */
     /*****************************************************************************/
     void SigNotifyUpload(
                            const Global::tRefType  &ref,
                            const QString           &filename
                          );

private slots:
     void CommandTimeOut(Global::tRefType Ref, const QString &CommandName);
     void ConnectedToMaster(const QString &name);
     void DisconnectedFromMaster(const QString &name);

     void ForwardEventReportCommand(    const quint32                           EventCode,
                                        const Global::tTranslatableStringList   &EventStringList,
                                        const bool                              EventStatus,
                                        quint32                                 EventKey,
                                        const Global::AlternateEventStringUsage AltStringUsage);

private:
     Q_DISABLE_COPY(MasterConnector)

     void OnAck(Global::tRefType Ref, const Global::AckOKNOK &Ack);
     void OnShutdownNotification(Global::tRefType Ref, const NetCommands::CmdRCNotifyShutdown &Cmd);
     void OnDataItemNotification(Global::tRefType Ref, const NetCommands::CmdRCNotifyDataItem &Cmd);
     void OnReconnection(Global::tRefType Ref, const NetCommands::CmdRCNotifyReconnection &Cmd);
     void OnNotifyAssetComplete(Global::tRefType Ref, const NetCommands::CmdRCNotifyAssetComplete &Cmd);
     void OnReportEvent(Global::tRefType Ref, const NetCommands::CmdRCReportEvent &Cmd);

     NetworkBase::CMasterLinkDevice *connectorMaster;                ///< MasterLink Device connector

     QEventLoop                      eventLoop;                      ///< Eventloop for waiting
};

} // end namespace

#endif // RCAGENTNAMESPACE_MASTERCONNECTOR_H
