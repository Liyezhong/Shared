/****************************************************************************/
/*! \file StationBase.cpp
 *
 *  \brief StationBase class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-11-29
 *   $Author:  $ Michael Thiel
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Containers/Stations/Include/StationBase.h"
#include <DataManager/Helper/Include/Helper.h>

namespace DataManager {


/****************************************************************************/
/*!
 *  \brief Default Constructor for Station class.
*/
/****************************************************************************/
CStationBase::CStationBase() :
    m_StationID("0"),
    m_StationTemp(0),
    m_Defect(false),
    m_Disabled(false)
{  
}

/****************************************************************************/
/*!
 *  \brief Parameterized constructor for Station class.
 *
 *  \iparam ID = Unique ID of station
 */
/****************************************************************************/
CStationBase::CStationBase(const QString ID) :
    m_StationID(ID),
    m_StationTemp(33),
    m_Defect(false),
    m_Disabled(false)
{
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam Station = Instance of the CStationBase class
 *
 *  \return
 */
/****************************************************************************/
CStationBase::CStationBase(const CStationBase& Station)
{
    CopyFromOther(Station);
}

void CStationBase::CopyFromOther(const CStationBase &Station)
{
    CStationBase &OtherStation = const_cast<CStationBase &>(Station);
    m_Defect = OtherStation.IsStationDefect();
    m_HeatingState = OtherStation.GetHeatingState();
    m_StationID = OtherStation.GetStationID();
    m_StationTemp = OtherStation.GetStationTemperature();
    m_Disabled = OtherStation.IsStationDisabled();
}

/****************************************************************************/
/*!
 *  \brief Writes to the Station object file from a XmlStreamWriter.
 *
 *  \iparam XmlStreamWriter = Instance of XmlStreamWriter Class
 *  \iparam CompleteData = Complete class Data , true or false
 *
 *  \return True or False
 */
/****************************************************************************/
bool CStationBase::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    QString StringValue;
    bool Result = true;

    XmlStreamWriter.writeStartElement("Station"); //Station

    XmlStreamWriter.writeAttribute("ID", GetStationID()); // Unique ID of the station

    //======NODE=======Temporary Data Variables=========================
    if (CompleteData) {
        // Need to work on the m_ReagentTemp
        XmlStreamWriter.writeAttribute("Temperature", QString::number(GetStationTemperature()));
        // Station is defect or not
        if (IsStationDefect())
        {
            XmlStreamWriter.writeAttribute("Defect", "true");
        }
        else
        {
            XmlStreamWriter.writeAttribute("Defect", "false");
        }
        if (IsStationDisabled())
        {
            XmlStreamWriter.writeAttribute("Disabled", "true");
        }
        else
        {
            XmlStreamWriter.writeAttribute("Disabled", "false");
        }
    }
    //======NODE=END====Temporary Data Variables=========================
    XmlStreamWriter.writeEndElement(); //Station
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XML file to a Station Class.
 *
 *  \iparam XmlStreamReader = Instance of XmlStreamReader Class
 *  \iparam CompleteData = Complete class Data , true or false
 *
 *  \return True or False
 */
/****************************************************************************/
bool CStationBase::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    // Station ID
    if (!XmlStreamReader.attributes().hasAttribute("ID")) {
        qDebug() << "### attribute <ID> is missing => abort reading";
        return false;
    }
    SetStationID(XmlStreamReader.attributes().value("ID").toString());


    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // TODO need to take care of m_Defect and m_StationTemp
        //Station Temperature
        if (!XmlStreamReader.attributes().hasAttribute("Temperature")) {
            qDebug() << "### attribute <Temperature> is missing => abort reading";
            return false;
        }
        SetStationTemperature(XmlStreamReader.attributes().value("Temperature").toString().toInt());

        //Defect
        if (!XmlStreamReader.attributes().hasAttribute("Defect")) {
            qDebug() << "### attribute <Defect> is missing => abort reading";
            return false;
        } else {
            bool Value = false;
            if (XmlStreamReader.attributes().value("Defect").toString().toUpper() == "TRUE") {
                Value = true;
            }
            SetStationDefect(Value);
        }

        //Disabled
        if (!XmlStreamReader.attributes().hasAttribute("Disabled")) {
            qDebug() << "### attribute <Disabled> is missing => abort reading";
            return false;
        } else {
            bool Value = false;
            if (XmlStreamReader.attributes().value("Disabled").toString().toUpper() == "TRUE") {
                Value = true;
            }
            SetStationDisabled(Value);
        }
    }
    //======NODE=END====Temporary Data Variables=========================
    return true;
}

/****************************************************************************/
/*!
 *  \brief Derives the Station type from Station ID.
 *
 *  \return Station Type DRY , OVEN , REAGENT
 */
/****************************************************************************/

StationsType_t CStationBase::GetStationType()
{
    QString StationID = GetStationID();
    if (StationID.isEmpty()) {
        qDebug() << "### CReagent::GetStationType Unknown Station Type: " << StationID;
        return INVALID_TYPE;
    }
    if(StationID.at(0) == 'D') {
        return DRY;
    }
    else if(StationID.at(0) == 'H') {
        return HEATED;
    }
    else if(StationID.at(0) == 'L') {
        return LOAD;
    }
    else if(StationID.at(0) == 'O') {
        return OVEN;
    }
    else if(StationID.at(0) == 'P') {
        return PARK;
    }
    else if(StationID.at(0) == 'R') {
        return REAGENT;
    }
    else if(StationID.at(0) == 'S') {
        return SLIDE;
    }
    else if(StationID.at(0) == 'T') {
        return TRANSFER;
    }
    else if(StationID.at(0) == 'U') {
        return UNLOAD;
    }
    else if(StationID.at(0) == 'W') {
        return WATER;
    }
    else {
        qDebug() << "### CReagent::GetStationType Unknown Station Type: " << m_StationID.at(0);
        return INVALID_TYPE;
    }
}


/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam Station = CStationBase class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CStationBase& Station)
{
    CStationBase* p_TempStation = const_cast<CStationBase*>(&Station);

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempStation->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CStationBase::Operator Streaming (SerializeContent) failed.";
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
 *  \iparam Station = CStationBase class object
 *
 *  \return Input Stream
 */
/****************************************************************************/

QDataStream& operator >>(QDataStream& InDataStream, CStationBase& Station)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

//    while (!XmlStreamReader.atEnd())
//    {
//        qDebug() << XmlStreamReader.name() << XmlStreamReader.tokenType();
//        XmlStreamReader.readNext();
//    }

    if (!DataManager::Helper::ReadNode(XmlStreamReader, QString("Station"))) {
        qDebug() << "CStationBase::Operator Streaming (DeSerializeContent) Node not found: Reagent";
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    if (!Station.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CStationBase::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }    

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceStation = CStationBase class object
 *
 *  \return CStationBase Class Object
 *  \note   Swap idiom is not used since CStationBase doesnt manage any resource
 *          and exception wont be throwns
 */
/****************************************************************************/
CStationBase& CStationBase::operator=(const CStationBase& SourceStation)
{
    CopyFromOther(SourceStation);
    return *this;
}

}  // namespace DataManager
