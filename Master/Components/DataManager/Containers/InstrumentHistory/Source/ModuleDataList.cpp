/****************************************************************************/
/*! \file DataManager/Containers/InstrumentHistory/Source/ModuleDataList.cpp
 *
 *  \brief Implementation file for class CModuleDataList.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-21
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

#include "DataManager/Containers/InstrumentHistory/Include/ModuleDataList.h"
#include "DataManager/Helper/Include/Helper.h"
#include "Global/Include/Utils.h"
//#include "Global/Include/Exception.h"
//#include "Global/Include/EventObject.h"

#include <QDebug>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDataStream>

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CModuleDataList::CModuleDataList() :
    m_ModuleTimeStamp("")
{   

}

/****************************************************************************/
/*!
 *  \brief Parameterized Constructor
 *  \iparam TimeStamp
 */
/****************************************************************************/
CModuleDataList::CModuleDataList(QString TimeStamp) :
    m_ModuleTimeStamp(TimeStamp)
{

}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ModuleDataList = Instance of the ModuleList class
 *
 */
/****************************************************************************/
CModuleDataList::CModuleDataList(const CModuleDataList& ModuleDataList)
{    
    CopyFromOther(ModuleDataList);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \iparam Other = Instance of the CModuleDataList class
.*  \note  Method for internal use only
 *
 *  \return
 */
/****************************************************************************/
void CModuleDataList::CopyFromOther(const CModuleDataList &Other)
{    
    QString TimeStamp = const_cast<CModuleDataList&>(Other).GetModuleTimeStamp();   
    this->SetModuleTimeStamp(TimeStamp);

    //qDebug() <<"ModuleList Keys"<< m_ModuleList.keys().toSet();
    //qDebug()<<"Other ModuleList keys" << Other.GetModuleIdList().toSet();
    QSet<QString> ModulesInOurList(static_cast< QSet<QString> >(m_ModuleList.keys().toSet()));
    QSet<QString> ModulesInOtherList(Other.GetModuleIdList().toSet());
    //we get a list of Modules that are not there in new list but present in our list, they are to be deleted.
    QSet<QString> ModulesToDelete(ModulesInOurList.subtract(ModulesInOtherList));
    QSetIterator<QString> Itr(ModulesToDelete);
    while (Itr.hasNext()) {
        DeleteModule(Itr.next());
    }

    //Recreate Modules in our list
    ModulesInOurList = m_ModuleList.keys().toSet();
    //Update Modules which are present in our list and other list
    QSet<QString> ModulesToUpdate(static_cast< QSet<QString> >(ModulesInOurList.intersect(ModulesInOtherList)));
    QSetIterator<QString> UpdateItr(ModulesToUpdate);
    while (UpdateItr.hasNext()) {
        const QString ModuleID = UpdateItr.next();
        const CModule *p_OtherModule = Other.GetModule(ModuleID);
        CModule *p_OurModule = GetModule(ModuleID);
        if (p_OtherModule && p_OurModule) {
            //There exist a Module with similar ID in the other Modulelist.
            //update our Module with values from other.
             *p_OurModule = *p_OtherModule;
        }
    }

    //Finally create Modules which are not present in our list but in other list.
    QSet<QString> ModulesToCreate(ModulesInOtherList.subtract(ModulesInOurList));
    QSetIterator<QString> CreateItr(ModulesToCreate);
    while (CreateItr.hasNext()) {
        const CModule *p_OtherModule = Other.GetModule(CreateItr.next());
        AddModuleWithoutVerification(p_OtherModule);
    }

    m_ListofModules.clear();
    m_ListofModules = Other.m_ListofModules;
}

/****************************************************************************/
/*!
 *  \brief  Returns the ID's of all modules present in the list
 *
 *  \return QStringList = list of moduled Ids
 */
/****************************************************************************/
QStringList CModuleDataList::GetModuleIdList() const
{
    return m_ModuleList.keys();
}

/****************************************************************************/
/*!
 *  \brief Adds Module to ModuleList
 *  \iparam p_Module = Module to add
 *  \warning
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
void CModuleDataList::AddModuleWithoutVerification(CModule const* p_Module)
{
    try {
        CHECKPTR(p_Module)

        CModule *p_TempModule = new CModule();
        *p_TempModule = *p_Module;

        m_ModuleList.insert(p_TempModule->GetModuleName(), p_TempModule);
        m_ListofModules.append(p_TempModule->GetModuleName());
    }
    CATCHALL()
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CModuleDataList::~CModuleDataList()
{    
    //Ignoring return value
    try {
        (void)DeleteAllModules(); //to avoid lint warning 534
    }
    CATCHALL_DTOR();

}

/****************************************************************************/
/*!
 *  \brief Reads the CModuleDataList Data from QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice - Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CModuleDataList::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{   
    bool Result = true;
    // Read Attribute TimeStamp
    if (!XmlStreamReader.attributes().hasAttribute("timestamp")) {
        qDebug() << " attribute <timestamp> is missing => abort reading";
        return false;
    }

    SetModuleTimeStamp(XmlStreamReader.attributes().value("timestamp").toString());

    Result = ReadModules(XmlStreamReader, CompleteData);

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Writes the CModuleDataList Data to QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CModuleDataList::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    // write section ModuleList
    XmlStreamWriter.writeStartElement("ModuleList");

    // write attribute timestamp
    XmlStreamWriter.writeAttribute("timestamp", GetModuleTimeStamp());

    // write all modules
    for (int i = 0; i < GetNumberofModules(); i++)
    {
        DataManager::CModule *p_Module = GetModule(i);
        if (p_Module != NULL) {
            if (!p_Module->SerializeContent(XmlStreamWriter, CompleteData)) {
                qDebug("DataManager::CModule SerializeContent failed ");
                return false;
            }
        }
    }

    XmlStreamWriter.writeEndElement(); // ModuleList

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads all the modules from the xml stream
 *
 *  \iparam XmlStreamReader = Reference to xml stream to read from
 *  \iparam CompleteData = true to read Complete data of object
 *
 *  \return true - read success or false - read failure
 */
/****************************************************************************/
bool CModuleDataList::ReadModules(const QXmlStreamReader &XmlStreamReader, bool CompleteData)
{
    bool Result = true;

    // Look for node <Module>
    while(!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(const_cast<QXmlStreamReader &>(XmlStreamReader).readNext()) == 1) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "Module") {
                DataManager::CModule Module;

                if (!Module.DeserializeContent(XmlStreamReader, CompleteData)) {
                    qDebug() << "second " << "CModule Deserialize failed!";
                    return false;
                }

                if (!AddModule(&Module)) {
                    qDebug() << "CModuleDataList::Add Module failed!";
                    return false;
                }
            } else {
                qDebug() << " Unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        } else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "ModuleList") {
            //            qDebug() << XmlStreamReader.name().toString();
            break; // exit from while loop
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Adds Module to ModuleList
 *  \iparam p_Module = Module to add
 *  \warning
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
bool CModuleDataList::AddModule(CModule const* p_Module)
{
    bool Result = true;
    if(p_Module == NULL) return false;

    QString name = const_cast<CModule*>(p_Module)->GetModuleName();
    if(m_ListofModules.contains(name)) {
        qDebug() << "Name already exists";
        return false;
    }
    Result = true;
    CModule *p_TempModule = new CModule();
    *p_TempModule = *p_Module;

    //QWriteLocker locker(mp_ReadWriteLock);
    m_ModuleList.insert(p_TempModule->GetModuleName(), p_TempModule);
    m_ListofModules.append(p_TempModule->GetModuleName());

    Result = true;
    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Updates Module
 *  \iparam p_Module = Module which needs to be updated
 *  \return true - update success, false - update failed
 */
/****************************************************************************/
bool CModuleDataList::UpdateModule(CModule const* p_Module)
{

    CHECKPTR_RETURN(p_Module, false);

    QString ModuleName = p_Module->GetModuleName();
    if (!m_ModuleList.contains(ModuleName))
    {
        return false;
    }

    bool Result = false;

    // do a deep copy
    *m_ModuleList.value(ModuleName) = *p_Module;
    Result = true;
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Function to retrieve module for given index
 *  \iparam Index = Index in module list
 *
 *  \return Module associated with the index
 */
/****************************************************************************/
CModule* CModuleDataList::GetModule(const unsigned int Index)
{
    if (Index < (unsigned int)m_ListofModules.count()) {
        QString Key = m_ListofModules.value(Index);
        CModule* p_Module = m_ModuleList.value(Key, NULL);

        return p_Module;
    }
    return NULL;
}

/****************************************************************************/
/*!
 *  \brief Function to retrieve module Infor for given Module Name. Module
 *  passed is Updated with a ModuleInfo having ModuleName in the list
 *
 *  \iparam ModuleName = Name of the Module
 *  \iparam Module = Module reference
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CModuleDataList::GetModule(const QString ModuleName, CModule& Module)
{    
    if (m_ModuleList.contains(ModuleName)) {
        Module = *(m_ModuleList.value(ModuleName));
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the Module
 *  \iparam Index
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CModuleDataList::DeleteModule(const unsigned int Index)
{
    if (Index < (unsigned int)m_ListofModules.count()) {
        QString ModuleName = m_ListofModules.value(Index);
        return DeleteModule(ModuleName);
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the Module
 *  \iparam ModuleName
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CModuleDataList::DeleteModule(const QString ModuleName)
{
    if(m_ModuleList.contains(ModuleName)) {
        //get Module from ModuleList and free memory
        delete m_ModuleList.value(ModuleName);

        //remove pointer to Module from ModuleList
        m_ModuleList.remove(ModuleName);

        //remove Module from list
        int IndexCount = -1;
        for (int i=0; i<m_ListofModules.count(); i++)
        {
            if (m_ListofModules[i] == ModuleName) {
                IndexCount = i;
                break;
            }
        }

        // IndexCount MUST never be -1
        m_ListofModules.removeAt(IndexCount);
        return true;
    }
    else {
        return false;
    }

    //return true;
}

/****************************************************************************/
/*!
 *  \brief  Deletes all the programs in the list
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CModuleDataList::DeleteAllModules()
{
    bool Result = true;

    while (m_ModuleList.size() > 0)
    {
        if (DeleteModule(0) == false) {
            Result = false;
        }
    }

    m_ModuleList.clear();
    m_ListofModules.clear();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam ListofModules = CModuleDataList class object
 *
 *  \return CModuleDataList Class Object
 */
/****************************************************************************/
CModuleDataList& CModuleDataList::operator=(const CModuleDataList& ListofModules)
{
    if (this != &ListofModules) {

        CopyFromOther(ListofModules);
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ModuleList = CModuleDataList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator << (const QDataStream& OutDataStream, const CModuleDataList& ModuleList)
{
    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());
    QDataStream &OutDataStreamRef = const_cast<QDataStream &>(OutDataStream);
    CModuleDataList* p_TempModuleList = const_cast<CModuleDataList*>(&ModuleList);

    if (!p_TempModuleList->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CModuleList::Operator Streaming (SerializeContent) failed.";
    }
    return OutDataStreamRef;

}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam ModuleList = CModuleDataList class object
 *
 *  \return Input Stream
 *
 */
/****************************************************************************/
QDataStream& operator >> (const QDataStream& InDataStream, const CModuleDataList& ModuleList)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());
    QDataStream &InDataStreamRef = const_cast<QDataStream &>(InDataStream);
    CModuleDataList &ModuleListRef = const_cast<CModuleDataList &>(ModuleList);

    if (!Helper::ReadNode(XmlStreamReader, QString("ModuleList"))) {
        qDebug() << "CModuleList::Operator Streaming (DeSerializeContent) Node not found: Module";
    }

    if (!ModuleListRef.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CModuleList::Operator Streaming (DeSerializeContent) failed.";
    }

    return InDataStreamRef;
}

}   // namespace DataManager

