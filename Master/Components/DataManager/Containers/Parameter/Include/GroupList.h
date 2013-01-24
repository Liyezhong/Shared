/****************************************************************************/
/*! \file GroupList.h
 *
 *  \brief Definition file for class CGroupList.
 *  This class is implemented to read GroupList information from the XML.
 *  Reads all Group information, stores in a container (QHash) and also writes
 *  data from container to XML.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-01
 *  $Author:    $ Thirumalesha R
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

#ifndef DATAMANAGER_GROUPLIST_H
#define DATAMANAGER_GROUPLIST_H

#include <QHash>
#include <QDateTime>
#include <QIODevice>
#include <QList>
#include <QReadWriteLock>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QString>

#include "Global/Include/Translator.h"
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "ColoradoDataManager/Containers/Parameter/Include/Group.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "ColoradoDataManager/Templates/Include/ColoradoDataManagerEventCodes.h"


namespace DataManager
{

typedef QHash<QString, CGroup*> ListofGroups_t; //!< Hash table for list of Groups

/****************************************************************************/
/*!
 *  \brief  This class implements Group List
 *  Reading all Group information from XML and FunctionStoring in a Container
 */
/****************************************************************************/
class CGroupList: public CDataContainerBase
{


public:
    CGroupList();
    CGroupList(const CGroupList&);
    ~CGroupList();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CGroupList&  GroupList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CGroupList& GroupList);
    CGroupList& operator = (const CGroupList&);

    void Init();

    /****************************************************************************/
    /*!
     *  \brief returns the  Data container type
     *  \return Data container type
     */
    /****************************************************************************/
     DataContainerType_t GetDataContainerType() {return PARAMETER;}

     /****************************************************************************/
     /*!
        *  \brief retrieves the status of Verification
        *  \return true-verification on , false-verification off
        */
     /****************************************************************************/
     bool GetDataVerificationMode() {return m_DataVerificationMode;}
     /****************************************************************************/
     /*!
        *  \brief sets verification mode
        *  \iparam Value = true-verification on, false - verification off
        */
     /****************************************************************************/
     void SetDataVerificationMode(const bool Value){m_DataVerificationMode = Value;}

     bool Read(QString Filename);

     /****************************************************************************/
     /*!
        *  \brief returns the version of the xml file read
        *  \return file version
        */
     /****************************************************************************/
     int  GetVersion() {return m_Version;}
	 
    /****************************************************************************/
    /*!
     *  \brief returns the filename of the xml file read
     *  \return file name
     */
    /****************************************************************************/
    QString GetFilename() {return m_Filename;}
	
     /****************************************************************************/
     /*!
     *  \brief  to set System Name
     *  \iparam Value = name to set
     */
     /****************************************************************************/
     int GetNumberofGroups() { return m_GroupList.count(); }
	 
    /****************************************************************************/
    /*!
     *  \brief Creates a new Group with Group Key sent
     *  \iparam GroupKey = Key of the Group
     *  \return Pointer to Group
     */
    /****************************************************************************/
    CGroup* CreateGroup(const QString GroupKey) {return new CGroup(GroupKey);} // given Key will be used

     /****************************************************************************/
    /*!
     *  \brief Retrieve a Group with the given Group Key
     *  \iparam GroupKey
     *  \return Pointer to Group
     */
    /****************************************************************************/
    CGroup const * GetGroup(const QString GroupKey) {return m_GroupList.value(GroupKey, NULL);}  // uses unique Group Key
    /****************************************************************************/
    /*!
     *  \brief Retrieve a Group with the given index
     *  \iparam Index
     *  \return Pointer to Group
     */
    /****************************************************************************/
    CGroup* GetGroup(const unsigned int Index); // uses order index
    bool GetGroup(const QString GroupKey, CGroup& Group);
    bool GetGroup(const unsigned int Index, CGroup& Group);

    bool AddGroup(const CGroup* p_Group);   // p_Group is added directly => don't delete outside!
    bool UpdateGroup(const CGroup* p_Group);  // content of p_Group will be copied  => delete outside!
    bool DeleteGroup(const QString GroupKey);   // uses unique Group Key
    bool DeleteGroup(const unsigned int Index);  // uses order index
    bool DeleteAllGroup();
private:
    int m_Version;  //!<  version of the file read
    bool m_DataVerificationMode;//!< Verification mode flag , verify the Container
    
    QString m_Filename;//!< filename of the XML file to read

    ListofGroups_t m_GroupList; //!< Group list

    ListOfKeys_t m_OrderedListOfGroupKeys; //!< Ordered list of Group Key's

    QReadWriteLock* mp_ReadWriteLock; //!< Lock for thread safety

    ErrorHash_t m_ErrorHash;    //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator
    
	bool SerializeContent(QIODevice& IODevice, bool CompleteData);
    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);

    bool ReadAllGroups(QXmlStreamReader& XmlStreamReader, bool CompleteData);

	/****************************************************************************/
    /*!
     *  \brief sets the xml version
     *  \iparam Value = Version to set
     */
    /****************************************************************************/
    void SetVersion(const int Value){m_Version = Value;}

    /****************************************************************************/
    /*!
     *  \brief Sets the XML file name
     *  \iparam Value = Filename to set
     */
    /****************************************************************************/
    void SetFilename(const QString Value) {m_Filename = Value;}
};


}   // namespace DataManager

#endif // DATAMANAGER_GROUPLIST_H
