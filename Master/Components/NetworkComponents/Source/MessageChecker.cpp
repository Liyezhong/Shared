/****************************************************************************/
/*! \file MessageChecker.cpp
 *
 *  \brief Implementatioin of MessageChecker class.
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

#include <NetworkComponents/Include/NetworkDevice.h>
#include <NetworkComponents/Include/MessageChecker.h>

namespace NetworkBase {


/****************************************************************************/
/*!
 *  \brief    This is the constructor for MessageChecker class
 *
 *
 *  \param    ptype  = type of checker: client or server
 *  \param    doctype = configuration document type
 *  \param    path = path to configuration document
 *  \param    pParent = pointer to parent
 *
 ****************************************************************************/
MessageChecker::MessageChecker(MessageLoaderType_t ptype, const QString & doctype, const QString & path, QObject *pParent) :
        QObject(pParent),
        m_myType(ptype),
        m_myPath(path),
        m_myDocType(doctype)
{
}

/****************************************************************************/
/*!
 *  \brief    This is the destructor for MessageChecker class
 *
 ****************************************************************************/
MessageChecker::~MessageChecker()
{
    try {
        // delete all instantiated QXmlSchemaValidator objects!
        QList<QXmlSchemaValidator*> values = m_MessageCmdToSchema.values();
        for (int i = 0; i < values.size(); i++) {
            if (values[i] != 0) {
                delete values[i];
            }
        }
    } catch (...) {
        // to please PCLint...
    }
}

/****************************************************************************/
/*!
 *  \brief    This function loads messags definitions
 *
 *      This function loads, parses, checks XML messages and
 *      fills in the corresponding message & schema lists.
 *
 *  \return   CMH_ALL_OK if success, otherwise error
 *
 ****************************************************************************/
MessageCheckerErrorType_t MessageChecker::LoadMessages()
{
    if (m_myType == CML_TYPE_CLIENT) {
        // No need to load schemas for Client --> exit
        return CMH_ALL_OK;
    }
    MessageLoader cml(m_myType, m_myDocType, m_myPath, this);
    MessageLoaderErrorType_t err =
                 cml.LoadMessages(&m_MessageCmdToSchema);

    if (err != CML_ALL_OK) {
        qDebug() << "\nMSGHDLR: cannot load messages! Error = " << static_cast<int>(err) << "\n";
        return CMH_CANNOT_LOAD_MESSAGES;
    }
    return CMH_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    This function checks the format of XML message
 *
 *
 *  \param    msgid  = ID of the message
 *  \param    domD   = pointer to the incoming XML message
 *
 *  \return   true if message is OK, false if message format is wrong
 *
 ****************************************************************************/
bool MessageChecker::CheckMsgSchema(const QString & msgid, QDomDocument *domD)
{
    if (m_myType == CML_TYPE_CLIENT) {
        // schemas are not checked for Client --> exit
        return true;
    }

    QByteArray barr = domD->toByteArray(-1);

    QXmlSchemaValidator *vdtr = NULL;
    vdtr = m_MessageCmdToSchema.value(msgid);

    if (vdtr == NULL) {
        qDebug() << "MessageChecker ERROR: validator is NULL !";
        return false;
    }

    if (!vdtr->validate(barr)) {
        qDebug() << "MessageChecker ERROR: incoming MSG document is invalid !";
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief    This function initializes the Messsage Checker
 *
 *      This function loads, parses, checks XML messages and
 *      fills in the corresponding message & schema lists.
 *
 *  \return   true if success, otherwise false
 *
 ****************************************************************************/
bool MessageChecker::Initialize()
{
    if (CMH_ALL_OK != LoadMessages()) {
        return false;
    }
    return true;
}

} // end namespace NetworkBase
