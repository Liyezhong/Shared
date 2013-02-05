/****************************************************************************/
/*! \file GroupList.h
 *
 *  \brief Definition file for class CGroupList.
 *  This class is implemented to read GroupList information from the XML.
 *  Reads all Group information, stores in a container (QHash) and also writes
 *  data from container to XML.
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

#ifndef DATAMANAGER_PROCESSSETTINGS_H
#define DATAMANAGER_PROCESSSETTINGS_H

#include <QHash>
#include <QMap>
#include <QIODevice>
#include <QReadWriteLock>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QString>

#include "Global/Include/Translator.h"
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Containers/Parameter/Include/Parameter.h"

namespace DataManager
{
typedef QMap<QString, CPositionParameter> PositionParam_t;
typedef QMap<QString, PositionParam_t> DevicePositionList_t;   //!< Hash table for list of positions

typedef QMap<QString, quint8> ProfileParam_t;
typedef QMap<QString, ProfileParam_t> MotionProfileList_t;     //!< Hash table for list of profiles

typedef QList<QString> DeviceList_t;

/****************************************************************************/
/*!
 *  \brief  This class implements Group List
 *  Reading all Group information from XML and FunctionStoring in a Container
 */
/****************************************************************************/
class CProcessSettings: public CDataContainerBase
{
public:
    CProcessSettings();
    CProcessSettings(const CProcessSettings&);
    ~CProcessSettings();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CProcessSettings&  GroupList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CProcessSettings& GroupList);
    CProcessSettings& operator = (const CProcessSettings&);

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

    bool GetSpeedProfile(QString GroupKey, QString ParamKey, quint8& SpeedProfile);
    bool SetSpeedProfile(QString GroupKey, QString ParamKey, quint8 SpeedProfile);

    bool GetPosition(QString GroupKey, QString ParamKey, qint32& Postion);
    bool SetPosition(QString GroupKey, QString ParamKey, const qint32 Position);
    bool ResetPositionOffset(QString GroupKey, QString ParamKey);

private:
    int m_Version;  //!<  version of the file read
    bool m_DataVerificationMode;//!< Verification mode flag , verify the Container
    
    QString m_Filename;//!< filename of the XML file to read

    DeviceList_t m_DeviceList;

    DevicePositionList_t m_DevicePositionList;      //!< Device default Position

    MotionProfileList_t m_SpeedProfileList;        //!< Speed motion profile list of all devices

    QReadWriteLock* mp_ReadWriteLock; //!< Lock for thread safety

    ErrorHash_t m_ErrorHash;    //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator
    
    bool SerializeContent(QIODevice& IODevice, bool CompleteData);

    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);

    bool ReadAllGroups(QXmlStreamReader& XmlStreamReader);
    bool ReadDeviceParameters(QXmlStreamReader& XmlStreamReader, QString DeviceKey);
    bool ReadSpeedProfiles(QXmlStreamReader& XmlStreamReader, QString DeviceKey);
    bool ReadPositions(QXmlStreamReader& XmlStreamReader, QString DeviceKey);

    bool WriteAllGroups(QXmlStreamWriter& XmlStreamWriter);

    bool DeleteAllGroup();

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

#endif // DATAMANAGER_PROCESSSETTINGS_H
