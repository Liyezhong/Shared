/****************************************************************************/
/*! \file DashboardStationBase.cpp
 *
 *  \brief DashboardStationBase class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16-Jan-2013
 *   $Author:  $ Santhosha Kumar Raju
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
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
#include "DataManager/Containers/DashboardStations/Include/DashboardStationBase.h"
#include <DataManager/Helper/Include/Helper.h>

namespace DataManager {


/****************************************************************************/
/*!
 *  \brief Default Constructor for Station class.
*/
/****************************************************************************/
CDashboardStationBase::CDashboardStationBase() :
    m_DashboardStationID("0"),
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
CDashboardStationBase::CDashboardStationBase(const QString ID) :
    m_DashboardStationID(ID),
    m_Defect(false),
    m_Disabled(false)
{
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam Station = Instance of the CDashboardStationBase class
 *
 *  \return
 */
/****************************************************************************/
CDashboardStationBase::CDashboardStationBase(const CDashboardStationBase& Station)
{
    CopyFromOther(Station);
}

void CDashboardStationBase::CopyFromOther(const CDashboardStationBase &Station)
{
    CDashboardStationBase &OtherStation = const_cast<CDashboardStationBase &>(Station);
    m_Defect = OtherStation.IsDashboardStationDefect();
    m_DashboardStationID = OtherStation.GetDashboardStationID();
    m_Disabled = OtherStation.IsDashboardStationDisabled();
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
bool CDashboardStationBase::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    QString StringValue;
    bool Result = true;

    XmlStreamWriter.writeStartElement("Station"); //Station

    XmlStreamWriter.writeAttribute("ID", GetDashboardStationID()); // Unique ID of the station
/**
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
**/
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
bool CDashboardStationBase::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    // Station ID
    if (!XmlStreamReader.attributes().hasAttribute("ID")) {
        qDebug() << "### attribute <ID> is missing => abort reading";
        return false;
    }
    SetDashboardStationID(XmlStreamReader.attributes().value("ID").toString());

/**
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
**/
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

DashboardStationType_t CDashboardStationBase::GetDashboardStationType()
{
    return INVALID_STATION;

}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam Station = CDashboardStationBase class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CDashboardStationBase& Station)
{
    CDashboardStationBase* p_TempStation = const_cast<CDashboardStationBase*>(&Station);

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempStation->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CDashboardStationBase::Operator Streaming (SerializeContent) failed.";
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
 *  \iparam Station = CDashboardStationBase class object
 *
 *  \return Input Stream
 */
/****************************************************************************/

QDataStream& operator >>(QDataStream& InDataStream, CDashboardStationBase& Station)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

    if (!DataManager::Helper::ReadNode(XmlStreamReader, QString("DashboardStation"))) {
        qDebug() << "CDashboardStationBase::Operator Streaming (DeSerializeContent) Node not found: Reagent";
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    if (!Station.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CDashboardStationBase::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }    

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceStation = CDashboardStationBase class object
 *
 *  \return CDashboardStationBase Class Object
 *  \note   Swap idiom is not used since CDashboardStationBase doesnt manage any resource
 *          and exception wont be throwns
 */
/****************************************************************************/
CDashboardStationBase& CDashboardStationBase::operator=(const CDashboardStationBase& SourceStation)
{
    CopyFromOther(SourceStation);
    return (*this);
}

}  // namespace DataManager
