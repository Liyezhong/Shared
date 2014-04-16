/****************************************************************************/
/*! \file NetSettings.h
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

#ifndef NETWORKBASE_NETSETTINGS_H
#define NETWORKBASE_NETSETTINGS_H

#include <QtXml>

namespace NetworkBase {

const QString NST_NET_CONFIG_FILE   = "/Communication/ServersAndClients.xml";    ///< Configuration file to read
const QString NST_NET_CONFIG_TAG    = "net_setup";              ///< Config file global tag
const QString NST_ATTR_VERSION      = "version";                ///< Config file version attribute
const QString NST_CURRENT_VERSION   = "1.0";                    ///< Config file version

const QString NST_AUTHENTICATION_SECTION  = "authentication";   ///< Authentication section identifier
const QString NST_AUTH_REPLY_ELEMENT      = "reply";            ///< Reply element identifier
const QString NST_AUTH_CMD_ELEMENT        = "cmd";              ///< Command element identifier

const QString NST_CLIENT_SECTION    = "clients";                ///< Section with client settings
const QString NST_CLIENT_ELEMENT    = "client";                 ///< Settings of an individual client

const QString NST_NET_SECTION       = "networks";               ///< Section with networks settings
const QString NST_NET_ELEMENT       = "network";                ///< Settings of an individual network

const QString NST_ATTR_KEY          = "key";                    ///< Individual network setting

/// Class's return error codes
typedef enum {
    NST_ALL_OK = 1,
    NST_CANNOT_OPEN_CONFIG_FILE,
    NST_CONFIG_FILE_PARSING_FAILED,
    NST_CONFIG_ELEMENT_TAG_MISMATCH,
    NST_CONFIG_FILE_VERSION_MISMATCH,
    NST_CONNECTION_SETTINGS_EMPTY,
    NST_NO_VALID_CLIENT_CONNECTIONS,
    NST_UNKNOWN_APP_REQUEST,
    NST_LISTEN_FAILED,
    NST_NO_AUTHENTICATION_VALUES,
    NST_NULL_POINTER
} NetSettingsErrorType_t;               ///< List of errors which this class might return

/****************************************************************************/
/**
 * \brief  This class loads the network settings from persistent storage.
 */
/****************************************************************************/
class NetSettings : public QObject
{
    Q_OBJECT

public:
    NetSettings(const QString & type, QObject *pParent = 0);

public:
    NetSettingsErrorType_t ReadSettings();
    void SetSettings( QString *ip,
                      QString *port,
                      QString *aReq,
                      QDomElement *aRepl,
                      QString *aConf,
                      QHash<QString, QString > *aConn);

private:
    void ParseNetElement(QDomElement *element);
    void ParseClientElement(QDomElement *element);
    NetSettingsErrorType_t ParseNetSection(QDomElement *element);
    NetSettingsErrorType_t ParseClientSection(QDomElement *element);
    NetSettingsErrorType_t ParseAuthenticationSection(QDomElement *element);
    NetSettingsErrorType_t LoadFile(QIODevice &device, const QString & tag);

private:
    /*!  list of all possible clients with their type strings
         Format: <ClientName, typeString > */
    QHash<QString, QString > m_availableConnections;
    /*!  network config data read from non-volatile memory */
    QDomDocument m_domDocument;

    /*!  IP to listen to */
    QString m_myIp;
    /*!  port to listen to */
    QString m_myPort;
    /*!  type of server: axeda, gui, etc */
    QString m_myType;
    /*!  connection type attribute: name of the network (not used so far) */
    QString m_myAttribute;
    /*!  authentication request string */
    QString m_authReq;
    /*!  authentication confirmation string */
    QString m_authConf;
    /*!  authentication reply XML Element */
    QDomElement m_authReply;

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(NetSettings)
};

} // end namespace NetworkBase

#endif // NETWORKBASE_NETSETTINGS_H
