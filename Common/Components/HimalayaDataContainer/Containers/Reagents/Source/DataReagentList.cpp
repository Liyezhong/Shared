
/****************************************************************************/
/*! \file DataReagentList.cpp
 *
 *  \brief DataReagentList Implementation.
 *
 *   $Version: $ 0.3
 *   $Date:    $ 2012-04-20,2013-01-25
 *   $Author:  $ Vikram MK,Swati Tiwari
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
#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/Reagent.h"
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include "Global/Include/UITranslator.h"
#include <QDataStream>
//lint -sem(QHash::insert, custodial(1))

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 *
 */
/****************************************************************************/
CDataReagentList::CDataReagentList() :m_Version(0), m_DataVerificationMode(true), m_Filename(""), mp_ReadWriteLock(NULL)
{
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ReagentList = Instance of the CDataReagentList class
 *
 *  \return
 */
/****************************************************************************/
CDataReagentList::CDataReagentList(const CDataReagentList& ReagentList) : CDataContainerBase()
{
    CDataReagentList* p_TempReagentList = const_cast<CDataReagentList*>(&ReagentList);

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

CDataReagentList::~CDataReagentList()
{
    try {
        (void)DeleteAllReagents(); // to avoid lint-534
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
 *  \brief Initilisation function for CDataReagentList
 *
 *  \return void
 */
/****************************************************************************/
void CDataReagentList::Init()
{
    m_Version = 0;
    m_Filename = "";

    if (!DeleteAllReagents()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_REAGENT_OPERATION_FAILED,
                                                   Global::tTranslatableStringList() << "CDataReagentList::Init()", true);
    }

}

/****************************************************************************/
/*!
 *  \brief Defaults to the initial state of Attributes of CDataReagentList
 *
 *  \return void
 */
/****************************************************************************/

void CDataReagentList::SetDefaultAttributes()
{
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    //    m_VerifierList.clear();
    // fill heating start mode hash
//    HashOfHeatingStartModes.insert("ATDEVICESTART", AT_DEVICE_START);
//    HashOfHeatingStartModes.insert("ATPROGRAMSTART", AT_PROGRAM_START);
    Init();
}

// the complete content of this class/object will be
// written to the IODevice
// used e.g. for transfer to GUI
/****************************************************************************/
/*!
 *  \brief Writes the CDataReagentList Data to QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentList::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    QString StringValue;
    bool Result = true;
    QXmlStreamWriter XmlStreamWriter;

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    //XmlStreamWriter.setAutoFormattingIndent(4);
    XmlStreamWriter.writeStartDocument();

    XmlStreamWriter.writeDTD("<!DOCTYPE Reagents>");
    XmlStreamWriter.writeStartElement("Reagents");

    // write attribute version
    (void)StringValue.setNum(GetVersion());  //to suppress lint-534
    XmlStreamWriter.writeAttribute("Version", StringValue);

    XmlStreamWriter.writeAttribute("NextUserReagentID", GetNextFreeReagentID(false));


    // write Leica reagents
    if (!WriteReagents(XmlStreamWriter, CompleteData)) {
        qDebug() << "CDataReagentListFileWriter::Write failed. WriteReagents (LEICA) failed!";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_REAGENT_LIST_WRITE_FAILED,
                                                   Global::tTranslatableStringList() << "LEICA", true);
        Result = false;
    }

    //======NODE=======Temporary Data Variables=========================
    if ((CompleteData) && (Result)) {
        //Need to be added the temporary members
        XmlStreamWriter.writeStartElement("ClassTemporaryData");

        XmlStreamWriter.writeAttribute("FileName", GetFilename());
        if (GetDataVerificationMode()) {
            XmlStreamWriter.writeAttribute("VerificationMode", "true");
        }
        else {
            XmlStreamWriter.writeAttribute("VerificationMode", "false");
        }
        //        XmlStreamWriter.writeAttribute("VerifierCount", QString::number(m_VerifierList.count()));
        //        for (qint32 I = 0; I < m_VerifierList.count(); I++) {
        //            QString VerifierPtr;
        //            QTextStream VerifierPointer(&VerifierPtr);
        //            VerifierPointer << m_VerifierList.at(I);
        //            qDebug()<<"\n\n Real Verifier Pointer"<<m_VerifierList.at(I);
        //            XmlStreamWriter.writeAttribute(QString("Verifier%1Pointer").arg(I + 1), VerifierPointer.readAll());
        //        }
        XmlStreamWriter.writeEndElement(); // for ClassTemporaryData
    }
    //======NODE=END====Temporary Data Variables=========================
    // The above Result has to be true to write the end element
    XmlStreamWriter.writeEndElement(); // for  Reagents
    XmlStreamWriter.writeEndDocument(); // End of Document

    return Result;
}

/****************************************************************************/
/*!
 *  \brief This writes the single reagent
 *
 *  \iparam XmlStreamWriter = Instance of the XmlStreamWriter
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentList::WriteReagents(QXmlStreamWriter& XmlStreamWriter,
                                     bool CompleteData)
{    
    for(int i=0;i<GetNumberOfReagents();i++) {        
        CReagent *p_Reagent =const_cast<CReagent*> (GetReagent(i));
        if (p_Reagent != NULL) {
            if (!p_Reagent->SerializeContent(XmlStreamWriter, CompleteData)) {
                qDebug() << "CReagentFileWriter::Write failed. WriteReagents failed!";
                return false;
            }
        }

    } // end of for loop
    XmlStreamWriter.writeEndElement(); // for HelperReagents
    return true;
}

// the content of IODevice will be
// mirrored into this class/object
/****************************************************************************/
/*!
 *  \brief Reads the CDataReagentList Data from QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentList::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{
    QXmlStreamReader XmlStreamReader;

    XmlStreamReader.setDevice(&IODevice);

    // look for node <ReagentGroups>
    if (!Helper::ReadNode(XmlStreamReader, "Reagents")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: Reagent";
        return false;
    }

    // Read attribute Version
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "Version", true);
        return false;
    }
    SetVersion(XmlStreamReader.attributes().value("Version").toString().toInt());
    //  Now Delete the Reagent list and ordered list of Reagent ID's
    if (!DeleteAllReagents()) {
        qDebug() << "DeserializeContent: abort reading. Deleting all the reagents failed";
        return false;
    }

    // get attribute NextReagentID
    if (!XmlStreamReader.attributes().hasAttribute("NextUserReagentID")) {
        qDebug() << "### attribute <NextReagentID> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "NextUserReagentID", true);
        return false;
    }
    // Need to check on the same
    SetNextFreeReagentID(XmlStreamReader.attributes().value("NextUserReagentID").toString());

    if (!ReadReagents(XmlStreamReader, CompleteData)) {
        qDebug() << "Failure in reading the Leica Reagents";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_REAGENT_LIST_READ_FAILED,
                                                   Global::tTranslatableStringList() << "LEICA", true);
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief This reads the single reagent
 *
 *  \iparam XmlStreamReader = Instance of the XmlStreamReader
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentList::ReadReagents(QXmlStreamReader& XmlStreamReader,
                                    bool CompleteData){

    while (!XmlStreamReader.atEnd()) {
        if (XmlStreamReader.readNext() == QXmlStreamReader::Invalid) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }
        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "Reagent") {
                //create a dummy reagent
                CReagent Reagent;
                // set unused values

                    Reagent.SetVisibleState(true);

                if (!Reagent.DeserializeContent(XmlStreamReader, CompleteData)) {
                    qDebug() << "CReagentFileReader::Read failed. ReadReagents failed!";
                    return false;
                }
                // Now add this Reagent
                if (!AddReagent(&Reagent)) {
                    qDebug() << "CReagentAdd::Add Reagent failed!";
                    return false;
                }
            }
            else {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "Reagents") {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this Reagent";
            break; // exit from while loop
        }
    } // end of while loop
    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ReagentList = CDataReagentList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CDataReagentList& ReagentList)
{
    CDataReagentList* p_TempReagentList = const_cast<CDataReagentList*>(&ReagentList);

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
 *  \iparam ReagentList = CDataReagentList class object
 *
 *  \return Input Stream
 */
/****************************************************************************/

QDataStream& operator >>(QDataStream& InDataStream, CDataReagentList& ReagentList)
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
 *  \iparam SourceReagentList = CDataReagentList class object
 *
 *  \return CReagent Class Object
 */
/****************************************************************************/

CDataReagentList& CDataReagentList::operator=(const CDataReagentList& SourceReagentList)
{
    // make sure not same object
    if (this != &SourceReagentList)
    {
        QByteArray TempByteArray;
        CDataReagentList* p_TempReagentList = const_cast<CDataReagentList*>(&SourceReagentList);
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
        {
            QWriteLocker locker(mp_ReadWriteLock);
            DataStream >> *this;
        }
        if (TempVerificationMode) {
            SetDataVerificationMode(true);
        }
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Reads the data and stores in CDataReagentList from the file
 *
 *  \iparam Filename = Name of the Input File
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentList::Read(QString Filename)
{

    //check if file exists
    if (!QFile::exists(Filename)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_FILE_NOT_EXISTS,
                                                   Global::tTranslatableStringList() << Filename, true);
        return false;
    }

    bool Result = true;

    if (m_DataVerificationMode) {
//        ErrorMap_t ErrorHash;
//        QWriteLocker locker(mp_ReadWriteLock);

        // create instance of CDataReagentList for verification
        CDataReagentList* p_DRL_Verification = new CDataReagentList();

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
            qDebug() << "### CDataReagentList::Read failed for file: " << Filename;
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
 *  \brief returns the Next available reagent Id
 *  \iparam AutoCount = true to increase reagent ID , false - doesn't increase
 *                       reagent ID.
 *  \return Next reagent ID
 */
/****************************************************************************/
QString CDataReagentList::GetNextFreeReagentID(bool AutoCount)
{
    QString NextReagID = m_NextReagentID;
    if (AutoCount) {
        unsigned int  NextReagentID = m_NextReagentID.mid(1, -1).toInt();
        NextReagID = m_NextReagentID;
        NextReagentID++;
        m_NextReagentID.clear();
        m_NextReagentID = QString::number(NextReagentID);
        //Ignore return value
        (void)m_NextReagentID.prepend("U");
    }
    return NextReagID;
}

/****************************************************************************/
/*!
 *  \brief Gets the Reagent Type from ID passed
 *
 *  \iparam ReagentID = ID of the Reagent
 *
 *  \return Reagent Type like lEICA , USER or SPECIAL
 */
/****************************************************************************/
ReagentType_t CDataReagentList::GetReagentType(const QString ReagentID)
{
    CReagent* p_Reagent = const_cast<CReagent*>(GetReagent(ReagentID));

    try {
        CHECKPTR(p_Reagent);
    }
    catch(Global::Exception &E) {
        Global::EventObject::Instance().RaiseEvent(E);
        return UNDEFINED_REAGENT_TYPE;
    }

    return p_Reagent->GetReagentType();
}

/****************************************************************************/
/*!
 *  \brief Gets the Reagent Type from the Index in Ordered list
 *
 *  \iparam Index =  Required Index number in the list
 *
 *  \return Reagent Type like lEICA or USER
 */
/****************************************************************************/
ReagentType_t CDataReagentList::GetReagentType(const unsigned int Index)
{
    CReagent* p_Reagent = const_cast<CReagent*> (GetReagent(Index));

    try {
        CHECKPTR(p_Reagent);
    }
    catch(Global::Exception &E) {
        Global::EventObject::Instance().RaiseEvent(E);
        return UNDEFINED_REAGENT_TYPE;
    }
    return p_Reagent->GetReagentType();
}

/****************************************************************************/
/*!
 *  \brief Gets the Reagent pointer by index from List
 *
 *  \iparam Index =  Required Index number in the list
 *
 *  \return CReagent pointer
 */
/****************************************************************************/
CReagent* CDataReagentList::GetReagent(const unsigned int Index)
{
    QReadLocker locker(mp_ReadWriteLock);

    if (Index < (unsigned int)m_OrderedListOfReagentIDs.count()) {
        QString Key = m_OrderedListOfReagentIDs.value(Index);
        CReagent* p_Reagent = m_ReagentList.value(Key, NULL);
        return p_Reagent;
    }
    else {
        return NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief Deep Copies the Reagent content Reagent Instance by Index
 *
 *  \iparam Index =  Required Index number in the list
 *  \iparam Reagent =  CReagent Instance
 *
 *  \return True or False
 */
/****************************************************************************/
//deep copy
bool CDataReagentList::GetReagent(const unsigned int Index, CReagent& Reagent)
{
    QReadLocker locker(mp_ReadWriteLock);

    if (Index < (unsigned int)m_OrderedListOfReagentIDs.count()) {
        QString ReagentID = m_OrderedListOfReagentIDs.value(Index);
        if (m_ReagentList.contains(ReagentID)) {
            Reagent = *m_ReagentList.value(ReagentID); // this give the exception if not copied
            return true;
        }
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief Deep Copies the Reagent content Reagent Instance by Reagent ID
 *
 *  \iparam ReagentID = ID of the Reagent
 *  \iparam Reagent =  CReagent Instance
 *
 *  \return True or False
 */
/****************************************************************************/
//deep copy
bool CDataReagentList::GetReagent(const QString ReagentID, CReagent& Reagent)
{
    QReadLocker locker(mp_ReadWriteLock);

    if (m_ReagentList.contains(ReagentID)) {
        Reagent = *m_ReagentList.value(ReagentID);
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Checks with ID whether specific reagent exists
 *
 *  \iparam ReagentID = ID of the Reagent
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentList::ReagentExists(const QString ReagentID)
{
    QReadLocker locker(mp_ReadWriteLock);
    return m_ReagentList.contains(ReagentID);
}

/****************************************************************************/
/*!
 *  \brief Adds the  new Reagent to the list
 *
 *  \iparam p_Reagent =  CReagent Instance
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentList::AddReagent(const CReagent* p_Reagent)
{

    // check for the unique Reagent Name existence
    if (!CheckForUniquePropeties(p_Reagent)) {
        return false;
    }

    CReagent *p_TempReagent = new CReagent();
    *p_TempReagent = *p_Reagent;

    bool Result = true;
    bool LocalVerify = true;
    if (m_DataVerificationMode) {

        CDataReagentList* p_DRL_Verification = new CDataReagentList();

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DRL_Verification = *this;

            // disable verification in clone
            p_DRL_Verification->SetDataVerificationMode(false);

            // execute required action (AddReagent) in clone
            Result = p_DRL_Verification->AddReagent(p_TempReagent);
            if (Result) {
                // now check new content => call all active verifiers
                LocalVerify = DoLocalVerification(p_DRL_Verification);
            }
        }

        if (Result && LocalVerify) {
            // if content ok, clone backwards
            *this = *p_DRL_Verification;
        }
        else {
            Result = false;
        }
        delete p_TempReagent;
        // delete test clone
        delete p_DRL_Verification;
    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);
        QString ID = p_Reagent->GetReagentID();
        m_ReagentList.insert(ID, p_TempReagent);
        m_OrderedListOfReagentIDs.append(ID);
        // White space at begining , end are removed from names. If continuous
        // White space exits between a name, they are replaced by a single white
        //space
        m_ReagentListNames.append(p_TempReagent->GetReagentName().simplified());

        Result = true;
    }
    /*lint -e831 */
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Updates the existing Reagent in the list
 *
 *  \iparam p_Reagent =  CReagent Instance
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentList::UpdateReagent(const CReagent* p_Reagent)
{
    try {
        CHECKPTR(p_Reagent);
    }
    catch(Global::Exception &E) {
        Global::EventObject::Instance().RaiseEvent(E);
        m_ErrorHash.insert(EVENT_DM_REAGENT_OPERATION_FAILED,
                           Global::tTranslatableStringList() << "CDataReagentList::UpdateReagent");
        SetErrorList(&m_ErrorHash);
        return false;
    }

    // check for the unique Reagent Name existence
    if (!CheckForUniquePropeties(p_Reagent, true)) {
        return false;
    }

    QString ID = p_Reagent->GetReagentID();
    CReagent const *p_UpdatedReagent = GetReagent(ID);
    if (!p_UpdatedReagent)
        return false;

    QString PreviousName  = p_UpdatedReagent->GetReagentName().simplified();

    bool Result = false;
    if (m_DataVerificationMode) {
        ErrorMap_t ErrorHash;
        CDataReagentList* p_DRL_Verification = new CDataReagentList();

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DRL_Verification = *this;

            // disable verification in clone
            p_DRL_Verification->SetDataVerificationMode(false);

            // execute required action (UpdateReagent) in clone
            Result = p_DRL_Verification->UpdateReagent(p_Reagent);
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
        // delete clone
        delete p_DRL_Verification;

    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);

        int Index;
        if (PreviousName != p_Reagent->GetReagentName().trimmed() ) {
            //Update Reagent Name list
            Index = m_ReagentListNames.indexOf(PreviousName);
            if (Index != -1) {
                m_ReagentListNames.replace(Index, p_Reagent->GetReagentName().simplified());
            }
            else {
                Result = false;
            }
        }

        *m_ReagentList[ID] = *p_Reagent;
        Result = true;
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Deletes the existing reagent from the list
 *
 *  \iparam ReagentID = ID of the Reagent
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataReagentList::DeleteReagent(const QString ReagentID)
{
    try {
        if (m_ReagentList.contains(ReagentID)) {
            //get reagent from ReagentList and free memory
            delete m_ReagentList.value(ReagentID);
            //remove pointer to reagent from ReagentList
            m_ReagentList.remove(ReagentID);

            //remove ReagentID from ID list
            int MatchIndex = -1;
            for (int i=0; i<m_OrderedListOfReagentIDs.count(); i++) {
                if (m_OrderedListOfReagentIDs[i] == ReagentID) {
                    MatchIndex = i;
                    break;
                }
            }
            // MatchIndex MUST never be -1 !!!
            Q_ASSERT(MatchIndex != -1);
            m_OrderedListOfReagentIDs.removeAt(MatchIndex);
            m_ReagentListNames.removeAt(MatchIndex);

            return true;
        }
        else {
            qDebug() << " The Reagent doesn't contains in the list ";
            // there is no reagent with the given ReagentID
            m_ErrorHash.insert(EVENT_DM_REAGENT_ID_DOESTNOT_EXISTS,
                               Global::tTranslatableStringList() << ReagentID);
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_REAGENT_ID_DOESTNOT_EXISTS,
                                                       Global::tTranslatableStringList() << ReagentID, true);
            SetErrorList(&m_ErrorHash);
            return false;
        }
    }
    catch (...) {
        qDebug() << "### exception in CDataReagentList::DeleteReagent(const QString ReagentID)";
        m_ErrorHash.insert(EVENT_DM_REAGENT_OPERATION_FAILED,
                           Global::tTranslatableStringList() << "DeleteReagent");
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
bool CDataReagentList::DeleteReagent(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedListOfReagentIDs.count()) {
        QString ReagentID = m_OrderedListOfReagentIDs.value(Index);
        return DeleteReagent(ReagentID);
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
bool CDataReagentList::DeleteAllReagents()
{
    while (m_ReagentList.size() > 0) {
        if (DeleteReagent(0) == false) {
            return false;
        }
    }

    m_ReagentList.clear();
    m_OrderedListOfReagentIDs.clear();
    // clear all the string list of Reagent long and short names
    m_ReagentListNames.clear();
    return true;
}

/****************************************************************************/
/*!
 *  \brief Verifies the Data with respect to Verifiers
 *
 *  \iparam p_Reagent = the reagent to be checked
 *  \iparam excludeSeft excludeSeft
 *
 *  \return On Successful (True) or not (False)
 */
/****************************************************************************/
bool CDataReagentList::CheckForUniquePropeties(const CReagent* p_Reagent, bool excludeSeft)
{
    bool Result = true;
    QString ID = const_cast<CReagent*>(p_Reagent)->GetReagentID();
    CReagent* oldReagent = m_ReagentList.value(ID);

    if (excludeSeft && !oldReagent)
        return false;

    bool isHave = false;
    if (excludeSeft)
    {
        ListOfReagents_t tempReagentList = m_ReagentList;
        tempReagentList.remove(ID);
        isHave = tempReagentList.contains(ID);
    }
    else
    {
        isHave = m_ReagentList.contains(ID);
    }

     if (isHave) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_REAGENT_ID_NOT_UNIQUE,
                                                   Global::tTranslatableStringList() << p_Reagent->GetReagentID()
                                                   << p_Reagent->GetReagentName(),
                                                   true,
                                                   Global::GUI_MSG_BOX);
        (void)m_ErrorHash.insert(EVENT_DM_REAGENT_ID_NOT_UNIQUE,
                           Global::tTranslatableStringList() << p_Reagent->GetReagentID()
                           << p_Reagent->GetReagentName());
        SetErrorList(&m_ErrorHash);
        Result = false;
    }

    // check for the Short name existence in the reagent list
//    if (excludeSeft)//update reagent
//    {
//        QStringList nameList = m_ReagentListNames;
//        (void)nameList.removeOne(oldReagent->GetReagentName());
//        isHave = nameList.contains(p_Reagent->GetReagentName().simplified(), Qt::CaseInsensitive);
//    }
//    else// add reagent
    {
        //isHave = m_ReagentListNames.contains(p_Reagent->GetReagentName().simplified(), Qt::CaseInsensitive);
        isHave = false;
        QList<QLocale::Language> lans = Global::UITranslator::TranslatorInstance().GetLanguages();
        foreach(QLocale::Language lan, lans)
        {
            if(isHave)
            {
                break;
            }
            QString NewRegName = p_Reagent->GetReagentName().simplified();
            if(p_Reagent->IsLeicaReagent())
            {
                bool ok = false;
                quint32 strid = p_Reagent->GetReagentNameID().toUInt(&ok);
                if(ok)
                {
                    NewRegName = Global::UITranslator::TranslatorInstance().TranslateToLanguage(lan, strid);
                }
            }
            foreach(CReagent* pReg, m_ReagentList)
            {
                QString RegName = pReg->GetReagentName().simplified();
                if(pReg->IsLeicaReagent())
                {
                    bool ok = false;
                    quint32 strid = pReg->GetReagentNameID().toUInt(&ok);
                    if(ok)
                    {
                        RegName = Global::UITranslator::TranslatorInstance().TranslateToLanguage(lan, strid);
                    }
                }
                if((NewRegName.compare(RegName,Qt::CaseInsensitive) == 0) &&
                        (p_Reagent->GetReagentID().compare(pReg->GetReagentID(),Qt::CaseInsensitive) != 0))
                {
                    isHave = true;
                    break;
                }
            }
        }
    }
    if (isHave) {
        (void)m_ErrorHash.insert(EVENT_DM_REAGENT_NAME_NOT_UNIQUE, Global::tTranslatableStringList() << p_Reagent->GetReagentName().simplified());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_REAGENT_NAME_NOT_UNIQUE,
                                                   Global::tTranslatableStringList() << p_Reagent->GetReagentName().simplified(),
                                                   true,
                                                   Global::GUI_MSG_BOX);
        SetErrorList(&m_ErrorHash);
         Result = false;
    }
    return Result;
}



void CDataReagentList::UpdateOnLanguageChanged()
{
    m_ReagentListNames.clear();
    for (qint32 I = 0; I < m_ReagentList.count(); I++) {
        CReagent *p_Reagent = GetReagent(I);
        if(p_Reagent && !p_Reagent->GetReagentNameID().isEmpty()){
            bool ok = false;
            quint32 strid = p_Reagent->GetReagentNameID().toUInt(&ok);
            if(ok && strid > 0)
            {
                m_ReagentListNames.append(p_Reagent->GetReagentName().simplified());
            }
        }
    }
}

} // namespace DataManager
