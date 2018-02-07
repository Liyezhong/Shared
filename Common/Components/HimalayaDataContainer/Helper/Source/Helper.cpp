/****************************************************************************/
/*! \file HimalayaDataContainer/Helper/Source/Helper.cpp
 *
 *  \brief Implementation file for general purpose functions.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-21
 *  $Author:    $ R. Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "HimalayaDataContainer/Helper/Include/Helper.h"
#include <QDebug>

 //lint -e1551

namespace DataManager {

XmlStartElementReaderHelper::XmlStartElementReaderHelper(QXmlStreamReader &reader)
    : m_NonEnd(false)
{
    while(!reader.atEnd() && !reader.hasError())
    {
        // try to find next start element
        if(QXmlStreamReader::StartElement == reader.readNext())
        {
            m_NonEnd = true;
            break;
        }
    }
}

bool XmlStartElementReaderHelper::IsValid(void)
{
    return m_NonEnd;
}

XmlElementReaderHelper::XmlElementReaderHelper(QXmlStreamReader &reader,
                                               bool             *hasChildren)
    : m_Reader(reader)
    , m_HasChildren(hasChildren)
    , m_IsEnd(true)
{
    // if the reader is at a start element
    if (m_Reader.isStartElement())
    {
        m_IsEnd = false;
    }
    // else, try to find a start element
    else
    {
        while(!m_Reader.atEnd() && !m_Reader.hasError())
        {
            (void)m_Reader.readNext();

            if (m_Reader.isStartElement())
            {
                m_IsEnd = false;
                break;
            }
        }
    }
}

XmlElementReaderHelper::~XmlElementReaderHelper(void)
{
    // if start element is found, try to read next
    if (!m_IsEnd)
    {
        if (0 != m_HasChildren)
        {
            *m_HasChildren = false;
        }

        while(!m_Reader.atEnd() && !m_Reader.hasError())
        {
            (void)m_Reader.readNext();

            if (m_Reader.isStartElement())
            {
                if (0 != m_HasChildren)
                {
                    *m_HasChildren = true;
                }

                break;
            }
            else if (m_Reader.isEndElement())
            {
                (void)m_Reader.readNext();
                break;
            }
        }
    }
}

bool XmlElementReaderHelper::IsEnd(void)
{
    return m_IsEnd;
}

XmlElementWriterHelper::XmlElementWriterHelper(
        QXmlStreamWriter &writer, const QString &elementName)
    : m_Writer(writer)
{
    m_Writer.writeStartElement(elementName);
}

XmlElementWriterHelper::~XmlElementWriterHelper(void)
{
    m_Writer.writeEndElement();
}

XmlDocumentWriterHelper::XmlDocumentWriterHelper(QXmlStreamWriter &writer)
    : m_Writer(writer)
{
    m_Writer.writeStartDocument();
}

XmlDocumentWriterHelper::~XmlDocumentWriterHelper(void)
{
    m_Writer.writeEndDocument();
}

XmlReadErrorType_t ReadAttributesOfElement(const QString     &elementName,
                                           const QStringList &attributeNameList,
                                           QXmlStreamReader  &reader,
                                           QStringList       &attributeList)
{
    bool hasChildren = false;

    {
        XmlElementReaderHelper helper(reader, &hasChildren);

        if (helper.IsEnd())
        {
            return END;
        }

        // check if element is "elementName"
        if (elementName != reader.name())
        {
            qDebug() << "### ReadAttributesOfElement failed: "
                        "the element should be \""
                     << elementName
                     << "\" not \""
                     << reader.name()
                     << "\"";

            return WRONG_NAME;
        }

        QStringList valueList;

        for (QStringList::const_iterator itr = attributeNameList.begin();
             attributeNameList.end() != itr;
             ++itr)
        {
            QString attributeName = *itr;
            QStringRef attribute = reader.attributes().value(attributeName);
            if (attribute.isNull())
            {
                qDebug() << "### ReadAttributesOfElement failed: "
                            "the element should have attribute \""
                         << attributeName
                         << "\" ";

                return WRONG_ATTRIBUTE;
            }

            valueList.append(attribute.toString());
        }

        attributeList = valueList;
    }

    return hasChildren ? HAS_CHILDREN : SUCCEED;
}

XmlReadErrorType_t ReadAttributesOfElement(const QString     &elementName,
                                           QXmlStreamReader  &reader,
                                           QStringList       &attributeNameList,
                                           QStringList       &attributeList)
{
    bool hasChildren = false;

    {
        XmlElementReaderHelper helper(reader, &hasChildren);

        if (helper.IsEnd())
        {
            return END;
        }

        // check if element is "elementName"
        if (elementName != reader.name())
        {
            qDebug() << "### ReadAttributesOfElement failed: "
                        "the element should be \""
                     << elementName
                     << "\" not \""
                     << reader.name()
                     << "\"";

            return WRONG_NAME;
        }

        // read all attributes
        QStringList nameList;
        QStringList valueList;

        QXmlStreamAttributes attributes = reader.attributes();

        for (QXmlStreamAttributes::const_iterator itr = attributes.constBegin();
             attributes.constEnd() != itr;
             ++itr)
        {
            nameList.append((*itr).name().toString());
            valueList.append((*itr).value().toString());
        }

        // copy out
        attributeNameList = nameList;
        attributeList     = valueList;
    }

    return hasChildren ? HAS_CHILDREN : SUCCEED;
}

void WriteElementWithAttributes(const QString     &elementName,
                                const QStringList &attributeNameList,
                                const QStringList &attributeList,
                                QXmlStreamWriter  &writer)
{
    XmlElementWriterHelper helper(writer, elementName);

    QStringList::const_iterator itrName  = attributeNameList.begin();
    QStringList::const_iterator itrValue = attributeList.begin();

    while (attributeNameList.end() != itrName)
    {
        writer.writeAttribute(*itrName, *itrValue);

        ++itrName;
        ++itrValue;
    }
}

}  // namespace DataManager
