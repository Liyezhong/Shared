/****************************************************************************/
/*! \file HimalayaUserSettings.h
 *
 *  \brief Definition file for class CHimalayaUserSettings.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2010-08-23, 2012-04-23
 *  $Author:    $ J.Bugariu, Raju
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

#ifndef DATAMANAGER_HIMALAYA_USERSETTINGS_H
#define DATAMANAGER_HIMALAYA_USERSETTINGS_H

#include "DataManager/Containers/UserSettings/Include/UserSettings.h"


namespace DataManager {

/****************************************************************************/
/**
 * \brief Enum for RMS mode.
 */
/****************************************************************************/
enum RMSMode {
    RMS_UNDEFINED = 0,  ///< Undefind. Used for initialization.
    RMS_OFF,            ///< RMS off.
    RMS_CASSETTES,      ///< RMS by cassettes.
    RMS_CYCLES,         ///< RMS by cycles.
    RMS_DAYS            ///< RMS by days.
};

/****************************************************************************/
/**
 * \brief Class for reading / writing XML based configuration file for user settings.
 *
 * <b>This class is not thread safe.</b>
 */
/****************************************************************************/
class CHimalayaUserSettings: public CUserSettings
{
public:
    CHimalayaUserSettings(void);
    CHimalayaUserSettings(const CHimalayaUserSettings &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CHimalayaUserSettings
     *
     *  \return from CHimalayaUserSettings
     */
    /****************************************************************************/
    CHimalayaUserSettings(const CUserSettings &);

    ~CHimalayaUserSettings(void);

    int GetSoundNumberInformation(void) const;

    void SetSoundNumberInformation(int SoundNumberInformation);

    int GetSoundLevelInformation(void) const;

    void SetSoundLevelInformation(int SoundLevelInformation);

    bool GetSoundPeriodicError(void) const;

    void SetSoundPeriodicWarning(bool SoundPeriodicWarning);

    bool GetSoundPeriodicInformation(void) const;

    void SetSoundPeriodicInformation(bool SoundPeriodicInformation);

    int GetSoundPeriodicTimeError(void) const;

    void SetSoundPeriodicTimeError(int SoundPeriodicTimeError);

    void SetSoundPeriodicTimeWarning(int SoundPeriodicTimeWarning);

    int GetSoundPeriodicTimeInformation(void) const;

    void SetSoundPeriodicTimeInformation(int SoundPeriodicTimeInformation);

    int GetTemperatureParaffinBath(void);

    void SetTemperatureParaffinBath(int TemperatureParaffinBath);

    int GetFrequencyPV(void);

    void SetFrequencyPV(int FrequencyPV);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetCassetteCount
     *
     *  \param CassetteCount = int type parameter
     *
     *  \return from SetCassetteCount
     */
    /****************************************************************************/
    void SetCassetteCount(int CassetteCount);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetCassetteCount
     *
     *  \return from GetCassetteCount
     */
    /****************************************************************************/
    int GetCassetteCount();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetReagentIdOfLastStep
     *
     *  \param reagentID = QString type parameter
     *
     *  \return from SetReagentIdOfLastStep
     */
    /****************************************************************************/
    void SetReagentIdOfLastStep(QString& reagentID);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetReagentIdOfLastStep
     *
     *  \return from GetReagentIdOfLastStep
     */
    /****************************************************************************/
    QString GetReagentIdOfLastStep();

    Global::RMSOptions_t GetModeRMSProcessing(void);

    void SetModeRMSProcessing(Global::RMSOptions_t ModeRMSProcessing);

    Global::RMSOptions_t GetModeRMSCleaning(void);

    void SetModeRMSCleaning(Global::RMSOptions_t ModeRMSCleaning);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetOperationHours
     *
     *  \param operationHours = int type parameter
     *
     *  \return from SetOperationHours
     */
    /****************************************************************************/
    void SetOperationHours(int operationHours);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetOperationHours
     *
     *  \return from GetOperationHours
     */
    /****************************************************************************/
    int GetOperationHours();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetActiveCarbonHours
     *
     *  \param operationHours = int type parameter
     *
     *  \return from SetActiveCarbonHours
     */
    /****************************************************************************/
    void SetActiveCarbonHours(int operationHours);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetActiveCarbonHours
     *
     *  \return from GetActiveCarbonHours
     */
    /****************************************************************************/
    int GetActiveCarbonHours();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetOperationLastResetDate
     *
     *  \param resetDate = QString type parameter
     *
     *  \return from SetOperationLastResetDate
     */
    /****************************************************************************/
    void SetOperationLastResetDate(QString& resetDate);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetOperationLastResetDate
     *
     *  \return from GetOperationLastResetDate
     */
    /****************************************************************************/
    QString GetOperationLastResetDate();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetActiveCarbonLastResetDate
     *
     *  \param resetDate = QString type parameter
     *
     *  \return from SetActiveCarbonLastResetDate
     */
    /****************************************************************************/
    void SetActiveCarbonLastResetDate(QString& resetDate);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetActiveCarbonLastResetDate
     *
     *  \return from GetActiveCarbonLastResetDate
     */
    /****************************************************************************/
    QString GetActiveCarbonLastResetDate();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetUseExhaustSystem
     *
     *  \param checked = int type parameter
     *
     *  \return from SetUseExhaustSystem
     */
    /****************************************************************************/
    void SetUseExhaustSystem(int checked);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetUseExhaustSystem
     *
     *  \return from GetUseExhaustSystem
     */
    /****************************************************************************/
    int GetUseExhaustSystem();

    bool operator == (const CHimalayaUserSettings &);
}; // end class CUserSettings

} // end namespace DataManager

#endif // DATAMANAGER_CUSERSETTINGS_H
