/****************************************************************************/
/*! \file ProtocolTxCommand.h
 *
 *  \brief ProtocolTxCommand base class definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 25.03.2011
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

#ifndef NETWORKBASE_PROTOCOLTXCOMMAND_H
#define NETWORKBASE_PROTOCOLTXCOMMAND_H

#include <Global/Include/AbstractFactory.h>
#include <NetworkComponents/Include/ProtocolCommand.h>
#include <NetworkComponents/Include/NetworkDevice.h>

#include <QStringList>
#include <QDomDocument>
#include <QTimer>

namespace NetworkBase {

class NetworkDevice;

const quint16 ACK_TIMEOUT = 3000; ///< communication timeout for Acks.

/****************************************************************************/
/*!
 *  \brief  This is a Base class for outgoing protocol commands.
 *
 *      This class shall be used as a base class for AbstractFactory of
 *      protocol command's derived class objects.
 *
 *      Use DEFINE_REG_CLASS(BaseProtocolTxCommand, DerivedProtocolTxCommand) macro
 *      in this class's implementation file to register a particular
 *      derived class. Thus it will be possible to instantiate a particular
 *      command by using its name as a string. The purpose of this
 *      "complication" is to make code maintenance much easier. Because we
 *      avoid giant switch-case statements OR need for loads of individual
 *      functions (and corresponding table of function pointers) in order
 *      to instantiate particular commands.
 *
 */
/****************************************************************************/
class ProtocolTxCommand : public ProtocolCommand {
    Q_OBJECT

    /// test classes are all friends:
    friend class TestNetworkServerDevice;
    friend class TestNetworkClientDevice;
    friend class TestProtocolTxCommand;
    friend class TestProtocolCommand;

public:

    ProtocolTxCommand();
    virtual ~ProtocolTxCommand();

public:

    virtual bool Initialize(const QString &, quint32 , QObject *);
    virtual bool Initialize(const QString &, quint32 , Global::tRefType, const QByteArray &, QObject *);
    static void LinkerTamer();
    virtual void DismissCommand();
    virtual QString GetReference();
    virtual void HandleAck(const QString &);

    /****************************************************************************/
    /*!
     *  \brief   Call this function to execute the command
     *
     *  \warning Implement this function in the derived class!
     *
     *  \return  TRUE if execution succeeded, FALSE otherwise
     *
     ****************************************************************************/
    virtual bool Execute();

protected slots:

    virtual void HandleAckTimeout();

protected:

    virtual bool ExecuteWithParameter(const QString &);
    virtual bool ExecuteWithParameters(NetMessageType_t type, const QByteArray &ba);

protected:

    /// command reference
    quint32          m_myRef;
    /// timer to guard acknowledge timeout
    QTimer           m_myTimer;
    /// payload as byte array
    QByteArray       m_myPayloadArray;
    /// application reference to be able to report a transmission error or timeout
    Global::tRefType m_myAppRef;

}; // ProtocolTxCommand

typedef Global::AbstractFactory<ProtocolTxCommand> BaseTxPCmd;    ///< Typedef for easier usage.

} // end namespace NetworkBase

#endif // NETWORKBASE_PROTOCOLTXCOMMAND_H
