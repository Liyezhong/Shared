/****************************************************************************/
/*! \file TestProtocolRxCommand.h
 *
 *  \brief Definition file for class TestProtocolRxCommand.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 31.03.2011
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

#ifndef NETWORKBASE_TESTPROTOCOLRXCOMMAND_H
#define NETWORKBASE_TESTPROTOCOLRXCOMMAND_H

#include <NetworkComponents/Include/ProtocolRxCommands/Ack.h>
#include <NetworkComponents/Include/ProtocolRxCommands/HeartBeatClient.h>
#include <NetworkComponents/Include/ProtocolRxCommands/HeartBeatServer.h>
#include <NetworkComponents/Include/ProtocolRxCommands/SetDateAndTime.h>
#include <NetworkComponents/Include/NetworkServerDevice.h>

namespace NetworkBase {


/****************************************************************************/
/**
 * \brief ProtocolRxCommand is virtual. This is its test realization.
 */
/****************************************************************************/
class RxCommand : public ProtocolRxCommand
{
public:
    /// constructor
    inline RxCommand() :
            ProtocolRxCommand::ProtocolRxCommand()  {}
    /// constructor
    ///  \param[in] attrs = list of data attributes of this command
    inline RxCommand(const QStringList &attrs) :
            ProtocolRxCommand::ProtocolRxCommand(attrs)   {}
    /// constructor
    ///  \param[in] attrs = list of data attributes of this command
    ///  \param[in] dattrs = list of data attributes of each data element of this command
    inline RxCommand(const QStringList &attrs, const QStringList &dattrs) :
            ProtocolRxCommand::ProtocolRxCommand(attrs, dattrs)    {}
    /// destructor
    inline ~RxCommand() {}
    /// run function
    inline bool Execute() {return true;}
};

/****************************************************************************/
/**
 * \brief Test class for ProtocolCommand class.
 */
/****************************************************************************/
class TestProtocolRxCommand: public QObject
{
  Q_OBJECT

public:

    TestProtocolRxCommand();
    ~TestProtocolRxCommand();

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    /// \warning: Each test case builds on the output of the previous one.
    /// All tests shall be run exactly in the order as they are listed here:
    void utTestCommandFunctions();           ///< Test N1
    void utTestBadInputHandling();           ///< Test N2

private:

    void CheckForNull();
    bool GetMsgAsDomDoc(const QString &message, QDomDocument *ddoc);

private:

    RxCommand            *m_RxCommand;  ///< the class to test
    NetworkServerDevice  *m_myDevice;   ///< helper network class
};

} // end namespace

#endif // NETWORKBASE_TESTPROTOCOLRXCOMMAND_H
