/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/SWVersions/Source/SWVersionList.cpp
 *
 *  \brief Implementation file for class CSWVersionList.
 *         This class reads the "SW_version.xml" file.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-07
 *  $Author:    $ Raju, Ramya GJ
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
#include <Global/Include/EventObject.h>
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/GlobalEventCodes.h"

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
    CopyFromOther(SWVersionList);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \iparam Other = Instance of the CSWVersionList class
.*  \note  Method for internal use only
 *
 *  \return
 */
/****************************************************************************/
void CSWVersionList::CopyFromOther(const CSWVersionList &Other)
{
    //QReadWriteLock is not copied. We use the existing lock object
    //m_DataVerificationMode is not copied. we use existing state

    CSWVersionList &OtherSWVersionList = const_cast<CSWVersionList &>(Other);
    m_Filename  = OtherSWVersionList.GetFilename();
    m_Version = OtherSWVersionList.GetVersion();
    m_SWReleaseVersion  = OtherSWVersionList.GetSWReleaseVersion();
    m_SWReleaseDate = OtherSWVersionList.GetSWReleaseDate();

    //qDebug() <<"SWDetailList Keys"<< m_SWDetailsList.keys().toSet();
    //qDebug()<<"Other SWDetailList keys" << Other.GetSWDetailIdList().toSet();
    QSet<QString> SWDetailsInOurList(static_cast< QSet<QString> >(m_SWDetailsList.keys().toSet()));
    QSet<QString> SWDetailsInOtherList(Other.GetSWDetailIdList().toSet());
    //we get a list of SWDetails that are not there in new list but present in our list, they are to be deleted.
    QSet<QString> SWDetailsToDelete(SWDetailsInOurList.subtract(SWDetailsInOtherList));
    QSetIterator<QString> Itr(SWDetailsToDelete);
    while (Itr.hasNext()) {
        DeleteSWDetails(Itr.next());
    }

    //Recreate SWDetails in our list
    SWDetailsInOurList = m_SWDetailsList.keys().toSet();
    //Update SWDetails which are present in our list and other list
    QSet<QString> SWDetailsToUpdate(static_cast< QSet<QString> >(SWDetailsInOurList.intersect(SWDetailsInOtherList)));
    QSetIterator<QString> UpdateItr(SWDetailsToUpdate);
    while (UpdateItr.hasNext()) {
        const QString SWDetailID = UpdateItr.next();
        const CSWDetails *p_OtherSWDetail = Other.GetSWDetails(SWDetailID);
        CSWDetails *p_OurSWDetail = GetSWDetails(SWDetailID);
        if (p_OtherSWDetail && p_OurSWDetail) {
            //There exist a SWDetail with similar ID in the other SWDetaillist.
            //update our SWDetail with values from other.
             *p_OurSWDetail = *p_OtherSWDetail;
        }
    }

    //Finally create SWDetails which are not present in our list but in other list.
    QSet<QString> SWDetailsToCreate(SWDetailsInOtherList.subtract(SWDetailsInOurList));
    QSetIterator<QString> CreateItr(SWDetailsToCreate);
    while (CreateItr.hasNext()) {
        const CSWDetails *p_OtherSWDetail = Other.GetSWDetails(CreateItr.next());
        AddSWDetailsWithoutVerification(p_OtherSWDetail);
    }
}
/****************************************************************************/
/*!
 *  \brief  Returns the ID's of all SWDetail present in the list
 *
 *  \return QStringList = list of SWDetail Ids
 */
/****************************************************************************/
QStringList CSWVersionList::GetSWDetailIdList() const
{
    return m_SWDetailsList.keys();
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

    XmlStreamWriter.writeEndElement(); //Release
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

    if (!WriteSWDetails(XmlStreamWriter, INITSCRPITS, CompleteData)) {
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
        return false;
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

    if (!ReadSWDetails(XmlStreamReader, "InitScripts", CompleteData)) {
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
                else if (NodeName == "Master_SW"){
                    p_SWDetails->SetSWType(MASTERSOFTWARE);
                }
                else {
                    p_SWDetails->SetSWType(INITSCRPITS);
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
    else if (SWType == INITSCRPITS) {
        XmlStreamWriter.writeStartElement("InitScripts");
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
        //DeleteAllSWDetails();
        CopyFromOther(SourceSWVersionList);
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
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_FILE_NOT_EXISTS, Global::tTranslatableStringList() << Filename, true);
        qDebug("File doesn't exists");
        return false;
    }
    bool Result = true;

    if (m_DataVerificationMode) {

    } else {
        //        QWriteLocker locker(mp_ReadWriteLock);

        m_Filename = "UNDEFINED";
        QFile File (Filename);
        if (!File.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "open file failed in Read: " << Filename;
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_FILE_OPEN_FAILED, Global::tTranslatableStringList() <<  Filename, true);
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
 *  \brief Add SWDetails to SWDetails List. Doesnt do any verification.
 *
 *  \iparam p_SWDetails = Instance of CSWDetails class
 *
 *  \warning  To be used from Assignment operator only
 */
/****************************************************************************/
void CSWVersionList::AddSWDetailsWithoutVerification(const CSWDetails *p_SWDetails)
{
    if (p_SWDetails) {
        QString ID = const_cast<CSWDetails*>(p_SWDetails)->GetSWName();
        // Here Copy constructor cannot be used ,
        CSWDetails *p_TempSWDetails = new CSWDetails();
        // Make a deep copy
        *p_TempSWDetails = *p_SWDetails;
        m_SWDetailsList.insert(ID, p_TempSWDetails);
        m_SWNameList.append(ID);
    }
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
    if (p_SWDetails == NULL) {
        return false;
    }

    QString SWName = const_cast<CSWDetails*>(p_SWDetails)->GetSWName();
    if (m_SWDetailsList.contains(SWName)) {
        // there is already a SWDetails with the given SWDetailsID
        // => use UpdateSWDetails instead
        //    or call DeleteSWDetails before AddSWDetails
        qDebug() <<"The SW Component ### already exists in the SWlist"
                << SWName;

        m_ErrorMap.insert(EVENT_SWVERSION_SW_COMPONENT_NOT_UNIQUE,
                          Global::tTranslatableStringList() << SWName);
        Global::EventObject::Instance().RaiseEvent(EVENT_SWVERSION_SW_COMPONENT_NOT_UNIQUE,
                                                   Global::tTranslatableStringList() << SWName,
                                                   true,
                                                   Global::GUI_MSG_BOX);
        SetErrorList(&m_ErrorMap);
        return false;
    }

    // Here Copy constructor cannot be used ,
    CSWDetails *p_TempSWDetails = new CSWDetails();
    *p_TempSWDetails = *p_SWDetails;

    bool Result = true;
    if (m_DataVerificationMode) {
        ErrorMap_t ErrorHash;
        CSWVersionList* p_SWVL_Verification = new CSWVersionList();

        // first lock current state for reading
        {   // code block defined for QReadLocker.

            // create clone from current state
            *p_SWVL_Verification = *this;

            // disable verification in clone
            p_SWVL_Verification->SetDataVerificationMode(false);

            // execute required action (AddSWDetails) in clone
            Result = p_SWVL_Verification->AddSWDetails(p_TempSWDetails);

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
        else {
            //Store errors.
            // Since we are going to delete p_SWVL_Verification
            // We need to store the errors generated by it.
            // For now Errors are not copied by assignment or copy constructors.
            ListOfErrors_t ErrorList = p_SWVL_Verification->GetErrorList();
            if (!ErrorList.isEmpty()) {
                // Control reaches here means Error list is not empty.
                // Considering only the first element in list since
                // verfier can atmost add only one Hash has to the error list
                m_ErrorMap = *(ErrorList.first());
                SetErrorList(&m_ErrorMap);
            }
        }

        // delete test clone
        delete p_SWVL_Verification;

        delete p_TempSWDetails;

    } else {

        m_SWDetailsList.insert(SWName, p_TempSWDetails);
        m_SWNameList.append(SWName);

        //lint -esym(593, p_TempSWDetails)
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
    bool Result = false;

    try {
        CHECKPTR(p_SWDetails);

        QString SWName = const_cast<CSWDetails*>(p_SWDetails)->GetSWName();
        if (!m_SWDetailsList.contains(SWName)) {
            // there is no SWDetails with the given SWDetailsID
            qDebug() <<"there is no SWDetails with the given SWDetailsID"
                    << SWName;

            m_ErrorMap.insert(EVENT_SWVERSION_SW_COMPONENT_DOESNOT_EXISTS,
                              Global::tTranslatableStringList() << SWName);
            Global::EventObject::Instance().RaiseEvent(EVENT_SWVERSION_SW_COMPONENT_DOESNOT_EXISTS,
                                                       Global::tTranslatableStringList() << SWName,
                                                       true,
                                                       Global::GUI_MSG_BOX);
            SetErrorList(&m_ErrorMap);
            return false;
        }


        if (m_DataVerificationMode) {
            ErrorMap_t ErrorHash;
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
            else {
                //Store errors.
                // Since we are going to delete p_SWVL_Verification
                // We need to store the errors generated by it.
                // For now Errors are not copied by assignment or copy constructors.
                ListOfErrors_t ErrorList = p_SWVL_Verification->GetErrorList();
                if (!ErrorList.isEmpty()) {
                    // Control reaches here means Error list is not empty.
                    // Considering only the first element in list since
                    // verfier can atmost add only one Hash has to the error list
                    m_ErrorMap = *(ErrorList.first());
                    SetErrorList(&m_ErrorMap);
                }
            }

            // delete test clone
            delete p_SWVL_Verification;

        } else {
            *m_SWDetailsList[SWName] = *p_SWDetails;
            Result = true;
        }

        return Result;
    }
    CATCHALL();

    return false;
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
            if(MatchIndex != -1) {
                m_SWNameList.removeAt(MatchIndex);

                return true;
            }
            else {
                return false;
            }
        }
        else {
            qDebug() << " The SW Name doesn't contains in the list ";
            // there is no SW name matched in the list with the given SWName

            m_ErrorMap.insert(EVENT_SWVERSION_SW_COMPONENT_DOESNOT_EXISTS,
                              Global::tTranslatableStringList() << SWName);
            Global::EventObject::Instance().RaiseEvent(EVENT_SWVERSION_SW_COMPONENT_DOESNOT_EXISTS,
                                                       Global::tTranslatableStringList() << SWName,
                                                       true,
                                                       Global::GUI_MSG_BOX);
            SetErrorList(&m_ErrorMap);
            return false;
        }


    }
    CATCHALL();

    qDebug() << "### exception in CSWVersionList::DeleteSWDetail(const QString SWName)";
    return false;
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
    bool Result = true;
    qint32 Size = m_SWDetailsList.size();
    for (qint32 I = 0; I < Size; I++) {
        if (DeleteSWDetails(0) == false) {
            Result = false;
        }
    }

    // clear the list
    m_SWDetailsList.clear();
    m_SWNameList.clear();

    return Result;
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
        qDebug() << "DeleteAllSWDetails failed";
    }

}

}  // namespace DataManager

