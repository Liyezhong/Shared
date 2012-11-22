/****************************************************************************/
/*! \file HeartBeat.h
 *
 *  \brief HeartBeat command definition.
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
#ifndef NETWORKBASE_HEARTBEAT_H
#define NETWORKBASE_HEARTBEAT_H

#include <NetworkComponents/Include/ProtocolTxCommand.h>
//#include <NetworkComponents/Include/NetworkDevice.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief  This class implements a HeartBeat command.
 *
 *      This class shall be used as a derived class for AbstractFactory of
 *      protocol command class objects. See description of the base class.
 */
/****************************************************************************/
class HeartBeat : public ProtocolTxCommand {
    Q_OBJECT

    ///< test classes are all friends:
    friend class TestNetworkServerDevice;
    friend class TestNetworkClientDevice;
    friend class TestProtocolCommand;

private:

    DECLARE_REG_CLASS(ProtocolTxCommand, HeartBeat);   ///< Declaration of static registration instance.

public:

    HeartBeat();
    ~HeartBeat();
    bool Execute();
    quint16 GetSentNumber();
    void HandleAck(const QString &status); ///< needs to be re-implemented for this command

private slots:

    void HandleAckTimeout();               ///< needs to be re-implemented for this command

private:

    /// up-counting Heartbeat number
    static quint16        m_myHeartBeatNumber;
    /// created command shall remember the number it has sent to the peer
    quint16               m_myActualNumber;

}; // command

} // end namespace NetworkBase

#endif // NETWORKBASE_HEARTBEAT_H
