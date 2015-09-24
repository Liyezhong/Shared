/****************************************************************************/
/*! \file Global/Include/Translator.h
 *
 *  \brief Definition file for class Translator.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
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

#ifndef GLOBAL_TRANSLATOR_H
#define GLOBAL_TRANSLATOR_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/TranslatableString.h>

#include <QHash>
#include <QReadWriteLock>
#include <QLocale>
#include <QStringList>

namespace Global {

typedef QHash<quint32,  QStringList>            tLanguageData;  ///< Typedef for translations for one language.
typedef QHash<QLocale::Language, tLanguageData> tTranslations;  ///< Typedef for translations for all data.

/****************************************************************************/
/**
 * \brief Class used to translate strings with arguments.
 *
 * See method InsertArguments for how translation is done. A default and
 * a fallback language are also defined.
 * <b>This class is thread safe.</b>
 */
/****************************************************************************/
class Translator {
friend class TestTranslator;
private:
    QLocale::Language       m_DefaultLanguage;      ///< Try to translate to this language by default.
    QLocale::Language       m_FallbackLanguage;     ///< If language for translation not found, try to translate to this language.
    tTranslations           m_Translations;         ///< Translations for all loaded languages.
    mutable QReadWriteLock  m_SyncObject;           ///< Synchronisation object.
    /****************************************************************************/
    /****************************************************************************/
    /**
     * \brief Disable copy and assignment
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(Translator)
    /****************************************************************************/
    /**
     * \brief Generate a minimal language independent string for displaying a string.
     *
     * This will look like this: '"StringID":'
     *
     * \iparam   StringID    ID to insert.
     * \return                  The generated string.
     */
    /****************************************************************************/
    QString GenerateMinimalString(quint32 StringID) const;
    /****************************************************************************/
    /**
     * \brief Insert arguments in string.
     *
     * Arguments will be inserted in rString in that way, that any placeholder of
     * type %x (with x > 0) will be replaced by the corresponding entry from
     * ArgumentList. If there is no matching argument in ArgumentList, the placeholder
     * will not be touched. At least if x < 10, else "%10" might be interpreted as
     * "%1"0. this means that for a rString like
     * "Bla %3 %2 %2 %4 %20" with ArgumentList containing the strings
     * "S1" "S2" "S3" the result will be "Bla S3 S2 S2 %4 S20"
     * <b>Remember that counting for placeholders starts by 1.</b>
     *
     * \param[in,out]   rString         String in which arguments have to be inserted.
     * \iparam       ArgumentList    List with arguments to insert.
     * \iparam       ChopArguments   true indicates Chop arguments
     */
    /****************************************************************************/
    void InsertArguments(QString &rString, const QStringList &ArgumentList, const bool ChopArguments) const;

protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    Translator();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~Translator();
    /****************************************************************************/
    /**
     * \brief Reset to defined state.
     *
     * All language data is deleted. m_DefaultLanguage and m_FallbackLanguage are resetted.
     */
    /****************************************************************************/
    void Reset();
    /****************************************************************************/
    /**
     * \brief Set new language data.
     *
     * Any existing data for the language will be replaced by the new data. If
     * wanted, the language will be marked as fallback language.
     *
     * \iparam   TheLanguage             The language to set.
     * \iparam   LanguageData            Data for the specific language.
     * \iparam   SetAsDefaultLanguage    If true, the default language will be set to this language.
     * \iparam   SetAsFallbackLanguage   If true, the fallback language will be set to this language.
     */
    /****************************************************************************/
    void SetLanguageData(QLocale::Language TheLanguage, const tLanguageData &LanguageData,
                         bool SetAsDefaultLanguage, bool SetAsFallbackLanguage);
    /****************************************************************************/
    /**
     * \brief Remove specific language data.
     *
     * All strings for a language will be removed. If there is no such language,
     * nothing happens. If it was the fallback language, the fallback language will
     * be set to QLocale::C.
     *
     * \iparam   TheLanguage     The language to remove.
     */
    /****************************************************************************/
    void RemoveLanguageData(QLocale::Language TheLanguage);
    /****************************************************************************/
    /**
     * \brief Set new default language.
     *
     * If desired language does not exist, the default language is not changed and
     * false is returned.
     *
     * \iparam   TheLanguage     The language to set.
     */
    /****************************************************************************/
    void SetDefaultLanguage(QLocale::Language TheLanguage);
    /****************************************************************************/
    /**
     * \brief Get default language.
     *
     * \return  Default language.
     */
    /****************************************************************************/
    QLocale::Language GetDefaultLanguage() const;
    /****************************************************************************/
    /**
     * \brief Set new fallback language.
     *
     * If desired language does not exist, the fallback language is not changed and
     * false is returned.
     *
     * \iparam   TheLanguage     The language to set.
     */
    /****************************************************************************/
    void SetFallbackLanguage(QLocale::Language TheLanguage);
    /****************************************************************************/
    /**
     * \brief Get fallback language.
     *
     * \return  Fallback language.
     */
    /****************************************************************************/
    QLocale::Language GetFallbackLanguage() const;
    /****************************************************************************/
    /**
     * \brief Translate into default language.
     *
     * See documentation to TranslateToLanguage.
     *
     * \iparam   String          String to translate.
     * \iparam   ChopArguments   true indicates chop arguments
     * \iparam   UseAlternateString   true indicates alternate string to be used
     * \return                      The translation.
     */
    /****************************************************************************/
    QString Translate(const TranslatableString &String, const bool UseAlternateString = false, const bool ChopArguments = false) const;
    /****************************************************************************/
    /**
     * \brief Translate.
     *
     * Translation is done with followig steps:
     * - check if language exists
     *   - if not:
     *     - if language is fallback language or fallback language not set
     *       - use predefined minimal strings
     *       - append arguments as string
     *     - else
     *       - translate into fallback language
     *   - if yes
     *     - try to get strings for StringID
     *       - if it fails
     *         - try to get strings for UNKNOWN_STRING_ID
     *           - if this fails
     *             - use predefined minimal strings
     *             - append arguments as string
     *           - if it succeeds
     *             - insert arguments
     *       - if it succeeds
     *         - insert arguments
     *
     * \iparam   TheLanguage     Language to translate into.
     * \iparam   String          String to translate.
     * \iparam   ChopArguments   true indicates chop arguments
     * \iparam   UseAlternateString   true indicates alternate string to be used
     * \return                      The translation.
     */
    /****************************************************************************/
    QString TranslateToLanguage(QLocale::Language TheLanguage, const TranslatableString &String, const bool UseAlternateString = false,
                                const bool ChopArguments = false) const;
    /****************************************************************************/
    /**
     * \brief Get languages.
     *
     * \return      List of languages.
     */
    /****************************************************************************/
    QList<QLocale::Language> GetLanguages() const;
    /****************************************************************************/
    /**
       * \brief Get Translations.
       *
       * \return  Translations.
       */
    /****************************************************************************/
    tTranslations GetTranslations() const { return m_Translations; }
}; // end class Translator

} // end namespace Global

#endif // GLOBAL_TRANSLATOR_H
