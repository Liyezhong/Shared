
/****************************************************************************/
/*! \file DataReagentList.h
 *
 *  \brief Definition file for class CDataReagentList.
 *
 *   $Version: $ 0.3
 *   $Date:    $ 2012-04-20,2013-01-17
 *   $Author:  $ Vikram MK,Swati Tiwari
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DATAMANAGER_DATAREAGENTLIST_H
#define DATAMANAGER_DATAREAGENTLIST_H

#include "DataManager/Helper/Include/Types.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/Reagent.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"
#include "Global/Include/EventObject.h"

//lint -sem(DataManager::CDataReagentList::SetDefaultAttributes,initializer)
//lint -sem(DataManager::CDataReagentList::AddReagent, custodial(1))
//lint -sem(DataManager::CDataReagentList::UpdateReagent, custodial(2))


namespace DataManager {

typedef QHash<QString, CReagent*> ListOfReagents_t;     //!< List of Reagents

/****************************************************************************/
/*!
 *  \brief CDataReagentList class Inherited from CDataContainerBase
 */
/****************************************************************************/
class CDataReagentList : public CDataContainerBase
{
    friend class CSpecialVerifierGroupA;

private:
    int m_Version;                              //!< version of the file read
    bool m_DataVerificationMode;                //!< Verification mode flag , verify the Container
    QString m_NextReagentID;                    //!< next Free Reagent ID of USER
    QString m_Filename;                         //!< filename of the file

    ListOfReagents_t m_ReagentList;             //!< Reagent list

    ListOfIDs_t m_OrderedListOfReagentIDs;      //!< Ordered list of Reagent ID

    QReadWriteLock* mp_ReadWriteLock;           //!< Reade Write lock
    QStringList m_ReagentListNames;             //!< List of Reagent Names

    ErrorMap_t m_ErrorHash;    //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

    bool SerializeContent(QIODevice& IODevice, bool CompleteData);
    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);

    bool WriteReagents(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool ReadReagents(QXmlStreamReader& XmlStreamReader, bool CompleteData);

    void SetDefaultAttributes();
    bool DeleteAllReagents();    
    bool CheckForUniquePropeties(const CReagent* p_Reagent, bool excludeSeft = false);

    /******************** INLINE FuNCTIONS **************************************/
    /****************************************************************************/
    /*!
     *  \brief Sets the Next Free Reagent id to the preferred value
     *
     *  \iparam Value = Reagent ID
     *
     *  \return
     */
    /****************************************************************************/
    void SetNextFreeReagentID(const QString Value){m_NextReagentID = Value;}

    /****************************************************************************/
    /*!
     *  \brief Sets the version of The Reagent list File
     *
     *  \iparam Value = Version number
     *
     *  \return
     */
    /****************************************************************************/
    void SetVersion(const int Value){m_Version = Value;}

public:
    CDataReagentList();
    ~CDataReagentList();

    CDataReagentList(const CDataReagentList&);
    friend QDataStream& operator <<(QDataStream& OutDataStream, const CDataReagentList& ReagentList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CDataReagentList& ReagentList);
    CDataReagentList& operator=(const CDataReagentList&);

    void Init();

    bool Read(QString Filename);
    QString GetNextFreeReagentID(bool AutoCount); // appends the string U and the m_NextReagentID.

    ReagentType_t GetReagentType(const QString ReagentID);
    ReagentType_t GetReagentType(const unsigned int Index);

    bool ReagentExists(const QString ReagentID);

    bool GetReagent(const QString ReagentID, CReagent& Reagent);
    bool GetReagent(const unsigned int Index, CReagent& Reagent);
    CReagent* GetReagent(const unsigned int Index); // uses order index

    bool AddReagent(const CReagent* p_Reagent);   // p_Reagent is added directly => don't delete outside!
    bool UpdateReagent(const CReagent* p_Reagent);  // content of p_Reagent will be copied  => delete outside!

    bool DeleteReagent(const QString ReagentID);   // uses unique reagent ID
    bool DeleteReagent(const unsigned int Index);  // uses order index

    HashOfHeatingStartMode_t HashOfHeatingStartModes;   //!< heating start modes AtDeviceStart and AtProgramStart

    /******************** INLINE FuNCTIONS **************************************/
    /****************************************************************************/
    /*!
     *  \brief Get's the Data Container Type for reagent
     *
     *  \return Datacontainer Type
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() {return REAGENTS;}

    /****************************************************************************/
    /*!
     *  \brief Get's the Count of Reagents available in the list
     *
     *  \return Reagents Count
     */
    /****************************************************************************/
    int GetNumberOfReagents() {return m_ReagentList.count();}
    /****************************************************************************/
    /*!
     *  \brief Get's the Verification Mode of the Reagent list File
     *
     *  \return true or false
     */
    /****************************************************************************/
    bool GetDataVerificationMode() {return m_DataVerificationMode;}

    /****************************************************************************/
    /*!
     *  \brief Sets the Verification Mode of the Reagent list File
     *
     *  \iparam Value = bool type , either true or false
     *
     *  \return
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool Value){m_DataVerificationMode = Value;}

    /****************************************************************************/
    /*!
     *  \brief Get's the version of The Reagent list File
     *
     *  \return Version Number
     */
    /****************************************************************************/
    int GetVersion() {return m_Version;}

    /****************************************************************************/
    /*!
     *  \brief Get's the fileName of the reagent list
     *
     *  \return File name String
     */
    /****************************************************************************/
    QString GetFilename() {return m_Filename;}

    // the CreateReagent functions won't add the created reagent to the internal
    // reagent list! This is the job of AddReagent.
    /****************************************************************************/
    /*!
     *  \brief Creates a Reagent
     *
     *  \return CReagent Class Instance
     */
    /****************************************************************************/
    // correct new ID will be generated
    CReagent* CreateReagent() {return new CReagent(GetNextFreeReagentID(true));}

    /****************************************************************************/
    /*!
     *  \brief Creates a Reagent with the Reagent ID passed
     *
     *  \iparam ReagentID = Reagent ID String
     *
     *  \return CReagent Class Instance
     */
    /****************************************************************************/
    // given ID will be used
    CReagent* CreateReagent(const QString ReagentID) {return new CReagent(ReagentID);}

    /****************************************************************************/
    /*!
     *  \brief Gets the CReagent Instance associated with the particular Reagent ID
     *
     *  \iparam ReagentID = Reagent ID String
     *
     *  \return CReagent Class Instance
     */
    /****************************************************************************/
    // uses unique reagent ID
    CReagent* GetReagent(const QString ReagentID) const {return m_ReagentList.value(ReagentID, NULL);}

        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of function UpdateOnLanguageChanged
         *
         *  \return from UpdateOnLanguageChanged
         */
        /****************************************************************************/
        void UpdateOnLanguageChanged();
};

} // namespace DataManager

#endif // DATAMANAGER_DATAREAGENTLIST_H
