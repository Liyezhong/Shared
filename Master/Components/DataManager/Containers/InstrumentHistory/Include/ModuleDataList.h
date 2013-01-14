/****************************************************************************/
/*! \file ModuleDataList.h
 *
 *  \brief Definition file for class CModuleDataList.
 *  Reads all Module information from XML, stores in a container (QHash) and also writes
 *  data from container to XML.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-20
 *  $Author:    $ Soumya. D
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

#ifndef DATAMANAGER_MODULEDATALIST_H
#define DATAMANAGER_MODULEDATALIST_H

#include <QString>
#include <QHash>
#include <QReadWriteLock>
#include <QDateTime>
#include <QIODevice>
#include <QList>

#include "DataManager/Containers/InstrumentHistory/Include/Module.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Helper/Include/Types.h"

namespace DataManager
{

typedef QHash<QString, CModule*> ListofModules_t; //!< Hash table for list of modules

typedef QList<QString> ListofOrderedModules_t; //!< List for module names

/****************************************************************************/
/*!
 *  \brief  This class implements Module List
 *  Reading all Module information from XML and FunctionStoring in a Container
 */
/****************************************************************************/
class CModuleDataList : public CDataContainerBase
{

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CModuleDataList&  ModuleList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CModuleDataList& ModuleList);

public:
    CModuleDataList();
    CModuleDataList(const CModuleDataList&);
    ~CModuleDataList();
    CModuleDataList& operator=(const CModuleDataList&);

    /****************************************************************************/
    /*!
     *  \brief  To set Instrument Name
     *  \iparam Value = Name to set
     */
    /****************************************************************************/
    void SetInstrumentName(const QString value) { m_InstrumentName = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the name of the Instrument
     *  \return Instrument name
     */
    /****************************************************************************/
    QString GetInstrumentName() { return m_InstrumentName; }

    /****************************************************************************/
    /*!
     *  \brief  To set module timestamp
     *  \iparam Value = Timestamp to set
     */
    /****************************************************************************/
    void SetModuleTimeStamp(const QString value) { m_ModuleTimeStamp = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the module time stamp
     *  \return Time stamp
     */
    /****************************************************************************/
    QString GetModuleTimeStamp() { return m_ModuleTimeStamp; }

    /****************************************************************************/
    /*!
     *  \brief  To get number of modules in modulelist
     *  \return Number of modules
     */
    /****************************************************************************/
    int GetNumberofModules() { return m_ModuleList.count(); }

    /****************************************************************************/
    /*!
     *  \brief  To get module info using module name
     *  \return Module info
     */
    /****************************************************************************/
    CModule const* GetModule(const QString ModuleName) {return m_ModuleList.value(ModuleName, NULL);}

    /****************************************************************************/
    /*!
     *  \brief  To get module info using Index
     *  \return Module info
     */
    /****************************************************************************/
    CModule* GetModule(const unsigned int Index); // uses order index

    /****************************************************************************/
    /*!
     *  \brief  To read XML file
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool ReadFile(const QString FileName);

    /****************************************************************************/
    /*!
     *  \brief  To write data to XML file
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool WriteFile(const QString FileName);

    /****************************************************************************/
    /*!
     *  \brief  To writes data to container
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool AddModule(CModule const* p_Module);

    /****************************************************************************/
    /*!
     *  \brief  Deletes the Module
     *  \iparam Index
     *  \return true - delete success , false - delete failure
     */
    /****************************************************************************/
    bool DeleteModule(const unsigned int Index);

    /****************************************************************************/
    /*!
     *  \brief  Deletes all the Modules in the list
     *  \return true - delete success , false - delete failure
     */
    /****************************************************************************/
    bool DeleteAllModules();

    /****************************************************************************/
    /*!
     *  \brief Sets the XML file name
     *  \iparam Value = Filename to set
     */
    /****************************************************************************/
    void SetFileName(const QString Value) { m_FileName = Value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the filename of the xml file read
     *  \return File name
     */
    /****************************************************************************/
    QString GetFileName() {return m_FileName;}

    /****************************************************************************/
    /*!
     *  \brief Sets verification mode
     *  \iparam Value = true-verification on, false - verification off
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool value) { m_DataVerificationMode = value; }

    /****************************************************************************/
    /*!
     *  \brief Retrieves the status of Verification
     *  \return true-verification on , false-verification off
     */
    /****************************************************************************/
    bool GetDataVerificationMode() { return m_DataVerificationMode; }

    bool UpdateModule(CModule const* p_Module);  // content of p_Module will be copied  => delete outside!


private:
    QString m_InstrumentName;   //!< name of the Instrument
    QString m_ModuleTimeStamp;  //!< Time Stamp of the Module
    ListofModules_t m_ModuleList;   //!< Module List
    ListofOrderedModules_t m_ListofModules; //!< List of Module Names
    QString m_FileName; //!< XML file name

    QReadWriteLock *mp_ReadWriteLock;
    ErrorHash_t *m_ErrorHash;

    bool m_DataVerificationMode; //!< Verification mode flag , verify the Container

    bool SerializeContent(QIODevice& IODevice, bool CompleteData);
    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);   

    /****************************************************************************/
    /*!
     *  \brief  To read module info from xml
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool ReadModules(QXmlStreamReader& XmlStreamReader, bool CompleteData);
};


}   // namespace DataManager

#endif // DATAMANAGER_MODULEDATALIST_H
