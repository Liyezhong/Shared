/****************************************************************************/
/*! \file TestNetworkServerDevice.h
 *
 *  \brief Definition file for class TestNetworkServerDevice.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 11.02.2011
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

#ifndef NETWORKBASE_TESTNETWORKSERVERDEVICE_H
#define NETWORKBASE_TESTNETWORKSERVERDEVICE_H

#include <NetworkComponents/Include/NetworkServerDevice.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief  This is a derived class for testing the NetworkServerDevice class.
 */
/****************************************************************************/
class DerivedNetworkServerDevice : public NetworkServerDevice
{
public:
    /// Constructor
    ///  \param[in] stype = type of server to create
    ///  \param[in] client = type of client to accept
    ///  \param[in] path = path to settings folder
    ///  \param[in] doctype = type of configuration file to load
    ///  \param[in] pParent = pointer to parent
    DerivedNetworkServerDevice(NetworkServerType_t stype, const QString & client,
                               const QString & path, /*const QString & doctype,*/ QObject * pParent) :
            NetworkServerDevice(stype, client, path, /*doctype,*/ pParent),
            m_FlagCommandExecuted(false)  {}
    /// Destructor
    virtual ~DerivedNetworkServerDevice() {}
    /// Run function
    void CommandExecuted()
    {
        qDebug() << "DerivedNetworkServerDevice: CommandExecuted called !";
        m_FlagCommandExecuted = true;
    }
    bool m_FlagCommandExecuted;  ///< flag to indicate that the Run function was called
};

/****************************************************************************/
/**
 * \brief Test class for NetworkServerDevice & NetworkDevice classes.
 */
/****************************************************************************/
class TestNetworkServerDevice: public QObject
{
  Q_OBJECT

public:

    TestNetworkServerDevice();
    ~TestNetworkServerDevice();

public slots:

    void TestSigPeerConnected(const QString &);
    void TestSigPeerDisconnected(const QString &);

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    /// \warning: Each test case builds on the output of the previous one.
    /// All tests shall be run exactly in the order as they are listed here:
    void utTestCommandsManagementInterface(); ///< Test N1
    void utTestCommandsCreationInterface();   ///< Test N2
    void utTestHeartBeatFunctions();          ///< Test N3
    void utTestWorkFunctions();               ///< Test N4
    void utTestBadInputHandling();            ///< Test N5

private:

    void CheckForNull();
    bool GetMsgAsDomDoc(const QString &message, QDomDocument *ddoc);
    bool GetMsgAsByteArray(const QString &message, QByteArray *barr);

private:

    DerivedNetworkServerDevice *m_myDevice;  ///< class to test
    QString m_String;                        ///< test string
    bool m_FlagPeerConnected;                ///< flag indicating a connected client
    bool m_FlagPeerDisconnected;             ///< flag indicating a disconnected client
};

} // end namespace

#endif // NETWORKBASE_TESTNETWORKSERVERDEVICE_H
