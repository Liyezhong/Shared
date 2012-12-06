/****************************************************************************/
/*! \file Rack.cpp
 *
 *  \brief CRack implementation.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2012-01-02, 2012-04-23
 *   $Author:  $ M.Scherer, Raju
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
#include "DataManager/Containers/Racks/Include/Rack.h"


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Constructor
 */
/****************************************************************************/
CRack::CRack() :
    m_RFIDUniqueID(0), m_RFIDUserData(0), m_ProgramID("0"), m_Transported(false), m_SepiaStation(false), m_UseSepia(false),
    m_StartTime(QDateTime::fromTime_t(0)), m_EndTimeColorado(Global::AdjustedTime::Instance().GetCurrentDateTime()), m_StationID("0"), m_ProgramStepIndex(0),
    m_EndTimeStation(QDateTime::fromTime_t(0)), m_Color("white"), m_EndTimeSepia(Global::AdjustedTime::Instance().GetCurrentDateTime()), m_Orientation(false),
    m_ActualSlideForecastValue(0), m_ActualSlides(0)
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor
 *
 *  \iparam RFIDUniqueID = Rack RFID
 *  \iparam RFIDUserData = Rack User data
 */
/****************************************************************************/
CRack::CRack(const quint32 RFIDUniqueID, const quint32 RFIDUserData) :
    m_RFIDUniqueID(RFIDUniqueID), m_ProgramID("0"), m_Transported(false), m_SepiaStation(false), m_UseSepia(false),
    m_StartTime(QDateTime::fromTime_t(0)), m_EndTimeColorado(QDateTime()), m_StationID("0"), m_ProgramStepIndex(0),
    m_EndTimeStation(QDateTime::fromTime_t(0)), m_Color("white"), m_EndTimeSepia(QDateTime::fromTime_t(0)),
    m_ActualSlideForecastValue(0), m_ActualSlides(0)
{
    // sets the color, orientation and size of the rack
    SetRFIDUserData(RFIDUserData);
    m_EndTimeColorado.setTimeSpec(Qt::UTC);
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam Rack = Instance of the CRack class
 *
 *  \return
 */
/****************************************************************************/
CRack::CRack(const CRack& Rack)
{
    // remove the constant using type cast
    CRack* p_TempRack = const_cast<CRack*>(&Rack);
    // do a deep copy of the data
    *this = *p_TempRack;
}


/****************************************************************************/
/*!
 *  \brief  Set the Rack RFID user data
 *
 *  \iparam RFIDUserData = Rack RF ID user data
 */
/****************************************************************************/
void CRack::SetRFIDUserData(const quint32 RFIDUserData)
{
    // set the user data
    m_RFIDUserData = RFIDUserData;

    // mask the data with 0x01 to get the orientation
    if (RFIDUserData & 0x01) {
        m_Orientation = true;
    }
    else
    {
        m_Orientation =  false;
    }

    // Right shift the bits by one and mask with 0x7F
    int Value = (RFIDUserData >> 1) & 0x7F;

    // set color of the Rack
    switch (Value) {        
        default:
        case WHITE:
            SetColor("white");
            break;
        case YELLOW:
            SetColor("yellow");
            break;
        case LIGHT_BLUE:
            SetColor("lightblue");
            break;
        case DARK_BLUE:
            SetColor("darkblue");
            break;
        case PINK:
            SetColor("pink");
            break;
        case RED:
            SetColor("red");
            break;
        case GREEN:
            SetColor("green");
            break;
        case BLACK:
            SetColor("black");
            break;
        case GREY:
            SetColor("grey");
            break;
    }
}


/****************************************************************************/
/*!
 *  \brief  Get the rack consists of 5 slides or not
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRack::IsRack5()
{    
    // Move the bits to right upto 8 times them Mask the data with 0xFF
    // this gives the size of the rack
    quint32 Value = (GetRFIDUserData() >> 8) & 0xFF;

    // If rack contains 5 slides then it will be saved as 5
    // same as applicable for the 20 slides and 30 slides
    if (Value == 5) {
        return true;
    }
    else {
        return false;
    }

}

/****************************************************************************/
/*!
 *  \brief  Streams the data items of a rack into a byte array
 *
 *  \iparam XmlStreamWriter = Xml stream writer to write the XML contents
 *  \iparam CompleteData = Complete data of the container
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRack::SerializeContent(QXmlStreamWriter &XmlStreamWriter, bool CompleteData)
{
    // write the start element
    XmlStreamWriter.writeStartElement("Rack");
    XmlStreamWriter.writeAttribute("RFIDUniqueID", QString::number(GetRFIDUniqueID()));
    XmlStreamWriter.writeAttribute("RFIDUserData", QString::number(GetRFIDUserData()));
    XmlStreamWriter.writeAttribute("ProgramID", GetProgramID());
    // check whether the Rack is transferrable or not
    if (GetTransported()) {
        XmlStreamWriter.writeAttribute("Transported", "true");
    }
    else {
        XmlStreamWriter.writeAttribute("Transported", "false");
    }
    // check for the Sepia station
    if (GetSepiaStation()) {
        XmlStreamWriter.writeAttribute("SepiaStation", "true");
    }
    else {
        XmlStreamWriter.writeAttribute("SepiaStation", "false");
    }

    // check for the Sepia usage
    if (GetUseSepia()) {
        XmlStreamWriter.writeAttribute("UseSepia", "true");
    }
    else {
        XmlStreamWriter.writeAttribute("UseSepia", "false");
    }

    XmlStreamWriter.writeAttribute("StartTime",GetStartTime().toString("'M'M'd'd'y'yyyyhh:mm:ss"));
    XmlStreamWriter.writeAttribute("EndTimeColorado", GetEndTimeColorado().toString("'M'M'd'd'y'yyyyhh:mm:ss"));
    XmlStreamWriter.writeAttribute("StationID", GetStationID());
    XmlStreamWriter.writeAttribute("ProgramStepIndex", QString::number(GetProgramStepIndex()));
    XmlStreamWriter.writeAttribute("EndTimeStation", GetEndTimeStation().toString("'M'M'd'd'y'yyyyhh:mm:ss"));
    XmlStreamWriter.writeAttribute("Color", GetColor());
    XmlStreamWriter.writeAttribute("EndTimeSepia",GetEndTimeSepia().toString("'M'M'd'd'y'yyyyhh:mm:ss"));
    XmlStreamWriter.writeAttribute("ActualSlides",QString::number(GetActualSlides()));
    XmlStreamWriter.writeAttribute("ActualSlideForecast",QString::number(GetActualSlideForecast()));

    if (CompleteData) {
        // Feature use
    }
    // write the end element
    XmlStreamWriter.writeEndElement();

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Streams the data items of a rack out of a byte array
 *
 *  \iparam XmlStreamReader = Xml reader to read the XML contents
 *  \iparam CompleteData = Complete data of the container
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRack::DeserializeContent(QXmlStreamReader &XmlStreamReader, bool CompleteData)
{
    // check RF ID attribute
    if (!XmlStreamReader.attributes().hasAttribute("RFIDUniqueID")) {
        qDebug() << "### attribute <RFIDUniqueID> is missing => abort reading";
        return false;
    }
    SetRFIDUniqueID(XmlStreamReader.attributes().value("RFIDUniqueID").toString().toInt());

    // check RF ID user data
    if (!XmlStreamReader.attributes().hasAttribute("RFIDUserData")) {
        qDebug() << "### attribute <RFIDUniqueID> is missing => abort reading";
        return false;
    }
    SetRFIDUserData(XmlStreamReader.attributes().value("RFIDUserData").toString().toInt());

    // check program ID attribute
    if (!XmlStreamReader.attributes().hasAttribute("ProgramID")) {
        qDebug() << "### attribute <ProgramID> is missing => abort reading";
        return false;
    }
    SetProgramID(XmlStreamReader.attributes().value("ProgramID").toString());

    // check Rack transferring flag attribute
    if (!XmlStreamReader.attributes().hasAttribute("Transported")) {
        qDebug() << "### attribute <Transported> is missing => abort reading";
        return false;
    }
    if (XmlStreamReader.attributes().value("Transported").toString() == "true") {
        SetTransported(true);
    } // else statement is not required because by default Transport flag is false
    else {
        SetTransported(false);
    }

    // check Sepia station flag attribute
    if (!XmlStreamReader.attributes().hasAttribute("SepiaStation")) {
        qDebug() << "### attribute <SepiaStation> is missing => abort reading";
        return false;
    }
    if (XmlStreamReader.attributes().value("SepiaStation").toString() == "true") {
        SetSepiaStation(true);
    }
    else {
        SetSepiaStation(false);
    }

    // check Sepia usage
    if (!XmlStreamReader.attributes().hasAttribute("UseSepia")) {
        qDebug() << "### attribute <UseSepia> is missing => abort reading";
        return false;
    }
    if (XmlStreamReader.attributes().value("UseSepia").toString() == "true") {
        SetUseSepia(true);
    }
    else {
        SetUseSepia(false);
    }

    // check Station time attribute
    if (!XmlStreamReader.attributes().hasAttribute("StartTime")) {
        qDebug() << "### attribute <StartTime> is missing => abort reading";
        return false;
    }
    QDateTime startTime = QDateTime::fromString(XmlStreamReader.attributes().value("StartTime").toString(), "'M'M'd'd'y'yyyyhh:mm:ss");
    startTime.setTimeSpec(Qt::UTC);
    SetStartTime(startTime);

    // check end time of colorado attribute
    if (!XmlStreamReader.attributes().hasAttribute("EndTimeColorado")) {
        qDebug() << "### attribute <EndTimeColorado> is missing => abort reading";
        return false;
    }
    QDateTime endTimeColorado = QDateTime::fromString(XmlStreamReader.attributes().value("EndTimeColorado").toString(), "'M'M'd'd'y'yyyyhh:mm:ss");
    endTimeColorado.setTimeSpec(Qt::UTC);
    SetEndTimeColorado(endTimeColorado);

    // check Station ID attribute
    if (!XmlStreamReader.attributes().hasAttribute("StationID")) {
        qDebug() << "### attribute <StationID> is missing => abort reading";
        return false;
    }
    SetStationID(XmlStreamReader.attributes().value("StationID").toString());

    // check Program step index attribute
    if (!XmlStreamReader.attributes().hasAttribute("ProgramStepIndex")) {
        qDebug() << "### attribute <ProgramStepIndex> is missing => abort reading";
        return false;
    }
    SetProgramStepIndex(XmlStreamReader.attributes().value("ProgramStepIndex").toString().toInt());

    // check end time of the station attribute
    if (!XmlStreamReader.attributes().hasAttribute("EndTimeStation")) {
        qDebug() << "### attribute <EndTimeStation> is missing => abort reading";
        return false;
    }
    QDateTime endTimeStation = QDateTime::fromString(XmlStreamReader.attributes().value("EndTimeStation").toString(), "'M'M'd'd'y'yyyyhh:mm:ss");
    endTimeStation.setTimeSpec(Qt::UTC);
    SetEndTimeStation(endTimeStation);

    // check color of the Rack attribute
    if (!XmlStreamReader.attributes().hasAttribute("Color")) {
        qDebug() << "### attribute <Color> is missing => abort reading";
        return false;
    }
    SetColor(XmlStreamReader.attributes().value("Color").toString());

    // check end time of Sepia attribute
    if (!XmlStreamReader.attributes().hasAttribute("EndTimeSepia")) {
        qDebug() << "### attribute <EndTimeSepia> is missing => abort reading";
        return false;
    }
    QDateTime endTimeSepia = QDateTime::fromString(XmlStreamReader.attributes().value("EndTimeSepia").toString(), "'M'M'd'd'y'yyyyhh:mm:ss");
    endTimeSepia.setTimeSpec(Qt::UTC);
    SetEndTimeSepia(endTimeSepia);

    // check Actual Slide
    if (!XmlStreamReader.attributes().hasAttribute("ActualSlides")) {
        qDebug() << "### attribute <ActualSlides> is missing => abort reading";
        return false;
    }
    SetActualSlides(XmlStreamReader.attributes().value("ActualSlides").toString().toInt());

    // check Actual Slide Forecast
    if (!XmlStreamReader.attributes().hasAttribute("ActualSlideForecast")) {
        qDebug() << "### attribute <ActualSlideForecast> is missing => abort reading";
        return false;
    }
    SetActualSlideForecast(XmlStreamReader.attributes().value("ActualSlideForecast").toString().toInt());

    // skip the current element and move to the next element
    XmlStreamReader.skipCurrentElement();
    if (CompleteData) {
        // Feature use
    }    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam Rack = CRack class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CRack& Rack)
{
    // remove the constant and store it in a local variable
    CRack* p_TempRack = const_cast<CRack*>(&Rack);
    QXmlStreamWriter XmlStreamWriter; ///< Writer for the XML

    // set the IO device
    QIODevice* IODevice = OutDataStream.device();

    XmlStreamWriter.setDevice(IODevice);

    // start the XML Document
    XmlStreamWriter.writeStartDocument();

    if (!p_TempRack->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CRack::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    // write enddocument
    XmlStreamWriter.writeEndDocument();    

    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam Rack = CRack class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CRack& Rack)
{
    QXmlStreamReader XmlStreamReader; ///< Reader for the XML
    // store the IO device
    QIODevice* IODevice = InDataStream.device();

    XmlStreamReader.setDevice(IODevice);

    // check whether xml reader at end or not
    while(!XmlStreamReader.atEnd()) {
        // read the next tag
        if (XmlStreamReader.readNext())
        {
            // check whther it is a start element or not
            if (XmlStreamReader.isStartElement()) {
                if (XmlStreamReader.name() == "Rack") {
                    // deserialize the content of the xml
                    if (!Rack.DeserializeContent(XmlStreamReader, true)) {
                        qDebug() << "CRack::Operator Streaming (DeSerializeContent) failed.";
                        // throws an exception
                        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
                    }
                }
            }
        }
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceRack = CRack class object
 *
 *  \return CRack Class Object
 */
/****************************************************************************/
CRack& CRack::operator=(const CRack& SourceRack)
{
    // make sure not same object
    if (this != &SourceRack)
    {
        // create the byte array
        QByteArray* p_TempByteArray = new QByteArray();
        // create the data stream to write into a file
        QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        p_TempByteArray->clear();
        // write the data into data stream from source
        DataStream << SourceRack;
        // reset the IO device pointer to starting position
        (void)DataStream.device()->reset(); //to avoid lint-534
        // read the data from data stream to destination object
        DataStream >> *this;

        delete p_TempByteArray;       
    }
    return *this;
}

}  // namespace DataManager

