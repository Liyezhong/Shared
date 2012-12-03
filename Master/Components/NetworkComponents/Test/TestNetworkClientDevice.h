/****************************************************************************/
/*! \file TestNetworkClientDevice.h
 *
 *  \brief Definition file for class TestNetworkClientDevice.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.02.2011
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

#ifndef NETWORKBASE_TESTNETWORKCLIENTDEVICE_H
#define NETWORKBASE_TESTNETWORKCLIENTDEVICE_H

#include <NetworkComponents/Include/NetworkClientDevice.h>

namespace NetworkBase {

const QString UT_CLIENT_IP = "127.0.0.12";  ///< test ip to connect to
const QString UT_CLIENT_PORT = "7779";      ///< test port to connect to

/****************************************************************************/
/**
 * \brief Test class for NetworkClientDevice & NetworkDevice classes.
 */
/****************************************************************************/
class TestNetworkClientDevice: public QObject
{
  Q_OBJECT

public:

    TestNetworkClientDevice();
    ~TestNetworkClientDevice();

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
    void utTestWorkFunctions();               ///< Test N1
    void utTestBadInputHandling();            ///< Test N2

private:

    void CheckForNull();

private:

    NetworkClientDevice     *m_myDevice;  ///< the class to test
    QString m_String;                     ///< variable to hold client name
    bool m_FlagPeerConnected;             ///< flag to indicate peer connection
    bool m_FlagPeerDisconnected;          ///< flag to indicate peer disconnection
};

} // end namespace

#endif // NETWORKBASE_TESTNETWORKCLIENTDEVICE_H
