/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/RCConfiguration/Source/RCConfiguration.cpp
 *
 *  \brief Implementation file for class CRCConfiguration.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2014-03-13
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

#include <QReadLocker>
#include <QWriteLocker>
#include <QDebug>
#include <QFile>

#include "DataManager/Containers/RCConfiguration/Include/RCConfiguration.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"
//lint -e641
//lint -e1536
//lint -sem(DataManager::CRCConfiguration::CopyFromOther,initializer)
namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CRCConfiguration::CRCConfiguration() :
    m_Version(0),
    m_QueueSize(2000000),
    m_HTTPConnectionPersistence(Global::ONOFFSTATE_ON),
    m_Debug(Global::ONOFFSTATE_OFF),
    m_HTTPSecureConnection(Global::ONOFFSTATE_ON),
    m_EncryptionLevel(RemoteCare::RCWebCryptoMedium),
    m_Authentication(Global::ONOFFSTATE_OFF),
    m_CertificateFileName("Certificate FileName"),
    m_ExecTime(5),
    m_ServerType(RemoteCare::RCDRMServerConfigPrimary),
    m_TargetDataBase("drm-data_source"),
    m_ServerURL("https://leica-sandbox.axeda.com:443/eMessage"),
    m_ServerId(15),
    m_DeviceType(RemoteCare::RCDRMDeviceMaster),
    m_DeviceId(15),
    m_ProxyProtocol(RemoteCare::RCWebProxyProtoNone),
    m_RemoteSessionName("HimlayaRemote"),
    m_RemoteSessionIPAddress("127.0.0.1"),
    m_Compression(Global::ONOFFSTATE_OFF),
    m_MaxChunkSize(1000000)
{
    // set default values
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam RCConfiguration = Instance of the CRCConfiguration class
 *
 *  \return
 */
/****************************************************************************/
CRCConfiguration::CRCConfiguration(const CRCConfiguration& RCConfiguration)
{
    CopyFromOther(RCConfiguration);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \iparam Other = Instance of the CDataStationList class
.*  \note  Method for internal use only
 *
 *  \return
 */
/****************************************************************************/
void CRCConfiguration::CopyFromOther(const CRCConfiguration &Other)
{
    //QReadWriteLock is not copied. We use the existing lock object
    CRCConfiguration &OtherRCConfig = const_cast<CRCConfiguration &>(Other);
    m_QueueSize  = OtherRCConfig.GetQueueSize();
    m_Version = OtherRCConfig.GetVersion();
    m_HTTPConnectionPersistence = OtherRCConfig.GetHTTPConnectionPersistence();
    m_Debug = OtherRCConfig.m_Debug;
    m_HTTPSecureConnection  = OtherRCConfig.GetHTTPSecureConnection();
    m_EncryptionLevel = OtherRCConfig.GetEncryptionLevel();
    m_Authentication = OtherRCConfig.GetAuthentication();
    m_CertificateFileName = OtherRCConfig.GetCertificateFileName();
    m_ExecTime  = OtherRCConfig.GetExecTime();
    m_ServerType = OtherRCConfig.GetServerType();
    m_TargetDataBase = OtherRCConfig.GetTargetDataBase();
    m_ServerURL = OtherRCConfig.GetServerURL();
    m_ServerId  = OtherRCConfig.GetServerId();
    m_DeviceType = OtherRCConfig.GetDeviceType();
    m_DeviceId = OtherRCConfig.GetDeviceId();
    m_ProxyProtocol = OtherRCConfig.GetProxyProtocol();
    m_RemoteSessionName = OtherRCConfig.GetRemoteSessionName();
    m_RemoteSessionIPAddress = OtherRCConfig.GetRemoteSessionIPAddress();
    m_Compression = OtherRCConfig.GetCompression();
    m_MaxChunkSize = OtherRCConfig.GetMaxChunkSize();
    m_ErrorMap = OtherRCConfig.m_ErrorMap;

    m_RCDataItemMap.clear();

    for (qint32 I = 0; I < OtherRCConfig.GetDataItemCount(); I++) {
        CRCDataItem RCDataItem;
        if(OtherRCConfig.GetDataItem(I, RCDataItem)) {
            (void)AddDataItem(RCDataItem.GetName(), RCDataItem);
        }
    }
}
/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CRCConfiguration::~CRCConfiguration()
{
}

/****************************************************************************/
/*!
 *  \brief Set the default values of the local variables
 */
/****************************************************************************/
void CRCConfiguration::SetDefaultAttributes()
{
    m_Version               =   0;
    m_QueueSize             =   2000000;
    m_HTTPConnectionPersistence =   Global::ONOFFSTATE_ON;
    m_Debug                 =   Global::ONOFFSTATE_OFF;
    m_HTTPSecureConnection  =   Global::ONOFFSTATE_ON;
    m_EncryptionLevel       =   RemoteCare::RCWebCryptoMedium;
    m_Authentication        =   Global::ONOFFSTATE_OFF;
    m_CertificateFileName   =   "Certificate FileName";
    m_ExecTime              =   5;
    m_ServerType            =   RemoteCare::RCDRMServerConfigPrimary;
    m_TargetDataBase        =   "drm-data_source";
    m_ServerURL             =   "https://leica-sandbox.axeda.com:443/eMessage";
    m_ServerId              =   15;
    m_DeviceType            =   RemoteCare::RCDRMDeviceMaster;
    m_DeviceId              =   15;
    m_ProxyProtocol         =   RemoteCare::RCWebProxyProtoNone;
    m_RemoteSessionName     =   "Colorado_Remote";
    m_RemoteSessionIPAddress    =   "127.0.0.1";
    m_Compression           =   Global::ONOFFSTATE_OFF;
    m_MaxChunkSize          =   1000000;
}



/****************************************************************************/
/*!
 *  \brief Writes from the CRCConfiguration object to a IODevice.
 *
 *  \oparam StreamWriter = Instance of the QXmlStreamWriter
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::SerializeContent(const QXmlStreamWriter& StreamWriter, const bool& CompleteData)
{
    QString StringValue; //!< to store the version number
    bool Result = true;
    QXmlStreamWriter& XmlStreamWriter = const_cast<QXmlStreamWriter &>(StreamWriter);

    // write version number
    (void) StringValue.setNum(GetVersion());  //to suppress lint-534
    XmlStreamWriter.writeAttribute("Version", StringValue);

    // write the localization details
    XmlStreamWriter.writeStartElement("General");
    XmlStreamWriter.writeAttribute("QueueSize", QString::number(GetQueueSize(), 10));
    XmlStreamWriter.writeAttribute("HTTPConnectionPersistence", Global::OnOffStateToString(GetHTTPConnectionPersistence()));
    XmlStreamWriter.writeAttribute("Debug", Global::OnOffStateToString(GetDebug()));
    XmlStreamWriter.writeEndElement();

    /// Secure connection config parameters
    XmlStreamWriter.writeStartElement("SecureConnection");
    XmlStreamWriter.writeAttribute("HTTPSecureConnection", Global::OnOffStateToString(GetHTTPSecureConnection()));

    int IndexOfEncryptionLevel = (int)GetEncryptionLevel();
    if ((IndexOfEncryptionLevel < 0) || (IndexOfEncryptionLevel >= NUMBER_OF_ENCRYPT_LEVEL))
    {
        IndexOfEncryptionLevel = (int)RemoteCare::RCWebCryptoInvalid;
        Result = false;
    }
    XmlStreamWriter.writeAttribute("EncryptionLevel", NameOfEncryptionLevels[IndexOfEncryptionLevel]);

    XmlStreamWriter.writeAttribute("Authentication", Global::OnOffStateToString(GetAuthentication()));
    XmlStreamWriter.writeAttribute("CertificateFileName", GetCertificateFileName());
    XmlStreamWriter.writeEndElement();

    /// Remote Care Server parameters
    XmlStreamWriter.writeStartElement("RemoteCareServer");
    XmlStreamWriter.writeAttribute("ExecTime", QString::number(GetExecTime(), 10));

    int IndexOfServerType = (int)GetServerType();
    if ((IndexOfServerType < 0) || (IndexOfServerType >= NUMBER_OF_SERVER_TYPE))
    {
        IndexOfServerType = (int)RemoteCare::RCDRMServerConfigInvalid;
        Result = false;
    }
    XmlStreamWriter.writeAttribute("ServerType", NameOfServerTypes[IndexOfServerType]);

    XmlStreamWriter.writeAttribute("TargetDataBase", GetTargetDataBase());
    XmlStreamWriter.writeAttribute("ServerURL", GetServerURL());
    XmlStreamWriter.writeAttribute("ServerId", QString::number(GetServerId(), 10));
    XmlStreamWriter.writeEndElement();

    /// Local Device Identification
    XmlStreamWriter.writeStartElement("LocalDevice");

    int IndexOfDeviceType = (int)GetDeviceType();
    if ((IndexOfDeviceType < 0) || (IndexOfDeviceType >= NUMBER_OF_DEVICE_TYPE))
    {
        IndexOfDeviceType = (int)RemoteCare::RCDRMDeviceInvalid;
        Result = false;
    }
    XmlStreamWriter.writeAttribute("DeviceType", NameOfDeviceTypes[IndexOfDeviceType]);

    XmlStreamWriter.writeAttribute("DeviceId", QString::number(GetDeviceId(), 10));
    XmlStreamWriter.writeEndElement();

    /// Local network proxy parameters
    XmlStreamWriter.writeStartElement("LocalNetworkProxy");

    int IndexOfProxyProtocol = (int)GetProxyProtocol();
    if ((IndexOfProxyProtocol < 0) || (IndexOfProxyProtocol >= NUMBER_OF_PROXY_PROTOCOL))
    {
        IndexOfProxyProtocol = (int)RemoteCare::RCWebProxyProtoInvalid;
        Result = false;
    }
    XmlStreamWriter.writeAttribute("ProxyProtocol", NameOfProxyProtocols[IndexOfProxyProtocol]);

    XmlStreamWriter.writeEndElement();

    /// Remote Session parameters
    XmlStreamWriter.writeStartElement("RemoteSession");
    XmlStreamWriter.writeAttribute("RemoteSessionName", GetRemoteSessionName());
    XmlStreamWriter.writeAttribute("RemoteSessionIPAddress", GetRemoteSessionIPAddress());
    XmlStreamWriter.writeEndElement();

    /// file upload parameters
    XmlStreamWriter.writeStartElement("FileUpload");
    XmlStreamWriter.writeAttribute("Compression", Global::OnOffStateToString(GetCompression()));
    XmlStreamWriter.writeAttribute("MaxChunkSize", QString::number(GetMaxChunkSize(), 10));

    XmlStreamWriter.writeStartElement("DataItems");

    QMapIterator<QString, CRCDataItem> Itr(m_RCDataItemMap);

    while(Itr.hasNext())
    {
        CRCDataItem RCDataItem = Itr.next().value();

        RCDataItem.SerializeContent(XmlStreamWriter, CompleteData);
    } // end of for loop

    XmlStreamWriter.writeEndElement(); // for Dataitems

    XmlStreamWriter.writeEndElement();

    if (CompleteData)
    {
        // do nothing
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CRCConfiguration object.
 *
 *  \iparam XmlStreamReader = Instance of the QXmlStreamReader.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::DeserializeContent(const QXmlStreamReader& XmlStreamReader, const bool& CompleteData)
{
    while ((!XmlStreamReader.atEnd()) &&
           ((XmlStreamReader.name() != "ClassTemporaryData") || (XmlStreamReader.tokenType() != QXmlStreamReader::StartElement)))
    {
        const_cast<QXmlStreamReader &>(XmlStreamReader).readNextStartElement();

        if (XmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
        {
            if (XmlStreamReader.name() == "General")
            {
                if (!ReadGeneral(XmlStreamReader))
                {
                    qDebug() << "CRCConfiguration::DeserializeContent: Read general is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "SecureConnection")
            {
                if (!ReadSecureConnectionSettings(XmlStreamReader))
                {
                    qDebug() << "CRCConfiguration::DeserializeContent: Read SecureConnection is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "RemoteCareServer")
            {
                if (!ReadRemoteCareServerSettings(XmlStreamReader))
                {
                    qDebug() << "CRCConfiguration::DeserializeContent: Read RemoteCareServer settings is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "LocalDevice")
            {
                if (!ReadLocalDeviceSettings(XmlStreamReader))
                {
                    qDebug() << "CRCConfiguration::DeserializeContent: Read LocalDevice settings is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "RemoteSession")
            {
                if (!ReadRemoteSessionSettings(XmlStreamReader))
                {
                    qDebug() << "CRCConfiguration::DeserializeContent: Read RemoteSession settings is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "LocalNetworkProxy")
            {
                if (!ReadLocalNetworkProxySettings(XmlStreamReader))
                {
                    qDebug() << "CRCConfiguration::DeserializeContent: Read LocalNetworkProxy is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "FileUpload")
            {
                if (!ReadFileUploadSettings(XmlStreamReader))
                {
                    qDebug() << "CRCConfiguration::DeserializeContent: Read FileUpload is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "DataItems")
            {
                if (!ReadDataItems(XmlStreamReader, CompleteData))
                {
                    qDebug() << "CRCConfiguration::DeserializeContent: Read DataItems is failed";
                    return false;
                }
            }
        }
    }

    if (CompleteData) {
        // do nothing
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the general settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::ReadGeneral(const QXmlStreamReader& XmlStreamReader)
{
    // read QueueSize
    if (!XmlStreamReader.attributes().hasAttribute("QueueSize"))
    {
        qDebug() << "CRCConfiguration::ReadGeneral:### attribute <QueueSize> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "QueueSize", true);
        return false;
    }
    SetQueueSize(XmlStreamReader.attributes().value("QueueSize").toString().toInt());

    // read HTTPConnectionPersistence
    if (!XmlStreamReader.attributes().hasAttribute("HTTPConnectionPersistence"))
    {
        qDebug() << "CRCConfiguration::ReadGeneral:### attribute <HTTPConnectionPersistence> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "HTTPConnectionPersistence", true);
        return false;
    }    
    SetHTTPConnectionPersistence(Global::StringToOnOffState(XmlStreamReader.attributes().value("HTTPConnectionPersistence").toString(), false));

    // read Debug
    if (!XmlStreamReader.attributes().hasAttribute("Debug"))
    {
        qDebug() << "CRCConfiguration::ReadGeneral:### attribute <Debug> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "Debug", true);
        return false;
    }
    SetDebug(Global::StringToOnOffState(XmlStreamReader.attributes().value("Debug").toString(), false));

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the Secure Connection Settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::ReadSecureConnectionSettings(const QXmlStreamReader &XmlStreamReader)
{
    // read HTTPSecureConnection
    if (!XmlStreamReader.attributes().hasAttribute("HTTPSecureConnection"))
    {
        qDebug() << "CRCConfiguration::ReadSecureConnectionSettings:### attribute <HTTPSecureConnection> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "HTTPSecureConnection", true);
        return false;
    }
    SetHTTPSecureConnection(Global::StringToOnOffState(XmlStreamReader.attributes().value("HTTPSecureConnection").toString(), false));

    // read Authentication
    if (!XmlStreamReader.attributes().hasAttribute("Authentication"))
    {
        qDebug() << "CRCConfiguration::ReadSecureConnectionSettings:### attribute <Authentication> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "Authentication", true);
        return false;
    }
    SetAuthentication(Global::StringToOnOffState(XmlStreamReader.attributes().value("Authentication").toString(), false));

    // read CertificateFileName
    if (!XmlStreamReader.attributes().hasAttribute("CertificateFileName"))
    {
        qDebug() << "CRCConfiguration::ReadSecureConnectionSettings:### attribute <CertificateFileName> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "CertificateFileName", true);
        return false;
    }
    SetCertificateFileName(XmlStreamReader.attributes().value("CertificateFileName").toString());

    // read EncryptionLevel
    if (!XmlStreamReader.attributes().hasAttribute("EncryptionLevel"))
    {
        qDebug() << "CRCConfiguration::ReadSecureConnectionSettings:### attribute <EncryptionLevel> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "EncryptionLevel", true);
        return false;
    }
    QString EncryptionLevel = XmlStreamReader.attributes().value("EncryptionLevel").toString();
    if (NameOfEncryptionLevels[RemoteCare::RCWebCryptoNone] == EncryptionLevel)
    {
        SetEncryptionLevel(RemoteCare::RCWebCryptoNone);
    }
    else if (NameOfEncryptionLevels[RemoteCare::RCWebCryptoLow] == EncryptionLevel)
    {
        SetEncryptionLevel(RemoteCare::RCWebCryptoLow);
    }
    else if (NameOfEncryptionLevels[RemoteCare::RCWebCryptoMedium] == EncryptionLevel)
    {
        SetEncryptionLevel(RemoteCare::RCWebCryptoMedium);
    }
    else if (NameOfEncryptionLevels[RemoteCare::RCWebCryptoHigh] == EncryptionLevel)
    {
        SetEncryptionLevel(RemoteCare::RCWebCryptoHigh);
    }
    else
    {
        SetEncryptionLevel(RemoteCare::RCWebCryptoInvalid);
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the Remote Care Server settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::ReadRemoteCareServerSettings(const QXmlStreamReader &XmlStreamReader)
{
    // read exectime
    if (!XmlStreamReader.attributes().hasAttribute("ExecTime"))
    {
        qDebug() << "CRCConfiguration::ReadRemoteCareServerSettings:### attribute <ExecTime> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ExecTime", true);
        return false;
    }
    SetExecTime(XmlStreamReader.attributes().value("ExecTime").toString().toInt());

    // read ServerType
    if (!XmlStreamReader.attributes().hasAttribute("ServerType"))
    {
        qDebug() << "CRCConfiguration::ReadSecureConnectionSettings:### attribute <ServerType> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ServerType", true);
        return false;
    }
    QString ServerType = XmlStreamReader.attributes().value("ServerType").toString();
    if (NameOfServerTypes[RemoteCare::RCDRMServerConfigPrimary] == ServerType)
    {
        SetServerType(RemoteCare::RCDRMServerConfigPrimary);
    }
    else if (NameOfServerTypes[RemoteCare::RCDRMServerConfigAdditional] == ServerType)
    {
        SetServerType(RemoteCare::RCDRMServerConfigAdditional);
    }
    else if (NameOfServerTypes[RemoteCare::RCDRMServerConfigBackup] == ServerType)
    {
        SetServerType(RemoteCare::RCDRMServerConfigBackup);
    }
    else
    {
        SetServerType(RemoteCare::RCDRMServerConfigInvalid);
    }

    // read TargetDataBase
    if (!XmlStreamReader.attributes().hasAttribute("TargetDataBase"))
    {
        qDebug() << "CRCConfiguration::ReadRemoteCareServerSettings:### attribute <TargetDataBase> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "TargetDataBase", true);
        return false;
    }
    SetTargetDataBase(XmlStreamReader.attributes().value("TargetDataBase").toString());

    // read ServerURL
    if (!XmlStreamReader.attributes().hasAttribute("ServerURL"))
    {
        qDebug() << "CRCConfiguration::ReadRemoteCareServerSettings:### attribute <ServerURL> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ServerURL", true);
        return false;
    }
    SetServerURL(XmlStreamReader.attributes().value("ServerURL").toString());

    // read ServerId
    if (!XmlStreamReader.attributes().hasAttribute("ServerId"))
    {
        qDebug() << "CRCConfiguration::ReadRemoteCareServerSettings:### attribute <ServerId> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ServerId", true);
        return false;
    }
    SetServerId(XmlStreamReader.attributes().value("ServerId").toString().toInt());

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the local device settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::ReadLocalDeviceSettings(const QXmlStreamReader& XmlStreamReader)
{
    // read DeviceType
    if (!XmlStreamReader.attributes().hasAttribute("DeviceType"))
    {
        qDebug() << "CRCConfiguration::ReadLocalDeviceSettings:### attribute <DeviceType> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "DeviceType", true);
        return false;
    }
    QString DeviceType = XmlStreamReader.attributes().value("DeviceType").toString();
    if (NameOfDeviceTypes[RemoteCare::RCDRMDeviceMaster] == DeviceType)
    {
        SetDeviceType(RemoteCare::RCDRMDeviceMaster);
    }
    else if (NameOfDeviceTypes[RemoteCare::RCDRMDeviceManaged] == DeviceType)
    {
        SetDeviceType(RemoteCare::RCDRMDeviceManaged);
    }
    else
    {
        SetDeviceType(RemoteCare::RCDRMDeviceInvalid);
    }

    // read DeviceId
    if (!XmlStreamReader.attributes().hasAttribute("DeviceId"))
    {
        qDebug() << "CRCConfiguration::ReadLocalDeviceSettings:### attribute <DeviceId> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "DeviceId", true);
        return false;
    }
    SetDeviceId(XmlStreamReader.attributes().value("DeviceId").toString().toInt());

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the local Network proxy settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::ReadLocalNetworkProxySettings(const QXmlStreamReader &XmlStreamReader)
{
    if (!XmlStreamReader.attributes().hasAttribute("ProxyProtocol"))
    {
        qDebug() << "CRCConfiguration::ReadLocalNetworkProxySettings:### attribute <ProxyProtocol> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ProxyProtocol", true);
        return false;
    }
    QString ProxyProtocol = XmlStreamReader.attributes().value("ProxyProtocol").toString();
    if (NameOfProxyProtocols[RemoteCare::RCWebProxyProtoNone] == ProxyProtocol)
    {
        SetProxyProtocol(RemoteCare::RCWebProxyProtoNone);
    }
    else if (NameOfProxyProtocols[RemoteCare::RCWebProxyProtoSOCKS] == ProxyProtocol)
    {
        SetProxyProtocol(RemoteCare::RCWebProxyProtoSOCKS);
    }
    else if (NameOfProxyProtocols[RemoteCare::RCWebProxyProtoHTTP] == ProxyProtocol)
    {
        SetProxyProtocol(RemoteCare::RCWebProxyProtoHTTP);
    }
    else
    {
        SetProxyProtocol(RemoteCare::RCWebProxyProtoInvalid);
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the remote session settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::ReadRemoteSessionSettings(const QXmlStreamReader& XmlStreamReader)
{
    if (!XmlStreamReader.attributes().hasAttribute("RemoteSessionName"))
    {
        qDebug() << "CRCConfiguration::ReadRemoteSessionSettings:### attribute <RemoteSessionName> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "RemoteSessionName", true);
        return false;
    }
    SetRemoteSessionName(XmlStreamReader.attributes().value("RemoteSessionName").toString());

    if (!XmlStreamReader.attributes().hasAttribute("RemoteSessionIPAddress"))
    {
        qDebug() << "CRCConfiguration::ReadRemoteSessionSettings:### attribute <RemoteSessionIPAddress> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "RemoteSessionIPAddress", true);
        return false;
    }
    SetRemoteSessionIPAddress(XmlStreamReader.attributes().value("RemoteSessionIPAddress").toString());

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the file upload settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::ReadFileUploadSettings(const QXmlStreamReader &XmlStreamReader)
{
    if (!XmlStreamReader.attributes().hasAttribute("Compression"))
    {
        qDebug() << "CRCConfiguration::ReadRemoteSessionSettings:### attribute <Compression> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "Compression", true);
        return false;
    }
    SetCompression(Global::StringToOnOffState(XmlStreamReader.attributes().value("Compression").toString(), false));

    if (!XmlStreamReader.attributes().hasAttribute("MaxChunkSize"))
    {
        qDebug() << "CRCConfiguration::ReadRemoteSessionSettings:### attribute <MaxChunkSize> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "MaxChunkSize", true);
        return false;
    }
    SetMaxChunkSize(XmlStreamReader.attributes().value("MaxChunkSize").toString().toInt());

    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam RCConfiguration = CRCConfiguration class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(const QDataStream& OutDataStream, const CRCConfiguration& RCConfiguration)
{
    // remove the constant and store it in a local variable
    CRCConfiguration* p_TempRCConfiguration = const_cast<CRCConfiguration*>(&RCConfiguration);
    QDataStream& OutDataStreamRef = const_cast<QDataStream &>(OutDataStream);
    QXmlStreamWriter XmlStreamWriter; //!< Writer for the XML

    // set the IO device
    QIODevice* IODevice = OutDataStreamRef.device();

    XmlStreamWriter.setDevice(IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    // start the XML Document
    XmlStreamWriter.writeStartDocument();
    // write the documnet type declaration
    XmlStreamWriter.writeDTD("<!DOCTYPE RCConfiguration>");
    XmlStreamWriter.writeStartElement("RCConfiguration");
    if (!p_TempRCConfiguration->SerializeContent(XmlStreamWriter, true))
    {
        qDebug() << "CRCConfiguration::Operator Streaming (SerializeContent) failed.";
        const_cast<CRCConfiguration &>(RCConfiguration).m_ErrorMap.insert(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "RCConfiguration");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "RCConfiguration", true);
    }

    // write enddocument
    XmlStreamWriter.writeEndDocument();
    return OutDataStreamRef;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam RCConfiguration = CRCConfiguration class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(const QDataStream& InDataStream, const CRCConfiguration& RCConfiguration)
{
    QDataStream& InDataStreamRef = const_cast<QDataStream &>(InDataStream);
    CRCConfiguration& RCConfigurationRef = const_cast<CRCConfiguration &>(RCConfiguration);
    QXmlStreamReader XmlStreamReader; //!< Reader for the XML
    // store the IO device
    QIODevice* IODevice = InDataStreamRef.device();

    XmlStreamReader.setDevice(IODevice);

    // deserialize the content of the xml
    if (!RCConfigurationRef.DeserializeContent(XmlStreamReader, true))
    {
        qDebug() << "CRCConfiguration::Operator Streaming (DeSerializeContent) failed.";
        RCConfigurationRef.m_ErrorMap.insert(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "RCConfiguration");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "RCConfiguration", true);
    }
    XmlStreamReader.device()->reset();
    qDebug()<<">> Stream OPERATOR "<<XmlStreamReader.device()->readAll();
    return InDataStreamRef;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam RCConfiguration = CRCConfiguration class object
 *
 *  \return CRCConfiguration Class Object
 */
/****************************************************************************/
CRCConfiguration& CRCConfiguration::operator=(const CRCConfiguration& RCConfiguration)
{
    // make sure not same object
    if (this != &RCConfiguration)
    {
        CopyFromOther(RCConfiguration);
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CRCConfiguration::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorMap;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XML file to a CRCDataItem map.
 *
 *  \iparam XmlStreamReader = Instance of XmlStreamReader Class
 *  \iparam CompleteData = Complete class Data , true or false
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfiguration::ReadDataItems(const QXmlStreamReader& XmlStreamReader, const bool& CompleteData)
{
    while (!XmlStreamReader.atEnd())
    {
        if (const_cast<QXmlStreamReader &>(XmlStreamReader).readNext() == QXmlStreamReader::Invalid)
        {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }
        if (XmlStreamReader.isStartElement())
        {
            if (XmlStreamReader.name() == "DataItem")
            {
                CRCDataItem RCDataItem;
                QString DataItem;

                if (!RCDataItem.DeserializeContent(XmlStreamReader, CompleteData))
                {
                    qDebug() << "CRCConfiguration::ReadDataItems failed";
                    return false;
                }
                DataItem = RCDataItem.GetName();

                // Now add this dataitem
                m_RCDataItemMap.insert(DataItem, RCDataItem);
            }

        } // end of start element comparison
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "DataItems")
        {
            break; // exit from while loop
        }
    } // end of while loop
    return true;
}

/****************************************************************************/
/*!
 *  \brief Add a data item
 *
 *  \iparam Name = Nameof data item.
 *  \iparam RCDataItem = Data item to be added to the map.
 *
 *  \return true or false.
 */
/****************************************************************************/
bool CRCConfiguration::AddDataItem(const QString& Name, const CRCDataItem& RCDataItem)
{
    m_RCDataItemMap.insert(Name,RCDataItem);

    return true;
}
/****************************************************************************/
/*!
 *  \brief Update a data item
 *
 *  \iparam Name = Name of data item.
 *  \iparam RCDataItem = Data item to be updated to the map.
 *
 *  \return true or false.
 */
/****************************************************************************/
bool CRCConfiguration::UpdateDataItem(const QString& Name, const CRCDataItem& RCDataItem)
{
    if(m_RCDataItemMap.contains(Name))
    {
        m_RCDataItemMap.remove(Name);
        m_RCDataItemMap.insert(Name, RCDataItem);
        return true;
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief Update a data item
 *
 *  \iparam Name = Name of data item.
 *
 *  \return true or false.
 */
/****************************************************************************/
bool CRCConfiguration::DeleteDataItem(const QString& Name)
{
    if(m_RCDataItemMap.contains(Name))
    {
        m_RCDataItemMap.remove(Name);
        return true;
    }

    return false;
}

} // end namespace DataManager
