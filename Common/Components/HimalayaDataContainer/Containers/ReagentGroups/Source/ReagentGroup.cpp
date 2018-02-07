/****************************************************************************/
/*! \file ReagentGroup.cpp
 *
 *  \brief Reagent group implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-29
 *   $Author:  $ Swati Tiwari
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
#include <QBuffer>
#include <QXmlStreamReader>
#include <iostream>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/ReagentGroup.h"
#include "Global/Include/EventObject.h"
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"
#include "Global/Include/UITranslator.h"
#include <QDataStream>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CReagentGroup::CReagentGroup()
            :m_GroupID(""), m_GroupName(""),
            m_GroupNameID(""), m_Color(""),
            m_IsCleaningReagentGroup(false),
            m_IsParraffinGroup(false),
            m_MinTemp(0),
            m_MaxTemp(0)

{

}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam ID
 */
/****************************************************************************/
CReagentGroup::CReagentGroup(const QString& ID)
    :m_GroupID(ID), m_GroupName(""),
    m_GroupNameID(""),m_Color(""),
    m_IsCleaningReagentGroup(false),
    m_IsParraffinGroup(false),
    m_MinTemp(0),m_MaxTemp(0)
{
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ReagentGroup = Instance of the CReagentGroup class
 */
/****************************************************************************/
CReagentGroup::CReagentGroup(const CReagentGroup& ReagentGroup)
{
    CReagentGroup* p_TempReagentGroup = const_cast<CReagentGroup*>(&ReagentGroup);
    *this = *p_TempReagentGroup;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CReagentGroup::~CReagentGroup()
{
}

/****************************************************************************/
/*!
 *  \brief Writes from the CReagentGroup object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CReagentGroup::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    bool Result = true;

    XmlStreamWriter.writeStartElement("ReagentGroup");

    XmlStreamWriter.writeAttribute("ID", GetGroupID());
    XmlStreamWriter.writeAttribute("Name", GetReagentGroupName(true));
    XmlStreamWriter.writeAttribute("NameID", GetGroupNameID());
    XmlStreamWriter.writeAttribute("Color", GetGroupColor());
    if (m_IsCleaningReagentGroup)
        XmlStreamWriter.writeAttribute("CleaningReagentGroup", "True");
    else
        XmlStreamWriter.writeAttribute("CleaningReagentGroup", "false");

    if (m_IsParraffinGroup)
        XmlStreamWriter.writeAttribute("IsParraffinGroup", "True");
    else
        XmlStreamWriter.writeAttribute("IsParraffinGroup", "false");

    XmlStreamWriter.writeAttribute("MinTemprature", QString::number(m_MinTemp));
    XmlStreamWriter.writeAttribute("MaxTemprature", QString::number(m_MaxTemp));
        //======NODE=======Temporary Data Variables=========================
        if(CompleteData) {
        // Do Nothing as the m_ReagentType is derived from m_ID
        }
        //======NODE=END====Temporary Data Variables=========================

    XmlStreamWriter.writeEndElement(); //ReagentGroup

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CReagentGroup object.
 *
 *  \iparam XmlStreamReader = Instance of the IODevice might be Buffer or File.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CReagentGroup::DeserializeContent(QXmlStreamReader& XmlStreamReader,bool CompleteData)
{
    // ID
    if (!XmlStreamReader.attributes().hasAttribute("ID")) {
        qDebug() << "### attribute <ID> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "ReagentGroup:ID", true);
        return false;
    }
    SetGroupID(XmlStreamReader.attributes().value("ID").toString());

    // GroupName
    if (XmlStreamReader.attributes().hasAttribute("NameID")) {
        SetGroupNameID(XmlStreamReader.attributes().value("NameID").toString().trimmed());
    }

    // GroupName
    if (!XmlStreamReader.attributes().hasAttribute("Name")) {
        qDebug() << "### attribute <Name> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "Reagent-Name", true);
        return false;
    }
    SetReagentGroupName(XmlStreamReader.attributes().value("Name").toString());

    // color
    if (!XmlStreamReader.attributes().hasAttribute("Color")) {
        qDebug() << "### attribute <Group> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "Reagent-Group", true);
        return false;
    }
    SetGroupColor(XmlStreamReader.attributes().value("Color").toString());

    // Is CleaningReagentGroup
    if (!XmlStreamReader.attributes().hasAttribute("CleaningReagentGroup")) {
        qDebug() << "### attribute <CleaningReagentGroup> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                   Global::tTranslatableStringList() << "Reagent-Name", true);
        return false;
    }

    if (XmlStreamReader.attributes().value("CleaningReagentGroup").toString().toUpper() == "TRUE") {
        m_IsCleaningReagentGroup = true;
    }
    else {
        m_IsCleaningReagentGroup = false;
    }

    //IsParraffinGroup
    if (!XmlStreamReader.attributes().hasAttribute("IsParraffinGroup")) {
        qDebug() << "### attribute <IsParraffinGroup> is missing => abort reading";
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                       Global::tTranslatableStringList() << "Reagent-Name", true);
        return false;
    }

    if (XmlStreamReader.attributes().value("IsParraffinGroup").toString().toUpper() == "TRUE") {
        m_IsParraffinGroup = true;
    }
    else {
        m_IsParraffinGroup = false;
    }

    //MinTemp
    if (!XmlStreamReader.attributes().hasAttribute("MinTemprature")) {
        qDebug() << "### attribute <MinTemprature> is missing => abort reading";
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                       Global::tTranslatableStringList() << "Reagent-Name", true);
        return false;
    }

    if (!XmlStreamReader.attributes().hasAttribute("MaxTemprature")) {
        qDebug() << "### attribute <MaxTemprature> is missing => abort reading";
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                                                       Global::tTranslatableStringList() << "Reagent-Name", true);
        return false;
    }

    m_MinTemp = XmlStreamReader.attributes().value("MinTemprature").toString().toInt();
    m_MaxTemp = XmlStreamReader.attributes().value("MaxTemprature").toString().toInt();


    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // Do Nothing as the m_ReagentType is derived from m_ID
    }
    //======NODE=END====Temporary Data Variables=========================

    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ReagentGroup = CReagentGroup class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CReagentGroup& ReagentGroup)
{
    CReagentGroup* p_TempReagentGroup = const_cast<CReagentGroup*>(&ReagentGroup);

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempReagentGroup->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CReagentGroup::Operator Streaming (SerializeContent) failed.";

        p_TempReagentGroup->m_ErrorHash.insert(EVENT_DM_STREAMOUT_FAILED,
                                              Global::tTranslatableStringList() << "ReagentGroup");
        p_TempReagentGroup->SetErrorList(&p_TempReagentGroup->m_ErrorHash);
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentGroup", true);
    }

    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam ReagentGroup = CReagentGroup class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CReagentGroup& ReagentGroup)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

    if (!Helper::ReadNode(XmlStreamReader, QString("ReagentGroup"))) {
        qDebug() << "CReagentGroup::Operator Streaming (DeSerializeContent) Node not found: Program";
        ReagentGroup.m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED,
                                              Global::tTranslatableStringList() << "ReagentGroup");
        ReagentGroup.SetErrorList(&ReagentGroup.m_ErrorHash);
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentGroup", true);
    }
    if (!ReagentGroup.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CReagentGroup::Operator Streaming (DeSerializeContent) failed.";
        ReagentGroup.m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED,
                                              Global::tTranslatableStringList() << "ReagentGroup");
        ReagentGroup.SetErrorList(&ReagentGroup.m_ErrorHash);
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentGroup", true);
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceReagentGroup = CReagentGroup class object
 *
 *  \return CReagentGroup Class Object
 */
/****************************************************************************/
CReagentGroup& CReagentGroup::operator=(const CReagentGroup& SourceReagentGroup)
{
    // make sure not same object
    if (this != &SourceReagentGroup)
    {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        TempByteArray.clear();
        DataStream << SourceReagentGroup;
        (void)DataStream.device()->reset();
        DataStream >> *this;
    }
    return *this;
}
QString CReagentGroup::GetReagentGroupName(bool forWriteXml) const
{
    bool ok = false;
    if(!GetGroupNameID().isEmpty() && !forWriteXml)
    {
        quint32 strid = GetGroupNameID().toUInt(&ok);
        if(ok)
        {
            return Global::UITranslator::TranslatorInstance().Translate(
                    Global::TranslatableString(strid));
        }
    }
    return m_GroupName;
}
}//End of namespace DataManager
