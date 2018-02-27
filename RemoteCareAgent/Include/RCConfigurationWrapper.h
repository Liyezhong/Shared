/****************************************************************************/
/*! \file RCConfigurationWrapper.h
 *
 *  \brief Definition file for class RCConfigurationWrapper.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-05-23
 *  $Author:    $ B. Stach
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

#ifndef RCAGENTNAMESPACE_RCCONFIGURATIONWRAPPER_H
#define RCAGENTNAMESPACE_RCCONFIGURATIONWRAPPER_H

#include <QString>
#include <AeTypes.h>

// DataManager
#include <DataManager/Containers/RCConfiguration/Include/RCConfigurationInterface.h>
#include <DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h>
#include <DataManager/Containers/UserSettings/Include/UserSettingsInterface.h>

//lint -sem(RCAgentNamespace::RCConfigurationWrapper::SetDefaultAttributes,initializer)

namespace DataManager {
  class CRCConfiguration;
  class CDeviceConfiguration;
  class CUserSettings;
}

namespace RCAgentNamespace {

const AeInt32 AGENT_UPLOAD_CHUNK_SIZE   = 1000000;                   ///< default upload chunk size is 1MB

const QString AGENT_COMMAND_PREFIX      = "RCAgentNamespace::CmdRC"; ///< prefix for commands instantiation
const QString AGENT_SLASH_IN_PATH       = "/";                       ///< slash for path extention \todo: it is linux-specific at the moment!

typedef struct RCConfig RCConfig_t;    ///< Self explaining

/****************************************************************************/
/*!
 * \brief Struct holding service link library configuration
 */
/****************************************************************************/
struct RCConfig
{
    /// general Agent configuration parameters
    AeInt32               queueSize;           ///< Outgoing message queue size (in bytes)
    AeBool				  httpPersist;         ///< If persistent HTTP(S) connection to RC Server shall be used
    AeTimeValue           execTime;            ///< Execution time period for the AeDrmExecute function
    AeBool                debugFlag;           ///< If debug messages shall be enabled
    /// Secure connection config parameters
    AeBool                httpSSL;             ///< If secure HTTPS connection to RC Server shall be used
    AeWebCryptoLevel      httpEncryptionLevel; ///< Encryption level (low, medium or high)
    AeBool                validateCertificate; ///< If server certificate shall be validated
    AeChar                *certFile;           ///< Link to file which contains certificate
    /// Remote Care Server parameters
    AeTimeValue           pingRate;            ///< How often the Server shall be pinged (in ms)
    AeDRMServerConfigType serverType;          ///< RC Server type: primary/secondary/etc.
    AeChar                *targetDataBase;     ///< The name of target database
    AeChar                *serverUrl;          ///< URL of the RC Server
    AeInt32               serverID;            ///< The assigned server ID (output parameter, assigned by Agent)
    /// Local Device Identification
    AeChar				  *serialNumber;       ///< Unique serial number of the local device
    AeChar				  *modelNumber;        ///< Local device's model number
    AeDRMDeviceType       deviceType;          ///< Local device's type (master or managed)
    AeInt32               deviceID;            ///< The assigned device ID (output parameter, assigned by Agent)
    /// Local network proxy parameters
    AeBool                useProxy;            ///< If Proxy Server shall be used
    AeWebProxyProtocol    proxyProtocol;       ///< Communication protocol to use with the Proxy
    AeChar                *proxyHost;          ///< Proxy Server IP (?)
    AeChar                *proxyUser;          ///< User Name for Proxy Authentication
    AeChar                *proxyPassword;      ///< Password for Proxy Authentication
    AeInt16               proxyPort;           ///< Proxy Server Port
    /// Remote Session parameters
    AeBool                setupRemoteSession;  ///< If Remote Session is configured
    AeChar                *nameRemoteSession;  ///< Arbitrary Remote Session name
    AeChar                *typeRemoteSession;  ///< Session type (e.g. desktop/telnet/browser)
    AeChar                *hostRemoteSession;  ///< Local VNC Server's IP (e.g (AeChar*)"127.0.0.1")
    AeInt16               portRemoteSession;   ///< Local VNC Server's Port (e.g 5900)
    /// file upload parameters
    AeBool				  useCompress;         ///< If compression shall be used for uploads
    AeInt32               maxChunkSize;        ///< Maximum size of the file upload data chunk (in bytes)
};

/****************************************************************************/
/*!
 * \brief Class as wrapper to DataContainer of RCConfiguration.
 */
/****************************************************************************/
class RCConfigurationWrapper {

    friend class TestRCConfigurationWrapper;

public:

    RCConfigurationWrapper();

    virtual ~RCConfigurationWrapper();

    bool ReadSettings();
    bool SetSettings();

    bool PostDataItems();
    bool PostItem(
                    const QString                           &nameStr,
                    const RemoteCare::RCDataItemType_t      &type,
                    const RemoteCare::RCDataItemQuality_t   &quality,
                    const QString                           &valueStr,
                    const QString                           &timestamp
            );

    const RCConfig_t &GetConfiguration() const;

    bool GetDataItemType(const QString &name, RemoteCare::RCDataItemType_t &type);

    void ConvertTime(QString time, AeTimeValue *timestamp);

    /****************************************************************************/
    /*!
     *  \brief Get download folder
     *
     *  \return string of download folder
     */
    /****************************************************************************/
    const std::string GetDownloadFolder() {
        return downloadFolder;
    }

protected:

private:

    void SetDefaultAttributes();               ///< Setting default parameter

    Global::OnOffState  AeBoolToONOFF(const AeBool &state) const;                       ///< Converts AeBool  to ONOFFState
    AeBool              ONOFFToAeBool(const Global::OnOffState &OnOffState) const;      ///< Converts ONOFFState to AeBool
    AeChar              *StdStringToAeChar(const std::string &string) const;            ///< Converts QString to AeChar*

    RCConfig_t  configRCA;      ///< RemoteCareAgent configuration data holder

    /// Strings in the configuration struct to hold the memory.
    std::string modNr;          ///< Model number
    std::string certFile;       ///< Cerfificate File
    std::string sessionName;    ///< Remote Session name
    std::string sessionType;    ///< Remote Session type
    std::string sessionIPAddr;  ///< Remote Session ip adress
    std::string proxyIPAddr;    ///< Proxy IP adress
    std::string proxyUserName;  ///< Proxy User Name
    std::string proxyPass;      ///< Proxy Password
    std::string targetDB;       ///< Target database
    std::string serverURL;      ///< Server url
    std::string serialNr;       ///< Serial number
    std::string downloadFolder; ///< Download folder

    /// DataManger
    DataManager::CRCConfigurationInterface      dmRCAConfInterface;             ///< Confguration Interface to XML DataContainer
    DataManager::CDeviceConfigurationInterface  dmDevConfInterface;             ///< Confguration Interface to XML DataContainer
    DataManager::CUserSettingsInterface         dmUserSettingsConfInterface;    ///< Confguration Interface to XML DataContainer
    DataManager::CRCConfiguration               *p_rcConfig;                    ///< Points to the RemoteCare configuration on stack
    DataManager::CDeviceConfiguration           *p_devConfig;                   ///< Points to the Device configuration on stack
    DataManager::CUserSettings                  *p_UserSettingsConfig;          ///< Points to the UserSettings configuration on stack

}; // end class RCConfigurationWrapper

} // end namespace RCAgentNamespace

#endif // RCAGENTNAMESPACE_RCCONFIGURATIONWRAPPER_H
