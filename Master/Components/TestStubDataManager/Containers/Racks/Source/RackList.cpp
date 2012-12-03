/****************************************************************************/
/*! \file RackList.cpp
 *
 *  \brief CRackList implementation.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2012-01-02, 2012-04-23
 *   $Author:  $ M.Scherer, Raju
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
#include <QReadLocker>
#include <QWriteLocker>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QList>

#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "TestStubDataManager/Helper/Include/Helper.h"
#include "TestStubDataManager/Containers/Racks/Include/RackList.h"
#include <Global/Include/EventObject.h>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Constructor
 */
/****************************************************************************/
CRackList::CRackList() : m_DataVerificationMode(true), mp_ReadWriteLock(NULL)
{
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam RackList = Instance of the CDataRackList class
 *
 */
/****************************************************************************/
CRackList::CRackList(const CRackList& RackList):CDataContainerBase()
{
    // remove constant cast from the object
    CRackList* p_TempRackList = const_cast<CRackList*>(&RackList);
    // set the data to default values
    SetDefaultAttributes();
    m_DataVerificationMode = true;
    // create deep copy of the object
    *this = *p_TempRackList;
}

/****************************************************************************/
/*!
 *  \brief  Deconstructor
 */
/****************************************************************************/
CRackList::~CRackList()
{
    // delete all racks from the list
    //    if (!DeleteAllRacks()) {
    //        // throw error ...
    //        LOGANDTHROWARG(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    //    }
    try {
        (void)DeleteAllRacks(); // to avoid lint-534
    }
    catch(...) {

    }

    // check the read write lock
    if (mp_ReadWriteLock != NULL) {
        try {
            delete mp_ReadWriteLock;
        }
        catch(...) {
            // catch the error

        }
        mp_ReadWriteLock = NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief  Clear/create the default values
 */
/****************************************************************************/
void CRackList::SetDefaultAttributes()
{
    // create the Read write lock for threads
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);    
    // initialize the data
    Init();    
}

/****************************************************************************/
/*!
 *  \brief  Initialization
 */
/****************************************************************************/
void CRackList::Init()
{
    // delete all racks from the list
    if (!DeleteAllRacks()) {
        // throw error ...
        THROWARG(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    }
}

/****************************************************************************/
/*!
 *  \brief  Serializes the data into a data stream
 *
 *      No serialization required, because there is no XML representation of
 *      this data structure. But for cloning serialization required.
 *
 *  \iparam p_Device = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackList::SerializeContent(QIODevice& p_Device, bool CompleteData)
{
    QXmlStreamWriter XmlStreamWriter; ///< Writer for the XML

    XmlStreamWriter.setDevice(&p_Device);

    // start the XML Document
    XmlStreamWriter.writeStartDocument();
    // add the RackList tag
    XmlStreamWriter.writeStartElement("RackList");
    // store all the in XML format so that all the classes are will follow the same structure
    for (int RackCount = 0; RackCount < m_RackList.count(); RackCount++) {

        CRack* p_Rack = m_RackList[m_OrderedListOfRFIDUniqueIDs[RackCount]];  ///< Get the Rack from the List and store it

        if (!p_Rack->SerializeContent(XmlStreamWriter, CompleteData)) {
            qDebug() << "CDataRackList::Write failed. Write Racks failed!";
            return false;
        }
    }

    // store temporary data variables
    if (CompleteData) {
        // need to be added the temporary members
        XmlStreamWriter.writeStartElement("ClassTemporaryData");
        // store verification mode flag
        if (GetDataVerificationMode()) {
            XmlStreamWriter.writeAttribute("VerificationMode", "true");
        }
        else {
            XmlStreamWriter.writeAttribute("VerificationMode", "false");
        }

        //        // store the verifier count
        //        XmlStreamWriter.writeAttribute("VerifierCount", QString::number(m_VerifierList.count()));
        //        for (qint32 I = 0; I < m_VerifierList.count(); I++) {
        //            QString VerifierPtr;
        //            // make the text stream input as string
        //            QTextStream VerifierPointer(&VerifierPtr);
        //            // get the verifier list pointer address
        //            VerifierPointer << m_VerifierList.at(I);
        //            qDebug() << "\n\n Real Verifier Pointer" << m_VerifierList.at(I);
        //            XmlStreamWriter.writeAttribute(QString("Verifier%1Pointer").arg(I + 1), VerifierPointer.readAll());
        //        }
        // check for the error string
        //        if (m_ErrorList.count() > 0) {
        //            // start the special tag for error list to store
        //            XmlStreamWriter.writeStartElement("ErrorData");
        //            for (int ErrorListCount = 0; ErrorListCount < m_ErrorList.count(); ErrorListCount++) {
        //                // start of the error string
        //                XmlStreamWriter.writeStartElement("Error");
        //                // write the description of the error string
        //                XmlStreamWriter.writeAttribute("Description", m_ErrorList.value(ErrorListCount));
        //                // end of the error string
        //                XmlStreamWriter.writeEndElement();
        //            }
        //            // end element for the error data
        //            XmlStreamWriter.writeEndElement();
        //        }

        XmlStreamWriter.writeEndElement(); // for ClassTemporaryData
    }

    // end of the RackList element
    XmlStreamWriter.writeEndElement();

    // write enddocument
    XmlStreamWriter.writeEndDocument();

    //    XmlStreamWriter.device()->reset();
    //    qDebug() << XmlStreamWriter.device()->readAll();

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Deserializes the data from a serial stream
 *
 *      No deserialization required, because there is no XML representation
 *      of this data structure.
 *
 *  \iparam p_Device = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackList::DeserializeContent(QIODevice& p_Device, bool CompleteData)
{
    (void)DeleteAllRacks(); // to avoid lint-534

    QXmlStreamReader XmlStreamReader; ///< Reader for the XML

    XmlStreamReader.setDevice(&p_Device);

    // check whether xml reader at end or not
    while(!XmlStreamReader.atEnd()) {
        // read the next tag
        if (XmlStreamReader.readNext()) {

            if (XmlStreamReader.isStartElement()) {

                // check the name of the XML tag value
                if (XmlStreamReader.name() == "Rack") {
                    // create rack with default values
                    CRack* p_Rack = CreateRack(0, 0); ///< Create new rack

                    if (!p_Rack->DeserializeContent(XmlStreamReader, CompleteData)) {
                        qDebug() << "CDataRackList::Read failed. Read Racks failed!";
                        return false;
                    }
                    // now add Rack to the list
                    if (!AddRack(p_Rack)) {
                        qDebug() << "CDataRackList::Add Rack failed!";
                        delete p_Rack;
                        return false;
                    }
                }
                else if (XmlStreamReader.name() == "ClassTemporaryData") {
                    if (CompleteData) {
                        // VerificationMode
                        if (!XmlStreamReader.attributes().hasAttribute("VerificationMode")) {
                            qDebug() << "CDataRackList::### attribute <VerificationMode> is missing => abort reading";
                            return false;
                        }
                        else {
                            bool Value = false;
                            if (XmlStreamReader.attributes().value("VerificationMode").toString().toUpper() == "TRUE") {
                                Value = true;
                            }
                            SetDataVerificationMode(Value);
                        }

                        //                    // get the verifier count
                        //                    int VerifierCount = XmlStreamReader.attributes().value("VerifierCount").toString().toInt();
                        //                    // clear the list
                        //                    m_VerifierList.clear();
                        //
                        //                    // start adding the verifiers
                        //                    for (qint32 I = 0; I < VerifierCount; I++) {
                        //                        if (!XmlStreamReader.attributes().hasAttribute(QString("Verifier%1Pointer").arg(I + 1))) {
                        //                            qDebug() << "### attribute <VerifierPointer> is missing";
                        //                            return false;
                        //                        }
                        //                        // get the verifier interface address from the XML file
                        //                        IVerifierInterface* p_VerifierInterface = reinterpret_cast<IVerifierInterface*>(XmlStreamReader.attributes().
                        //                                                                                                   value(QString("Verifier%1Pointer").
                        //                                                                                                         arg(I + 1)).toString().toInt(0, 16));
                        //                        qDebug() << "\n\n Verifier Pointer" << p_VerifierInterface;
                        //                        // append the verifier
                        //                        m_VerifierList.append(p_VerifierInterface);
                        //                    }

                        // this node is not a compulsory node, so we don't require to return
                        // unsuccessful(false)
                        //                    if (Helper::ReadNode(XmlStreamReader, "ErrorData")) {
                        //                        ReadErrorList(XmlStreamReader);
                        //                    }
                    }
                }
            }
        }
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam RackList = CDataRackList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CRackList& RackList)
{
    // remove the constant by using type cast opeator
    CRackList* p_TempRackList = const_cast<CRackList*>(&RackList);

    if (!p_TempRackList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CDataRackList::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam RackList = CDataRackList class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CRackList& RackList)
{
    // deserialize the content from the data stream
    if (!RackList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CDataRackList::Operator Streaming (DeSerializeContent) failed because it does not have any Data to stream.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceRackList = CDataRackList class object
 *
 *  \return CRack Class Object
 */
/****************************************************************************/
CRackList& CRackList::operator=(const CRackList& SourceRackList)
{
    // make sure not same object
    if (this != &SourceRackList)
    {
        // create the byte array
        QByteArray* p_TempByteArray = new QByteArray();
        // create the data stream to write into a file
        QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        p_TempByteArray->clear();
        // write the data into data stream from source
        DataStream << SourceRackList;
        // reset the IO device pointer to starting position
        (void)DataStream.device()->reset(); //to avoid lint-534

        // copy the local verification flag in a temporary variable
        bool VerificationModeLocal = GetDataVerificationMode();
        // make verification flag to false, so that verification is not required
        // for deep copy
        if (VerificationModeLocal) {
            SetDataVerificationMode(false);
        }

        // read the data from data stream to destination object
        DataStream >> *this;

        delete p_TempByteArray;
        SetDataVerificationMode(VerificationModeLocal);
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief  Returns a rack from the ordered list of indexes
 *
 *  \iparam Index = Ordered list index
 *
 *  \return Pointer to a rack
 */
/****************************************************************************/
CRack* CRackList::GetRack(const int Index)
{
    QReadLocker locker(mp_ReadWriteLock);

    if (Index < m_OrderedListOfRFIDUniqueIDs.count()) {
        // get the key value from the list
        quint32 Key = m_OrderedListOfRFIDUniqueIDs.value(Index);
        // get the rack object using key value from the Hash table
        CRack* p_Rack = m_RackList.value(Key, NULL);
        return p_Rack;  //m_RackList.value(Key, NULL);
    }
    else {
        return NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief  Copies rack data
 *
 *  \iparam Index = Ordered list index
 *  \iparam p_Rack = Contains Rack data of Rack Instance
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackList::GetRack(const int Index, CRack* p_Rack)
{
    // used for threads
    QReadLocker locker(mp_ReadWriteLock);

    if (p_Rack == NULL) {
        return false;
    }

    bool Result = false;
    if (Index < m_OrderedListOfRFIDUniqueIDs.count()) {
        // get the key value from the list
        quint32 RackRFID = m_OrderedListOfRFIDUniqueIDs.value(Index);
        // check whether rack list contains RF ID or not
        if (m_RackList.contains(RackRFID)) {
            // get the rack class object
            *p_Rack = *m_RackList.value(RackRFID);
            Result = true;
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Copies rack data
 *
 *  \iparam RackRFID = Rack RFID number
 *  \iparam p_Rack = Contains Rack data of Rack Instance
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackList::GetRack(const quint32 RackRFID, CRack* p_Rack)
{
    // used for thread
    QReadLocker locker(mp_ReadWriteLock);

    if (p_Rack == NULL) {
        return false;
    }

    // check whether rack list contains RF ID or not
    if (m_RackList.contains(RackRFID)) {
        *p_Rack = *m_RackList.value(RackRFID);
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Checks if a certain rack is in the list
 *
 *  \iparam RackRFID = Rack RFID number
 *
 *  \return Exists (true) or not (false)
 */
/****************************************************************************/
bool CRackList::RackExists(const quint32 RackRFID)
{
    // used for threads
    QReadLocker locker(mp_ReadWriteLock);
    // Get the flag whether rack exists or not
    return m_RackList.contains(RackRFID);
}

/****************************************************************************/
/*!
 *  \brief  Adds a rack to the list
 *
 *  \iparam p_Rack = Pointer to a rack
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackList::AddRack(const CRack* p_Rack)
{
    if (p_Rack == NULL) {
        return false;
    }

    // get the RF ID
    quint32 RFID = const_cast<CRack*>(p_Rack)->GetRFIDUniqueID();
    // check whether Hash table consists of RF ID or not
    if (m_RackList.contains(RFID)) {
        // there is already a rack with the given RackRFID
        // => use UpdateRack instead
        //    or call DeleteRack before AddRack
        return false;
    }

    // Create clone and add it in temporary variable. Otherwise the
    // local copy will be deleted, who is calling the function
    CRack *p_TempRack = new CRack();
    *p_TempRack = *p_Rack;

    // store the result flag
    bool Result = true;
    // check the verification flags
    if (m_DataVerificationMode) {
        ErrorHash_t ErrorHash;
        // create the temporary CDataRackList class object
        CRackList* p_DRL_Verification = new CRackList();
        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DRL_Verification = *this;

            // disable verification in clone
            p_DRL_Verification->SetDataVerificationMode(false);

            // execute required action (AddRack) in clone
            Result = p_DRL_Verification->AddRack(p_TempRack);

            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_DRL_Verification);
            }
        }

        if (Result) {
            // if content ok, clone backwards
            *this = *p_DRL_Verification;
            Result = true;
        }

        delete p_TempRack;
        // delete test clone
        delete p_DRL_Verification;

    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);
        // add the object into the hash table
        m_RackList.insert(RFID, p_TempRack);
        // add rack RF ID to the list
        m_OrderedListOfRFIDUniqueIDs.append(RFID);

        Result = true;
    }

    //delete p_TempRack;
    //delete p_Rack;

    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Updates a rack in the list
 *
 *  \iparam p_Rack = Pointer to a rack
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackList::UpdateRack(const CRack* p_Rack)
{
    if (p_Rack == NULL) {
        return false;
    }
    // get the RF ID
    quint32 RFID = const_cast<CRack*>(p_Rack)->GetRFIDUniqueID();
    // check whether Hash table consists of RF ID or not
    if (!m_RackList.contains(RFID)) {
        // there is no rack with the given RackRFID
        return false;
    }

    bool Result = false;
    // check the verification flags
    if (m_DataVerificationMode) {
        ErrorHash_t ErrorHash;
        // create the temporary CDataRackList class object
        CRackList* p_DRL_Verification = new CRackList();

        // Create clone and add it in temporary variable. Otherwise the
        // local copy will be deleted, who is calling the function
        CRack* p_TempRack = new CRack(RFID, const_cast<CRack*>(p_Rack)->GetRFIDUserData());
        *p_TempRack = *p_Rack;

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DRL_Verification = *this;

            // disable verification in clone
            p_DRL_Verification->SetDataVerificationMode(false);

            // execute required action (UpdateRack) in clone
            Result = p_DRL_Verification->UpdateRack(p_Rack);

            if (Result) {                
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_DRL_Verification);
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_DRL_Verification;
            Result = true;
        }

        // delete the temporary rack object
        delete p_TempRack;
        // delete test clone
        delete p_DRL_Verification;

    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);
        // replace the old class object with updated class object
        *m_RackList[RFID] = *p_Rack;
        Result = true;
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Deletes a rack from the list
 *
 *  \iparam RackRFID = RFID number of the rack
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackList::DeleteRack(const quint32 RackRFID)
{
    if (m_RackList.contains(RackRFID)) {
        //get rack from RackList and free memory
        delete m_RackList.value(RackRFID);

        //remove pointer to rack from RackList
        m_RackList.remove(RackRFID);

        //remove RackRFID from RFID list
        int MatchIndex = -1;
        for (int ListCount = 0; ListCount < m_OrderedListOfRFIDUniqueIDs.count(); ListCount++) {
            if (m_OrderedListOfRFIDUniqueIDs[ListCount] == RackRFID) {
                MatchIndex = ListCount;
                break;
            }
        }
        // MatchIndex MUST never be -1 !!!
        Q_ASSERT(MatchIndex != -1);
        m_OrderedListOfRFIDUniqueIDs.removeAt(MatchIndex);

        return true;
    }
    else {
        // there is no rack with the given RackRFID
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes a rack from the list
 *
 *  \iparam Index = Ordered list index
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackList::DeleteRack(const int Index)
{
    if (Index < m_OrderedListOfRFIDUniqueIDs.count()) {
        // get the RF ID from the list
        quint32 RackRFID = m_OrderedListOfRFIDUniqueIDs.value(Index);
        // delete the Rack
        return DeleteRack(RackRFID);
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes all racks from the list
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackList::DeleteAllRacks()
{
    bool Result = true;    

    while (m_RackList.size() > 0) {
        if (DeleteRack(0) == false) {
            Result = false;
        }
    }
    // clear the Hash table
    m_RackList.clear();
    // clear the list
    m_OrderedListOfRFIDUniqueIDs.clear();

    return Result;
}

} // namespace DataManager
