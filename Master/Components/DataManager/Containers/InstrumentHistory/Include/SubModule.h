/****************************************************************************/
/*! \file SubModule.h
 *
 *  \brief Definition file for class CSubSystem.
 *  This class provides functions to read SybSystem and Parameter information
 *  from the XML and writing data to the XML
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

#ifndef DATAMANAGER_BOARD_H
#define DATAMANAGER_BOARD_H

#include "Helper.h"

#include <QString>
#include <QIODevice>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QHash>

namespace DataManager
{

//!< Structure for Parameter list of Subsystem.
struct Parameter
{
    QString ParameterName;
    QString ParameterUnit;
    QString ParameterValue;
};
class CSubModule;

typedef QList<CSubModule*> SubModulesList_t;   //!< QList for list of submodule.
typedef QList<Parameter> ParameterList_t; //!< QList for List of parameters.

/****************************************************************************/
/*!
 *  \brief  This class implements SubModule
 *  Reads and Writes SubModule and Parameter information to XML
 */
/****************************************************************************/
class CSubModule
{

    friend class CModule;
    friend QDataStream& operator <<(QDataStream& OutDataStream, const CSubModule&  SubModule);
    friend QDataStream& operator >>(QDataStream& InDataStream, CSubModule& SubModule);

public:
    CSubModule();
    CSubModule(const CSubModule&);
    ~CSubModule();
    CSubModule& operator=(const CSubModule&);


    /****************************************************************************/
    /*!
     *  \brief  To set SubModule Name
     *  \iparam Value = Name to set
     */
    /****************************************************************************/
    void SetSubModuleName(const QString value) { m_SubModuleName = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the name of the SubModule
     *  \return SubModule name
     */
    /****************************************************************************/
    QString GetSubModuleName() { return m_SubModuleName; }

    /****************************************************************************/
    /*!
     *  \brief  To set SubModule Type
     *  \iparam Value = Type to set
     */
    /****************************************************************************/
    void SetSubModuleType(const QString value) { m_SubModuleType = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the type of the SubModule
     *  \return SubModule type
     */
    /****************************************************************************/
    QString GetSubModuleType() { return m_SubModuleType; }

    /****************************************************************************/
    /*!
     *  \brief  To set SubModule description
     *  \iparam Value = description
     */
    /****************************************************************************/
    void SetSubModuleDescription(const QString value) { m_SubModuleDescription = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the description of the SubModule
     *  \return SubModule description
     */
    /****************************************************************************/
    QString GetSubModuleDescription() { return m_SubModuleDescription; }

    /****************************************************************************/
    /*!
     *  \brief  To set SubModule's Paramter Info
     *  \iparam Value = Name and unit of Parameter
     */
    /****************************************************************************/
    void SetParameterInfo(const QString name, const QString unit, const QString value)
    {
        m_StructParameter.ParameterName = name;
        m_StructParameter.ParameterUnit = unit;
        m_StructParameter.ParameterValue = value;
    }   

    /****************************************************************************/
    /*!
     *  \brief Returns the Parameter Information
     *  \return Parameter Struct
     */
    /****************************************************************************/
    Parameter GetParameterInfo() { return m_StructParameter; }

private:
    QString m_SubModuleName;    //!< name of the SubModule
    QString m_SubModuleType;    //!< Type of SubModule
    QString m_SubModuleDescription; //!< Description of the SubModule

    SubModulesList_t m_SubModuleList;    //!< List of SubModule
    ParameterList_t m_ParameterList; //!< List of parameters in the SubModule
    Parameter m_StructParameter;    //!< Parameter Information

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

};

}   // namespace DataManager
#endif // DATAMANAGER_BOARD_H