/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/ExportConfiguration/Include/ConfigurationList.h
 *
 *  \brief Implementation file for class CConfigurationList.
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
#ifndef CONFIGURATIONLIST_H
#define CONFIGURATIONLIST_H

#include <QStringList>
#include <QXmlStreamWriter>
#include "DataManager/Helper/Include/Helper.h"


namespace DataManager {

// forward declaration for the classes
class CServiceConfiguration;
class CUserConfiguration;

/****************************************************************************/
/*!
 *  \brief  This class implements configuration which consists of number
 *  of files and the attributes to the number of files
 */
/****************************************************************************/
class CConfigurationList {

    friend class CServiceConfiguration;
    friend class CUserConfiguration;

private:
    QStringList m_FileList; ///< Store the list of files
    bool m_GroupFlag;  ///< Store the Group flag value
    QString m_GroupFileName; ///< Store the group file name
    bool m_Encryption; ///< Store the encryption flag value
    bool m_Compressed; ///< Store the compressed flag
    bool m_Certified; ///< Store the certified flag
    QString m_PackageType; ///< Store the package type
    QString m_TagName;     ///< Store the tag name for the xml

    bool ReadFileList(QXmlStreamReader& XmlStreamReader);
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);


public:
    CConfigurationList();

    CConfigurationList(const CConfigurationList&);
    void CopyFromOther(const CConfigurationList &ConfigList);

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CConfigurationList& ExportConfiguration);
    friend QDataStream& operator >>(QDataStream& InDataStream, CConfigurationList& ExportConfiguration);
    CConfigurationList& operator=(const CConfigurationList&);

    /****************************************************************************/
    /*!
     *  \brief Retrieve the file list
     *  \return list of files
     */
    /****************************************************************************/
    QStringList GetFileList() {return m_FileList;}

    /****************************************************************************/
    /*!
     *  \brief Sets the list of files
     *
     *  \iparam FileList - List of files
     */
    /****************************************************************************/
    void SetFileList(const QStringList FileList) {m_FileList = FileList;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the group flag value
     *  \return flag value
     */
    /****************************************************************************/
    bool GetGroupListFlag() {return m_GroupFlag;}

    /****************************************************************************/
    /*!
     *  \brief Sets the group flag value
     *
     *  \iparam GroupFlag = true -set ,false - not set
     */
    /****************************************************************************/
    void SetGroupListFlag(const bool GroupFlag) {m_GroupFlag = GroupFlag;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the group file name
     *  \return file name
     */
    /****************************************************************************/
    QString GetGroupFileName() {return m_GroupFileName;}

    /****************************************************************************/
    /*!
     *  \brief Sets the group file name
     *
     *  \iparam FileName = file name of the Group
     */
    /****************************************************************************/
    void SetGroupFileName(const QString FileName) {m_GroupFileName = FileName;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the encryption flag value
     *  \return flag value
     */
    /****************************************************************************/
    bool GetEncryptionFlag() {return m_Encryption;}

    /****************************************************************************/
    /*!
     *  \brief Sets the encryption flag
     *
     *  \iparam Encryption = true -set ,false - not set
     */
    /****************************************************************************/
    void SetEncryptionFlag(const bool Encryption) {m_Encryption = Encryption;}


    /****************************************************************************/
    /*!
     *  \brief Retrieve the compressed flag value
     *  \return flag value
     */
    /****************************************************************************/
    bool GetCompressedFlag() {return m_Compressed;}

    /****************************************************************************/
    /*!
     *  \brief Sets the compressed flag
     *
     *  \iparam Compressed = true -set ,false - not set
     */
    /****************************************************************************/
    void SetCompressedFlag(const bool Compressed) {m_Compressed = Compressed;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the certification flag value
     *  \return flag value
     */
    /****************************************************************************/
    bool GetCertifiedFlag() {return m_Certified;}

    /****************************************************************************/
    /*!
     *  \brief Sets the certification flag
     *
     *  \iparam Certified = true -set ,false - not set
     */
    /****************************************************************************/
    void SetCertifiedFlag(const bool Certified) {m_Certified = Certified;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the package type of the file
     *  \return package type
     */
    /****************************************************************************/
    QString GetPackageType() {return m_PackageType;}

    /****************************************************************************/
    /*!
     *  \brief Sets the package type
     *
     *  \iparam PackageType = type of the package
     */
    /****************************************************************************/
    void SetPackageType(const QString PackageType) {m_PackageType = PackageType;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the tag name of the xml
     *  \return package type
     */
    /****************************************************************************/
    QString GetTagName() {return m_TagName;}

    /****************************************************************************/
    /*!
     *  \brief Sets the tag name
     *
     *  \iparam TagName = type of the package
     */
    /****************************************************************************/
    void SetTagName(const QString TagName) {m_TagName = TagName;}

};
}


#endif // CONFIGURATIONLIST_H
