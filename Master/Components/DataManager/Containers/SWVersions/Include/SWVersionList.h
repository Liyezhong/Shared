/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/SWVersions/Include/SWVersionList.h
 *
 *  \brief Definition file for class CSWVersionList.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-07
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

#ifndef DATAMANAGER_SWVERSIONLIST_H
#define DATAMANAGER_SWVERSIONLIST_H

#include <QString>
#include <QXmlStreamReader>
#include <QWriteLocker>

#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Containers/SWVersions/Include/SWDetails.h"

//lint -sem(DataManager::CSWVersionList::AddSWDetails, custodial(1))

namespace DataManager {

typedef QHash<QString, CSWDetails*> ListOfSWVersions_t; ///< Hash table for the racks

/****************************************************************************/
/*!
 *  \brief CSWVersionList class containing the SWVersionList attributes
 */
/****************************************************************************/
class CSWVersionList: public CDataContainerBase
{
private:    
    int m_Version;                                  ///< Version no of SWVersionList.xml
    bool m_DataVerificationMode;                    ///< Verification status
    QString m_Filename;                             ///< XML File name
    QString m_SWReleaseVersion;                     ///< Store the SW release version
    QString m_SWReleaseDate;                        ///< Store the SW release date
    ListOfSWVersions_t m_SWDetailsList;             ///< Store the SW details in the hash table
    ListOfIDs_t m_SWNameList;                       ///< Store SW names in the list

    ErrorMap_t m_ErrorMap;    //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

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

    /****************************************************************************/
    /*!
     *  \brief Creates a SWDetails with the SW Name passed
     *
     *  \iparam SWName = SW Name String
     *
     *  \return CSWDetails Class Instance
     */
    /****************************************************************************/
    // given ID will be used
    CSWDetails* CreateSWDetails(const QString SWName) {return new CSWDetails(SWName);}    

    const CSWDetails* GetSWDetails(const unsigned int Index); // uses order index

    void Init();

    bool WriteSWDetails(QXmlStreamWriter& XmlStreamWriter, SWType_t SWType, bool CompleteData);
    bool ReadSWDetails(QXmlStreamReader& XmlStreamReader, QString NodeName, bool CompleteData);

    QStringList GetSWDetailIdList() const;

public:
    CSWVersionList();
    CSWVersionList(const CSWVersionList&);
    void CopyFromOther(const CSWVersionList &Other);
    void AddSWDetailsWithoutVerification(const CSWDetails *p_SWDetails);
    virtual ~CSWVersionList() {}

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CSWVersionList& SWVersionList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CSWVersionList& SWVersionList);
    CSWVersionList& operator=(const CSWVersionList&);

    bool Read(QString Filename);
    bool GetSWDetails(const unsigned int Index, CSWDetails& SWDetails);
    bool GetSWDetails(const QString SWName, CSWDetails& SWDetails);

    bool AddSWDetails(const CSWDetails* p_SWDetails);   // p_SWDetails is added directly => don't delete outside!
    bool UpdateSWDetails(const CSWDetails* p_SWDetails);  // content of p_SWDetails will be copied  => delete outside!

    bool DeleteSWDetails(const QString SWName);   // uses unique SW name
    bool DeleteSWDetails(const unsigned int Index);  // uses order index

    bool DeleteAllSWDetails();

    /****************************************************************************/
    /*!
     *  \brief Get the data container type
     *
     *  \return Type of the data container
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() {return SWVERSION;}

    /****************************************************************************/
    /*!
     *  \brief Get's the Count of SW details from the list
     *
     *  \return SW details Count
     */
    /****************************************************************************/
    int GetNumberOfSWDetails() {return m_SWDetailsList.count();}

    /****************************************************************************/
    /*!
     *  \brief Get's the Verification Mode of the SW version list File
     *
     *  \return true or false
     */
    /****************************************************************************/
    bool GetDataVerificationMode() {return m_DataVerificationMode;}

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
     *  \brief Retrieve the version of the xml file
     *
     *  \return version
     */
    /****************************************************************************/
    int GetVersion() {return m_Version;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the SW release version from the xml file
     *
     *  \return sw release version
     */
    /****************************************************************************/
    QString GetSWReleaseVersion() {return m_SWReleaseVersion;}

    /****************************************************************************/
    /*!
     *  \brief Sets the SW release version
     *
     *  \iparam Value = release version
     */
    /****************************************************************************/
    void SetSWReleaseVersion (const QString Value) {m_SWReleaseVersion = Value;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the SW release date from the xml file
     *
     *  \return sw release date
     */
    /****************************************************************************/
    QString GetSWReleaseDate() {return m_SWReleaseDate;}

    /****************************************************************************/
    /*!
     *  \brief Sets the SW release date
     *
     *  \iparam Value = release date
     */
    /****************************************************************************/
    void SetSWReleaseDate (const QString Value) {m_SWReleaseDate = Value;}

    /****************************************************************************/
    /*!
     *  \brief Gets the CSWDetails Instance associated with the particular SW name
     *
     *  \iparam SWName = SW name String
     *
     *  \return CSWDetails Class Instance
     */
    /****************************************************************************/    
    CSWDetails* GetSWDetails(const QString SWName) const {return m_SWDetailsList.value(SWName, NULL);}


};

} // namespace DataManager

#endif // DATAMANAGER_SWVERSIONLIST_H
