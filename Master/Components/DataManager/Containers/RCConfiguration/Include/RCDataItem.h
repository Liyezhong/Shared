/****************************************************************************/
/*! \file RCDataItem.h
 *
 *  \brief Definition for RCDataItem class.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-07-08
 *  $Author:    $ Ramya GJ
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

#ifndef DATAMANAGER_RCDATAITEM_H
#define DATAMANAGER_RCDATAITEM_H

#include <QString>
#include <QXmlStreamReader>

#include "Global/Include/RemoteCareTypes.h"
namespace DataManager {

const int NUMBER_OF_RDI_QUALITY = 4; ///< total number of rdi quality types
const QString NameOfRDIQuality[4] = {"DataGood", "DataBad", "DataUncertain", "Invalid"}; ///< text for quality

const int NUMBER_OF_RDI_TYPE = 5;  ///< total number of rdi data types
const QString NameOfRDIType[5] = {"Analog", "Digital", "String", "Undefined", "Invalid"};///< text for each type


/****************************************************************************/
/*!
 *  \brief  This class implements bath layout configuration rule.
 */
/****************************************************************************/
class CRCDataItem
{    
    friend class CRCConfiguration;

private:
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

protected:

    QString                 m_Name;       ///< DataItem name
    RemoteCare::RCDataItemType_t     m_Type;       ///< DataItem type (digital/analog/string)
    RemoteCare::RCDataItemQuality_t  m_Quality;    ///< DataItem quality (good/bad/uncertain)
    QString                 m_Value;      ///< The value of the DataItem

public:
    CRCDataItem();
    CRCDataItem(const QString Name);
    CRCDataItem(const CRCDataItem&);
    void CopyFromOther(const CRCDataItem &RCDataItem);
    ~CRCDataItem();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CRCDataItem& RCDataItem);
    friend QDataStream& operator >>(QDataStream& InDataStream, CRCDataItem& RCDataItem);
    CRCDataItem& operator=(const CRCDataItem&);

    /******************** INLINE FuNCTIONS **************************************/
    /****************************************************************************/
    /*!
     *  \brief Get's the Name
     *
     *  \return Quality
     */
    /****************************************************************************/
    QString  GetName() const
    {
        return m_Name;
    }

    /****************************************************************************/
    /*!
     *  \brief Set's the Name
     *
     *  \iparam Name
     *
     *  \return
     */
    /****************************************************************************/
    void SetName(const QString Name)
    {
        m_Name = Name;
    }

    /****************************************************************************/
    /*!
     *  \brief Get's the Quality
     *
     *  \return Quality
     */
    /****************************************************************************/
    RemoteCare::RCDataItemType_t  GetType() const
    {
        return m_Type;
    }

    /****************************************************************************/
    /*!
     *  \brief Set's the Type
     *
     *  \iparam Type
     *
     *  \return
     */
    /****************************************************************************/
    void SetType(const RemoteCare::RCDataItemType_t Type)
    {
        m_Type = Type;
    }

    /****************************************************************************/
    /*!
     *  \brief Get's the Quality
     *
     *  \return Quality
     */
    /****************************************************************************/
    RemoteCare::RCDataItemQuality_t  GetQuality() const
    {
        return m_Quality;
    }

    /****************************************************************************/
    /*!
     *  \brief Set's the Quality
     *
     *  \iparam Quality
     *
     *  \return
     */
    /****************************************************************************/
    void SetQuality(const RemoteCare::RCDataItemQuality_t Quality)
    {
        m_Quality = Quality;
    }

    /****************************************************************************/
    /*!
     *  \brief Get's the value
     *
     *  \return value String
     */
    /****************************************************************************/
    QString  GetValue() const
    {
        return m_Value;
    }

    /****************************************************************************/
    /*!
     *  \brief Set's the value
     *
     *  \iparam Value = value string
     *
     *  \return
     */
    /****************************************************************************/
    void SetValue(const QString Value)
    {
        m_Value = Value.trimmed();
    }
};
}   // namespace DataManager

#endif // DATAMANAGER_RCDATAITEM_H
