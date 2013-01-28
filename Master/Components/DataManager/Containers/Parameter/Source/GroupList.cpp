/****************************************************************************/
/*! \file GroupList.cpp
 *
 *  \brief Group List implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-03-01
 *   $Author:  $ Thirumalesha R
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
#include <QReadLocker>
#include <QWriteLocker>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QStringList>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/Parameter/Include/GroupList.h"
#include "Global/Include/EventObject.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CGroupList::CGroupList() :CDataContainerBase(), m_Version(0), m_DataVerificationMode(true), m_Filename(""), mp_ReadWriteLock(NULL)
{
	mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
	Init();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam DeviceParameterList = Instance of the CGroupList class
 *
 *  \return Parameter class object
 */
/****************************************************************************/
CGroupList::CGroupList(const CGroupList& ParameterList) : CDataContainerBase(),
 m_Version(0), m_DataVerificationMode(true), m_Filename(""), mp_ReadWriteLock(NULL)
{
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    Init();
    *this = ParameterList;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CGroupList::~CGroupList()
{
    try {
        (void)DeleteAllGroup(); // to avoid lint 534
    }
    catch(...) {

    }
    if (mp_ReadWriteLock != NULL) {
        try {
            delete mp_ReadWriteLock;
        }
        catch(...) {
            //to please PClint
        }
        mp_ReadWriteLock = NULL;
    }

}

/****************************************************************************/
/*!
 *  \brief Initialize the Parameter  list
 */
/****************************************************************************/
void CGroupList::Init()
{
    QWriteLocker Locker(mp_ReadWriteLock);
	m_Version = 1;
    m_Filename = "";
    if (!DeleteAllGroup()) {
        //throw error ...
        THROWARG(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    }
}
/****************************************************************************/
/*!
 *  \brief Reads an xml file and fill Grouplist object
 *
 *  \iparam Filename = Filename to read
 *
 *  \return true- read success , false - read failed
 */
/****************************************************************************/
bool CGroupList::Read(QString Filename)
{
    //check if file exists
    if (!QFile::exists(Filename)) {
        return false;
    }

    bool Result = true;

    if (m_DataVerificationMode) {
        //To make sure other threads cant write when reading is active
        QWriteLocker locker(mp_ReadWriteLock);

        // create instance of CGroupList for verification
        CGroupList* p_DPL_Verification = new CGroupList();

        // create clone from current state
        *p_DPL_Verification = *this;

        // disable verification in clone
        p_DPL_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        if (!p_DPL_Verification->Read(Filename)) {
            Result = false;
        } else {

            // now check new content => call all active verifiers
            if (DoLocalVerification(p_DPL_Verification)) {
                // if content ok, clone backwards
                *this = *p_DPL_Verification;
                Result = true;
            }
            else {
                Result = false;
            }
        }
        // delete test clone
        delete p_DPL_Verification;
    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);

        // clear content
        Init();

        // Initialise the m_Filename to a known string "UNDEFINED"
        m_Filename = "UNDEFINED";

        QFile File(Filename);
        if (!File.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "open file failed in Read: " << Filename;
            return false;
        }

        if (!DeserializeContent(File, false)) {
            qDebug() << "### CGroupList::Read failed for file during deserializing: " << Filename;
            return false;
        }

        //Now initialize the filename member since Read is succcess
        m_Filename = Filename;

        File.close();

        return true;
    }
    return Result;
}
/****************************************************************************/
/*!
 *  \brief Function to retrieve CGroupList for given index
 *  \iparam Index = Index in CGroupList list
 *
 *  \return Group associated with the index
 */
/****************************************************************************/
CGroup* CGroupList::GetGroup(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedListOfGroupKeys.count()) {
        QString Key = m_OrderedListOfGroupKeys.value(Index);
        CGroup* p_Group = m_GroupList.value(Key, NULL);
        return p_Group;
    }
    return NULL;
}

/****************************************************************************/
/*!
 *  \brief Function to retrieve Group for given index. Group passed is
 *         Updated with the Group at "Index" in the list
 *
 *  \iparam Index = Index in the Group list
 *  \iparam Group = Reference to Group
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CGroupList::GetGroup(const unsigned int Index, CGroup& Group)
{
    QReadLocker locker(mp_ReadWriteLock);

    bool Result = true;
    if (Index < (unsigned int)m_OrderedListOfGroupKeys.count()) {
        QString GroupKey = m_OrderedListOfGroupKeys.value(Index);
        if (m_GroupList.contains(GroupKey)) {
            DataManager::CGroup *p_GroupTemp = m_GroupList.value(GroupKey, NULL);
            if (p_GroupTemp != NULL) {
                Group = *p_GroupTemp;
            } else {
                Q_ASSERT(p_GroupTemp != NULL);
            }
        }
    }
    else {
        return false;
    }
    return Result;
}

//deep copy
/****************************************************************************/
/*!
 *  \brief Function to retrieve Group for given GroupKey. Group passed is
 *         Updated with a Group having GroupKey in the list
 *
 *  \iparam GroupKey = Key of the Group
 *  \iparam Group = Group reference
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CGroupList::GetGroup(const QString GroupKey, CGroup& Group)
{
    QReadLocker locker(mp_ReadWriteLock);

    if (m_GroupList.contains(GroupKey)) {
        Group = *(m_GroupList.value(GroupKey));
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Adds Group step to Group
 *  \iparam Group = Group to add
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
bool CGroupList::AddGroup(const CGroup* p_Group)
{
    if (p_Group == NULL) return false;

    QString Key = const_cast<CGroup*>(p_Group)->GetGroupKey();
    if (m_GroupList.contains(Key)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_ID_NOT_UNIQUE,
                                                   Global::tTranslatableStringList() << p_Group->GetGroupKey());
        m_ErrorHash.insert(EVENT_DM_PROG_ID_NOT_UNIQUE,
                           Global::tTranslatableStringList() << p_Group->GetGroupKey());
        SetErrorList(&m_ErrorHash);
        return false;
    }
    bool Result = true;
   
   //Don't DELETE !!!!! deleting the cloned Group list is suffice
    CGroup *p_TempGroup = new CGroup();
    
    *p_TempGroup = *p_Group;

    Result = true;
    if ((m_DataVerificationMode)) {
        CGroupList* p_DPL_Verification = new CGroupList();

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DPL_Verification = *this;

            // disable verification in clone
            p_DPL_Verification->SetDataVerificationMode(false);

            // execute required action (AddGroup) in clone
            Result = p_DPL_Verification->AddGroup(p_TempGroup);

            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_DPL_Verification);
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_DPL_Verification;
        }
        else {
            //Store errors.
            // Since we are going to delete p_DPL_Verification
            // We need to store the errors generated by it.
            // For now Errors are not copied by assignment or copy constructors.
            ListOfErrors_t ErrorList = p_DPL_Verification->GetErrorList();
            if (!ErrorList.isEmpty()) {
                // Control reaches here means Error list is not empty.
                // Considering only the first element in list since
                // verfier can atmost add only one Hash has to the error list
                m_ErrorHash = *(ErrorList.first());
                SetErrorList(&m_ErrorHash);
            }
        }
        // delete test clone
        delete p_DPL_Verification;

    } else {
        QWriteLocker locker(mp_ReadWriteLock);
        m_GroupList.insert(Key, p_TempGroup);
        m_OrderedListOfGroupKeys.append(Key);
        // White space at begining , end are removed from names. If continuous
        // White space exits between a name, they are replaced by a single white
        //space
        Result = true;
    }

    return Result;
}
/****************************************************************************/
/*!
 *  \brief  Updates Group
 *  \iparam p_Group = Group which needs to be updated
 *  \return true - update success, false - update failed
 */
/****************************************************************************/
bool CGroupList::UpdateGroup(const CGroup* p_Group)
{
    CHECKPTR(p_Group);

    QString Key = p_Group->GetGroupKey();
    if (!m_GroupList.contains(Key)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROGRAM_ID_NOT_FOUND,
                                                   Global::tTranslatableStringList() << p_Group->GetGroupKey());
        m_ErrorHash.insert(EVENT_DM_PROGRAM_ID_NOT_FOUND,
                           Global::tTranslatableStringList() << p_Group->GetGroupKey());
        SetErrorList(&m_ErrorHash);
        // there is no Group with the given GroupKey
        return false;
    }

    bool Result = false;
    if (m_DataVerificationMode) {
        CGroupList* p_DPL_Verification = new CGroupList();
        CGroup const* p_Group = GetGroup(Key);
        CHECKPTR(p_Group);
        
        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DPL_Verification = *this;

            // disable verification in clone
            p_DPL_Verification->SetDataVerificationMode(false);

            // execute required action (UpdateGroup) in clone
            Result = p_DPL_Verification->UpdateGroup(p_Group);
            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_DPL_Verification);
                if (!Result) {
                    //Store errors.
                    // Since we are going to delete p_DPL_Verification
                    // We need to store the errors generated by it.
                    // For now Errors are not copied by assignment or copy constructors.
                    ListOfErrors_t ErrorList = p_DPL_Verification->GetErrorList();
                    if (!ErrorList.isEmpty()) {
                        // Control reaches here means Error list is not empty.
                        // Considering only the first element in list since
                        // verfier can atmost add only one Hash has to the error list
                        m_ErrorHash = *(ErrorList.first());
                        SetErrorList(&m_ErrorHash);
                    }
                }
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_DPL_Verification;
        }

        //Update expanded step list
        p_Group = m_GroupList.value(Key, NULL);
        // delete the cloned objects
        delete p_DPL_Verification;

    } else {
        QWriteLocker locker(mp_ReadWriteLock);

        // do a deep copy
        *m_GroupList.value(Key) = *p_Group;
        Result = true;
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Deletes one progarm
 *  \iparam GroupKey = Group Key associated with the Group to be deleted
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CGroupList::DeleteGroup(const QString GroupKey)
{
    if (m_GroupList.contains(GroupKey)) {
        CGroup *p_Group = m_GroupList.value(GroupKey);
        /* We store Group Name so that we can use it later on for Error reporting */
        QString GroupName = p_Group->GetGroupKey();
        //get Group from GroupList and free memory
        delete p_Group;

        //remove pointer to Group from GroupList
        m_GroupList.remove(GroupKey);

        //remove GroupKey from Key list
        int MatchIndex = -1;
        for (int i=0; i<m_OrderedListOfGroupKeys.count(); i++) {
            if (m_OrderedListOfGroupKeys[i] == GroupKey) {
                MatchIndex = i;
                break;
            }
        }
        // MatchIndex MUST never be -1 !!!
        if (MatchIndex != -1) {
            m_OrderedListOfGroupKeys.removeAt(MatchIndex);
           }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROGRAM_INVALID_INDEX,
                                                       Global::tTranslatableStringList() << GroupName);
            m_ErrorHash.insert(EVENT_DM_PROGRAM_INVALID_INDEX,
                               Global::tTranslatableStringList() << GroupName);
            return false;
        }

        return true;
    } else {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_DELETE_FAILED_ID_NOT_FOUND,
                                                   Global::tTranslatableStringList() << GroupKey);
        m_ErrorHash.insert(EVENT_DM_PROG_DELETE_FAILED_ID_NOT_FOUND,
                           Global::tTranslatableStringList() << GroupKey);
        // there is no Group with the given GroupKey
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the progarm
 *  \iparam Index
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CGroupList::DeleteGroup(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedListOfGroupKeys.count()) {
        QString GroupKey = m_OrderedListOfGroupKeys.value(Index);
        return DeleteGroup(GroupKey);
    } else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes all the Groups in the list
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CGroupList::DeleteAllGroup()
{
    bool Result = true;

    while (m_GroupList.size() > 0) {
        if (DeleteGroup(0) == false) {
            Result = false;
        }
    }

    m_GroupList.clear();
   return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads all the Groups from the xml stream
 *
 *  \iparam XmlStreamReader = Reference to xml stream to read from
 *  \iparam CompleteData = true to read Complete data of object
 *
 *  \return true - read success or false - read failure
 */
/****************************************************************************/
bool CGroupList::ReadAllGroups(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    bool Result = true;
    // Look for node <Group>
    while(!XmlStreamReader.atEnd()) {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "Group") {

                DataManager::CGroup Group;

                if (!Group.DeserializeContent(XmlStreamReader, CompleteData)) {
                    qDebug() << "CGroup Deserialize failed!";
                    return false;
                }
                // Now add this Reagent
                if (!AddGroup(&Group)) {
                    qDebug() << "CGroupList::Add Group failed!";
                    return false;
                }
            }
            else {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "GroupList") {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this Reagent";
            break; // exit from while loop
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Writes from the CGroupList object to a IODevice.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CGroupList::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    QString StringValue;
    bool Result = true;
    QXmlStreamWriter XmlStreamWriter;

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    XmlStreamWriter.writeDTD("<!DOCTYPE process_settings_layout>");
    XmlStreamWriter.writeStartElement("process_settings");

    // write attribute version
    (void)StringValue.setNum(GetVersion()); //to suppress lint-534
    XmlStreamWriter.writeAttribute("Version", StringValue);

    // write all Groups
    for (int i = 0; i < GetNumberofGroups(); i++) {
        CGroup *p_Group = GetGroup(i);
        if (p_Group != NULL) {
            if (!p_Group->SerializeContent(XmlStreamWriter, CompleteData)) {
                qDebug("DataManager::CGroup SerializeContent failed ");
                return false;
            }
        }
    }

    if (CompleteData) {
        XmlStreamWriter.writeStartElement("NonXmlData");
        if (GetDataVerificationMode()) {
            XmlStreamWriter.writeAttribute("VerificationMode", "true");
        }
        else {
            XmlStreamWriter.writeAttribute("VerificationMode", "false");
        }

        XmlStreamWriter.writeAttribute("FileName", GetFilename());

        XmlStreamWriter.writeEndElement();//NonXmlData
    }
    XmlStreamWriter.writeEndElement(); // Parameter
    XmlStreamWriter.writeEndDocument(); // End of Document

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CGroupList object.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CGroupList::DeserializeContent(QIODevice& IODevice ,bool CompleteData)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(&IODevice);
    bool Result = true;
    // look for node <process_settings>
    if (!Helper::ReadNode(XmlStreamReader, "process_settings")) {
        qDebug()<<"process_settings NodeName:"<<XmlStreamReader.name()<<"NodeName LineNumber"<<XmlStreamReader.lineNumber();
        qDebug() << "DeserializeContent: abort reading. Node not found: process_settings";
        return false;
    }
    // Read attribute Version
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        return false;
    }

    SetVersion(XmlStreamReader.attributes().value("Version").toString().toInt());
    
    if (!DeleteAllGroup()) {
        //throw error ...
        THROWARG(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    }
	
    Result = ReadAllGroups(XmlStreamReader, CompleteData);
    
	if (CompleteData) {
        // look for node 
        if (!Helper::ReadNode(XmlStreamReader, "ClassTemporaryData")) {
            qDebug() << "DeserializeContent: abort reading. Node not found: ClassTemporaryData";
            return false;
        }
        // File name
        if (!XmlStreamReader.attributes().hasAttribute("FileName")) {
            qDebug() << "### attribute <FileName> is missing => abort reading";
            return false;
        }
        m_Filename = XmlStreamReader.attributes().value("FileName").toString();

        // VerificationMode
        if (!XmlStreamReader.attributes().hasAttribute("VerificationMode")) {
            qDebug() << "### attribute <VerificationMode> is missing => abort reading";
            return false;
        } else {
            bool Value = false;
            if (XmlStreamReader.attributes().value("VerificationMode").toString().toUpper() == "TRUE") {
                Value = true;
            }
            SetDataVerificationMode(Value);
        }
           } // Reading of temporary Class Data completed
    //======NODE=END====Temporary Data Variables=========================
    return Result;
}

///****************************************************************************/
///*!
// *  \brief Reads the data from the XML stream, the data read is not part of the
// *         XML file;e.g. Flags
// *  \iparam XmlStreamReader = XML stream
// *
// *  \return true - read success , false - read failed
// */
///****************************************************************************/
//bool CGroupList::ReadCompleteData(QXmlStreamReader& XmlStreamReader)
//{
//    if (!XmlStreamReader.attributes().hasAttribute("VerificationMode")) {
//        qDebug() << "### attribute <VerificationMode> is missing";
//        return false;
//    }

//    if (XmlStreamReader.attributes().value("VerificationMode").toString() == "true") {
//        SetDataVerificationMode(true);
//    }
//    else {
//        SetDataVerificationMode(false);
//    }

//    if (!XmlStreamReader.attributes().hasAttribute("FileName")) {
//        qDebug() << "### attribute <FileName> is missing";
//        return false;
//    }

//    SetFilename(XmlStreamReader.attributes().value("FileName").toString());

//    return true;
//}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam GroupList = CGroupList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CGroupList& GroupList) {

    CGroupList* p_TempGroupList = const_cast<CGroupList*>(&GroupList);

    if (!p_TempGroupList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CParameter::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE );
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam GroupList = CGroupList class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CGroupList& GroupList) {

    if (!GroupList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CParameter::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceGroupList = CGroupList class object
 *
 *  \return CParameter Class Object
 */
/****************************************************************************/
CGroupList& CGroupList::operator = (const CGroupList& SourceGroupList) {

    // make sure not same object
    if (this != &SourceGroupList)
    {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        DataStream << SourceGroupList;
        (void)DataStream.device()->reset(); //to avoid lint-534
        bool VerificationModeLocal = GetDataVerificationMode();
        if (VerificationModeLocal) {
            SetDataVerificationMode(false);
        }
        DataStream >> *this;
        //Set back verification mode to original state
        SetDataVerificationMode(VerificationModeLocal);
    }
    return *this;
}


}//End of namespace DataManager
