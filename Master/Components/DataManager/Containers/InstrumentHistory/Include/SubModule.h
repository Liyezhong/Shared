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

#ifndef DATAMANAGER_SUBMODULE_H
#define DATAMANAGER_SUBMODULE_H

#include <QString>
#include <QIODevice>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QHash>
#include <QDebug>

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
typedef QList<QString> ListOfParameterNames_t; //!< QList of ParameterNames.
typedef QHash<QString, Parameter*> ListOfParameters_t; //!< QHash for List of Parameters.

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
    CSubModule( QString );
    CSubModule(QString, QString, QString);
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
     *  \brief  To set SubModule's Parameter Info
     *  \iparam Value = Name, unit and value of Parameter
     */
    /****************************************************************************/
    void AddParameterInfo(const QString name, const QString unit, const QString value)
    {
        Parameter* StructParameter = new Parameter;
        StructParameter->ParameterName = name;
        StructParameter->ParameterUnit = unit;
        StructParameter->ParameterValue = value;

        m_ParameterNames.append(StructParameter->ParameterName);
        m_ListOfParameters.insert(StructParameter->ParameterName, StructParameter);
    }

    /****************************************************************************/
    /*!
     *  \brief  To set SubModule's Parameter Info
     *  \iparam Value = Name and value of Parameter
     */
    /****************************************************************************/
    void AddParameterInfo(const QString name, const QString value)
    {
        Parameter* StructParameter = new Parameter;
        StructParameter->ParameterName = name;
        StructParameter->ParameterUnit = "";
        StructParameter->ParameterValue = value;

        m_ParameterNames.append(StructParameter->ParameterName);
        m_ListOfParameters.insert(StructParameter->ParameterName, StructParameter);
    }

    /****************************************************************************/
    /*!
     *  \brief  To update SubModule's Parameter Info
     *  \iparam Value = Name and value of Parameter
     *  \return true if update is successful, false if unsuccessful
     */
    /****************************************************************************/
    bool UpdateParameterInfo(const QString ParameterName, const QString ParameterValue)
    {
        bool Result = false;

        if (m_ListOfParameters.contains(ParameterName)) {
            Parameter* Param = m_ListOfParameters.value(ParameterName);
            Param->ParameterValue = ParameterValue;
            m_ListOfParameters.insert(ParameterName, Param);
            Result = true;
            return Result;
        }

        qDebug() << "Parameter Name Doesnot exist" << endl;
        return Result;
    }

    /****************************************************************************/
    /*!
     *  \brief Returns the Parameter Information
     *  \iparam Value = Name Parameter
     *  \return Parameter Struct
     */
    /****************************************************************************/
    Parameter* GetParameterInfo(QString ParameterName)
    {
        if (m_ListOfParameters.contains(ParameterName)) {

            Parameter* Param = m_ListOfParameters.value(ParameterName);
            return Param;

        }
    }

    /****************************************************************************/
    /*!
     *  \brief Retrieve a Parameter Info with the given index
     *  \iparam Index
     *  \return Parameter Struct
     */
    /****************************************************************************/
    Parameter* GetParameterInfo(const unsigned int Index)
    {
        QString ParameterName = m_ParameterNames.at(Index);
        if (m_ListOfParameters.contains(ParameterName))
        {
            return m_ListOfParameters.value(ParameterName);
        }
    }

    /****************************************************************************/
    /*!
     *  \brief Returns number of parameters in the list
     *  \return number of parameters in the list
     */
    /****************************************************************************/
    int GetNumberOfParameters()
    {
        return m_ListOfParameters.count();
    }

    /****************************************************************************/
    /*!
     *  \brief  Deletes all the Parameters in the list
     *  \return true - delete success , false - delete failure
     */
    /****************************************************************************/
    bool DeleteAllParameters();

private:
    QString m_SubModuleName;    //!< name of the SubModule
    QString m_SubModuleType;    //!< Type of SubModule
    QString m_SubModuleDescription; //!< Description of the SubModule

    SubModulesList_t m_SubModuleList;    //!< List of SubModule
    ListOfParameterNames_t m_ParameterNames; //!< List of Parameter Name;
    ListOfParameters_t m_ListOfParameters; //!< Parameter Information in QHash

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

};

}   // namespace DataManager
#endif // DATAMANAGER_BOARD_H
