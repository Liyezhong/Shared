/****************************************************************************/
/*! \file TestProtocolTxCommand.h
 *
 *  \brief Definition file for class TestProtocolTxCommand.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 14.02.2011
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

#ifndef NETWORKBASE_TESTPROTOCOLTXCOMMAND_H
#define NETWORKBASE_TESTPROTOCOLTXCOMMAND_H

#include <NetworkComponents/Include/ProtocolTxCommands/HeartBeat.h>
#include <NetworkComponents/Include/ProtocolTxCommand.h>
#include <NetworkComponents/Include/ProtocolTxCommands/DateAndTime.h>
#include <NetworkComponents/Include/NetworkServerDevice.h>

namespace NetworkBase {


/****************************************************************************/
/**
 * \brief ProtocolTxCommand is virtual. This is its test realization.
 */
/****************************************************************************/
class TxCommand : public ProtocolTxCommand
{
public:
    inline TxCommand() :
            ProtocolTxCommand::ProtocolTxCommand() {}
    inline bool Execute() {return true;}
    inline bool ExecuteWithParameter(const QString &s) {
        return ProtocolTxCommand::ExecuteWithParameter(s);
    }
    inline bool ExecuteWithParameters(NetMessageType_t type, const QByteArray &ba) {
        return ProtocolTxCommand::ExecuteWithParameters(type, ba);
    }
};

/****************************************************************************/
/**
 * \brief Test class for ProtocolCommand class.
 */
/****************************************************************************/
class TestProtocolTxCommand: public QObject
{
  Q_OBJECT

public:

    TestProtocolTxCommand();
    ~TestProtocolTxCommand();

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    /// \warning: Each test case builds on the output of the previous one.
    /// All tests shall be run exactly in the order as they are listed here:
    void utTestCommandFunctions();      ///< Test N1
    void utTestBadInputHandling();      ///< Test N2

private:

    void CheckForNull();

private:

    TxCommand            *m_TxCommand;  ///< the class to test
    HeartBeat            *m_HeartBeat;  ///< helper command class
    NetworkServerDevice  *m_myDevice;   ///< helper network class
    DateAndTime          *m_DateTime;   ///< helper command class
};

} // end namespace

#endif // NETWORKBASE_TESTPROTOCOLTXCOMMAND_H
