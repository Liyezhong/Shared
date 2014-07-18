/****************************************************************************/
/*! \file DataManager/Containers/InstrumentHistory/Source/Module.cpp
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

#include <Global/Include/Exception.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/Utils.h>

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
    m_CalibrationDate(""),
    m_CalibrationResult(""),
    m_TestDate(""),
    m_TestResult("")
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
CModule::CModule(QString ModuleName, QString Description, QString SerialNumber, QString OperatingHrs)
{
    m_ModuleName = ModuleName;
    m_ModuleDescription = Description;
    m_SerialNumber = SerialNumber;
    m_OperatingHours = OperatingHrs;
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
   CopyFromOther(Module);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \iparam ModuleInfo = Instance of the CModule class
.*  \note  Method for internal use only
 *
 *  \return
 */
/****************************************************************************/
void CModule::CopyFromOther(const CModule &Other)
{
    CModule &OtherModule = const_cast<CModule&>(Other);

    m_ModuleName = OtherModule.GetModuleName();
    m_ModuleDescription = OtherModule.GetModuleDescription();
    m_SerialNumber = OtherModule.GetSerialNumber();
    m_OperatingHours = OtherModule.GetOperatingHours();
    m_CalibrationDate = OtherModule.GetCalibrationDate();
    m_CalibrationResult = OtherModule.GetCalibrationResult();
    m_TestDate = OtherModule.GetTestDate();
    m_TestResult = OtherModule.GetTestResult();

    //qDebug() <<"SubModuleList Keys"<< m_SubModuleList.keys().toSet();
    //qDebug()<<"Other SubModuleList keys" << Other.GetSubModuleIdList().toSet();
    QSet<QString> SubModulesInOurList(static_cast< QSet<QString> >(m_SubModuleList.keys().toSet()));
    QSet<QString> SubModulesInOtherList(Other.GetSubModuleIdList().toSet());
    //we get a list of SubModules that are not there in new list but present in our list, they are to be deleted.
    QSet<QString> SubModulesToDelete(SubModulesInOurList.subtract(SubModulesInOtherList));
    QSetIterator<QString> Itr(SubModulesToDelete);
    while (Itr.hasNext()) {
        DeleteSubModule(Itr.next());
    }

    //Recreate SubModules in our list
    SubModulesInOurList = m_SubModuleList.keys().toSet();
    //Update SubModules which are present in our list and other list
    QSet<QString> SubModulesToUpdate(static_cast< QSet<QString> >(SubModulesInOurList.intersect(SubModulesInOtherList)));
    QSetIterator<QString> UpdateItr(SubModulesToUpdate);
    while (UpdateItr.hasNext()) {
        const QString SubModuleID = UpdateItr.next();
        const CSubModule *p_OtherSubModule = Other.GetSubModuleInfo(SubModuleID);
        CSubModule *p_OurSubModule = GetSubModuleInfo(SubModuleID);
        if (p_OtherSubModule && p_OurSubModule) {
            //There exist a SubModule with similar ID in the other SubModulelist.
            //update our SubModule with values from other.
             *p_OurSubModule = *p_OtherSubModule;
        }
    }

    //Finally create SubModules which are not present in our list but in other list.
    QSet<QString> SubModulesToCreate(SubModulesInOtherList.subtract(SubModulesInOurList));
    QSetIterator<QString> CreateItr(SubModulesToCreate);
    while (CreateItr.hasNext()) {
        const CSubModule *p_OtherSubModule = Other.GetSubModuleInfo(CreateItr.next());
        AddSubModuleInfo(p_OtherSubModule);
    }

    m_OrderedSubModuleList.clear();
    m_OrderedSubModuleList = OtherModule.m_OrderedSubModuleList;
}

/****************************************************************************/
/*!
 *  \brief  Returns the ID's of all sub modules present in the list
 *
 *  \return QStringList = list ofsub  moduled Ids
 */
/****************************************************************************/
QStringList CModule::GetSubModuleIdList() const
{
    return m_SubModuleList.keys();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CModule::~CModule()
{
    try {
        (void)DeleteAllSubModule(); // to avoid lint-534
    }
    CATCHALL_DTOR();
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
//            qDebug() << XmlStreamReader.name().toString();
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

    QString Name = const_cast<CSubModule*>(p_SubModule)->GetSubModuleName();
    if(m_OrderedSubModuleList.contains(Name)){
        qDebug() << "Name already exists";
        return false;
    }

    CSubModule *p_TempSubModule = new CSubModule();
    *p_TempSubModule = *p_SubModule;

    m_SubModuleList.insert(Name, p_TempSubModule);
    m_OrderedSubModuleList.append(Name);    

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
        if (IndexCount == -1) {
            return false;
        }
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
    CSubModule* p_SubModuleData = new CSubModule();
    if (p_SubModule == NULL || p_SubModuleData == NULL) {
        return false;
    }

    *p_SubModuleData = *p_SubModule;
    if (m_SubModuleList.contains(Name)) {
        m_SubModuleList.insert(Name, p_SubModuleData);
        return Result;
    }
    else {
        return false;
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads the CModule Data from QIODevice
 *
 *  \iparam XmlStreamReader = Xml reader to read the XML contents
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CModule::DeserializeContent(const QXmlStreamReader& XmlStreamReader, bool CompleteData)
{    
    bool Result = true;
    QXmlStreamReader &XmlStreamReaderRef = const_cast<QXmlStreamReader &>(XmlStreamReader);
    //Module Name
    if (!XmlStreamReaderRef.attributes().hasAttribute("name")) {
        qDebug() <<  " attribute Module <name> is missing => abort reading";
        return false;
    }
    SetModuleName(XmlStreamReaderRef.attributes().value("name").toString());

    //Module Description
    if (!XmlStreamReaderRef.attributes().hasAttribute("description")) {
        qDebug() <<  " attribute Module <description> is missing => abort reading";
        return false;
    }
    SetModuleDescription(XmlStreamReaderRef.attributes().value("description").toString());

    // Serial Number
    if (!Helper::ReadNode(XmlStreamReaderRef, "SerialNumber")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: SerialNumber";
        return false;
    }
    SetSerialNumber(XmlStreamReaderRef.readElementText());

    //Operating Hrs
    if (!Helper::ReadNode(XmlStreamReaderRef, "OperatingHours")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: OperatingHours";
        return false;
    }
    SetOperatingHours(XmlStreamReaderRef.readElementText());

    //Calibration Date
    if (!Helper::ReadNode(XmlStreamReaderRef, "CalibrationDate")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: CalibrationDate";
        return false;
    }
    SetCalibrationDate(XmlStreamReaderRef.readElementText());

    //Calibration Result
    if (!Helper::ReadNode(XmlStreamReaderRef, "CalibrationResult")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: CalibrationResult";
        return false;
    }
    SetCalibrationResult(XmlStreamReaderRef.readElementText());

    //Test Date
    if (!Helper::ReadNode(XmlStreamReaderRef, "TestDate")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: TestDate";
        return false;
    }
    SetTestDate(XmlStreamReaderRef.readElementText());

    //Test Result
    if (!Helper::ReadNode(XmlStreamReaderRef, "TestResult")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: TestResult";
        return false;
    }
    SetTestResult(XmlStreamReaderRef.readElementText());

    Result = ReadSubModuleInfo(XmlStreamReaderRef, CompleteData);

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Writes the CModule Data to QIODevice
 *
 *  \iparam XmlStreamWriter = Xml stream writer to write the XML contents
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

    XmlStreamWriter.writeStartElement("CalibrationDate");
    XmlStreamWriter.writeCharacters(GetCalibrationDate());
    XmlStreamWriter.writeEndElement(); // CalibrationDate

    XmlStreamWriter.writeStartElement("CalibrationResult");
    XmlStreamWriter.writeCharacters(GetCalibrationResult());
    XmlStreamWriter.writeEndElement(); // CalibrationResult

    XmlStreamWriter.writeStartElement("TestDate");
    XmlStreamWriter.writeCharacters(GetTestDate());
    XmlStreamWriter.writeEndElement(); // TestDate

    XmlStreamWriter.writeStartElement("TestResult");
    XmlStreamWriter.writeCharacters(GetTestResult());
    XmlStreamWriter.writeEndElement(); // TestResult

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

        //this->DeleteAllSubModule();
        CopyFromOther(ModuleInfo);
    }

    return *this;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam Moduleinfo = CModule class object
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
 *  \iparam Moduleinfo = CModule class object
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
