/****************************************************************************/
/*! \file Components/DataManager/Containers/RCConfiguration/Include/RCConfiguration.h
 *
 *  \brief Definition file for class CRCConfiguration.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-05-23
 *  $Author:    $ Ramya GJ
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

#ifndef DATAMANAGER_CRCConfiguration_H
#define DATAMANAGER_CRCConfiguration_H

#include <QString>
#include <QReadWriteLock>
#include <QLocale>
#include <QXmlStreamReader>

#include "Global/Include/GlobalDefines.h"
#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "Global/Include/RemoteCareTypes.h"
#include <DataManager/Containers/RCConfiguration/Include/RCDataItem.h>

namespace DataManager {

const int NUMBER_OF_PROXY_PROTOCOL = 4; ///< total number of proxy protocols
const QString NameOfProxyProtocols[4] = {"None", "SOCKS", "HTTP", "Invalid" }; ///< text for each protocol

const int NUMBER_OF_ENCRYPT_LEVEL = 5;  ///< total number of encryption levels
const QString NameOfEncryptionLevels[5] = {"None", "Low", "Medium", "High", "Invalid" };///< text for each encryption level

const int NUMBER_OF_DEVICE_TYPE = 3; ///< total number of device types
const QString NameOfDeviceTypes[3] = {"Master", "Managed", "Invalid" }; ///< text for each device type

const int NUMBER_OF_SERVER_TYPE = 4; ///< total number of server types
const QString NameOfServerTypes[4] = {"Primary", "Additional", "Backup", "Invalid" }; ///< text for each server type

typedef QMap<QString, CRCDataItem> RCDataItemMap_t;               //!< Hash table for data items

/****************************************************************************/
/**
 * \brief Class for reading / writing XML based configuration file for remote care.
 *
 * <b>This class is not thread safe.</b>
 */
/****************************************************************************/
class CRCConfiguration {
    friend class CRCConfigurationInterface;
private:
    int                 m_Version;              ///< Store the version number of the file
    quint32             m_QueueSize;           ///< Outgoing message queue size (in bytes)
    Global::OnOffState  m_HTTPConnectionPersistence;            ///< If persistent HTTP(S) connection to RC Server shall be used
    Global::OnOffState  m_Debug;              ///< If debug messages shall be enabled

    /// Secure connection config parameters
    Global::OnOffState  m_HTTPSecureConnection;             ///< If secure HTTPS connection to RC Server shall be used
    RemoteCare::RCWebCryptoLevel_t    m_EncryptionLevel;          ///< Encryption level (low, medium or high)
    Global::OnOffState  m_Authentication;           ///< If server certificate shall be validated
    QString             m_CertificateFileName;          ///< Link to file which contains certificate

    /// Remote Care Server parameters
    quint32                 m_ExecTime;            ///< exec time (in ms)
    RemoteCare::RCDRMServerConfigType_t   m_ServerType;          ///< RC Server type: primary/additional/backup.
    QString                 m_TargetDataBase;             ///< The name of target database
    QString                 m_ServerURL;         ///< URL of the RC Server
    quint32                 m_ServerId;            ///< The assigned server ID (output parameter, assigned by Agent)

    /// Local Device Identification
    RemoteCare::RCDRMDeviceType_t         m_DeviceType;          ///< Local device's type (master or managed)
    quint32                 m_DeviceId;            ///< The assigned device ID (output parameter, assigned by Agent)

    /// Local network proxy parameters
    RemoteCare::RCWebProxyProtocol_t m_ProxyProtocol;       ///< Communication protocol to use with the Proxy (no proxy, SOCKS proxy, HTTP proxy)

    /// Remote Session parameters
    QString                 m_RemoteSessionName; ///< Arbitrary Remote Session name
    QString                 m_RemoteSessionType; ///< Session type (e.g. desktop/telnet/browser)
    QString                 m_RemoteSessionIPAddress; ///< Local VNC Server's IP (e.g (AeChar*)"127.0.0.1")
    int                     m_RemoteSessionIPPort;   ///< Local VNC Server's Port (e.g 5900)

    /// file upload parameters
    Global::OnOffState      m_Compression;           ///< If compression shall be used for uploads
    quint32                 m_MaxChunkSize;        ///< Maximum size of the file upload data chunk (in bytes)

    RCDataItemMap_t         m_RCDataItemMap; //!< Map of Data items

    ErrorMap_t                  m_ErrorMap;             //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator
    QMap<QString, QString>      m_ValueList; //!< User Settings Map
    /****************************************************************************/

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

    bool ReadGeneral(QXmlStreamReader& XmlStreamReader);
    bool ReadSecureConnectionSettings(QXmlStreamReader& XmlStreamReader);
    bool ReadRemoteCareServerSettings(QXmlStreamReader& XmlStreamReader);
    bool ReadLocalDeviceSettings(QXmlStreamReader& XmlStreamReader);
    bool ReadLocalNetworkProxySettings(QXmlStreamReader& XmlStreamReader);
    bool ReadRemoteSessionSettings(QXmlStreamReader& XmlStreamReader);
    bool ReadFileUploadSettings(QXmlStreamReader& XmlStreamReader);
    bool ReadDataItems(QXmlStreamReader& XmlStreamReader, bool CompleteData);

    /****************************************************************************/
    /*!
     *  \brief Set the version number of the Xml config file
     *
     *  \iparam Value = version number
     */
    /****************************************************************************/
    void SetVersion(int Value)
    {
        m_Version = Value;
    }

protected:
public:

    CRCConfiguration();
    CRCConfiguration(const CRCConfiguration &);
    void CopyFromOther(const CRCConfiguration &Other);

    ~CRCConfiguration();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CRCConfiguration& RCConfiguration);
    friend QDataStream& operator >>(QDataStream& InDataStream, CRCConfiguration& RCConfiguration);
    CRCConfiguration & operator = (const CRCConfiguration &);

    void SetDefaultAttributes();

    ErrorMap_t &GetErrors();


    /****************************************************************************/
    /*!
     *  \brief Get the version number
     *
     *  \return version number
     */
    /****************************************************************************/
    int GetVersion() const
    {
        return m_Version;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Outgoing message queue size (in bytes).
     *
     *  \return QueueSize  = Outgoing message queue size (in bytes).
     */
    /****************************************************************************/
    quint32 GetQueueSize() const
    {
        return m_QueueSize;
    }

    /****************************************************************************/
    /*!
     *  \brief Set Outgoing message queue size (in bytes)
     *
     *  \iparam QueueSize = Outgoing message queue size (in bytes).
     */
    /****************************************************************************/
    void SetQueueSize(quint32 QueueSize)
    {
        m_QueueSize = QueueSize;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets If persistent HTTP(S) connection to RC Server shall be used.
     *
     *  \return HTTPConnectionPersistenceOnOffState  = On if persistent HTTP(S)
     *              connection to RC Server shall be use else off.
     */
    /****************************************************************************/
    Global::OnOffState GetHTTPConnectionPersistence() const
    {
        return m_HTTPConnectionPersistence;
    }

    /****************************************************************************/
    /*!
     *  \brief Set If persistent HTTP(S) connection to RC Server shall be used
     *
     *  \iparam HTTPConnectionPersistenceOnOffState = On if If persistent HTTP(S)
     *              connection to RC Server shall be use else off.
     */
    /****************************************************************************/
    void SetHTTPConnectionPersistence(Global::OnOffState HTTPConnectionPersistenceOnOffState)
    {
        m_HTTPConnectionPersistence = HTTPConnectionPersistenceOnOffState;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Debug logs are enabled or not.
     *
     *  \return Debug value  = On if enabled else off.
     */
    /****************************************************************************/
    Global::OnOffState GetDebug() const
    {
        return m_Debug;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Debug usage state
     *
     *  \iparam DebugOnOffState = On if debug is enabled else off.
     */
    /****************************************************************************/
    void SetDebug(Global::OnOffState DebugOnOffState)
    {
        m_Debug = DebugOnOffState;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the HTTPSecureConnection should be used or not.
     *
     *  \return HTTPSecureConnection value  = On if SecureConnection needs to be used else off.
     */
    /****************************************************************************/
    Global::OnOffState GetHTTPSecureConnection() const
    {
        return m_HTTPSecureConnection;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the HTTPSecureConnection usage state
     *
     *  \iparam HTTPSecureConnectionOnOffState = On if SecureConnection needs to be used else off.
     */
    /****************************************************************************/
    void SetHTTPSecureConnection(Global::OnOffState HTTPSecureConnectionOnOffState)
    {
        m_HTTPSecureConnection = HTTPSecureConnectionOnOffState;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the EncryptionLevel .
     *
     *  \return EncryptionLevel (none, low, medium or high).
     */
    /****************************************************************************/
    RemoteCare::RCWebCryptoLevel_t    GetEncryptionLevel() const
    {
        return m_EncryptionLevel;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the EncryptionLevel.
     *
     *  \iparam EncryptionLevel (none, low, medium or high).
     */
    /****************************************************************************/
    void SetEncryptionLevel(RemoteCare::RCWebCryptoLevel_t EncryptionLevel)
    {
        m_EncryptionLevel = EncryptionLevel;
    }


    /****************************************************************************/
    /*!
     *  \brief Gets If server certificate shall be validated.
     *
     *  \return Authentication value  = On if is enabled or else off.
     */
    /****************************************************************************/
    Global::OnOffState GetAuthentication() const
    {
        return m_Authentication;
    }

    /****************************************************************************/
    /*!
     *  \brief Set If server certificate shall be validated.
     *
     *  \iparam AuthenticationOnOffState = On if Authentication is enabled.
     */
    /****************************************************************************/
    void SetAuthentication(Global::OnOffState AuthenticationOnOffState)
    {
        m_Authentication = AuthenticationOnOffState;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the CertificateFileName.
     *
     *  \return CertificateFileName.
     */
    /****************************************************************************/
    QString   GetCertificateFileName() const
    {
        return m_CertificateFileName;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the CertificateFileName.
     *
     *  \iparam CertificateFileName
     */
    /****************************************************************************/
    void SetCertificateFileName(QString CertificateFileName)
    {
        m_CertificateFileName = CertificateFileName;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets exec time (in ms).
     *
     *  \return ExecTime value.
     */
    /****************************************************************************/
    quint32   GetExecTime() const
    {
        return m_ExecTime;
    }

    /****************************************************************************/
    /*!
     *  \brief Set exec time (in ms).
     *
     *  \iparam ExecTime value.
     */
    /****************************************************************************/
    void SetExecTime(quint32 ExecTime)
    {
        m_ExecTime = ExecTime;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the ServerType.
     *
     *  \return ServerType value (primary/additional/backup).
     */
    /****************************************************************************/
    RemoteCare::RCDRMServerConfigType_t   GetServerType() const
    {
        return m_ServerType;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the ServerType.
     *
     *  \iparam ServerType (primary/additional/backup).
     */
    /****************************************************************************/
    void SetServerType(RemoteCare::RCDRMServerConfigType_t ServerType)
    {
        m_ServerType = ServerType;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the TargetDataBase.
     *
     *  \return TargetDataBase value.
     */
    /****************************************************************************/
    QString GetTargetDataBase() const
    {
        return m_TargetDataBase;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the TargetDataBase.
     *
     *  \iparam TargetDataBase
     */
    /****************************************************************************/
    void SetTargetDataBase(QString TargetDataBase)
    {
        m_TargetDataBase = TargetDataBase;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the URL of the RC Server.
     *
     *  \return ServerURL value.
     */
    /****************************************************************************/
    QString GetServerURL() const
    {
        return m_ServerURL;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the URL of the RC Server.
     *
     *  \iparam ServerURL
     */
    /****************************************************************************/
    void SetServerURL(QString ServerURL)
    {
        m_ServerURL = ServerURL;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the The assigned server ID (output parameter, assigned by Agent).
     *
     *  \return ServerId value.
     */
    /****************************************************************************/
    quint32 GetServerId() const
    {
        return m_ServerId;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the The assigned server ID (output parameter, assigned by Agent).
     *
     *  \iparam ServerId
     */
    /****************************************************************************/
    void SetServerId(quint32 ServerId)
    {
        m_ServerId = ServerId;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the DeviceType.
     *
     *  \return DeviceType value (master or managed).
     */
    /****************************************************************************/
    RemoteCare::RCDRMDeviceType_t GetDeviceType() const
    {
        return m_DeviceType;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the DeviceType.
     *
     *  \iparam DeviceType = master or managed.
     */
    /****************************************************************************/
    void SetDeviceType(RemoteCare::RCDRMDeviceType_t DeviceType)
    {
        m_DeviceType = DeviceType;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the DeviceId.
     *
     *  \return DeviceId value.
     */
    /****************************************************************************/
    quint32 GetDeviceId() const
    {
        return m_DeviceId;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the DeviceId.
     *
     *  \iparam DeviceId = Id of the local device.
     */
    /****************************************************************************/
    void SetDeviceId(quint32 DeviceId)
    {
        m_DeviceId = DeviceId;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the ProxyProtocol.
     *
     *  \return ProxyProtocol value (no proxy, SOCKS proxy, HTTP proxy).
     */
    /****************************************************************************/
    RemoteCare::RCWebProxyProtocol_t  GetProxyProtocol() const
    {
        return m_ProxyProtocol;
    }
    /****************************************************************************/
    /*!
     *  \brief Set the ProxyProtocol.
     *
     *  \iparam ProxyProtocol = proxy protocol(no proxy, SOCKS proxy, HTTP proxy).
     */
    /****************************************************************************/
    void SetProxyProtocol( RemoteCare::RCWebProxyProtocol_t ProxyProtocol)
    {
        m_ProxyProtocol = ProxyProtocol;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the RemoteSession Name
     *
     *  \return Remot Session Name
     */
    /****************************************************************************/
    QString GetRemoteSessionName() const
    {
        return m_RemoteSessionName;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Remote Session Name
     *
     *  \iparam RemoteSessionName = Remote Session Name
     */
    /****************************************************************************/
    void SetRemoteSessionName(QString RemoteSessionName)
    {
        m_RemoteSessionName = RemoteSessionName;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the RemoteSession Type
     *
     *  \return Remot Session Type
     */
    /****************************************************************************/
    QString GetRemoteSessionType() const
    {
        return m_RemoteSessionType;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Remote Session Type
     *
     *  \iparam RemoteSessionType = Remote Session Type
     */
    /****************************************************************************/
    void SetRemoteSessionType(QString RemoteSessionType)
    {
        m_RemoteSessionType = RemoteSessionType;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the RemoteSession IP Address
     *
     *  \return Remot Session IP Address
     */
    /****************************************************************************/
    QString GetRemoteSessionIPAddress() const
    {
        return m_RemoteSessionIPAddress;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Remote Session IP Address
     *
     *  \iparam RemoteSessionIPAddress = Remote Session IP Address
     */
    /****************************************************************************/
    void SetRemoteSessionIPAddress(QString RemoteSessionIPAddress)
    {
        m_RemoteSessionIPAddress = RemoteSessionIPAddress;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the Remote Session IP Port
     *
     *  \return int
     */
    /****************************************************************************/
    int GetRemoteSessionIPPort() const
    {
        return m_RemoteSessionIPPort;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Remote Session IP Port
     *
     *  \iparam RemoteSessionIPPort = Remote Session IP Port
     */
    /****************************************************************************/
    void SetRemoteSessionIPPort(int RemoteSessionIPPort)
    {
        m_RemoteSessionIPPort = RemoteSessionIPPort;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets If compression shall be used for uploads
     *
     *  \return CompressionOnOffState  = On if compression is needed.
     */
    /****************************************************************************/
    Global::OnOffState GetCompression() const
    {
        return m_Compression;
    }

    /****************************************************************************/
    /*!
     *  \brief Set If compression shall be used for uploads
     *
     *  \iparam CompressionOnOffState = On if compression is needed.
     */
    /****************************************************************************/
    void SetCompression(Global::OnOffState CompressionOnOffState)
    {
        m_Compression = CompressionOnOffState;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets Maximum size of the file upload data chunk (in bytes)
     *
     *  \return MaxChunkSize.
     */
    /****************************************************************************/
    quint32 GetMaxChunkSize() const
    {
        return m_MaxChunkSize;
    }

    /****************************************************************************/
    /*!
     *  \brief Set Maximum size of the file upload data chunk (in bytes)
     *
     *  \iparam MaxChunkSize = Maximum file upload databyte.
     */
    /****************************************************************************/
    void SetMaxChunkSize(quint32 MaxChunkSize)
    {
        m_MaxChunkSize = MaxChunkSize;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets a particular data item based on item anme
     *
     *  \iparam Name = Nameof data item.
     *  \iparam RCDataItem = Data item which will be filled.
     *
     *  \return true if the dataitem exists, else false.
     */
    /****************************************************************************/
    bool GetDataItem(const QString Name, CRCDataItem &RCDataItem) const
    {
        if(m_RCDataItemMap.contains(Name))
        {
            RCDataItem = m_RCDataItemMap.value(Name);
            return true;
        }

        return false;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets a particular data item based on index
     *
     *  \iparam Index = Index in the data item Map.
     *  \iparam RCDataItem = Data item which will be filled.
     *
     *  \return true if the indexed dataitem exists, else false.
     */
    /****************************************************************************/
    bool GetDataItem(const int Index, CRCDataItem &RCDataItem) const
    {
        if(Index >= 0 && Index < m_RCDataItemMap.count())
        {
            QStringList DataItemString = m_RCDataItemMap.keys();

            RCDataItem = m_RCDataItemMap.value(DataItemString.at(Index));
            return true;
        }
        return false;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the count of data items
     *
     *  \return Data items count.
     */
    /****************************************************************************/
    int GetDataItemCount() const
    {
        return m_RCDataItemMap.count();
    }

    bool AddDataItem(const QString Name, const CRCDataItem &RCDataItem);
    bool UpdateDataItem(const QString Name, const CRCDataItem &RCDataItem);
    bool DeleteDataItem(const QString Name);

}; // end class CRCConfiguration

} // end namespace DataManager

#endif // DATAMANAGEMENT_CRCConfiguration_H
