/****************************************************************************/
/*! \file Global/Include/Utils.h
 *
 *  \brief Some utility functions.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-18
 *  $Author:    $ J.Bugariu
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

#ifndef GLOBAL_UTILS_H
#define GLOBAL_UTILS_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/AdjustedTime.h>
#include <Global/Include/Exception.h>
#include <Global/Include/EventObject.h>
#include <QString>
#include <QLocale>

#include <iostream>

// the following macros must not be paranthesized
//lint -esym(773, FILE_LINE)
//lint -esym(773, FILE_LINE_LIST)
//lint -esym(773, WHEREAMI)
//lint -e666
/****************************************************************************/
/**
 * \brief Kit/Bottle max retry count in case of consumable already used and
 *  bottles lot number mismatch.
 */
/****************************************************************************/
const int KIT_BOTTLE_RETRY_COUNT = 3;   //!< Kit/Bottle max retry count

/****************************************************************************/
/**
const int MOUNTING_MEDIUM_COVERSLIP_RETRY_COUNT = 3;   //!< Coverslip/Mounting medium max retry count
* \brief  TWENTY_FOUR_HOURS_IN_SECONDS
*  provides value of 24 hours in seconds
*/
/****************************************************************************/
const quint32 TWENTY_FOUR_HOURS_IN_SECONDS = 86400;  //!< value of 24 hours in seconds

/****************************************************************************/
/**
* \brief NINTY_NINE_HOURS_IN_SECONDS
*    provides value of 99 hours in seconds
*/
/****************************************************************************/
const quint32 NINTY_NINE_HOURS_IN_SECONDS = 356400;  //!< value of 99 hours in seconds

/****************************************************************************/
/**
* \brief SIXTY_SECONDS
*    provides value of 1 minute in seconds
*/
/****************************************************************************/
const quint32 SIXTY_SECONDS = 60;  //!< value of 1 minute in seconds

/****************************************************************************/
/**
* \brief Event log temporary file name suffix
*/
/****************************************************************************/
const QString EVENTLOG_TEMP_FILE_NAME_SUFFIX = "Events_Tmp.log";

/****************************************************************************/
/**
 * \brief Create a QString with file and line information.
 *
 * \return The string.
 */
/****************************************************************************/
#define FILE_LINE QString(__FILE__) << QString::number(__LINE__, 10)
/****************************************************************************/
/**
 * \brief Create a tTranslatableStringList with file and line information.
 *
 * \return The tTranslatableStringList.
 */
/****************************************************************************/
#define FILE_LINE_LIST Global::tTranslatableStringList() << FILE_LINE
/****************************************************************************/
/**
 * \brief Create a QString with timestamp, thread ID, and function name.
 *
 * \return The string.
 */
/****************************************************************************/
#define WHEREAMI Global::AdjustedTime::Instance().GetCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " " + "0x" + static_cast<char*>(QThread::currentThreadId()) + " -> " + __PRETTY_FUNCTION__

/****************************************************************************/
/**
 * \brief Output of current location to console.
 *
 * Output will be done only in debug mode (QT_NO_DEBUG not defined)
 */
/****************************************************************************/
#if defined(QT_NO_DEBUG)
    // we are in release mode
    #define DEBUGWHEREAMI
#else
    // we are in debug mode
    #define DEBUGWHEREAMI try{QString StrTOCONSOLEWHEREAMI(WHEREAMI); Global::ToConsole(StrTOCONSOLEWHEREAMI);}catch(...){}
#endif

/****************************************************************************/
/**
 * \brief Output of a string to console.
 *
 * Output will be done only in debug mode (QT_NO_DEBUG not defined)
 */
/****************************************************************************/
#if defined(QT_NO_DEBUG)
    // we are in release mode
    #define DEBUGMSG(Message)
#else
    // we are in debug mode
    #define DEBUGMSG(Message) try{QString StrDEBUGMSG(Message); Global::ToConsole(StrDEBUGMSG);}catch(...){}
#endif

/****************************************************************************/
/**
 * \brief Guarded QObject::connect for signal -> slot.
 *
 * Guarded QObject::connect for signal -> slot. Throws an exception if connect fails.
 */
/****************************************************************************/
#define CONNECTSIGNALSLOT(pSource, SignalSource, pTarget, SlotTarget) \
    (void)QObject::disconnect((pSource), SIGNAL(SignalSource), (pTarget), SLOT(SlotTarget)); \
    if(!QObject::connect(pSource, SIGNAL(SignalSource), pTarget, SLOT(SlotTarget))) { \
        LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT, \
                  Global::tTranslatableStringList() << #pSource << #SignalSource << #pTarget << #SlotTarget << FILE_LINE) \
    }


/****************************************************************************/
/**
 * \brief Guarded QObject::queued connect for signal -> slot.
 *
 * Guarded QObject::connect for signal -> slot for queued connections.
 * Throws an exception if connect fails.
 */
/****************************************************************************/
#define CONNECTSIGNALSLOTQUEUED(pSource, SignalSource, pTarget, SlotTarget) \
    if(!QObject::connect(pSource, SIGNAL(SignalSource), pTarget, SLOT(SlotTarget), Qt::QueuedConnection)) { \
        LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT, \
                  Global::tTranslatableStringList() << #pSource << #SignalSource << #pTarget << #SlotTarget << FILE_LINE) \
    }

/****************************************************************************/
/**
 * \brief Guarded QObject::direct connect for signal -> slot.
 *
 * Guarded QObject::connect for signal -> slot for direct connections.
 * Throws an exception if connect fails.
 */
/****************************************************************************/
#define CONNECTSIGNALSLOTDIRECT(pSource, SignalSource, pTarget, SlotTarget) \
    if(!QObject::connect(pSource, SIGNAL(SignalSource), pTarget, SLOT(SlotTarget), Qt::DirectConnection)) { \
        LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT, \
                  Global::tTranslatableStringList() << #pSource << #SignalSource << #pTarget << #SlotTarget << FILE_LINE) \
    }

/****************************************************************************/
/**
 * \brief Guarded QObject::connect for signal -> signal.
 *
 * Guarded QObject::connect for signal -> signal. Throws an exception if connect fails.
 */
/****************************************************************************/
#define CONNECTSIGNALSIGNAL(pSource, SignalSource, pTarget, SlotTarget) \
    (void)QObject::disconnect((pSource), SIGNAL(SignalSource), (pTarget), SIGNAL(SlotTarget)); \
    if(!QObject::connect(pSource, SIGNAL(SignalSource), pTarget, SIGNAL(SlotTarget))) { \
        LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT, \
                  Global::tTranslatableStringList() << #pSource << #SignalSource << #pTarget << #SlotTarget << FILE_LINE) \
    }

/****************************************************************************/
/**
 * \brief Guarded QObject::connect for signal -> slot.
 *
 * Guarded QObject::connect for signal -> slot. Log if connect fails.
 */
/****************************************************************************/
#define CONNECTSIGNALSLOTGUI(pSource, SignalSource, pTarget, SlotTarget) \
    (void)QObject::disconnect((pSource), SIGNAL(SignalSource), (pTarget), SLOT(SlotTarget)); \
    if(!QObject::connect(pSource, SIGNAL(SignalSource), pTarget, SLOT(SlotTarget))) { \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT,\
                  Global::tTranslatableStringList() << #pSource << #SignalSource << #pTarget << #SlotTarget << FILE_LINE);\
    }

/****************************************************************************/
/**
 * \brief Guarded QObject::connect for signal -> signal.
 *
 * Guarded QObject::connect for signal -> signal. Log if connect fails.
 */
/****************************************************************************/
#define CONNECTSIGNALSIGNALGUI(pSource, SignalSource, pTarget, SlotTarget) \
    (void)QObject::disconnect((pSource), SIGNAL(SignalSource), (pTarget), SIGNAL(SlotTarget)); \
    if(!QObject::connect(pSource, SIGNAL(SignalSource), pTarget, SIGNAL(SlotTarget))) { \
         Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT, \
                  Global::tTranslatableStringList() << #pSource << #SignalSource << #pTarget << #SlotTarget << FILE_LINE); \
    }

/****************************************************************************/
/**
 * \brief Check if pointer is NULL.
 *
 * Check if pointer is NULL and throws an exception if it is.
 */
/****************************************************************************/
#define CHECKPTR(pPtr) \
    if(pPtr == NULL) { \
        LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << #pPtr << FILE_LINE) \
    }

/****************************************************************************/
/**
 * \brief If pointer is NULL an error message is logged and the value is returned.
 *
 * \param[in]   pPtr    The checked pointer variable.
 * \param[in]   result  The return value.
 */
/****************************************************************************/
#define CHECKPTR_RETURN(pPtr, result)                                                           \
    if(pPtr == NULL) {                                                                          \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_NULL_POINTER,     \
                                                   Global::FmtArgs() << #pPtr                   \
                                                                     << __FILE__ << __LINE__);  \
        return result;                                                                          \
    }


namespace Global {

const QString FixationReagent                = "RG1";
const QString WaterReagent                   = "RG2";
const QString DehydratingDiluted             = "RG3";
const QString DehydratingAbsolute            = "RG4";
const QString ClearingReagent                = "RG5";
const QString ParaffinReagent                = "RG6";

const QString DehydratingDilutedFixation     = "RG3RG1";
const QString FixationDehydratingDiluted     = "RG1RG3";

/****************************************************************************/
/**
* \brief Storage path
*    When USB is mounted then the device will be mounted to /mnt_storage
*/
/****************************************************************************/
const QString DIRECTORY_MNT_STORAGE = "/mnt/mnt_storage";  //!< Storage path of the mounted device


/****************************************************************************/
/**
* \brief Script location path
*    location of the script where mounting of USB device is available
*/
/****************************************************************************/
const QString MNT_SCRIPT = "/home/Leica/Scripts/EBox-Utils.sh";  //!< location of the mounted script


/****************************************************************************/
/**
 * \brief Create a list of language names by using the qt library.
 *
 * This uses the qt library matching locales and creates list of
 * QLocale classes. Then it will be converted to string value
 */
/****************************************************************************/
void CreateListOfLanaguageNames();

/****************************************************************************/
/**
 * \brief Convert string to language enum.
 *
 * This conversion is case insensitive. This means that "German", "german" and
 * "GERMAN" return QLocale::German
 * If no match found, QLocale::C is returned.
 *
 * \iparam   LanguageName    The name of the language as readable language name.
 * \return                      The language.
 */
/****************************************************************************/
QLocale::Language StringToLanguage(const QString &LanguageName);

/****************************************************************************/
/**
 * \brief Convert language enum to string.
 *
 * We return the readable language name.
 *
 * \iparam   TheLanguage     The language.
 * \return                      The language name.
 */
/****************************************************************************/
QString LanguageToString(const QLocale::Language TheLanguage);

/****************************************************************************/
/**
 * \brief Convert language enum to string language code as in ISO 639.
 *
 * We return the readable language code as specified in ISO 639.
 *
 * \iparam   TheLanguage     The language.
 * \return                      The language code as in ISO 639.
 */
/****************************************************************************/
QString LanguageToLanguageCode(const QLocale::Language TheLanguage);

/****************************************************************************/
/**
 * \brief Convert from gui message type enum to string.
 *
 * \iparam   TheGUIMessageType   The gui message type to convert.
 * \return                          To string converted gui message type.
 */
/****************************************************************************/
QString GUIMessageTypeToString(GUIMessageType TheGUIMessageType);

/****************************************************************************/
/**
 * \brief Convert string to GUIMessageType enum.
 *
 * If no match, \ref GUIMSGTYPE_UNDEFINED is returned.
 *
 * \iparam   GUIMessageTypeString    The name of the gui message type.
 * \iparam   CaseSensitive           If true, search is done case sensitive.
 * \return                              The gui message type.
 */
/****************************************************************************/
GUIMessageType StringToGUIMessageType(const QString &GUIMessageTypeString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Convert from temperature format enum to string.
 *
 * \iparam   TheTemperatureFormat    The temperature format to convert.
 * \return                              To string converted temperature format.
 */
/****************************************************************************/
QString TemperatureFormatToString(TemperatureFormat TheTemperatureFormat);

/****************************************************************************/
/**
 * \brief Convert string to TemperatureFormat enum.
 *
 * If no match, \ref TEMP_FORMAT_UNDEFINED is returned.
 *
 * \iparam   TemperatureFormatString     The name of the temperature format.
 * \iparam   CaseSensitive               If true, search is done case sensitive.
 * \return                                  The temperature format.
 */
/****************************************************************************/
TemperatureFormat StringToTemperatureFormat(const QString &TemperatureFormatString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Convert from on off state enum to string.
 *
 * \iparam   TheState    The state format to convert.
 * \return                  To string converted on off state.
 */
/****************************************************************************/
QString OnOffStateToString(OnOffState TheState);

/****************************************************************************/
/**
 * \brief Convert string to OnOffState enum.
 *
 * If no match, \ref ONOFFSTATE_UNDEFINED is returned.
 *
 * \iparam   OnOffStateString    Ok-Yes+No-Continue+Stop-Ok+CancelThe name of the on off state.
 * \iparam   CaseSensitive       If true, search is done case sensitive.
 * \return                          The on off sate.
 */
/****************************************************************************/
OnOffState StringToOnOffState(const QString &OnOffStateString, bool CaseSensitive = false);

/****************************************************************************/
/**
 * \brief Convert from date format enum to string.
 *
 * \iparam   TheFormat   The date format to convert.
 * \return                  To string converted date format.
 */
/****************************************************************************/
QString DateFormatToString(DateFormat TheFormat);

/****************************************************************************/
/**
 * \brief Convert string to DateFormat enum.
 *
 * If no match, \ref DATE_UNDEFINED is returned.
 *
 * \iparam   DateFormatString    The name of the date format.
 * \iparam   CaseSensitive       If true, search is done case sensitive.
 * \return                          The date format.
 */
/****************************************************************************/
DateFormat StringToDateFormat(const QString &DateFormatString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Convert from time format enum to string.
 *
 * \iparam   TheFormat   The time format to convert.
 * \return                  To string converted time format.
 */
/****************************************************************************/
QString TimeFormatToString(TimeFormat TheFormat);

/****************************************************************************/
/**
 * \brief Convert string to TimeFormat enum.
 *
 * If no match, \ref TIME_UNDEFINED is returned.
 *
 * \iparam   TimeFormatString    The name of the time format.
 * \iparam   CaseSensitive       If true, search is done case sensitive.
 * \return                          The date format.
 */
/****************************************************************************/
TimeFormat StringToTimeFormat(const QString &TimeFormatString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Compares GivenDate with CurrentDate.
 *
 * If no match, \ref TIME_UNDEFINED is returned.
 *
 * \iparam   CurrentDate = Current Date.
 * \iparam   DateToBeCompared = Date to be comapared.
 * \return      Bool Type True(If DateToBeCompared is more than CurrentDate)
 *              else False
 */
/****************************************************************************/
bool CompareDate(QDate CurrentDate,QDate DateToBeCompared);
/****************************************************************************/
/**
 * \brief Convert from oven start mode enum to string.
 *
 * \iparam   TheMode     The oven start mode to convert.
 * \return                  To string converted oven start mode.
 */
/****************************************************************************/
QString OvenStartModeToString(OvenStartMode TheMode);

/****************************************************************************/
/**
 * \brief Convert string to OvenStartMode enum.
 *
 * If no match, \ref OVENSTART_UNDEFINED is returned.
 *
 * \iparam   OvenStartModeString     The name of oven start mode.
 * \iparam   CaseSensitive           If true, search is done case sensitive.
 * \return                              The oven start mode.
 */
/****************************************************************************/
OvenStartMode StringToOvenStartMode(const QString &OvenStartModeString, bool CaseSensitive = false);


/****************************************************************************/
/**
 * \brief Output string to console.
 *
 * \iparam   TheString   The string to write to console
 */
/****************************************************************************/
inline void ToConsole(const QString &TheString) {
    std::cout << "ToConsole: " << TheString.toStdString() << std::endl;
}

// This method is used to convert something to int. Putting it outside of this
// namespace only to satisfy lint seems counterproductively.
//lint -esym(1573, Global::AsInt)
/****************************************************************************/
/**
 * \brief Convert to int.
 *
 * Used for example for safe convertions of enums to int.
 *
 * \iparam   Value   Value to convert.
 * \return              To integer converted value.
 */
/****************************************************************************/
template<class EnumType> inline int AsInt(const EnumType &Value) {
    return static_cast<int>(Value);
}

// This method is used to convert something to quint32. Putting it outside of this
// namespace only to satisfy lint seems counterproductively.
//lint -esym(1573, Global::Asquint32)
/****************************************************************************/
/**
 * \brief Convert to quint32.
 *
 * Used for example for safe convertions of enums to quint32.
 *
 * \iparam   Value   Value to convert.
 * \return              To quint32 converted value.
 */
/****************************************************************************/
template<class EnumType> inline quint32 Asquint32(const EnumType &Value) {
    return static_cast<quint32>(Value);
}

/****************************************************************************/
/**
 * \brief Convert from water type enum to string.
 *
 * \iparam   TheType     The water type to convert.
 * \return                  To string converted water type.
 */
/****************************************************************************/
QString WaterTypeToString(WaterType TheType);

/****************************************************************************/
/**
 * \brief Convert string to WaterType enum.
 *
 * If no match, \ref WATER_TYPE_UNDEFINED is returned.
 *
 * \iparam   WaterTypeString         The name of water type.
 * \iparam   CaseSensitive           If true, search is done case sensitive.
 * \return                              The water type.
 */
/****************************************************************************/
WaterType StringToWaterType(const QString &WaterTypeString, bool CaseSensitive = false);

/****************************************************************************/
/**
 * \brief Convert from boolean true/false to Yes/No string.
 *
 * \iparam   YesNo       boolean true/false to convert.
 * \return                  To string Yes/No string.
 */
/****************************************************************************/
QString BoolToStringYesNo(bool YesNo);
/****************************************************************************/
/**
 * \brief Convert from Yes/No string to boolean true/false.
 *
 * \iparam   YesNoStateString "Yes"/"No"  string to boolean convert.
 * \iparam      CaseSensitive = true if case sensitive, else false
 * \return                  To boolean true/false.
 */
/****************************************************************************/
bool StringToTrueFalse(const QString &YesNoStateString, bool CaseSensitive);
/****************************************************************************/
/**
 * \brief   Returns the Number of Buttons associated with the Button Type
 *
 * \iparam   ButtonType
 * \return      Button count
 */
/****************************************************************************/
quint32 GetButtonCountFromButtonType(Global::GuiButtonType ButtonType);

/****************************************************************************/
/**
 * \brief   Returns the ButtonType enum for String passed
 *
 * \iparam   ButtonTypeString  e.g. "Yes+No",  "Ok"
 * \return      Button count
 */
/****************************************************************************/
Global::GuiButtonType   StringToGuiButtonType(QString ButtonTypeString);


/****************************************************************************/
/**
 * \brief   Mounts the storage device.
 *          First come first serve (It means if the two devices are detected
 *          and names are 'sda' and 'sdb' then 'sda' will be mounted.
 *          If any of the file needs to be searched then need to pass the required
 *          arguments (i.e. Specify the file name as argument)
 *
 * \iparam   Name     Search the file/folder in the mounted device
 *
 * \return   Exit code of the USB mount script
 */
/****************************************************************************/
qint32 MountStorageDevice(QString Name = "");


/****************************************************************************/
/**
 * \brief   Unmounts the storage device
 *
 */
/****************************************************************************/
void UnMountStorageDevice();

/****************************************************************************/
/**
 * \brief  Dump string to console.
 * \param  StringToDump string
 */
/****************************************************************************/
void DumpToConsole(const QString StringToDump);

/****************************************************************************/
/**
 * \brief   Workaround checking
 *
 * \param  Type = workaround type
 *
 * \return bool true - workaround enabled, false - workaround disabled
 */
/****************************************************************************/
bool Workaroundchecking(const QString& Type);

/****************************************************************************/
/**
 * \brief  Thread Priority types
 *
 */
/****************************************************************************/
typedef enum {
    LOW_PRIO = 1, //!< Thread running as LOW_PRIO would only run when the processor would otherwise be idle
    BATCH_PRIO, //!< Batch style execution
    DEFAULT_PRIO, //!< default prio -> simple round robin mechanism
    HIGH_PRIO //!< Thread will be given more priority -> real time round robin mechanism
}ThreadPrio_t;

/****************************************************************************/
/**
 * \brief  This function sets the priority for a running thread.
 * //!\warning Setting thread priority can lead to race conditions!
 *
 * \iparam ThreadPriority = Low/Default/High
 */
/****************************************************************************/
void SetThreadPriority(const ThreadPrio_t ThreadPriority);

/**
* \brief function  UpdateRebootFile
* \param RebootFileContent content
*/
void UpdateRebootFile(const QMap<QString, QString> RebootFileContent);

/**
* \brief function  WriteBootVariable
* \param content = variable
* \param file = boot file
*/
void WriteBootVariable(const QMap<QString, QString> content, QFile &file);

/****************************************************************************/
/**
 * \brief  Creates the symbolic link to font files to system font folder
 *
 */
/****************************************************************************/
void CreateSymbolicLinkToFonts();
/****************************************************************************/
/**
 * \brief  Get the Flag of MantainenceFirstRecord
 * \return true indicates the first record for Mantainence
 *
 */
/****************************************************************************/
bool GetMantainenceFirstRecordFlag();
/****************************************************************************/
/**
 * \brief  Set the Flag of MantainenceFirstRecord
 * \param true indicates the first record for Mantainence
 *
 */
/****************************************************************************/
void SetMantainenceFirstRecordFlag(bool bSet);

} // end namespace Global

#endif // GLOBAL_UTILS_H
