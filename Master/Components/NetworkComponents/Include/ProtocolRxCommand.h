/****************************************************************************/
/*! \file ProtocolRxCommand.h
 *
 *  \brief ProtocolRxCommand base class definition.
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

#ifndef NETWORKBASE_PROTOCOLRXCOMMAND_H
#define NETWORKBASE_PROTOCOLRXCOMMAND_H

#include <Global/Include/AbstractFactory.h>
#include <NetworkComponents/Include/ProtocolCommand.h>

#include <QStringList>
#include <QDomDocument>
#include <QTimer>

namespace NetworkBase {

/// data type representing the data tree of the incoming message
typedef struct {
    QStringList         DataitemsValues;    ///< attribute values of dataitems element
    QList<QStringList>  SubDataitemsValues; ///< attribute values of elements enclosed in dataitems element
} MessageDataTree_t;

/****************************************************************************/
/*!
 *  \brief  This is a Base class for incoming protocol commands.
 *
 *      This class shall be used as a base class for AbstractFactory of
 *      protocol command's derived class objects.
 *
 *      Use DEFINE_REG_CLASS(BaseProtocolRxCommand, DerivedProtocolRxCommand) macro
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

class ProtocolRxCommand : public ProtocolCommand {
    Q_OBJECT

    /// test classes are all friends:
    friend class TestNetworkServerDevice;
    friend class TestNetworkClientDevice;
    friend class TestProtocolRxCommand;
    friend class TestProtocolCommand;

public:

    ProtocolRxCommand();
    ProtocolRxCommand(const QStringList &attrs);
    ProtocolRxCommand(const QStringList &attrs, const QStringList &dattrs);
    virtual ~ProtocolRxCommand();

public:

    virtual bool Initialize(const QString &, QObject *);
    virtual bool Initialize(const QString &, const QDomDocument &, QObject *);
    virtual bool Initialize(const QString &, const QString &, const QByteArray &, QObject *);
    static void LinkerTamer();
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

    bool ExtractAttributeValues(QDomElement *Element, QStringList *Tags, QStringList *DataTags, QList<MessageDataTree_t> *Values);
    bool ExtractSubAttributeValues(QDomElement *Item, QStringList *DataTags, MessageDataTree_t *Values);
    bool ExtractMessageReference(QDomElement *Element, QString *Reference);
    bool ExtractMessageHeader(QDomElement *Element, QString *Ref, qint16 *NofItems);
    bool ExtractNofItems(QDomElement *Element, qint16 *NofItems, const QString &targettag, const QString &sourcetag);
    bool CheckStatus(const QString &status);

private:

    void PrintMessageDataTree();

protected:

    /// reference of the incoming message
    QString              m_myRef;
    /// number of dataitems in the incoming message
    qint16               my_NOfDataitems;
    /// incoming payload as byte array
    QByteArray           m_myPayloadArray;
    /// attributes used by this command
    QStringList          m_myAttributes;
    /// data attributes used by this command
    QStringList          m_myDataAttributes;
    /// data attribute values extracted from incoming message
    QList<MessageDataTree_t>   m_myMessageDataTree;

}; // ProtocolRxCommand

typedef Global::AbstractFactory<ProtocolRxCommand> BaseRxPCmd;    ///< Typedef for easier usage.

} // end namespace NetworkBase

#endif // NETWORKBASE_PROTOCOLRXCOMMAND_H
