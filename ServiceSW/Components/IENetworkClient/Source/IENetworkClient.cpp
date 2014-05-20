/****************************************************************************/
/*! \file IENetworkClient.cpp
 *
 *  \brief This class provides implementation for accessing Manufacturing Server.
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

#include "../Include/PlatformServiceEventCodes.h"
#include "IENetworkClient/Include/IENetworkClient.h"
#include <QFile>
#include <QApplication>
#include <QDebug>

namespace NetworkClient {


/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam IPAddress = IPAddress
 */
/****************************************************************************/
IENetworkClient::IENetworkClient(QString IPAddress, QString ApplicationDirPath):
    m_IPAddress(IPAddress)
  , m_ApplicationDirPath(ApplicationDirPath)
  , m_HostPingScriptName("Ping.sh")
  , m_ServiceAvailableScriptName("sshServiceAvaiable.sh")
  , m_AccessRightsScriptName("sshAccessRights.sh")
  , m_CheckForNewFilesScriptName("sshCheckForNewFiles.sh")
  , m_FileDownloadScriptName("sshDownloadFiles.sh")
{    

}

/****************************************************************************/
bool IENetworkClient::SetIPAddress(QString IPAddress)
{
    m_IPAddress = IPAddress;
    return true;
}

/****************************************************************************/
bool IENetworkClient::PerformHostReachableTest()
{
    QString Command("");
    QString FilePath(m_ApplicationDirPath);
    FilePath.append("/");
    FilePath.append(m_HostPingScriptName);
    if (QFile::exists(FilePath))
    {
        Command.append("chmod 755 ");
        Command.append(m_HostPingScriptName);
        qDebug()<<"Executing command "<<Command;
        (void) system(Command.toStdString().c_str());
        Command.clear();
        Command.append("./");
        Command.append(m_HostPingScriptName);
        Command.append(" ");
        Command.append(m_IPAddress);
        qDebug()<<"Executing command "<<Command;
        if(0 == system(Command.toStdString().c_str()))
        {
            return true;
        }
    }
    return false;
}

/****************************************************************************/
bool IENetworkClient::PerformServiceAvailableTest()
{
    return false;
}

/****************************************************************************/
bool IENetworkClient::PerformAccessRightsCheck()
{
    return false;
}

/****************************************************************************/
bool IENetworkClient::CheckForNewFiles()
{
    return false;
}

/****************************************************************************/
bool IENetworkClient::DownloadFiles()
{
    return false;
}

}   // end of namespace NetworkClient

