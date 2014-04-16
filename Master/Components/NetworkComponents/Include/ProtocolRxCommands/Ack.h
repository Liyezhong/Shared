/****************************************************************************/
/*! \file Ack.h
 *
 *  \brief Acknowledge command definition.
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

#ifndef NETWORKBASE_ACK_H
#define NETWORKBASE_ACK_H

#include <Global/Include/AbstractFactory.h>
#include <NetworkComponents/Include/ProtocolRxCommand.h>
#include <NetworkComponents/Include/NetworkDevice.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief  This class implements a Ack command.
 *
 *      This class shall be used as a derived class for AbstractFactory of
 *      protocol command class objects. See description of the base class.
 */
/****************************************************************************/
class Ack : public ProtocolRxCommand {
    Q_OBJECT

    /// all test classes are friends:
    friend class TestProtocolCommand;

private:

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(Ack)
    DECLARE_REG_CLASS(ProtocolRxCommand, Ack);   ///< Declaration of static registration instance.

public:

    Ack();
    ~Ack();
    bool Execute();
    static bool SendAcknowledge(NetworkDevice *pNd, const QString &Ref, const QString &CmdName, const QString &Status);

}; // command

} // end namespace NetworkBase

#endif // NETWORKBASE_ACK_H
