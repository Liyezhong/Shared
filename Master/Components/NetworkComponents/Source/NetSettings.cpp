/****************************************************************************/
/*! \file NetSettings.cpp
 *
 *  \brief This Module loads Network Server's settings from
 *         non-volatile Memory.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 29.06.2010
 *   $Author:  $ Y.Novak
 *
 *  \b Description:
 *
 *       This Module reads, checks and sets network configuration for the
 *       TCP-IP Network Server.
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

#include <NetworkComponents/Include/NetworkServer.h>
#include <NetworkComponents/Include/NetSettings.h>
#include <Global/Include/SystemPaths.h>
#include <NetworkComponents/Include/NetworkComponentEventCodes.h>
#include <Global/Include/EventObject.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief    Constructor for the NetSettings
 *
 *  \param    type    = Server Type: axeda, gui, etc
 *  \param    pParent = pointer to parent object
 *
 ****************************************************************************/
NetSettings::NetSettings(const QString & type, QObject *pParent) :
                         QObject(pParent),
                         m_myType(type)
{
}

/****************************************************************************/
/*!
 *  \brief    This API function reads Server configuration from HDD/Flash.
 *
 *      This function shall be called by Network Server to read, check
 *      and set its settings.
 *
 *  \return   NST_ALL_OK if operation is successful, otherwise error.
 *
 ****************************************************************************/
NetSettingsErrorType_t NetSettings::ReadSettings()
{
    const QString fileName = Global::SystemPaths::Instance().GetSettingsPath() + NST_NET_CONFIG_FILE;
    QString tagName  = NST_NET_CONFIG_TAG;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << (QString)("ERROR: Cannot open NetSettings File: " + fileName);
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_ERROR_FILE_OPEN,
                                                   Global::tTranslatableStringList() << fileName);
        return NST_CANNOT_OPEN_CONFIG_FILE;
    }

    NetSettingsErrorType_t err = LoadFile(file, tagName);
    file.close();
    if (err != NST_ALL_OK) {
        return err;
    }

    return NST_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    This function loads and parses the network configuration file.
 *
 *
 *  \param    device = device from which data shall be read
 *  \param    tag    = tag name of the configuration section
 *
 *  \return   NST_ALL_OK if successful, otherwise error.
 *
 ****************************************************************************/
NetSettingsErrorType_t NetSettings::LoadFile(QIODevice &device, const QString & tag)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!m_domDocument.setContent(&device, true, &errorStr, &errorLine,
                                &errorColumn)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_CONFIG_FILE_PARSING_FAILED,
                                Global::tTranslatableStringList() << errorStr);
        return NST_CONFIG_FILE_PARSING_FAILED;
    }

    QDomElement root = m_domDocument.documentElement();
    if (root.tagName() != tag) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_CONFIG_ELEMENT_TAG_MISMATCH,
                                Global::tTranslatableStringList() << tag << root.tagName());
        return NST_CONFIG_ELEMENT_TAG_MISMATCH;
    }

    if (root.hasAttribute(NST_ATTR_VERSION)
               && root.attribute(NST_ATTR_VERSION) != NST_CURRENT_VERSION) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_CONFIG_FILE_VERSION_MISMATCH,
                                Global::tTranslatableStringList() << NST_ATTR_VERSION);
        return NST_CONFIG_FILE_VERSION_MISMATCH;
    }

    NetSettingsErrorType_t err = ParseNetSection(&root);
    if (err != NST_ALL_OK) {
        return err;
    }

    err = ParseClientSection(&root);
    if (err != NST_ALL_OK) {
        return err;
    }

    err = ParseAuthenticationSection(&root);
    if (err != NST_ALL_OK) {
        return err;
    }

    return NST_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    This function loads and parses the network section of
 *            the configuration file.
 *
 *
 *  \param    element = pointer to top section level XML element
 *
 *  \return   NST_ALL_OK if successful, otherwise error.
 *
 ****************************************************************************/
NetSettingsErrorType_t NetSettings::ParseNetSection(QDomElement *element)
{
    if (element == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_NULL_POINTER,
                                                   Global::tTranslatableStringList() << FILE_LINE);
        return NST_NULL_POINTER;
    }

    QDomElement child = element->firstChildElement(NST_NET_SECTION);
    child = child.firstChildElement(m_myType);
    child = child.firstChildElement(NST_NET_ELEMENT);
    if (child.isNull()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_CONNECTION_SETTINGS_EMPTY);
        return NST_CONNECTION_SETTINGS_EMPTY;
    }

    // In future, if there are several nets of the same type - iterate
    //   through all network elements to find correct ip/port
    //while (!child.isNull()) {
    //    parseNetElement(&child);
    //    child = child.nextSiblingElement();
    //}

    m_myIp = child.firstChildElement().text();
    m_myPort = child.lastChildElement().text();

    if ((m_myIp.isEmpty()) || (m_myPort.isEmpty())) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_CONNECTION_SETTINGS_EMPTY);
        return NST_CONNECTION_SETTINGS_EMPTY;
    }

    return NST_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    This function loads and parses one network element.
 *
 *      Not used so far, but might be used later. The "key" element allows
 *      to use different network settings for one server type.
 *
 *  \param    element = pointer to the XML element to parse
 *
 ****************************************************************************/
void NetSettings::ParseNetElement(QDomElement *element)
{
    if (element == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_NULL_POINTER,
                                                   Global::tTranslatableStringList() << FILE_LINE);
        return;
    }

    if (element->hasAttribute(NST_ATTR_KEY)) {
        if (element->attribute(NST_ATTR_KEY) == m_myAttribute) {
            m_myIp = element->firstChildElement().text();
            m_myPort = element->lastChildElement().text();
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    This function loads and parses the client section of
 *            the configuration file.
 *
 *
 *  \param    element = pointer to top section level XML element
 *
 *  \return   NST_ALL_OK if successful, otherwise error.
 *
 ****************************************************************************/
NetSettingsErrorType_t NetSettings::ParseClientSection(QDomElement *element)
{
    if (element == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_NULL_POINTER,
                                                   Global::tTranslatableStringList() << FILE_LINE);
        return NST_NULL_POINTER;
    }

    QDomElement child = element->firstChildElement(NST_CLIENT_SECTION);
    child = child.firstChildElement(m_myType);
    child = child.firstChildElement(NST_CLIENT_ELEMENT);
    if (child.isNull()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_NO_VALID_CLIENT_CONNECTIONS);
        return NST_NO_VALID_CLIENT_CONNECTIONS;
    }

    while (!child.isNull()) {
        ParseClientElement(&child);
        child = child.nextSiblingElement();
    }

    if (m_availableConnections.isEmpty()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_NO_VALID_CLIENT_CONNECTIONS);
        return NST_NO_VALID_CLIENT_CONNECTIONS;
    }

    return NST_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    This function loads and parses one client element.
 *
 *      One client element is parsed and inserted into the list of
 *      allowed clients.
 *
 *  \param    element = pointer to the XML element to parse
 *
 ****************************************************************************/
void NetSettings::ParseClientElement(QDomElement *element)
{
    if (element == NULL) {
        return;
    }
    m_availableConnections.insert(element->firstChildElement().text(),
                                      element->lastChildElement().text());
}

/****************************************************************************/
/*!
 *  \brief    This function loads and parses the authentication section of
 *            the configuration file.
 *
 *
 *  \param    element = pointer to top section level XML element
 *
 *  \return   NST_ALL_OK if successful, otherwise error.
 *
 ****************************************************************************/
NetSettingsErrorType_t NetSettings::ParseAuthenticationSection(QDomElement *element)
{
    if (element == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_NULL_POINTER,
                                                   Global::tTranslatableStringList() << FILE_LINE);
        return NST_NULL_POINTER;
    }

    QDomElement child = element->firstChildElement(NST_AUTHENTICATION_SECTION);

    if (child.isNull()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_NO_AUTHENTICATION_VALUES);
        return NST_NO_AUTHENTICATION_VALUES;
    }

    m_authReq = child.firstChildElement().text();
    m_authConf = child.lastChildElement().text();
    m_authReply = child.firstChildElement(NST_AUTH_REPLY_ELEMENT);
    child = m_authReply.firstChildElement(NST_AUTH_CMD_ELEMENT);
    QString str = child.text();

    if (m_authReq.isEmpty() || m_authConf.isEmpty() || str.isEmpty()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NST_NO_AUTHENTICATION_VALUES);
        return NST_NO_AUTHENTICATION_VALUES;
    }

    return NST_ALL_OK;
}


/****************************************************************************/
/*!
 *  \brief    This function sets all configurable Server parameters.
 *
 *      This function shall be called by Server after loading of parameters
 *      was successful.
 *
 *  \param    ip    = pointer to ip string
 *  \param    port  = pointer to port string
 *  \param    aReq  = pointer to authentication request string
 *  \param    aRepl = pointer to authentication reply XML element
 *  \param    aConf = pointer to confirmation string
 *  \param    aConn = pointer to list of available connections
 *
 ****************************************************************************/
void NetSettings::SetSettings( QString *ip,
                               QString *port,
                               QString *aReq,
                               QDomElement *aRepl,
                               QString *aConf,
                               QHash<QString, QString > *aConn)
{
    *ip = m_myIp;
    *port = m_myPort;
    *aReq = m_authReq;
    *aRepl = m_authReply;
    *aConf = m_authConf;
    *aConn = m_availableConnections;
}

} // end namespace NetworkBase
