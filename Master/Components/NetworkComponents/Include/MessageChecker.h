/****************************************************************************/
/*! \file MessageChecker.h
 *
 *  \brief Definition of MessageChecker class.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 05.04.2011
 *   $Author:  $ Y.Novak
 *
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

#ifndef NETWORKBASE_MESSAGECHECKER_H
#define NETWORKBASE_MESSAGECHECKER_H

#include <QtNetwork>
#include <QtXml>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

#include <NetworkComponents/Include/MessageLoader.h>


namespace NetworkBase {

/// Enum for class return codes
typedef enum {
    CMH_ALL_OK = 1,            ///< operation successful
    CMH_CANNOT_LOAD_MESSAGES   ///< loading messages failed
} MessageCheckerErrorType_t;

/****************************************************************************/
/**
 * \brief Message Handler for communication with Network peer.
 *
 */
/****************************************************************************/
class MessageChecker : public QObject
{
    Q_OBJECT

public:

    MessageChecker(MessageLoaderType_t ptype, const QString & doctype, const QString & path, QObject *pParent = 0);
    ~MessageChecker();
    bool Initialize();
    bool CheckMsgSchema(const QString & msgid, QDomDocument *domD);

private:

    MessageChecker();                                           ///< Not implemented.
    MessageChecker(const MessageChecker &);                     ///< Not implemented.
    const MessageChecker & operator = (const MessageChecker &); ///< Not implemented.

    MessageCheckerErrorType_t LoadMessages();

private:

    /// type of network device: server or client
    MessageLoaderType_t m_myType;
    /// Path to config files
    QString   m_myPath;
    /// configuration Doc type
    QString   m_myDocType;
    /*! List of Server message validators sorted according to message ID
        Format: <ServerMessageID, ServerMessageValidator > */
    QHash<QString, QXmlSchemaValidator*> m_MessageCmdToSchema;
};

} // end namespace NetworkBase

#endif // NETWORKBASE_MESSAGECHECKER_H
