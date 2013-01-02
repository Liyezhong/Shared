/****************************************************************************/
/*! \file RackList.h
 *
 *  \brief CRackList definition.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2012-01-02, 2012-04-23
 *   $Author:  $ M.Scherer, Raju
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

#ifndef DATAMANAGER_DATARACKLIST_H
#define DATAMANAGER_DATARACKLIST_H


#include <QHash>
#include <QList>
#include <QReadWriteLock>
#include <QByteArray>
#include <QBuffer>
#include <QStringList>

#include "TestStubDataManager/Containers/Racks/Include/Rack.h"
#include "TestStubDataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "TestStubDataManager/Helper/Include/Types.h"
#include "Global/Include/GlobalDefines.h"

//lint -sem(DataManager::CRackList::AddRack, custodial(1))
//lint -sem(DataManager::CRackList::UpdateRack, custodial(2))


namespace DataManager {

typedef QHash<quint32, CRack*> ListOfRacks_t; ///< Hash table for the racks
typedef QList<quint32> ListOfRFIDs_t; ///< List to store RF IDs

/****************************************************************************/
/*!
 *  \brief  This class implements a list of rack data items.
 */
/****************************************************************************/
class CRackList : public CDataContainerBase
{
    friend class CSpecialVerifierGroupA;

private:
    bool m_DataVerificationMode; ///< To store the Data verification mode

    ListOfRacks_t m_RackList;   ///< The list of rack items
    ListOfRFIDs_t m_OrderedListOfRFIDUniqueIDs; ///< List of RFID numbers

    QReadWriteLock* mp_ReadWriteLock;   ///< Synchronisation for data access





    bool SerializeContent(QIODevice& p_Device, bool CompleteData);
    bool DeserializeContent(QIODevice& p_Device, bool CompleteData);

    void SetDefaultAttributes();
    
public:
    CRackList();
    CRackList(const CRackList&);

    ~CRackList();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CRackList& RackList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CRackList& RackList);
    CRackList& operator=(const CRackList&);

    void Init();

    bool RackExists(const quint32 RFIDUniqueID);

    bool GetRack(const quint32 RFIDUniqueID, CRack* p_Rack);
    bool GetRack(const int Index, CRack* p_Rack);
    /****************************************************************************/
    /*!
     *  \brief Gets the rack from the list
     *
     *  \iparam Index = Index of the rack
     *
     *  \return Rack class object
     */
    /****************************************************************************/
    CRack* GetRack(const int Index);   // uses order index
    /****************************************************************************/
    /*!
     *  \brief Gets the rack from the list
     *
     *  \iparam RFIDUniqueID = Rack RF ID
     *
     *  \return Rack class object
     */
    /****************************************************************************/
    CRack* GetRack(const quint32 RFIDUniqueID) { return m_RackList.value(RFIDUniqueID, NULL); }   // uses unique rack ID

    bool AddRack(const CRack* p_Rack);    // p_Rack is added directly => don't delete outside!

    bool UpdateRack(const CRack* p_Rack); // content of p_Rack will be copied  => delete outside!

    bool DeleteRack(const quint32 RackID);  // uses unique rack ID
    bool DeleteRack(const int Index);   // uses order index

    bool DeleteAllRacks();


    //    void ResetLastErrors();
    //    QStringList GetLastErrors();

    /****************************************************************************/
    /*!
     *  \brief Get the container type
     *
     *  \return True or False
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() { return RACKS; }    

    /****************************************************************************/
    /*!
     *  \brief Get the Data verification mode flag
     *
     *  \return True or False
     */
    /****************************************************************************/
    bool GetDataVerificationMode() { return m_DataVerificationMode; }

    /****************************************************************************/
    /*!
     *  \brief Set the Data verification mode flag value
     *
     *  \iparam Value = flag value
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool Value) { m_DataVerificationMode = Value; }    

    /****************************************************************************/
    /*!
     *  \brief Get the number of racks from the list
     *
     *  \return integer
     */
    /****************************************************************************/
    int GetNumberOfRacks() { return m_RackList.count(); }

    /****************************************************************************/
    /*!
     *  \brief Create the rack with RF ID
     *
     *  \iparam RFIDUniqueID = Unique RF ID of the rack
     *
     *  \iparam RFIDUserData = User data RF ID of the rack
     *
     *      Note: the CreateRack functions won't add the created rack to the
     *      internal rack list! This is the job of AddRack.
     *
     *  \return Created rack object
     */
    /****************************************************************************/
    CRack* CreateRack(const quint32 RFIDUniqueID, const quint32 RFIDUserData) { return new CRack(RFIDUniqueID, RFIDUserData); }  // given ID will be used
    /****************************************************************************/
    /*!
     *  \brief Get's the version of The Rack list File
     *
     *  \return Version Number
     */
    /****************************************************************************/
    int GetVersion() {return 0;}

    /****************************************************************************/
    /*!
     *  \brief Get's the fileName of the rack list
     *
     *  \return File name String
     */
    /****************************************************************************/
    QString GetFilename() {return "";}

};

} // namespace DataManager

#endif // DATAMANAGER_DATARACKLIST_H
