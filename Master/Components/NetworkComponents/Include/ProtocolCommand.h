/****************************************************************************/
/*! \file ProtocolCommand.h
 *
 *  \brief ProtocolCommand base class definition.
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

#ifndef NETWORKBASE_PROTOCOLCOMMAND_H
#define NETWORKBASE_PROTOCOLCOMMAND_H

#include <Global/Include/AbstractFactory.h>
#include <QDomDocument>

namespace NetworkBase {

class NetworkDevice;

/****************************************************************************/
/*!
 *  \brief  This is a Base class for protocol commands.
 *
 *      This class shall be used as a base class for AbstractFactory of
 *      protocol command's derived class objects.
 *
 *      Use DEFINE_REG_CLASS(BaseProtocolCommand, DerivedProtocolCommand) macro
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

class ProtocolCommand : public QObject {
    Q_OBJECT

    /// test classes are all friends:
    friend class TestNetworkServerDevice;
    friend class TestNetworkClientDevice;
    friend class TestProtocolCommand;

public:

    ProtocolCommand();
    ~ProtocolCommand();

public:

    virtual QString GetName();

    /****************************************************************************/
    /*!
     *  \brief   Call this function to execute the command
     *
     *  \warning Implement this function in the derived class!
     *
     *  \return  TRUE if execution succeeded, FALSE otherwise
     *
     ****************************************************************************/
    virtual bool Execute() = 0;


protected:

    /// command name
    QString          m_myName;
    /// pointer to the device, which uses the command
    NetworkDevice*   m_myDevice;
    /// incoming/outgoing message as XML document
    QDomDocument     m_myMessage;

}; // ProtocolCommand

} // end namespace NetworkBase

#endif // NETWORKBASE_PROTOCOLCOMMAND_H
