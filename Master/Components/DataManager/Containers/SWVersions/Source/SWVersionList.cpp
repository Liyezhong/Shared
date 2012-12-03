/****************************************************************************/
/*! \file SWVersionList.cpp
 *
 *  \brief Implementation file for class CSWVersionList.
 *         This class reads the "SW_version.xml" file.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-07
 *  $Author:    $ Raju
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
#include <QString>
#include <QBuffer>
#include <QXmlStreamReader>
#include <iostream>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Containers/SWVersions/Include/SWVersionList.h"


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 *
 */
/****************************************************************************/
CSWVersionList::CSWVersionList()  : CDataContainerBase(),
    m_Version(1),    
    m_DataVerificationMode(true),
    m_Filename(""),
    m_SWReleaseVersion(""),
    m_SWReleaseDate("")
{
    // clear the details
    m_SWDetailsList.clear();
    m_SWNameList.clear();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam SWVersionList = Instance of the CSWVersionList class
 *
 *  \return
 */
/****************************************************************************/
CSWVersionList::CSWVersionList(const CSWVersionList& SWVersionList)  :
    CDataContainerBase(),    
    m_Version(1),    
    m_DataVerificationMode(true),
    m_Filename(""),
    m_SWReleaseVersion(""),
    m_SWReleaseDate("")
{
    CSWVersionList* p_TempSWVersionList = const_cast<CSWVersionList*>(&SWVersionList);

    *this = *p_TempSWVersionList;
}

/****************************************************************************/
/*!
 *  \brief Writes from the CSWVersionList object to a IODevice.
 *
 *  \iparam IODevice = Instance of the QIODevice
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    QXmlStreamWriter XmlStreamWriter; ///< Xml stream writer object to write the Xml contents in a file
    bool Result = true;

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    //XmlStreamWriter.setAutoFormattingIndent(4);
    // write the document
    XmlStreamWriter.writeStartDocument();

    // start the xml element <SW_Version>
    XmlStreamWriter.writeStartElement("SW_Version");
    XmlStreamWriter.writeAttribute("Version", QString::number(GetVersion()));


    // start the xml element <SW>
    XmlStreamWriter.writeStartElement("SW");
    XmlStreamWriter.writeStartElement("Release");
    XmlStreamWriter.writeAttribute("Version", GetSWReleaseVersion());
    XmlStreamWriter.writeAttribute("Date", GetSWReleaseDate());

    XmlStreamWriter.writeEndElement(); //SW



    // write Master SW details in the xml file
    if (!WriteSWDetails(XmlStreamWriter, MASTERSOFTWARE, CompleteData)) {
        qDebug() << "CSWVersionList::Write failed. WriteSWDetails (MASTER) failed!";
        Result = false;
    }

    // write firmware details in the xml file
    if (!WriteSWDetails(XmlStreamWriter, FIRMWARE, CompleteData)) {
        qDebug() << "CSWVersionList::Write failed. WriteSWDetails (FIRMWARE) failed!";
        Result = false;
    }


    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
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

    XmlStreamWriter.writeEndElement(); //SW_Version
    // write enddocument
    XmlStreamWriter.writeEndDocument();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XmlStreamReader to CSWVersionList object.
 *
 *  \iparam IODevice = Instance of the QIODevice.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{
    QXmlStreamReader XmlStreamReader;

    XmlStreamReader.setDevice(&IODevice);
    // read the <SWVersion> tag
    if(!Helper::ReadNode(XmlStreamReader, "SW_Version")) {
        return false;
    }

    // read the version attribute
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        return false;
    }
    SetVersion(XmlStreamReader.attributes().value("Version").toString().toUInt());

    // read the <SW> tag
    if(!Helper::ReadNode(XmlStreamReader, "SW")) {
        return false;
    }

    // read the <Release> tag
    if(!Helper::ReadNode(XmlStreamReader, "Release")) {
        return false;
    }

    // read the version attribute
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        return false;
    }
    SetSWReleaseVersion(XmlStreamReader.attributes().value("Version").toString());

    // read the date attribute
    if (!XmlStreamReader.attributes().hasAttribute("Date")) {
        qDebug() << "### attribute <Date> is missing => abort reading";
        return false;
    }
    SetSWReleaseDate(XmlStreamReader.attributes().value("Date").toString());

    //  Now Delete the SWDetails list and SW names
    if (!DeleteAllSWDetails()) {
        //throw error ...
        THROWARG(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    }

    (void)XmlStreamReader.readElementText(QXmlStreamReader::IncludeChildElements);// lint -e534

    if (!ReadSWDetails(XmlStreamReader, "Master_SW", CompleteData)) {
        qDebug() << "Failure in reading the Master sw";
        return false;
    }    

    if (!ReadSWDetails(XmlStreamReader, "Firmware", CompleteData)) {
        qDebug() << "Failure in reading the firmware";
        return false;
    }

    //======NODE=======Temporary Data Variables=========================
    if (CompleteData) {        
        if (!Helper::ReadNode(XmlStreamReader, "ClassTemporaryData")) {
            qDebug() << "DeserializeContent: abort reading. Node not found: ClassTemporaryData";
            return false;
        }
        // File name
        if (!XmlStreamReader.attributes().hasAttribute("FileName")) {
            qDebug() << "### attribute <Filename> is missing => abort reading";
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
        //        // Verifier Pointers
        //        if (!XmlStreamReader.attributes().hasAttribute("VerifierCount")) {
        //            qDebug() << "### attribute <VerifierCount> is missing";
        //            return false;
        //        }
        //        int VerifierCount = XmlStreamReader.attributes().value("VerifierCount").toString().toInt();
        //        m_VerifierList.clear();
        //        for (qint32 I = 0; I < VerifierCount; I++) {
        //            if (!XmlStreamReader.attributes().hasAttribute(QString("Verifier%1Pointer").arg(I + 1))) {
        //                qDebug() << "### attribute <VerifierPointer> is missing";
        //                return false;
        //            }
        //            IVerifierInterface* p_VerifierInterface = reinterpret_cast<IVerifierInterface*>(XmlStreamReader.attributes().
        //                                                                                       value(QString("Verifier%1Pointer").
        //                                                                                             arg(I + 1)).toString().toInt(0, 16));
        //            qDebug()<<"\n\n Verifier Pointer"<<p_VerifierInterface;
        //            m_VerifierList.append(p_VerifierInterface);
        //        } // end of for loop of Verifier

    }
    //======NODE=END====Temporary Data Variables=========================

    return true;
}

/****************************************************************************/
/*!
 *  \brief This reads the single SWDetails
 *
 *  \iparam XmlStreamReader = Instance of the XmlStreamReader
 *  \iparam NodeName = Name of the node in the Xml file
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::ReadSWDetails(QXmlStreamReader& XmlStreamReader, QString NodeName, bool CompleteData)
{
    if (!Helper::ReadNode(XmlStreamReader, NodeName)) {
        qDebug() << "node not found with the given name" << NodeName;
        return false;
    }

    while (!XmlStreamReader.atEnd()) {
        if (XmlStreamReader.readNext() == QXmlStreamReader::Invalid) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }
        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "file") {
                //create a dummy SWDetails
                CSWDetails* p_SWDetails = CreateSWDetails("dummy");

                if (!p_SWDetails->DeserializeContent(XmlStreamReader, CompleteData)) {
                    qDebug() << "CSWDetails::Read failed. ReadSWDetailss failed!";
                    delete p_SWDetails;
                    return false;
                }
                // check the node name
                if (NodeName == "Firmware") {
                    p_SWDetails->SetSWType(FIRMWARE);
                }
                else {
                    p_SWDetails->SetSWType(MASTERSOFTWARE);
                }

                // Now add this SWDetails
                if (!AddSWDetails(p_SWDetails)) {
                    qDebug() << "CSWDetails::Add SWDetails failed!";
                    delete p_SWDetails;
                    return false;
                }
            }
            else {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == NodeName) {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this SWDetails";
            break; // exit from while loop
        }
    } // end of while loop
    return true;
}

/****************************************************************************/
/*!
 *  \brief This writes the single SWDetails
 *
 *  \iparam XmlStreamWriter = Instance of the XmlStreamWriter
 *  \iparam SWType = SW Type  MASTERSOFTWARE and FIRMWARE
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::WriteSWDetails(QXmlStreamWriter& XmlStreamWriter, SWType_t SWType, bool CompleteData)
{
    if(SWType == MASTERSOFTWARE) {
        XmlStreamWriter.writeStartElement("Master_SW");
    }
    else if(SWType == FIRMWARE) {
        XmlStreamWriter.writeStartElement("Firmware");
    }
    else {
        qDebug() << "### Parameter Unknown SW Type: " << (int)SWType;
        return false;
    }
    for(int SWDetailsCount = 0; SWDetailsCount < GetNumberOfSWDetails(); SWDetailsCount++) {
        CSWDetails *p_SWDetails =const_cast<CSWDetails*> (GetSWDetails(SWDetailsCount));
        if (p_SWDetails != NULL) {
            // check the software type
            if (p_SWDetails->GetSWType() == SWType) {
                if (!p_SWDetails->SerializeContent(XmlStreamWriter, CompleteData)) {
                    qDebug() << "CSWVersionList::Write failed. WriteSWDetails failed!";
                    return false;
                }
            }
        }

    } // end of for loop
    XmlStreamWriter.writeEndElement(); // for types of SW
    return true;
}


/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam SWVersionList = CSWVersionList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CSWVersionList& SWVersionList)
{
    CSWVersionList* p_TempSWVersionList = const_cast<CSWVersionList*>(&SWVersionList);

    if (!p_TempSWVersionList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CSWVersionList::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the QDataStream
 *  \iparam SWVersionList = CSWVersionList class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CSWVersionList& SWVersionList)
{
    if (!SWVersionList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CSWVersionList::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceSWVersionList = CSWVersionList class object
 *
 *  \return CSWVersionList Class Object
 */
/****************************************************************************/
CSWVersionList& CSWVersionList::operator=(const CSWVersionList& SourceSWVersionList)
{
    // make sure not same object
    if (this != &SourceSWVersionList)
    {
        QByteArray TempByteArray;
        CSWVersionList* p_TempSWVersionList = const_cast<CSWVersionList*>(&SourceSWVersionList);
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        DataStream << *p_TempSWVersionList;
        (void)DataStream.device()->reset(); //to avoid lint-534
//        bool TempVerificationMode = GetDataVerificationMode();
//        if (TempVerificationMode) {
//            SetDataVerificationMode(false);
//        }
        DataStream >> *this;
//        if (TempVerificationMode) {
//            SetDataVerificationMode(true);
//        }
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Reads the data and stores in CSWVersionList from the file
 *
 *  \iparam Filename = Name of the Input File
 *
 *  \return  Successful (true) or not (false)
 */
/****************************************************************************/
bool CSWVersionList::Read(QString Filename)
{
    //check if file exists
    if (!QFile::exists(Filename)) {
        qDebug("File doesn't exists");
        return false;
    }
    bool Result = true;

    if (m_DataVerificationMode) {
//        QWriteLocker locker(mp_ReadWriteLock);
//        // create instance of CSWVersionList for verification
//        CSWVersionList* p_EC_Verification = new CSWVersionList();
//        *p_EC_Verification = *this;
//        // disable verification in clone
//        p_EC_Verification->SetDataVerificationMode(false);

//        // execute required action (Read) in clone
//        Result = true;
//        if (!p_EC_Verification->Read(Filename)) {
//            Result = false;
//        } else {
//            // now check new content => call all active verifiers
//            if (DoLocalVerification(p_EC_Verification)) {
//                // if content ok, clone backwards
//                *this = *p_EC_Verification;
//            } else {
//                Result = false;
//            }
//        }
//        // delete test clone
//        delete p_EC_Verification;

    } else {
//        QWriteLocker locker(mp_ReadWriteLock);

        m_Filename = "UNDEFINED";
        QFile File (Filename);
        if (!File.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "open file failed in Read: " << Filename;
            return false;
        }
        if (!DeserializeContent(File, false)) {
            qDebug() << "### CSWVersionList::Read failed for file: " << Filename;
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
 *  \brief Gets the SWDetails pointer by index from List
 *
 *  \iparam Index =  Required Index number in the list
 *
 *  \return CSWDetails pointer
 */
/****************************************************************************/
const CSWDetails* CSWVersionList::GetSWDetails(const unsigned int Index)
{
    if (Index < (unsigned int)m_SWNameList.count()) {
        QString Key = m_SWNameList.value(Index);
        CSWDetails* p_SWDetails = m_SWDetailsList.value(Key, NULL);
        return p_SWDetails;
    } else {
        return NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief Deep Copies the SWDetails content SWDetails Instance by Index
 *
 *  \iparam Index =  Required Index number in the list
 *  \iparam SWDetails =  CSWDetails Instance
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::GetSWDetails(const unsigned int Index, CSWDetails& SWDetails)
{
    if (Index < (unsigned int)m_SWNameList.count()) {
        QString SWDetailsID = m_SWNameList.value(Index);
        if (m_SWDetailsList.contains(SWDetailsID)) {
            SWDetails = *m_SWDetailsList.value(SWDetailsID); // this give the exception if not copied
            return true;
        }
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief Deep Copies the SWDetails content SWDetails Instance by SWDetails ID
 *
 *  \iparam SWName = name of the Software
 *  \iparam SWDetails =  CSWDetails Instance
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::GetSWDetails(const QString SWName, CSWDetails& SWDetails)
{
    if (m_SWDetailsList.contains(SWName)) {
        SWDetails = *m_SWDetailsList.value(SWName);
        return true;
    } else {
        return false;
    }
}

///****************************************************************************/
///*!
// *  \brief Checks with ID whether specific SWDetails exists
// *
// *  \iparam SWName = name of the SW
// *
// *  \return True or False
// */
///****************************************************************************/
//bool CSWVersionList::SWDetailsExists(const QString SWName)
//{
//    return m_SWDetailsList.contains(SWName);
//}

/****************************************************************************/
/*!
 *  \brief Adds the  new SWDetails to the list
 *
 *  \iparam p_SWDetails =  CSWDetails Instance
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::AddSWDetails(const CSWDetails* p_SWDetails)
{
    if (p_SWDetails == NULL) return false;

    QString SWName = const_cast<CSWDetails*>(p_SWDetails)->GetSWName();
    if (m_SWDetailsList.contains(SWName)) {
        // there is already a SWDetails with the given SWDetailsID
        // => use UpdateSWDetails instead
        //    or call DeleteSWDetails before AddSWDetails
        return false;
    }

    // Here Copy constructor cannot be used ,
    CSWDetails *p_TempSWDetails = new CSWDetails();
    *p_TempSWDetails = *p_SWDetails;

    bool Result = true;
    if (m_DataVerificationMode) {
        ErrorHash_t ErrorHash;
        CSWVersionList* p_SWVL_Verification = new CSWVersionList();

        // first lock current state for reading
        {   // code block defined for QReadLocker.

            // create clone from current state
            *p_SWVL_Verification = *this;

            // disable verification in clone
            p_SWVL_Verification->SetDataVerificationMode(false);

            // execute required action (AddSWDetails) in clone
            Result = p_SWVL_Verification->AddSWDetails(p_TempSWDetails);
            delete p_TempSWDetails;
            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_SWVL_Verification);
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_SWVL_Verification;
            Result = true;
        }
        // delete test clone
        delete p_SWVL_Verification;

    } else {

        m_SWDetailsList.insert(SWName, p_TempSWDetails);
        m_SWNameList.append(SWName);

        Result = true;
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Updates the existing SWDetails in the list
 *
 *  \iparam p_SWDetails =  CSWDetails Instance
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::UpdateSWDetails(const CSWDetails* p_SWDetails)
{
    if (p_SWDetails == NULL) return false;

    QString SWName = const_cast<CSWDetails*>(p_SWDetails)->GetSWName();
    if (!m_SWDetailsList.contains(SWName)) {
        // there is no SWDetails with the given SWDetailsID
        return false;
    }

    bool Result = false;
    if (m_DataVerificationMode) {
        ErrorHash_t ErrorHash;
        CSWVersionList* p_SWVL_Verification = new CSWVersionList();

        // first lock current state for reading
        {   // code block defined for QReadLocker.

            // create clone from current state
            *p_SWVL_Verification = *this;

            // disable verification in clone
            p_SWVL_Verification->SetDataVerificationMode(false);

            // execute required action (UpdateSWDetails) in clone
            Result = p_SWVL_Verification->UpdateSWDetails(p_SWDetails);
            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_SWVL_Verification);
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_SWVL_Verification;
            Result = true;
        }

        // delete test clone
        delete p_SWVL_Verification;

    } else {
        *m_SWDetailsList[SWName] = *p_SWDetails;
        Result = true;
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Deletes the existing SW Details from the list
 *
 *  \iparam SWName = Name of the software
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::DeleteSWDetails(const QString SWName)
{
    try {
        if (m_SWDetailsList.contains(SWName)) {
            //get SWName from SWDetailsList and free memory
            delete m_SWDetailsList.value(SWName);
            //remove pointer to SWName from SWDetailsList
            m_SWDetailsList.remove(SWName);

            //remove SWName from Name list
            int MatchIndex = -1;
            for (int i = 0; i < m_SWNameList.count(); i++) {
                if (m_SWNameList[i] == SWName) {
                    MatchIndex = i;
                    break;
                }
            }
            // MatchIndex MUST never be -1 !!!
            Q_ASSERT(MatchIndex != -1);
            m_SWNameList.removeAt(MatchIndex);

            return true;
        } else {
            qDebug() << " The SW Name doesn't contains in the list ";
            // there is no SW name matched in the list with the given SWName
            return false;
        }
    }
    catch (...) {
        qDebug() << "### exception in CSWVersionList::DeleteSWDetail(const QString SWName)";
        return false;
    }
}


/****************************************************************************/
/*!
 *  \brief Deletes the existing SW details from the list
 *
 *  \iparam Index =  Required Index number in the list
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::DeleteSWDetails(const unsigned int Index)
{
    if (Index < (unsigned int)m_SWNameList.count()) {
        QString SWName = m_SWNameList.value(Index);
        return DeleteSWDetails(SWName);
    } else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Deletes  all the existing SW details from the list
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWVersionList::DeleteAllSWDetails()
{
    while (m_SWDetailsList.size() > 0) {
        if (DeleteSWDetails(0) == false) {
            return false;
        }
    }
    // clear the list
    m_SWDetailsList.clear();
    m_SWNameList.clear();

    return true;
}

/****************************************************************************/
/*!
 *  \brief Initilisation function for CSWVersionList
 *
 *  \return void
 */
/****************************************************************************/
void CSWVersionList::Init()
{
    if (!DeleteAllSWDetails()) {
        //throw error ...
        THROWARG(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    }

}

}  // namespace DataManager

