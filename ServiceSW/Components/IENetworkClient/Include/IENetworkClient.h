/****************************************************************************/
/*! \file IENetworkClient.h
 *
 *  \brief IENetworkClient class definition
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-04-23
 *   $Author:  $ Srivathsa HH
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

#ifndef IE_NETWORKCLIENT_H
#define IE_NETWORKCLIENT_H

#include <QString>

namespace NetworkClient {

/****************************************************************************/
/**
 * \brief This class handles the USB Key Validation.
 */
/****************************************************************************/
class IENetworkClient
{
    QString m_IPAddress;
    QString m_ApplicationDirPath;
    QString m_HostPingScriptName;
    QString m_ServiceAvailableScriptName;
    QString m_AccessRightsScriptName;
    QString m_CheckForNewFilesScriptName;
    QString m_FileDownloadScriptName;

public:

    IENetworkClient(QString IPAddress, QString ApplicationDirPath);
    bool SetIPAddress(QString IPAddress);

    bool PerformHostReachableTest();
    bool PerformServiceAvailableTest();
    bool PerformAccessRightsCheck();
    bool CheckForNewFiles();
    bool DownloadFiles();

};

}   // end of namespace NetworkClient


#endif // IE_NETWORKCLIENT_H
