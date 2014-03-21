/****************************************************************************/
/*! \file Module.cpp
 *
 *  \brief Implementation file for class CModule.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-21
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

#include <QDebug>
#include <QFile>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "DataManager/Containers/InstrumentHistory/Include/Module.h"
#include "DataManager/Helper/Include/Helper.h"

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CModule::CModule() :m_ModuleName(""),
    m_ModuleDescription(""),
    m_SerialNumber(""),    
    m_OperatingHours(""),
    m_DateOfProduction("")
{

}

/****************************************************************************/
/*!
 *  \brief Parameterized Constructor
 *
 *  \iparam ModuleName
 */
/****************************************************************************/
CModule::CModule(QString ModuleName)
{
    m_ModuleName = ModuleName;
}

/****************************************************************************/
/*!
 *  \brief Parameterized Constructor
 */
/****************************************************************************/
CModule::CModule(QString ModuleName, QString Description, QString SerialNumber, QString OperatingHrs, QString DateOfProduction)
{
    m_ModuleName = ModuleName;
    m_ModuleDescription = Description;
    m_SerialNumber = SerialNumber;
    m_OperatingHours = OperatingHrs;
    m_DateOfProduction = DateOfProduction;
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam Module = Instance of the Module class
 *
 */
/****************************************************************************/
CModule::CModule(const CModule& Module)
{
    *this = Module;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CModule::~CModule()
{
    DeleteAllSubModule();
}

/****************************************************************************/
/*!
 *  \brief Searches for the node and returns true if found
 *
 *  \iparam XmlStreamReader = Instance of the QXmlStreamReader
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CModule::ReadSubModuleInfo(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    bool Result = true;

    // Look for node <SubModule>
    while (!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "SubModule") {

                DataManager::CSubModule SubModule;
                if (!SubModule.DeserializeContent(XmlStreamReader, CompleteData)) {
                    qDebug() << "CSubModule Deserialize failed!";
                    return false;
                }

                if (!AddSubModuleInfo(&SubModule)) {
                    qDebug() << "CModule::Add SubModuleInfo failed!";
                    return false;
                }              
            }
            else {
                qDebug() << " Unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return true;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "Module") {
            qDebug() << XmlStreamReader.name().toString();
            break; // exit from while loop
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Adds SubModuleInfo to Module
 *  \iparam p_SubModule = SubModuleInfo to add
 *  \warning
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
bool CModule::AddSubModuleInfo(CSubModule const* p_SubModule)
{
    bool Result = true;
    if(p_SubModule == NULL) return false;

    QString name = const_cast<CSubModule*>(p_SubModule)->GetSubModuleName();
    if(m_SubModuleList.contains(name)){
        qDebug() << "Name already exists";
        return false;
    }

    CSubModule *p_TempSubModule = new CSubModule();
    *p_TempSubModule = *p_SubModule;

    m_SubModuleList.insert(p_TempSubModule->GetSubModuleName(), p_TempSubModule);
    m_OrderedSubModuleList.append(p_TempSubModule->GetSubModuleName());

    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Deletes the SubModule
 *  \iparam Index
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CModule::DeleteSubModule(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedSubModuleList.count()) {

        QString SubModuleName = m_OrderedSubModuleList.value(Index);
        DeleteSubModule(SubModuleName);
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the SubModule
 *  \iparam SubModuleName
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CModule::DeleteSubModule(const QString SubModuleName)
{
    if (m_SubModuleList.contains(SubModuleName)) {
        //get Module from ModuleList and free memory
        delete m_SubModuleList.value(SubModuleName);

        //remove pointer to Module from ModuleList
        m_SubModuleList.remove(SubModuleName);

        //remove Module from list
        int IndexCount = -1;
        for (int i=0; i<m_OrderedSubModuleList.count(); i++)
        {
            if (m_OrderedSubModuleList[i] == SubModuleName) {
                IndexCount = i;
                break;
            }
        }
        // IndexCount MUST never be -1
        Q_ASSERT(IndexCount != -1);
        m_OrderedSubModuleList.removeAt(IndexCount);
    }
    else {
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Deletes all the submodules in the list
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CModule::DeleteAllSubModule()
{
    bool Result = true;

    while (m_SubModuleList.size() > 0)
    {
        if (DeleteSubModule(0) == false) {
            Result = false;
        }
    }

    m_SubModuleList.clear();
    m_OrderedSubModuleList.clear();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Updates SubModule
 *  \iparam p_SubModule = Sub Module which needs to be updated
 *  \return true - update success, false - update failed
 */
/****************************************************************************/
bool CModule::UpdateSubModule(const CSubModule* p_SubModule)
{
    bool Result = true;
    QString Name = const_cast<CSubModule*>(p_SubModule)->GetSubModuleName();
    CSubModule* p_CurrentSubModuleData = GetSubModuleInfo(Name);
    CSubModule* p_SubModuleData = m_SubModuleList.value(Name, NULL);

    if (QString::compare(p_SubModuleData->GetSubModuleName(), p_CurrentSubModuleData->GetSubModuleName()) == 0) {
        *p_SubModuleData = *p_CurrentSubModuleData;

        Result = true;
        if (p_SubModuleData == NULL) {
            return false;
        }

        if (m_SubModuleList.contains(Name)) {
            m_SubModuleList.insert(Name, p_SubModuleData);
            return Result;
        }
        else {
            return false;
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads the CModule Data from QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice - Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CModule::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{    
    bool Result = true;

    //Module Name
    if (!XmlStreamReader.attributes().hasAttribute("name")) {
        qDebug() <<  " attribute Module <name> is missing => abort reading";
        return false;
    }
    SetModuleName(XmlStreamReader.attributes().value("name").toString());   

    //Module Description
    if (!XmlStreamReader.attributes().hasAttribute("description")) {
        qDebug() <<  " attribute Module <description> is missing => abort reading";
        return false;
    }
    SetModuleDescription(XmlStreamReader.attributes().value("description").toString());

    // Serial Number
    if (!Helper::ReadNode(XmlStreamReader, "SerialNumber")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: SerialNumber";
        return false;
    }
    SetSerialNumber(XmlStreamReader.readElementText());      

    //Operating Hrs
    if (!Helper::ReadNode(XmlStreamReader, "OperatingHours")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: OperatingHours";
        return false;
    }
    SetOperatingHours(XmlStreamReader.readElementText());

    //Date of production
    if (!Helper::ReadNode(XmlStreamReader, "DateOfProduction")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: DateOfProduction";
        return false;
    }
    SetDateOfProduction(XmlStreamReader.readElementText());   

    Result = ReadSubModuleInfo(XmlStreamReader, CompleteData);

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Writes the CModule Data to QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CModule::SerializeContent(QXmlStreamWriter &XmlStreamWriter, bool CompleteData)
{
    bool Result = true;

    XmlStreamWriter.writeStartElement("Module");
    XmlStreamWriter.writeAttribute("name", GetModuleName());
    XmlStreamWriter.writeAttribute("description",GetModuleDescription());

    XmlStreamWriter.writeStartElement("SerialNumber");
    XmlStreamWriter.writeCharacters(GetSerialNumber());
    XmlStreamWriter.writeEndElement(); // Serial Number

    XmlStreamWriter.writeStartElement("OperatingHours");
    XmlStreamWriter.writeCharacters(GetOperatingHours());
    XmlStreamWriter.writeEndElement(); // OperatingHours

    XmlStreamWriter.writeStartElement("DateOfProduction");
    XmlStreamWriter.writeCharacters(GetDateOfProduction());
    XmlStreamWriter.writeEndElement(); // DateOfProduction

    for (int i = 0; i < GetNumberofSubModules(); i++)
    {
        DataManager::CSubModule* p_SubModule = GetSubModuleInfo(i);
        if (p_SubModule != NULL) {
            if (!p_SubModule->SerializeContent(XmlStreamWriter, CompleteData)) {
                qDebug("DataManager::CModule SerializeContent failed ");
                return false;
            }
        }
    }
    XmlStreamWriter.writeEndElement(); // Module
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Function to retrieve submodule for given index
 *  \iparam Index = Index in submodule list
 *
 *  \return submodule associated with the index
 */
/****************************************************************************/
CSubModule* CModule::GetSubModuleInfo(const unsigned int Index) const
{
    if (Index < (unsigned int)m_OrderedSubModuleList.count()) {
        QString Key = m_OrderedSubModuleList.value(Index);
        CSubModule* p_SubModule = m_SubModuleList.value(Key, NULL);
        return p_SubModule;
    }
    return NULL;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam ModuleInfo = CModule class object
 *
 *  \return CModule Class Object
 */
/****************************************************************************/
CModule& CModule::operator=(const CModule& ModuleInfo)
{
    if(this != &ModuleInfo) {

        this->DeleteAllSubModule();

        QString Name = const_cast<CModule&>(ModuleInfo).GetModuleName();
        QString Description = const_cast<CModule&>(ModuleInfo).GetModuleDescription();
        QString SerialNumber = const_cast<CModule&>(ModuleInfo).GetSerialNumber();
        QString OpHrs = const_cast<CModule&>(ModuleInfo).GetOperatingHours();
        QString DateOProd = const_cast<CModule&>(ModuleInfo).GetDateOfProduction();

        this->SetModuleName(Name);
        this->SetModuleDescription(Description);
        this->SetSerialNumber(SerialNumber);
        this->SetOperatingHours(OpHrs);
        this->SetDateOfProduction(DateOProd);

        int Count = const_cast<CModule&>(ModuleInfo).GetNumberofSubModules();

        for(size_t i=0; i<Count; i++)
        {
            CSubModule *SubModule = const_cast<CModule&>(ModuleInfo).GetSubModuleInfo(i);
            this->AddSubModuleInfo(SubModule);
        }
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam CModule = CModule class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator << (QDataStream& OutDataStream, const CModule& Moduleinfo)
{
    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());
    CModule *p_TempModule = const_cast<CModule*>(&Moduleinfo);
    if (!p_TempModule->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CModule::Operator Streaming (SerializeContent) failed.";
    }

    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam CModule = CModule class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >> (QDataStream& InDataStream, CModule& Moduleinfo)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());    

    if (!Helper::ReadNode(XmlStreamReader, QString("Module"))) {
        qDebug() << "CModule::Operator Streaming (DeSerializeContent) Node not found: Module";
    }

    if (!Moduleinfo.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CModule::Operator Streaming (DeSerializeContent) failed.";
    }

    return InDataStream;
}

}   // namespace DataManager
