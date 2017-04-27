/****************************************************************************/
/*! \file  Platform/Master/Components/DataManager/Containers/RCConfiguration/Source/RCConfigurationInterface.cpp
 *
 *  \brief Implementation file for class CRCConfigurationInterface.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2014-03-13
 *  $Author:    $ Ramya GJ
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
#include "DataManager/Containers/RCConfiguration/Include/RCConfigurationVerifier.h"
#include "DataManager/Containers/RCConfiguration/Include/RCConfigurationInterface.h"
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
CRCConfigurationInterface::CRCConfigurationInterface()
    : mp_ReadWriteLock(NULL)
    , m_DataVerificationMode(true)
    , mp_RCConfiguration(NULL)
    , m_FileName("RCConfiguration.xml")
{
    // create the Read write lock for threads
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    mp_RCConfiguration = new CRCConfiguration();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam RCConfigurationInterface = Instance of the RCConfigurationInterface class
 *
 *  \return
 */
/****************************************************************************/
CRCConfigurationInterface::CRCConfigurationInterface(const CRCConfigurationInterface& RCConfigurationInterface)
    : CDataContainerBase()
    , mp_ReadWriteLock(NULL)
    , m_DataVerificationMode(true)
    , mp_RCConfiguration(NULL)
    , m_FileName("RCConfiguration.xml")
{
    // create the Read write lock for threads
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    mp_RCConfiguration = new CRCConfiguration();
    CopyFromOther(RCConfigurationInterface);
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
void CRCConfigurationInterface::CopyFromOther(const CRCConfigurationInterface &Other)
{
    //QReadWriteLock is not copied. We use the existing lock object
    CRCConfigurationInterface &OtherConfigInterface = const_cast<CRCConfigurationInterface &>(Other);
    m_FileName  = OtherConfigInterface.GetFilename();
    m_DataVerificationMode = OtherConfigInterface.m_DataVerificationMode;
    mp_RCConfiguration->CopyFromOther(*(OtherConfigInterface.GetRCConfiguration()));
}
/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CRCConfigurationInterface::~CRCConfigurationInterface()
{
    if (mp_ReadWriteLock != NULL)
    {
        try {
            delete mp_ReadWriteLock;
        }
        CATCHALL_DTOR();
        mp_ReadWriteLock = NULL;
    }

    try {
        delete mp_RCConfiguration;
    }
    CATCHALL_DTOR();
    mp_RCConfiguration = NULL;

}

/****************************************************************************/
/*!
 *  \brief  Updates rc configuration
 *
 *  \iparam p_RCConfiguration = Pointer to a rc configuration
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRCConfigurationInterface::UpdateRCConfiguration(const CRCConfiguration* p_RCConfiguration)
{
    if (p_RCConfiguration == NULL)
    {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_NULL_PTR, Global::GUI_MSG_BOX);
        m_ErrorMap.insert(EVENT_DM_NULL_PTR, Global::tTranslatableStringList());
        SetErrorList(&m_ErrorMap);
        return false;
    }

    bool Result = false;
    // check the verification flags
    if (m_DataVerificationMode)
    {
        // create the temporary CDataRackList class object
        CRCConfigurationInterface* p_USI_Verification = new CRCConfigurationInterface();

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_USI_Verification = *this;

            // disable verification in clone
            p_USI_Verification->SetDataVerificationMode(false);

            // execute required action (UpdateRack) in clone
            Result = p_USI_Verification->UpdateRCConfiguration(p_RCConfiguration);

            if (Result)
            {
                Result = DoLocalVerification(p_USI_Verification);
                ListOfErrors_t ErrorList = p_USI_Verification->GetErrorList();
                if (!ErrorList.isEmpty())
                {
                    // If the control reaches here means Error hash is empty
                    // Considering only the first element in Hash since
                    // verfier can atmost add only one Hash has to the error list
                    m_ErrorMap = *(ErrorList.first());
                    SetErrorList(&m_ErrorMap);
                }
            }
        }
        if (Result)
        {
            // if content ok, clone backwards
            *this = *p_USI_Verification;
            Result = true;
        }
        // delete test clone
        delete p_USI_Verification;

    }
    else
    {
        QWriteLocker locker(mp_ReadWriteLock);
        if (mp_RCConfiguration)
        {
            // replace the old class object with updated class object
            *mp_RCConfiguration = *p_RCConfiguration;
            Result = true;
        }
        else
        {
            Result = false;
        }
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Get the rc configuration settings
 *
 *  \iparam CopySettings = Flag for copying settings (either deep or shallow copy)
 *
 *  \return rc configuration class
 */
/****************************************************************************/
CRCConfiguration* CRCConfigurationInterface::GetRCConfiguration(const bool& CopySettings)
{
    CHECKPTR_RETURN(mp_RCConfiguration, NULL);

    if (!CopySettings)
    {
        return mp_RCConfiguration;
    }
    else
    {
        // create the new user settings
        CRCConfiguration* p_RCConfiguration = new CRCConfiguration();
        // Do a deep copy
        *p_RCConfiguration = *mp_RCConfiguration;
        return p_RCConfiguration;
    }
}

/****************************************************************************/
/*!
 *  \brief Writes from the CRCConfigurationInterface object to a IODevice.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfigurationInterface::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    if (!mp_RCConfiguration)
    {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_NULL_PTR, Global::GUI_MSG_BOX);
        m_ErrorMap.insert(EVENT_DM_NULL_PTR, Global::tTranslatableStringList());
        SetErrorList(&m_ErrorMap);
        return false;
    }
    qDebug()<<"COMPLETE DATA"<<CompleteData;
    QXmlStreamWriter XmlStreamWriter; //!< Xml stream writer object to write the Xml contents in a file

    XmlStreamWriter.setDevice(&(const_cast<QIODevice &>(IODevice)));
    XmlStreamWriter.setAutoFormatting(true);

    // start the XML Document
    XmlStreamWriter.writeStartDocument();

    // write the documnet type declaration
    XmlStreamWriter.writeDTD("<!DOCTYPE RCConfiguration>");
    XmlStreamWriter.writeStartElement("RCConfiguration");

    if (!mp_RCConfiguration->SerializeContent(XmlStreamWriter, CompleteData))
    {
        qDebug() << "CDataRackList::Write failed. Write Racks failed!";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_SERIALIZE_FAILED, Global::tTranslatableStringList() << "RCConfiguration", true);
        return false;
    }
    // store the Class temporary data
    if(CompleteData)
    {
        //Need to be added the temporary members
        XmlStreamWriter.writeStartElement("ClassTemporaryData");

        XmlStreamWriter.writeAttribute("FileName", GetFilename());

        // write the verification mode
        if (GetDataVerificationMode())
        {
            XmlStreamWriter.writeAttribute("VerificationMode", "true");
        }
        else
        {
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
 *  \brief Reads from the IODevice to CRCConfigurationInterface object.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfigurationInterface::DeserializeContent(QIODevice& IODevice ,bool CompleteData)
{
    if (!mp_RCConfiguration)
    {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_NULL_PTR, Global::GUI_MSG_BOX);
        m_ErrorMap.insert(EVENT_DM_NULL_PTR, Global::tTranslatableStringList());
        SetErrorList(&m_ErrorMap);
        return false;
    }
    QXmlStreamReader XmlStreamReader;

    XmlStreamReader.setDevice(&IODevice);


    if (!mp_RCConfiguration->DeserializeContent(XmlStreamReader, CompleteData))
    {
        qDebug() << "CDataRackList::Read failed. Read Racks failed!";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_DESERIALIZE_FAILED, Global::tTranslatableStringList() << "RCConfiguration", true);
        return false;
    }

    //======NODE=======Temporary Data Variables=========================
    if (CompleteData)
    {
        while ((!XmlStreamReader.atEnd()) &&
               ((XmlStreamReader.name() != "ClassTemporaryData") || (XmlStreamReader.tokenType() != QXmlStreamReader::EndElement)))
        {
            if (XmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
            {
                if (XmlStreamReader.name() == "ClassTemporaryData")
                {
                    // File name
                    if (!XmlStreamReader.attributes().hasAttribute("FileName"))
                    {
                        qDebug() << "### attribute <FileName> is missing => abort reading";
                        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() <<"FileName", true);
                        return false;
                    }
                    m_FileName = XmlStreamReader.attributes().value("FileName").toString();

                    // VerificationMode
                    if (!XmlStreamReader.attributes().hasAttribute("VerificationMode"))
                    {
                        qDebug() << "### attribute <VerificationMode> is missing => abort reading";
                        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() <<"VerificationMode", true);
                        return false;
                    }
                    else
                    {
                        bool Value = false;
                        if (XmlStreamReader.attributes().value("VerificationMode").toString().toUpper() == "TRUE")
                        {
                            Value = true;
                        }
                        SetDataVerificationMode(Value);
                    }

                }
            }
            XmlStreamReader.readNextStartElement();
        }

    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the data and stores in CRCConfigurationInterface from the file
 *
 *  \iparam FileName = Name of the Input File
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCConfigurationInterface::Read(const QString& FileName)
{
    //check if file exists
    if (!QFile::exists(FileName))
    {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_FILE_NOT_EXISTS, Global::tTranslatableStringList() << "RCConfiguration", true);
        return false;
    }

    bool Result = true; //!< store the result flag in the local variable

    if (m_DataVerificationMode)
    {
        QWriteLocker locker(mp_ReadWriteLock);

        // create the user settings class
        CRCConfigurationInterface *p_RCConfiguration_Verification = new CRCConfigurationInterface();
        // reset the data verification flag
        p_RCConfiguration_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        // read the data from the file
        if (!p_RCConfiguration_Verification->Read(FileName))
        {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_SETTINGS_XML_READ_FAILED);
            Result = false;
        }
        else
        {
            // verify the data
            if (DoLocalVerification(p_RCConfiguration_Verification))
            {
                // if content ok, clone backwards
                *this = *p_RCConfiguration_Verification;
                Result = true;
            }
            else
            {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_SETTINGS_VERIFICATION_FAILED);
                Result = false;
            }            
        }
        // delete test clone
        delete p_RCConfiguration_Verification;

    }
    else
    {
        QWriteLocker locker(mp_ReadWriteLock);

        // Initialise the m_Filename to a known string "UNDEFINED"
        m_FileName = "UNDEFINED";

        // create the file
        QFile File(FileName);
        if (!File.open(QFile::ReadOnly | QFile::Text))
        {
            qDebug() << "open file failed in Read: " << FileName;
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_FILE_OPEN_FAILED, Global::tTranslatableStringList() <<  "RCConfiguration.xml", true);
            return false;
        }

        if (!DeserializeContent(File, false))
        {
            qDebug() << "### CRCConfigurationInterface::Read failed for file: " << FileName;
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
 *  \iparam RCInterface = CRCConfigurationInterface class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(const QDataStream& OutDataStream, const CRCConfigurationInterface& RCInterface)
{
    // remove the constant by using type cast opeator
    CRCConfigurationInterface* p_TempRCInterface = const_cast<CRCConfigurationInterface*>(&RCInterface);
    QDataStream& OutDataStreamRef = const_cast<QDataStream &>(OutDataStream);

    if (!p_TempRCInterface->SerializeContent(*OutDataStreamRef.device(), true))
    {
        qDebug() << "CRCConfigurationInterface::Operator Streaming (SerializeContent) failed.";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "RCConfiguration", true);
    }

    return OutDataStreamRef;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam RCInterface = CRCConfigurationInterface class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(const QDataStream& InDataStream, const CRCConfigurationInterface& RCInterface)
{
    QDataStream& InDataStreamRef = const_cast<QDataStream &>(InDataStream);
    CRCConfigurationInterface& RCInterfaceRef = const_cast<CRCConfigurationInterface &>(RCInterface);
    // deserialize the content from the data stream
    if (!RCInterfaceRef.DeserializeContent(*InDataStreamRef.device(), true))
    {
        qDebug() << "CRCConfigurationInterface::Operator Streaming (DeSerializeContent) failed because it does not have any Data to stream.";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "RCConfiguration", true);
    }
    return InDataStreamRef;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceSettings = CRCConfigurationInterface class object
 *
 *  \return CRCConfigurationInterface Class Object
 */
/****************************************************************************/
CRCConfigurationInterface& CRCConfigurationInterface::operator=(const CRCConfigurationInterface& SourceSettings)
{
    // make sure not same object
    if (this != &SourceSettings)
    {
        CopyFromOther(SourceSettings);

    }
    return *this;
}


} // end namespace DataManager
