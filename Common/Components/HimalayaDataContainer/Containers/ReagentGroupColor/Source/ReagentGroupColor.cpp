/****************************************************************************/
/*! \file ReagentGroupColor.cpp
 *
 *  \brief Reagent group implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-03-04
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
#include "HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColor.h"
#include "Global/Include/EventObject.h"
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"
#include <QDataStream>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CReagentGroupColor::CReagentGroupColor() :m_Color(""), m_ColorID("")
{

}
/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam ID
 */
/****************************************************************************/
CReagentGroupColor::CReagentGroupColor(const QString ID) : m_Color(""), m_ColorID("")
{
    Q_UNUSED(ID)
}
/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ReagentGroupColor = Instance of the CReagentGroupColor class
 */
/****************************************************************************/
CReagentGroupColor::CReagentGroupColor(const CReagentGroupColor& ReagentGroupColor)
{
    CReagentGroupColor* p_TempReagentGroupColor = const_cast<CReagentGroupColor*>(&ReagentGroupColor);
    *this = *p_TempReagentGroupColor;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CReagentGroupColor::~CReagentGroupColor()
{
}

/****************************************************************************/
/*!
 *  \brief Writes from the CReagentGroupColor object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CReagentGroupColor::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    bool Result = true;

    XmlStreamWriter.writeStartElement("Color");

    XmlStreamWriter.writeAttribute("ID", GetColorID());
    XmlStreamWriter.writeAttribute("Value", GetColorValue());

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
 *  \brief Reads from the IODevice to CReagentGroupColor object.
 *
 *  \iparam XmlStreamReader = Instance of the IODevice might be Buffer or File.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CReagentGroupColor::DeserializeContent(QXmlStreamReader& XmlStreamReader,bool CompleteData)
{
    // Color ID
    if (!XmlStreamReader.attributes().hasAttribute("ID")) {
        qDebug() << "### attribute <ID> is missing => abort reading";
        return false;
    }

    SetColorID(XmlStreamReader.attributes().value("ID").toString());

    // Color Value
    if (!XmlStreamReader.attributes().hasAttribute("Value")){
        qDebug() << "### attribute <Value> is missing => abort reading";
         return false;
    }
    SetColorValue(XmlStreamReader.attributes().value("Value").toString());

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
    }
    //======NODE=END====Temporary Data Variables=========================
    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ReagentGroupColor = CReagentGroupColor class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CReagentGroupColor& ReagentGroupColor)
{
    CReagentGroupColor* p_TempReagentGroupColor = const_cast<CReagentGroupColor*>(&ReagentGroupColor);
    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());
    if (!p_TempReagentGroupColor->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CReagentGroupColor::Operator Streaming (SerializeContent) failed.";
        p_TempReagentGroupColor->m_ErrorHash.insert(EVENT_DM_STREAMOUT_FAILED,
                                              Global::tTranslatableStringList() << "ReagentGroupColor");
        p_TempReagentGroupColor->SetErrorList(&p_TempReagentGroupColor->m_ErrorHash);

        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentGroupColor", true);

    }
    return OutDataStream;
}
/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam ReagentGroupColor = CReagentGroup class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CReagentGroupColor& ReagentGroupColor)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());
    if (!Helper::ReadNode(XmlStreamReader, QString("Color"))) {
        qDebug() << "CReagentGroupColor::Operator Streaming (DeSerializeContent) Node not found: Reagents";
        ReagentGroupColor.m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED,
                                              Global::tTranslatableStringList() << "ReagentGroupColors");

        ReagentGroupColor.SetErrorList(&ReagentGroupColor.m_ErrorHash);
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentGroupColors", true);
    }

    if (!ReagentGroupColor.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CReagentGroupColor::Operator Streaming (DeSerializeContent) failed.";
        ReagentGroupColor.m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED,
                                              Global::tTranslatableStringList() << "ReagentGroupColors");
        ReagentGroupColor.SetErrorList(&ReagentGroupColor.m_ErrorHash);
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED,
                                                   Global::tTranslatableStringList() << "ReagentGroupColors", true);
    }
    return InDataStream;
}
/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceReagentGroupColor = CReagentGroupColor class object
 *
 *  \return CReagentGroupColor Class Object
 */
/****************************************************************************/
CReagentGroupColor& CReagentGroupColor::operator=(const CReagentGroupColor& SourceReagentGroupColor)
{
    // make sure not same object
    if (this != &SourceReagentGroupColor)
    {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        TempByteArray.clear();
        DataStream << SourceReagentGroupColor;
        (void)DataStream.device()->reset();
        DataStream >> *this;
    }
    return *this;
}
}//End of namespace DataManager



