/****************************************************************************/
/*! \file ConfigLoader.h
 *
 *  \brief This Module loads External Process's configuration from
 *         non-volatile Memory.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.02.2011
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

#ifndef EXTERNALPROCESSCONTROL_CONFIGLOADER_H
#define EXTERNALPROCESSCONTROL_CONFIGLOADER_H

#include <QtXml>

namespace ExternalProcessControl {

const QString CLD_PROCESS_CONFIG_FILE   = "/ExternalProcesses.xml";   ///< Configuration file to read
const QString CLD_PROCESS_CONFIG_TAG    = "external_processes";       ///< Config file global tag
const QString CLD_ATTR_VERSION          = "version";                  ///< Config file version attribute
const QString CLD_CURRENT_VERSION       = "1.0";                      ///< Config file version

/// Parameters section of individual Process
const QString CLD_PROCESS_SECTION  = "process";
/// Process name tag
const QString CLD_PROCESS_NAME     = "name";
/// Process start commando definition
const QString CLD_STARTCOMMAND     = "startcmd";
/// Remote login setting
const QString CLD_REMOTE_FLAG      = "remotelogin";
/// Remote login timeout setting
const QString CLD_REMOTETIMEOUT    = "remotetimeout";

/// List of errors which this class might return
typedef enum {
    CLD_ALL_OK = 1,
    CLD_CANNOT_OPEN_CONFIG_FILE,
    CLD_CONFIG_FILE_PARSING_FAILED,
    CLD_CONFIG_ELEMENT_TAG_MISMATCH,
    CLD_CONFIG_FILE_VERSION_MISMATCH,
    CLD_PROCESSNAME_NOT_FOUND,
    CLD_STARTCOMMAND_EMPTY,
    CLD_STRINGTOINT_CONVERSION_FAILED
} ConfigLoaderErrorType_t;

/****************************************************************************/
/**
 * \brief  This class loads the External Process Manager configurtion
 *         from persistent storage.
 */
/****************************************************************************/
class ConfigLoader : public QObject
{
    Q_OBJECT

public:

    ConfigLoader(const QString &processName);

public:

    ConfigLoaderErrorType_t ReadSettings(QString *startCommand, QString *remoteFlag, int *timeOut);

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(ConfigLoader)
    ConfigLoaderErrorType_t ParseDocument(const QString &processName, const QDomElement &rootElement);
    ConfigLoaderErrorType_t LoadFile(QIODevice &device, const QString &tag);
    ConfigLoaderErrorType_t ParseProcessSection(const QDomElement &processSection);

private:

    /*! The process name  */
    QString m_ProcessName;
    /*! Command to execute in order to start the process  */
    QString m_StartCommand;
    /*! Remote login: Yes/No  (if empty, "No" is assumed)
        If External Process is not started by Master and its exact login timing is undefined,
        remotelogin shall be set to "Yes". */
    QString m_RemoteLoginEnabled;
    /*! Timeout for remote login option, in seconds. Zero means infinite waiting. */
    int m_RemoteLoginTimeout;
};

} // end namespace

#endif // EXTERNALPROCESSCONTROL_CONFIGLOADER_H
