/****************************************************************************/
/*! \file  Platform/Master/Components/DataManager/Containers/UserSettings/Source/UserSettingsInterface.cpp
 *
 *  \brief Implementation file for class CUserSettingsInterface.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-04-23
 *  $Author:    $ Raju123, Ramya GJ
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

#include <QReadLocker>
#include <QWriteLocker>
#include <QDebug>
#include <QFile>
#include "DataManager/Containers/UserSettings/Include/UserSettingsVerifier.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/EventObject.h"
#include <QDataStream>


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CUserSettingsInterface::CUserSettingsInterface()
    : mp_ReadWriteLock(NULL)
    , m_DataVerificationMode(true)
    , mp_UserSettings(NULL)
    , m_FileName("")
    , m_WorkStationMode(false)
    , m_Version(1)
{
    // create the Read write lock for threads
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    mp_UserSettings = new CHimalayaUserSettings();
}
/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam p_UserSettings = Instance of the UserSettingsInterface class
 *
 *  \return
 */
/****************************************************************************/
CUserSettingsInterface::CUserSettingsInterface(CHimalayaUserSettings *p_UserSettings)
{
    // create the Read write lock for threads

    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    mp_UserSettings = p_UserSettings;
    m_DataVerificationMode = true;
    m_FileName = "";
    m_WorkStationMode = false;
    m_Version = 1;
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam UserSettingsInterface = Instance of the UserSettingsInterface class
 *
 *  \return
 */
/****************************************************************************/
CUserSettingsInterface::CUserSettingsInterface(const CUserSettingsInterface& UserSettingsInterface)
    : CDataContainerBase()
    , mp_ReadWriteLock(NULL)
    , m_DataVerificationMode(true)
    , mp_UserSettings(NULL)
    , m_FileName("")
    , m_WorkStationMode(false),
      m_Version(1)
{
    // create the Read write lock for threads
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    mp_UserSettings = new CHimalayaUserSettings();
    CopyFromOther(UserSettingsInterface);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \iparam Other = Instance of the CDataStationList class
.*  \note  Method for internal use only
 *
 *  \return
 */
/****************************************************************************/
void CUserSettingsInterface::CopyFromOther(const CUserSettingsInterface &Other)
{
    //QReadWriteLock is not copied. We use the existing lock object
    CUserSettingsInterface &OtherSettingsList = const_cast<CUserSettingsInterface &>(Other);
    m_FileName  = OtherSettingsList.GetFilename();
    m_Version = OtherSettingsList.GetVersion();
    m_WorkStationMode = OtherSettingsList.GetWorkStationMode();
    m_DataVerificationMode = OtherSettingsList.GetDataVerificationMode();
    mp_UserSettings->CopyFromOther(*(OtherSettingsList.GetUserSettings()));
}
/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CUserSettingsInterface::~CUserSettingsInterface()
{
    if (mp_ReadWriteLock != NULL) {
    try {
    delete mp_ReadWriteLock;
    }
    CATCHALL_DTOR();
    mp_ReadWriteLock = NULL;
    }

    try {
    delete mp_UserSettings;
    }
    CATCHALL_DTOR();
    mp_UserSettings = NULL;
}

/****************************************************************************/
/*!
 *  \brief  Updates user settings
 *
 *  \iparam p_UserSettings = Pointer to a User settings
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CUserSettingsInterface::UpdateUserSettings(const CUserSettings* p_UserSettings)
{
    if (p_UserSettings == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_NULL_PTR, Global::GUI_MSG_BOX);
        m_ErrorMap.insert(EVENT_DM_NULL_PTR, Global::tTranslatableStringList());
        SetErrorList(&m_ErrorMap);
        return false;
    }

    bool Result = false;
    // check the verification flags
    if (m_DataVerificationMode) {
        // create the temporary CDataRackList class object
        CUserSettingsInterface* p_USI_Verification = new CUserSettingsInterface();

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_USI_Verification = *this;

            // disable verification in clone
            p_USI_Verification->SetDataVerificationMode(false);

            // execute required action (UpdateRack) in clone
            Result = p_USI_Verification->UpdateUserSettings(p_UserSettings);

            if (Result) {
                Result = DoLocalVerification(p_USI_Verification);
                ListOfErrors_t ErrorList = p_USI_Verification->GetErrorList();
                if (!ErrorList.isEmpty()) {
                    // If the control reaches here means Error hash is empty
                    // Considering only the first element in Hash since
                    // verfier can atmost add only one Hash has to the error list
                    m_ErrorMap = *(ErrorList.first());
                    SetErrorList(&m_ErrorMap);
                }
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_USI_Verification;
            Result = true;
        }
        // delete test clone
        delete p_USI_Verification;

    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);
        if (mp_UserSettings) {
            // replace the old class object with updated class object
            *mp_UserSettings = *p_UserSettings;
            Result = true;
        }
        else {
            Result = false;
        }
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Get the user settings
 *
 *  \iparam CopySettings = Flag for copying settings (either deep or shallow copy)
 *
 *  \return user settings class
 */
/****************************************************************************/
CUserSettings* CUserSettingsInterface::GetUserSettings(bool CopySettings)
{
    CHECKPTR_RETURN(mp_UserSettings, NULL);

    if (!CopySettings) {
        return mp_UserSettings;
    }
    else {
        // create the new user settings
        CUserSettings* p_UserSettings = new CUserSettings();
        // Do a deep copy
        *p_UserSettings = *mp_UserSettings;
        return p_UserSettings;
    }
}

/****************************************************************************/
/*!
 *  \brief Writes from the CUserSettingsInterface object to a IODevice.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserSettingsInterface::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    if (!mp_UserSettings) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_NULL_PTR, Global::GUI_MSG_BOX);
        m_ErrorMap.insert(EVENT_DM_NULL_PTR, Global::tTranslatableStringList());
        SetErrorList(&m_ErrorMap);
        return false;
    }
    //qDebug()<<"COMPLETE DATA"<<CompleteData;
    QXmlStreamWriter XmlStreamWriter; ///< Xml stream writer object to write the Xml contents in a file

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);

    // start the XML Document
    XmlStreamWriter.writeStartDocument();

    // write the documnet type declaration
    XmlStreamWriter.writeDTD("<!DOCTYPE Settings>");
    XmlStreamWriter.writeStartElement("Settings");
    QString StringValue; ///< to store the version number

    // write version number
    (void) StringValue.setNum(GetVersion());  //to suppress lint-534
    XmlStreamWriter.writeAttribute("Version", StringValue);


    if (!mp_UserSettings->SerializeContent(XmlStreamWriter, CompleteData)) {
        qDebug() << "CDataRackList::Write failed. Write Racks failed!";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_SERIALIZE_FAILED, Global::tTranslatableStringList() << "UserSettings", true);
        return false;
    }
    // store the Class temporary data
    if(CompleteData) {
        //Need to be added the temporary members
        XmlStreamWriter.writeStartElement("ClassTemporaryData");

        XmlStreamWriter.writeAttribute("FileName", GetFilename());

        // write the workstation mode in the xml file
        if (GetWorkStationMode()) {
            XmlStreamWriter.writeAttribute("WorkStationMode", "true");
        }
        else {
            XmlStreamWriter.writeAttribute("WorkStationMode", "false");
        }

        // write the verification mode
        if (GetDataVerificationMode()) {
            XmlStreamWriter.writeAttribute("VerificationMode", "true");
        }
        else {
            XmlStreamWriter.writeAttribute("VerificationMode", "false");
        }

        XmlStreamWriter.writeEndElement(); // for ClassTemporaryData
    }

    // write root element end (Settings)
    XmlStreamWriter.writeEndElement();
    // write enddocument
    XmlStreamWriter.writeEndDocument();

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CUserSettingsInterface object.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserSettingsInterface::DeserializeContent(QIODevice& IODevice ,bool CompleteData)
{
    if (!mp_UserSettings) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_NULL_PTR, Global::GUI_MSG_BOX);
        m_ErrorMap.insert(EVENT_DM_NULL_PTR, Global::tTranslatableStringList());
        SetErrorList(&m_ErrorMap);
        return false;
    }
    QXmlStreamReader XmlStreamReader;

    XmlStreamReader.setDevice(&IODevice);
    // look for node <Settings>
    if (!Helper::ReadNode(XmlStreamReader, "Settings")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: Settings";
        return false;
    }

    // Read attribute Version
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        return false;
    }
    SetVersion(XmlStreamReader.attributes().value("Version").toString().toInt());

    if (!mp_UserSettings->DeserializeContent(XmlStreamReader, CompleteData)) {
        qDebug() << "CDataRackList::Read failed. Read Racks failed!";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_DESERIALIZE_FAILED, Global::tTranslatableStringList() << "UserSettings", true);
        return false;
    }

    //======NODE=======Temporary Data Variables=========================
    if (CompleteData)
    {
        while ((!XmlStreamReader.atEnd()) &&
               ((XmlStreamReader.name() != "ClassTemporaryData") || (XmlStreamReader.tokenType() != QXmlStreamReader::EndElement)))
        {
            //            qDebug() << "CUserSettings::DeserializeContent, checking" << XmlStreamReader.name() << XmlStreamReader.tokenType();

            if (XmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
            {
                if (XmlStreamReader.name() == "ClassTemporaryData")
                {
                    // File name
                    if (!XmlStreamReader.attributes().hasAttribute("FileName")) {
                        qDebug() << "### attribute <FileName> is missing => abort reading";
                        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() <<"FileName", true);
                        return false;
                    }
                    m_FileName = XmlStreamReader.attributes().value("FileName").toString();

                    // WorkStation mode
                    if (!XmlStreamReader.attributes().hasAttribute("WorkStationMode")) {
                        qDebug() << "### attribute <WorkStationMode> is missing => abort reading";
                        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() <<"WorkStationMode", true);
                        return false;
                    } else {
                        bool Value = false;
                        if (XmlStreamReader.attributes().value("WorkStationMode").toString().toUpper() == "TRUE") {
                            Value = true;
                        }
                        SetWorkStationMode(Value);
                    }

                    // VerificationMode
                    if (!XmlStreamReader.attributes().hasAttribute("VerificationMode")) {
                        qDebug() << "### attribute <VerificationMode> is missing => abort reading";
                        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() <<"VerificationMode", true);
                        return false;
                    } else {
                        bool Value = false;
                        if (XmlStreamReader.attributes().value("VerificationMode").toString().toUpper() == "TRUE") {
                            Value = true;
                        }
                        SetDataVerificationMode(Value);
                    }

                }
            }
            XmlStreamReader.readNextStartElement();
        }

    }
    //XmlStreamReader.device()->reset();
    //    qDebug()<<"User Settings Interface Deserialize Content"<<XmlStreamReader.device()->readAll();
    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the data and stores in CUserSettingsInterface from the file
 *
 *  \iparam FileName = Name of the Input File
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserSettingsInterface::Read(QString FileName)
{
    //check if file exists
    if (!QFile::exists(FileName)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_FILE_NOT_EXISTS, Global::tTranslatableStringList() << "UserSettings", true);
        return false;
    }

    bool Result = true; ///< store the result flag in the local variable

    if (m_DataVerificationMode) {
        QWriteLocker locker(mp_ReadWriteLock);

        // create the user settings class
        CUserSettingsInterface *p_UserSettings_Verification = new CUserSettingsInterface();
        // reset the data verification flag
        p_UserSettings_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        // read the data from the file
        if (!p_UserSettings_Verification->Read(FileName)){
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_SETTINGS_XML_READ_FAILED);
            Result = false;
        }
        else {
            // verify the data
            if (DoLocalVerification(p_UserSettings_Verification)) {
                // if content ok, clone backwards
                *this = *p_UserSettings_Verification;
                Result = true;
            }
            else {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_SETTINGS_VERIFICATION_FAILED);
                Result = false;
            }
        }
        // delete test clone
        delete p_UserSettings_Verification;

    } else {
        QWriteLocker locker(mp_ReadWriteLock);

        // clear content and add default values
        //SetDefaultAttributes();

        // Initialise the m_Filename to a known string "UNDEFINED"
        m_FileName = "UNDEFINED";

        // create the file
        QFile File(FileName);
        if (!File.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "open file failed in Read: " << FileName;
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_FILE_OPEN_FAILED, Global::tTranslatableStringList() <<  "UserSettings.xml", true);
            return false;
        }

        if (!DeserializeContent(File, false)) {
            qDebug() << "### CUserSettingsInterface::Read failed for file: " << FileName;
            return false;
        }
        //Now initialize the filename member since Read is succcess
        m_FileName = FileName;
        // close the file
        File.close();
        //return true;
        Result = true;
    }

    return Result;
}
/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam USInterface = CUserSettingsInterface class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CUserSettingsInterface& USInterface)
{
    // remove the constant by using type cast opeator
    CUserSettingsInterface* p_TempUSInterface = const_cast<CUserSettingsInterface*>(&USInterface);

    if (!p_TempUSInterface->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CUserSettingsInterface::Operator Streaming (SerializeContent) failed.";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "UserSettings", true);
    }

    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam USInterface = CUserSettingsInterface class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CUserSettingsInterface& USInterface)
{
    // deserialize the content from the data stream
    if (!USInterface.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CUserSettingsInterface::Operator Streaming (DeSerializeContent) failed because it does not have any Data to stream.";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "UserSettings", true);
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceSettings = CUserSettingsInterface class object
 *
 *  \return CUserSettingsInterface Class Object
 */
/****************************************************************************/
CUserSettingsInterface& CUserSettingsInterface::operator=(const CUserSettingsInterface& SourceSettings)
{
    // make sure not same object
    if (this != &SourceSettings)
    {
        CopyFromOther(SourceSettings);
    }
    return *this;
}


} // end namespace DataManager
