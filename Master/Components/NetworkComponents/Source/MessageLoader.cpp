/****************************************************************************/
/*! \file MessageLoader.cpp
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

#include <NetworkComponents/Include/MessageLoader.h>
#include <Global/Include/SystemPaths.h>
#include <NetworkComponents/Include/NetworkComponentEventCodes.h>
#include <Global/Include/EventObject.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief    This is the constructor for MessageLoader class
 *
 *  \param    ptype  = type of loader: client or server
 *  \param    doctype = configuration document type
 *  \param    path = path to configuration document
 *  \param    pParent = pointer to parent
 *
 ****************************************************************************/
MessageLoader::MessageLoader(MessageLoaderType_t ptype, const QString &doctype, const QString &path, QObject *pParent)
    : QObject(pParent),
    m_MessageCmdToSchema(NULL),
    m_Type(ptype),
    m_myPath(path),
    m_myDocType(doctype)
{
}

/****************************************************************************/
/*!
 *  \brief    Destructor
 *
 ****************************************************************************/
MessageLoader::~MessageLoader()
{
    m_MessageCmdToSchema = NULL;
}

/****************************************************************************/
/*!
 *  \brief    API function to call
 *
 *      This function loads, parses, checks Server & Client XML messages and
 *      fills in the corresponding message & schema lists.
 *
 *  \param    schemaMap = pointer to list of corresponding XML schemas
 *  \param    schema    = schema for validation
 *
 *  \return   CML_ALL_OK if success, otherwise error
 *
 ****************************************************************************/
MessageLoaderErrorType_t MessageLoader::LoadMessages(QHash<QString, QXmlSchemaValidator*> *schemaMap, QXmlSchema *schema)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (schemaMap == NULL) {
        return CML_NULL_POINTER;
    }

    m_MessageCmdToSchema = schemaMap;

    const QString fileName = m_myPath + CML_MSG_FILE;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_ML_ERROR_FILE_OPEN,
                                                   Global::tTranslatableStringList() << fileName);
        return CML_CANNOT_OPEN_CONFIG_FILE;
    }

    QDomDocument domDocument;
    if (!domDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
        file.close();
        Global::EventObject::Instance().RaiseEvent(EVENT_ML_ERROR_FILE_PARSING,
                                                   Global::tTranslatableStringList() << fileName);
        return CML_CONFIG_FILE_PARSING_FAILED;
    }
    file.close();

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != m_myDocType) {
        Global::EventObject::Instance().RaiseEvent(EVENT_ML_ERROR_ELEMENT_TAG_MISMATCH,
                                                   Global::tTranslatableStringList() << fileName << root.tagName());
        return CML_CONFIG_ELEMENT_TAG_MISMATCH;
    }

    MessageLoaderErrorType_t err = ParseCommands(&root, schema);

    return err;
}

/****************************************************************************/
/*!
 *  \brief    This function parses commands in the peer section
 *
 *      This function parses commands and fills the list of
 *      all defined/allowed peer messages.
 *
 *  \param    element = pointer to top level XML element
 *  \param    schema = schema for validation
 *  \return   GMR_ALL_OK if success, otherwise error
 *
 ****************************************************************************/
MessageLoaderErrorType_t MessageLoader::ParseCommands(QDomElement *element, QXmlSchema *schema)
{
    if ((element == NULL) || (m_MessageCmdToSchema == NULL)) {
        return CML_NULL_POINTER;
    }

    QString peer = "";
    switch (m_Type) {
    case CML_TYPE_SERVER:
        peer = CML_CLIENT_MSGS;
        break;
    case CML_TYPE_CLIENT:
        peer = CML_CONTROLLER_MSGS;
        break;
    default:
        return CML_WRONG_PEER_TYPE;
    }

    QDomElement child = element->firstChildElement(peer);
    if (child.isNull()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_ML_NO_VALID_MSGS,
                                                   Global::tTranslatableStringList());
        return CML_NO_VALID_MSGS;
    }

    child = child.firstChildElement(CML_MSG_ELEMENT);
    if (child.isNull()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_ML_NO_VALID_MSGS,
                                                   Global::tTranslatableStringList());
        return CML_NO_VALID_MSGS;
    }

    while (!child.isNull()) {

        if (!child.firstChildElement(CML_COMMAND).isNull()) {
            QDomElement command = child.firstChildElement(CML_COMMAND);
            if (!command.hasAttribute(CML_CMDNAME)) {
                return CML_CMD_ATTRIBUTE_NOT_FOUND;
            }
            QString cmdstring = command.attribute(CML_CMDNAME);
            if (cmdstring == "") {
                return CML_EMPTY_CMD_ATTRIBUTE;
            }
            // load corresponding schema:
            if (!LoadSchema(cmdstring, schema)) {
                return CML_NO_VALID_XMLSCHEMA;
            }
        }
        child = child.nextSiblingElement();
    }

    if (m_MessageCmdToSchema->empty()) {
        return CML_NO_VALID_XMLSCHEMAS;
    }

    return CML_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    This function loads Server XML Schemas
 *
 *      This function loads and checks a Server XML Schema for each
 *      defined/allowed Server message.
 *
 *  \param    msgid = ID of the Server message
 *  \param    schema = schema for validation
 *
 *  \return   true if schema is loaded and valid, false if not
 *
 ****************************************************************************/
bool MessageLoader::LoadSchema(const QString &msgid, QXmlSchema *schema)
{
    if  (m_MessageCmdToSchema == NULL || schema == NULL) {
        return false;
    }

    const QString filename = m_myPath + CML_SCHEMAFILE_PREFIX + msgid + CML_SCHEMAFILE_ENDFIX;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_ML_ERROR_FILE_OPEN,
                                                   Global::tTranslatableStringList() << filename);
        return false;
    }

    if (!schema->load(&file, QUrl::fromLocalFile(file.fileName()))) {
        Global::EventObject::Instance().RaiseEvent(EVENT_ML_ERROR_FILE_PARSING,
                                                   Global::tTranslatableStringList() << filename);
        file.close();
        return false;
    }

    file.close();

    if (schema->isValid()) {
        QXmlSchemaValidator *vdtr = NULL;
        vdtr = new QXmlSchemaValidator(*schema);
        if (vdtr == NULL) {
            return false;
        }
        m_MessageCmdToSchema->insert(msgid, vdtr);
    }
    else {
        return false;
    }

    return true;
}

} // end namespace NetworkBase
