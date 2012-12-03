/****************************************************************************/
/*! \file SWDetails.cpp
 *
 *  \brief Implementation file for class CSWDetails.
 *         This class reads the SW information from the "SW_version.xml" file
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-07
 *  $Author:    $ Raju
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
#include <iostream>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Containers/SWVersions/Include/SWDetails.h"


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 *
 */
/****************************************************************************/
CSWDetails::CSWDetails() :
    m_SWName(""),
    m_SWVersion(""),
    m_SWDate("")
{
}

/****************************************************************************/
/*!
 *  \brief  Parameterized Constructor
 *
 */
/****************************************************************************/
CSWDetails::CSWDetails(const QString SWName) :
    m_SWName(SWName),
    m_SWVersion(""),
    m_SWDate("")
{
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam SWDetails = Instance of the CSWDetails class
 *
 *  \return
 */
/****************************************************************************/
CSWDetails::CSWDetails(const CSWDetails& SWDetails)
{
    CSWDetails* p_TempSWDetails = const_cast<CSWDetails*>(&SWDetails);

    *this = *p_TempSWDetails;
}

/****************************************************************************/
/*!
 *  \brief Writes from the CSWDetails object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the QXmlStreamWriter
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWDetails::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    // write the main tag to store all the details
    // write the start element
    XmlStreamWriter.writeStartElement("file");// <file>

    XmlStreamWriter.writeAttribute("Filename", m_SWName);

    XmlStreamWriter.writeAttribute("Version", m_SWVersion);

    XmlStreamWriter.writeAttribute("Date", m_SWDate);


    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        QString SWType;
        // store temporary data
        switch(m_SWType) {
            default:
            case MASTERSOFTWARE:
                SWType = "0";
                break;
            case FIRMWARE:
                SWType = "1";
                break;
        }
        XmlStreamWriter.writeAttribute("SWType", SWType);

    }
    //======NODE=END====Temporary Data Variables=========================
    XmlStreamWriter.writeEndElement(); // </file>

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XmlStreamReader to CSWDetails object.
 *
 *  \iparam XmlStreamReader = Instance of the XmlStreamReader.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSWDetails::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    bool Result = true;    

    // FileName
    if (!XmlStreamReader.attributes().hasAttribute("Filename")) {
        qDebug() << "### attribute <Filename> is missing => abort reading";
        return false;
    }
    // set the SW file name
    SetSWName(XmlStreamReader.attributes().value("Filename").toString());

    // Vesrion
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        return false;
    }
    // set the SW version
    SetSWVersion(XmlStreamReader.attributes().value("Version").toString());

    // Date
    if (!XmlStreamReader.attributes().hasAttribute("Date")) {
        qDebug() << "### attribute <Date> is missing => abort reading";
        return false;
    }
    // set the SW version
    SetSWDate(XmlStreamReader.attributes().value("Date").toString());

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // Date
        if (!XmlStreamReader.attributes().hasAttribute("SWType")) {
            qDebug() << "### attribute <SWType> is missing => abort reading";
            return false;
        }
        switch(XmlStreamReader.attributes().value("SWType").toString().toInt()) {
            default:
            case 0:
                m_SWType = MASTERSOFTWARE;
                break;
            case 1:
                m_SWType = FIRMWARE;
                break;
        }

        // set the SW version
        SetSWType(m_SWType);
    }
    //======NODE=END====Temporary Data Variables=========================

    return Result;
}


/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam SWDetails = CSWDetails class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CSWDetails& SWDetails)
{
    CSWDetails* p_TempSWDetails = const_cast<CSWDetails*>(&SWDetails);

    QXmlStreamWriter XmlStreamWriter;
    // set the device to streamer to write the file
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempSWDetails->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CSWDetails::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the QDataStream
 *  \iparam SWDetails = CSWDetails class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CSWDetails& SWDetails)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

    if (!Helper::ReadNode(XmlStreamReader, QString("file"))) {
        qDebug() << "CSWDetails::Operator Streaming (DeSerializeContent) Node not found: file";
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    if (!SWDetails.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CSWDetails::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceSWDetails = CSWDetails class object
 *
 *  \return CSWDetails Class Object
 */
/****************************************************************************/
CSWDetails& CSWDetails::operator=(const CSWDetails& SourceSWDetails)
{
    // make sure not same object
    if (this != &SourceSWDetails)
    {
        QByteArray TempByteArray;
        CSWDetails* p_TempSWDetails = const_cast<CSWDetails*>(&SourceSWDetails);
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        DataStream << *p_TempSWDetails;
        (void)DataStream.device()->reset(); //to avoid lint-534
        DataStream >> *this;
    }
    return *this;
}

}  // namespace DataManager


