/****************************************************************************/
/*! \file SepiaParameterSetList.cpp
 *
 *  \brief Implementation file for class CSepiaParameterSetList.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-06
 *  $Author:    $ Michael Thiel
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ParameterSets/Include/SepiaParameterSetList.h"



namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CSepiaParameterSetList::CSepiaParameterSetList()
    : m_Version(0)
    , m_Filename("")
//    , mp_ReadWriteLock(NULL)
    , m_ParameterId("")
{
//    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);

}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ProgramList = Instance of the ProgramList class
 *
 */
/****************************************************************************/
CSepiaParameterSetList::CSepiaParameterSetList(const CSepiaParameterSetList& ProgramList)
    : m_Version(0)
    , m_Filename("")
//    , mp_ReadWriteLock(NULL)
    , m_ParameterId("")
{
//    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);

    //! \todo catch exceptions
    *this = ProgramList;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CSepiaParameterSetList::~CSepiaParameterSetList()
{
    //Ignoring return value
    try {
        (void)DeleteAllPrograms(); //to avoid lint warning 534
    }
    catch(...) {
        //to please PClint
    }

//    if (mp_ReadWriteLock != NULL) {
//       try {
//           delete mp_ReadWriteLock;
//       }
//       catch(...) {
//           //to please PClint
//       }
//       mp_ReadWriteLock = NULL;
//    }
}

/****************************************************************************/
/*!
 *  \brief Initializes the object
 */
/****************************************************************************/
void CSepiaParameterSetList::Init()
{
//    QWriteLocker Locker(mp_ReadWriteLock);
    m_Version = 0;
    m_Filename = "";

    if (!DeleteAllPrograms()) {
        //throw error ...
        THROWARG(EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    }

}

/****************************************************************************/
/*!
 *  \brief Reads an xml file and fill programlist object
 *
 *  \iparam Filename = Filename to read
 *
 *  \return true- read success , false - read failed
 */
/****************************************************************************/
bool CSepiaParameterSetList::Read(QString Filename)
{
    //check if file exists
    if (!QFile::exists(Filename)) {
        return false;
    }

//    QWriteLocker locker(mp_ReadWriteLock);

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
        qDebug() << "### CSepiaParameterSetList::Read failed for file during deserializing: " << Filename;
        return false;
    }

    //Now initialize the filename member since Read is succcess
    m_Filename = Filename;

    File.close();

    return true;

}

/****************************************************************************/
/*!
 *  \brief Function to retrieve program for given index
 *  \iparam Index = Index in program list
 *
 *  \return Program associated with the index
 */
/****************************************************************************/
CSepiaParameterSet* CSepiaParameterSetList::GetParameterSet(QString ParameterId)
{
    return m_ProgramList.value(ParameterId, NULL);
}

/****************************************************************************/
/*!
 *  \brief Checks program exists in the list
 *  \iparam ProgramID = Program ID
 *
 *  \return true =exists , false - not exists
 */
/****************************************************************************/

bool CSepiaParameterSetList::ParameterSetExists(QString ParameterId) {
    return m_ProgramList.contains(ParameterId);
}

/****************************************************************************/
/*!
 *  \brief Adds program step to program
 *  \iparam p_Program = Program to add
 *  \warning If a program is added with verification mode false,
 *           then expanded step list is not updated. One has to explicitly
 *           Call UpdateExpandedSteps()
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
bool CSepiaParameterSetList::AddParameterSet(CSepiaParameterSet* p_Program)
{
    if (p_Program == NULL) return false;

    if (m_ProgramList.contains(p_Program->GetID())) {
        // there is already a program with the given ProgramID
        // => use UpdateProgram instead
        //    or call DeleteProgram before AddProgram
        return false;
    }

//    p_Program->SetID(p_Program->GetID());
    m_ProgramList.insert(p_Program->GetID(), p_Program);

    return true;
}


/****************************************************************************/
/*!
 *  \brief  Deletes one progarm
 *  \iparam ProgramID = Program ID associated with the program to be deleted
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CSepiaParameterSetList::DeleteProgram(QString ParameterID)
{
    if (m_ProgramList.contains(ParameterID)) {

        //get Program from ProgramList and free memory
        delete m_ProgramList.value(ParameterID);

        //remove pointer to Program from ProgramList
        m_ProgramList.remove(ParameterID);

        return true;
    } else {
        // there is no program with the given ProgramID
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes all the programs in the list
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CSepiaParameterSetList::DeleteAllPrograms()
{
    bool Result = true;

    qDeleteAll(m_ProgramList);

    m_ProgramList.clear();
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads all the programs from the xml stream
 *
 *  \iparam XmlStreamReader = Reference to xml stream to read from
 *  \iparam CompleteData = true to read Complete data of object
 *
 *  \return true - read success or false - read failure
 */
/****************************************************************************/
bool CSepiaParameterSetList::ReadAllPrograms(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    bool Result = true;
    // Look for node <Program>
    while(!XmlStreamReader.atEnd()) {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "ParameterSet") {

                DataManager::CSepiaParameterSet *Program = new DataManager::CSepiaParameterSet();

                if (!Program->DeserializeContent(XmlStreamReader, CompleteData)) {
                    qDebug() << "CProgram Deserialize failed!";
                    return false;
                }

                // Now add this ParameterSet
                if (!AddParameterSet(Program)) {
                    qDebug() << "CProgramList::Add Program failed!";
                    return false;
                }
            }
            else {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "ParameterSets") {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this Reagent";
            break; // exit from while loop
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads the CSepiaParameterSetList Data from QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice - Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
// the content of p_Device (xml stream expected) will be
// mirrored into this class/object
bool CSepiaParameterSetList::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{
//    QWriteLocker Locker(mp_ReadWriteLock);
    bool Result = true;
    QXmlStreamReader XmlStreamReader;

    XmlStreamReader.setDevice(&IODevice);

    // look for node <StainingPrograms>
    if (!DataManager::Helper::ReadNode(XmlStreamReader, "CoverslippingParameterSets")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: CoverslippingParameterSets";
        return false;
    }

    // Read attribute Version
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        return false;
    }
    SetVersion(XmlStreamReader.attributes().value("Version").toString().toInt());

    //  Now Delete the Program list and ordered list of Program ID's
    if (!DeleteAllPrograms()) {
        //throw error ...
        THROWARG(EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << FILE_LINE);
    }

    // Look for node <ProgramList>
    if (!DataManager::Helper::ReadNode(XmlStreamReader, "ParameterSets")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: ParameterSets";
        return false;
    }

    Result = ReadAllPrograms(XmlStreamReader, CompleteData);

    //if completedata
    if (CompleteData && Result) {
        if (!DataManager::Helper::ReadNode(XmlStreamReader, "NonXmlData")) {
            return false;
        }

        if (!XmlStreamReader.attributes().hasAttribute("FileName")) {
            qDebug() << "### attribute <FileName> is missing";
            return false;
        }
        SetFilename(XmlStreamReader.attributes().value("FileName").toString());


	}
    (void)XmlStreamReader.device()->reset();
//    qDebug()<<"\n\n\n PROGRAM LIST"<<XmlStreamReader.device()->readAll();
    return Result;
}

// the complete content of this class/object will be
// written into p_ByteArray as XML stream
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
bool CSepiaParameterSetList::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    //QReadLocker Locker(mp_ReadWriteLock);
    QString StringValue;
    QXmlStreamWriter XmlStreamWriter;

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    //XmlStreamWriter.setAutoFormattingIndent(4);
    XmlStreamWriter.writeStartDocument();

    XmlStreamWriter.writeDTD("<!DOCTYPE CoverslippingParameterSets>");
    XmlStreamWriter.writeStartElement("CoverslippingParameterSets");

    // write attribute version
    (void)StringValue.setNum(GetVersion()); //to suppress lint-534
    XmlStreamWriter.writeAttribute("Version", StringValue);

    //=========================================================================================
    // write section ParameterSets

    XmlStreamWriter.writeStartElement("ParameterSets");

    // write all programs
    QHashIterator<QString, CSepiaParameterSet*> i(m_ProgramList);
    while (i.hasNext())
    {
        i.next();
        CSepiaParameterSet *p_Prog = i.value();
        if (p_Prog != NULL) {
            if (!p_Prog->SerializeContent(XmlStreamWriter, CompleteData)) {
                qDebug("DataManager::CProgram SerializeContent failed ");
                return false;
            }
        }
    }

//    for (int i = 0; i < GetNumberOfPrograms(); i++) {
//        CSepiaParameterSet *p_Prog = GetParameterSet(i);
//        if (p_Prog != NULL) {
//            if (!p_Prog->SerializeContent(XmlStreamWriter, CompleteData)) {
//                qDebug("DataManager::CProgram SerializeContent failed ");
//                return false;
//            }
//        }
//    }
    XmlStreamWriter.writeEndElement(); // ProgramList

     //======NODE=======Temporary Data Variables=========================
    if (CompleteData) {
        XmlStreamWriter.writeStartElement("NonXmlData");

        XmlStreamWriter.writeAttribute("FileName", GetFilename());

        XmlStreamWriter.writeEndElement();
     }
     //======NODE=END====Temporary Data Variables=========================
    // The above Result has to be true to write the end element
    XmlStreamWriter.writeEndElement(); // for Coverslipping Parameter sets
    XmlStreamWriter.writeEndDocument(); // End of Document

    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ProgramList = CSepiaParameterSetList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator << (QDataStream& OutDataStream, const CSepiaParameterSetList& ProgramList) {

    CSepiaParameterSetList* p_TempProgramList = const_cast<CSepiaParameterSetList*>(&ProgramList);

    if (!p_TempProgramList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CProgramList::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam ProgramList = CSepiaParameterSetList class object
 *
 *  \return Input Stream
 */
/****************************************************************************/

QDataStream& operator >> (QDataStream& InDataStream, CSepiaParameterSetList& ProgramList) {

    if (!ProgramList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CProgramList::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceProgramList = CSepiaParameterSetList class object
 *
 *  \return CSepiaParameterSetList Class Object
 */
/****************************************************************************/
CSepiaParameterSetList& CSepiaParameterSetList::operator = (const CSepiaParameterSetList& SourceProgramList) {

    //Read should be blocked since internally Serialization takes place
    //QReadLocker Locker(mp_ReadWriteLock);
    // make sure not same object
    if (this != &SourceProgramList)
    {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        TempByteArray.clear();
        //Serialize
        DataStream << SourceProgramList;
        (void)DataStream.device()->reset();
        //Deserialize
        DataStream >> *this;
    }
    return *this;
}


} // namespace DataManager
