/****************************************************************************/
/*! \file MessageLoader.h
 *
 *  \brief Module which loads protocol message definitions.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 04.07.2010
 *   $Author:  $ Y.Novak
 *
 *  \b Description:
 *
 *       This Module loads protocol message XML definitions from a
 *       non-volatile memory.
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

#ifndef NETWORKBASE_MESSAGELOADER_H
#define NETWORKBASE_MESSAGELOADER_H

#include <QtXml>
#include <QXmlSchema>
#include <QXmlSchemaValidator>


namespace NetworkBase {

/// Messaging file to load
const QString CML_MSG_FILE           = "/ProtocolMessages.xml";
/// Common part of schemas path
const QString CML_SCHEMAFILE_PREFIX  = "/schemas/schema_";
/// Schemas extension
const QString CML_SCHEMAFILE_ENDFIX  = ".xsd";
/// Message identifiaction tag
const QString CML_MSG_ELEMENT        = "message";
/// Command identifiaction tag
const QString CML_COMMAND            = "cmd";
/// File section with Server messages
const QString CML_CONTROLLER_MSGS    = "server_messages";
/// File section with Client messages
const QString CML_CLIENT_MSGS        = "client_messages";
/// Schema attribute for command name
const QString CML_CMDNAME            = "name";

/// Class's return error codes
typedef enum {
        CML_ALL_OK = 1,
        CML_CANNOT_OPEN_CONFIG_FILE,
        CML_CONFIG_FILE_PARSING_FAILED,
        CML_CONFIG_ELEMENT_TAG_MISMATCH,
        CML_NO_VALID_MSGS,
        CML_NO_VALID_XMLSCHEMA,
        CML_NO_VALID_XMLSCHEMAS,
        CML_CMD_ATTRIBUTE_NOT_FOUND,
        CML_EMPTY_CMD_ATTRIBUTE,
        CML_WRONG_PEER_TYPE,
        CML_NULL_POINTER
} MessageLoaderErrorType_t;

/// Identification of message Types
typedef enum {
    CML_TYPE_SERVER = 1,   ///< Server message type
    CML_TYPE_CLIENT        ///< Client message type
} MessageLoaderType_t;

/****************************************************************************/
/**
 * \brief  This class loads message definitions from persistent storage.
 */
/****************************************************************************/
class MessageLoader : public QObject
{

public:

    MessageLoader(MessageLoaderType_t ptype, const QString & doctype, const QString & path, QObject *pParent = 0);
    ~MessageLoader();

public:

    MessageLoaderErrorType_t LoadMessages(QHash<QString, QXmlSchemaValidator*> *schemaMap);

private:

    MessageLoader();                                            ///< Not implemented.
    MessageLoader(const MessageLoader &);                       ///< Not implemented.
    const MessageLoader & operator = (const MessageLoader &);   ///< Not implemented.

    MessageLoaderErrorType_t ParseCommands(QDomElement *element);
    bool LoadSchema(const QString & msgid);

private:
    /*! Pointer to list of Server message validators sorted according to message ID
        Format: <ServerMessageID, ServerMessageValidator > */
    QHash<QString, QXmlSchemaValidator*> *m_MessageCmdToSchema;
    /// type of peer: server or client
    MessageLoaderType_t m_Type;
    /// Path to configuration files
    QString m_myPath;
    /// Document type
    QString m_myDocType;
};

} // end namespace NetworkBase

#endif // NETWORKBASE_MESSAGELOADER_H
