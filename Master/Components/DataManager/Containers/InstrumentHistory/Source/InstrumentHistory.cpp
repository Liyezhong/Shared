/****************************************************************************/
/*! \file InstrumentHistory.cpp
 *
 *  \brief Implementation file for class InstrumentHistory.
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

#include "DataManager/Containers/InstrumentHistory/Include/InstrumentHistory.h"
#include <Global/Include/SystemPaths.h>
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Helper.h"
#include <QtXml>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CInstrumentHistory::CInstrumentHistory() : CDataContainerBase(),
    m_FileName(""),
    m_DataVerificationMode(true),
    mp_ModuleList(NULL),
    mp_ReadWriteLock(NULL)
{
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *  \iparam InstrumentHistory = Instance of the InstrumentHistory class
 */
/****************************************************************************/
CInstrumentHistory::CInstrumentHistory(const CInstrumentHistory& InstrumentHistory)
{
    try {
        mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
        *this = InstrumentHistory;
    }
    catch (...) {

    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CInstrumentHistory::~CInstrumentHistory()
{
    //Ignoring return value
    try {
        (void)DeleteAllModuleList(); //to avoid lint warning 534
    }
    CATCHALL_DTOR();

    if (mp_ReadWriteLock != NULL) {
        try {
            delete mp_ReadWriteLock;
        }
        CATCHALL_DTOR();
        mp_ReadWriteLock = NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief Reads an xml file and fill ModuleListArchive object
 *  \iparam FileName = Filename to read
 *  \return true on success, false on failure
 */
/****************************************************************************/
bool CInstrumentHistory::ReadFile(const QString FileName)
{
    QFile File(FileName);
    if (!QFile::exists(FileName)) {
        return false;
    }
    bool Result = true;
    if (m_DataVerificationMode) {
        //To make sure other threads cant write when reading is active
        QWriteLocker locker(mp_ReadWriteLock);

        // create instance of CModuleDataList for verification
        CInstrumentHistory* p_MDL_Verification = new CInstrumentHistory();

        // create clone from current state
        *p_MDL_Verification = *this;

        // disable verification in clone
        p_MDL_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        if (!p_MDL_Verification->ReadFile(FileName)) {
            Result = false;
        } else {

            // now check new content => call all active verifiers
            if (DoLocalVerification(p_MDL_Verification)) {
                // if content ok, clone backwards
                *this = *p_MDL_Verification;
                Result = true;
            }
            else {
                Result = false;
            }
        }
        // delete test clone
        delete p_MDL_Verification;
    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);

        // Initialise the m_Filename to a known string "UNDEFINED"
        m_FileName = "UNDEFINED";

        if (!File.open(QFile::ReadOnly | QFile::Text )) {
            qDebug() << "Open file to read failed " << FileName;
            return false;
        }
        m_FileName = FileName;

        if (!DeserializeContent(File, false)) {
            qDebug() << " CInstrumentHistory::Read failed for file during deserializing: " << FileName;
            return false;
        }

        File.close();
    }

    return Result;
}

//Review: to be removed
/****************************************************************************/
/*!
 *  \brief Reads an xml file and fill ModuleListArchive object
 *  \iparam FileName = Filename to write
 *  \return true on success, false on failure
 */
/****************************************************************************/
bool CInstrumentHistory::WriteArchiveFile(const QString FileName)
{
    QString InstrumentHistoryFile = Global::SystemPaths::Instance().GetSettingsPath() + "/InstrumentHistory.xml";
    QFile XMLFile(InstrumentHistoryFile);
    if (!QFile::exists(InstrumentHistoryFile)) {
        return false;
    }

    if ( !XMLFile.open( QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Open file to write failed" << InstrumentHistoryFile;
        return false;
    }

    QDomDocument DocumentXML;
    (void) DocumentXML.setContent(&XMLFile);
    QDomElement Element = DocumentXML.documentElement();
    QDomNodeList List = Element.elementsByTagName("ModuleList"); //.childNodes();
    XMLFile.close();

    QFile File(FileName);    
    if (!QFile::exists(FileName)) {
        return false;
    }

    bool Result = true;

    if ( !File.open( QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Open file to write failed" << FileName;
        return false;
    }

    QDomDocument DocumentArchive;
    (void) DocumentArchive.setContent(&File);
    QDomElement ElementArchive = DocumentArchive.documentElement();
    (void) ElementArchive.appendChild(List.at(0).toElement());

    File.close();

    if ( !File.open( QFile::WriteOnly | QFile::Text)) {
        qDebug() << "Open file to write failed" << FileName;
        return false;
    }

    QByteArray Byte = DocumentArchive.toByteArray();
    (void) File.write(Byte);

    File.close();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief  To read moduleList info from xml
 *  \iparam XmlStreamReader = Xml reader to read the XML contents
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *  \return true on success, false on failure
 */
/****************************************************************************/
bool CInstrumentHistory::ReadModuleLists(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    bool Result = true;

    // Look for node <Module>
    while(!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "ModuleList") {
                DataManager::CModuleDataList ModuleList;

                if (!ModuleList.DeserializeContent(XmlStreamReader, CompleteData)) {
                    qDebug() << "second " << "CModuleDataListArchive Deserialize failed!";
                    return false;
                }

                if (!AddModuleListInfo(&ModuleList)) {
                    qDebug() << "CModuleDataList::Add Module failed!";
                    return false;
                }
            } else {
                qDebug() << " Unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        } else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "InstrumentHistory") {
            break; // exit from while loop
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads the CInstrumentHistoryArchive Data from QIODevice
 *  \iparam IODevice = Instance of the IODevice - Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *  \return True or False
 */
/****************************************************************************/
bool CInstrumentHistory::DeserializeContent(QIODevice &IODevice, bool CompleteData)
{
    bool Result = true;
    QXmlStreamReader XmlStreamReader;    
    XmlStreamReader.setDevice(&(const_cast<QIODevice &>(IODevice)));

    // look for node <InstrumentHistory>
    if (!Helper::ReadNode(XmlStreamReader, "InstrumentHistory")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: InstrumentHistory";
        return false;
    }

    // Read attribute Name
    if (!XmlStreamReader.attributes().hasAttribute("name")) {
        qDebug() << " attribute System <name> is missing => abort reading";
        return false;
    }

    SetInstrumentName(XmlStreamReader.attributes().value("name").toString());

    Result = ReadModuleLists(XmlStreamReader, CompleteData);

    if (CompleteData && Result) {
        if (!Helper::ReadNode(XmlStreamReader, "NonXmlData")) {
            return false;
        }

        if (!XmlStreamReader.attributes().hasAttribute("VerificationMode")) {
            qDebug() << "### attribute <VerificationMode> is missing";
            return false;
        }

        if (XmlStreamReader.attributes().value("VerificationMode").toString() == "true") {
            SetDataVerificationMode(true);
        }
        else {
            SetDataVerificationMode(false);
        }

        if (!XmlStreamReader.attributes().hasAttribute("FileName")) {
            qDebug() << "### attribute <FileName> is missing";
            return false;
        }
        SetFilename(XmlStreamReader.attributes().value("FileName").toString());

    }

    (void)XmlStreamReader.device()->reset();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Writes the CInstrumentHistoryArchive Data to QIODevice
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *  \return True or False
 */
/****************************************************************************/
bool CInstrumentHistory::SerializeContent(QIODevice &IODevice, bool CompleteData)
{
    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(&(const_cast<QIODevice &>(IODevice)));

    XmlStreamWriter.setAutoFormatting(true);
    XmlStreamWriter.writeStartDocument();

    // XmlStreamWriter.writeDTD("<InstrumentHistory>");
    XmlStreamWriter.writeStartElement("InstrumentHistory");

    // write attribute name
    XmlStreamWriter.writeAttribute("name", GetInstrumentName());

    // write all modulelists
    for (int i = 0; i < GetNumberOfModuleLists(); i++)
    {
        DataManager::CModuleDataList *p_ModuleList = GetModuleList(i);
        if (p_ModuleList != NULL) {
            if (!p_ModuleList->SerializeContent(XmlStreamWriter, CompleteData)) {
                qDebug("DataManager::CModuleDataListArchive SerializeContent failed ");
                return false;
            }
        }
    }

    if (CompleteData) {
        XmlStreamWriter.writeStartElement("NonXmlData");
        if (GetDataVerificationMode()) {
            XmlStreamWriter.writeAttribute("VerificationMode", "true");
        } else {
            XmlStreamWriter.writeAttribute("VerificationMode", "false");
        }

        XmlStreamWriter.writeAttribute("FileName", GetFilename());
        XmlStreamWriter.writeEndElement();
    }

    XmlStreamWriter.writeEndElement(); // for Instrument History
    XmlStreamWriter.writeEndDocument(); // End of Document

    return true;
}

/****************************************************************************/
/*!
 *  \brief  To writes data to container
 *  \iparam p_ModuleList = ModuleList Object
 *  \return true on success, false on failure
 */
/****************************************************************************/
bool CInstrumentHistory::AddModuleListInfo(CModuleDataList *p_ModuleList)
{
    bool Result = true;
    if(p_ModuleList == NULL) return false;

    Result = true;
    if ((m_DataVerificationMode)) {
        CInstrumentHistory* p_MDL_Verification = new CInstrumentHistory();
        CHECKPTR(p_MDL_Verification);
        //p_MDL_Verification = NULL;

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_MDL_Verification = *this;

            // disable verification in clone
            p_MDL_Verification->SetDataVerificationMode(false);

            // execute required action (AddModule) in clone
            Result = p_MDL_Verification->AddModuleListInfo(p_ModuleList);

            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_MDL_Verification);
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_MDL_Verification;
        }
        else {
            //Store errors.
            // Since we are going to delete p_MDL_Verification
            // We need to store the errors generated by it.
            // For now Errors are not copied by assignment or copy constructors.
            ListOfErrors_t ErrorList = p_MDL_Verification->GetErrorList();
            if (!ErrorList.isEmpty()) {
                // Control reaches here means Error list is not empty.
                // Considering only the first element in list since
                // verfier can atmost add only one Hash has to the error list
                m_ErrorMap = *(ErrorList.first());
                SetErrorList(&m_ErrorMap);
            }
        }

        // delete test clone
        delete p_MDL_Verification;

    } else {

        CModuleDataList *p_TempModuleList = new CModuleDataList();
        *p_TempModuleList = *p_ModuleList;

        m_ModuleList.insert(p_TempModuleList->GetModuleTimeStamp(), p_TempModuleList);

        if(!m_ListofModules.contains(p_TempModuleList->GetModuleTimeStamp())) {
            m_ListofModules.append(p_TempModuleList->GetModuleTimeStamp());

            Result = true;
        }
    }

   return Result;
}
/****************************************************************************/
/*!
 *  \brief  Updates ModuleList
 *  \iparam p_ModuleList = ModuleList which needs to be updated
 *  \return true - update success, false - update failed
 */
/****************************************************************************/
bool CInstrumentHistory::UpdateModuleList(CModuleDataList const* p_ModuleList)
{
    CHECKPTR_RETURN(p_ModuleList, false);

    QString TimeStamp = p_ModuleList->GetModuleTimeStamp();
    if (!m_ModuleList.contains(TimeStamp))
    {
        return false;
    }

    bool Result = false;
    if (m_DataVerificationMode) {

        CInstrumentHistory MDL_Verification;

        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            MDL_Verification = *this;

            // disable verification in clone
            MDL_Verification.SetDataVerificationMode(false);

            // execute required action (UpdateModule) in clone
            Result = MDL_Verification.UpdateModuleList(p_ModuleList);

            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(&MDL_Verification);
                if (!Result) {
                    //Store errors.
                    // Since we are going to delete p_DPL_Verification
                    // We need to store the errors generated by it.
                    // For now Errors are not copied by assignment or copy constructors.
                    ListOfErrors_t ErrorList = MDL_Verification.GetErrorList();
                    if (!ErrorList.isEmpty()) {
                        // Control reaches here means Error list is not empty.
                        // Considering only the first element in list since
                        // verfier can atmost add only one Hash has to the error list
                        m_ErrorMap = *(ErrorList.first());
                        SetErrorList(&m_ErrorMap);
                    }
                }
            }

            if (Result) {
                *this = MDL_Verification;
            }

        }
    } else {
        QWriteLocker locker(mp_ReadWriteLock);

        // do a deep copy
        *m_ModuleList.value(TimeStamp) = *p_ModuleList;
        Result = true;
    }

    return Result;
}
/****************************************************************************/
/*!
 *  \brief  Deletes all the programs in the list
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CInstrumentHistory::DeleteAllModuleList()
{
    bool Result = true;
    while (m_ModuleList.size() > 0)
    {
        if (DeleteModuleList(0) == false) {
            Result = false;
        }
    }

    m_ModuleList.clear();
    m_ListofModules.clear();
//    delete mp_ModuleList;

    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Deletes the ModuleList
 *  \iparam Index = Index of the modulelist
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CInstrumentHistory::DeleteModuleList(const unsigned int Index)
{
    if (Index < (unsigned int)m_ListofModules.count()) {
        QString TimeStamp = m_ListofModules.value(Index);
        return DeleteModuleList(TimeStamp);
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the ModuleList
 *  \iparam TimeStamp - Module timestamp
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CInstrumentHistory::DeleteModuleList(const QString TimeStamp)
{
    if(m_ModuleList.contains(TimeStamp)) {
        //get Module from ModuleList and free memory
        delete m_ModuleList.value(TimeStamp);

        //remove pointer to Module from ModuleList
        m_ModuleList.remove(TimeStamp);

        //remove Module from list
        int IndexCount = -1;
        for (int i=0; i<m_ListofModules.count(); i++)
        {
            if (m_ListofModules[i] == TimeStamp) {
                IndexCount = i;
                break;
            }
        }

        // IndexCount MUST never be -1
        Q_ASSERT(IndexCount != -1);
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
 *  \brief Assignment Operator which copies from rhs to lhs.
 *  \iparam ListofModules = CModuleDataList class object
 *  \return CModuleDataList Class Object
 */
/****************************************************************************/
CInstrumentHistory& CInstrumentHistory::operator=(const CInstrumentHistory& ListofModules)
{
    if (this != &ListofModules) {

        QString InstrumentName = const_cast<CInstrumentHistory&>(ListofModules).GetInstrumentName();
        bool DataVerificationMode = const_cast<CInstrumentHistory&>(ListofModules).GetDataVerificationMode();
        QString FileName = const_cast<CInstrumentHistory&>(ListofModules).GetFilename();

        this->SetInstrumentName(InstrumentName);
        this->SetDataVerificationMode(DataVerificationMode);
        this->SetFilename(FileName);

        int Count = const_cast<CInstrumentHistory&>(ListofModules).GetNumberOfModuleLists();

        for(int i=0; i<Count; i++)
        {
            CModuleDataList* ModuleDataList = const_cast<CInstrumentHistory&>(ListofModules).GetModuleList(i);
            (void) this->AddModuleListInfo(ModuleDataList);
        }
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Function to retrieve moduleList for given index
 *  \iparam Index = Index in module list
 *  \return Module associated with the index
 */
/****************************************************************************/
CModuleDataList* CInstrumentHistory::GetModuleList(const unsigned int Index)
{
    if (Index < (unsigned int)m_ListofModules.count()) {
        QString Key = m_ListofModules.value(Index);
        CModuleDataList* p_ModuleList = m_ModuleList.value(Key, NULL);

        return p_ModuleList;
    }
    return NULL;
}

/****************************************************************************/
/*!
 *  \brief  To get module info using module name
 *  \iparam TimeStamp = Module list timestamp
 *  \return Module list information
 */
/****************************************************************************/
CModuleDataList* CInstrumentHistory::GetModuleDataList(const QString TimeStamp)
{
    CModuleDataList* p_ModuleList = m_ModuleList.value(TimeStamp, NULL);
    mp_ModuleList = new DataManager::CModuleDataList();

    mp_ModuleList->SetModuleTimeStamp(p_ModuleList->GetModuleTimeStamp());

    for (int i = 0; i < p_ModuleList->GetNumberofModules(); i++) {
        DataManager::CModule *Module;
        Module = p_ModuleList->GetModule(i);

        if (Module != NULL) {
            (void) mp_ModuleList->AddModule(Module);
        }
    }
    return mp_ModuleList;
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
QDataStream& operator << (const QDataStream& OutDataStream, const CInstrumentHistory& ModuleList)
{
    QDataStream &OutDataStreamRef = const_cast<QDataStream &>(OutDataStream);
    CInstrumentHistory* p_TempModuleList = const_cast<CInstrumentHistory*>(&ModuleList);

    if (!p_TempModuleList->SerializeContent(*OutDataStreamRef.device(), true)) {
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
QDataStream& operator >> (const QDataStream& InDataStream, const CInstrumentHistory& ModuleList)
{
    QDataStream &InDataStreamRef = const_cast<QDataStream &>(InDataStream);
    CInstrumentHistory &ModuleListRef = const_cast<CInstrumentHistory &>(ModuleList);

    if (!ModuleListRef.DeserializeContent(*InDataStreamRef.device(), true)) {
        qDebug() << "CModuleList::Operator Streaming (DeSerializeContent) failed.";
    }

    return InDataStreamRef;
}

}   // end of namespace DataManager
