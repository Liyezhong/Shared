/****************************************************************************/
/*! \file SetDateAndTime.h
 *
 *  \brief SetDateAndTime command definition.
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

#ifndef NETLAYER_SETDATEANDTIME_H
#define NETLAYER_SETDATEANDTIME_H

#include <NetworkComponents/Include/ProtocolRxCommand.h>
#include <NetworkComponents/Include/ProtocolTxCommands/DateAndTime.h>

namespace NetworkBase {

/// XML attributes processed by this command:
const QStringList SetDateAndTimeAttr = QStringList() << "dt";

/****************************************************************************/
/*!
 *  \brief  This class implements a SetDateAndTime command.
 *
 *      This class shall be used as a derived class for AbstractFactory of
 *      protocol command class objects. See description of the base class.
 */
/****************************************************************************/

class SetDateAndTime : public ProtocolRxCommand {
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
    Q_DISABLE_COPY(SetDateAndTime)
    DECLARE_REG_CLASS(ProtocolRxCommand, SetDateAndTime);   ///< Declaration of static registration instance.

public:

    SetDateAndTime();
    ~SetDateAndTime();
    bool Execute();

}; // command

} // end namespace

#endif
