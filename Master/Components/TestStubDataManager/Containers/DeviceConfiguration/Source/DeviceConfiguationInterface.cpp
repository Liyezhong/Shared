/****************************************************************************/
/*! \file TestStubDataManager/Containers/DeviceConfiguration/Source/DeviceConfigurationInterface.cpp
 *
 *  \brief DeviceConfigurationInterface class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-09-04
 *   $Author:  $ Ningu
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
#include <QDomDocument>

#include "TestStubDataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h"
#include "TestStubDataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"



namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CDeviceConfigurationInterface::CDeviceConfigurationInterface() :
    mp_ReadWriteLock(NULL)
    ,m_DataVerificationMode(true)
    ,mp_DeviceConfig(NULL)
{
    // set default values
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam DeviceConfigterface = Instance of the DeviceConfigurationInterface class
 *
 *  \return
 */
/****************************************************************************/
CDeviceConfigurationInterface::CDeviceConfigurationInterface(const CDeviceConfigurationInterface& DeviceConfigterface) : CDataContainerBase()
{
    // remove constant cast from the object
    CDeviceConfigurationInterface* p_DevConfigInterface = const_cast<CDeviceConfigurationInterface*>(&DeviceConfigterface);
    // set the data to default values
    SetDefaultAttributes();
    m_DataVerificationMode = true;
    // create deep copy of the object
    *this = *p_DevConfigInterface;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDeviceConfigurationInterface::~CDeviceConfigurationInterface()
{
    try {
        delete mp_ReadWriteLock;
    }
    catch(...) {
        //to please PClint
    }
    // clear the device configuration

    try {
        delete mp_DeviceConfig;
    }
    catch(...) {
        //to please PClint
    }
}

/****************************************************************************/
/*!
 *  \brief Set the default values of the local variables
 */
/****************************************************************************/
void CDeviceConfigurationInterface::SetDefaultAttributes()
{
    // create the Read write lock for threads
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    m_DataVerificationMode = true;
    mp_DeviceConfig = new CDeviceConfiguration();
}

/****************************************************************************/
/*!
 *  \brief  Updates Device Configuration
 *
 *  \iparam p_DeviceConfig = Pointer to a Device Configuration
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDeviceConfigurationInterface::UpdateDeviceConfiguration(const CDeviceConfiguration* p_DeviceConfig)
{
    try {
        CHECKPTR(p_DeviceConfig);
    } catch(const Global::Exception &E) {
        // and send error message
        Global::EventObject::Instance().RaiseException(E);
        return false;
    }

    bool Result = false;
    // check the verification flags
    if (m_DataVerificationMode) {
        ErrorHash_t ErrorHash;
        // create the temporary CDataRackList class object
        CDeviceConfigurationInterface* p_DCI_Verification = new CDeviceConfigurationInterface();

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_DCI_Verification = *this;

            // disable verification in clone
            p_DCI_Verification->SetDataVerificationMode(false);

            // execute required action (UpdateRack) in clone
            Result = p_DCI_Verification->UpdateDeviceConfiguration(p_DeviceConfig);

            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_DCI_Verification);
                ListOfErrors_t ErrorList = p_DCI_Verification->GetErrorList();
                if (!ErrorList.isEmpty()) {
                    // If the control reaches here means Error hash is empty
                    // Considering only the first element in Hash since
                    // verfier can atmost add only one Hash has to the error list
                    m_ErrorHash = *(ErrorList.first());
                    SetErrorList(&m_ErrorHash);
                }
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_DCI_Verification;
            Result = true;
        }
        // delete test clone
        delete p_DCI_Verification;

    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);
        // replace the old class object with updated class object
        *mp_DeviceConfig = *p_DeviceConfig;
        Result = true;
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Get the Device Configuration
 *  \iparam CopyConfiguration = Flag for copying Configuration

 *  \return DeviceConfiguration class
 */
/****************************************************************************/
CDeviceConfiguration* CDeviceConfigurationInterface::GetDeviceConfiguration(bool CopyConfiguration)
{
    try {
        CHECKPTR(mp_DeviceConfig);
    } catch(const Global::Exception &E) {
        // and send error message
        Global::EventObject::Instance().RaiseException(E);
        return false;
    }
    if (!CopyConfiguration) {
        return mp_DeviceConfig;
    }
    else {
        // create the new Device Configuration
        CDeviceConfiguration* p_DeviceConfig = new CDeviceConfiguration();
        // Do a deep copy
        *p_DeviceConfig = *mp_DeviceConfig;
        return p_DeviceConfig;
    }
}

/****************************************************************************/
/*!
 *  \brief Writes from the CDeviceConfigurationInterface object to a IODevice.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDeviceConfigurationInterface::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    QXmlStreamWriter XmlStreamWriter; ///< Xml stream writer object to write the Xml contents in a file

    try {
        CHECKPTR(mp_DeviceConfig);
    } catch(const Global::Exception &E) {
        // and send error message
        Global::EventObject::Instance().RaiseException(E);
        return false;
    }


//    QFile file("TestDevice2.xml");
//    if (!file.open(QIODevice::WriteOnly))
//    {
//        return false;
//    }
//    XmlStreamWriter.setDevice(&file);

    XmlStreamWriter.setDevice(&IODevice);

    XmlStreamWriter.setAutoFormatting(true);
    //XmlStreamWriter.setAutoFormattingIndent(4);
    // start the XML Document
    XmlStreamWriter.writeStartDocument();
    XmlStreamWriter.writeDTD("<!DOCTYPE DeviceConfiguration>");
    XmlStreamWriter.writeStartElement("DeviceConfiguration");

    if (!mp_DeviceConfig->SerializeContent(XmlStreamWriter, CompleteData)) {
        qDebug() << "CDeviceConfigurationInterface::SerializeContent failed to serialize";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_SERIALIZE_FAILED, Global::tTranslatableStringList() << "DeviceConfiguartion", true);
        return false;
    }
    //XmlStreamWriter.writeEndElement();

    // store the Class temporary data
    if(CompleteData) {
        //Need to be added the temporary members
        XmlStreamWriter.writeStartElement("ClassTemporaryData");

        XmlStreamWriter.writeAttribute("FileName", GetFilename());

        // write the verification mode
        if (GetDataVerificationMode()) {
            XmlStreamWriter.writeAttribute("VerificationMode", "true");
        }
        else {
            XmlStreamWriter.writeAttribute("VerificationMode", "false");
        }

        XmlStreamWriter.writeEndElement(); // for ClassTemporaryData
    }

    // write root element end
    XmlStreamWriter.writeEndElement();  // DeviceConfiguration
    // write enddocument
    XmlStreamWriter.writeEndDocument();



    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CDeviceConfigurationInterface object.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDeviceConfigurationInterface::DeserializeContent(QIODevice& IODevice ,bool CompleteData)
{
    QXmlStreamReader XmlStreamReader;

   try {
        CHECKPTR(mp_DeviceConfig);
    } catch(const Global::Exception &E) {
        // and send error message
        Global::EventObject::Instance().RaiseException(E);
        return false;
    }

    XmlStreamReader.setDevice(&IODevice);

    if (!mp_DeviceConfig->DeserializeContent(XmlStreamReader, CompleteData)) {
        qDebug() << "CDeviceConfigurationInterface:DeserializeContent. Read failed!";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_DESERIALIZE_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration", true);
        return false;
    }

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData)
    {
        while ((XmlStreamReader.name() != "ClassTemporaryData") && (!XmlStreamReader.atEnd()))
        {
            XmlStreamReader.readNextStartElement();
        }

        // File name
        if (!XmlStreamReader.attributes().hasAttribute("FileName")) {
            qDebug() << "### attribute <FileName> is missing => abort reading";
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "FileName", true);
            return false;
        }
        m_FileName = XmlStreamReader.attributes().value("FileName").toString();

         // VerificationMode
        if (!XmlStreamReader.attributes().hasAttribute("VerificationMode")) {
            qDebug() << "### attribute <VerificationMode> is missing => abort reading";
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "VerificationMode", true);
            return false;
        } else {
            bool Value = false;
            if (XmlStreamReader.attributes().value("VerificationMode").toString().toUpper() == "TRUE") {
                Value = true;
            }
            SetDataVerificationMode(Value);
        }

    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the data and stores in CDeviceConfigurationInterface from the file
 *
 *  \iparam FileName = Name of the Input File
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDeviceConfigurationInterface::Read(QString FileName)
{
    //check if file exists
    if (!QFile::exists(FileName)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_FILE_NOT_EXISTS, Global::tTranslatableStringList() << "DeviceConfiguration", true);
        return false;
    }

    bool Result = true; ///< store the result flag in the local variable

    if (m_DataVerificationMode) {
        QWriteLocker locker(mp_ReadWriteLock);

        // create the Device Configuration Interface class
        CDeviceConfigurationInterface *p_DeviceConfig_Verification = new CDeviceConfigurationInterface();
        // reset the data verification flag
        p_DeviceConfig_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        // read the data from the file
        if (!p_DeviceConfig_Verification->Read(FileName)){
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_DEVICE_CONFIG_XML_READ_FAILED, Global::tTranslatableStringList() << "", true);
            Result = false;
        }
        else {
            // verify the data
            if (DoLocalVerification(p_DeviceConfig_Verification)) {
                // if content ok, clone backwards
                *this = *p_DeviceConfig_Verification;
                Result = true;
            }
            ListOfErrors_t ErrorList = p_DeviceConfig_Verification->GetErrorList();
            if (!ErrorList.isEmpty()) {
                // If the control reaches here means Error hash is empty
                // Considering only the first element in Hash since
                // verfier can atmost add only one Hash has to the error list
                m_ErrorHash = *(ErrorList.first());
                SetErrorList(&m_ErrorHash);
            }
            else {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_DEVICE_CONFIG_VERIFICATION_FAILED, Global::tTranslatableStringList() << "", true);
                Result = false;
            }            
        }
        // delete test clone
        delete p_DeviceConfig_Verification;

    } else {
        QWriteLocker locker(mp_ReadWriteLock);
        m_FileName = "UNDEFINED";
        // clear content and add default values
        //SetDefaultAttributes();

        // create the file
        QFile File(FileName);
        if (!File.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "open file failed in Read: " << FileName;
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_FILE_OPEN_FAILED, Global::tTranslatableStringList() <<  "DeviceConfiguration XML", true);
            return false;
        }

        if (!DeserializeContent(File, false)) {
            qDebug() << "### CDeviceConfigurationInterface::Read failed for file: " << FileName;
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
 *  \iparam DCInterface = CDeviceConfigurationInterface class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CDeviceConfigurationInterface& DCInterface)
{
    // remove the constant by using type cast opeator
    CDeviceConfigurationInterface* p_TempDCInterface = const_cast<CDeviceConfigurationInterface*>(&DCInterface);

    if (!p_TempDCInterface->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CDeviceConfigurationInterface::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        //THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
        const_cast<CDeviceConfigurationInterface &>(DCInterface).m_ErrorHash.insert(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration", true);

    }

    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam DCInterface = CDeviceConfigurationInterface class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CDeviceConfigurationInterface& DCInterface)
{
    // deserialize the content from the data stream
    if (!DCInterface.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CDeviceConfigurationInterface::Operator Streaming (DeSerializeContent) failed because it does not have any Data to stream.";
        // throws an exception
        //THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
        DCInterface.m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration", true);

    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceConfiguration = CDeviceConfigurationInterface class object
 *
 *  \return CDeviceConfigurationInterface Class Object
 */
/****************************************************************************/
CDeviceConfigurationInterface& CDeviceConfigurationInterface::operator=(const CDeviceConfigurationInterface& SourceConfiguration)
{
    // make sure not same object
    if (this != &SourceConfiguration)
    {
        // create the byte array
        QByteArray* p_TempByteArray = new QByteArray();
        // create the data stream to write into a file
        QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        p_TempByteArray->clear();
        // write the data into data stream from source
        DataStream << SourceConfiguration;
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


} // end namespace DataManager

