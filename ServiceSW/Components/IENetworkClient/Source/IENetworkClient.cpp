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
IENetworkClient::IENetworkClient(QString IPAddress, QString UserName, QString ApplicationDirPath):
    m_IPAddress(IPAddress)
  , m_UserName(UserName)
  , m_ApplicationDirPath(ApplicationDirPath)
  , m_HostPingParameter("ping_SaM_server")
  , m_AccessRightsParameter("test_file_copy_to_SaM_server")
  , m_SendReportParameter("send_reports_to_SaM_server")
  , m_CheckNewFileParameter("check_if_SaM_server_has_new_firmware")
  , m_DownloadFileParameter("copy_firmware_files_from_SaM_server")
{    
    m_SshAddress = m_UserName + "@" + m_IPAddress;
    m_ScriptName = m_ApplicationDirPath + "/EBox-Utils-Manufacturing-Support.sh";
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
    if (QFile::exists(m_ScriptName))
    {
        Command.append(m_ScriptName);
        Command.append(" ");
        Command.append(m_HostPingParameter);
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
bool IENetworkClient::PerformAccessRightsCheck(QString& ServicePath)
{
    QString Command;
    if (QFile::exists(m_ScriptName))
    {
        Command.append(m_ScriptName);
        Command.append(" ");
        Command.append(m_AccessRightsParameter);
        Command.append(" ");
        Command.append(m_ScriptName);
        Command.append(" ");
        Command.append(m_SshAddress);
        Command.append(" ");
        Command.append(ServicePath);

        qDebug()<<"Executing command "<<Command;
        int ret = system(Command.toStdString().c_str());
        if(0 == ret)
        {
            return true;
        }
    }

    return false;
}

/****************************************************************************/
bool IENetworkClient::SendReprotFile(QString& ReportFile, QString& ReportPath)
{
    QString Command;
    if (QFile::exists(m_ScriptName))
    {
        Command.append(m_ScriptName);
        Command.append(" ");
        Command.append(m_SendReportParameter);
        Command.append(" ");
        Command.append(ReportFile);
        Command.append(" ");
        Command.append(m_SshAddress);
        Command.append(" ");
        Command.append(ReportPath);

        qDebug()<<"Executing command "<<Command;
        int ret = system(Command.toStdString().c_str());
        if(0 == ret)
        {
            return true;
        }
    }

    return false;
}

/****************************************************************************/
bool IENetworkClient::CheckForNewFiles(QString& FilePath)
{
    QString Command;
    if (QFile::exists(m_ScriptName))
    {
        Command.append(m_ScriptName);
        Command.append(" ");
        Command.append(m_CheckNewFileParameter);
        Command.append(" ");
        Command.append(m_SshAddress);
        Command.append(" ");
        Command.append(FilePath);

        qDebug()<<"Executing command "<<Command;
        int ret = system(Command.toStdString().c_str());
        if(0 == ret)
        {
            return true;
        }
    }

    return false;
}

/****************************************************************************/
bool IENetworkClient::DownloadFiles(QString& DestPath)
{
    QString Command;
    if (QFile::exists(m_ScriptName))
    {
        Command.append(m_ScriptName);
        Command.append(" ");
        Command.append(m_DownloadFileParameter);
        Command.append(" ");
        Command.append(m_SshAddress);
        Command.append(" ");
        Command.append(DestPath);

        qDebug()<<"Executing command "<<Command;
        int ret = system(Command.toStdString().c_str());
        if(0 == ret)
        {
            return true;
        }
    }

    return false;
}

}   // end of namespace NetworkClient

