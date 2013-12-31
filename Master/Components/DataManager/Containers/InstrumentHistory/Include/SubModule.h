/****************************************************************************/
/*! \file DataManager/Containers/InstrumentHistory/Include/SubModule.h
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

//lint -e429

namespace DataManager
{

//!< Structure for Parameter list of Subsystem.
typedef struct {
    QString ParameterName;  //!< name of the Parameter for Module
    QString ParameterUnit;  //!< unit of the Parameter for Module
    QString ParameterValue; //!< Parameter value for Module
} Parameter_t;  //!< structure variable or Parameter

class CSubModule;

typedef QList<QString> ListOfParameterNames_t;              //!< QList of ParameterNames.
typedef QHash<QString, Parameter_t*> ListOfParameters_t;    //!< QHash for List of Parameters.

/****************************************************************************/
/*!
 *  \brief  This class implements SubModule
 *  Reads and Writes SubModule and Parameter information to XML
 */
/****************************************************************************/
class CSubModule
{

    QString m_SubModuleName;        //!< Name of the SubModule
    QString m_SubModuleType;        //!< Type of SubModule
    QString m_SubModuleDescription; //!< Description of the SubModule

    ListOfParameterNames_t m_ParameterNames;    //!< List of Parameter Name;
    ListOfParameters_t m_ListOfParameters;      //!< Parameter Information in QHash

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

    friend class CModule;
    friend QDataStream& operator <<(QDataStream& OutDataStream, const CSubModule&  SubModuleInfo);
    friend QDataStream& operator >>(QDataStream& InDataStream, CSubModule& SubModuleInfo);

public:
    CSubModule();
    CSubModule(QString);
    CSubModule(QString, QString, QString);
    CSubModule(const CSubModule&);  //!< Copy Constructor
    void CopyFromOther(const CSubModule &SubModuleInfo);
    ~CSubModule();
    CSubModule& operator=(const CSubModule&);   //!< Assignment Operator Overloading

    /****************************************************************************/
    /*!
     *  \brief  To set SubModule Name
     *  \iparam value = Name to set
     */
    /****************************************************************************/
    void SetSubModuleName(const QString value) { m_SubModuleName = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the name of the SubModule
     *  \return SubModule name
     */
    /****************************************************************************/
    const QString &GetSubModuleName() const { return m_SubModuleName; }

    /****************************************************************************/
    /*!
     *  \brief  To set SubModule Type
     *  \iparam value = Type to set
     */
    /****************************************************************************/
    void SetSubModuleType(const QString value) { m_SubModuleType = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the type of the SubModule
     *  \return SubModule type
     */
    /****************************************************************************/
    const QString &GetSubModuleType() const { return m_SubModuleType; }

    /****************************************************************************/
    /*!
     *  \brief  To set SubModule description
     *  \iparam value = description
     */
    /****************************************************************************/
    void SetSubModuleDescription(const QString value) { m_SubModuleDescription = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the description of the SubModule
     *  \return SubModule description
     */
    /****************************************************************************/
    const QString &GetSubModuleDescription() const { return m_SubModuleDescription; }

    /****************************************************************************/
    /*!
     *  \brief  To set SubModule's Parameter Info
     *  \iparam name = Name of Parameter
     *  \iparam unit = Unit of Parameter
     *  \iparam value = value of Parameter
     */
    /****************************************************************************/
    void AddParameterInfo(const QString name, const QString unit, const QString value)
    {
        Parameter_t* StructParameter = new Parameter_t;

        StructParameter->ParameterName = name;
        StructParameter->ParameterUnit = unit;
        StructParameter->ParameterValue = value;

        m_ParameterNames.append(StructParameter->ParameterName);
        m_ListOfParameters.insert(StructParameter->ParameterName, StructParameter);

    }

    /****************************************************************************/
    /*!
     *  \brief  To set SubModule's Parameter Info
     *  \iparam name = Name of Parameter
     *  \iparam value = value of Parameter
     */
    /****************************************************************************/
    void AddParameterInfo(const QString name, const QString value)
    {
        Parameter_t* StructParameter = new Parameter_t;

        StructParameter->ParameterName = name;
        StructParameter->ParameterUnit = "";
        StructParameter->ParameterValue = value;

        m_ParameterNames.append(StructParameter->ParameterName);
        m_ListOfParameters.insert(StructParameter->ParameterName, StructParameter);

    }

    /****************************************************************************/
    /*!
     *  \brief  To update SubModule's Parameter Info
     *  \iparam ParameterName = Name of Parameter
     *  \iparam ParameterValue = Value of Parameter
     *  \return true if update is successful, false if unsuccessful
     */
    /****************************************************************************/
    bool UpdateParameterInfo(const QString ParameterName, const QString ParameterValue)
    {
        bool Result = false;

        if (m_ListOfParameters.contains(ParameterName)) {
            Parameter_t* Param = m_ListOfParameters.value(ParameterName);
            Param->ParameterValue = ParameterValue;
            m_ListOfParameters.insert(ParameterName, Param);
            Result = true;
            return Result;
        }

        qDebug() << "Parameter name does not exist." << endl;
        return Result;
    }

    /****************************************************************************/
    /*!
     *  \brief Returns the Parameter Information
     *  \iparam ParameterName = Name Parameter
     *  \return Parameter Struct
     */
    /****************************************************************************/
    Parameter_t* GetParameterInfo(QString ParameterName) const
    {
        if (m_ListOfParameters.contains(ParameterName)) {
            Parameter_t* Param = m_ListOfParameters.value(ParameterName);
            return Param;
        }
        return NULL;
    }

    /****************************************************************************/
    /*!
     *  \brief Retrieve a Parameter Info with the given index
     *  \iparam Index
     *  \return Parameter Struct
     */
    /****************************************************************************/
    Parameter_t* GetParameterInfo(const unsigned int Index) const
    {
        QString ParameterName = m_ParameterNames.at(Index);
        if (m_ListOfParameters.contains(ParameterName))
        {
            Parameter_t *Param = m_ListOfParameters.value(ParameterName);
            return Param;
        }
        return NULL;
    }

    /****************************************************************************/
    /*!
     *  \brief Returns number of parameters in the list
     *  \return number of parameters in the list
     */
    /****************************************************************************/
    int GetNumberOfParameters() const { return m_ListOfParameters.count(); }

    /****************************************************************************/
    /*!
     *  \brief  Deletes all the Parameters in the list
     */
    /****************************************************************************/
    void DeleteAllParameters();

};

} // namespace DataManager

#endif // DATAMANAGER_BOARD_H
