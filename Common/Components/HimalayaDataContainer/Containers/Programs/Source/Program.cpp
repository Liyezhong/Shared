/****************************************************************************/
/*! \file Program.cpp
 *
 *  \brief Implementation file for class CProgram.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-01
 *  $Author:    $ F. Toth, N.Kamath
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Helper.h"
#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"
#include <QDataStream>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CProgram::CProgram() : CProgramBase("0","",""),
    m_LeicaProgram(false), m_Icon(""), m_Favorite(false),m_BottleCheck(true)
{
    Init();
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam ID = program ID
 */
/****************************************************************************/
CProgram::CProgram(const QString ID) : CProgramBase(ID,"",""),
     m_LeicaProgram(false), m_Icon(""), m_Favorite(false),m_BottleCheck(true)
{
    Init();
}


/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam ID = program ID
 *  \param Name =  QString type parameter
 *  \param LongName =  QString type parameter
 *  \param IsLeicaProgram =  bool type parameter
 *  \param Icon =  QString type parameter
 *  \param Favorite =  bool type parameter
 */
/****************************************************************************/
CProgram::CProgram(const QString ID, const QString Name, const QString LongName, bool IsLeicaProgram, QString Icon, bool Favorite):
    CProgramBase(ID, Name, LongName),
    m_LeicaProgram(IsLeicaProgram),
    m_Icon(Icon),
    m_Favorite(Favorite),
    m_BottleCheck(true)
{
    Init();
}

/****************************************************************************/
/*!
 *  \brief Initialize object
 */
/****************************************************************************/
void CProgram::Init()
{
    m_ReagentIDList.clear();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam SourceProgram
 *
 */
/****************************************************************************/
CProgram::CProgram(const CProgram& SourceProgram):CProgramBase(SourceProgram)
{
    Init();
    try {
        *this = SourceProgram;
    }
    catch(...) {
        qDebug()<<" CProgram::Copy constructor failed";
    }

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CProgram::~CProgram()
{
    try {
        m_ReagentIDList.clear();
    }
    catch (...){
        //to please lint.
    }

}

/****************************************************************************/
/*!
 *  \brief  Retreive program duration in second
 *  \return unsigned int
 */
/****************************************************************************/
quint32 CProgram::GetProgramDurationInSeconds() const
{
    quint32 duration = 0;
    for (qint32 I = 0; I < GetNumberOfSteps(); I++) {
        const CProgramStep *step = GetProgramStep(I);//use order index
        if (step) {
            duration += step->GetDurationInSeconds();
        }
    }
    return duration;
}

/****************************************************************************/
/*!
 *  \brief Writes the CProgram Data to QIODevice
 *
 *  \iparam XmlStreamWriter = XmlStream to Write
 *  \iparam CompleteData = if true writes Complete data of object into
 *                           the stream
 *
 *  \return true - success ,false - failure
 */
/****************************************************************************/
bool CProgram::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    //QReadLocker Locker(mp_ReadWriteLock);
    bool Result = true;

    XmlStreamWriter.writeStartElement("Program");
    XmlStreamWriter.writeAttribute("ID", GetID());
    XmlStreamWriter.writeAttribute("Name", GetName(true));
    XmlStreamWriter.writeAttribute("NameID", GetNameID());
    XmlStreamWriter.writeAttribute("Icon", GetIcon());
    if (IsLeicaProgram()) {
        XmlStreamWriter.writeAttribute("LeicaProgram", "true");
    } else {
        XmlStreamWriter.writeAttribute("LeicaProgram", "false");
    }

    if (IsFavorite()) {
        XmlStreamWriter.writeAttribute("Favorite", "true");
    } else {
        XmlStreamWriter.writeAttribute("Favorite", "false");
    }

//    if (GetBottleCheck()) {
//        XmlStreamWriter.writeAttribute("BottleCheck", "true");
//    } else {
//        XmlStreamWriter.writeAttribute("BottleCheck", "false");
//    }

    XmlStreamWriter.writeStartElement("StepList");
    XmlStreamWriter.writeAttribute("NextStepID", GetNextFreeStepID(false));


    for (int i = 0; i < GetNumberOfSteps() ; i++) {
        CProgramStep p_ProgStep;
        Result = GetProgramStep(i, p_ProgStep);
        if (Result) {
            if (!p_ProgStep.SerializeContent(XmlStreamWriter, CompleteData)) {
                qDebug("DataManager::CProgramStep SerializeContent failed ");
                return false;
            }
        }
    }
    XmlStreamWriter.writeEndElement(); // StepList
    XmlStreamWriter.writeEndElement(); // Program
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CProgram object.
 *
 *  \iparam XmlStreamReader = Xml stream to read from
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return true-success, false - failure
 */
/****************************************************************************/
bool CProgram::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
//    QWriteLocker Locker(mp_ReadWriteLock);
    bool Result = true;
    // ID
    if (!XmlStreamReader.attributes().hasAttribute("ID")) {
        qDebug() << "### attribute <ID> is missing => abort reading";
        return false;
    }

    SetID(XmlStreamReader.attributes().value("ID").toString());
    // NameID
    if (XmlStreamReader.attributes().hasAttribute("NameID")) {
        SetNameID(XmlStreamReader.attributes().value("NameID").toString());
    }

    // Name
    if (!XmlStreamReader.attributes().hasAttribute("Name")) {
        qDebug() << "### attribute <Name> is missing => abort reading";
        return false;
    }
    SetName(XmlStreamReader.attributes().value("Name").toString());

    // Icon
    if (!XmlStreamReader.attributes().hasAttribute("Icon")) {
        qDebug() << "### attribute <Icon> is missing => abort reading";
        return false;
    }
    SetIcon(XmlStreamReader.attributes().value("Icon").toString());

    // LeicaProgram
    if (!XmlStreamReader.attributes().hasAttribute("LeicaProgram")) {
        qDebug() << "### attribute <LeicaProgram> is missing => abort reading";
        return false;
    }

    if (XmlStreamReader.attributes().hasAttribute("BottleCheck")) {
        if (XmlStreamReader.attributes().value("BottleCheck").toString().toUpper() == "TRUE") {
            m_BottleCheck = true;
        }
        else {
            m_BottleCheck = false;
        }
    }
    if (XmlStreamReader.attributes().value("LeicaProgram").toString().toUpper() == "TRUE") {
        m_LeicaProgram = true;
    }
    else {
        m_LeicaProgram = false;
    }

    // Favorite
    if (!XmlStreamReader.attributes().hasAttribute("Favorite")) {
        qDebug() << "### attribute <Favorite> is missing => abort reading";
        return false;
    }
    if (XmlStreamReader.attributes().value("Favorite").toString().toUpper() == "TRUE") {
        m_Favorite = true;
    }
    else {
        m_Favorite = false;
    }
    // Look for node <StepList>
    if (!Helper::ReadNode(XmlStreamReader, "StepList")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: StepList";
        return false;
    }

    // get attribute NextStepID
    if (!XmlStreamReader.attributes().hasAttribute("NextStepID")) {
        qDebug() << "NextStepID missing";
        return false;
    } else {
        SetNextFreeStepID(XmlStreamReader.attributes().value("NextStepID").toString());
    }


    //Delete program steps
    (void)DeleteAllProgramSteps();
    m_ReagentIDList.clear();
    quint8 RowIndex = 0;
    while(!XmlStreamReader.atEnd()) {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
            return false;
        }
        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "Step") {
                //Create a program step
                CProgramStep* p_NewProgramStep = CreateProgramStep("Dummy");
                if (p_NewProgramStep != NULL) {
                    if (!p_NewProgramStep->DeserializeContent(XmlStreamReader, CompleteData)) {
                        qDebug() << "CProgramStep Deserialize failed!";
                        delete p_NewProgramStep;
                        return false;
                    }
                    if (!AddProgramStep(RowIndex, p_NewProgramStep)) {
                        qDebug() << "CProgram::Add ProgramStep failed!";
                        delete p_NewProgramStep;
                        return false;
                    }
                    m_ReagentIDList.append(p_NewProgramStep->GetReagentID());
                }
                RowIndex++;
            }
            else {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "StepList") {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this Reagent";
            break; // exit from while loop
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam Program = CProgram class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator << (QDataStream& OutDataStream, const CProgram& Program)
{

    CProgram* p_TempProgram = const_cast<CProgram*>(&Program);

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempProgram->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CProgram::Operator Streaming (SerializeContent) failed.";
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
 *  \iparam Program = CProgram class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >> (QDataStream& InDataStream, CProgram& Program)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());
    (void)XmlStreamReader.readElementText(QXmlStreamReader::IncludeChildElements); //lint -e534
    if (!Helper::ReadNode(XmlStreamReader, QString("Program"))) {
        qDebug() << "CProgram::Operator Streaming (DeSerializeContent) Node not found: Program";
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    if (!Program.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CProgram::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceProgram  = Program to assign from
 *
 *  \return CProgram Class Object
 */
/****************************************************************************/
CProgram& CProgram::operator = (const CProgram& SourceProgram)
{
    // make sure not same object
    if (this != &SourceProgram) {
        CProgramBase::operator =(SourceProgram);
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        TempByteArray.clear();
        try {
            DataStream << SourceProgram;
            (void)DataStream.device()->reset();
            DataStream >> *this;
        }
        catch(...) {
            //! \todo re-throw or Just log ? delete memory allocated ?
            (void)DeleteAllProgramSteps();
            qDebug()<<"CProgram::Assignment failed";
        }
    }
    return *this;
}

}  // namespace DataManager

