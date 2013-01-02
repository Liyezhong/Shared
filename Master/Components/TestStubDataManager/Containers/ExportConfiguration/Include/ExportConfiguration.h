/****************************************************************************/
/*! \file ExportConfiguration.h
 *
 *  \brief Definition file for class CExportConfiguration.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-06
 *  $Author:    $ Aparna AL
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

#ifndef DATAMANAGER_EXPORTCONFIGURATION_H
#define DATAMANAGER_EXPORTCONFIGURATION_H

#include <QString>
#include <QXmlStreamReader>
#include <QWriteLocker>

#include "TestStubDataManager/Helper/Include/Types.h"
#include "TestStubDataManager/Helper/Include/Helper.h"
#include "TestStubDataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "TestStubDataManager/Containers/ExportConfiguration/Include/ImportExportTypes.h"
#include "TestStubDataManager/Containers/ExportConfiguration/Include/ServiceConfiguration.h"
#include "TestStubDataManager/Containers/ExportConfiguration/Include/UserConfiguration.h"

namespace DataManager {


/****************************************************************************/
/*!
 *  \brief CExportConfiguration class containing the ExportConfiguration attributes
 */
/****************************************************************************/
class CExportConfiguration: public CDataContainerBase
{
private:
    QString m_SourceDir;                            ///< Source directory for exporting
    QString m_TargetDir;                            ///< Target directory for exporting
    QString m_TargetFileName;                       ///< Target file name
    int m_Version;                                  ///< Version no of ExportConfiguration.xml
    bool m_UserConfigurationFlag;                   ///< Store the user configuration flag only to read user configuration from XML
    bool m_ServiceConfigurationFlag;                ///< Store the service configuration flag only to read service configuration from XML
    bool m_DataVerificationMode;                    ///< Verification status
    QString m_Filename;                             ///< XML File name

    CUserConfiguration m_UserConfiguration;         ///< store the user configuration
    CServiceConfiguration m_ServiceConfiguration;   ///< store the service configuration

    bool SerializeContent(QIODevice& p_Device, bool CompleteData);
    bool DeserializeContent(QIODevice& p_Device, bool CompleteData);

    /****************************************************************************/
    /*!
     *  \brief Sets the version of the xml file
     *
     *  \iparam Value = version of the file
     */
    /****************************************************************************/
    void SetVersion(const int Value){m_Version = Value;}

    /****************************************************************************/
    /*!
     *  \brief Sets the file name of the xml
     *
     *  \iparam Value = file name
     */
    /****************************************************************************/
    void SetFilename (const QString Value) {m_Filename = Value;}

protected:

public:
    CExportConfiguration();
    CExportConfiguration(const ImportExportConfig_t ConfigType); //UserType
    CExportConfiguration(const CExportConfiguration&);
    virtual ~CExportConfiguration() {}

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CExportConfiguration& ExportConfiguration);
    friend QDataStream& operator >>(QDataStream& InDataStream, CExportConfiguration& ExportConfiguration);
    CExportConfiguration& operator=(const CExportConfiguration&);

    bool Read(QString Filename);

    /****************************************************************************/
    /*!
     *  \brief Get the data container type
     *
     *  \return Type of the data container
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() {return EXPORT;}    

    /****************************************************************************/
    /*!
     *  \brief Sets the data verification status
     *
     *  \iparam Value = true -set ,false - not set
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool Value){m_DataVerificationMode = Value;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve filename of the XML
     *
     *  \return file name
     */
    /****************************************************************************/
    QString GetFilename() { return m_Filename;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the source directory
     *
     *  \return directory name
     */
    /****************************************************************************/
    QString GetSourceDir() {return m_SourceDir;}

    /****************************************************************************/
    /*!
     *  \brief Sets the source directory
     *
     *  \iparam SourceDir = source directory name
     */
    /****************************************************************************/
    void SetSourceDir(QString SourceDir) {m_SourceDir = SourceDir;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the source directory
     *
     *  \return directory name
     */
    /****************************************************************************/
    QString GetTargetDir() {return m_TargetDir;}

    /****************************************************************************/
    /*!
     *  \brief Sets the target directory
     *
     *  \iparam TargetDir = target directory name
     */
    /****************************************************************************/
    void SetTargetDir(QString TargetDir) {m_TargetDir = TargetDir;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the target file name
     *
     *  \return file name
     */
    /****************************************************************************/
    QString GetTargetFileName() {return m_TargetFileName;}

    /****************************************************************************/
    /*!
     *  \brief Sets the target file name
     *
     *  \iparam FileName = target file name
     */
    /****************************************************************************/
    void SetTargetFileName(QString FileName) {m_TargetFileName = FileName;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the version of the xml file
     *
     *  \return version
     */
    /****************************************************************************/
    int GetVersion() {return m_Version;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the service configuration
     *
     *  \return ServiceConfiguration class
     */
    /****************************************************************************/
    CServiceConfiguration GetServiceConfiguration() {return m_ServiceConfiguration;}

    /****************************************************************************/
    /*!
     *  \brief Sets the service configuration
     *
     *  \iparam Service = configurtion class for the service
     */
    /****************************************************************************/
    void SetServiceConfiguration(CServiceConfiguration Service) {m_ServiceConfiguration = Service;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the user configuration
     *
     *  \return UserConfiguration class
     */
    /****************************************************************************/
    CUserConfiguration GetUserConfiguration() {return m_UserConfiguration;}

    /****************************************************************************/
    /*!
     *  \brief Sets the user configuration
     *
     *  \iparam User = configuration class for the user
     */
    /****************************************************************************/
    void SetUserConfiguration(CUserConfiguration User) {m_UserConfiguration = User;}

    /****************************************************************************/
    /*!
     *  \brief Sets the user configuration flag
     *
     *  \iparam UserFlag = true -set ,false - not set
     */
    /****************************************************************************/
    void SetUserConfigurationFlag(bool UserFlag) {m_UserConfigurationFlag = UserFlag;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve user configuration flag
     *
     *  \return true - read user configuration. false - don't read user configuration
     */
    /****************************************************************************/
    bool GetUserConfigurationFlag() {return m_UserConfigurationFlag;}

    /****************************************************************************/
    /*!
     *  \brief Sets the service configuration flag
     *
     *  \iparam ServiceFlag = true -set ,false - not set
     */
    /****************************************************************************/
    void SetServiceConfigurationFlag(bool ServiceFlag) {m_ServiceConfigurationFlag = ServiceFlag;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the service configuration flag
     *
     *  \return true - read service configuration. false - don't read service configuration
     */
    /****************************************************************************/
    bool GetServiceConfigurationFlag() {return m_ServiceConfigurationFlag;}


};

} // namespace DataManager

#endif // DATAMANAGER_EXPORTCONFIGURATION_H
