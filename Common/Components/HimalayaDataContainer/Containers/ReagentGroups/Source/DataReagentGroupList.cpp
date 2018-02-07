/****************************************************************************/
/*! \file DataReagentGroupList.cpp
 *
 *  \brief ReagentGroup  implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-30
 *   $Author:  $ Swati Tiwari
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


#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "Global/Include/EventObject.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"
#include "Global/Include/UITranslator.h"
#include <QDataStream>

//lint -sem(QHash::insert, custodial(1))

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CDataReagentGroupList::CDataReagentGroupList() : m_Version(0), m_DataVerificationMode(true), m_Filename(""), mp_ReadWriteLock(NULL)
{
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ReagentGroupList = Instance of the CReagentGroup class
 *
 *  \return ReagentGroup class object
 */
/****************************************************************************/
CDataReagentGroupList::CDataReagentGroupList(const CDataReagentGroupList& ReagentGroupList) : CDataContainerBase()
{
    CDataReagentGroupList* p_TempReagentGroupList = const_cast<CDataReagentGroupList*>(&ReagentGroupList);

    SetDefaultAttributes();
    m_DataVerificationMode = true;
    *this = *p_TempReagentGroupList;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDataReagentGroupList::~CDataReagentGroupList()
{
    try {
        (void)DeleteAllReagentGroups(); // to avoid lint 534
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
 *  \brief Defaults to the initial state of Attributes of CDataReagentList
 */
/****************************************************************************/
void CDataReagentGroupList::SetDefaultAttributes()
{
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    //    m_VerifierList.clear();
    Init();
}
/****************************************************************************/
/*!
 *  \brief Initialize the Program Sequence list
 */
/****************************************************************************/
void CDataReagentGroupList::Init()
{
    m_Version = 1;
    m_Filename = "";
    (void)DeleteAllReagentGroups(); // to suppress lint-534
}


/****************************************************************************/
/*!
 *  \brief Writes from the CDataReagentGroupList object to a IODevice.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentGroupList::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    QString StringValue;
    bool Result = true;
    QXmlStreamWriter XmlStreamWriter;

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);

    XmlStreamWriter.writeDTD("<!DOCTYPE ReagentGroups>");
    XmlStreamWriter.writeStartElement("ReagentGroups");

    // write attribute version
    (void)StringValue.setNum(GetVersion()); //to suppress lint-534
    XmlStreamWriter.writeAttribute("Version", StringValue);

    for(int i=0;i<GetNumberOfReagentGroups();i++)
    {
        CReagentGroup *p_ReagentGroup= const_cast<CReagentGroup*> (GetReagentGroup(i));
        if (p_ReagentGroup != NULL) {
            if(!p_ReagentGroup->SerializeContent(XmlStreamWriter,CompleteData)){
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
    XmlStreamWriter.writeEndElement(); // ReagentGroup
    XmlStreamWriter.writeEndDocument(); // End of Document

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CDataReagentGroupList object.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentGroupList::DeserializeContent(QIODevice& IODevice ,bool CompleteData)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(&IODevice);

    // look for node <ReagentGroup>
    if (!Helper::ReadNode(XmlStreamReader, "ReagentGroups")) {
        qDebug()<<"ReagentGroup NodeName:"<<XmlStreamReader.name()<<"NodeName LineNumber"<<XmlStreamReader.lineNumber();
        qDebug() << "DeserializeContent: abort reading. Node not found: ReagentGroup";
        return false;
    }
    // Read attribute Version
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
//        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
//                                                   Global::tTranslatableStringList() << "ReagentGroupList:Version", true);
        return false;
    }
    SetVersion(XmlStreamReader.attributes().value("Version").toString().toInt());
    //  Now Delete the Program list and ordered list of ReagentGroup ID's
    if(!DeleteAllReagentGroups()) {
        qDebug() << "DeserializeContent: abort reading. Deleting all the program sequence failed";
        return false;
    }

    while (!XmlStreamReader.atEnd()) {
        if (XmlStreamReader.readNext() == QXmlStreamReader::Invalid) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }
        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "ReagentGroup") {
                CReagentGroup ReagentGroup;
                if (!ReagentGroup.DeserializeContent(XmlStreamReader, CompleteData)) {
                    return false;
                }
                // Now add this ReagentGroup
                if (!AddReagentGroup(&ReagentGroup)) {
                    qDebug() << "CDataReagentGroupList::Add ReagentGroup failed!";
                    return false;
                }
            }
            else if (CompleteData) {
                if (XmlStreamReader.name() == "NonXmlData") {
                    if (!ReadCompleteData(XmlStreamReader)) {
                        return false;
                    }
                }
            }
            else {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "ReagentGroups") {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this Reagent";
            break; // exit from while loop
        }
    } // end of while loop


    //======NODE=END====Temporary Data Variables=========================
    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the data from the XML stream, the data read is not part of the
 *         XML file;e.g. Flags
 *  \iparam XmlStreamReader = XML stream
 *
 *  \return true - read success , false - read failed
 */
/****************************************************************************/
bool CDataReagentGroupList::ReadCompleteData(QXmlStreamReader& XmlStreamReader)
{
    if (!XmlStreamReader.attributes().hasAttribute("VerificationMode")) {
        qDebug() << "### attribute <VerificationMode> is missing";
//        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
//                                                   Global::tTranslatableStringList() << "ReagentGroupList:VerificationMode", true);
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

    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ReagentGroupList = CDataReagentGroupList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CDataReagentGroupList& ReagentGroupList) {

    CDataReagentGroupList* p_TempReagentGroupList = const_cast<CDataReagentGroupList*>(&ReagentGroupList);

    if (!p_TempReagentGroupList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CReagentGroup::Operator Streaming (SerializeContent) failed.";

        // raise an event and insert to the error list
        p_TempReagentGroupList->m_ErrorHash.insert(EVENT_DM_STREAMOUT_FAILED,
                                              Global::tTranslatableStringList() << "ReagentGroupList");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentGroupList", true);
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam ReagentGroupList = CDataReagentGroupList class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CDataReagentGroupList& ReagentGroupList) {

    if (!ReagentGroupList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CReagentGroup::Operator Streaming (DeSerializeContent) failed.";
        // raise an event and insert to the error list
        ReagentGroupList.m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED,
                                       Global::tTranslatableStringList() << "ReagentGroupList");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentGroupList", true);
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceReagentGroupList = CDataReagentGroupList class object
 *
 *  \return CReagentGroup Class Object
 */
/****************************************************************************/
CDataReagentGroupList& CDataReagentGroupList::operator = (const CDataReagentGroupList& SourceReagentGroupList) {

    // make sure not same object
    if (this != &SourceReagentGroupList)
    {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        DataStream << SourceReagentGroupList;
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

/****************************************************************************/
/*!
 *  \brief Reads the data and stores in CDataReagentGroupList from the file
 *
 *  \iparam Filename = Name of the Input File
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentGroupList::Read(QString Filename)
{
    //check if file exists
    if (!QFile::exists(Filename)) {
        qDebug("File doesn't exists");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_FILE_NOT_EXISTS,
                                                   Global::tTranslatableStringList() << Filename, true);
        return false;
    }    
    bool Result = true;

    if (m_DataVerificationMode) {
        QWriteLocker locker(mp_ReadWriteLock);

        // create instance of CDataReagentGroupList for verification
        CDataReagentGroupList* p_DPSL_Verification = new CDataReagentGroupList();

        // disable verification in clone
        p_DPSL_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        if (!p_DPSL_Verification->Read(Filename)) {
            Result = false;
        }
        else {
            // now check new content => call all active verifiers
            if (DoLocalVerification(p_DPSL_Verification)) {
                // if content ok, clone backwards
            }
            else {
                Result = false;
            }
        }
        // delete test clone
        delete p_DPSL_Verification;

    } else {
        QWriteLocker locker(mp_ReadWriteLock);

        // clear content
        Init();

        m_Filename = "UNDEFINED";
        QFile File (Filename);
        if (!File.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "open file failed in Read: " << Filename;
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_FILE_OPEN_FAILED,
                                                       Global::tTranslatableStringList() << Filename, true);
            return false;
        }
        if (!DeserializeContent(File, false)) {
            qDebug() << "### CDataReagentGroupList::Read failed for file: " << Filename;
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_DESERIALIZE_FAILED,
                                                       Global::tTranslatableStringList() << Filename, true);
            return false;
        }        
        File.close();

        //Now initialize the filename member since Read is succcess
        m_Filename = Filename;

        Result = true;
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Get program Sequence Object
 *
 *  \iparam Index
 *
 *  \return  m_ReagentGroupsList or NULL if not found
 */
/****************************************************************************/
CReagentGroup* CDataReagentGroupList::GetReagentGroup(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedListOfReagentGroupIDs.count()) {
        QString Key = m_OrderedListOfReagentGroupIDs.value(Index);
        CReagentGroup* p_ReagentGroup = m_ReagentGroupsList.value(Key, NULL);
        return p_ReagentGroup;
    }
    else {
        return NULL;
    }
}
/****************************************************************************/
/*!
 *  \brief Get program Sequence Object
 *
 *  \iparam Index - Index of the program sequence list
 *  \iparam ReagentGroup - Program sequence class
 *
 *  \return  True-success else false
 */
/****************************************************************************/
//deep copy
bool CDataReagentGroupList::GetReagentGroup(const unsigned int Index, CReagentGroup& ReagentGroup)
{
    QReadLocker locker(mp_ReadWriteLock);

    if (Index < (unsigned int)m_OrderedListOfReagentGroupIDs.count()) {
        QString ReagentGroupID = m_OrderedListOfReagentGroupIDs.value(Index);
        if (m_ReagentGroupsList.contains(ReagentGroupID)) {
            ReagentGroup = *m_ReagentGroupsList.value(ReagentGroupID); // this gives the exception if not copied
            return true;
        }
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief Deep Copies the ReagentGroup content ReagentGroup Instance by ReagentGroup ID
 *
 *  \iparam ID = ID of the ReagentGroup
 *  \iparam ReagentGroup =  CReagentGroup Instance
 *
 *  \return True or False
 */
/****************************************************************************/
//deep copy
bool CDataReagentGroupList::GetReagentGroup(const QString ID, CReagentGroup& ReagentGroup)
{
    QReadLocker locker(mp_ReadWriteLock);

    if (m_ReagentGroupsList.contains(ID)) {
        ReagentGroup = *m_ReagentGroupsList.value(ID);
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Add program Sequence object to the list
 *  \iparam p_ReagentGroup
 *
 *  \return true -Add success else false
 */
/****************************************************************************/
bool CDataReagentGroupList::AddReagentGroup(const CReagentGroup* p_ReagentGroup)
{
    try {
        CHECKPTR(p_ReagentGroup);
    }
    catch(Global::Exception &E) {
        Global::EventObject::Instance().RaiseEvent(E);
        return false;
    }

    QString  ID = const_cast<CReagentGroup*>(p_ReagentGroup)->GetGroupID();
    if (m_ReagentGroupsList.contains(ID)) {
        return false;
    }

    //Don't DELETE !!!!! deleting the cloned program list is suffice
    CReagentGroup *p_TempReagentGroup = new CReagentGroup();
    *p_TempReagentGroup = *p_ReagentGroup;
    bool Result = true;
    if ((m_DataVerificationMode)) {
        CDataReagentGroupList* p_DSPL_Verification = new CDataReagentGroupList();

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DSPL_Verification = *this;

            // disable verification in clone
            p_DSPL_Verification->SetDataVerificationMode(false);

            // execute required action (AddReagentGroup) in clone
            Result = p_DSPL_Verification->AddReagentGroup(p_TempReagentGroup);
            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_DSPL_Verification);
                if (!Result) {
                    //Store errors.
                    // Since we are going to delete p_DSPL_Verification
                    // We need to store the errors generated by it.
                    // For now Errors are not copied by assignment or copy constructors.
                    ListOfErrors_t ErrorList = p_DSPL_Verification->GetErrorList();
                    if (!ErrorList.isEmpty()) {
                        // If the control reaches here means Error hash is empty
                        // Considering only the first element in Hash since
                        // verfier can atmost add only one Hash has to the error list
                        m_ErrorHash = *(ErrorList.first());
                        SetErrorList(&m_ErrorHash);
                    }
                }
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_DSPL_Verification;
            Result = true;
        }

        // delete test clone
        delete p_DSPL_Verification;

    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);
        m_ReagentGroupsList.insert(ID, p_TempReagentGroup);
        m_OrderedListOfReagentGroupIDs.append(ID);
        Result = true;
    }
    /*lint -e593 */
    return Result;
}


/****************************************************************************/
/*!
 *  \brief Update ReagentGroup
 *  \iparam p_ReagentGroup
 *
 *  \return true -Update success else false
 */
/****************************************************************************/
bool CDataReagentGroupList::UpdateReagentGroup(const CReagentGroup* p_ReagentGroup)
{
    CHECKPTR(p_ReagentGroup);

    QString ID = const_cast<CReagentGroup*>(p_ReagentGroup)->GetGroupID();
    if (!m_ReagentGroupsList.contains(ID)) {
        // there is no ReagentGroup with the given ReagentGroupID
        m_ErrorHash.insert(EVENT_DM_REAGENT_ID_DOESTNOT_EXISTS,
                           Global::tTranslatableStringList() << ID);
        SetErrorList(&m_ErrorHash);
        return false;
    }

    bool Result = false;
    if (m_DataVerificationMode) {
        CDataReagentGroupList* p_DRL_Verification = new CDataReagentGroupList();

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DRL_Verification = *this;

            // disable verification in clone
            p_DRL_Verification->SetDataVerificationMode(false);

            // execute required action (UpdateReagentGroup) in clone
            Result = p_DRL_Verification->UpdateReagentGroup(p_ReagentGroup);
            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_DRL_Verification);
                if (!Result) {
                    //Store errors.
                    // Since we are going to delete p_DRL_Verification
                    // We need to store the errors generated by it.
                    // For now Errors are not copied by assignment or copy constructors.
                    ListOfErrors_t ErrorList = p_DRL_Verification->GetErrorList();
                    if (!ErrorList.isEmpty()) {
                        // If the control reaches here means Error hash is empty
                        // Considering only the first element in Hash since
                        // verfier can atmost add only one Hash has to the error list
                        m_ErrorHash = *(ErrorList.first());
                        SetErrorList(&m_ErrorHash);
                    }
                }
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_DRL_Verification;
            Result = true;
        }

        // delete test clone
        delete p_DRL_Verification;


    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);
        // deep copy is happening for the program sequence
        *m_ReagentGroupsList[ID] = *p_ReagentGroup;
        Result = true;
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Delete Program Sequence
 *  \iparam ID
 *
 *  \return true -Delete success else false
 */
/****************************************************************************/
bool CDataReagentGroupList::DeleteReagentGroup(const QString ID)
{
    try {
        if (m_ReagentGroupsList.contains(ID)) {
            //get Program from List and free memory
            delete m_ReagentGroupsList.value(ID);

            //remove pointer to Program from List
            m_ReagentGroupsList.remove(ID);
           // qDebug("Program deleted");

            //remove ID from ID list
            int MatchIndex = -1;
            for (int i=0; i<m_OrderedListOfReagentGroupIDs.count(); i++) {
                if (m_OrderedListOfReagentGroupIDs[i] == ID) {
                    MatchIndex = i;
                    break;
                }
            }
            // MatchIndex MUST never be -1 !!!
            Q_ASSERT(MatchIndex != -1);
            m_OrderedListOfReagentGroupIDs.removeAt(MatchIndex);

            return true;
        }
        else {
            return false;
        }
    }
    catch (...) {
        qDebug() << "### exception in CDataReagentGroupList::DeleteReagentGroup";
        m_ErrorHash.insert(EVENT_DM_REAGENT_OPERATION_FAILED,
                           Global::tTranslatableStringList()<<"DeleteReagentGroup");
        SetErrorList(&m_ErrorHash);
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Delete Program Sequence  based on Index
 *  \iparam Index
 *
 *  \return true -Delete succes else false
 */
/****************************************************************************/
bool CDataReagentGroupList::DeleteReagentGroup(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedListOfReagentGroupIDs.count()) {
        QString ID = m_OrderedListOfReagentGroupIDs.value(Index);
        return DeleteReagentGroup(ID);
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Delete all program sequence s
 *
 *  \return true- Delete success else false
 */
/****************************************************************************/
bool CDataReagentGroupList::DeleteAllReagentGroups()
{
    bool Result = true;

    while (m_ReagentGroupsList.size() > 0) {
        if (DeleteReagentGroup(0) == false) {
            Result = false;
        }
    }
    m_ReagentGroupsList.clear();
    m_OrderedListOfReagentGroupIDs.clear();

    return Result;
}


void CDataReagentGroupList::UpdateOnLanguageChanged()
{
//   QWriteLocker Locker(mp_ReadWriteLock);
//   for (qint32 I = 0; I < m_ReagentGroupsList.count(); I++) {
//       CReagentGroup *p_ReagentGroup = GetReagentGroup(I);
//       if(p_ReagentGroup && !p_ReagentGroup->GetGroupNameID().isEmpty()){
//           bool ok = false;
//           quint32 strid = p_ReagentGroup->GetGroupNameID().toUInt(&ok);
//           if(ok && strid > 0)
//           {
//               p_ReagentGroup->SetReagentGroupName(Global::UITranslator::TranslatorInstance().Translate(
//                                                       Global::TranslatableString(strid)));
//           }
//       }
//   }
//   (void)Write();
}

}//End of namespace DataManager
