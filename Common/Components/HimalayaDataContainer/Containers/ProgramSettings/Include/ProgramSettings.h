/****************************************************************************/
/*! \file ProgramSettings.h
 *
 *  \brief Definition file for class CProgramSettings.
 *  This class is implemented to read Process Settings information from the XML.
 *  Reads all profile and position information, stores in a container (QHash)
 *  and also writes data from container to XML.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-04-02
 *  $Author:    $ L. Yang
 *
 *  \b Company:
 *
 *       Leica Biosystems SH CN
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DATAMANAGER_PRGRAMSETTINGS_H
#define DATAMANAGER_PRGRAMSETTINGS_H

#include "Global/Include/Translator.h"
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"

namespace DataManager{

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class FunctionKey_t
 */
/****************************************************************************/

typedef struct {
    QString key;       ///<  Definition/Declaration of variable key
    QString name;       ///<  Definition/Declaration of variable position
    QString sequence;       ///<  Definition/Declaration of variable group
} FunctionKey_t;
typedef QMap<QString,QString> ParameterKeyValue_t;       ///<  Definition/Declaration of typedef key
typedef QMap<FunctionKey_t,ParameterKeyValue_t> FunctionParameter_t;       ///<  Definition/Declaration of typedef key
typedef QMap<QString,FunctionParameter_t> DeviceFunction_t;       ///<  Definition/Declaration of typedef key

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator<
 *  \iparam key1 key1
 *  \iparam key2 key2
 *  \return from operator<
 */
/****************************************************************************/
inline bool operator <(const FunctionKey_t& key1,const FunctionKey_t& key2)
{
    return (key1.key+key1.name+key1.sequence < key2.key+key2.name+key2.sequence);
}

/****************************************************************************/
/*!
 *  \brief  This class implements Group List
 *  Reading all Group information from XML and FunctionStoring in a Container
 */
/****************************************************************************/
class CProgramSettings: public CDataContainerBase
{

public:
    CProgramSettings();
    CProgramSettings(const CProgramSettings&);
    ~CProgramSettings();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CProgramSettings&  GroupList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CProgramSettings& GroupList);
    CProgramSettings& operator = (const CProgramSettings&);

    void Init();

    /****************************************************************************/
    /*!
     *  \brief returns the  Data container type
     *  \return Data container type
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() {return PARAMETER;}

    /****************************************************************************/
    /*!
        *  \brief retrieves the status of Verification
        *  \return true-verification on , false-verification off
        */
    /****************************************************************************/
    bool GetDataVerificationMode() {return m_DataVerificationMode;}

    /****************************************************************************/
    /*!
        *  \brief sets verification mode
        *  \iparam Value = true-verification on, false - verification off
        */
    /****************************************************************************/
    void SetDataVerificationMode(const bool Value){m_DataVerificationMode = Value;}

    bool Read(QString Filename);

    /****************************************************************************/
    /*!
        *  \brief returns the version of the xml file read
        *  \return file version
        */
    /****************************************************************************/
    int GetVersion() {return m_Version;}

    /****************************************************************************/
    /*!
     *  \brief returns the filename of the xml file read
     *  \return file name
     */
    /****************************************************************************/
    QString GetFilename() {return m_Filename;}

    /****************************************************************************/
    /*!
     *  \brief get parameter's value
     *  \param DeviceKey = const QString type parameter
     *  \param FunctionKey =  const QString type parameter
     *  \param ParameterKey = const QString type parameter
     *  \param ok =  bool type parameter
     *  \return parameter value
     */
    /****************************************************************************/
    double GetParameterValue(const QString& DeviceKey, const QString& FunctionKey,const QString& ParameterKey, bool& ok);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetParameterValue
     *
     *  \param DeviceKey = const QString type parameter
     *  \param FunctionKey =  const FunctionKey_t type parameter
     *  \param ParameterKey = const QString type parameter
     *  \param ok = bool type parameter
     *  \return from GetParameterValue
     */
    /****************************************************************************/
    double GetParameterValue(const QString& DeviceKey, const FunctionKey_t& FunctionKey,const QString& ParameterKey, bool& ok);
    /**
      * \brief  GetParameterStrValue
      * \iparam DeviceKey device key
      * \iparam FunctionKey function key
      * \iparam ParameterKey paramter key
      * \return return str value
    */
    QString GetParameterStrValue(const QString& DeviceKey, const FunctionKey_t& FunctionKey, const QString& ParameterKey);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetParameterValue
     *
     *  \param DeviceKey = const QString type parameter
     *  \param FunctionKey =  const QString type parameter
     *  \param ParameterKey =  const QString type parameter
     *  \param value =  double type parameter
     *
     *  \return from SetParameterValue
     */
    /****************************************************************************/
    bool SetParameterValue(const QString& DeviceKey, const QString& FunctionKey, const QString& ParameterKey, double value);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetParameterValue
     *
     *  \param DeviceKey = const QString type parameter
     *  \param FunctionKey =  const FunctionKey_t type parameter
     *  \param ParameterKey =  const QString type parameter
     *  \param value =  double type parameter
     *
     *  \return from SetParameterValue
     */
    /****************************************************************************/
    bool SetParameterValue(const QString& DeviceKey, const FunctionKey_t& FunctionKey, const QString& ParameterKey, double value);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetParameterValue
     *
     *  \param DeviceKey = const QString type parameter
     *  \param FunctionKey =  const QString type parameter
     *  \param ParameterKey = const QString type parameter
     *  \param value =  const QString type parameter
     *
     *  \return from SetParameterValue
     */
    /****************************************************************************/
    bool SetParameterValue(const QString& DeviceKey, const QString& FunctionKey,const QString& ParameterKey, const QString& value);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetParameterValue
     *
     *  \param DeviceKey = const QString type parameter
     *  \param FunctionKey =  const FunctionKey_t type parameter
     *  \param ParameterKey =  const QString type parameter
     *  \param value =  const QString type parameter
     *
     *  \return from SetParameterValue
     */
    /****************************************************************************/
    bool SetParameterValue(const QString& DeviceKey, const FunctionKey_t& FunctionKey, const QString& ParameterKey, const QString& value);


private:
    int m_Version;  //!<  version of the file read
    bool m_DataVerificationMode;//!< Verification mode flag , verify the Container
    
    QString m_Filename;//!< filename of the XML file to read

    DeviceFunction_t m_Parameters;       ///<  Definition/Declaration of variable m_Parameters

    QReadWriteLock* mp_ReadWriteLock; //!< Lock for thread safety

    ErrorMap_t m_ErrorMap;    //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator
    
    bool SerializeContent(QIODevice& IODevice, bool CompleteData);

    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);

    bool ReadAllParameters(QXmlStreamReader& XmlStreamReader);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ReadDeviceParameters
     *
     *  \param XmlStreamReader = QXmlStreamReader type parameter
     *  \param DeviceKey =  const QString type parameter
     *
     *  \return from ReadDeviceParameters
     */
    /****************************************************************************/
    bool ReadDeviceParameters(QXmlStreamReader& XmlStreamReader, const QString& DeviceKey);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ReadFunctionParameters
     *
     *  \param XmlStreamReader = QXmlStreamReader type parameter
     *  \param DeviceKey =  const QString type parameter
     *  \param FunctionKey =  const FunctionKey_t type parameter
     *
     *  \return from ReadFunctionParameters
     */
    /****************************************************************************/
    bool ReadFunctionParameters(QXmlStreamReader &XmlStreamReader, const QString& DeviceKey, const FunctionKey_t& FunctionKey);


    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function WriteAllParameters
     *
     *  \param XmlStreamWriter = QXmlStreamWriter type parameter
     *
     *  \return from WriteAllParameters
     */
    /****************************************************************************/
    bool WriteAllParameters(QXmlStreamWriter& XmlStreamWriter);

    bool DeleteAll();

    /****************************************************************************/
    /*!
     *  \brief sets the xml version
     *  \iparam Value = Version to set
     */
    /****************************************************************************/
    void SetVersion(const int Value)    {m_Version = Value;}

    /****************************************************************************/
    /*!
     *  \brief Sets the XML file name
     *  \iparam Value = Filename to set
     */
    /****************************************************************************/
    void SetFilename(const QString Value) {m_Filename = Value;}
};


}   // namespace DataManager

#endif // DATAMANAGER_PRGRAMSETTINGS_H
