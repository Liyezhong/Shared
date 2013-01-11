/****************************************************************************/
/*! \file SubModule.cpp
 *
 *  \brief Implementation file for class CSubModule.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-04
 *  $Author:    $ Soumya. D
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

#include "SubModule.h"
#include <QDebug>

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CSubModule::CSubModule() :m_SubModuleName(""),
    m_SubModuleType("")
{

}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam SubModuleInfo = Instance of the SubModule class
 *
 */
/****************************************************************************/
CSubModule::CSubModule(const CSubModule& SubModuleInfo)
{
    *this = SubModuleInfo;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CSubModule::~CSubModule()
{
    for(int i=0; i<m_SubModuleList.count(); i++)
    {
        m_SubModuleList.removeAt(i);
    }

    m_SubModuleList.clear();
    m_ParameterList.clear();
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SubModuleInfo = CSubModule class object
 *
 *  \return CSubModule Class Object
 */
/****************************************************************************/
CSubModule& CSubModule::operator=(const CSubModule& SubModuleInfo)
{
    if(this != &SubModuleInfo) {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        TempByteArray.clear();

        //Serialize
        DataStream << SubModuleInfo;
        (void)DataStream.device()->reset();

        //Deserialize
        DataStream >> *this;
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Reads the CSubModule Data from QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice - Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CSubModule::DeserializeContent(QXmlStreamReader &XmlStreamReader, bool CompleteData)
{
    bool Result = true;

    //SubModule Name
    if (!XmlStreamReader.attributes().hasAttribute("name")) {
        qDebug() <<  " attribute SubModule<name> is missing => abort reading";
        return false;
    }
    SetSubModuleName(XmlStreamReader.attributes().value("name").toString());

    //SubModule type
    if (!XmlStreamReader.attributes().hasAttribute("type")) {
        qDebug() <<  " attribute SubModule<type> is missing => abort reading";
        return false;
    }
    SetSubModuleType(XmlStreamReader.attributes().value("type").toString());

    //SubModule Description
    if(!XmlStreamReader.attributes().hasAttribute("description")) {
        qDebug() << " attributes SubModule<Description> is missing => abort reading";
        return false;
    }
    SetSubModuleDescription(XmlStreamReader.attributes().value("description").toString());

    // Parameter Node
    while (!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "parameter") {
                if (!XmlStreamReader.attributes().hasAttribute("name")) {
                    qDebug() <<  " attribute <name> is missing => abort reading";
                    return false;
                }
                QString ParamName = XmlStreamReader.attributes().value("name").toString();

                if (!XmlStreamReader.attributes().hasAttribute("unit")) {
                    qDebug() <<  " attribute <unit> is missing => abort reading";
                    return false;
                }
                QString ParamUnit = XmlStreamReader.attributes().value("unit").toString();

                SetParameterInfo(ParamName, ParamUnit, XmlStreamReader.readElementText());
                m_ParameterList.append(GetParameterInfo());
            }
        } // end of start element comparison

        else if(XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "SubModule") {
            qDebug() << XmlStreamReader.name().toString();
            break;
        }
     } // end of while loop
    m_SubModuleList.append(this);
    return Result;
}

/****************************************************************************/
/*!
 *
 *  \brief Writes the CSubModule Data to QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *l
 *  \return True or False
 */
/****************************************************************************/
bool CSubModule::SerializeContent(QXmlStreamWriter &XmlStreamWriter, bool CompleteData)
{
    bool Result = true;

    XmlStreamWriter.writeStartElement("SubModule");
    XmlStreamWriter.writeAttribute("name", GetSubModuleName());
    XmlStreamWriter.writeAttribute("type", GetSubModuleType());
    XmlStreamWriter.writeAttribute("description", GetSubModuleDescription());


    for(int i = 0; i<m_ParameterList.count(); i++)
    {
        XmlStreamWriter.writeStartElement("parameter");
        Parameter StructParam;
        StructParam = m_ParameterList.at(i);
        XmlStreamWriter.writeAttribute("name", StructParam.ParameterName);
        XmlStreamWriter.writeAttribute("unit", StructParam.ParameterUnit);
        XmlStreamWriter.writeCharacters(StructParam.ParameterValue);
        XmlStreamWriter.writeEndElement();
    }
    XmlStreamWriter.writeEndElement();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam CSubModule = CSubModule class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator << (QDataStream& OutDataStream, const CSubModule& SubModuleInfo)
{
    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());
    CSubModule *p_TempSubModule = const_cast<CSubModule*>(&SubModuleInfo);
    if (!p_TempSubModule->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CSubModule::Operator Streaming (SerializeContent) failed.";
    }

    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam CSubModule = CSubModule class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >> (QDataStream& InDataStream, CSubModule& SubModuleInfo)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());


    if (!Helper::ReadNode(XmlStreamReader, QString("SubModule"))) {
        qDebug() << "CSubModule::Operator Streaming (DeSerializeContent) Node not found: SubModule";
    }

    if (!SubModuleInfo.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CSubModule::Operator Streaming (DeSerializeContent) failed.";
    }

    return InDataStream;
}

}   // namespace DataManager
