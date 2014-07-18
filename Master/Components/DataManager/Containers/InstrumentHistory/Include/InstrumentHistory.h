/****************************************************************************/
/*! \file InstrumentHistory.h
 *
 *  \brief Definition file for class CInstrumentHistory.
 *  Reads all ModuleList information from XML, stores in a container (QHash)
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-05-24
 *  $Author:    $ Soumya. D , Shuvasmita
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
#ifndef DATAMANAGER_INSTRUMENTHISTORY_H
#define DATAMANAGER_INSTRUMENTHISTORY_H

#include <QString>
#include <QHash>
#include <QDateTime>
#include <QIODevice>
#include <QList>
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
//#include "ServiceDataManager/Include/ModuleDataListArchive.h"
#include "DataManager/Helper/Include/Types.h"
#include "Global/Include/Translator.h"
#include "DataManager/Containers/InstrumentHistory/Include/ModuleDataList.h"

//lint -e429
//lint -sem(DataManager::CInstrumentHistoryArchive::DeleteAllModuleList,cleanup)

namespace DataManager
{

typedef QHash<QString, CModuleDataList*> ModuleDataList_t;  //!< Hash table for module lists

typedef QList<QString> ModuleList_t;                                  //!< List for module lists

/***********************************************************************************/
/*!
 *  \brief  This class implements Module List
 *  Reading all ModuleList information from XML and Function Storing in a Container
 */
/**********************************************************************************/
class CInstrumentHistory : public CDataContainerBase
{
    friend class CTestServiceDataManager;
    friend class TestDataModuleList;
    ModuleDataList_t m_ModuleList;    //!< Module Lists
    ModuleList_t m_ListofModules;        //!< List of Module Names
    QString m_FileName;                         //!< XML file name
    QString m_InstrumentName;                   //!< Stores System name
    bool m_DataVerificationMode;                //!< Verification mode flag , verify the Container
    DataManager::CModuleDataList *mp_ModuleList;//!< Module list object
    QReadWriteLock *mp_ReadWriteLock; //!< Synchronization for data access
    ErrorMap_t m_ErrorMap;      //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);
    bool SerializeContent(QIODevice& IODevice, bool CompleteData);


    /****************************************************************************/
    /*!
     *  \brief  To read moduleList info from xml
     *  \iparam XmlStreamReader = Xml reader to read the XML contents
     *  \iparam CompleteData = bool type if true writes Complete data of object
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool ReadModuleLists(QXmlStreamReader& XmlStreamReader, bool CompleteData);

public:
    CInstrumentHistory();
    CInstrumentHistory(const CInstrumentHistory&);            //!< Copy Constructor
    ~CInstrumentHistory();
    CInstrumentHistory& operator=(const CInstrumentHistory&); //!< Assignment Operator Overloading
    friend QDataStream& operator <<(const QDataStream& OutDataStream, const CInstrumentHistory& ModuleList);
    friend QDataStream& operator >>(const QDataStream& InDataStream, const CInstrumentHistory& ModuleList);

    /****************************************************************************/
    /*!
     *  \brief returns the  Data container type
     *  \return Data container type
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() {return INSTRUMENTHISTORY;}

    /****************************************************************************/
    /*!
     *  \brief Returns the filename of the xml file read
     *  \return File name
     */
    /****************************************************************************/
     QString GetFilename() {return m_FileName;}

    /****************************************************************************/
    /*!
     *  \brief Sets the filename of the xml file read
     *  \iparam FileName = File name
     */
    /****************************************************************************/
    void SetFilename(QString FileName) { m_FileName = FileName;}

    /****************************************************************************/
    /*!
     *  \brief Sets verification mode
     *  \iparam value = true-verification on, false - verification off
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool value) { m_DataVerificationMode = value; }

    /****************************************************************************/
    /*!
     *  \brief Gets verification mode
     *  \return true-verification on, false - verification off
     */
    /****************************************************************************/
    bool GetDataVerificationMode() { return m_DataVerificationMode; }

    /****************************************************************************/
    /*!
     *  \brief  To set system name
     *  \iparam value = name to set
     */
    /****************************************************************************/
    void SetInstrumentName(const QString value) { m_InstrumentName = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the system name
     *  \return System name
     */
    /****************************************************************************/
    QString GetInstrumentName() { return m_InstrumentName; }

    /****************************************************************************/
    /*!
     *  \brief  Deletes all the ModuleList in the list
     *  \return true - delete success , false - delete failure
     */
    /****************************************************************************/
    bool DeleteAllModuleList();

    /****************************************************************************/
    /*!
     *  \brief  Deletes the ModuleList
     *  \iparam Index = Index of the modulelist
     *  \return true - delete success , false - delete failure
     */
    /****************************************************************************/
    bool DeleteModuleList(const unsigned int Index);

    /****************************************************************************/
    /*!
     *  \brief  Deletes the ModuleList
     *  \iparam TimeStamp - Module timestamp
     *  \return true - delete success , false - delete failure
     */
    /****************************************************************************/
    bool DeleteModuleList(const QString TimeStamp);

    /****************************************************************************/
    /*!
     *  \brief  To writes data to container
     *  \iparam p_ModuleList = ModuleList Object
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool AddModuleListInfo(CModuleDataList *p_ModuleList);

    /****************************************************************************/
    /*!
     *  \brief  To read XML file
     *  \iparam FileName = File name to read
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool ReadFile(const QString FileName);

    /****************************************************************************/
    /*!
     *  \brief  To write into XML file
     *  \iparam FileName = File name to write
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool WriteArchiveFile(const QString FileName);

    /****************************************************************************/
    /*!
     *  \brief  To write into XML file
     *  \return Number of modulelists present in the InstrumentHistory
     */
    /****************************************************************************/
    int GetNumberOfModuleLists() {
        return m_ListofModules.count();
    }

    /****************************************************************************/
    /*!
     *  \brief  To get ModuleList info using Index
     *  \iparam Index = Index of the Module list
     *  \return ModuleList info
     */
    /****************************************************************************/
    CModuleDataList* GetModuleList(const unsigned int Index); // uses order index

    /****************************************************************************/
    /*!
     *  \brief  To get module info using module name
     *  \iparam TimeStamp = Module list timestamp
     *  \return Module list information
     */
    /****************************************************************************/
    CModuleDataList* GetModuleList(const QString TimeStamp) const {
            return m_ModuleList.value(TimeStamp, NULL);
    }

    /****************************************************************************/
    /*!
     *  \brief  To get module info using module name
     *  \iparam TimeStamp = Module list timestamp
     *  \return Module list information
     */
    /****************************************************************************/
    CModuleDataList* GetModuleDataList(const QString TimeStamp);
    /****************************************************************************/
    /*!
     *  \brief  To update moduleList into to XML
     *  \iparam p_ModuleList = Object of ModuleList
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool UpdateModuleList(CModuleDataList const* p_ModuleList);

};

} // end of namespace DataManager
#endif // DATAMANAGER_INSTRUMENTHISTORY_H
