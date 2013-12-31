/****************************************************************************/
/*! \file DataManager/Containers/InstrumentHistory/Source/SubModule.cpp
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


#include <Global/Include/Exception.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/Utils.h>

#include "DataManager/Containers/InstrumentHistory/Include/SubModule.h"
#include "DataManager/Helper/Include/Helper.h"

#include <QDebug>

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CSubModule::CSubModule() :m_SubModuleName(""),
    m_SubModuleType(""),
    m_SubModuleDescription("")
{

}

/****************************************************************************/
/*!
 *  \brief Parameterized Constructor
 *
 *  \iparam SubModuleName
 */
/****************************************************************************/
CSubModule::CSubModule(QString SubModuleName)
{
    m_SubModuleName = SubModuleName;
}

/****************************************************************************/
/*!
 *  \brief Parameterized Constructor
 *  \iparam SubModuleName = name of the SubModule
 *  \iparam SubModuleType = type of the SubModule
 *  \iparam SubModuleDescription = Description of the SubModule
 */
/****************************************************************************/
CSubModule::CSubModule(QString SubModuleName, QString SubModuleType, QString SubModuleDescription)
{
    m_SubModuleName = SubModuleName;
    m_SubModuleType = SubModuleType;
    m_SubModuleDescription = SubModuleDescription;
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
    CopyFromOther(SubModuleInfo);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \note  Method for internal use only
 *
 *  \iparam SubModuleInfo = Instance of the CSubModule class
 *
 *  \return
 */
/****************************************************************************/
void CSubModule::CopyFromOther(const CSubModule &SubModuleInfo)
{
    QString Name = const_cast<CSubModule&>(SubModuleInfo).GetSubModuleName();
    QString Unit = const_cast<CSubModule&>(SubModuleInfo).GetSubModuleType();
    QString Type = const_cast<CSubModule&>(SubModuleInfo).GetSubModuleDescription();

    m_SubModuleName = Name;
    m_SubModuleType = Unit;
    m_SubModuleDescription = Type;

    int Count = const_cast<CSubModule&>(SubModuleInfo).GetNumberOfParameters();

    for(int i=0; i<Count; i++)
    {
        Parameter_t* StructParameter = new Parameter_t;

        StructParameter->ParameterName = SubModuleInfo.GetParameterInfo(i)->ParameterName;
        StructParameter->ParameterUnit = SubModuleInfo.GetParameterInfo(i)->ParameterUnit;
        StructParameter->ParameterValue = SubModuleInfo.GetParameterInfo(i)->ParameterValue;

        m_ParameterNames.append(StructParameter->ParameterName);
        m_ListOfParameters.insert(StructParameter->ParameterName, StructParameter);
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CSubModule::~CSubModule()
{
    try {
        (void)DeleteAllParameters(); // to avoid lint-534
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
/*!
 *  \brief  Deletes all the submodules in the list
 */
/****************************************************************************/
void CSubModule::DeleteAllParameters()
{
    // get parameter pointer for each entry and free memory
    foreach(Parameter_t* p, m_ListOfParameters) {
        delete p;
    }
    m_ListOfParameters.clear();
    m_ParameterNames.clear();
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

        this->DeleteAllParameters();
        CopyFromOther(SubModuleInfo);

    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Reads the CSubModule Data from QIODevice
 *
 *  \iparam XmlStreamReader = Xml reader to read the XML contents
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

                AddParameterInfo(ParamName, ParamUnit, XmlStreamReader.readElementText());
            }
        } // end of start element comparison
        else if(XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "SubModule") {
            //            qDebug() << XmlStreamReader.name().toString();
            break;
        }
    } // end of while loop

    if (CompleteData) {
        // Future use
    }

    return Result;
}

/****************************************************************************/
/*!
 *
 *  \brief Writes the CSubModule Data to QIODevice
 *
 *  \iparam XmlStreamWriter = Xml stream writer to write the XML contents
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

    QHash<QString, Parameter_t*>::const_iterator i;
    for(i = m_ListOfParameters.constBegin(); i != m_ListOfParameters.constEnd(); ++i)
    {
        XmlStreamWriter.writeStartElement("parameter");
        Parameter_t* StructParam;
        StructParam = i.value();
        XmlStreamWriter.writeAttribute("name", StructParam->ParameterName);
        XmlStreamWriter.writeAttribute("unit", StructParam->ParameterUnit);
        XmlStreamWriter.writeCharacters(StructParam->ParameterValue);
        XmlStreamWriter.writeEndElement();
    }
    XmlStreamWriter.writeEndElement();

    if (CompleteData) {
        // Future Use
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam SubModuleInfo = CSubModule class object
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
 *  \iparam SubModuleInfo = CSubModule class object
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

