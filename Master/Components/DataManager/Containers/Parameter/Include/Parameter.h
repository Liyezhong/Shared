/****************************************************************************/
/*! \file Parameter.h
 *
 *  \brief Definition file for class CParameter.
 *  This class provides functions to read Parameter attributes
 *  from the XML and writing data to the XML
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-01
 *  $Author:    $ Thirumalesha R
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

#ifndef DATAMANAGER_PARAMETER_H
#define DATAMANAGER_PARAMETER_H

#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"

#include <QString>
#include <QStringList>
#include <QIODevice>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace DataManager
{


/****************************************************************************/
/*!
 *  \brief  This class implements Parameter
 *  Reads and Writes Parameter information to XML
 */
/****************************************************************************/
class CParameter
{

    friend class CGroup;

private:
    QString m_ParameterKey;              //!< Key of the Parameter
    int m_ParameterDefaultValue;         //!< DefaultValue of Parameter
    int m_ParameterOffset;               //!< Offset of Parameter
    int m_NumberOfSteps;                 //!< Number of Parameter
    QStringList m_ParameterKeyList;      //!< Parameter Keys list

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

public:
    CParameter();
    CParameter(const QString ParameterKey);
    CParameter(const QString ParameterKey, const int ParameterDefaultValue, const int ParameterOffset);
    CParameter(const CParameter&);
    ~CParameter();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CParameter&  Parameter);
    friend QDataStream& operator >>(QDataStream& InDataStream, CParameter& Parameter);
    CParameter& operator = (const CParameter&);

    /****************************************************************************/
    /*!
     *  \brief  to set Parameter Key
     *  \iparam ParameterKey = Key to set
     */
    /****************************************************************************/
     void SetParameterKey(const QString ParameterKey) { m_ParameterKey =ParameterKey; }

    /****************************************************************************/
    /*!
     *  \brief returns the Key of the Parameter
     *  \return Parameter Key
     */
    /****************************************************************************/
     QString GetParameterKey() const { return m_ParameterKey; }

    /****************************************************************************/
    /*!
     *  \brief  to set Default Value's of Paramter Info
     *  \iparam Parameter Default Value.
     */
    /****************************************************************************/
     void SetParameterDefaultValue(int ParameterDefaultValue) { m_ParameterDefaultValue = ParameterDefaultValue ;}
    
    /****************************************************************************/
    /*!
     *  \brief returns the Parameter Default Value the Parameter
     *  \return Parameter DefaultValue
     */
    /****************************************************************************/
     int GetParameterDefaultValue() { return m_ParameterDefaultValue; }
	/****************************************************************************/
	/*!
     *  \brief  to set Parameter Offset Info
     *  \iparam Value = Offset of Parameter
     */
    /****************************************************************************/
     void SetParameterOffset(int ParameterOffset) { m_ParameterOffset = ParameterOffset ;}
    
    /****************************************************************************/
    /*!
     *  \brief returns the Offset of the Parameter
     *  \return Offset
     */
    /****************************************************************************/
     int GetParameterOffset() { return m_ParameterOffset; }

    /****************************************************************************/
    /*!
     *  \brief Retrieves Number of Parameter of the Group
     *
     *  \return Number of Parameter
     */
    /****************************************************************************/
     int GetNumberOfSteps() const {return m_NumberOfSteps;}

};

}   // namespace DataManager
#endif // DATAMANAGER_PARAMETER_H
