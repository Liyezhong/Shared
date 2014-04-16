/****************************************************************************/
/*! \file DataManager/Containers/InstrumentHistory/Include/Module.h
 *
 *  \brief Definition file for class CModule.
 *  This class implements functionalities to read module information
 *  and storing in container
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-20
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
#ifndef DATAMANAGER_MODULE_H
#define DATAMANAGER_MODULE_H

#include <QString>
#include <QDateTime>
#include <QIODevice>
#include <QHash>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "DataManager/Containers/InstrumentHistory/Include/SubModule.h"

//lint -e429

namespace DataManager
{

typedef QHash<QString, CSubModule*> ListofSubModule_t; //!< Hash Table for list of SubModules
typedef QList<QString> SubModuleList_t; //!< List of SubModule Names

/****************************************************************************/
/*!
 *  \brief  This class implements Module
 *  Reads and writes module information
 */
/****************************************************************************/
class CModule
{
    QString m_ModuleName;              //!< name of the Module
    QString m_ModuleDescription;       //!< Description of the Module
    QString m_SerialNumber;            //!< Serial Number of the Module
    QString m_OperatingHours;          //!< Operating Hours
    QString m_CalibrationDate;         //!< Calibration Date
    QString m_CalibrationResult;       //!< Calibration Result
    QString m_TestDate;                //!< Test Date
    QString m_TestResult;              //!< Test Result

    ListofSubModule_t m_SubModuleList; //!< List of SubModules
    SubModuleList_t m_OrderedSubModuleList; //!< List of SubModule Names

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

    bool ReadSubModuleInfo(QXmlStreamReader& XmlStreamReader, bool CompleteData);

    friend class CModuleDataListArchive;
    friend class CModuleDataList;
    friend QDataStream& operator <<(QDataStream& OutDataStream, const CModule&  Moduleinfo);
    friend QDataStream& operator >>(QDataStream& InDataStream, CModule& Moduleinfo);

    QStringList GetSubModuleIdList() const;

public:
    CModule();
    CModule( QString );
    CModule(QString, QString, QString, QString); //!< To Set ModuleName, Description, SerialNumber, Operating hrs
    CModule(const CModule&);    //!< Copy Constructor
    void CopyFromOther(const CModule &ModuleInfo);
    ~CModule();

    CModule& operator=(const CModule& ModuleInfo);     //!< Assignment Operator Overloading

    /****************************************************************************/
    /*!
     *  \brief  To set Module Name
     *  \iparam value = name to set
     */
    /****************************************************************************/
    void SetModuleName(const QString value) {  m_ModuleName = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the name of the Module
     *  \return Module name
     */
    /****************************************************************************/
    QString GetModuleName() const { return m_ModuleName; }

    /****************************************************************************/
    /*!
     *  \brief  To set Module Description
     *  \iparam value = Module Description to set
     */
    /****************************************************************************/
    void SetModuleDescription(const QString value) {  m_ModuleDescription = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the description of the Module
     *  \return Module Description
     */
    /****************************************************************************/
    QString GetModuleDescription() const { return m_ModuleDescription; }

    /****************************************************************************/
    /*!
     *  \brief  To set Serial Number
     *  \iparam value = serial Number to set
     */
    /****************************************************************************/
    void SetSerialNumber(const QString value) {  m_SerialNumber = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the Serial Number
     *  \return Serial Number
     */
    /****************************************************************************/
    QString GetSerialNumber() const { return m_SerialNumber; }   

    /****************************************************************************/
    /*!
     *  \brief  To set Operating hours
     *  \iparam value = number of hrs to set
     */
    /****************************************************************************/
    void SetOperatingHours(const QString value) {  m_OperatingHours = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the Operating hours
     *  \return number of hours
     */
    /****************************************************************************/
    QString GetOperatingHours() const { return m_OperatingHours; }

    /****************************************************************************/
    /*!
     *  \brief  To set Calibration Date
     *  \iparam value = Date to set
     */
    /****************************************************************************/
    void SetCalibrationDate(const QString value) {  m_CalibrationDate = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the Calibration Date
     *  \return Date
     */
    /****************************************************************************/
    QString GetCalibrationDate() const { return m_CalibrationDate; }

    /****************************************************************************/
    /*!
     *  \brief  To set Calibration Result
     *  \iparam value = Result to set
     */
    /****************************************************************************/
    void SetCalibrationResult(const QString value) {  m_CalibrationResult = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the Calibration Result
     *  \return Result
     */
    /****************************************************************************/
    QString GetCalibrationResult() const { return m_CalibrationResult; }

    /****************************************************************************/
    /*!
     *  \brief  To set Test Date
     *  \iparam value = Date to set
     */
    /****************************************************************************/
    void SetTestDate(const QString value) {  m_TestDate = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the Test Date
     *  \return Date
     */
    /****************************************************************************/
    QString GetTestDate() const { return m_TestDate; }

    /****************************************************************************/
    /*!
     *  \brief  To set Test Result
     *  \iparam value = Result to set
     */
    /****************************************************************************/
    void SetTestResult(const QString value) {  m_TestResult = value; }

    /****************************************************************************/
    /*!
     *  \brief Returns the Test Result
     *  \return Result
     */
    /****************************************************************************/
    QString GetTestResult() const { return m_TestResult; }

    /****************************************************************************/
    /*!
     *  \brief Returns number of SubModule in the list
     *  \return Number of SubModule
     */
    /****************************************************************************/
    int GetNumberofSubModules() const { return m_SubModuleList.count(); }

    /****************************************************************************/
    /*!
     *  \brief Retrieve a SubModule Info with the given SubModuleName
     *  \iparam SubModuleName
     *  \return Pointer to SubModule
     */
    /****************************************************************************/
    CSubModule* GetSubModuleInfo(const QString SubModuleName) const {return m_SubModuleList.value(SubModuleName, NULL);}

    /****************************************************************************/
    /*!
     *  \brief Retrieve a SubModule Info with the given index
     *  \iparam Index
     *  \return Pointer to SubModuleInfo
     */
    /****************************************************************************/
    CSubModule* GetSubModuleInfo(const unsigned int Index) const; // uses order index

    /****************************************************************************/
    /*!
     *  \brief  To writes data to container
     *  \iparam p_SubModule = SubModule Object
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    bool AddSubModuleInfo(CSubModule const* p_SubModule);

    /****************************************************************************/
    /*!
     *  \brief  Deletes the SubModuleInfo
     *  \iparam Index
     *  \return true - delete success , false - delete failure
     */
    /****************************************************************************/
    bool DeleteSubModule(const unsigned int Index);

    /****************************************************************************/
    /*!
     *  \brief  Deletes all the SubSystem in the list
     *  \return true - delete success , false - delete failure
     */
    /****************************************************************************/
    bool DeleteAllSubModule();

    /****************************************************************************/
    /*!
     *  \brief  Deletes the SubModule
     *  \iparam SubModuleName
     *  \return true - delete success , false - delete failure
     */
    /****************************************************************************/
    bool DeleteSubModule(const QString SubModuleName);

    /****************************************************************************/
    /*!
     *  \brief  Updates SubModule
     *  \iparam p_SubModule = Sub Module which needs to be updated
     *  \return true - update success, false - update failed
     */
    /****************************************************************************/
    bool UpdateSubModule(const CSubModule* p_SubModule);


};

}   // namespace DataManager

#endif // DATAMANAGER_MODULE_H
