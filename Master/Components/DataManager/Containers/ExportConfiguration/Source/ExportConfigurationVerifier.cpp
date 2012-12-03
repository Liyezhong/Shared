/****************************************************************************/
/*! \file ExportConfigurationVerifier.cpp
 *
 *  \brief Implementation file for class CExportConfigurationVerifier.
 *         This class verifies the CExportConfiguration container data
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-25
 *  $Author:    $ Raju
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
#include <QDir>
#include "DataManager/Containers/ExportConfiguration/Include/ExportConfigurationVerifier.h"
#include "DataManager/Containers/ExportConfiguration/Include/ExportConfiguration.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Constructor
 */
/****************************************************************************/
CExportConfigurationVerifier::CExportConfigurationVerifier() : mp_ECD(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief  Verifies export configuration
 *
 *  \iparam p_Configuration = Export configuration data
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CExportConfigurationVerifier::VerifyData(CDataContainerBase* p_Configuration)
{
    // to store the error description
    QString ErrorDescription;
    // by default make the verification flag to true
    bool VerifiedData = true;
    // assign pointer to member variable
    mp_ECD = static_cast<CExportConfiguration*>(p_Configuration);

    // check the existence of the container
    if (mp_ECD != NULL) {
        // check the source directory existence
        if(mp_ECD->GetSourceDir().compare("") == 0) {
            VerifiedData = false;
        }

        // check the destination directory existence
        if(mp_ECD->GetTargetDir().compare("") != 0) {           
            if(mp_ECD->GetTargetFileName().compare("") == 0) {
                VerifiedData = false;
            }
        }
        else {
            VerifiedData = false;
        }

        // check the user configuration flg so that verifier can verify only the user configuration
        if (mp_ECD->GetServiceConfigurationFlag()) {
            // check the service configuration
            if (!CheckFileNames(mp_ECD->GetServiceConfiguration().GetServiceConfigurationList())) {
                VerifiedData = false;
            }
        }

        // check the user configuration flg so that verifier can verify only the user configuration
        if (mp_ECD->GetUserConfigurationFlag()) {
            // check user configuration
            if (!CheckFileNames(mp_ECD->GetUserConfiguration().GetUserConfigurationList())) {
                VerifiedData = false;
            }
            // check user report configuration
            if (!CheckFileNames(mp_ECD->GetUserConfiguration().GetUserReportList())) {
                VerifiedData = false;
            }
        }

        qDebug() << "Error Description" << ErrorDescription;

        if (!VerifiedData) {
            // store error string with verification code
            //m_ErrorsList.append(QString("0,") + ErrorDescription);
        }
    }

    // return the boolean flag
    return VerifiedData;
}


/****************************************************************************/
/*!
 *  \brief  Get the synchronous object
 *
 *  \iparam p_ReadWriteLock = Lock for the threads
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CExportConfigurationVerifier::GetSyncObject(QReadWriteLock* p_ReadWriteLock)
{
    p_ReadWriteLock = NULL;
    return true;
}


/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorHash_t& CExportConfigurationVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorsHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CExportConfigurationVerifier::ResetLastErrors()
{
    m_ErrorsHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
bool CExportConfigurationVerifier::IsLocalVerifier()
{
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Check the filenames in the list
 *
 *  \iparam Configuration = Configuration data
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CExportConfigurationVerifier::CheckFileNames(CConfigurationList Configuration)
{
    if (mp_ECD != NULL) {
        bool Result = true;

        QStringList PackageTye;
        PackageTye << "native" << "nonnative";
        // check the package type
        if (!PackageTye.contains(Configuration.GetPackageType())) {
            Result = false;
        }

        // check the files and the path of the files
        // check all the files path existence
        foreach (QString FileName, Configuration.GetFileList()) {
            // if the file name is null then dont check for the file path
            if (FileName.compare("")== 0) {
                Result = false;
            }
        }
        // check the group flag so that it can check the directory path existence
        if (Configuration.GetGroupListFlag()) {
            // if the directory name is null then dont check the path
            if(Configuration.GetGroupFileName().compare("") == 0) {
                Result = false;
            }
        }        

        return Result;
    }
    else {
        return false;
    }
}

}
