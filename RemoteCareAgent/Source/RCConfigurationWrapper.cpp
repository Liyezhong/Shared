/****************************************************************************/
/*! \file RCConfigurationWrapper.cpp
 *
 *  \brief Implementation file for class RCConfigurationWrapper. This class
 *  is a wrapper of the required DataContainer configuration for RemoteCareAgent.
 *  The registration and configuration of the client is done as well. The DataItems
 *  provided by the configuration file are parsed and posted to Remote Enterprise
 *  Server.
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

// Global
#include <Global/Include/SystemPaths.h>

// Remote Interface
#include <AeOSLocal.h>
#include <AeTypes.h>
#include <AeError.h>
#include <AeOS.h>
#include <AeInterface.h>

// Network
#include <NetworkComponents/Include/NetworkDevice.h>

// DataManager
#include <DataManager/Containers/RCConfiguration/Include/RCDataItem.h>
#include <DataManager/Containers/RCConfiguration/Include/RCConfiguration.h>
#include <DataManager/Containers/RCConfiguration/Include/RCConfigurationInterface.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>

// Types
#include <Global/Include/RemoteCareTypes.h>

// Internal
#include "RCConfigurationWrapper.h"
#include "RemoteCareEventCodes.h"
//lint -e641
//lint -e1536


namespace RCAgentNamespace {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
RCConfigurationWrapper::RCConfigurationWrapper()
{
    // set default values
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
RCConfigurationWrapper::~RCConfigurationWrapper()
{
    p_rcConfig = NULL;
    p_devConfig = NULL;
    p_UserSettingsConfig = NULL;
}

/****************************************************************************/
/*!
 *  \brief Parsing confugration of the RemoteCare Agent
 *
 *  \return  true if successful, false otherwise
 */
/****************************************************************************/
bool RCConfigurationWrapper::ReadSettings()
{
    DataManager::CRCDataItem    rcDataItem;
    QString                     syspath = Global::SystemPaths::Instance().GetSettingsPath() + "/";

    dmRCAConfInterface.SetDataVerificationMode(false);
    dmDevConfInterface.SetDataVerificationMode(false);
    dmUserSettingsConfInterface.SetDataVerificationMode(false);

    if (!dmDevConfInterface.Read(syspath + "DeviceConfiguration.xml"))
    {
        qDebug() << "Cannot read device configuration" << syspath << " " << dmDevConfInterface.GetFilename();
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_READ_SETTINGS_DEVICE);
        return false;
    }

    if (!dmUserSettingsConfInterface.Read(syspath + "UserSettings.xml"))
    {
        qDebug() << "Cannot read user settings" ;
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_READ_SETTINGS_USER);
        return false;
    }

    if (!dmRCAConfInterface.Read(syspath + "RCConfiguration.xml")) {
        qDebug() << "Cannot read rc configuration" ;
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_READ_SETTINGS_RC);
        return false;
    }

    if (p_rcConfig == NULL || p_devConfig == NULL || p_UserSettingsConfig == NULL)
    {
        return false;
    }

    /// toStdString should be executed before applying it to the conversion otherwise reference is not present
    /// RemoteCare specific configuration
    certFile                          = p_rcConfig->GetCertificateFileName().toStdString();
    sessionName                       = p_rcConfig->GetRemoteSessionName().toStdString();
    if (p_rcConfig->GetDataItem(RemoteCare::RC_DATAITEM_STATE_SESSION_TYPE, rcDataItem) == true) {
        sessionType                   = rcDataItem.GetValue().toStdString();
    }
    sessionIPAddr                     = p_rcConfig->GetRemoteSessionIPAddress().toStdString();
    targetDB                          = p_rcConfig->GetTargetDataBase().toStdString();
    serverURL                         = p_rcConfig->GetServerURL().toStdString();

    if (p_rcConfig->GetDataItem(RemoteCare::RC_DATAITEM_STATE_DOWNLOAD_FOLDER, rcDataItem) == true) {
        downloadFolder                = rcDataItem.GetValue().toStdString();
    }

    configRCA.certFile                = StdStringToAeChar(certFile);
    configRCA.nameRemoteSession       = StdStringToAeChar(sessionName);
    configRCA.typeRemoteSession       = StdStringToAeChar(sessionType);
    configRCA.hostRemoteSession       = StdStringToAeChar(sessionIPAddr);
    configRCA.targetDataBase          = StdStringToAeChar(targetDB);
    configRCA.serverUrl               = StdStringToAeChar(serverURL);

    configRCA.queueSize               = static_cast<AeInt32>(p_rcConfig->GetQueueSize());
    configRCA.httpPersist             = ONOFFToAeBool(p_rcConfig->GetHTTPConnectionPersistence());
    configRCA.execTime.iSec           = static_cast<AeInt32>(p_rcConfig->GetExecTime());
    configRCA.execTime.iMicroSec      = 0;
    configRCA.debugFlag               = ONOFFToAeBool(p_rcConfig->GetDebug());
    configRCA.httpSSL                 = ONOFFToAeBool(p_rcConfig->GetHTTPSecureConnection());
    configRCA.httpEncryptionLevel     = static_cast<AeWebCryptoLevel>(p_rcConfig->GetEncryptionLevel());
    configRCA.validateCertificate     = ONOFFToAeBool(p_rcConfig->GetAuthentication());

    if (p_rcConfig->GetDataItem(RemoteCare::RC_DATAITEM_SET_PING_RATE, rcDataItem) == true) {
        configRCA.pingRate.iSec           = rcDataItem.GetValue().toLong();
    }

    configRCA.pingRate.iMicroSec      = 0;
    configRCA.serverType              = static_cast<AeDRMServerConfigType>(p_rcConfig->GetServerType());
    configRCA.serverID                = static_cast<AeInt32>(p_rcConfig->GetServerId());
    configRCA.deviceType              = static_cast<AeDRMDeviceType>(p_rcConfig->GetDeviceType());
    configRCA.deviceID                = static_cast<AeInt32>(p_rcConfig->GetDeviceId());

    configRCA.proxyProtocol           = static_cast<AeWebProxyProtocol>(p_rcConfig->GetProxyProtocol());

    configRCA.setupRemoteSession      = AeFalse;
    if (p_rcConfig->GetDataItem(RemoteCare::RC_DATAITEM_REQUEST_REMOTE_SESSION, rcDataItem) == true) {
        configRCA.setupRemoteSession  = (rcDataItem.GetValue().toInt() == 1) ? AeTrue : AeFalse;
    }

    if (p_rcConfig->GetDataItem(RemoteCare::RC_DATAITEM_STATE_SESSION_PORT, rcDataItem) == true) {
        configRCA.portRemoteSession   = static_cast<AeInt16>(rcDataItem.GetValue().toInt());
    }
    configRCA.useCompress             = ONOFFToAeBool(p_rcConfig->GetHTTPSecureConnection());
    configRCA.maxChunkSize            = static_cast<AeInt32>(p_rcConfig->GetMaxChunkSize());

    /// Device configuration
    modNr                             = p_devConfig->GetValue("DEVICENAME").remove(" ").toStdString();
    serialNr                          = p_devConfig->GetValue("SERIALNUMBER").toStdString();

    configRCA.modelNumber             = StdStringToAeChar(modNr);
    configRCA.serialNumber            = StdStringToAeChar(serialNr);

    /// User Settings will not be written by RCA
    proxyIPAddr                       = p_UserSettingsConfig->GetProxyIPAddress().toStdString();
    proxyUserName                     = p_UserSettingsConfig->GetProxyUserName().toStdString();
    proxyPass                         = p_UserSettingsConfig->GetProxyPassword().toStdString();

    /// toggling between DirectConnection and UseProxy if directconnection is on then useproxy is off
    configRCA.useProxy                = !ONOFFToAeBool(p_UserSettingsConfig->GetDirectConnection());
    configRCA.proxyPort               = static_cast<AeInt16>(p_UserSettingsConfig->GetProxyIPPort());
    configRCA.proxyHost               = StdStringToAeChar(proxyIPAddr);
    configRCA.proxyUser               = StdStringToAeChar(proxyUserName);
    configRCA.proxyPassword           = StdStringToAeChar(proxyPass);

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Apply RCConfigurationwrapper settings
 *
 *  \return  true if successfully applied settings, false otherwise
 */
/*****************************************************************************/
bool RCConfigurationWrapper::SetSettings()
{
    AeError rc;

    // run the default Axeda Agent Embedded initialization
    if (AeInitialize() != AeEOK) {
        qDebug() << "RCConfigurationwrapper ERROR: failed to run the default Agent init !";
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_SET_SETTING_AE_INIT);
        return false;
    }

    // set message queue size
    AeDRMSetQueueSize(configRCA.queueSize);

    qDebug() << "RCConfigurationwrapper DeviceType" << configRCA.deviceType
             << " ModelNr: " << configRCA.modelNumber
             << " SerialNr: " << configRCA.serialNumber;

    if (!strcmp(configRCA.modelNumber,""))
    {
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_SET_SETTINGS_MODELNR_EMPTY);
        return false;
    }

    // add device, which shall be managed by Agent
    rc = AeDRMAddDevice(configRCA.deviceType, configRCA.modelNumber,
                        configRCA.serialNumber, &configRCA.deviceID);

    qDebug() << "RCConfigurationWrapper: got DeviceId = " << configRCA.deviceID;
    if (rc != AeEOK)
    {
        qDebug() << "RCConfigurationwrapper ERROR: failed to add device " << configRCA.deviceID
                 << " with error: " << AeGetErrorString(rc);
        Global::EventObject::Instance().RaiseEvent(
                     EVENT_REMOTECARE_ERROR_SET_SETTINGS_ADD_DEVICE,
                     Global::tTranslatableStringList() << "Axeda error " << AeGetErrorString(rc),
                     true
                    );

        return false;
    }

    // tell Agent which Remote Care server to use
    rc = AeDRMAddServer(configRCA.serverType, configRCA.serverUrl,
                        configRCA.targetDataBase, &configRCA.pingRate, &configRCA.serverID);

    qDebug() << "RCConfigurationWrapper: got ServerId = " << configRCA.serverID
             << "Url " << QString(configRCA.serverUrl);

    if (rc != AeEOK)
    {
        qDebug() << "RCConfigurationwrapper ERROR: failed to add server " << configRCA.serverUrl
                 << " with error: " << AeGetErrorString(rc);
        Global::EventObject::Instance().RaiseEvent(
                                EVENT_REMOTECARE_ERROR_SET_SETTINGS_ADD_SERVER,
                                Global::tTranslatableStringList() << "Axeda error " << AeGetErrorString(rc),
                                true
                                );
        return false;
    }

    // set HTTP persistence
    if (configRCA.httpPersist == true) {
        AeError err= AeWebSetPersistent(configRCA.serverID, true);
        if (err != AeEOK) {
            qDebug() << "RCConfigurationwrapper ERROR: AeWebSetPersistent failed ! Error code: " << static_cast<int>(err);
            Global::EventObject::Instance().RaiseEvent(
                        EVENT_REMOTECARE_ERROR_SET_SETTINGS_WEB_PERSIST,
                        Global::tTranslatableStringList() << "Axeda error " << AeGetErrorString(err),
                        true
                        );
            return false;
        }
    }

    // set remote session parameters
    // after RemoteSession has been requested Agent reconnects to Remote Enterprise Server with remotesession setup
    // this will be then seen on the Web UI interface as possible remote session
    if (
        (configRCA.nameRemoteSession != NULL)  &&
        (configRCA.typeRemoteSession != NULL)  &&
        (configRCA.hostRemoteSession != NULL)  &&
        (configRCA.portRemoteSession != 0)
       )
    {
        qDebug() << "RCConfigurationwrapper : Setup remote session " ;

        ///< \note maybe there is another solution to prevent session from appearing in the WEB UI
        ///< at the moment using empty space for the name, which does not provide any access to start a session
        if (configRCA.setupRemoteSession == false)
        {
            configRCA.nameRemoteSession = const_cast<AeChar*>(" ");
        }

        rc = AeDRMAddRemoteSession(configRCA.deviceID, configRCA.nameRemoteSession, const_cast<AeChar*>(""), \
                                   configRCA.typeRemoteSession,
                                   configRCA.hostRemoteSession,
                                   configRCA.portRemoteSession);

        if (rc != AeEOK)
        {
            qDebug() << "RCConfigurationwrapper ERROR: Failed to add Remote session, error: " << AeGetErrorString(rc);
            Global::EventObject::Instance().RaiseEvent(
                                EVENT_REMOTECARE_ERROR_SET_SETTINGS_REMOTE_SESSION,
                                Global::tTranslatableStringList() << "Axeda error " << AeGetErrorString(rc),
                                true
                        );
            return false;
        }
    }

    // set SSL settings
    if (configRCA.httpSSL == true)
    {
        rc = AeWebSetSSL(configRCA.httpEncryptionLevel, configRCA.validateCertificate,
                         configRCA.certFile);
        if (rc != AeEOK)
        {
            qDebug() << "RCConfigurationwrapper ERROR: Failed to set SSL parameters, error: " << AeGetErrorString(rc);
            Global::EventObject::Instance().RaiseEvent(
                                EVENT_REMOTECARE_ERROR_SET_SETTINGS_SSL,
                                Global::tTranslatableStringList() << "Axeda error " << AeGetErrorString(rc),
                                true
                                );
            return false;
        }
    }

    // set proxy server settings
    if (configRCA.useProxy == true)
    {
        rc = AeWebSetProxy(configRCA.proxyProtocol, configRCA.proxyHost,
                           configRCA.proxyPort, configRCA.proxyUser,
                           configRCA.proxyPassword);
        if (rc != AeEOK)
        {
            qDebug() << "RCConfigurationwrapper ERROR: Failed to configure proxy, error: " << AeGetErrorString(rc);
            Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_SET_SETTINGS_PROXY,
                                                       Global::tTranslatableStringList() << "Axeda error " << AeGetErrorString(rc),
                                                       true
                                                       );
            return false;
        }
    }

    // enable/disable debug messages
    if (configRCA.debugFlag == true)
    {
        AeDRMSetLogLevel(AeLogDebug);
    }
    else {
        AeDRMSetLogLevel(AeLogNone);
    }

    qDebug() << "RCConfigurationWrapper: got ServerId = " << configRCA.serverID;

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Post all dataitems to RE-Server
 *
 *  \return  true if successful, false otherwise
 */
/*****************************************************************************/
bool RCConfigurationWrapper::PostDataItems()
{
    DataManager::CRCDataItem        rcDataItem;

    QString timestamp   = QDateTime::currentDateTime().toString(NetworkBase::DATEANDTIME_FORMAT);
    int itemCount       = p_rcConfig->GetDataItemCount();

    for (int i = 0; i < itemCount; i++)
    {
        if (p_rcConfig->GetDataItem(i, rcDataItem) == true)
        {
            if (!PostItem(rcDataItem.GetName(), rcDataItem.GetType(), rcDataItem.GetQuality(), rcDataItem.GetValue(), timestamp))
            {
                return false;
            }
        }
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Post Data Item to RE-Server
 *
 *  \iparam  nameStr       = parameter's name
 *  \iparam  type          = type of the parameter (analog/digital/string)
 *  \iparam  quality       = quality of the parameter
 *  \iparam  valueStr      = the value of the parameter
 *  \iparam  timestamp     = Master's timestamp value
 *
 *  \return  true if successful, false otherwise
 *
 *  \warning   If timestamp is in the past, the Axeda Server will not complain
 *             about the message contents, but will silently discard the
 *             message.
 */
/*****************************************************************************/
bool RCConfigurationWrapper::PostItem(
                                                const QString                            &nameStr,
                                                const RemoteCare::RCDataItemType_t       &type,
                                                const RemoteCare::RCDataItemQuality_t    &quality,
                                                const QString                            &valueStr,
                                                const QString                            &timestamp
                                             )
{
    AeDRMDataItem dataItem;
    bool ok = true;

    qDebug() << "RCConfigurationwrapper::SubmitDataItemRequest : " << nameStr;

    // set time stamp
    ConvertTime(timestamp, &dataItem.value.timeStamp);

    QByteArray name   = nameStr.toUtf8();
    QByteArray value  = valueStr.toUtf8();

    // prepare data item
    dataItem.pName          = const_cast<AeChar*>(name.data());
    dataItem.value.iType    = static_cast<AeDRMDataType>(type);
    dataItem.value.iQuality = static_cast<AeDRMDataQuality>(quality);

    switch (dataItem.value.iType) {
        case AeDRMDataAnalog:
            dataItem.value.data.dAnalog = value.toFloat(&ok);
            if (!ok) {
                qDebug() << "RCConfigurationWrapper ERROR :  cannot convert value of type " << static_cast<int>(type) << " !";
                qDebug() << "RCConfigurationWrapper ERROR :  converted value =" << dataItem.value.data.dAnalog;
                Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_POSTITEM_DA_CONVERSION);
            }
            break;
        case AeDRMDataDigital:
            dataItem.value.data.bDigital = value.toShort(&ok, 10);
            if ((!ok) || (dataItem.value.data.bDigital > 1) || (dataItem.value.data.bDigital < 0)) {
                qDebug() << "RCConfigurationWrapper ERROR :  cannot convert value of type " << static_cast<int>(type) << " !";
                ok = false;
                Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_POSTITEM_DD_CONVERSION);
            }
            break;
        case AeDRMDataString:
            dataItem.value.data.pString = const_cast<AeChar*>(value.data());
            break;
        default:
            qDebug() << "RCConfigurationWrapper ERROR :  do not know the dataItem type " << static_cast<int>(type) << " !";
            Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_POSTITEM_NOT_KNOWN);
            ok = false;
            break;
    }
    // check if data conversion was ok
    if (ok) {
        // submit data to outgoing queue and check if request was submitted
        if (AeDRMPostDataItem(configRCA.deviceID, configRCA.serverID, AeDRMQueuePriorityNormal, &dataItem) != AeEOK) {
            qDebug() << "RCConfigurationWrapper ERROR :  could not post DataItem " << name << " for upload !";
            Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_POSTITEM);
            return false;
        }
        // all ok, inform upper layer
        return true;
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief Set the default values of the local variables
 *
 *  \return RConfig_t struct
 */
/****************************************************************************/
const RCConfig_t &RCConfigurationWrapper::GetConfiguration() const
{
    return configRCA;
}

/****************************************************************************/
/*!
 *  \brief Get type of specific DataItem
 *
 *  \iparam name  = Name of DataItem
 *  \oparam type  = Type of DataItem
 *
 *  \return true - type OK, false - if no type have been found
 */
/****************************************************************************/
bool RCConfigurationWrapper::GetDataItemType(const QString &name, RemoteCare::RCDataItemType_t &type)
{
    DataManager::CRCDataItem    rcDataItem;
    type = RemoteCare::RDI_TypeInvalid;

    if (p_rcConfig == NULL)
    {
        return false;
    }

    if (p_rcConfig->GetDataItem(name, rcDataItem) == true) {
        type = rcDataItem.GetType();
        return true;
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief Set the default values of the local variables
 */
/****************************************************************************/
void RCConfigurationWrapper::SetDefaultAttributes()
{
    p_rcConfig                        = dmRCAConfInterface.GetRCConfiguration();
    p_devConfig                       = dmDevConfInterface.GetDeviceConfiguration();
    p_UserSettingsConfig              = dmUserSettingsConfInterface.GetUserSettings();

    // to please lint
    dmRCAConfInterface                = dmRCAConfInterface;
    dmDevConfInterface                = dmDevConfInterface;
    dmUserSettingsConfInterface       = dmUserSettingsConfInterface;

    targetDB                          = "";
    serverURL                         = "";
    serialNr                          = "";
    modNr                             = "";
    certFile                          = "";
    proxyIPAddr                       = "";
    proxyUserName                     = "";
    proxyPass                         = "";
    sessionName                       = "";
    sessionType                       = "";
    sessionIPAddr                     = "";
    downloadFolder                    = "../RemoteCare/";

    configRCA.queueSize               = AGENT_UPLOAD_CHUNK_SIZE*2;
    configRCA.httpPersist             = AeTrue;
    configRCA.execTime.iSec           = 5;
    configRCA.execTime.iMicroSec      = 0;
    configRCA.debugFlag               = AeFalse;
    configRCA.httpSSL                 = AeTrue;
    configRCA.httpEncryptionLevel     = AeWebCryptoMedium;
    configRCA.validateCertificate     = AeFalse;
    configRCA.certFile                = NULL;
    configRCA.pingRate.iSec           = 60;
    configRCA.pingRate.iMicroSec      = 0;
    configRCA.serverType              = AeDRMServerConfigPrimary;
    configRCA.targetDataBase          = StdStringToAeChar(targetDB.c_str());
    configRCA.serverUrl               = StdStringToAeChar(serverURL.c_str());
    configRCA.serverID                = 0;
    configRCA.serialNumber            = StdStringToAeChar(serialNr.c_str());
    configRCA.modelNumber             = StdStringToAeChar(modNr.c_str());
    configRCA.deviceType              = AeDRMDeviceMaster;
    configRCA.deviceID                = 0;
    configRCA.useProxy                = AeFalse;
    configRCA.proxyProtocol           = AeWebProxyProtoNone;
    configRCA.proxyHost               = StdStringToAeChar(proxyIPAddr.c_str());
    configRCA.proxyUser               = StdStringToAeChar(proxyUserName.c_str());
    configRCA.proxyPassword           = StdStringToAeChar(proxyPass.c_str());
    configRCA.proxyPort               = 0;
    configRCA.setupRemoteSession      = AeFalse;
    configRCA.nameRemoteSession       = StdStringToAeChar(sessionName.c_str());
    configRCA.typeRemoteSession       = StdStringToAeChar(sessionType.c_str());
    configRCA.hostRemoteSession       = StdStringToAeChar(sessionIPAddr.c_str());
    configRCA.portRemoteSession       = 5900;
    configRCA.useCompress             = AeFalse;
    configRCA.maxChunkSize            = AGENT_UPLOAD_CHUNK_SIZE;
}

/****************************************************************************/
/*!
 *  \brief Converts soup library specific AeBool type to OnOffState
 *
 *  \iparam state = state to convert
 *
 *  \return ONOFFSTATE_OFF (false) or ONOFFSTATE_ON (true)
 */
/****************************************************************************/
Global::OnOffState RCConfigurationWrapper::AeBoolToONOFF(const AeBool &state) const {
    Global::OnOffState Result = Global::ONOFFSTATE_UNDEFINED;
    switch(state) {
        case 0:
                Result = Global::ONOFFSTATE_OFF;
                break;
        case 1:
                Result = Global::ONOFFSTATE_ON;
                break;
    };
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Converts OnOffState to soup library specific AeBool type
 *
 *  \iparam OnOffState = state to convert
 *
 *  \return true (ONOFFSTATE_ON) or false (ONOFFSTATE_OFF)
 */
/****************************************************************************/
AeBool  RCConfigurationWrapper::ONOFFToAeBool(const Global::OnOffState &OnOffState) const {
    AeBool Result = 0;
    if(OnOffState == Global::ONOFFSTATE_ON) {
        Result = 1;
    } else if(OnOffState == Global::ONOFFSTATE_OFF) {
        Result = 0;
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Converts std::string to char array
 *
 *  \iparam string = String to convert
 *
 *  \return char array of standard string
 */
/****************************************************************************/
AeChar * RCConfigurationWrapper::StdStringToAeChar(const std::string &string) const {
    return const_cast<AeChar*>(string.c_str());
}

/****************************************************************************/
/*!
 *  \brief   Convert Master timestamp into Agent time format
 *
 *  \iparam  timevalue = Master's timestamp value
 *  \iparam  timestamp = pointer to Agent's timestamp structure
 */
/*****************************************************************************/
void RCConfigurationWrapper::ConvertTime(QString timevalue, AeTimeValue *timestamp)
{
    if (timevalue.isEmpty())
    {
        timevalue = Global::AdjustedTime::Instance().GetCurrentDateTime().toString(NetworkBase::DATEANDTIME_FORMAT);
    }

    QDateTime dt = QDateTime::fromString(timevalue, NetworkBase::DATEANDTIME_FORMAT);
    timestamp->iSec = static_cast<AeInt32>(dt.toTime_t());
    timestamp->iMicroSec = 0;
}

} // end namespace RCAgentNamespace
