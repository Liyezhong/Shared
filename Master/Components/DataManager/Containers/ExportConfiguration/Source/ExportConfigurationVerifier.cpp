/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/ExportConfiguration/Source/ExportConfigurationVerifier.cpp
 *
 *  \brief Implementation file for class CExportConfigurationVerifier.
 *         This class verifies the CExportConfiguration container data
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-25
 *  $Author:    $ Raju, Ramya GJ
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
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/EventObject.h"
#include "Global/Include/Utils.h"

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
    // by default make the verification flag to true
    bool VerifiedData = true;
    try {
        CHECKPTR(p_Configuration);

        // assign pointer to member variable
        mp_ECD = static_cast<CExportConfiguration*>(p_Configuration);

        CHECKPTR(mp_ECD);

        // check the existence of the container
        if (mp_ECD != NULL) {
            // check the source directory existence
            if(mp_ECD->GetSourceDir().compare("") == 0) {
                qDebug() << "ExportConfig:Empty Source directory name.";
                /*lint -e534 */
                m_ErrorMap.insert(EVENT_EXPORTCONFIG_EMPTY_SOURCEDIR,
                                  Global::tTranslatableStringList() << "");
                Global::EventObject::Instance().RaiseEvent(EVENT_EXPORTCONFIG_EMPTY_SOURCEDIR,
                                                           Global::tTranslatableStringList() << "",
                                                           true,
                                                           Global::GUI_MSG_BOX);
                VerifiedData = false;
            }

            // check the destination directory existence
            if(mp_ECD->GetTargetDir().compare("") != 0) {
                if(mp_ECD->GetTargetFileName().compare("") == 0) {
                    qDebug() << "ExportConfig:Empty target file name";
                    /*lint -e534 */
                    m_ErrorMap.insert(EVENT_EXPORTCONFIG_EMPTY_TARGETFILE,
                                      Global::tTranslatableStringList() << "");
                    Global::EventObject::Instance().RaiseEvent(EVENT_EXPORTCONFIG_EMPTY_TARGETFILE,
                                                               Global::tTranslatableStringList() << "",
                                                               true,
                                                               Global::GUI_MSG_BOX);
                    VerifiedData = false;
                }
            }
            else {
                VerifiedData = false;
            }

            // check the service configuration flg so that verifier can verify only the service configuration
            if (mp_ECD->GetServiceConfigurationFlag()) {
                // check the service configuration
                if (!CheckFileNames(mp_ECD->GetServiceConfiguration().GetServiceConfigurationList(), "Service")) {
                    VerifiedData = false;
                }
            }

            // check the user configuration flg so that verifier can verify only the user configuration
            if (mp_ECD->GetUserConfigurationFlag()) {
                // check user configuration
                if (!CheckFileNames(mp_ECD->GetUserConfiguration().GetUserConfigurationList(), "User")) {
                    VerifiedData = false;
                }
                // check user report configuration
                if (!CheckFileNames(mp_ECD->GetUserConfiguration().GetUserReportList(), "User Report")) {
                    VerifiedData = false;
                }
            }
        }

        // return the boolean flag
        return VerifiedData;
    }
    CATCHALL();

    return false;
}


/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CExportConfigurationVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorMap;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CExportConfigurationVerifier::ResetErrors()
{
    m_ErrorMap.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 *
 *  \return true or false
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
 *  \iparam ConfigurationName = configuration name
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CExportConfigurationVerifier::CheckFileNames(CConfigurationList Configuration, QString ConfigurationName)
{
    bool Result = true;
    try {
        CHECKPTR(mp_ECD);

        QStringList PackageTye;
        PackageTye << "native" << "nonnative";
        // check the package type
        if (!PackageTye.contains(Configuration.GetPackageType())) {
            qDebug() << "ExportConfig:The package type of ### configuration is invalid, it should be native or nonnative."
                        <<ConfigurationName;
            /*lint -e534 */
            m_ErrorMap.insert(EVENT_EXPORTCONFIG_INVALID_PACKAGETYPE,
                              Global::tTranslatableStringList() << ConfigurationName);
            Global::EventObject::Instance().RaiseEvent(EVENT_EXPORTCONFIG_INVALID_PACKAGETYPE,
                                Global::tTranslatableStringList() << ConfigurationName,
                                                       true,
                                                       Global::GUI_MSG_BOX);
            Result = false;
        }

        // check the files and the path of the files
        // check all the files path existence
        foreach (QString FileName, Configuration.GetFileList()) {
            // if the file name is null then dont check for the file path
            if (FileName.compare("")== 0) {
                qDebug() << "ExportConfig:Empty file name found in file list of the ### configuration."
                            <<ConfigurationName;
                /*lint -e534 */
                m_ErrorMap.insert(EVENT_EXPORTCONFIG_EMPTY_FILE_LIST,
                                  Global::tTranslatableStringList() << ConfigurationName);
                Global::EventObject::Instance().RaiseEvent(EVENT_EXPORTCONFIG_EMPTY_FILE_LIST,
                                    Global::tTranslatableStringList() << ConfigurationName,
                                                           true,
                                                           Global::GUI_MSG_BOX);
                Result = false;
            }
        }
        // check the group flag so that it can check the directory path existence
        if (Configuration.GetGroupListFlag()) {
            // if the directory name is null then dont check the path
            if(Configuration.GetGroupFileName().compare("") == 0) {
                qDebug() << "ExportConfig:Empty file name found in group file list of ### configuration."
                            <<ConfigurationName;
                /*lint -e534 */
                m_ErrorMap.insert(EVENT_EXPORTCONFIG_EMPTY_GROUP_FILE,
                                  Global::tTranslatableStringList() << ConfigurationName);
                Global::EventObject::Instance().RaiseEvent(EVENT_EXPORTCONFIG_EMPTY_GROUP_FILE,
                                    Global::tTranslatableStringList() << ConfigurationName,
                                                           true,
                                                           Global::GUI_MSG_BOX);
                Result = false;
            }
        }

        return Result;
    }
    CATCHALL();

    return false;
}

}
