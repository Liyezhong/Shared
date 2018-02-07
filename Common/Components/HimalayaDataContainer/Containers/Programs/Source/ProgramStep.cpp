/****************************************************************************/
/*! \file ProgramStep.cpp
 *
 *  \brief Implementation file for class CProgramStep.
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

#include <QStringList>
#include <QDebug>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Helper.h"
#include "HimalayaDataContainer/Containers/Programs/Include/ProgramStep.h"
#include <QDataStream>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CProgramStep::CProgramStep():
    m_StepID("INVALID"), m_ReagentID("0"),
    m_Duration("0s"),m_Temperature("0"),m_Pressure("Off"),m_Vacuum("On"),m_NumberOfSteps(0)
{
    m_StationIDList.clear();
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam StepID
 */
/****************************************************************************/
CProgramStep::CProgramStep(QString StepID):
    m_StepID(StepID), m_ReagentID("0"),
    m_Duration("0s"),m_Temperature("0"),m_Pressure("Off"),m_Vacuum("On"),m_NumberOfSteps(0)
{
    m_StationIDList.clear();
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \param StepID
 *  \param ReagentID
 *  \param Duration
 *  \param Temperature
 *  \param Pressure
 *  \param Vacuum
 */
/****************************************************************************/
CProgramStep::CProgramStep(const QString StepID, const QString ReagentID, const QString Duration,
                           const QString Temperature, const QString Pressure, const QString Vacuum):
    m_StepID(StepID), m_ReagentID(ReagentID),m_Duration(Duration),m_Temperature(Temperature),
    m_Pressure(Pressure), m_Vacuum(Vacuum),m_NumberOfSteps(0)
{
     m_StationIDList.clear();
}


/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam SourceProgramStep
 */
/****************************************************************************/
CProgramStep::CProgramStep(const CProgramStep &SourceProgramStep)
{
    m_StationIDList.clear();
    *this = SourceProgramStep;
}

/****************************************************************************/
/*!
 *  \brief Writes from the CProgramStep object to a IODevice.
 *
 *  \iparam XmlStreamWriter = XML stream to write into.
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CProgramStep::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    Q_UNUSED(CompleteData)
    bool Result = true;
    XmlStreamWriter.writeStartElement("Step");
    XmlStreamWriter.writeAttribute("ID", GetStepID());
    XmlStreamWriter.writeAttribute("ReagentID", GetReagentID());

    XmlStreamWriter.writeAttribute("Duration", GetDuration());
    XmlStreamWriter.writeAttribute("Temperature", GetTemperature());
    XmlStreamWriter.writeAttribute("Pressure", GetPressure());
    XmlStreamWriter.writeAttribute("Vacuum", GetVacuum());

    XmlStreamWriter.writeEndElement(); //Step
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CProgramStep object.
 *
 *  \iparam XmlStreamReader  = XML stream to read from
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CProgramStep::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    Q_UNUSED(CompleteData)
    // get attribute ID
    if (!XmlStreamReader.attributes().hasAttribute("ID")) {
        qDebug() << "### attribute <ID> is missing => abort reading";
        return false;
    }
    SetStepID(XmlStreamReader.attributes().value("ID").toString());

    // get reagent ID
    if (!XmlStreamReader.attributes().hasAttribute("ReagentID")) {
        qDebug() << "### attribute <ReagentID> is missing => abort reading";
        return false;
    }
    SetReagentID(XmlStreamReader.attributes().value("ReagentID").toString());


    // get Duration
    if (!XmlStreamReader.attributes().hasAttribute("Duration")) {
        qDebug() << "### attribute <Duration> is missing => abort reading";
        return false;
    }
    SetDuration(XmlStreamReader.attributes().value("Duration").toString());


    // get Temperature
    if (!XmlStreamReader.attributes().hasAttribute("Temperature")) {
        qDebug() << "### attribute <Temperature> is missing => abort reading";
        return false;
    }
    SetTemperature(XmlStreamReader.attributes().value("Temperature").toString());


    // get Pressure
    if (!XmlStreamReader.attributes().hasAttribute("Pressure")) {
        qDebug() << "### attribute <Pressure> is missing => abort reading";
        return false;
    }
    SetPressure(XmlStreamReader.attributes().value("Pressure").toString());

    // get Vacuum
    if (!XmlStreamReader.attributes().hasAttribute("Vacuum")) {
        qDebug() << "### attribute <Vacuum> is missing => abort reading";
        return false;
    }
    SetVacuum(XmlStreamReader.attributes().value("Vacuum").toString());

    return true;
}


/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ProgramStep = CProgramStep class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator << (QDataStream& OutDataStream, const CProgramStep& ProgramStep) {

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());
    CProgramStep *p_TempProgStep = const_cast<CProgramStep*>(&ProgramStep);
    if (!p_TempProgStep->SerializeContent(XmlStreamWriter, true)) {
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
 *  \iparam ProgramStep = CProgramStep class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >> (QDataStream& InDataStream, CProgramStep& ProgramStep) {

    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());
    if (!Helper::ReadNode(XmlStreamReader, QString("Step"))) {
        qDebug() << "CProgram::Operator Streaming (DeSerializeContent) Node not found: Reagent";
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    if (!ProgramStep.DeserializeContent(XmlStreamReader, true)) {
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
 *  \iparam SourceProgramStep = CProgramStep class object
 *
 *  \return CProgramStep Class Object
 */
/****************************************************************************/
CProgramStep& CProgramStep::operator = (const CProgramStep& SourceProgramStep) {

    // make sure not same object
    if (this != &SourceProgramStep) {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        TempByteArray.clear();
        DataStream << SourceProgramStep;
        (void)DataStream.device()->reset();
        DataStream >> *this;
    }
    return *this;
}

}  // namespace DataManager
