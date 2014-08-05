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
    QString m_UserName;
    QString m_SshAddress;
    QString m_ApplicationDirPath;
    QString m_ScriptName;
    QString m_HostPingParameter;
    QString m_AccessRightsParameter;
    QString m_SendReportParameter;
    QString m_CheckNewFileParameter;
    QString m_DownloadFileParameter;
public:

    IENetworkClient(QString IPAddress, QString UserName, QString ApplicationDirPath);
    bool SetIPAddress(QString IPAddress);

    bool PerformHostReachableTest();
    bool PerformAccessRightsCheck(QString& ServicePath);
    bool SendReprotFile(QString& ReportFile, QString& ReportPath);
    bool CheckForNewFiles(QString& FilePath);
    bool DownloadFiles(QString& DestPath);

};

}   // end of namespace NetworkClient


#endif // IE_NETWORKCLIENT_H
