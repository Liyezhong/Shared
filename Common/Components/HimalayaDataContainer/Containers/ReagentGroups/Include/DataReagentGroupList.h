/****************************************************************************/
/*! \file DataReagentGroupList.h
 *
 *  \brief ReagentGroup defination.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-30
 *   $Author:  $ Swati Tiwari
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
#ifndef DATAMANAGER_DATAREAGENTGROUPLIST_H
#define DATAMANAGER_DATAREAGENTGROUPLIST_H


#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/ReagentGroup.h"

//lint -sem(DataManager::CDataReagentGroupList::AddReagentGroup, custodial(1))
//lint -sem(DataManager::CDataReagentGroupList::UpdateReagentGroup, custodial(2))
//lint -e526

namespace DataManager {
//!< List of reagent groups
typedef QHash<QString, CReagentGroup*> ListOfReagentGroups_t; //!< List of reagent groups
//!< Verifier list

/****************************************************************************/
/*!
 *  \brief  This class contains list of program sequence
 */
/****************************************************************************/
class CDataReagentGroupList:public CDataContainerBase
{
public:
    CDataReagentGroupList();
    ~CDataReagentGroupList();

    void Init();

    /****************************************************************************/
    /*!
     *  \brief Get the data container type
     *
     *  \return Type of the data container
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() { return REAGENTGROUP; }

    bool Read(QString Filename);

    CDataReagentGroupList(const CDataReagentGroupList&);
    friend QDataStream& operator << (QDataStream& OutDataStream, const CDataReagentGroupList& ReagentGroupList);
    friend QDataStream& operator >> (QDataStream& InDataStream, CDataReagentGroupList& ReagentGroupList);
    CDataReagentGroupList& operator = (const CDataReagentGroupList&);

    /****************************************************************************/
    /*!
     *  \brief Retrieve verification status
     *  \return true - verfication on. false - verification off
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
     *  \brief Retrieve the version of the xml file
     *  \return version
     */
    /****************************************************************************/
    int  GetVersion() {return m_Version;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve filename of the XML
     *  \return file name
     */
    /****************************************************************************/
    QString GetFilename () {return m_Filename;}

    //SECTION PROGRAMS//
    /****************************************************************************/
    /*!
     *  \brief Retrieve the number of programs in the ReagentGroup list
     *  \return Program count
     */
    /****************************************************************************/
    int  GetNumberOfReagentGroups() {return m_ReagentGroupsList.count();}

    // the CreateStep functions won't add the created step to the internal step list!
    /****************************************************************************/
    /*!
     *  \brief Create a program sequence step
     *  \return Program sequence step
     */
    /****************************************************************************/
    CReagentGroup* CreateReagentGroup() {return new CReagentGroup();}
    /****************************************************************************/
    /*!
     *  \brief Create program sequence step for the given program id  
     *
     *	\iparam ProgramID = Program ID
     *
     *  \return program sequence step
     */
    /****************************************************************************/
    CReagentGroup* CreateReagentGroup(const QString ProgramID) {return (new CReagentGroup(ProgramID));} // given ID will be used

    /****************************************************************************/
    /*!
     *  \brief Create program sequence step for the given program id
     *
     *	\iparam ID = Program ID
     *
     *  \return program sequence
     */
    /****************************************************************************/
    CReagentGroup* GetReagentGroup(const QString ID) {return m_ReagentGroupsList.value(ID, NULL);}  // uses unique step ID

    bool GetReagentGroup(const unsigned int Index, CReagentGroup& ReagentGroup);
    bool GetReagentGroup(const QString ID, CReagentGroup& ReagentGroup);

    CReagentGroup* GetReagentGroup(const unsigned int Index); // uses order index

    bool AddReagentGroup(const CReagentGroup* p_ReagentGroupStep);   // p_ProgramStep is added directly => don't delete outside!
    bool UpdateReagentGroup(const CReagentGroup* p_ReagentGroupStep);  // content of p_ProgramStep will be copied  => delete outside!

    bool DeleteReagentGroup(const QString StepID);   // uses unique step ID
    bool DeleteReagentGroup(const unsigned int Index);  // uses order index

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetReagentGroupIndex
     *
     *  \param GroupID = const QString type parameter
     *
     *  \return from GetReagentGroupIndex
     */
    /****************************************************************************/
    qint32 GetReagentGroupIndex(const QString& GroupID) { return m_OrderedListOfReagentGroupIDs.indexOf(GroupID); }   // uses unique group ID

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateOnLanguageChanged
     *
     *  \return from UpdateOnLanguageChanged
     */
    /****************************************************************************/
    void UpdateOnLanguageChanged();

private:
    int m_Version;                  //!< version of the xml
    bool m_DataVerificationMode;    //!< Verification status
    QString m_Filename;             //!< XML File name


    ListOfReagentGroups_t m_ReagentGroupsList;   //!< A list containg the reagent groups
    ListOfIDs_t m_OrderedListOfReagentGroupIDs;  //!< A List in which reagent group are stored in order

    QReadWriteLock* mp_ReadWriteLock;//!< File read write protection

    ErrorMap_t m_ErrorHash;    //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

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

    bool SerializeContent(QIODevice& IODevice, bool CompleteData); // Writes from the CDataReagentGroupList object to a IODevice.
    bool DeserializeContent(QIODevice& IODevice, bool CompleteData); // Reads from the IODevice to CDataReagentGroupList object.
    bool ReadCompleteData(QXmlStreamReader& XmlStreamReader);

    void SetDefaultAttributes();
    bool DeleteAllReagentGroups();

};

}//End of namespace DataManager
#endif // DATAMANAGER_DATAREAGENTGROUPLIST_H
