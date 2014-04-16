/****************************************************************************/
/*! \file HeartBeatClient.h
 *
 *  \brief HeartBeatClient command definition.
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
#ifndef NETWORKBASE_HEARTBEATCLIENT_H
#define NETWORKBASE_HEARTBEATCLIENT_H

#include <QDomDocument>
#include <Global/Include/AbstractFactory.h>
#include <NetworkComponents/Include/ProtocolRxCommand.h>
#include <NetworkComponents/Include/NetworkDevice.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief  This class implements a HeartBeatClient command.
 *
 *      This class shall be used as a derived class for AbstractFactory of
 *      protocol command class objects. See description of the base class.
 */
/****************************************************************************/
class HeartBeatClient : public ProtocolRxCommand {
    Q_OBJECT

    /// test classes are all friends:
    friend class TestNetworkServerDevice;
    friend class TestNetworkClientDevice;
    friend class TestProtocolCommand;

private:

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(HeartBeatClient)
    DECLARE_REG_CLASS(ProtocolRxCommand, HeartBeatClient);   ///< Declaration of static registration instance.

public:

    HeartBeatClient();
    ~HeartBeatClient();
    bool Execute();

}; // command

} // end namespace NetworkBase

#endif // NETWORKBASE_HEARTBEATCLIENT_H
