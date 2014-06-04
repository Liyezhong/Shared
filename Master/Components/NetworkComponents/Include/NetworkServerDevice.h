/****************************************************************************/
/*! \file NetworkServerDevice.h
 *
 *  \brief Network Server Device definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.11.2010
 *   $Author:  $ Y.Novak
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

#ifndef NETWORKBASE_NETWORKSERVERDEVICE_H
#define NETWORKBASE_NETWORKSERVERDEVICE_H

#include <QTimer>
#include <NetworkComponents/Include/NetworkServer.h>
#include <NetworkComponents/Include/NetworkDevice.h>
#include <NetworkComponents/Include/ProtocolTxCommand.h>

/// Test classes
namespace ExternalProcessControl {
    class TestExternalProcessDvc;
    class TestExternalProcessCtlr;
    class TestExternalProcessCtlr2;
}

namespace NetworkBase {

/*! Time interval for the periodic DateAndTime sync scenario.
    \warning
    WARNING! We need 1 Hour timer and the QTimer::start(int)
    takes INT as input parameter. INT type on our current
    embedded platform has 32-bit width, which is OK.
    If on any future platform INT type will have a 16-bit width,
    this timer will not work as expected!
*/
const int DATETIME_SYNC_DEFAULT_DELAY = 3600000; ///< 3600000 ms = 1 Hour

/****************************************************************************/
/**
 * \brief This class controls communication with NetworkClientDevice.
 *
 */
/****************************************************************************/
class NetworkServerDevice : public NetworkDevice
{
   Q_OBJECT

   /// all test classes are friends:
   friend class ExternalProcessControl::TestExternalProcessDvc;
   friend class ExternalProcessControl::TestExternalProcessCtlr;
   friend class ExternalProcessControl::TestExternalProcessCtlr2;
   friend class TestNetworkServerDevice;

public:

    NetworkServerDevice(NetworkServerType_t stype, const QString &client, const QString &path, QObject *pParent = 0);
    virtual ~NetworkServerDevice();
    virtual bool InitializeDevice();
    virtual bool StartDevice();
    virtual void DestroyDevice();
    virtual void DisconnectPeer();
    virtual void PeerDisconnected(const QString &);
    bool StartDateTimeSync(int period);

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    virtual void HandleNetworkError(NetworkBase::DisconnectType_t , const QString &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    virtual void SyncDateAndTime();

private:

    NetworkServerDevice();                                                  ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(NetworkServerDevice)
    bool InitializeNetwork();

private:

    /// Network server for communication with peer
    NetworkServer          *m_myServer;
    /// Type of server to instantiate
    NetworkServerType_t     m_myServerType;
    /// Client which is allowed to connect to this server
    QString                 m_myClient;
    /// Periodic DateAndTime sync timer (actual DateTime can be sent to clients periodically)
    QTimer                  m_DateTimeSyncTimer;
    /// Period for the DateAndTime sync timer
    int                     m_dtTimerPeriod;
};

} // end namespace NetworkBase

#endif // NETWORKBASE_NETWORKSERVERDEVICE_H
