/****************************************************************************/
/*! \file TestNetworkServer.h
 *
 *  \brief Definition file for class TestNetworkServer.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 10.02.2011
 *  $Author:  $ Y.Novak
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

#ifndef NETWORKBASE_TESTNETWORKSERVER_H
#define NETWORKBASE_TESTNETWORKSERVER_H

#include <NetworkComponents/Include/ConnectionManager.h>
#include <NetworkComponents/Include/NetworkServer.h>
#include <NetworkComponents/Include/NetworkServerDevice.h>

namespace NetworkBase {

/****************************************************************************/
/**
 * \brief Test class for NetworkServer class.
 */
/****************************************************************************/
class TestNetworkServer: public QObject
{
  Q_OBJECT

public:

    TestNetworkServer();
    ~TestNetworkServer();

public slots:

    void TestClientConnected(const QString &);
    void TestClientDisconnected(const QString &);
    void TestConnectionError(NetworkBase::DisconnectType_t, const QString &);
    void TestDestroyConnection(NetworkBase::DisconnectType_t);

    void utTestGetFreeConnectionNumber();
    void utTestServerTypeStringLookup();     ///< Test N1
    void utTestReferenceGeneration();        ///< Test N2
    void utTestDisconnectSignal();           ///< Test N3
    void utTestWorkFunctions();              ///< Test N4
    void utTestBadInputHandling();           ///< Test N5

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    /// \warning: Each test case builds on the output of the previous one.
    /// All tests shall be run exactly in the order as they are listed here:
//    void utTestServerTypeStringLookup();     ///< Test N1
//    void utTestReferenceGeneration();        ///< Test N2
//    void utTestDisconnectSignal();           ///< Test N3
//    void utTestWorkFunctions();              ///< Test N4
//    void utTestBadInputHandling();           ///< Test N5

private:

    void CheckForNull();

private:

    NetworkServer        *m_myServer;             ///< the class under test
    NetworkServerDevice  *m_myNetworkDevice;      ///< the derived class
    ConnectionManager    *m_myConnectionManager;  ///< helper class

    bool               m_FlagClientConnected;     ///< flag indicating a connected client
    bool               m_FlagClientDisconnected;  ///< flag indicating a disconnected client
    bool               m_FlagConnectionError;     ///< flag indicating a connection error
    bool               m_FlagDestroyConnection;   ///< flag indicating the destroyed connection

    QString            m_ClientName;              ///< name of client
    DisconnectType_t   m_DisconnectType;          ///< reason for disconneciton

};

} // end namespace

#endif // NETWORKBASE_TESTNETWORKSERVER_H
