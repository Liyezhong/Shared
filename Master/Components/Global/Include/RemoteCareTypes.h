/****************************************************************************/
/*! \file RemoteCareTypes.h
 *
 *  \brief RemoteCare definition file (Axeda specific types).
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 27.02.2013
 *  $Author:    $ T.Scheck
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2013 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef REMOTECARETYPES_H
#define REMOTECARETYPES_H

namespace RCAgentNamespace {

/// Type of data item value
typedef enum {
    ADI_Analog,         //!< analog
    ADI_Digital,        //!< digital (0/1)
    ADI_String,         //!< string
    ADI_Undefined       //!< undefined
} AxedaDataItemType_t;

/// Quality of the data item value
typedef enum {
    ADI_DataGood,       //!< good
    ADI_DataBad,        //!< bad
    ADI_DataUncertain   //!< uncertain (undefined)
} AxedaDataItemQuality_t;

/// Range for the digital data type
typedef enum {
    ADI_DataNull,       //!< digital 0
    ADI_DataOne,        //!< digital 1
    ADI_DataError       //!< erroneous value
} AxedaDigitalDataItemRange_t;
}


namespace RemoteCare {

/****************************************************************************/
/**
 * \brief Proxy server protocol.
 */
/****************************************************************************/
typedef enum
{
    RCWebProxyProtoNone,            //!< no proxy
    RCWebProxyProtoSOCKS,           //!< SOCKS proxy
    RCWebProxyProtoHTTP,             //!< HTTP proxy
    RCWebProxyProtoInvalid          //!< Invalid value
} RCWebProxyProtocol_t;

/****************************************************************************/
/**
 * \brief SSL encryption level.
 */
/****************************************************************************/
typedef enum
{
    RCWebCryptoNone,                /* no encryption (SSL disabled) */
    RCWebCryptoLow,                 /* low-strength cipher */
    RCWebCryptoMedium,              /* medium-strength cipher */
    RCWebCryptoHigh,                 /* high-strength cipher */
    RCWebCryptoInvalid                 /* high-strength cipher */
}RCWebCryptoLevel_t;

/****************************************************************************/
/**
 * \brief Device type.
 */
/****************************************************************************/
typedef enum
{
    RCDRMDeviceMaster,              /* master device */
    RCDRMDeviceManaged,              /* managed device (Gateway only) */
    RCDRMDeviceInvalid
}RCDRMDeviceType_t;

/****************************************************************************/
/**
 * \brief Configuration type for the Enterprise server - Primary, Backup, Additional.
*/
/****************************************************************************/
typedef enum
{
    RCDRMServerConfigPrimary,       /* primary Enterprise server */
    RCDRMServerConfigAdditional,    /* additional Enterprise server */
    RCDRMServerConfigBackup,         /* backup Enterprise server */
    RCDRMServerConfigInvalid
} RCDRMServerConfigType_t;

/****************************************************************************/
/**
 * \brief remote care data item types.
 */
/****************************************************************************/
typedef enum {
    RDI_Analog,         //!< analog
    RDI_Digital,        //!< digital (0/1)
    RDI_String,         //!< string
    RDI_Undefined,       //!< undefined
    RDI_TypeInvalid
} RCDataItemType_t;

/****************************************************************************/
/**
 * \brief remote care data item quality.
 */
/****************************************************************************/
typedef enum {
    RDI_DataGood,       //!< good
    RDI_DataBad,        //!< bad
    RDI_DataUncertain,   //!< uncertain (undefined)
    RDI_DataInvalid
} RCDataItemQuality_t;

const QString RC_DATAITEM_REQUEST_REMOTE_SESSION    = "RequestRemoteSession";   //!< Request remote session
const QString RC_DATAITEM_REQUEST_ASSET_INFO        = "RequestAssetInformation"; //!< request asset info
const QString RC_DATAITEM_SET_EVENT_CLASS           = "SetEventClass";          //!< set event class
const QString RC_DATAITEM_SET_EVENT_PRIORITY        = "SetEventPriority";       //!< set event priority
const QString RC_DATAITEM_SET_LOG_NUMBER            = "SetLogNumber";           //!< set number of log files
const QString RC_DATAITEM_SET_SUBSCRIPTION          = "SetSubscription";        //!< set subscription
const QString RC_DATAITEM_SET_UPDATE_AVAILABLE      = "SetUpdateAvailable";     //!< set update available
const QString RC_DATAITEM_SET_DOWNLOAD_FINISHED     = "SetDownloadFinished";    //!< set download finished
const QString RC_DATAITEM_SET_PING_RATE             = "SetPingRate";            //!< set ping rate
const QString RC_DATAITEM_ENABLE_SOFTWARE_UPDATE    = "EnableSoftwareUpdate";   //!< enable software update
const QString RC_DATAITEM_STATE_ASSET_COMPLETE      = "StateAssetComplete";     //!< state asset complete
const QString RC_DATAITEM_STATE_DOWNLOAD_FOLDER     = "StateDownloadFolder";    //!< state download folder
const QString RC_DATAITEM_STATE_SESSION_TYPE        = "StateSessionType";       //!< state remote session type
const QString RC_DATAITEM_STATE_SESSION_PORT        = "StateSessionPort";       //!< state remote session port
} // end namespace

#endif // REMOTECARETYPES_H

