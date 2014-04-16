/****************************************************************************/
/*! \file Components/DataManager/Containers/RCConfiguration/Include/RCConfigurationVerifier.h
 *
 *  \brief RCConfigurationVerifier definition.
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
#ifndef DATAMANAGER_CRCConfigurationVERIFIER_H
#define DATAMANAGER_CRCConfigurationVERIFIER_H

#include <QString>
#include <QXmlStreamReader>

#include "DataManager/Containers/RCConfiguration/Include/RCConfigurationInterface.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "Global/Include/EventObject.h"
//lint -e1540

namespace DataManager {

const quint32 MIN_QUEUE_SIZE   = 10; ///< Minimum value of queue size
const quint32 MAX_QUEUE_SIZE   = 10000000; ///< Maximum value of queue size

const quint32 MIN_EXEC_TIME   = 1; ///< Minimum value of exec time
const quint32 MAX_EXEC_TIME   = 60; ///< Maximum value of exec time

const int MIN_CERT_FILENAME_LENGTH = 10;///< Minimum length
const int MAX_CERT_FILENAME_LENGTH = 100;///< Maximum length

const int MIN_TARGET_DATABASE_LENGTH = 10;///< Minimum length
const int MAX_TARGET_DATABASE_LENGTH = 100;///< Maximum length

const int MIN_SERVER_URL_LENGTH = 10;///< Minimum length
const int MAX_SERVER_URL_LENGTH = 100;///< Maximum length

const quint32 MIN_SERVER_ID   = 12; ///< Minimum value of server id
const quint32 MAX_SERVER_ID   = 10000002; ///< Maximum value of server id

const quint32 MIN_DEVICE_ID   = 10; ///< Minimum value of device id
const quint32 MAX_DEVICE_ID   = 10000003; ///< Maximum value of device id

const int MIN_IP_PORT   = 1; ///< Minimum value of IP/remote session Port
const int MAX_IP_PORT   = 65535; ///< Maximum value of IP/remote session Port

const int MIN_RS_IP_ADDRESS_NUMBER = 0;    ///< Minimum value of IP Address Number
const int MAX_RS_IP_ADDRESS_NUMBER = 255;  ///< Maximum value of IP Address Number

const int MIN_REMOTESESSION_NAME_LENGTH = 10;///< Minimum length
const int MAX_REMOTESESSION_NAME_LENGTH = 100;///< Maximum length

const int MIN_REMOTESESSION_TYPE_LENGTH = 5;///< Minimum length
const int MAX_REMOTESESSION_TYPE_LENGTH = 100;///< Maximum length

const quint32 MIN_MAXCHUNKSIZE   = 10; ///< Minimum value of max xhunk size for file upload
const quint32 MAX_MAXCHUNKSIZE   = 2000000; ///< Maximum value of max xhunk size for file upload


/****************************************************************************/
/*!
 *  \brief  This class implements a list of rack data verifiers.
 */
/****************************************************************************/
class CRCConfigurationVerifier : public IVerifierInterface
{
public:
    CRCConfigurationVerifier();

    bool VerifyData(CDataContainerBase* p_RCConfigurationInterface);  // use concrete class for concrete verifier

    ErrorMap_t &GetErrors();

    void ResetErrors();
    bool IsLocalVerifier();

protected:
    CRCConfigurationInterface* mp_RCConfigurationInterface;   ///< Local pointer to a rack list
    ErrorMap_t m_ErrorMap;          //!< To store Error ID and any arguments associated
private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CRCConfigurationVerifier)

    void CheckGeneralSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData);
    void CheckHTTPSecureConnectionSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData);
    void CheckRemoteCareServerSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData);
    void CheckLocalDeviceSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData);
    void CheckLocalNetWorkSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData);
    void CheckRemoteSessionSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData);
    void CheckFileUploadSettings(CRCConfiguration* p_RCConfiguration, bool& VerifiedData);
    bool CheckIPAddress(QString IPAddress);
    bool CheckIPPort(int IPPort, const quint32 EventId);
    bool CheckOnOffState(Global::OnOffState Attribute, const quint32 EventId);
    bool CheckStringLenghtRange(const QString Attribute, const int MinLenght,
                                const int MaxLenght, const quint32 EventId);
    bool CheckDataRange(quint64 Attribute, quint64 MinValue, quint64 MaxValue, const quint32 EventId);
    void CheckDataItems(CRCConfiguration* p_RCConfiguration, bool& VerifiedData);
};

} // namespace DataManager
#endif // DATAMANAGER_CRCConfigurationVERIFIER_H
