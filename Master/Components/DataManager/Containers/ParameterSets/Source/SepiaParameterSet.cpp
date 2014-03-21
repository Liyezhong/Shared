/****************************************************************************/
/*! \file SepiaParameterSet.cpp
 *
 *  \brief Implementation file for class CSepiaParameterSet.
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ParameterSets/Include/SepiaParameterSet.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CSepiaParameterSet::CSepiaParameterSet(const QString ID)
    : m_MountantName("")
    , m_CoverglassLength(0)
    , m_MountantVolume(0)
    , m_ID(ID)
{
}

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CSepiaParameterSet::CSepiaParameterSet()
    : m_MountantName("")
    , m_CoverglassLength(0)
    , m_MountantVolume(0)
    , m_ID("")
{
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam MountantName = MountantName associated with the program
 */
/****************************************************************************/
CSepiaParameterSet::CSepiaParameterSet(const QString ID, const QString MountantName, const quint16 CoverglassLength, const quint16 MountantVolume)
    : m_MountantName(MountantName)
    , m_CoverglassLength(CoverglassLength)
    , m_MountantVolume(MountantVolume)
    , m_ID(ID)
{
}


/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CSepiaParameterSet::~CSepiaParameterSet()
{
}


/****************************************************************************/
/*!
 *  \brief Writes the CSepiaParameterSet Data to QIODevice
 *
 *  \iparam XmlStreamWriter = XmlStream to Write
 *  \iparam CompleteData = if true writes Complete data of object into
 *                           the stream
 *
 *  \return true - success ,false - failure
 */
/****************************************************************************/
bool CSepiaParameterSet::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    Q_UNUSED(CompleteData)
    //QReadLocker Locker(mp_ReadWriteLock);
    bool Result = true;

    XmlStreamWriter.writeStartElement("ParameterSet");
    XmlStreamWriter.writeAttribute("ID", GetID());
    XmlStreamWriter.writeAttribute("MountantName", GetMountantName());
    XmlStreamWriter.writeAttribute("CoverglassLength", QString::number(GetCoverglassLength()));
    XmlStreamWriter.writeAttribute("MountantVolume", QString::number(GetMountantVolume()));
    XmlStreamWriter.writeEndElement(); // ParameterSet
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CSepiaParameterSet object.
 *
 *  \iparam XmlStreamReader = Xml stream to read from
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return true-success, false - failure
 */
/****************************************************************************/
bool CSepiaParameterSet::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    Q_UNUSED(CompleteData)
//    QWriteLocker Locker(mp_ReadWriteLock);
    bool Result = true;
    // ID
    if (!XmlStreamReader.attributes().hasAttribute("ID")) {
        qDebug() << "### attribute <ID> is missing => abort reading";
        return false;
    }
    SetID(XmlStreamReader.attributes().value("ID").toString());

    // MountantName
    if (!XmlStreamReader.attributes().hasAttribute("MountantName")) {
        qDebug() << "### attribute <MountantName> is missing => abort reading";
        return false;
    }
    SetMountantName(XmlStreamReader.attributes().value("MountantName").toString());

    // CoverglassLength
    if (!XmlStreamReader.attributes().hasAttribute("CoverglassLength")) {
        qDebug() << "### attribute <CoverglassLength> is missing => abort reading";
        return false;
    }
    SetCoverglassLength(XmlStreamReader.attributes().value("CoverglassLength").toString().toInt());

    // MountantVolume
    if (!XmlStreamReader.attributes().hasAttribute("MountantVolume")) {
        qDebug() << "### attribute <MountantVolume> is missing => abort reading";
        return false;
    }
    SetMountantVolume(XmlStreamReader.attributes().value("MountantVolume").toString().toInt());


    return Result;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam Program = CSepiaParameterSet class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator << (QDataStream& OutDataStream, const CSepiaParameterSet& Program)
{

    CSepiaParameterSet* p_TempProgram = const_cast<CSepiaParameterSet*>(&Program);

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempProgram->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CSepiaParameterSet::Operator Streaming (SerializeContent) failed.";
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
 *  \iparam Program = CSepiaParameterSet class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >> (QDataStream& InDataStream, CSepiaParameterSet& ParameterSet)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());
    (void)XmlStreamReader.readElementText(QXmlStreamReader::IncludeChildElements); //lint -e534
    if (!DataManager::Helper::ReadNode(XmlStreamReader, QString("ParameterSet"))) {
        qDebug() << "CSepiaParameterSet::Operator Streaming (DeSerializeContent) Node not found: Program";
        THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    if (!ParameterSet.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CSepiaParameterSet::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceProgram  = Program to assign from
 *
 *  \return CSepiaParameterSet Class Object
 */
/****************************************************************************/
CSepiaParameterSet& CSepiaParameterSet::operator = (const CSepiaParameterSet& SourceParameterSet)
{
    // make sure not same object
    if (this != &SourceParameterSet)
    {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        TempByteArray.clear();
        try {
            DataStream << SourceParameterSet;
            (void)DataStream.device()->reset();
            DataStream >> *this;
        }
        catch(...) {
            //! \todo re-throw or Just log ? delete memory allocated ?
            qDebug()<<"CSepiaParameterSet::Assignment failed";
        }
    }
    return *this;
}


}  // namespace DataManager

