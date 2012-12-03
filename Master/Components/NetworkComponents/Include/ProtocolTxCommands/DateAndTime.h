/****************************************************************************/
/*! \file DateAndTime.h
 *
 *  \brief DateAndTime command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 23.08.2011
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

#ifndef NETWORKBASE_DATEANDTIME_H
#define NETWORKBASE_DATEANDTIME_H

#include <NetworkComponents/Include/ProtocolTxCommand.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief  This class implements a DateAndTime command.
 *
 *      This class shall be used as a derived class for AbstractFactory of
 *      protocol command class objects. See description of the base class.
 */
/****************************************************************************/

class DateAndTime : public ProtocolTxCommand {
    Q_OBJECT

private:
    DECLARE_REG_CLASS(ProtocolTxCommand, DateAndTime);   ///< Declaration of static registration instance.

public:

    DateAndTime();
    ~DateAndTime();
    bool Execute();
    void HandleAck(const QString &status); ///< needs to be re-implemented for this command

private slots:

    void HandleAckTimeout();               ///< needs to be re-implemented for this command

}; // command

} // end namespace

#endif
