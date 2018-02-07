/****************************************************************************/
/*! \file HimalayaDataContainer/Helper/Include/Helper.h
 *
 *  \brief Definition file for general purpose functions.
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

#ifndef DATAMANAGER_HELPER_H
#define DATAMANAGER_HELPER_H

#include<QStringList>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace DataManager {

/****************************************************************************/
/**
 * \brief Helper class, which provides some static methods for general use.
 */
/****************************************************************************/

class XmlStartElementReaderHelper
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function XmlStartElementReaderHelper
     *
     *  \param reader = QXmlStreamReader type parameter
     *
     *  \return from XmlStartElementReaderHelper
     */
    /****************************************************************************/
    XmlStartElementReaderHelper(QXmlStreamReader &reader);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsValid
     *
     *  \return from IsValid
     */
    /****************************************************************************/
    bool IsValid(void);

private:
    bool m_NonEnd;       ///<  Definition/Declaration of variable m_NonEnd
};

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class XmlElementReaderHelper
 */
/****************************************************************************/
class XmlElementReaderHelper
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function XmlElementReaderHelper
     *
     *  \param reader = QXmlStreamReader type parameter
     *  \param hasChildren =  bool type parameter
     *
     *  \return from XmlElementReaderHelper
     */
    /****************************************************************************/
    XmlElementReaderHelper(QXmlStreamReader &reader, bool *hasChildren = 0);
    ~XmlElementReaderHelper(void);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsEnd
     *
     *  \return from IsEnd
     */
    /****************************************************************************/
    bool IsEnd(void);

private:
    QXmlStreamReader &m_Reader;       ///<  Definition/Declaration of variable m_Reader
    bool             *m_HasChildren;       ///<  Definition/Declaration of variable m_HasChildren
    bool              m_IsEnd;       ///<  Definition/Declaration of variable m_IsEnd
};

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class XmlElementReaderHelper
 */
/****************************************************************************/
class XmlElementWriterHelper
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function XmlElementWriterHelper
     *
     *  \param writer = QXmlStreamWriter type parameter
     *  \param elementName =  const QString type parameter
     *
     *  \return from XmlElementWriterHelper
     */
    /****************************************************************************/
    XmlElementWriterHelper(QXmlStreamWriter &writer, const QString &elementName);

    ~XmlElementWriterHelper(void);

private:
    QXmlStreamWriter &m_Writer;       ///<  Definition/Declaration of variable m_Writer
};

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class XmlElementReaderHelper
 */
/****************************************************************************/
class XmlDocumentWriterHelper
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function XmlDocumentWriterHelper
     *
     *  \param writer = QXmlStreamWriter type parameter
     *
     *  \return from XmlDocumentWriterHelper
     */
    /****************************************************************************/
    XmlDocumentWriterHelper(QXmlStreamWriter &writer);

    ~XmlDocumentWriterHelper(void);

private:
    QXmlStreamWriter &m_Writer;       ///<  Definition/Declaration of variable m_Writer
};
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum  XmlReadErrorType_t
 */
/****************************************************************************/
typedef enum
{
    WRONG_NAME = -256,
    WRONG_ATTRIBUTE,
    END = -1,
    SUCCEED,
    HAS_CHILDREN
} XmlReadErrorType_t;

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function ReadAttributesOfElement
 *  \iparam elementName elementName
 *  \iparam attributeNameList attributeNameList
 *  \iparam reader reader
 *  \iparam attributeList attributeList
 *  \return from ReadAttributesOfElement
 */
/****************************************************************************/
XmlReadErrorType_t ReadAttributesOfElement(const QString     &elementName,
                                           const QStringList &attributeNameList,
                                           QXmlStreamReader  &reader,
                                           QStringList       &attributeList);


/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function ReadAttributesOfElement
 *  \iparam elementName elementName
 *  \iparam reader reader
 *  \iparam attributeNameList attributeNameList
 *  \iparam attributeList attributeList
 *  \return from ReadAttributesOfElement
 */
/****************************************************************************/
XmlReadErrorType_t ReadAttributesOfElement(const QString     &elementName,
                                           QXmlStreamReader  &reader,
                                           QStringList       &attributeNameList,
                                           QStringList       &attributeList);


/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function WriteElementWithAttributes
 *  \iparam elementName elementName
 *  \iparam attributeNameList attributeNameList
 *  \iparam attributeList attributeList
 *  \iparam writer writer
 *  \return from WriteElementWithAttributes
 */
/****************************************************************************/
void WriteElementWithAttributes(const QString     &elementName,
                                const QStringList &attributeNameList,
                                const QStringList &attributeList,
                                QXmlStreamWriter  &writer);

} // namespace DataManager

#endif // DATAMANAGER_HELPER_H
