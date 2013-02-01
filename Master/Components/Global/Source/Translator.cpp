/****************************************************************************/
/*! \file Global/Source/Translator.cpp
 *
 *  \brief Implementation file for class Translator.
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

#include <Global/Include/Translator.h>

#include <QStringList>
#include <QReadLocker>
#include <QWriteLocker>

namespace Global {

/****************************************************************************/
Translator::Translator() :
    m_DefaultLanguage(QLocale::C),
    m_FallbackLanguage(QLocale::C)
{
}

/****************************************************************************/
Translator::~Translator() {
    m_DefaultLanguage = QLocale::C;
    m_FallbackLanguage = QLocale::C;
}

/****************************************************************************/
void Translator::Reset() {
    QWriteLocker WL(&m_SyncObject);
    // free all resources
    m_Translations.clear();
    // reset m_FallbackLanguage
    m_DefaultLanguage = QLocale::C;
    m_FallbackLanguage = QLocale::C;
}

/****************************************************************************/
void Translator::SetLanguageData(QLocale::Language TheLanguage, const tLanguageData &LanguageData,
                                 bool SetAsDefaultLanguage, bool SetAsFallbackLanguage) {
    QWriteLocker WL(&m_SyncObject);
    m_Translations.insert(TheLanguage, LanguageData);
    if(SetAsDefaultLanguage) {
        m_DefaultLanguage = TheLanguage;
    }
    if(SetAsFallbackLanguage) {
        m_FallbackLanguage = TheLanguage;
    }
}

/****************************************************************************/
void Translator::RemoveLanguageData(QLocale::Language TheLanguage) {
    QWriteLocker WL(&m_SyncObject);
    // remove language data
    m_Translations.remove(TheLanguage);
    // check if it was default language
    if(m_DefaultLanguage == TheLanguage) {
        // set default language to undefined
        m_DefaultLanguage = QLocale::C;
    }
    // check if it was fallback language
    if(m_FallbackLanguage == TheLanguage) {
        // set fallback language to undefined
        m_FallbackLanguage = QLocale::C;
    }
}

/****************************************************************************/
void Translator::SetDefaultLanguage(QLocale::Language TheLanguage) {
    QWriteLocker WL(&m_SyncObject);
    // check if language exists
    if(!m_Translations.contains(TheLanguage)) {
        // Language does not exist. Keep old default language.
        return;
    }
    // set as default language
    m_DefaultLanguage = TheLanguage;
}

/****************************************************************************/
QLocale::Language Translator::GetDefaultLanguage() const {
    QReadLocker WL(&m_SyncObject);
    return m_DefaultLanguage;
}

/****************************************************************************/
void Translator::SetFallbackLanguage(QLocale::Language TheLanguage) {
    QWriteLocker WL(&m_SyncObject);
    // check if language exists
    if(!m_Translations.contains(TheLanguage)) {
        // Language does not exist. Keep old fallback language.
        return;
    }
    // set as fallback language
    m_FallbackLanguage = TheLanguage;
}

/****************************************************************************/
QLocale::Language Translator::GetFallbackLanguage() const {
    QReadLocker WL(&m_SyncObject);
    return m_FallbackLanguage;
}

/****************************************************************************/
void Translator::InsertArguments(QString &rString, const QStringList &ArgumentList) const {
    // we do not use QString.arg(...) because of following:
    // - arg replaces the lowest number unreplaced, which makes something like
    //   "gnagna %1 with %3" (not displaying argument 2 from argument list)
    //   impossible.
    // - From Qt's QString documentation for 4.6.3:
    //   "If there is no unreplaced place marker remaining, a warning message is
    //   output and the result is undefined."
    //   which makes additional checkings mandatory (using QRegExp for example)
    // So, if we have to check ourself we can use our own code for replacing...
    QString StringToBeReplaced;
    QStringListIterator It(ArgumentList);
    quint32 IndexCount = 1;
    bool VariableNumberOfArguements = false;
    while(It.hasNext()) {
        StringToBeReplaced = QString("%")+QString::number(IndexCount, 10);
        QString s = It.next();
        if (rString.contains(StringToBeReplaced)) {
            rString.replace(StringToBeReplaced, s);
        }
        else if (rString.contains("%n")) {
            VariableNumberOfArguements = true;
            break;
        }
        IndexCount++;
    }
    if (VariableNumberOfArguements) {
        QString Arguement;
        It.previous();
        while (It.hasNext()) {
            Arguement.append(It.next());
            Arguement.append(",");
        }
        //remove the last comma
        Arguement.chop(1);
        rString.replace("%n", Arguement);
    }
}

/****************************************************************************/
QString Translator::GenerateMinimalString(quint32 StringID) const {
    return QString("\"") + QString::number(StringID, 10) + "\":";
}

/****************************************************************************/
QString Translator::TranslateToLanguage(QLocale::Language TheLanguage, const TranslatableString &String, const bool UseAlternateString) const {
    QString Result;
    // check if String is plain string
    if(String.IsString()) {
        Result = String.GetString();
        return Result;
    }
    // extract data
    quint32 StringID = String.GetStringID();
    const tTranslatableStringList & ArgumentList = String.GetArgumentList();
    // check if language exists
    tTranslations::const_iterator it = m_Translations.find(TheLanguage);
    if(it == m_Translations.constEnd()) {
        // language not found.
        // check if already fallback language or undefined fallback language
        if((TheLanguage == m_FallbackLanguage) || (QLocale::C == m_FallbackLanguage)){
            // no translation can be done. Take some extremely basic string with only the string id.
            Result = GenerateMinimalString(StringID);
            // now append arguments
            for(tTranslatableStringList::const_iterator its = ArgumentList.constBegin(); its != ArgumentList.constEnd(); ++its) {
                QString ArgumentTranslation = TranslateToLanguage(TheLanguage, (*its));
                Result = Result + " \"" + ArgumentTranslation + "\"";
            }
        } else {
            // try to translate to fallback language
            Result = TranslateToLanguage(m_FallbackLanguage, String);
        }
    } else {
        // language found. now get string
        tLanguageData::const_iterator it2 = (*it).find(StringID);
        if(it2 == (*it).constEnd()) {
            // string not found. Get string for EVENT_GLOBAL_UNKNOWN_STRING_ID
            it2 = (*it).find(EVENT_GLOBAL_UNKNOWN_STRING_ID);
            if(it2 == (*it).constEnd()) {
                // text for EVENT_GLOBAL_UNKNOWN_STRING_ID also not found.
                // Take some extremely basic string with only the string id.
                Result = GenerateMinimalString(StringID);
            } else {
                // translation for EVENT_GLOBAL_UNKNOWN_STRING_ID found. Insert StringID
                QStringList tmp;
                tmp << QString::number(StringID, 10);
                QStringList StringList = *it2;
                if (StringList.size() >= 1) {
                    Result = StringList.at(0);
                }
                InsertArguments(Result, tmp);
            }
            // now append arguments
            for(tTranslatableStringList::const_iterator its = ArgumentList.constBegin(); its != ArgumentList.constEnd(); ++its) {
                QString ArgumentTranslation = TranslateToLanguage(TheLanguage, (*its));
                Result = Result + " \"" + ArgumentTranslation + "\"";
            }
        } else {
            // string found
            QStringList StringList = *it2;
            if (StringList.size() == 2) {
                if (UseAlternateString) {
                    qDebug()<<"Translator:Alternate String \n\n\n";
                    Result = StringList.at(1);
                }
                else {
                    Result = StringList.at(0);
                }
            }
            else {
                Result = GenerateMinimalString(StringID);
            }
            // now insert arguments
            QStringList Arguments;
            for(tTranslatableStringList::const_iterator its = ArgumentList.constBegin(); its != ArgumentList.constEnd(); ++its) {
                QString ArgumentTranslation = TranslateToLanguage(TheLanguage, (*its));
                // append translated arguments
                Arguments << ArgumentTranslation;
            }
            InsertArguments(Result, Arguments);
        }
    }
    return Result;
}

/****************************************************************************/
QString Translator::Translate(const TranslatableString &String, const bool UseAlternateString) const {
    QReadLocker WL(&m_SyncObject);
    // translate into the default language
    return TranslateToLanguage(m_DefaultLanguage, String, UseAlternateString);
}

/****************************************************************************/
QList<QLocale::Language> Translator::GetLanguages() const {
    return m_Translations.keys();
}

} // end namespace Global
