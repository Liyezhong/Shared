/****************************************************************************/
/*! \file ProgramSettings.cpp
 *
 *  \brief Implementation of CProgramSettings class
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-04-03
 *   $Author:  $ L. Yang
 *
 *  \b Company:
 *
 *       Leica Biosystems SH. CN.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
/*#include <QDebug>
#include <QFile>
#include <QReadLocker>
#include <QWriteLocker>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QRegExp>
*/
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Helper.h"
#include "HimalayaDataContainer/Containers/ProgramSettings/Include/ProgramSettings.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"
#include <QDataStream>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CProgramSettings::CProgramSettings() :CDataContainerBase(), m_Version(0), m_DataVerificationMode(true), m_Filename(""), mp_ReadWriteLock(NULL)
{
	mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
	Init();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ParameterList = Instance of CProgramSettings class
 */
/****************************************************************************/
CProgramSettings::CProgramSettings(const CProgramSettings& ParameterList) : CDataContainerBase(),
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
CProgramSettings::~CProgramSettings()
{
    try {
        (void)DeleteAll(); // to avoid lint 534
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
void CProgramSettings::Init()
{
    QWriteLocker Locker(mp_ReadWriteLock);
    m_Version = 0;
    m_Filename = "";
    if (!DeleteAll()) {
        //throw error ...
//        Global::EventObject::Instance().RaiseEvent(EVENT_DM_NULL_PTR);
//        THROWARG(EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);

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
bool CProgramSettings::Read(QString Filename)
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
        //To make sure other threads cant write when reading is active
        QWriteLocker locker(mp_ReadWriteLock);

        // create instance of CGroupList for verification
        CProgramSettings* p_DPL_Verification = new CProgramSettings();

        // create clone from current state
        *p_DPL_Verification = *this;

        // disable verification in clone
        p_DPL_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        if (!p_DPL_Verification->Read(Filename)) {
            qDebug() << "CAdjustment::Read - file read failed";
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_SETTINGS_XML_READ_FAILED);
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
                qDebug() << "CAdjustment::Read - verification failed";
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_SETTINGS_VERIFICATION_FAILED);
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
            qDebug() << "CAdjustment::Read - file open failed :" << Filename;
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_FILE_OPEN_FAILED,
                                                      Global::tTranslatableStringList() << Filename, true);
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


double CProgramSettings::GetParameterValue(const QString& DeviceKey, const FunctionKey_t& FunctionKey,const QString& ParameterKey, bool& ok)
{
    QString value="";
    double ret = -1;
    ok = false;
    if(m_Parameters.contains(DeviceKey))
    {
        if(m_Parameters[DeviceKey].contains(FunctionKey))
        {
            if(m_Parameters[DeviceKey][FunctionKey].contains(ParameterKey))
            {
                ok = true;
                value = m_Parameters[DeviceKey][FunctionKey][ParameterKey];
                if (value.isEmpty())
                {
                    value = "0";
                }
            }
        }
    }
    if(ok)
    {
        QRegExp TimeFormat("\\d+[DdHhMmSs]");
        if(value.contains(TimeFormat))
        {
            ret = Helper::ConvertTimeStringToSeconds(value);
        }
        else
        {
            ret = value.toDouble(&ok);
        }
    }
    return ret;
}
double CProgramSettings::GetParameterValue(const QString& DeviceKey, const QString& FunctionKey, const QString& ParameterKey, bool& ok)
{
    FunctionKey_t FunctionKeyWithGroup ={"", "", ""};
    FunctionKeyWithGroup.key = FunctionKey;
    FunctionKeyWithGroup.name = "";
    FunctionKeyWithGroup.sequence = "";
    return this->GetParameterValue(DeviceKey, FunctionKeyWithGroup, ParameterKey, ok);
}
QString CProgramSettings::GetParameterStrValue(const QString& DeviceKey, const FunctionKey_t& FunctionKey,const QString& ParameterKey)
{
    QString value="";
    if(m_Parameters.contains(DeviceKey))
    {
        if(m_Parameters[DeviceKey].contains(FunctionKey))
        {
            if(m_Parameters[DeviceKey][FunctionKey].contains(ParameterKey))
            {
                value = m_Parameters[DeviceKey][FunctionKey][ParameterKey];
                if (value.isEmpty())
                {
                    value = "0";
                }
            }
        }
    }

    return value;
}

bool CProgramSettings::SetParameterValue(const QString& DeviceKey, const FunctionKey_t& FunctionKey, const QString& ParameterKey, const QString& value)
{
    bool ok = false;
    if(m_Parameters.contains(DeviceKey))
    {
        if(m_Parameters[DeviceKey].contains(FunctionKey))
        {
            if(m_Parameters[DeviceKey][FunctionKey].contains(ParameterKey))
            {
                ok = true;
                m_Parameters[DeviceKey][FunctionKey][ParameterKey] = value;
            }
        }
    }

    if(ok)
    {
        ok = Write();
    }
    return ok;
}

bool CProgramSettings::SetParameterValue(const QString& DeviceKey, const QString& FunctionKey, const QString& ParameterKey, const QString& value)
{
    FunctionKey_t FunctionKeyWithGroup = {"", "", ""};
    FunctionKeyWithGroup.key = FunctionKey;
    FunctionKeyWithGroup.name = "";
    FunctionKeyWithGroup.sequence  = "";
    return this->SetParameterValue(DeviceKey, FunctionKeyWithGroup, ParameterKey, value);
}

bool CProgramSettings::SetParameterValue(const QString& DeviceKey, const FunctionKey_t& FunctionKey,const QString& ParameterKey, double value)
{
    return SetParameterValue(DeviceKey, FunctionKey, ParameterKey, QString::number(value));
}

bool CProgramSettings::SetParameterValue(const QString& DeviceKey, const QString& FunctionKey, const QString& ParameterKey, double value)
{
    FunctionKey_t FunctionKeyWithGroup = {"", "", ""};
    FunctionKeyWithGroup.key = FunctionKey;
    FunctionKeyWithGroup.name = "";
    FunctionKeyWithGroup.sequence  = "";
    return this->SetParameterValue(DeviceKey, FunctionKeyWithGroup, ParameterKey, value);
}

/****************************************************************************/
/*!
 *  \brief  Deletes all the Groups in the list
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CProgramSettings::DeleteAll()
{
    m_Parameters.clear();

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads all the Groups from the xml stream
 *
 *  \iparam XmlStreamReader = Reference to xml stream to read from
 *
 *  \return true - read success or false - read failure
 */
/****************************************************************************/
bool CProgramSettings::ReadAllParameters(QXmlStreamReader& XmlStreamReader)
{
    bool Result = false;

    QString Key;

    // Look for node <DeviceList>
    if (!Helper::ReadNode(XmlStreamReader, "DeviceList"))
    {
        qDebug() << "DeserializeContent: abort reading. Node not found: Device";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "DeviceList", true);
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

bool CProgramSettings::ReadDeviceParameters(QXmlStreamReader& XmlStreamReader, const QString& DeviceKey)
{
    bool Result = false;
    FunctionKey_t FunctionKey;

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
            if (XmlStreamReader.name() == "Function")
            {
                FunctionKey.key  = XmlStreamReader.attributes().value("Key").toString();
                FunctionKey.name = XmlStreamReader.attributes().value("name").toString();
                FunctionKey.sequence = XmlStreamReader.attributes().value("sequence").toString();
                if(ReadFunctionParameters(XmlStreamReader,DeviceKey,FunctionKey) == false)
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

bool CProgramSettings::ReadFunctionParameters(QXmlStreamReader &XmlStreamReader, const QString& DeviceKey, const FunctionKey_t& FunctionKey)
{
    bool Result = false;

    QString ParamKey;
    QString Value;

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
                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                               Global::tTranslatableStringList() << "Key", true);

                    break;
                }
                ParamKey = XmlStreamReader.attributes().value("Key").toString();

                // Value
                if (!XmlStreamReader.attributes().hasAttribute("Value"))
                {
                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                               Global::tTranslatableStringList() << "Value", true);

                    qDebug() << "### attribute <Value> is missing => abort reading";

                    break;
                }
                Value = XmlStreamReader.attributes().value("Value").toString();

                m_Parameters[DeviceKey][FunctionKey][ParamKey] = Value;
            }
            else
            {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                break;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "Function")
        {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this node";
            Result = true;
            break; // exit from while loop
        }
    }

    return Result;
}

bool CProgramSettings::WriteAllParameters(QXmlStreamWriter& XmlStreamWriter)
{
    DeviceFunction_t::const_iterator DeviceIterator;
    FunctionParameter_t::const_iterator FunctionIterator;
    ParameterKeyValue_t::const_iterator ParameterIterator;
    QString DKey;
    FunctionKey_t FKey = {"", "", ""};

    for(DeviceIterator = m_Parameters.begin(); DeviceIterator != m_Parameters.end(); DeviceIterator++)
    {
        DKey = DeviceIterator.key();

        XmlStreamWriter.writeStartElement("Device");

        XmlStreamWriter.writeAttribute("Key", DKey);
        for(FunctionIterator = m_Parameters[DKey].begin(); FunctionIterator != m_Parameters[DKey].end(); FunctionIterator++)
        {
            FKey = FunctionIterator.key();
            XmlStreamWriter.writeStartElement("Function");
            XmlStreamWriter.writeAttribute("Key", FKey.key);
            XmlStreamWriter.writeAttribute("name", FKey.name);
            XmlStreamWriter.writeAttribute("sequence", FKey.sequence);

            for (ParameterIterator = m_Parameters[DKey][FKey].begin();
                 ParameterIterator != m_Parameters[DKey][FKey].end(); ParameterIterator++)
            {
                XmlStreamWriter.writeStartElement("Parameter");

                XmlStreamWriter.writeAttribute("Key", ParameterIterator.key());

                XmlStreamWriter.writeAttribute("Value", ParameterIterator.value());

                XmlStreamWriter.writeEndElement(); // end element of Parameter
            }
            XmlStreamWriter.writeEndElement(); // end element of Function
        }
        XmlStreamWriter.writeEndElement(); // end element of Device

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
bool CProgramSettings::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    Q_UNUSED(CompleteData)

    QString StringValue;
    bool Result = true;
    QXmlStreamWriter XmlStreamWriter;

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    XmlStreamWriter.writeDTD("<!DOCTYPE ProgramSettings>");
    XmlStreamWriter.writeStartElement("ProgramSettings");

    // write attribute version
    (void)StringValue.setNum(GetVersion()); //to suppress lint-534
    XmlStreamWriter.writeAttribute("Version", StringValue);

    XmlStreamWriter.writeStartElement("DeviceList");

    // write all Groups
    if (!WriteAllParameters(XmlStreamWriter))
    {
        qDebug("DataManager::CGroupList::WriteAllGroups failed ");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_SERIALIZE_FAILED,
                                                   Global::tTranslatableStringList() << "ProgramSettings", true);
        return false;
    }

    XmlStreamWriter.writeEndElement(); // DeviceList

    XmlStreamWriter.writeEndElement(); // ProgramSettings

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
bool CProgramSettings::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{
    Q_UNUSED(CompleteData)

    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(&IODevice);
    bool Result = true;

    // look for node <process_settings>
    if (!Helper::ReadNode(XmlStreamReader, "ProgramSettings")) {
        qDebug()<<"ProgramSettings NodeName:"<<XmlStreamReader.name()<<"NodeName LineNumber"<<XmlStreamReader.lineNumber();
        qDebug() << "DeserializeContent: abort reading. Node not found: ProgramSettings";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "ProgramSettings", true);
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
    
    if (!DeleteAll()) {
        //throw error ...
//        Global::EventObject::Instance().RaiseEvent(EVENT_DM_NULL_PTR);
//        THROWARG(EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    }
	
    Result = ReadAllParameters(XmlStreamReader);

    if (false == Result)
    {
        qDebug() << "DeserializeContent: abort reading. Parsing failed: ProgramSettings";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_XML_DESERIALIZE_FAILED,
                                                   Global::tTranslatableStringList() << "ProgramSettings", true);
        return false;
    }
    
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
QDataStream& operator <<(QDataStream& OutDataStream, const CProgramSettings& GroupList) {

    CProgramSettings* p_TempGroupList = const_cast<CProgramSettings*>(&GroupList);

    if (!p_TempGroupList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CGroupList::Operator Streaming (SerializeContent) failed.";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED,
                                                   Global::tTranslatableStringList() << "ProgramSettings", true);
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE );
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
QDataStream& operator >>(QDataStream& InDataStream, CProgramSettings& GroupList) {

    if (!GroupList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CGroupList::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED,
                                                   Global::tTranslatableStringList() << "ProgramSettings", true);

        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
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
CProgramSettings& CProgramSettings::operator = (const CProgramSettings& SourceGroupList) {

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

}//End of namespace DataManager
