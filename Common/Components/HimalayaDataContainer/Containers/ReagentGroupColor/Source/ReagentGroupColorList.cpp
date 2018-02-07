
/****************************************************************************/
/*! \file ReagentGroupColorList.cpp
 *
 *  \brief ReagentGroupColorList Implementation.
 *
 *   $Version: $ 0.3
 *   $Date:    $2013-03-04
 *   $Author:  $ Swati Tiwari
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/



#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColorList.h"
#include "HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColor.h"
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <QDataStream>

//lint -sem(QHash::insert, custodial(1))

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 *
 */
/****************************************************************************/
CReagentGroupColorList::CReagentGroupColorList() :m_Version(0), m_DataVerificationMode(true), m_Filename(""), mp_ReadWriteLock(NULL)
{
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ReagentGroupColorList = Instance of the CReagentGroupColorList class
 *
 *  \return
 */
/****************************************************************************/
CReagentGroupColorList::CReagentGroupColorList(const CReagentGroupColorList& ReagentGroupColorList) : CDataContainerBase()
{
    CReagentGroupColorList* p_TempReagentList = const_cast<CReagentGroupColorList*>(&ReagentGroupColorList);

    SetDefaultAttributes();
    m_DataVerificationMode = true;
    //CDataContainerBase::SetDataVerificationMode(true);

    *this = *p_TempReagentList;
}
/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/

CReagentGroupColorList::~CReagentGroupColorList()
{
    try {
        (void)DeleteAllReagentsGroupColor(); // to avoid lint-534
    }
    catch(...) {
        //to please PClint
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
 *  \brief Initilisation function for CReagentGroupColorList
 *
 *  \return void
 */
/****************************************************************************/
void CReagentGroupColorList::Init()
{
    m_Version = 0;
    m_Filename = "";

    if (!DeleteAllReagentsGroupColor()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_REAGENT_OPERATION_FAILED,
                                                   Global::tTranslatableStringList() << "CReagentGroupColorList::Init()", true);
    }

}

/****************************************************************************/
/*!
 *  \brief Defaults to the initial state of Attributes of CReagentGroupColorList
 *
 *  \return void
 */
/****************************************************************************/

void CReagentGroupColorList::SetDefaultAttributes()
{
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    //    m_VerifierList.clear();
    // fill heating start mode hash
//    HashOfHeatingStartModes.insert("ATDEVICESTART", AT_DEVICE_START);
//    HashOfHeatingStartModes.insert("ATPROGRAMSTART", AT_PROGRAM_START);
    Init();
}

/****************************************************************************/
/*!
 *  \brief Reads the CReagentGroupColorList Data from QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CReagentGroupColorList::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(&IODevice);

    // look for node <ReagentGroup>
    if (!Helper::ReadNode(XmlStreamReader, "ReagentGroupColors")) {
        qDebug()<<"ReagentGroup NodeName:"<<XmlStreamReader.name()<<"NodeName LineNumber"<<XmlStreamReader.lineNumber();
        qDebug() << "DeserializeContent: abort reading. Node not found: ReagentGroup";
        return false;
    }


    //  Now Delete the Program list and ordered list of ReagentGroup ID's
    if(!DeleteAllReagentsGroupColor()) {
        qDebug() << "DeserializeContent: abort reading. Deleting all the program sequence failed";
        return false;
    }

    while (!XmlStreamReader.atEnd()) {
        if (XmlStreamReader.readNext() == QXmlStreamReader::Invalid) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }
        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "Color") {
                CReagentGroupColor ReagentGroupColor;
                if (!ReagentGroupColor.DeserializeContent(XmlStreamReader, CompleteData)) {
                    return false;
                }
                // Now add this ReagentGroup
                if (!AddReagentColorGroup(&ReagentGroupColor)) {
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
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "ReagentGroupColors") {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this Reagent";
            break; // exit from while loop
        }
    } // end of while loop


    //======NODE=END====Temporary Data Variables=========================
    return true;
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
bool CReagentGroupColorList::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    bool Result = true;
    QXmlStreamWriter XmlStreamWriter;

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);

    XmlStreamWriter.writeDTD("<!DOCTYPE ReagentGroupColors>");
    XmlStreamWriter.writeStartElement("ReagentGroupColors");

    // write attribute version
   // (void)StringValue.setNum(GetVersion()); //to suppress lint-534
   // XmlStreamWriter.writeAttribute("Version", StringValue);

    for(int i=0;i<GetNumberOfReagentGroupColor();i++)
    {
        CReagentGroupColor *p_ReagentGroupColor= const_cast<CReagentGroupColor*> (GetCReagentGroupColor(i));

        if (p_ReagentGroupColor != NULL) {
            if(!p_ReagentGroupColor->SerializeContent(XmlStreamWriter,CompleteData)){
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
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ReagentList = CReagentGroupColorList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CReagentGroupColorList& ReagentList)
{
    CReagentGroupColorList* p_TempReagentList = const_cast<CReagentGroupColorList*>(&ReagentList);

    if (!p_TempReagentList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CReagent::Operator Streaming (SerializeContent) failed.";
        // raise an event and insert to the error list
        p_TempReagentList->m_ErrorHash.insert(EVENT_DM_STREAMOUT_FAILED,
                                              Global::tTranslatableStringList() << "ReagentList");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentList", true);
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam ReagentList = CReagentGroupColorList class object
 *
 *  \return Input Stream
 */
/****************************************************************************/

QDataStream& operator >>(QDataStream& InDataStream, CReagentGroupColorList& ReagentList)
{
    if (!ReagentList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CReagent::Operator Streaming (DeSerializeContent) failed.";
        // raise an event and insert to the error list
        ReagentList.m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED,
                                       Global::tTranslatableStringList() << "ReagentList");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentList", true);
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceReagentList = CReagentGroupColorList class object
 *
 *  \return CReagent Class Object
 */
/****************************************************************************/

CReagentGroupColorList& CReagentGroupColorList::operator=(const CReagentGroupColorList& SourceReagentList)
{
    // make sure not same object
    if (this != &SourceReagentList)
    {
        QByteArray TempByteArray;
        CReagentGroupColorList* p_TempReagentList = const_cast<CReagentGroupColorList*>(&SourceReagentList);
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        {
            QReadLocker locker(mp_ReadWriteLock);
            DataStream << *p_TempReagentList;
        }
        (void)DataStream.device()->reset(); //to avoid lint-534
        bool TempVerificationMode = GetDataVerificationMode();
        if (TempVerificationMode) {
            SetDataVerificationMode(false);
        }
       /* {
            QWriteLocker locker(mp_ReadWriteLock);
            DataStream >> *this;
        }*/
        if (TempVerificationMode) {
            SetDataVerificationMode(true);
        }
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Reads the data and stores in CReagentGroupColorList from the file
 *
 *  \iparam Filename = Name of the Input File
 *
 *  \return True or False
 */
/****************************************************************************/
bool CReagentGroupColorList::Read(QString Filename)
{
    //check if file exists
    if (!QFile::exists(Filename)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_FILE_NOT_EXISTS,
                                                   Global::tTranslatableStringList() << Filename, true);
        return false;
    }

    bool Result = true;

    if (m_DataVerificationMode) {
        ErrorMap_t ErrorHash;
        QWriteLocker locker(mp_ReadWriteLock);

        // create instance of CReagentGroupColorList for verification
        CReagentGroupColorList* p_DRL_Verification = new CReagentGroupColorList();

        // create clone from current state
        *p_DRL_Verification = *this;

        // disable verification in clone
        p_DRL_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        if (!p_DRL_Verification->Read(Filename)) {
            Result = false;
        }
        else {
            // now check new content => call all active verifiers
            if (DoLocalVerification(p_DRL_Verification)) {
                // if content ok, clone backwards
                *this = *p_DRL_Verification;
                Result = true;
            }
            else {
                Result = false;
            }
        }
        // delete test clone
        delete p_DRL_Verification;

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
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_FILE_OPEN_FAILED,
                                                       Global::tTranslatableStringList() << Filename, true);
            return false;
        }

        if (!DeserializeContent(File, false)) {
            qDebug() << "### CReagentGroupColorList::Read failed for file: " << Filename;
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_DESERIALIZE_FAILED,
                                                       Global::tTranslatableStringList() << Filename, true);
            return false;
        }

        //After when the Reading is done an dcopied to Object
        // Copy the filename to the member attribute which can be used while writing
        m_Filename = Filename;

        File.close();
        //return true;
        Result = true;
    }
    return Result;
}


/****************************************************************************/
/*!
 *  \brief Add program Sequence object to the list
 *  \iparam p_ReagentGroupColor
 *
 *  \return true -Add success else false
 */
/****************************************************************************/
bool CReagentGroupColorList::AddReagentColorGroup(const CReagentGroupColor* p_ReagentGroupColor)
{
    try {
        CHECKPTR(p_ReagentGroupColor);
    }
    catch(Global::Exception &E) {
        Global::EventObject::Instance().RaiseEvent(E);
        return false;
    }

    QString  ID = const_cast<CReagentGroupColor*>(p_ReagentGroupColor)->GetColorID();
    if (m_ReagentGroupColorList.contains(ID)) {

        return false;
    }

    //Don't DELETE !!!!! deleting the cloned program list is suffice
    CReagentGroupColor *p_TempReagentGroupColor = new CReagentGroupColor();
    *p_TempReagentGroupColor = *p_ReagentGroupColor;
  bool Result = true;

    m_DataVerificationMode = false;
    if ((m_DataVerificationMode)) {
        CReagentGroupColorList* p_DSPL_Verification = new CReagentGroupColorList();

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DSPL_Verification = *this;

            // disable verification in clone
            p_DSPL_Verification->SetDataVerificationMode(false);

            // execute required action (AddReagentGroup) in clone
            Result = p_DSPL_Verification->AddReagentColorGroup(p_TempReagentGroupColor);
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
        m_ReagentGroupColorList.insert(ID, p_TempReagentGroupColor);
        m_OrderedListOfReagentGroupColorIDs.append(ID);
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
bool CReagentGroupColorList::DeleteReagentGroupColor(const QString ID)
{
    try {
        if (m_ReagentGroupColorList.contains(ID)) {
            //get Program from List and free memory
            delete m_ReagentGroupColorList.value(ID);

            //remove pointer to Program from List
            m_ReagentGroupColorList.remove(ID);
           // qDebug("Program deleted");

            //remove ID from ID list
            int MatchIndex = -1;
            for (int i=0; i<m_OrderedListOfReagentGroupColorIDs.count(); i++) {
                if (m_OrderedListOfReagentGroupColorIDs[i] == ID) {
                    MatchIndex = i;
                    break;
                }
            }
            // MatchIndex MUST never be -1 !!!
            Q_ASSERT(MatchIndex != -1);
            m_OrderedListOfReagentGroupColorIDs.removeAt(MatchIndex);

            return true;
        }
        else {
            // there is no program  with the given ID
//            Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROGSEQ_ID_DOESTNOT_EXISTS,
//                                                       Global::tTranslatableStringList() << ID,
//                                                       true, Global::GUI_MSG_BOX);
            return false;
        }
    }
    catch (...) {
        qDebug() << "### exception in CDataReagentGroupList::DeleteReagentGroup";
        m_ErrorHash.insert(EVENT_DM_REAGENT_OPERATION_FAILED,
                           Global::tTranslatableStringList() << "DeleteReagentGroup");
        SetErrorList(&m_ErrorHash);
        return false;
    }
}
/****************************************************************************/
/*!
 *  \brief Deletes the existing reagent from the list
 *
 *  \iparam Index =  Required Index number in the list
 *
 *  \return True or False
 */
/****************************************************************************/
bool CReagentGroupColorList::DeleteReagentGroupColor(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedListOfReagentGroupColorIDs.count()) {
        QString ReagentGroupColorID = m_OrderedListOfReagentGroupColorIDs.value(Index);
        return DeleteReagentGroupColor(ReagentGroupColorID);
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Deletes  all the existing reagents from the list
 *
 *  \return True or False
 */
/****************************************************************************/
bool CReagentGroupColorList::DeleteAllReagentsGroupColor()
{
    while (m_ReagentGroupColorList.size() > 0) {
        if (DeleteReagentGroupColor(0) == false) {
            return false;
        }
    }

    m_ReagentGroupColorList.clear();
    m_OrderedListOfReagentGroupColorIDs.clear();

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

bool CReagentGroupColorList::ReadCompleteData(QXmlStreamReader& XmlStreamReader)
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
 *  \brief Get program Sequence Object
 *
 *  \iparam Index - Index of the program sequence list
 *
 *  \return  True-success else false
 */
/****************************************************************************/
const CReagentGroupColor* CReagentGroupColorList::GetCReagentGroupColor(const unsigned int Index)// uses order index
{
    if (Index < (unsigned int)m_OrderedListOfReagentGroupColorIDs.count()) {
        QString Key = m_OrderedListOfReagentGroupColorIDs.value(Index);
        CReagentGroupColor* p_ReagentGroupColor = m_ReagentGroupColorList.value(Key, NULL);
        return p_ReagentGroupColor;
    }
    else {
        return NULL;
    }
}

} // namespace DataManager

