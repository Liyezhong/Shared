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
#include "DataManager/Containers/Parameter/Include/ProcessSettings.h"
#include "Global/Include/EventObject.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CProcessSettings::CProcessSettings() :CDataContainerBase(), m_Version(0), m_DataVerificationMode(true), m_Filename(""), mp_ReadWriteLock(NULL)
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
CProcessSettings::CProcessSettings(const CProcessSettings& ParameterList) : CDataContainerBase(),
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
CProcessSettings::~CProcessSettings()
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
void CProcessSettings::Init()
{
    QWriteLocker Locker(mp_ReadWriteLock);
    m_Version = 0;
    m_Filename = "";
    if (!DeleteAllGroup()) {
        //throw error ...
        THROWARG(EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
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
bool CProcessSettings::Read(QString Filename)
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
        CProcessSettings* p_DPL_Verification = new CProcessSettings();

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

                //Now initialize the filename member since Read is succcess
                m_Filename = Filename;

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
 *  \brief  Deletes all the Groups in the list
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CProcessSettings::DeleteAllGroup()
{
    DevicePositionList_t::iterator PositionIterator;
    while (m_DevicePositionList.size() > 0)
    {
       PositionIterator = m_DevicePositionList.begin();
       PositionIterator.value().clear();
       m_DevicePositionList.remove(PositionIterator.key());
    }
    m_DevicePositionList.clear();

    MotionProfileList_t::Iterator SpeedProfileIterator;
    while (m_SpeedProfileList.size() > 0)
    {
        SpeedProfileIterator = m_SpeedProfileList.begin();
        SpeedProfileIterator.value().clear();
        m_SpeedProfileList.remove(SpeedProfileIterator.key());
    }
    m_SpeedProfileList.clear();

    return true;
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
bool CProcessSettings::ReadAllGroups(QXmlStreamReader& XmlStreamReader)
{
    bool Result = false;

    QString Key;

    // Look for node <DeviceList>
    if (!Helper::ReadNode(XmlStreamReader, "DeviceList"))
    {
        qDebug() << "DeserializeContent: abort reading. Node not found: Device";
        return false;
    }

    // Look for node <Device>
    while(!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1)
        {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
            break;
        }

        if (XmlStreamReader.isStartElement())
        {
            if (XmlStreamReader.name() == "Device")
            {
                Key = XmlStreamReader.attributes().value("Key").toString();

                m_DeviceList.append(Key);

                if (ReadDeviceParameters(XmlStreamReader, Key) == false)
                {
                    break;
                }
            }
            else
            {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                break;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "DeviceList")
        {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this node";
            Result = true;
            break; // exit from while loop
        }
    }

    return Result;
}

bool CProcessSettings::ReadDeviceParameters(QXmlStreamReader& XmlStreamReader, QString DeviceKey)
{
    bool Result = false;

    qDebug() << "Reading parameter: " << DeviceKey;

    while(!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1)
        {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
            break;
        }

        if (XmlStreamReader.isStartElement())
        {
            if (XmlStreamReader.name() == "SpeedProfile")
            {
                if (ReadSpeedProfiles(XmlStreamReader, DeviceKey) == false)
                {
                    break;
                }
            }
            else if(XmlStreamReader.name() == "Position")
            {
                if (ReadPositions(XmlStreamReader, DeviceKey) == false)
                {
                    break;
                }
            }
            else
            {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                break;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "Device")
        {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this node";
            Result = true;
            break; // exit from while loop
        }
    }

    return Result;
}

bool CProcessSettings::ReadSpeedProfiles(QXmlStreamReader &XmlStreamReader, QString DeviceKey)
{
    bool Result = false;

    QString ParamKey;
    qint16 Value;

    qDebug() << "Reading Speed profiles";

    while(!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1)
        {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
            break;
        }

        if (XmlStreamReader.isStartElement())
        {
            if (XmlStreamReader.name() == "Parameter")
            {
                // Key
                if (!XmlStreamReader.attributes().hasAttribute("Key"))
                {
                    qDebug() << "### attribute <Key> is missing => abort reading";
                    break;
                }
                ParamKey = XmlStreamReader.attributes().value("Key").toString();

                // Value
                if (!XmlStreamReader.attributes().hasAttribute("Value"))
                {
                    qDebug() << "### attribute <Value> is missing => abort reading";
                    break;
                }
                Value = XmlStreamReader.attributes().value("Value").toString().toInt();

                m_SpeedProfileList[DeviceKey][ParamKey] = Value;
            }
            else
            {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                break;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "SpeedProfile")
        {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this node";
            Result = true;
            break; // exit from while loop
        }
    }

    return Result;
}

bool CProcessSettings::ReadPositions(QXmlStreamReader &XmlStreamReader, QString DeviceKey)
{
    bool Result = false;

    QString ParamKey;
    qint16 Value;
    qint16 Offset;

    qDebug() << "Reading positions";

    while(!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1)
        {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
            break;
        }

        if (XmlStreamReader.isStartElement())
        {
            if (XmlStreamReader.name() == "Parameter")
            {
                // Key
                if (!XmlStreamReader.attributes().hasAttribute("Key"))
                {
                    qDebug() << "### attribute <Key> is missing => abort reading";
                    break;
                }
                ParamKey = XmlStreamReader.attributes().value("Key").toString();

                // Value
                if (!XmlStreamReader.attributes().hasAttribute("Value"))
                {
                    qDebug() << "### attribute <Value> is missing => abort reading";
                    break;
                }
                Value = XmlStreamReader.attributes().value("Value").toString().toInt();

                // Offset
                if (!XmlStreamReader.attributes().hasAttribute("Offset"))
                {
                    qDebug() << "### attribute <Offset> is missing => abort reading";
                    break;
                }
                Offset = XmlStreamReader.attributes().value("Offset").toString().toInt();

                m_DevicePositionList[DeviceKey][ParamKey] = CPositionParameter(Value, Offset);
            }
            else
            {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                break;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "Position")
        {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this node";
            Result = true;
            break; // exit from while loop
        }
    }

    return Result;
}

bool CProcessSettings::WriteAllGroups(QXmlStreamWriter& XmlStreamWriter)
{
    DeviceList_t::const_iterator DeviceIterator;
    QString Key;
    ProfileParam_t::const_iterator SpeedProfileIterator;
    PositionParam_t::const_iterator ParamIterator;
    CPositionParameter Parameter;

    QString StringValue;

    for (DeviceIterator = m_DeviceList.constBegin(); DeviceIterator != m_DeviceList.constEnd(); DeviceIterator++)
    {
        Key = *DeviceIterator;

        XmlStreamWriter.writeStartElement("Device");

        XmlStreamWriter.writeAttribute("Key", Key);

        if (m_SpeedProfileList.contains(Key))
        {
            XmlStreamWriter.writeStartElement("SpeedProfile");

            for (SpeedProfileIterator = m_SpeedProfileList[Key].constBegin(); SpeedProfileIterator != m_SpeedProfileList[Key].constEnd(); SpeedProfileIterator++)
            {
                XmlStreamWriter.writeStartElement("Parameter");

                XmlStreamWriter.writeAttribute("Key", SpeedProfileIterator.key());

                (void)StringValue.setNum(SpeedProfileIterator.value());
                XmlStreamWriter.writeAttribute("Value", StringValue);

                XmlStreamWriter.writeEndElement();
            }

            XmlStreamWriter.writeEndElement(); // </SpeedProfile>
        }

        if (m_DevicePositionList.contains(Key))
        {
            XmlStreamWriter.writeStartElement("Position");

            for (ParamIterator = m_DevicePositionList[Key].constBegin(); ParamIterator != m_DevicePositionList[Key].constEnd(); ParamIterator++)
            {
                Parameter = ParamIterator.value();

                XmlStreamWriter.writeStartElement("Parameter");

                XmlStreamWriter.writeAttribute("Key", ParamIterator.key());

                (void)StringValue.setNum(Parameter.GetValue());
                XmlStreamWriter.writeAttribute("Value", StringValue);

                (void)StringValue.setNum(Parameter.GetOffsetValue());
                XmlStreamWriter.writeAttribute("Offset", StringValue);

                XmlStreamWriter.writeEndElement();
            }

            XmlStreamWriter.writeEndElement(); // </Position>
        }

        XmlStreamWriter.writeEndElement(); // </Device>
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Writes from the process settings to a IODevice.
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CProcessSettings::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    Q_UNUSED(CompleteData)

    QString StringValue;
    bool Result = true;
    QXmlStreamWriter XmlStreamWriter;

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    XmlStreamWriter.writeDTD("<!DOCTYPE ProcessSettings>");
    XmlStreamWriter.writeStartElement("ProcessSettings");

    // write attribute version
    (void)StringValue.setNum(GetVersion()); //to suppress lint-534
    XmlStreamWriter.writeAttribute("Version", StringValue);

    XmlStreamWriter.writeStartElement("DeviceList");

    // write all Groups
    if (!WriteAllGroups(XmlStreamWriter))
    {
        qDebug("DataManager::CGroupList::WriteAllGroups failed ");
        return false;
    }

    XmlStreamWriter.writeEndElement(); // DeviceList

    XmlStreamWriter.writeEndElement(); // ProcessSettings

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
bool CProcessSettings::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{
    Q_UNUSED(CompleteData)

    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(&IODevice);
    bool Result = true;

    // look for node <process_settings>
    if (!Helper::ReadNode(XmlStreamReader, "ProcessSettings")) {
        qDebug()<<"ProcessSettings NodeName:"<<XmlStreamReader.name()<<"NodeName LineNumber"<<XmlStreamReader.lineNumber();
        qDebug() << "DeserializeContent: abort reading. Node not found: ProcessSettings";
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
        THROWARG(EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    }
	
    Result = ReadAllGroups(XmlStreamReader);
    
    return Result;
}

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
QDataStream& operator <<(QDataStream& OutDataStream, const CProcessSettings& GroupList) {

    CProcessSettings* p_TempGroupList = const_cast<CProcessSettings*>(&GroupList);

    if (!p_TempGroupList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CGroupList::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE );
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
QDataStream& operator >>(QDataStream& InDataStream, CProcessSettings& GroupList) {

    if (!GroupList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CGroupList::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
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
CProcessSettings& CProcessSettings::operator = (const CProcessSettings& SourceGroupList) {

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

bool CProcessSettings::GetSpeedProfile(QString GroupKey, QString ParamKey, quint8 &SpeedProfile)
{
    bool Result = false;

    if (m_SpeedProfileList.contains(GroupKey))
    {
        if (m_SpeedProfileList[GroupKey].contains(ParamKey))
        {
            SpeedProfile = m_SpeedProfileList[GroupKey][ParamKey];

            Result = true;
        }
    }

    return Result;
}

bool CProcessSettings::SetSpeedProfile(QString GroupKey, QString ParamKey, quint8 SpeedProfile)
{
    bool Result = false;

    if (m_SpeedProfileList.contains(GroupKey))
    {
        if (m_SpeedProfileList[GroupKey].contains(ParamKey))
        {
            m_SpeedProfileList[GroupKey][ParamKey] = SpeedProfile;

            Result = true;
        }
    }

    return Result;
}

bool CProcessSettings::GetPosition(QString GroupKey, QString ParamKey, qint32 &Postion)
{
    bool Result = false;

    if (m_DevicePositionList.contains(GroupKey))
    {
        if (m_DevicePositionList[GroupKey].contains(ParamKey))
        {
            Postion = m_DevicePositionList[GroupKey][ParamKey].GetPosition();

            Result = true;
        }
    }

    return Result;
}

bool CProcessSettings::SetPosition(QString GroupKey, QString ParamKey, const qint32 Position)
{
    bool Result = false;

    if (m_DevicePositionList.contains(GroupKey))
    {
        if (m_DevicePositionList[GroupKey].contains(ParamKey))
        {
            m_DevicePositionList[GroupKey][ParamKey].SetPosition(Position);

            Result = true;
        }
    }

    return Result;
}

bool CProcessSettings::ResetPositionOffset(QString GroupKey, QString ParamKey)
{
    bool Result = false;

    if (m_DevicePositionList.contains(GroupKey))
    {
        if (m_DevicePositionList[GroupKey].contains(ParamKey))
        {
            m_DevicePositionList[GroupKey][ParamKey].ResetOffset();

            Result = true;
        }
    }

    return Result;
}

}//End of namespace DataManager
