/****************************************************************************/
/*! \file  Platform/Master/Components/DataManager/Containers/RCConfiguration/Source/RCConfigurationVerifier.cpp
 *
 *  \brief Implementation file for class RCConfigurationVerifier.
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

#include <QDebug>
#include <QFile>

#include "DataManager/Containers/RCConfiguration/Include/RCConfigurationVerifier.h"
#include "DataManager/Containers/RCConfiguration/Include/RCConfiguration.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"
//lint -e641

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CRCConfigurationVerifier::CRCConfigurationVerifier() : mp_RCConfigurationInterface(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief  Verifies rc configuration
 *
 *  \iparam p_RCConfigurationInterface = rc configuration interface class object
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRCConfigurationVerifier::VerifyData(CDataContainerBase* p_RCConfigurationInterface)
{    
    // by default make the verification flag to true
    bool VerifiedData = true;
    try
    {
        CHECKPTR(p_RCConfigurationInterface)
        // to store the error description
        QString ErrorDescription;

        // assign pointer to member variable
        mp_RCConfigurationInterface = static_cast<CRCConfigurationInterface*>
                (const_cast<CDataContainerBase*>(p_RCConfigurationInterface));
        CHECKPTR(mp_RCConfigurationInterface)
        // check content of user settings
        CRCConfiguration* p_RCConfiguration = mp_RCConfigurationInterface->GetRCConfiguration();
        CHECKPTR(p_RCConfiguration)

        CheckGeneralSettings(p_RCConfiguration, VerifiedData);

        // check the htpp secure connection settings
        CheckHTTPSecureConnectionSettings(p_RCConfiguration, VerifiedData);

        CheckRemoteCareServerSettings(p_RCConfiguration, VerifiedData);

        CheckLocalDeviceSettings(p_RCConfiguration, VerifiedData);

        // check network settings parameters
        CheckLocalNetWorkSettings(p_RCConfiguration, VerifiedData);

        CheckRemoteSessionSettings(p_RCConfiguration, VerifiedData);

        CheckFileUploadSettings(p_RCConfiguration, VerifiedData);

        CheckDataItems(p_RCConfiguration, VerifiedData);

        return VerifiedData;
    }
    CATCHALL();

    return false;
}

/****************************************************************************/
/*!
 *  \brief  Retrieve errors occured during verification
 *
 *  \return  ErrorMap
 */
/****************************************************************************/
ErrorMap_t& CRCConfigurationVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorMap;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CRCConfigurationVerifier::ResetErrors()
{
    m_ErrorMap.clear();
}

/****************************************************************************/
/*!
 *  \brief  Returns whether verifier is a local verifier or not
 *
 *  \return bool - On successful (True) otherwise (False)
 */
/****************************************************************************/
bool CRCConfigurationVerifier::IsLocalVerifier()
{
    return true;
}
/****************************************************************************/
/*!
 *  \brief  Check general Settings.
 *
 *  \iparam p_RCConfiguration - Pointer to CRCConfiguration
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CRCConfigurationVerifier::CheckGeneralSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData)
{
    if(!CheckDataRange(p_RCConfiguration->GetQueueSize(), MIN_QUEUE_SIZE, MAX_QUEUE_SIZE,
                       EVENT_DM_RC_QUEUE_SIZE_OUTOF_RANGE))
    {
        qDebug() << "queue size is out of range";
        VerifiedData = false;
    }

    if(!CheckOnOffState(p_RCConfiguration->GetHTTPConnectionPersistence(), EVENT_DM_RC_INVALID_HTTPCON_PERSISTENCE_ONOFFSTATE))
    {
        qDebug() << "invalid http connection persistance state";
        VerifiedData = false;
    }

    if(!CheckOnOffState(p_RCConfiguration->GetDebug(), EVENT_DM_RC_INVALID_DEBUG_ONOFFSTATE))
    {
        qDebug() << "invalid debug state";
        VerifiedData = false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Check HTTP secure connection Settings.
 *
 *  \iparam p_RCConfiguration - Pointer to CRCConfiguration
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CRCConfigurationVerifier::CheckHTTPSecureConnectionSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData)
{
    if(!CheckOnOffState(p_RCConfiguration->GetHTTPSecureConnection(), EVENT_DM_RC_INVALID_HTTPSECURE_CON_ONOFFSTATE))
    {
        qDebug() << "invalid http secure connection state";
        VerifiedData = false;
    }

    if(!CheckDataRange((qint64)p_RCConfiguration->GetEncryptionLevel(), (qint64)RemoteCare::RCWebCryptoNone,
                       (qint64)RemoteCare::RCWebCryptoHigh, EVENT_DM_RC_ENCRYPTION_OUTOF_RANGE))
    {
        qDebug() << "encryption level is out of range";
        VerifiedData = false;
    }

    if(!CheckOnOffState(p_RCConfiguration->GetAuthentication(), EVENT_DM_RC_INVALID_AUTHENTICATION_ONOFFSTATE))
    {
        qDebug() << "invalid authentication state";
        VerifiedData = false;
    }

    if(!CheckStringLenghtRange(p_RCConfiguration->GetCertificateFileName(), MIN_CERT_FILENAME_LENGTH,
                               MAX_CERT_FILENAME_LENGTH, EVENT_DM_RC_CERT_FILENAME_LENGHT_OUTOF_RANGE))
    {
        qDebug() << "certificate filename lenght is out of range";
        VerifiedData = false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Check remote care server Settings.
 *
 *  \iparam p_RCConfiguration - Pointer to CRCConfiguration
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CRCConfigurationVerifier::CheckRemoteCareServerSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData)
{
    if(!CheckDataRange(p_RCConfiguration->GetExecTime(), MIN_EXEC_TIME, MAX_EXEC_TIME,
                       EVENT_DM_RC_EXEC_TIME_OUTOF_RANGE))
    {
        qDebug() << "ExecTime is out of range";
        VerifiedData = false;
    }

    if(!CheckDataRange((qint32)p_RCConfiguration->GetServerType(), (qint32)RemoteCare::RCDRMServerConfigPrimary,
                       (qint32)RemoteCare::RCDRMServerConfigBackup, EVENT_DM_RC_SERVERTYPE_OUTOF_RANGE))
    {
        qDebug() << "server type is out of range";
        VerifiedData = false;
    }

    if(!CheckStringLenghtRange(p_RCConfiguration->GetTargetDataBase(), MIN_TARGET_DATABASE_LENGTH,
                               MAX_TARGET_DATABASE_LENGTH, EVENT_DM_RC_TARGET_DATABASE_LENGHT_OUTOF_RANGE))
    {
        qDebug() << "target database lenght is out of range";
        VerifiedData = false;
    }

    if(!CheckStringLenghtRange(p_RCConfiguration->GetServerURL(), MIN_SERVER_URL_LENGTH,
                               MAX_SERVER_URL_LENGTH, EVENT_DM_RC_SERVER_URL_LENGHT_OUTOF_RANGE))
    {
        qDebug() << "server url lenght is out of range";
        VerifiedData = false;
    }

    if(!CheckDataRange(p_RCConfiguration->GetServerId(), MIN_SERVER_ID, MAX_SERVER_ID,
                       EVENT_DM_RC_SERVER_ID_OUTOF_RANGE))
    {
        qDebug() << "server id out of range";
        VerifiedData = false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Check local device Settings.
 *
 *  \iparam p_RCConfiguration - Pointer to CRCConfiguration
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CRCConfigurationVerifier::CheckLocalDeviceSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData)
{
    if(!CheckDataRange((qint32)p_RCConfiguration->GetDeviceType(), (qint32)RemoteCare::RCDRMDeviceMaster,
                       (qint32)RemoteCare::RCDRMDeviceManaged, EVENT_DM_RC_DEVICETYPE_OUTOF_RANGE))
    {
        qDebug() << "device type is out of range";
        VerifiedData = false;
    }

    if(!CheckDataRange(p_RCConfiguration->GetDeviceId(), MIN_DEVICE_ID, MAX_DEVICE_ID,
                       EVENT_DM_RC_DEVICE_ID_OUTOF_RANGE))
    {
        qDebug() << "device id out of range";
        VerifiedData = false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Check local Network Settings.
 *
 *  \iparam p_RCConfiguration - Pointer to CRCConfiguration
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CRCConfigurationVerifier::CheckLocalNetWorkSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData)
{
   if(!CheckDataRange((qint32)p_RCConfiguration->GetProxyProtocol(), (qint32)RemoteCare::RCWebProxyProtoNone,
                       (qint32)RemoteCare::RCWebProxyProtoHTTP, EVENT_DM_RC_PROXY_PROTOCOL_OUTOF_RANGE))
    {
        qDebug() << "proxy protocol is out of range";
        VerifiedData = false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Check local Network Settings.
 *
 *  \iparam p_RCConfiguration - Pointer to CRCConfiguration
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CRCConfigurationVerifier::CheckRemoteSessionSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData)
{
    if(!CheckStringLenghtRange(p_RCConfiguration->GetRemoteSessionName(), MIN_REMOTESESSION_NAME_LENGTH,
                               MAX_REMOTESESSION_NAME_LENGTH, EVENT_DM_RC_REMOTESESSION_NAME_LENGHT_OUTOF_RANGE))
    {
        qDebug() << "REMOTE SESSIONNAME LENGHT OUT OF RANGE";
        VerifiedData = false;
    }

    if (!(CheckIPAddress(p_RCConfiguration->GetRemoteSessionIPAddress())))
    {
        qDebug() << "REMOTE SESSION IP ADDRESS NOT VALID";
        m_ErrorMap.insert(EVENT_DM_RC_INVALID_REMOTESESSION_IP_ADDRESS, Global::tTranslatableStringList() << p_RCConfiguration->GetRemoteSessionIPAddress());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_RC_INVALID_REMOTESESSION_IP_ADDRESS, Global::tTranslatableStringList() <<  p_RCConfiguration->GetRemoteSessionIPAddress(), true);
        VerifiedData = false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Check file upload Settings.
 *
 *  \iparam p_RCConfiguration - Pointer to CRCConfiguration
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CRCConfigurationVerifier::CheckFileUploadSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData)
{
    if(!CheckOnOffState(p_RCConfiguration->GetCompression(), EVENT_DM_RC_INVALID_COMPRESSION_ONOFFSTATE))
    {
        qDebug() << "not a valid state for compression";
        VerifiedData = false;
    }

    if(!CheckDataRange(p_RCConfiguration->GetMaxChunkSize(), MIN_MAXCHUNKSIZE, MAX_MAXCHUNKSIZE,
                       EVENT_DM_RC_MAXCHUNKSIZE_OUTOF_RANGE))
    {
        qDebug() << "max chunk size is out of range";
        VerifiedData = false;
    }
}

/****************************************************************************/
/*!
 *  \brief Check IP Address of local proxy Network/ remote session Settings.
 *
 *  \iparam IPAddress - Ip address to be checked
 *  \return True - If IP address is valid else False
 */
/****************************************************************************/
bool CRCConfigurationVerifier::CheckIPAddress(const QString& IPAddress)
{
    qDebug()<<"CRCConfigurationVerifier::CheckIPAddress, Count= "<< IPAddress.split(".").count();
    bool VerifiedIPAddress = false;
    if (IPAddress.split(".").count() == 4)
    {
        for (int SplitCount = 0; SplitCount < IPAddress.split(".").count(); SplitCount++)
        {
            QString AddressNumberString = (IPAddress.split(".").value(SplitCount));
            if (AddressNumberString.length() <= 3)
            {
                bool Result = false;
                int AddressNumber = AddressNumberString.toInt(&Result, 10);
                if (Result)
                {
                    qDebug()<<"CRCConfigurationVerifier::CheckIPAddress, AddressNumber ="<< AddressNumber;
                    if (AddressNumber < MIN_RS_IP_ADDRESS_NUMBER || AddressNumber > MAX_RS_IP_ADDRESS_NUMBER || AddressNumberString == "")
                    {
                        return false;
                    }
                    else if (SplitCount == 3)
                    {
                        if (AddressNumber == 0) {
                            return false;
                        }
                        else
                        {
                            VerifiedIPAddress = true;
                        }
                    }
                    else
                    {
                        VerifiedIPAddress = true;
                    }
                }
                else
                {
                    VerifiedIPAddress = false;
                    return VerifiedIPAddress;
                }
            }
            else
            {
                VerifiedIPAddress = false;
                return VerifiedIPAddress;
            }
        }
    }
    else {
        VerifiedIPAddress = false;
    }
    return VerifiedIPAddress;
}

/****************************************************************************/
/*!
 *  \brief Check Proxy IP Port of local proxy Network/ remote session Settings.
 *
 *  \iparam IPPort - IP port to be checked
 *  \iparam EventId - event id to log in case of error
 *  \return True - If IP address is valid else False
 */
/****************************************************************************/
bool CRCConfigurationVerifier::CheckIPPort(const int& IPPort, const quint32& EventId)
{
    if (IPPort < MIN_IP_PORT || IPPort > MAX_IP_PORT)
    {
        m_ErrorMap.insert(EventId, Global::tTranslatableStringList() << QString::number(IPPort) << MIN_IP_PORT
                                                                    << MAX_IP_PORT);
        Global::EventObject::Instance().RaiseEvent(EventId,
                                                   Global::tTranslatableStringList() << QString::number(IPPort)
                                                                                    << MIN_IP_PORT
                                                                                    << MAX_IP_PORT , true);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Check ON/Off state of a Global::OnOffState variable.
 *
 *  \iparam Attribute - variable to be checked
 *  \iparam EventId - event id to log in case of error
 *  \return True - If valid else False
 */
/****************************************************************************/
bool CRCConfigurationVerifier::CheckOnOffState(const Global::OnOffState& Attribute, const quint32& EventId)
{
    if (!((Attribute == Global::ONOFFSTATE_ON || Attribute == Global::ONOFFSTATE_OFF)))
    {
        m_ErrorMap.insert(EventId, Global::tTranslatableStringList() << Attribute);
        Global::EventObject::Instance().RaiseEvent(EventId, Global::tTranslatableStringList() << Attribute, true);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Check lenght of a QString variable against a minimum and maximum value.
 *
 *  \iparam Attribute - variable to be checked
 *  \iparam MinLenght - minimum lenght
 *  \iparam MaxLenght - maximum lenght
 *  \iparam EventId - event id to log in case of error
 *  \return True - If valid else False
 */
/****************************************************************************/
bool CRCConfigurationVerifier::CheckStringLenghtRange(const QString& Attribute, const int& MinLenght,
                                                      const int& MaxLenght, const quint32& EventId)
{
    if (!((Attribute.isEmpty() != true && Attribute.length() >= MinLenght && Attribute.length() <= MaxLenght)))
    {
        m_ErrorMap.insert(EventId, Global::tTranslatableStringList() << Attribute
                                                                    << QString::number(MinLenght)
                                                                    << QString::number(MaxLenght));
        Global::EventObject::Instance().RaiseEvent(EventId, Global::tTranslatableStringList() << Attribute
                                                                                << QString::number(MinLenght)
                                                                                << QString::number(MaxLenght) , true);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Check value of a variable against a minimum and maximum value.
 *
 *  \iparam Attribute - variable to be checked
 *  \iparam MinValue - minimum value
 *  \iparam MaxValue - maximum value
 *  \iparam EventId - event id to log in case of error
 *  \return True - If valid else False
 */
/****************************************************************************/
bool CRCConfigurationVerifier::CheckDataRange(const quint64& Attribute, const quint64& MinValue,
                                              const quint64& MaxValue, const quint32& EventId)
{
    if (Attribute < MinValue || Attribute > MaxValue)
    {
        m_ErrorMap.insert(EventId, Global::tTranslatableStringList() << QString::number(Attribute)
                                                                    << QString::number(MinValue)
                                                                    << QString::number(MaxValue));
        Global::EventObject::Instance().RaiseEvent(EventId,
                                                   Global::tTranslatableStringList() << QString::number(Attribute)
                                                                                    << QString::number(MinValue)
                                                                                    << QString::number(MaxValue) , true);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Check the data items.
 *
 *  \iparam p_RCConfiguration - Pointer to CRCConfiguration
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CRCConfigurationVerifier::CheckDataItems(CRCConfiguration* p_RCConfiguration, bool& VerifiedData)
{
    const int DataItemCount = p_RCConfiguration->GetDataItemCount();

    for(int Index=0; Index<DataItemCount; Index++)
    {
        CRCDataItem DataItem;

        p_RCConfiguration->GetDataItem(Index, DataItem);

        if( ((qint32)DataItem.GetType() < (qint32)RemoteCare::RDI_Analog) ||
                ((qint32)DataItem.GetType() > (qint32)RemoteCare::RDI_Undefined))
        {
            qDebug() << "dataitem type is out of range";

            m_ErrorMap.insert(EVENT_DM_RC_DATAITEM_TYPE_INVALID,
                              Global::tTranslatableStringList() << DataItem.GetName());

            Global::EventObject::Instance().RaiseEvent(EVENT_DM_RC_DATAITEM_TYPE_INVALID,
                                                       Global::tTranslatableStringList() << DataItem.GetName() , true);
            VerifiedData = false;
        }

        if( ((qint32)DataItem.GetQuality() < (qint32)RemoteCare::RDI_DataGood) ||
                ((qint32)DataItem.GetQuality() > (qint32)RemoteCare::RDI_DataUncertain))
        {
            qDebug() << "dataitem quality is out of range";

            m_ErrorMap.insert(EVENT_DM_RC_DATAITEM_QUALITY_INVALID,
                              Global::tTranslatableStringList() << DataItem.GetName());

            Global::EventObject::Instance().RaiseEvent(EVENT_DM_RC_DATAITEM_QUALITY_INVALID,
                                                       Global::tTranslatableStringList() << DataItem.GetName() , true);
            VerifiedData = false;
        }
    }
}

}  // namespace DataManager


