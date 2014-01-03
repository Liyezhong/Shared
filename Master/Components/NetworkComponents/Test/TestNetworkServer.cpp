/****************************************************************************/
/*! \file TestNetworkServer.cpp
 *
 *  \brief Implementation file for class TestNetworkServer.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 10.02.2011
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

#include <QTest>
#include <NetworkComponents/Test/TestNetworkServer.h>
#include <Global/Include/SystemPaths.h>

namespace NetworkBase {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestNetworkServer::TestNetworkServer() :
            m_myServer(NULL),
            m_myNetworkDevice(NULL),
            m_myConnectionManager(NULL),
            m_FlagClientConnected(false),
            m_FlagClientDisconnected(false),
            m_FlagConnectionError(false),
            m_FlagDestroyConnection(false),
            m_ClientName(""),
            m_DisconnectType((DisconnectType_t)UNDEFINED_NO_INIT)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestNetworkServer::~TestNetworkServer()
{
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkServer::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkServer::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkServer::initTestCase()
{
    // initialize settings path:
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");
    // create server
    m_myServer = new NetworkServer(NSE_TYPE_NORMAL_GUI, this);
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)0);
    QCOMPARE(m_myServer->m_connectionCounterRolledOver, (ConnectionCounterType_t)NS_NO);
    QCOMPARE(m_myServer->m_myIp, (QString)"");
    QCOMPARE(m_myServer->m_myPort, (QString)"");
    QCOMPARE(m_myServer->m_myType, (NetworkServerType_t)NSE_TYPE_NORMAL_GUI);
    QCOMPARE(m_myServer->m_authReq, (QString)"");
    QCOMPARE(m_myServer->m_authConf, (QString)"");
    QCOMPARE(m_myServer->GetServerType(), (NetworkServerType_t)NSE_TYPE_NORMAL_GUI);
    QCOMPARE(m_myServer->FetchServerTypeString(), (QString)NSS_TYPE_GUI);
    QCOMPARE(m_myServer->m_connectionsList.size(), (int)0);
    QCOMPARE(m_myServer->m_registeredHandlersList.size(), (int)0);
    QCOMPARE(m_myServer->m_availableConnections.size(), (int)0);
    QCOMPARE(m_myServer->m_takenConnections.size(), (int)0);

    // connect test slots:
    if (!QObject::connect(m_myServer, SIGNAL(ClientDisconnected(const QString &)),
                          this, SLOT(TestClientDisconnected(const QString &)))) {
        QFAIL("TestNetworkServer: cannot connect ClientDisconnected signal !");
    }
    if (!QObject::connect(m_myServer, SIGNAL(ConnectionError(NetworkBase::DisconnectType_t, const QString &)),
                          this, SLOT(TestConnectionError(NetworkBase::DisconnectType_t, const QString &)))) {
        QFAIL("TestNetworkServer: cannot connect TestConnectionError signal !");
    }
    if (!QObject::connect(m_myServer, SIGNAL(DestroyConnection(NetworkBase::DisconnectType_t)),
                          this, SLOT(TestDestroyConnection(NetworkBase::DisconnectType_t)))) {
        QFAIL("TestNetworkServer: cannot connect DestroyConnection signal !");
    }
    if (!QObject::connect(m_myServer, SIGNAL(ClientConnected(const QString &)),
                          this, SLOT(TestClientConnected(const QString &)))) {
        QFAIL("TestNetworkServer: cannot connect ClientConnected signal !");
    }

    // initialize server
    QCOMPARE(m_myServer->InitializeServer(), (NetworkServerErrorType_t)NS_ALL_OK);
    QVERIFY(m_myServer->m_myIp != "");
    QVERIFY(m_myServer->m_myPort != "");
    QVERIFY(m_myServer->m_authReq != "");
    QVERIFY(m_myServer->m_authReply.isNull() != true);
    QVERIFY(m_myServer->m_authConf != "");
    QVERIFY(m_myServer->m_availableConnections.size() != 0);

    // create message handler:
    QString path =  Global::SystemPaths::Instance().GetSettingsPath() + "/Communication";
    m_myNetworkDevice = new NetworkServerDevice(NSE_TYPE_NORMAL_GUI, (QString)"Colorado Device GUI", path,  this);
    QCOMPARE(m_myNetworkDevice->InitializeDevice(), true);

    // test message handler registration:
    // "Colorado Device GUI" is the real client name read from config XML file!!!
    QCOMPARE(m_myServer->RegisterMessageHandler(m_myNetworkDevice, (QString)"Colorado Device GUI"), true);

    m_myServer->GetFreeConnectionNumber();
    QCOMPARE(m_myServer->GetFreeConnectionNumber(),quint32(0));

    // check reference generation:
    QCOMPARE(m_myServer->GenerateReference(), true);
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)1);
    QCOMPARE(m_myServer->m_connectionCounterRolledOver, (ConnectionCounterType_t)NS_NO);
    // simulate max allowed number of connections taken:
    m_myServer->m_connectionCounter = NS_MAX_CONNECTIONS;
    QCOMPARE(m_myServer->GenerateReference(), true);
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)0);
    // rolled-over flag shall be set now:
    QCOMPARE(m_myServer->m_connectionCounterRolledOver, (ConnectionCounterType_t)NS_YES);
    QCOMPARE(m_myServer->GenerateReference(), true);
    // refs shall be given from the list of free refs now:
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)0);
    // put some dummy connections in:
    m_myServer->m_connectionsList.insert((quint32)0, NULL);
    m_myServer->m_connectionsList.insert((quint32)1, NULL);
    m_myServer->m_connectionsList.insert((quint32)2, NULL);
    m_myServer->m_connectionsList.insert((quint32)4, NULL);
    // fetch free reference number. it shall be "3", not "5":
    QCOMPARE(m_myServer->GenerateReference(), true);
    QVERIFY(m_myServer->m_connectionCounter != (quint32)5);
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)3);
    // reset rolled-over flag:
    m_myServer->m_connectionCounterRolledOver = (ConnectionCounterType_t)NS_NO;
    QCOMPARE(m_myServer->GenerateReference(), true);
    // reference shall be generated normally (was: 3, now shall be: 4):
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)4);

}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkServer::cleanupTestCase()
{
    // cleanup:
    delete m_myServer;
    delete m_myNetworkDevice;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestNetworkServer::CheckForNull()
{
    if ((m_myServer == NULL) ||
        (m_myNetworkDevice == NULL)) {
        QFAIL("TestNetworkServer: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test server type string lookup.
 */
/****************************************************************************/
void TestNetworkServer::utTestServerTypeStringLookup()
{
    CheckForNull();

    // test server type lookup function:
    m_myServer->m_myType = (NetworkServerType_t)NSE_TYPE_UNKNOWN;
    QCOMPARE(m_myServer->FetchServerTypeString(), (QString)NSS_TYPE_INVALID);
    m_myServer->m_myType = (NetworkServerType_t)NSE_TYPE_AXEDA;
    QCOMPARE(m_myServer->FetchServerTypeString(), (QString)NSS_TYPE_AXEDA);
    m_myServer->m_myType = (NetworkServerType_t)NSE_TYPE_SERVICE;
    QCOMPARE(m_myServer->FetchServerTypeString(), (QString)NSS_TYPE_SERVICE);
    m_myServer->m_myType = (NetworkServerType_t)NSE_TYPE_SEPIA;
    QCOMPARE(m_myServer->FetchServerTypeString(), (QString)NSS_TYPE_SEPIA);
    m_myServer->m_myType = (NetworkServerType_t)NSE_TYPE_INVALID;
    QCOMPARE(m_myServer->FetchServerTypeString(), (QString)NSS_TYPE_INVALID);
    // get back to the right server type (as initialized):
    m_myServer->m_myType = (NetworkServerType_t)NSE_TYPE_NORMAL_GUI;
    QCOMPARE(m_myServer->FetchServerTypeString(), (QString)NSS_TYPE_GUI);
}

/****************************************************************************/
/**
 * \brief Test generation of the connection reference.
 */
/****************************************************************************/
void TestNetworkServer::utTestReferenceGeneration()
{
    CheckForNull();

    // check reference generation:
    QCOMPARE(m_myServer->GenerateReference(), true);
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)1);
    QCOMPARE(m_myServer->m_connectionCounterRolledOver, (ConnectionCounterType_t)NS_NO);
    // simulate max allowed number of connections taken:
    m_myServer->m_connectionCounter = NS_MAX_CONNECTIONS;
    QCOMPARE(m_myServer->GenerateReference(), true);
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)0);
    // rolled-over flag shall be set now:
    QCOMPARE(m_myServer->m_connectionCounterRolledOver, (ConnectionCounterType_t)NS_YES);
    QCOMPARE(m_myServer->GenerateReference(), true);
    // refs shall be given from the list of free refs now:
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)0);
    // put some dummy connections in:
    m_myServer->m_connectionsList.insert((quint32)0, NULL);
    m_myServer->m_connectionsList.insert((quint32)1, NULL);
    m_myServer->m_connectionsList.insert((quint32)2, NULL);
    m_myServer->m_connectionsList.insert((quint32)4, NULL);
    // fetch free reference number. it shall be "3", not "5":
    QCOMPARE(m_myServer->GenerateReference(), true);
    QVERIFY(m_myServer->m_connectionCounter != (quint32)5);
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)3);
    // reset rolled-over flag:
    m_myServer->m_connectionCounterRolledOver = (ConnectionCounterType_t)NS_NO;
    QCOMPARE(m_myServer->GenerateReference(), true);
    // reference shall be generated normally (was: 3, now shall be: 4):
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)4);
}

/****************************************************************************/
/**
 * \brief Test emission of the disconnection signal.
 */
/****************************************************************************/
void TestNetworkServer::utTestDisconnectSignal()
{
    CheckForNull();

    // check signal emission in disconnect cases:
    m_myServer->HandleDisconnectReason((DisconnectType_t)SOCKET_DISCONNECT, (QString)"DummyClient_1");
    QCOMPARE(m_FlagClientDisconnected, true);
    QCOMPARE(m_ClientName, (QString)"DummyClient_1");
    // reset flag:
    m_FlagClientDisconnected = false;
    m_myServer->HandleDisconnectReason((DisconnectType_t)USER_REQUEST, (QString)"DummyClient_2");
    QCOMPARE(m_FlagClientDisconnected, true);
    QCOMPARE(m_ClientName, (QString)"DummyClient_2");
    // reset flag:
    m_FlagClientDisconnected = false;
    // simulate error:
    m_myServer->HandleDisconnectReason((DisconnectType_t)SOCKET_ERROR, (QString)"DummyClient_3");
    QCOMPARE(m_DisconnectType, (DisconnectType_t)SOCKET_ERROR);
    QCOMPARE(m_ClientName, (QString)"DummyClient_3");
    QCOMPARE(m_FlagConnectionError, true);
    // reset the flag:
    m_FlagConnectionError = false;
    m_myServer->HandleDisconnectReason((DisconnectType_t)NO_CONNECTION_IN_HASH, (QString)"DummyClient_4");
    QCOMPARE(m_DisconnectType, (DisconnectType_t)NO_CONNECTION_IN_HASH);
    QCOMPARE(m_ClientName, (QString)"DummyClient_4");
    QCOMPARE(m_FlagConnectionError, true);
    // reset the flag:
    m_FlagConnectionError = false;
    m_ClientName = "";
    m_DisconnectType = (DisconnectType_t)0;

    // check if socket kill generates the signal:
    m_myServer->DisconnectTheSocket(NO_AVAILABLE_CONNECTIONS, (int)14);
    QCOMPARE(m_DisconnectType, (DisconnectType_t)NO_AVAILABLE_CONNECTIONS);
    QCOMPARE(m_ClientName, NO_CLIENT);
    QCOMPARE(m_FlagConnectionError, true);
    // reset the flag:
    m_FlagConnectionError = false;
    m_ClientName = "";
    m_DisconnectType = (DisconnectType_t)0;
}

/****************************************************************************/
/**
 * \brief Test server operation.
 */
/****************************************************************************/
void TestNetworkServer::utTestWorkFunctions()
{
    CheckForNull();

    // start server:
    QCOMPARE(m_myServer->StartServer(), (NetworkServerErrorType_t)NS_ALL_OK);
    // start 2nd server on the same IP/Port, it shall fail:
    QCOMPARE(m_myServer->StartServer(), (NetworkServerErrorType_t)NS_LISTEN_FAILED);

    // try to simulate connection:
    m_myServer->m_connectionsList.clear();
    QCOMPARE(m_myServer->m_connectionsList.isEmpty(), true);
    m_myServer->incomingConnection((int)15);
    // give process some time:
    QTest::qWait(100);
    // since we use a dummy socket descriptor, incoming connection will fail.
    // check that disconnect signal got generated:
    QCOMPARE(m_DisconnectType, (DisconnectType_t)SOCKET_ERROR);
    QCOMPARE(m_ClientName, (QString)"");
    QCOMPARE(m_FlagConnectionError, true);
    // reset flags:
    m_FlagConnectionError = false;
    m_DisconnectType = (DisconnectType_t)0;


    // simulate an existing connection:
    m_myConnectionManager = new ConnectionManager(m_myServer->m_availableConnections, (quint32)22, m_myServer->m_authReq, \
                                                        m_myServer->m_authConf, m_myServer->m_authReply, this);
    m_myServer->m_connectionsList.insert((quint32)22, m_myConnectionManager);
    QCOMPARE(m_myServer->m_takenConnections.size(), (int)0);
    int ACSize = m_myServer->m_availableConnections.size();
    // register simulated connection:
    m_myServer->RegisterConnection((quint32)22, (QString)"Colorado Device GUI");
    // check that connection moved from "available" to "taken" list:
    QCOMPARE(m_myServer->m_takenConnections.size(), (int)1);
    QCOMPARE(m_myServer->m_availableConnections.size(), (ACSize-1));
    // destroy connection:
    m_myServer->DestroyManager((quint32)22, (QString)"Colorado Device GUI", (DisconnectType_t)UNKNOWN_ERROR);
    // check that connection moved from "taken" to "available" list:
    QCOMPARE(m_myServer->m_takenConnections.size(), (int)0);
    QCOMPARE(m_myServer->m_availableConnections.size(), ACSize);
    // check that disconnect happend with UNKNOWN_ERROR code:
    QCOMPARE(m_DisconnectType, (DisconnectType_t)UNKNOWN_ERROR);
    QCOMPARE(m_ClientName, (QString)"Colorado Device GUI");
    QCOMPARE(m_FlagConnectionError, true);
    // reset flags:
    m_FlagConnectionError = false;
    m_DisconnectType = (DisconnectType_t)0;
    m_ClientName = "";

    // try to register an allowed connection, but without connected client:
    m_myServer->RegisterConnection((quint32)12714, (QString)"Colorado Device GUI");
    // check that disconnect happend with NULL_POINTER_IN_HASH code:
    QCOMPARE(m_DisconnectType, (DisconnectType_t)NULL_POINTER_IN_HASH);
    QCOMPARE(m_ClientName, (QString)"Colorado Device GUI");
    QCOMPARE(m_FlagConnectionError, true);
    // reset flags:
    m_FlagConnectionError = false;
    m_DisconnectType = (DisconnectType_t)0;
    m_ClientName = "";
}
/****************************************************************************/
/**
 * \brief Test to get free connection number.
 */
/****************************************************************************/
void TestNetworkServer::utTestGetFreeConnectionNumber()
{
    m_myServer->GetFreeConnectionNumber();
    QCOMPARE(m_myServer->GetFreeConnectionNumber(),quint32(3));
}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestNetworkServer::utTestBadInputHandling()
{
    // pack some more dummy connections to the hash and kill the server:
    m_myConnectionManager = new ConnectionManager(m_myServer->m_availableConnections, (quint32)22, m_myServer->m_authReq, \
                                                        m_myServer->m_authConf, m_myServer->m_authReply, this);
    m_myServer->m_connectionsList.insert((quint32)22, m_myConnectionManager);
    m_myServer->m_connectionsList.insert((quint32)1234, NULL);
    m_myServer->m_connectionsList.insert((quint32)548, NULL);
    // run another function before killing the object:
    m_myServer->DestroyAllConnections();
    delete m_myServer;

    // initialize settings path:
    Global::SystemPaths::Instance().SetSettingsPath("SomeDummyPath");

    // create server
    m_myServer = new NetworkServer(NSE_TYPE_UNKNOWN, this);
    // initialize server, it shall fail:
    QCOMPARE(m_myServer->InitializeServer(), (NetworkServerErrorType_t)NS_UNKNOWN_APP_REQUEST);
    // set a correct server type:
    m_myServer->m_myType = NSE_TYPE_SERVICE;
    // initialize server again, it shall fail again:
    QCOMPARE(m_myServer->InitializeServer(), (NetworkServerErrorType_t)NS_LOAD_SETTINGS_FAILED);
    QCOMPARE(m_myServer->m_connectionCounter, (quint32)0);
    QCOMPARE(m_myServer->m_connectionCounterRolledOver, (ConnectionCounterType_t)NS_NO);
    QCOMPARE(m_myServer->m_myIp, (QString)"");
    QCOMPARE(m_myServer->m_myPort, (QString)"");
    QCOMPARE(m_myServer->m_myType, (NetworkServerType_t)NSE_TYPE_SERVICE);
    QCOMPARE(m_myServer->m_authReq, (QString)"");
    QCOMPARE(m_myServer->m_authConf, (QString)"");
    QCOMPARE(m_myServer->GetServerType(), (NetworkServerType_t)NSE_TYPE_SERVICE);
    QCOMPARE(m_myServer->FetchServerTypeString(), (QString)NSS_TYPE_SERVICE);
    QCOMPARE(m_myServer->m_connectionsList.size(), (int)0);
    QCOMPARE(m_myServer->m_registeredHandlersList.size(), (int)0);
    QCOMPARE(m_myServer->m_availableConnections.size(), (int)0);
    QCOMPARE(m_myServer->m_takenConnections.size(), (int)0);

    // connect test slots:
    if (!QObject::connect(m_myServer, SIGNAL(ClientDisconnected(const QString &)),
                          this, SLOT(TestClientDisconnected(const QString &)))) {
        QFAIL("TestNetworkServer: cannot connect ClientDisconnected signal !");
    }
    if (!QObject::connect(m_myServer, SIGNAL(ConnectionError(NetworkBase::DisconnectType_t, const QString &)),
                          this, SLOT(TestConnectionError(NetworkBase::DisconnectType_t, const QString &)))) {
        QFAIL("TestNetworkServer: cannot connect TestConnectionError signal !");
    }
    if (!QObject::connect(m_myServer, SIGNAL(DestroyConnection(NetworkBase::DisconnectType_t)),
                          this, SLOT(TestDestroyConnection(NetworkBase::DisconnectType_t)))) {
        QFAIL("TestNetworkServer: cannot connect DestroyConnection signal !");
    }
    if (!QObject::connect(m_myServer, SIGNAL(ClientConnected(const QString &)),
                          this, SLOT(TestClientConnected(const QString &)))) {
        QFAIL("TestNetworkServer: cannot connect ClientConnected signal !");
    }

    // try to register MessageHandler with dummy client:
    QCOMPARE(m_myServer->RegisterMessageHandler(NULL, (QString)"DummyClient"), false);

    // try to start a not initialized server:
    QCOMPARE(m_myServer->StartServer(), (NetworkServerErrorType_t)NS_LISTEN_FAILED);
    m_myServer->incomingConnection((int)32);
    // give process some time:
    QTest::qWait(100);
    // list of available connections is empty.
    // check that disconnect happend with NO_AVAILABLE_CONNECTIONS code:
    QCOMPARE(m_DisconnectType, (DisconnectType_t)NO_AVAILABLE_CONNECTIONS);
    QCOMPARE(m_ClientName, NO_CLIENT);
    QCOMPARE(m_FlagConnectionError, true);
    // reset flags:
    m_FlagConnectionError = false;
    m_DisconnectType = (DisconnectType_t)0;
    m_ClientName = "";

    // try to disconnect non-existent connection:
    m_myServer->DestroyManager((quint32)12714, (QString)"Dummy1", (DisconnectType_t)NO_CONNECTION_IN_HASH);
    // check that disconnect happend with NO_CONNECTION_IN_HASH code:
    QCOMPARE(m_DisconnectType, (DisconnectType_t)NO_CONNECTION_IN_HASH);
    QCOMPARE(m_ClientName, (QString)"Dummy1");
    QCOMPARE(m_FlagConnectionError, true);
    // reset flags:
    m_FlagConnectionError = false;
    m_DisconnectType = (DisconnectType_t)0;
    m_ClientName = "";

    // try to register a not-allowed connection:
    m_myServer->RegisterConnection((quint32)12714, (QString)"Dummy1");
    // check that disconnect happend with NO_AVAILABLE_CONNECTIONS code:
    QCOMPARE(m_DisconnectType, (DisconnectType_t)NO_AVAILABLE_CONNECTIONS);
    QCOMPARE(m_ClientName, (QString)"Dummy1");
    QCOMPARE(m_FlagConnectionError, true);
    // reset flags:
    m_FlagConnectionError = false;
    m_DisconnectType = (DisconnectType_t)0;
    m_ClientName = "";
}

/****************************************************************************/
/**
 *  \brief Test slot for client conneciton.
 *
 *  \param  str1 = client name
 */
/****************************************************************************/
void TestNetworkServer::TestClientConnected(const QString &str1)
{
    m_ClientName = str1;
    m_FlagClientConnected = true;
}

/****************************************************************************/
/**
 *  \brief Test slot for client disconneciton.
 *
 *  \param  str1 = client name
 */
/****************************************************************************/
void TestNetworkServer::TestClientDisconnected(const QString &str1)
{
    m_ClientName = str1;
    m_FlagClientDisconnected = true;
}

/****************************************************************************/
/**
 *  \brief Test slot for conneciton error.
 *
 *  \param  dt = disconnect reason
 *  \param  str1 = client name
 */
/****************************************************************************/
void TestNetworkServer::TestConnectionError(NetworkBase::DisconnectType_t dt, const QString &str1)
{
    m_DisconnectType = dt;
    m_ClientName = str1;
    m_FlagConnectionError = true;
}

/****************************************************************************/
/**
 *  \brief Test slot for conneciton destruction.
 *
 *  \param  dt = disconnect reason
 */
/****************************************************************************/
void TestNetworkServer::TestDestroyConnection(NetworkBase::DisconnectType_t dt)
{
    m_DisconnectType = dt;
    m_FlagDestroyConnection = true;
}

} // end namespace

QTEST_MAIN(NetworkBase::TestNetworkServer)
