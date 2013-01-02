/****************************************************************************/
/*! \file UserSettingsInterface.h
 *
 *  \brief UserSettingsVerifier definition.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-04-23
 *  $Author:    $ Raju
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
#ifndef DATAMANAGER_USERSETTINGSINTERFACE_H
#define DATAMANAGER_USERSETTINGSINTERFACE_H

#include <QString>
#include <QReadWriteLock>
#include <QLocale>
#include <QXmlStreamReader>

#include "TestStubDataManager/Containers/UserSettings/Include/UserSettings.h"
#include "TestStubDataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "TestStubDataManager/Containers/ContainerBase/Include/DataContainerBase.h"

//lint -sem(DataManager::CUserSettingsInterface::SetDefaultAttributes,initializer)
//lint -sem(DataManager::CUserSettingsInterface::UpdateUserSettings, custodial(1))

namespace DataManager {

/****************************************************************************/
/**
 * \brief Class for reading / writing XML based configuration file for user settings.
 *
 * <b>This class isl thread safe.</b>
 */
/****************************************************************************/
class CUserSettingsInterface : public CDataContainerBase
{
private:
    QReadWriteLock*             mp_ReadWriteLock;       ///< Synchronisation object.
    bool                        m_DataVerificationMode; ///< Store the Date verification mode flag
    CUserSettings*              mp_UserSettings;        ///< Store the user settings
    QString                     m_FileName;             ///< Store the file name
    bool                        m_WorkStationMode;      ///< Store the workstation mode
    /****************************************************************************/


    bool SerializeContent(QIODevice& p_Device, bool CompleteData);
    bool DeserializeContent(QIODevice& p_Device, bool CompleteData);

protected:
public:

    CUserSettingsInterface();
    CUserSettingsInterface(const CUserSettingsInterface &);

    ~CUserSettingsInterface();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CUserSettingsInterface& USInterface);
    friend QDataStream& operator >>(QDataStream& InDataStream, CUserSettingsInterface& USInterface);

    CUserSettingsInterface & operator = (const CUserSettingsInterface &);

    void SetDefaultAttributes();

    /****************************************************************************/
    /*!
     *  \brief Get the container type
     *
     *  \return True or False
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() { return USERSETTINGS; }

    /****************************************************************************/
    /*!
     *  \brief Get the Work station mode flag
     *
     *  \return True or False
     */
    /****************************************************************************/
    bool GetWorkStationMode() const
    {
        return m_WorkStationMode;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Work station mode flag value
     *
     *  \iparam Value = flag value
     */
    /****************************************************************************/
    void SetWorkStationMode(const bool Value)
    {
        m_WorkStationMode = Value;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the Data verification mode flag
     *
     *  \return True or False
     */
    /****************************************************************************/
    bool GetDataVerificationMode() { return m_DataVerificationMode; }

    /****************************************************************************/
    /*!
     *  \brief Set the Data verification mode flag value
     *
     *  \iparam Value = flag value
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool Value) { m_DataVerificationMode = Value; }

    bool Read(QString FileName);

    bool UpdateUserSettings(const CUserSettings* p_UserSettings);
    // not required now, because by default the interface class does this.
    //CUserSettings* CreateUserSettings();
    CUserSettings* GetUserSettings(bool CopySettings);

    /****************************************************************************/
    /*!
     *  \brief Get the user settings (shallow copy)
     *
     *  \return user settings class
     */
    /****************************************************************************/
    CUserSettings* GetUserSettings() const
    {
        if (mp_UserSettings) {
            return mp_UserSettings;
        }
        else {
            return NULL;
        }
    }

    /****************************************************************************/
    /*!
     *  \brief Get's the version of The User settings File
     *
     *  \return Version Number
     */
    /****************************************************************************/
    int GetVersion()
    {
        // container we don't store the version instead we write everything in CUserSettings class
        // itself. Because User Settings is not a container.
        // check whether user settings exists or not
        if (mp_UserSettings != NULL) {
            return mp_UserSettings->GetVersion();
        }
        return 0;
    }

    /****************************************************************************/
    /*!
     *  \brief Get's the fileName of the User settings
     *
     *  \return File name String
     */
    /****************************************************************************/
    QString GetFilename() {return m_FileName;}


}; // end class CUserSettings

} // end namespace DataManager


#endif // USERSETTINGSINTERFACE_H
