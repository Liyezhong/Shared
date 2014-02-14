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
#include <QString>
#include <QLocale>

#include <iostream>

// the following macros must not be paranthesized
//lint -esym(773, FILE_LINE)
//lint -esym(773, FILE_LINE_LIST)
//lint -esym(773, WHEREAMI)

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
#define WHEREAMI Global::AdjustedTime::Instance().GetCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " " + "0x" + QString::number((unsigned int)QThread::currentThreadId(), 16).toUpper() + " -> " + __PRETTY_FUNCTION__

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
    if(!QObject::connect(pSource, SIGNAL(SignalSource), pTarget, SLOT(SlotTarget))) { \
        THROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT, \
                  Global::tTranslatableStringList() << #pSource << #SignalSource << #pTarget << #SlotTarget << FILE_LINE); \
    }


/****************************************************************************/
/**
 * \brief Guarded QObject::connect for signal -> slot.
 *
 * Guarded QObject::connect for signal -> slot for wueued connections.
 * Throws an exception if connect fails.
 */
/****************************************************************************/
#define CONNECTSIGNALSLOTQUEUED(pSource, SignalSource, pTarget, SlotTarget) \
    if(!QObject::connect(pSource, SIGNAL(SignalSource), pTarget, SLOT(SlotTarget), Qt::QueuedConnection)) { \
        THROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT, \
                  Global::tTranslatableStringList() << #pSource << #SignalSource << #pTarget << #SlotTarget << FILE_LINE); \
    }


/****************************************************************************/
/**
 * \brief Guarded QObject::connect for signal -> signal.
 *
 * Guarded QObject::connect for signal -> signal. Throws an exception if connect fails.
 */
/****************************************************************************/
#define CONNECTSIGNALSIGNAL(pSource, SignalSource, pTarget, SlotTarget) \
    if(!QObject::connect(pSource, SIGNAL(SignalSource), pTarget, SIGNAL(SlotTarget))) { \
        THROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT, \
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
        THROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << #pPtr << FILE_LINE); \
    }


namespace Global {

/****************************************************************************/
/**
 * \brief Set al supported languages.
 *
 * This method initialize the list with an set of languages that have to be
 * supported always. Basicly we call \ref AddSupportedLanguage. If one wants
 * to add some supplementary languages, it has to call \ref AddSupportedLanguage.
 * \warning This method is not thread safe!
 */
/****************************************************************************/
void InitSupportedLanguages();

/****************************************************************************/
/**
 * \brief Add a language to the list of supported languages.
 *
 * Adds a language to the list of supported languages.
 * See also \ref InitSupportedLanguages
 * \warning This method is not thread safe!
 *
 * \param[in]   TheLanguage     The language.
 */
/****************************************************************************/
void AddSupportedLanguage(QLocale::Language TheLanguage);

/****************************************************************************/
/**
 * \brief Convert string to language enum.
 *
 * This conversion is case insensitive. This means that "German", "german" and
 * "GERMAN" return QLocale::German
 * If no match found, QLocale::C is returned.
 *
 * \param[in]   LanguageName    The name of the language as readable language name.
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
 * \param[in]   TheLanguage     The language.
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
 * \param[in]   TheLanguage     The language.
 * \return                      The language code as in ISO 639.
 */
/****************************************************************************/
QString LanguageToLanguageCode(const QLocale::Language TheLanguage);

/****************************************************************************/
/**
 * \brief Convert from gui message type enum to string.
 *
 * \param[in]   TheGUIMessageType   The gui message type to convert.
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
 * \param[in]   GUIMessageTypeString    The name of the gui message type.
 * \param[in]   CaseSensitive           If true, search is done case sensitive.
 * \return                              The gui message type.
 */
/****************************************************************************/
GUIMessageType StringToGUIMessageType(const QString &GUIMessageTypeString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Convert from temperature format enum to string.
 *
 * \param[in]   TheTemperatureFormat    The temperature format to convert.
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
 * \param[in]   TemperatureFormatString     The name of the temperature format.
 * \param[in]   CaseSensitive               If true, search is done case sensitive.
 * \return                                  The temperature format.
 */
/****************************************************************************/
TemperatureFormat StringToTemperatureFormat(const QString &TemperatureFormatString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Convert from on off state enum to string.
 *
 * \param[in]   TheState    The state format to convert.
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
 * \param[in]   OnOffStateString    The name of the on off state.
 * \param[in]   CaseSensitive       If true, search is done case sensitive.
 * \return                          The on off sate.
 */
/****************************************************************************/
OnOffState StringToOnOffState(const QString &OnOffStateString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Convert from date format enum to string.
 *
 * \param[in]   TheFormat   The date format to convert.
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
 * \param[in]   DateFormatString    The name of the date format.
 * \param[in]   CaseSensitive       If true, search is done case sensitive.
 * \return                          The date format.
 */
/****************************************************************************/
DateFormat StringToDateFormat(const QString &DateFormatString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Convert from time format enum to string.
 *
 * \param[in]   TheFormat   The time format to convert.
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
 * \param[in]   TimeFormatString    The name of the time format.
 * \param[in]   CaseSensitive       If true, search is done case sensitive.
 * \return                          The date format.
 */
/****************************************************************************/
TimeFormat StringToTimeFormat(const QString &TimeFormatString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Convert from oven start mode enum to string.
 *
 * \param[in]   TheMode     The oven start mode to convert.
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
 * \param[in]   OvenStartModeString     The name of oven start mode.
 * \param[in]   CaseSensitive           If true, search is done case sensitive.
 * \return                              The oven start mode.
 */
/****************************************************************************/
OvenStartMode StringToOvenStartMode(const QString &OvenStartModeString, bool CaseSensitive);


/****************************************************************************/
/**
 * \brief Output string to console.
 *
 * \param[in]   TheString   The string to write to console
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
 * \param[in]   Value   Value to convert.
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
 * \param[in]   Value   Value to convert.
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
 * \param[in]   TheType     The water type to convert.
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
 * \param[in]   WaterTypeString         The name of water type.
 * \param[in]   CaseSensitive           If true, search is done case sensitive.
 * \return                              The water type.
 */
/****************************************************************************/
WaterType StringToWaterType(const QString &WaterTypeString, bool CaseSensitive);

/****************************************************************************/
/**
 * \brief Convert from boolean true/false to Yes/No string.
 *
 * \param[in]   YesNo       boolean true/false to convert.
 * \return                  To string Yes/No string.
 */
/****************************************************************************/
QString BoolToStringYesNo(bool YesNo);
/****************************************************************************/
/**
 * \brief Convert from Yes/No string to boolean true/false.
 *
 * \param[in]   "Yes"/"No"  string to boolean convert.
 * \return                  To boolean true/false.
 */
/****************************************************************************/
bool StringToTrueFalse(const QString &YesNoStateString, bool CaseSensitive);

} // end namespace Global

#endif // GLOBAL_UTILS_H
