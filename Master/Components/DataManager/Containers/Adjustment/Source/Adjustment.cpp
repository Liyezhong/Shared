/****************************************************************************/
/*! \file Adjustment.cpp
 *
 *  \brief CAdjustment implementation.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-21
 *  $Author:    $ Stalin
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
#include <QReadLocker>
#include <QWriteLocker>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QList>

#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/Adjustment/Include/Adjustment.h"
#include <Global/Include/EventObject.h>

namespace DataManager {

CAdjustment::CAdjustment() : m_Version(0), m_DataVerificationMode(true), m_Filename("")
{
    Init();
}

CAdjustment::CAdjustment(const CAdjustment &Adjustment)
{
    CAdjustment* p_Adjustment = const_cast<CAdjustment*>(&Adjustment);
    *this = *p_Adjustment;
}

CAdjustment::~CAdjustment()
{

}

/****************************************************************************/
/*!
 *  \brief Reads the data and stores in CProgramSequenceList from the file
 *
 *  \iparam Filename = Name of the Input File
 *
 *  \return True or False
 */
/****************************************************************************/
bool CAdjustment::Read(QString Filename)
{
    bool Result;

    //check if file exists
    if (!QFile::exists(Filename))
    {
        qDebug("File doesn't exists");
        return false;
    }

    if (m_DataVerificationMode)
    {
        CAdjustment* p_AdjVerification = new CAdjustment();

//        *p_AdjVerification = *this;

        p_AdjVerification->SetDataVerificationMode(false);

        Result = true;
        if (!p_AdjVerification->Read(Filename))
        {
            qDebug() << "CAdjustment::Read - file read failed";
            Result = false;
        }
        else
        {
            // now check new content => call all active verifiers
            if (DoLocalVerification(p_AdjVerification))
            {
                qDebug() << "CAdjustment::Read - clone backwards";
                // if content ok, clone backwards
                *this = *p_AdjVerification;

                //Now initialize the filename member since Read is succcess
                m_Filename = Filename;

                Result = true;
            }
            else
            {
                qDebug() << "CAdjustment::Read - verification failed";
                Result = false;
            }

            delete p_AdjVerification;
        }
    }
    else
    {
        // clear content
        Init();

        m_Filename = "UNDEFINED";
        QFile File (Filename);
        if (!File.open(QFile::ReadOnly | QFile::Text))
        {
            qDebug() << "CAdjustment::Read - file open failed :" << Filename;
            return false;
        }

        if (!DeserializeContent(File, false))
        {
            qDebug() << "CAdjustment::Read - read failed: " << Filename;
            return false;
        }

        File.close();

        //Now initialize the filename member since Read is succcess
        m_Filename = Filename;

        Result = true;
    }

    return Result;
}

bool CAdjustment::SerializeContent(QIODevice &p_Device, bool CompleteData)
{
    Q_UNUSED(CompleteData)

    //QReadLocker Locker(mp_ReadWriteLock);
    QString StringValue;
    QXmlStreamWriter XmlStreamWriter;
    bool Result;

    XmlStreamWriter.setDevice(&p_Device);
    XmlStreamWriter.setAutoFormatting(true);
    XmlStreamWriter.writeStartDocument();

    XmlStreamWriter.writeDTD("<!DOCTYPE colorado_adjustment_data>");
    XmlStreamWriter.writeStartElement("colorado_adjustment");

    // write attribute version
    (void)StringValue.setNum(GetVersion()); //to suppress lint-534
    XmlStreamWriter.writeAttribute("version", StringValue);

    XmlStreamWriter.writeStartElement("adjustment_data");

    // Serialize Left Xyz content
    Result = SerializeXyzContent(XmlStreamWriter, LEFT_XYZ);

    if (false == Result)
    {
        qDebug() << "SerializeContent: abort reading. writing failed: grappler_left";
        return false;
    }

    // Serialize Right Xyz content
    Result = SerializeXyzContent(XmlStreamWriter, RIGHT_XYZ);

    if (false == Result)
    {
        qDebug() << "SerializeContent: abort reading. writing failed: grappler_right";
        return false;
    }

    XmlStreamWriter.writeEndElement();  // <adjustment_data>

    XmlStreamWriter.writeEndDocument(); // End of Document

    return true;
}

bool CAdjustment::DeserializeContent(QIODevice &p_Device, bool CompleteData)
{
    Q_UNUSED(CompleteData)

    bool Result = true;
    QXmlStreamReader XmlStreamReader;

    XmlStreamReader.setDevice(&p_Device);

    // look for node <colorado_adjustment>
    if (!Helper::ReadNode(XmlStreamReader, "colorado_adjustment"))
    {
        qDebug() << "DeserializeContent: abort reading. Node not found: colorado_adjustment";
        return false;
    }

    // Read attribute Version
    if (!XmlStreamReader.attributes().hasAttribute("version"))
    {
        qDebug() << "### attribute <Version> is missing => abort reading";
        return false;
    }

    SetVersion(XmlStreamReader.attributes().value("version").toString().toInt());

    // Read rest of fields & attributes
    // Deserialize Left Xyz content
    Result = DeserializeXyzContent(XmlStreamReader, LEFT_XYZ);

    if (false == Result)
    {
        qDebug() << "DeserializeContent: abort reading. Parsing failed: grappler_left";
        return false;
    }

    // Deserialize Right Xyz content
    Result = DeserializeXyzContent(XmlStreamReader, RIGHT_XYZ);

    if (false == Result)
    {
        qDebug() << "DeserializeContent: abort reading. Parsing failed: grappler_right";
        return false;
    }

    (void)XmlStreamReader.device()->reset();

    return Result;
}

bool CAdjustment::GetXyzPosition(Xyz_t Xyz, quint8 Row, quint8 Column, PositionXYZ &Position)
{
    bool Result = true;

    // Check boundary conditions
    if ((Row > XYZ_MAX_ROWS) || (Column > XYZ_MAX_COLS))
    {
        // Error
        Result = false;
    }

    if (LEFT_XYZ == Xyz)
    {
        Position = m_StaionPosLeft[Column][Row];
    }
    else if (RIGHT_XYZ == Xyz)
    {
        Position = m_StaionPosRight[Column][Row];
    }
    else
    {
        // Error
        Result = false;
    }

    return Result;
}

bool CAdjustment::SetXyzPosition(Xyz_t Xyz, quint8 Row, quint8 Column, PositionXYZ Position)
{
    bool Result = true;

    // Check boundary conditions
    if ((Row > XYZ_MAX_ROWS) || (Column > XYZ_MAX_COLS))
    {
        // Error
        Result = false;
    }

    if (LEFT_XYZ == Xyz)
    {
        m_StaionPosLeft[Column][Row] = Position;
    }
    else if (RIGHT_XYZ == Xyz)
    {
        m_StaionPosRight[Column][Row] = Position;
    }
    else
    {
        // Error
        Result = false;
    }

    return Result;
}

bool CAdjustment::SerializeXyzContent(QXmlStreamWriter &XmlStreamWriter, Xyz_t Xyz)
{
    quint8 RowIndex;
    quint8 ColumnIndex;
    PositionXYZ Position;
    QString StringValue;

    if (LEFT_XYZ == Xyz)
    {
        XmlStreamWriter.writeStartElement("grappler_left");
    }
    else
    {
        XmlStreamWriter.writeStartElement("grappler_right");
    }

    for (RowIndex = 0; RowIndex < XYZ_MAX_ROWS; RowIndex++)
    {
        XmlStreamWriter.writeStartElement("row");

        // write attribute index
        (void)StringValue.setNum(RowIndex + 1);
        XmlStreamWriter.writeAttribute("index", StringValue);

        // write all vessel positions
        for (ColumnIndex = 0; ColumnIndex < XYZ_MAX_COLS; ColumnIndex++)
        {
            GetXyzPosition(Xyz, RowIndex, ColumnIndex, Position);

            XmlStreamWriter.writeStartElement("vessel");

            (void)StringValue.setNum(ColumnIndex + 1);
            XmlStreamWriter.writeAttribute("pos", StringValue);

            (void)StringValue.setNum(Position.PositionY);
            XmlStreamWriter.writeAttribute("row", StringValue);

            (void)StringValue.setNum(Position.PositionX);
            XmlStreamWriter.writeAttribute("column", StringValue);

            (void)StringValue.setNum(Position.PositionZ);
            XmlStreamWriter.writeAttribute("depth", StringValue);

            XmlStreamWriter.writeEndElement();  // <\vessel>
        }

        XmlStreamWriter.writeEndElement();      // <\row>
    }

    XmlStreamWriter.writeEndElement();

    return true;
}

bool CAdjustment::DeserializeXyzContent(QXmlStreamReader &XmlStreamReader, Xyz_t Xyz)
{
    bool Result = true;

    quint8 RowIndex;
    quint8 ColumnIndex;
    PositionXYZ Position;

    if (LEFT_XYZ == Xyz)
    {
        // Look for node <grappler_left>
        if (!Helper::ReadNode(XmlStreamReader, "grappler_left"))
        {
            qDebug() << "DeserializeContent: abort reading. Node not found: grappler_left";
            return false;
        }
    }
    else
    {
        // Look for node <grappler_right>
        if (!Helper::ReadNode(XmlStreamReader, "grappler_right"))
        {
            qDebug() << "DeserializeContent: abort reading. Node not found: grappler_right";
            return false;
        }
    }

    // Look for node <row>
    while(!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1)
        {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement() && XmlStreamReader.name() == "row")
        {
            // Index (Row Index)
            if (!XmlStreamReader.attributes().hasAttribute("index"))
            {
                qDebug() << "### attribute <index> is missing => abort reading";
                return false;
            }

            RowIndex = XmlStreamReader.attributes().value("index").toString().toUInt();

            while(!XmlStreamReader.atEnd())
            {
                if (static_cast<int>(XmlStreamReader.readNext()) == 1)
                {
                    qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
                    qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
                }

                if (XmlStreamReader.isStartElement() && XmlStreamReader.name() == "vessel")
                {
                    // pos (column index)
                    if (!XmlStreamReader.attributes().hasAttribute("pos"))
                    {
                        qDebug() << "### attribute <pos> is missing => abort reading";
                        return false;
                    }

                    ColumnIndex = XmlStreamReader.attributes().value("pos").toString().toUInt();

                    // row (y position)
                    if (!XmlStreamReader.attributes().hasAttribute("row"))
                    {
                        qDebug() << "### attribute <row> is missing => abort reading";
                        return false;
                    }

                    Position.PositionY = XmlStreamReader.attributes().value("row").toString().toUInt();

                    // column (x position)
                    if (!XmlStreamReader.attributes().hasAttribute("column"))
                    {
                        qDebug() << "### attribute <column> is missing => abort reading";
                        return false;
                    }

                    Position.PositionX = XmlStreamReader.attributes().value("column").toString().toUInt();

                    // depth (z position)
                    if (!XmlStreamReader.attributes().hasAttribute("depth"))
                    {
                        qDebug() << "### attribute <depth> is missing => abort reading";
                        return false;
                    }

                    Position.PositionZ = XmlStreamReader.attributes().value("depth").toString().toUInt();

                    SetXyzPosition(Xyz, RowIndex-1, ColumnIndex-1, Position);

//                    qDebug() << Xyz << RowIndex - 1 << ColumnIndex - 1 << Position.PositionX
//                             << Position.PositionY << Position.PositionZ;
                }
                else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "row")
                {
                    break; // exit from while loop
                }
            }
        }
        else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "grappler_left")
        {
            //qDebug() << "It has reached the end of the particular Node , Stop reading for this Reagent";
            break; // exit from while loop
        }
    }

    return Result;
}

void CAdjustment::Init()
{
    quint8 Row;
    quint8 Column;

    for (Row = 0; Row < XYZ_MAX_ROWS; Row++)
    {
        for (Column = 0; Column < XYZ_MAX_COLS; Column++)
        {
            m_StaionPosLeft[Column][Row].Clear();
            m_StaionPosRight[Column][Row].Clear();
        }
    }

    m_Version = 0;
    m_Filename = "";
}

CAdjustment& CAdjustment::operator =(const CAdjustment& SourceProgramSequenceList)
{
    // make sure not same object
    if (this != &SourceProgramSequenceList)
    {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        DataStream << SourceProgramSequenceList;
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

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam Adjustment = CAdjustment class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CAdjustment& Adjustment)
{
    CAdjustment* p_TempProgramList = const_cast<CAdjustment*>(&Adjustment);

    if (!p_TempProgramList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CAdjustment::Operator Streaming (SerializeContent) failed.";
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
 *  \iparam Adjustment = CAdjustment class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CAdjustment& Adjustment)
{
    if (!Adjustment.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CAdjustment::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

}
