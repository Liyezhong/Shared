/****************************************************************************/
/*! \file Reagent.cpp
 *
 *  \brief Reagent Implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-17
 *   $Author:  $ Swati Tiwari
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/Reagent.h"
#include "Global/Include/EventObject.h"
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"
#include <QDataStream>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 *
 */
/****************************************************************************/

CReagent::CReagent() : m_Visible(true), m_Name(""), m_NameID(""), m_CassettesUntilChange(0),
    m_CyclesUntilChange(0), m_DaysUntilChange(0), m_ReagentID("U100"), m_GroupID("")
{
}

/****************************************************************************/
/*!
 *  \brief Constructor with Reagent ID
 *
 *  \iparam ID = Reagent ID
 */
/****************************************************************************/

CReagent::CReagent(const QString ID) : m_Visible(true), m_Name(""), m_NameID(""),  m_CassettesUntilChange(0),
    m_CyclesUntilChange(0), m_DaysUntilChange(0), m_ReagentID(ID), m_GroupID("")
{
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam Reagent = Instance of the CReagent class
 *
 *  \return
 */
/****************************************************************************/
CReagent::CReagent(const CReagent& Reagent)
{
    CReagent* p_TempReagent = const_cast<CReagent*>(&Reagent);

    *this = *p_TempReagent;
}

/****************************************************************************/
/*!
 *  \brief Writes from the CReagent object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the QXmlStreamWriter
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CReagent::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    QString StringValue;
    bool Result = true;

    XmlStreamWriter.writeStartElement("Reagent");

    XmlStreamWriter.writeAttribute("ID", GetReagentID());
    XmlStreamWriter.writeAttribute("Name", GetReagentName(true));
    XmlStreamWriter.writeAttribute("NameID", GetReagentNameID());
    XmlStreamWriter.writeAttribute("Group", GetGroupID());

//    if((IsUserReagent()) || (IsLeicaReagent())) {

        (void)StringValue.setNum(GetMaxCassettes());  //to suppress lint-534
        XmlStreamWriter.writeAttribute("CassettesUntilChange", StringValue);

        (void)StringValue.setNum(GetMaxCycles());  //to suppress lint-534
        XmlStreamWriter.writeAttribute("CyclesUntilChange", StringValue);

        (void)StringValue.setNum(GetMaxDays());
        XmlStreamWriter.writeAttribute("DaysUntilChange", StringValue);

//        }
        //======NODE=END=====UsageLimitation=========================


        //======NODE=======Temporary Data Variables=========================
        if(CompleteData) {
        // Do Nothing as the m_ReagentType is derived from m_ID
        }
        //======NODE=END====Temporary Data Variables=========================

    XmlStreamWriter.writeEndElement(); //Reagent

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XmlStreamReader to CReagent object.
 *
 *  \iparam XmlStreamReader = Instance of the XmlStreamReader.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CReagent::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{

    // ID
    if (!XmlStreamReader.attributes().hasAttribute("ID")) {
        qDebug() << "### attribute <ID> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "Reagent-ID", true);
        return false;
    }
    SetReagentID(XmlStreamReader.attributes().value("ID").toString());

    // ReagentNameID
    if (XmlStreamReader.attributes().hasAttribute("NameID")) {
        SetReagentNameID(XmlStreamReader.attributes().value("NameID").toString().trimmed());
    }

    // ReagentName
    if (!XmlStreamReader.attributes().hasAttribute("Name")) {
        qDebug() << "### attribute <Name> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "Reagent-Name", true);
        return false;
    }
    SetReagentName(XmlStreamReader.attributes().value("Name").toString());

    // Group
    if (!XmlStreamReader.attributes().hasAttribute("Group")) {
        qDebug() << "### attribute <Group> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "Reagent-Group", true);
        return false;
    }
    SetGroupID(XmlStreamReader.attributes().value("Group").toString());

    // read the usage limitation <UsageLimitation>
    if (!ReadUsageLimitation(XmlStreamReader)) {
        // clean up
        return false;
    }

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // Do Nothing as the m_ReagentType is derived from m_ID
    }
    //======NODE=END====Temporary Data Variables=========================

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the Usage limitaqtion from the xml stream.
 *
 *  \iparam XmlStreamReader  = XML stream to read from
 *
 *  \return True or False
 */
/****************************************************************************/
bool CReagent::ReadUsageLimitation(QXmlStreamReader& XmlStreamReader)
{    
    // MaxTime
    if (!XmlStreamReader.attributes().hasAttribute("DaysUntilChange")) {
        qDebug() << "### attribute <DaysUntilChange> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "Reagent-DaysUntilChange", true);
        return false;
    }
   SetMaxDays(XmlStreamReader.attributes().value("DaysUntilChange").toString().toInt());

   // MaxCassettes
   if (!XmlStreamReader.attributes().hasAttribute("CassettesUntilChange")) {
       qDebug() << "### attribute <CassettesUntilChange> is missing => abort reading";
       Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                  Global::tTranslatableStringList() << "Reagent-CassettesUntilChange", true);
       return false;
   }
   SetMaxCassettes(XmlStreamReader.attributes().value("CassettesUntilChange").toString().toInt());

   // MaxCycles
   if (!XmlStreamReader.attributes().hasAttribute("CyclesUntilChange")) {
       qDebug() << "### attribute <CyclesUntilChange> is missing => abort reading";
       Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                  Global::tTranslatableStringList() << "Reagent-CyclesUntilChange", true);
       return false;
   }
   SetMaxCycles(XmlStreamReader.attributes().value("CyclesUntilChange").toString().toInt());

    return true;
}

/****************************************************************************/
/*!
 *  \brief Derives the Reagent type from Reagent ID.
 *
 *  \return Reagent Type LEICA , SPECIAL or USER Reagent
 */
/****************************************************************************/

ReagentType_t CReagent::GetReagentType() const
{
    QString ReagentID = GetReagentID();
    if (ReagentID.isEmpty()) {
        qDebug() << "### CReagent::GetReagentType Unknown Reagent Type: " << ReagentID;
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_REAGENT_ID_DOESTNOT_EXISTS,
                                                   Global::tTranslatableStringList() << ReagentID, true);
        return UNDEFINED_REAGENT_TYPE;
    }
    if(ReagentID.at(0) == 'U') {
        return USER_REAGENT;
    }
    else if(ReagentID.at(0) == 'L') {
        return LEICA_REAGENT;
    }
    else {
        qDebug() << "### CReagent::GetReagentType Unknown Reagent Type: " << GetReagentID();
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_REAGENT_ID_DOESTNOT_EXISTS,
                                                   Global::tTranslatableStringList() << GetReagentID(), true);
        return UNDEFINED_REAGENT_TYPE;
    }
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam Reagent = CReagent class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CReagent& Reagent)
{
    CReagent* p_TempReagent = const_cast<CReagent*>(&Reagent);

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempReagent->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CReagent::Operator Streaming (SerializeContent) failed.";
        p_TempReagent->m_ErrorHash.insert(EVENT_DM_STREAMOUT_FAILED,
                                              Global::tTranslatableStringList() << "Reagent");
        p_TempReagent->SetErrorList(&p_TempReagent->m_ErrorHash);
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED,
                                                   Global::tTranslatableStringList() << "Reagent", true);
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the QDataStream
 *  \iparam Reagent = CReagent class object
 *
 *  \return Input Stream
 */
/****************************************************************************/

QDataStream& operator >>(QDataStream& InDataStream, CReagent& Reagent)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

    if (!Helper::ReadNode(XmlStreamReader, QString("Reagent"))) {
        qDebug() << "CReagent::Operator Streaming (DeSerializeContent) Node not found: Reagent";
        Reagent.m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED,
                                              Global::tTranslatableStringList() << "Reagent");
        Reagent.SetErrorList(&Reagent.m_ErrorHash);
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED,
                                                   Global::tTranslatableStringList() << "Reagent", true);
    }

    if (!Reagent.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CReagent::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        Reagent.m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED,
                                              Global::tTranslatableStringList() << "Reagent");
        Reagent.SetErrorList(&Reagent.m_ErrorHash);
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED,
                                                   Global::tTranslatableStringList() << "Reagent", true);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceReagent = CReagent class object
 *
 *  \return CReagent Class Object
 */
/****************************************************************************/

CReagent& CReagent::operator=(const CReagent& SourceReagent)
{
    // make sure not same object
    if (this != &SourceReagent)
    {
        QByteArray TempByteArray;
        CReagent* p_TempReagent = const_cast<CReagent*>(&SourceReagent);
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        DataStream << *p_TempReagent;
        (void)DataStream.device()->reset(); //to avoid lint-534
        DataStream >> *this;
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief  Reset the error list
 */
/****************************************************************************/
void CReagent::ResetLastErrors()
{
    qint32 I = 0;
    while(!m_ListOfErrors.isEmpty()) {
        const_cast<ErrorMap_t *>(m_ListOfErrors.at(I))->clear();
        m_ListOfErrors.removeFirst();
        I++;
    }
    m_ListOfErrors.clear();
}
QString CReagent::GetReagentName(bool forWriteXml) const
{
    bool ok = false;
    if(IsLeicaReagent()&& !forWriteXml)
    {
        quint32 strid = GetReagentNameID().toUInt(&ok);
        if(ok)
        {
            return Global::UITranslator::TranslatorInstance().Translate(
                    Global::TranslatableString(strid));
        }
    }
    return m_Name;
}

}  // namespace DataManager

