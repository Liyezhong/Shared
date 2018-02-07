/****************************************************************************/
/*! \file HimalayaUserSettings.h
 *
 *  \brief Implementation file for class CUserSettings.
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

#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"
#include "DataManager/Helper/Include/Helper.h"


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CHimalayaUserSettings::CHimalayaUserSettings()
    : CUserSettings()
{
    // set default values
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam UserSettings = Instance of the CHimalayaUserSettings class
 *
 *  \return
 */
/****************************************************************************/
CHimalayaUserSettings::CHimalayaUserSettings(
        const CHimalayaUserSettings& UserSettings)
    : CUserSettings(UserSettings)
{
}

CHimalayaUserSettings::CHimalayaUserSettings(const CUserSettings& UserSettings)
    : CUserSettings(UserSettings)
{
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CHimalayaUserSettings::~CHimalayaUserSettings()
{
}

/****************************************************************************/
/*!
 *  \brief Get the sound number information
 *
 *  \return sound number information
 */
/****************************************************************************/
int CHimalayaUserSettings::GetSoundNumberInformation(void) const
{
    return GetValue(INFORMATION_TONE_NUMBER).toInt();
}

/****************************************************************************/
/*!
 *  \brief Set the sound number information
 *
 *  \iparam SoundNumberInformation = number of the sound information
 */
/****************************************************************************/
void CHimalayaUserSettings::SetSoundNumberInformation(
        int SoundNumberInformation)
{
    SetValue(INFORMATION_TONE_NUMBER, QString::number(SoundNumberInformation));
}

/****************************************************************************/
/*!
 *  \brief Get the sound level information
 *
 *  \return sound level information value
 */
/****************************************************************************/
int CHimalayaUserSettings::GetSoundLevelInformation(void) const
{
    return GetValue(INFORMATION_TONE_LEVEL).toInt();
}

/****************************************************************************/
/*!
 *  \brief Set the sound level information
 *
 *  \iparam SoundLevelInformation = volume of the sound level
 */
/****************************************************************************/
void CHimalayaUserSettings::SetSoundLevelInformation(int SoundLevelInformation)
{
    SetValue(INFORMATION_TONE_LEVEL, QString::number(SoundLevelInformation));
}

/****************************************************************************/
/*!
 *  \brief Get the error sound periodic flag
 *
 *  \return error sound periodic flag
 */
/****************************************************************************/
bool CHimalayaUserSettings::GetSoundPeriodicError(void) const
{
    return true;
}

/****************************************************************************/
/*!
 *  \brief Set the warning sound periodic flag
 *
 *  \iparam SoundPeriodicWarning = periodic on/off
 */
/****************************************************************************/
void CHimalayaUserSettings::SetSoundPeriodicWarning(bool SoundPeriodicWarning)
{
    QString Value = PERIODIC_OFF;
    if (SoundPeriodicWarning)
    {
        Value = PERIODIC_ON;
    }

    SetValue(WARNING_TONE_PERIODIC, Value);
}

/****************************************************************************/
/*!
 *  \brief Get the information sound periodic flag
 *
 *  \return information sound periodic flag
 */
/****************************************************************************/
bool CHimalayaUserSettings::GetSoundPeriodicInformation(void) const
{
    QString Value = GetValue(INFORMATION_TONE_PERIODIC).toLower();
    if (PERIODIC_ON == Value)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Set the information sound periodic flag
 *
 *  \iparam SoundPeriodicInformation = periodic on/off
 */
/****************************************************************************/
void CHimalayaUserSettings::SetSoundPeriodicInformation(
        bool SoundPeriodicInformation)
{
    QString Value = PERIODIC_OFF;
    if (SoundPeriodicInformation)
    {
        Value = PERIODIC_ON;
    }

    SetValue(INFORMATION_TONE_PERIODIC, Value);
}

/****************************************************************************/
/*!
 *  \brief Get the error sound periodic time
 *
 *  \return error sound periodic time
 */
/****************************************************************************/
int CHimalayaUserSettings::GetSoundPeriodicTimeError(void) const
{
    return Helper::ConvertTimeStringToSeconds
            (GetValue(ERROR_TONE_PERIODIC_TIME));
}

/****************************************************************************/
/*!
 *  \brief Set the error sound periodic time
 *
 *  \iparam SoundPeriodicTimeError = error sound periodic time
 */
/****************************************************************************/
void CHimalayaUserSettings::SetSoundPeriodicTimeError(
        int SoundPeriodicTimeError)
{
    SetValue(ERROR_TONE_PERIODIC_TIME,
             Helper::ConvertSecondsToTimeString(SoundPeriodicTimeError));
}

/****************************************************************************/
/*!
 *  \brief Set the warning sound periodic time
 *
 *  \iparam SoundPeriodicTimeWarning = warning sound periodic time
 */
/****************************************************************************/
void CHimalayaUserSettings::SetSoundPeriodicTimeWarning(
        int SoundPeriodicTimeWarning)
{
    SetValue(WARNING_TONE_PERIODIC_TIME,
             Helper::ConvertSecondsToTimeString(SoundPeriodicTimeWarning));
}

/****************************************************************************/
/*!
 *  \brief Get the information sound periodic time
 *
 *  \return information sound periodic time
 */
/****************************************************************************/
int CHimalayaUserSettings::GetSoundPeriodicTimeInformation(void) const
{
    return Helper::ConvertTimeStringToSeconds
            (GetValue(INFORMATION_TONE_PERIODIC_TIME));
}

/****************************************************************************/
/*!
 *  \brief Set the information sound periodic time
 *
 *  \iparam SoundPeriodicTimeInformation = information sound periodic time
 */
/****************************************************************************/
void CHimalayaUserSettings::SetSoundPeriodicTimeInformation(
        int SoundPeriodicTimeInformation)
{
    SetValue(INFORMATION_TONE_PERIODIC_TIME,
             Helper::ConvertSecondsToTimeString(SoundPeriodicTimeInformation));
}

/****************************************************************************/
/*!
 *  \brief Gets the temperature of paraffin bath.
 *
 *  \return Paraffin bath temperature.
 */
/****************************************************************************/
int CHimalayaUserSettings::GetTemperatureParaffinBath(void)
{
    return GetValue(PARAFFIN_BATH_TEMPERATURE).toInt();
}

/****************************************************************************/
/*!
 *  \brief Set the temperature of paraffin bath.
 *
 *  \iparam TemperatureParaffinBath = paraffin bath temperature.
 */
/****************************************************************************/
void CHimalayaUserSettings::SetTemperatureParaffinBath(
        int TemperatureParaffinBath)
{
    SetValue(PARAFFIN_BATH_TEMPERATURE,
             QString::number(TemperatureParaffinBath));
}

/****************************************************************************/
/*!
 *  \brief Gets the frequency of Pressure/Vacuum.
 *
 *  \return Frequency of pressure/vacuum.
 */
/****************************************************************************/
int CHimalayaUserSettings::GetFrequencyPV(void)
{
    return GetValue(PV_FREQUENCY).toInt();
}

/****************************************************************************/
/*!
 *  \brief Set the frequency of pressure/vacuum.
 *
 *  \iparam FrequencyPV = frequency of pressure/vacuum.
 */
/****************************************************************************/
void CHimalayaUserSettings::SetFrequencyPV(int FrequencyPV)
{
    SetValue(PV_FREQUENCY, QString::number(FrequencyPV));
}

void CHimalayaUserSettings::SetCassetteCount(int CassetteCount)
{
    SetValue(CASSETTE_COUNT, QString::number(CassetteCount));
}

int CHimalayaUserSettings::GetCassetteCount()
{
    return GetValue(CASSETTE_COUNT).toInt();
}

void CHimalayaUserSettings::SetReagentIdOfLastStep(QString& reagentID)
{
    SetValue(REAGENTID_OF_LASTSTEP, reagentID);
}

QString CHimalayaUserSettings::GetReagentIdOfLastStep()
{
    return GetValue(REAGENTID_OF_LASTSTEP);
}

void CHimalayaUserSettings::SetOperationHours(int operationHours)
{
    SetValue(SERVICE_OPERATION_HOURS, QString::number(operationHours));
}

int CHimalayaUserSettings::GetOperationHours()
{
    return GetValue(SERVICE_OPERATION_HOURS).toInt();
}

void CHimalayaUserSettings::SetOperationLastResetDate(QString& resetDate)
{
    SetValue(SERVICE_OPERATION_LAST_RESET_DATE, resetDate);
}

QString CHimalayaUserSettings::GetOperationLastResetDate()
{
    return GetValue(SERVICE_OPERATION_LAST_RESET_DATE);
}

void CHimalayaUserSettings::SetActiveCarbonLastResetDate(QString& resetDate)
{
    SetValue(SERVICE_ACTIVE_CARBON_LAST_RESET_DATE, resetDate);
}

QString CHimalayaUserSettings::GetActiveCarbonLastResetDate()
{
    return GetValue(SERVICE_ACTIVE_CARBON_LAST_RESET_DATE);
}

void CHimalayaUserSettings::SetActiveCarbonHours(int operationHours)
{
    SetValue(SERVICE_ACTIVE_CARBON_HOURS, QString::number(operationHours));
}

int CHimalayaUserSettings::GetActiveCarbonHours()
{
    return GetValue(SERVICE_ACTIVE_CARBON_HOURS).toInt();
}

void CHimalayaUserSettings::SetUseExhaustSystem(int checked)
{
    SetValue(SERVICE_USE_EXHAUST_SYSTEM, QString::number(checked));
}

int CHimalayaUserSettings::GetUseExhaustSystem()
{
    return GetValue(SERVICE_USE_EXHAUST_SYSTEM).toInt();
}


/****************************************************************************/
/*!
 *  \brief Gets the RMS mode of processing reagents.
 *
 *  \return RMS mode of processing reagents.
 */
/****************************************************************************/
Global::RMSOptions_t CHimalayaUserSettings::GetModeRMSProcessing(void)
{
    QString Value = GetValue(RMS_PROCESSING_MODE).toLower();
    if (RMS_CASSETTES_STRING == Value)
    {
        return Global::RMS_CASSETTES;
    }
    else if (RMS_CYCLES_STRING == Value)
    {
        return Global::RMS_CYCLES;
    }
    else if (RMS_DAYS_STRING == Value)
    {
        return Global::RMS_DAYS;
    }
    else if (RMS_OFF_STRING == Value)
    {
        return Global::RMS_OFF;
    }
    else
    {
        return Global::RMS_UNDEFINED;
    }
}

/****************************************************************************/
/*!
 *  \brief Set the RMS mode of processing reagents.
 *
 *  \iparam ModeRMSProcessing = RMS mode of processing reagents.
 */
/****************************************************************************/
void CHimalayaUserSettings::SetModeRMSProcessing(Global::RMSOptions_t ModeRMSProcessing)
{
    QString Value;
    switch (ModeRMSProcessing)
    {
    case Global::RMS_CASSETTES:
        Value = RMS_CASSETTES_STRING;
        break;
    case Global::RMS_CYCLES:
        Value = RMS_CYCLES_STRING;
        break;
    case Global::RMS_DAYS:
        Value = RMS_DAYS_STRING;
        break;
    case Global::RMS_OFF:
        Value = RMS_OFF_STRING;
        break;
    default:
        return;
    }

    SetValue(RMS_PROCESSING_MODE, Value);
}

/****************************************************************************/
/*!
 *  \brief Gets the RMS mode of cleaning reagents.
 *
 *  \return RMS mode of cleaning reagents.
 */
/****************************************************************************/
Global::RMSOptions_t CHimalayaUserSettings::GetModeRMSCleaning(void)
{
    QString Value = GetValue(RMS_CLEANING_MODE).toLower();
    if (RMS_CYCLES_STRING == Value)
    {
        return Global::RMS_CYCLES;
    }
    else if (RMS_DAYS_STRING == Value)
    {
        return Global::RMS_DAYS;
    }
    else if (RMS_OFF_STRING == Value)
    {
        return Global::RMS_OFF;
    }
    else
    {
        return Global::RMS_UNDEFINED;
    }
}

/****************************************************************************/
/*!
 *  \brief Set the RMS mode of cleaning reagents.
 *
 *  \iparam ModeRMSCleaning = RMS mode of cleaning reagents.
 */
/****************************************************************************/
void CHimalayaUserSettings::SetModeRMSCleaning(Global::RMSOptions_t ModeRMSCleaning)
{
    QString Value;
    switch (ModeRMSCleaning)
    {
    case Global::RMS_CYCLES:
        Value = RMS_CYCLES_STRING;
        break;
    case Global::RMS_DAYS:
        Value = RMS_DAYS_STRING;
        break;
    case Global::RMS_OFF:
        Value = RMS_OFF_STRING;
        break;
    default:
        return;
    }

    SetValue(RMS_CLEANING_MODE, Value);
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which checks if the two object is same.
 *
 *  \iparam UserSettings = CUserSettings class object
 *
 *  \return bool
 */
/****************************************************************************/
bool CHimalayaUserSettings::operator ==(const CHimalayaUserSettings& UserSettings)
{
    // make sure not same object
    if ( this == &UserSettings )
        return true;

    if (this->GetVersion()  != UserSettings.GetVersion() ||
        this->GetLanguage() != UserSettings.GetLanguage() ||
        this->GetDateFormat() != UserSettings.GetDateFormat() ||
        this->GetTimeFormat() != UserSettings.GetTimeFormat() ||
        this->GetTemperatureFormat() != UserSettings.GetTemperatureFormat() ||
        this->GetSoundNumberError() != UserSettings.GetSoundNumberError() ||
        this->GetSoundLevelError() != UserSettings.GetSoundLevelError() ||
        this->GetSoundNumberWarning() != UserSettings.GetSoundNumberWarning() ||
        this->GetSoundLevelWarning() != UserSettings.GetSoundLevelWarning() ||
        this->GetRemoteCare() != UserSettings.GetRemoteCare() ||
        this->GetDirectConnection() != UserSettings.GetDirectConnection() ||
        this->GetProxyUserName() != UserSettings.GetProxyUserName() ||
        this->GetProxyPassword() != UserSettings.GetProxyPassword() ||
        this->GetProxyIPAddress() != UserSettings.GetProxyIPAddress() ||
        this->GetProxyIPPort() != UserSettings.GetProxyIPPort() ||
        this->GetValueList() != UserSettings.GetValueList() )
        return false;

    return true;
}

} // end namespace DataManager
