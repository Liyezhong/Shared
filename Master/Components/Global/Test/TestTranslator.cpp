/****************************************************************************/
/*! \file TestTranslator.cpp
 *
 *  \brief Implementation file for class TestTranslator.
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

#include <QTest>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/UITranslator.h>
#include <Global/Include/GlobalEventCodes.h>

namespace Global {

Translator      TheTranslator;      ///< The translator instance.

/****************************************************************************/
/**
 * \brief Test class for Translator class.
 */
/****************************************************************************/
class TestTranslator : public QObject {
    Q_OBJECT
private:
    /****************************************************************************/
    /**
     * \brief Create a predefined set of strings without EVENT_GLOBAL_UNKNOWN_STRING_ID in German.
     *
     * \return  Language strings for german language.
     */
    /****************************************************************************/
    tLanguageData CreateLanguageStringsGermanWithoutUnknownStringID();
    /****************************************************************************/
    /**
     * \brief Create a predefined set of strings with EVENT_GLOBAL_UNKNOWN_STRING_ID in German.
     *
     * \return  Language strings for german language.
     */
    /****************************************************************************/
    tLanguageData CreateLanguageStringsGermanWithUnknownStringID();
    /****************************************************************************/
    /**
     * \brief Create a predefined set of strings without EVENT_GLOBAL_UNKNOWN_STRING_ID in English.
     *
     * \return  Language strings for english language.
     */
    /****************************************************************************/
    tLanguageData CreateLanguageStringsEnglishWithoutUnknownStringID();
    /****************************************************************************/
    /**
     * \brief Create a predefined set of strings with EVENT_GLOBAL_UNKNOWN_STRING_ID in English.
     *
     * \return  Language strings for english language.
     */
    /****************************************************************************/
    tLanguageData CreateLanguageStringsEnglishWithUnknownStringID();
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     *
     * Here is the only place where we can test the default constructor.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     *
     * Reset translator to a defined state.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();
    /****************************************************************************/
    /**
     * \brief Test of Translator::Reset method.
     */
    /****************************************************************************/
    void utTestReset();
    /****************************************************************************/
    /**
     * \brief Test of Translator::GenerateMinimalString method.
     */
    /****************************************************************************/
    void utTestGenerateMinimalString();
    /****************************************************************************/
    /**
     * \brief Data generation for \ref utTestInsertArguments method.
     */
    /****************************************************************************/
    void utTestInsertArguments_data();
    /****************************************************************************/
    /**
     * \brief Test of Translator::InsertArguments method.
     */
    /****************************************************************************/
    void utTestInsertArguments();
    /****************************************************************************/
    /**
     * \brief Test of Translator::SetLanguageData method.
     */
    /****************************************************************************/
    void utTestSetLanguageData();
    /****************************************************************************/
    /**
     * \brief Test of Translator::RemoveLanguageData method.
     */
    /****************************************************************************/
    void utTestRemoveLanguageData();
    /****************************************************************************/
    /**
     * \brief Test of Translator::SetDefaultLanguage method.
     */
    /****************************************************************************/
    void utTestSetDefaultLanguage();
    /****************************************************************************/
    /**
     * \brief Test of Translator::SetFallbackLanguage method.
     */
    /****************************************************************************/
    void utTestSetFallbackLanguage();
    /****************************************************************************/
    /**
     * \brief Test of Translator::Translate method for plain string arguments.
     */
    /****************************************************************************/
    void utTestTranslatePlainArgs();
    /****************************************************************************/
    /**
     * \brief Test of Translator::Translate method for translatable string arguments.
     */
    /****************************************************************************/
    void utTestTranslateTranslatableArgs();
    /****************************************************************************/
    /**
     * \brief Test of EventTranslator and UITranslator
     */
    /****************************************************************************/
    void utTestEventAndUITranslator();
}; // end class TestTranslator

/****************************************************************************/
void TestTranslator::initTestCase() {
    QCOMPARE(TheTranslator.m_DefaultLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,     QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),  0);
}

/****************************************************************************/
void TestTranslator::init() {
    TheTranslator.Reset();
}

/****************************************************************************/
void TestTranslator::cleanup() {
}

/****************************************************************************/
void TestTranslator::cleanupTestCase() {
}

/****************************************************************************/
void TestTranslator::utTestReset() {
    // test reset function

    // alter test object
    tLanguageData LanguageData;
    LanguageData.insert(1, "SomeText");
    TheTranslator.SetLanguageData(QLocale::German, LanguageData, false, false);
    TheTranslator.m_DefaultLanguage = QLocale::English;
    TheTranslator.m_FallbackLanguage = QLocale::English;
    // test results
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::English);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::English);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);
    // call reset
    TheTranslator.Reset();
    // test results
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   0);
}

/****************************************************************************/
void TestTranslator::utTestGenerateMinimalString() {
    // test creation of a general minimal string
    QCOMPARE(TheTranslator.GenerateMinimalString(0),    QString("\"0\":"));
    QCOMPARE(TheTranslator.GenerateMinimalString(1),    QString("\"1\":"));
    QCOMPARE(TheTranslator.GenerateMinimalString(10),   QString("\"10\":"));
    QCOMPARE(TheTranslator.GenerateMinimalString(123),  QString("\"123\":"));
}

/****************************************************************************/
void TestTranslator::utTestInsertArguments_data() {
    QTest::addColumn<QString>("Result");
    QTest::addColumn<QString>("ExpectedResult");

    // Set 1: argumentlist and arguments in same order. no arguments missing, no supplementary  arguments
    QString String1         = "String: %1, %2. %3%4 %5-%6 %7 %8 %9 %10 %11!";
    QStringList ArgList1    = QStringList() <<"\"A1\""<<"\"A2\""<<"\"A3\""<<"\"A4\""<<"\"A5\""
                                            <<"\"A6\""<<"\"A7\""<<"\"A8\""<<"\"A9\""<<"\"A10\""<<"\"A11\"";
    QString Expected1       = "String: \"A1\", \"A2\". \"A3\"\"A4\" \"A5\"-\"A6\" \"A7\" \"A8\" \"A9\" \"A10\" \"A11\"!";
    TheTranslator.InsertArguments(String1, ArgList1);
    QTest::newRow("Set 1")   << String1 << Expected1;

    // Set 2: argumentlist and arguments in reversed order. no arguments missing, no supplementary  arguments
    QString String2         = "String: %11, %10. %9%8 %7-%6 %5 %4 %3 %2 %1!";
    QStringList ArgList2    = QStringList() <<"\"A1\""<<"\"A2\""<<"\"A3\""<<"\"A4\""<<"\"A5\""
                                            <<"\"A6\""<<"\"A7\""<<"\"A8\""<<"\"A9\""<<"\"A10\""<<"\"A11\"";
    QString Expected2       = "String: \"A11\", \"A10\". \"A9\"\"A8\" \"A7\"-\"A6\" \"A5\" \"A4\" \"A3\" \"A2\" \"A1\"!";
    TheTranslator.InsertArguments(String2, ArgList2);
    QTest::newRow("Set 2")   << String2 << Expected2;

    // Set 3: argumentlist and arguments in same order. last 3 arguments missing, no supplementary  arguments.
    // Remember:(%10) -> (%1)0 and (%11) -> (%1)1 !!!
    QString String3         = "String: %1, %2. %3%4 %5-%6 %7 %8 %9 %10 %11!";
    QStringList ArgList3    = QStringList() <<"\"A1\""<<"\"A2\""<<"\"A3\""<<"\"A4\""<<"\"A5\""
                                            <<"\"A6\""<<"\"A7\""<<"\"A8\"";
    QString Expected3       = "String: \"A1\", \"A2\". \"A3\"\"A4\" \"A5\"-\"A6\" \"A7\" \"A8\" %9 \"A1\"0 \"A1\"1!";
    TheTranslator.InsertArguments(String3, ArgList3);
    QTest::newRow("Set 3")   << String3 << Expected3;

    // Set 4: argumentlist and arguments in same order. no arguments missing, 3 supplementary  arguments.
    QString String4         = "String: %1, %2. %3%4 %5-%6 %7 %8 %9 %10 %11!";
    QStringList ArgList4    = QStringList() <<"\"A1\""<<"\"A2\""<<"\"A3\""<<"\"A4\""<<"\"A5\""
                                            <<"\"A6\""<<"\"A7\""<<"\"A8\""<<"\"A9\""<<"\"A10\""<<"\"A11\""
                                            <<"\"A12\""<<"\"A13\""<<"\"A14\"";
    QString Expected4       = "String: \"A1\", \"A2\". \"A3\"\"A4\" \"A5\"-\"A6\" \"A7\" \"A8\" \"A9\" \"A10\" \"A11\"!";
    TheTranslator.InsertArguments(String4, ArgList4);
    QTest::newRow("Set 4")   << String4 << Expected4;

    // Set 5: argumentlist and arguments in arbitrary order. arguments missing, supplementary arguments.
    QString String5         = "String: %1, %5. %3%9 %7-%13 %11 %17 %15 %21 %19!";
    QStringList ArgList5    = QStringList() <<"\"A1\""<<"\"A2\""<<"\"A3\""<<"\"A4\""<<"\"A5\""
                                            <<"\"A6\""<<"\"A7\""<<"\"A8\""<<"\"A9\""<<"\"A10\""<<"\"A11\""
                                            <<"\"A12\""<<"\"A13\""<<"\"A14\""<<"\"A15\""<<"\"A16\""<<"\"A17\""
                                            <<"\"A18\""<<"\"A19\""<<"\"A20\""<<"\"A21\""<<"\"A22\""<<"\"A23\""
                                            <<"\"A24\""<<"\"A25\""<<"\"A26\""<<"\"A27\""<<"\"A28\""<<"\"A29\"";
    QString Expected5       = "String: \"A1\", \"A5\". \"A3\"\"A9\" \"A7\"-\"A13\" \"A11\" \"A17\" \"A15\" \"A21\" \"A19\"!";
    TheTranslator.InsertArguments(String5, ArgList5);
    QTest::newRow("Set 5")   << String5 << Expected5;

}

/****************************************************************************/
void TestTranslator::utTestInsertArguments() {
    QFETCH(QString, Result);
    QFETCH(QString, ExpectedResult);

    QCOMPARE(Result, ExpectedResult);
}

/****************************************************************************/
tLanguageData TestTranslator::CreateLanguageStringsGermanWithoutUnknownStringID() {
    tLanguageData Result;
    Result.insert(2, "Zeile 2: %1 %2 %3");
    Result.insert(3, "Zeile 3: %1 %2 %3");
    Result.insert(5, "Z%1");
    return Result;
}

/****************************************************************************/
tLanguageData TestTranslator::CreateLanguageStringsGermanWithUnknownStringID() {
    tLanguageData Result = CreateLanguageStringsGermanWithoutUnknownStringID();
    Result.insert(EVENT_GLOBAL_UNKNOWN_STRING_ID, "Unbekannter Text: %1. Argumente:");
    return Result;
}

/****************************************************************************/
tLanguageData TestTranslator::CreateLanguageStringsEnglishWithoutUnknownStringID() {
    tLanguageData Result;
    Result.insert(2, "Line 2: %1 %2 %3");
    Result.insert(3, "Line 3: %1 %2 %3");
    Result.insert(5, "L%1");
    return Result;
}

/****************************************************************************/
tLanguageData TestTranslator::CreateLanguageStringsEnglishWithUnknownStringID() {
    tLanguageData Result = CreateLanguageStringsEnglishWithoutUnknownStringID();
    Result.insert(EVENT_GLOBAL_UNKNOWN_STRING_ID, "Unknown text: %1. Arguments:");
    return Result;
}

/****************************************************************************/
void TestTranslator::utTestSetLanguageData() {
    tLanguageData DataGerman  = CreateLanguageStringsGermanWithUnknownStringID();
    tLanguageData DataEnglish = CreateLanguageStringsEnglishWithUnknownStringID();
    // test initial state
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   0);

    // insert german language no default no fallback
    TheTranslator.SetLanguageData(QLocale::German, DataGerman, false, false);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);
    QCOMPARE(TheTranslator.GetLanguages().size(),   1);

    // reinsert german language as default
    TheTranslator.SetLanguageData(QLocale::German, DataGerman, true, false);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::German);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);
    QCOMPARE(TheTranslator.GetLanguages().size(),   1);

    // insert english language as fallback
    TheTranslator.SetLanguageData(QLocale::English, DataEnglish, false, true);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::German);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::English);
    QCOMPARE(TheTranslator.m_Translations.size(),   2);
    QCOMPARE(TheTranslator.GetLanguages().size(),   2);

    // reinsert german language as fallback
    TheTranslator.SetLanguageData(QLocale::German, DataGerman, false, true);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::German);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::German);
    QCOMPARE(TheTranslator.m_Translations.size(),   2);
    QCOMPARE(TheTranslator.GetLanguages().size(),   2);

    // reinsert english language default no fallback
    TheTranslator.SetLanguageData(QLocale::English, DataEnglish, true, false);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::English);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::German);
    QCOMPARE(TheTranslator.m_Translations.size(),   2);
    QCOMPARE(TheTranslator.GetLanguages().size(),   2);

}

/****************************************************************************/
void TestTranslator::utTestRemoveLanguageData() {
    tLanguageData DataGerman  = CreateLanguageStringsGermanWithUnknownStringID();
    tLanguageData DataEnglish = CreateLanguageStringsEnglishWithUnknownStringID();

    // test initial state
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   0);

    // remove not existing english language
    TheTranslator.RemoveLanguageData(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   0);

    // insert german language no default no fallback
    TheTranslator.SetLanguageData(QLocale::German, DataGerman, false, false);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);

    // remove not existing english language
    TheTranslator.RemoveLanguageData(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);

    // remove existing german language
    TheTranslator.RemoveLanguageData(QLocale::German);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   0);

    // insert german language default
    TheTranslator.SetLanguageData(QLocale::German, DataGerman, true, false);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::German);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);

    // remove not existing english language
    TheTranslator.RemoveLanguageData(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::German);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);

    // remove existing german language
    TheTranslator.RemoveLanguageData(QLocale::German);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   0);

    // insert german language fallback
    TheTranslator.SetLanguageData(QLocale::German, DataGerman, false, true);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::German);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);

    // remove not existing english language
    TheTranslator.RemoveLanguageData(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::German);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);

    // remove existing german language
    TheTranslator.RemoveLanguageData(QLocale::German);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   0);

    // insert english language default and fallback
    TheTranslator.SetLanguageData(QLocale::English, DataEnglish, true, true);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::English);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::English);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);

    // remove not existing german language
    TheTranslator.RemoveLanguageData(QLocale::German);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::English);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::English);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);

    // insert german language default and fallback
    TheTranslator.SetLanguageData(QLocale::German, DataGerman, true, true);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::German);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::German);
    QCOMPARE(TheTranslator.m_Translations.size(),   2);

    // remove existing german language
    TheTranslator.RemoveLanguageData(QLocale::German);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   1);

    // remove existing english language
    TheTranslator.RemoveLanguageData(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.m_Translations.size(),   0);
}

/****************************************************************************/
void TestTranslator::utTestSetDefaultLanguage() {
    tLanguageData DataGerman  = CreateLanguageStringsGermanWithUnknownStringID();
    tLanguageData DataEnglish = CreateLanguageStringsEnglishWithUnknownStringID();

    // test initial state
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetDefaultLanguage(),    TheTranslator.m_DefaultLanguage);

    // set not existing english language as default
    TheTranslator.SetDefaultLanguage(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetDefaultLanguage(),    TheTranslator.m_DefaultLanguage);

    // insert german language no default
    TheTranslator.SetLanguageData(QLocale::German, DataGerman, false, false);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetDefaultLanguage(),    TheTranslator.m_DefaultLanguage);

    // set not existing english language as default
    TheTranslator.SetDefaultLanguage(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetDefaultLanguage(),    TheTranslator.m_DefaultLanguage);

    // set german language default
    TheTranslator.SetDefaultLanguage(QLocale::German);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::German);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetDefaultLanguage(),    TheTranslator.m_DefaultLanguage);

    // insert english language default
    TheTranslator.SetLanguageData(QLocale::English, DataEnglish, true, false);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::English);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetDefaultLanguage(),    TheTranslator.m_DefaultLanguage);

    // set german language default
    TheTranslator.SetDefaultLanguage(QLocale::German);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::German);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetDefaultLanguage(),    TheTranslator.m_DefaultLanguage);

    // set english language default
    TheTranslator.SetDefaultLanguage(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::English);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetDefaultLanguage(),    TheTranslator.m_DefaultLanguage);
}

/****************************************************************************/
void TestTranslator::utTestSetFallbackLanguage() {
    tLanguageData DataGerman  = CreateLanguageStringsGermanWithUnknownStringID();
    tLanguageData DataEnglish = CreateLanguageStringsEnglishWithUnknownStringID();

    // test initial state
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetFallbackLanguage(),   TheTranslator.m_FallbackLanguage);

    // set not existing english language as fallback
    TheTranslator.SetFallbackLanguage(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetFallbackLanguage(),   TheTranslator.m_FallbackLanguage);

    // insert german language no fallback
    TheTranslator.SetLanguageData(QLocale::German, DataGerman, false, false);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetFallbackLanguage(),   TheTranslator.m_FallbackLanguage);

    // set not existing english language as fallback
    TheTranslator.SetFallbackLanguage(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::C);
    QCOMPARE(TheTranslator.GetFallbackLanguage(),   TheTranslator.m_FallbackLanguage);

    // set german language fallback
    TheTranslator.SetFallbackLanguage(QLocale::German);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::German);
    QCOMPARE(TheTranslator.GetFallbackLanguage(),   TheTranslator.m_FallbackLanguage);

    // insert english language fallback
    TheTranslator.SetLanguageData(QLocale::English, DataEnglish, false, true);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::English);
    QCOMPARE(TheTranslator.GetFallbackLanguage(),   TheTranslator.m_FallbackLanguage);

    // set german language fallback
    TheTranslator.SetFallbackLanguage(QLocale::German);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::German);
    QCOMPARE(TheTranslator.GetFallbackLanguage(),   TheTranslator.m_FallbackLanguage);

    // set english language fallback
    TheTranslator.SetFallbackLanguage(QLocale::English);
    // test
    QCOMPARE(TheTranslator.m_DefaultLanguage,       QLocale::C);
    QCOMPARE(TheTranslator.m_FallbackLanguage,      QLocale::English);
    QCOMPARE(TheTranslator.GetFallbackLanguage(),   TheTranslator.m_FallbackLanguage);
}

/****************************************************************************/
void TestTranslator::utTestTranslatePlainArgs() {
    // create an argument list.
    tTranslatableStringList ArgList = tTranslatableStringList() <<"A1"<<"A2"<<"A3";
    TranslatableString TS2(2, ArgList);
    TranslatableString TS4(4, ArgList);
    TranslatableString TS5(2);
    TranslatableString TS90(2, tTranslatableStringList() << "A1" << "A2");
    TranslatableString TS91(2, tTranslatableStringList() << "A1" << "A2" << "A3" << "A4");
    QString Result;

    // 1. language found, string found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  true);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, false);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS2);
    QCOMPARE(Result,    QString("Zeile 2: A1 A2 A3"));
    Result = TheTranslator.Translate(TS2);
    QCOMPARE(Result,    QString("Line 2: A1 A2 A3"));

    // 2. language found, EVENT_GLOBAL_UNKNOWN_STRING_ID string found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  true);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, false);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS4);
    QCOMPARE(Result,    QString("Unbekannter Text: 4. Argumente: \"A1\" \"A2\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("Unknown text: 4. Arguments: \"A1\" \"A2\" \"A3\""));

    // 3. language found, EVENT_GLOBAL_UNKNOWN_STRING_ID string not found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithoutUnknownStringID(), true,  true);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithoutUnknownStringID(),  false, false);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"A2\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"A2\" \"A3\""));

    // 4. language not found, fallback language found, string found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::French, TS2);
    QCOMPARE(Result,    QString("Zeile 2: A1 A2 A3"));
    Result = TheTranslator.Translate(TS2);
    QCOMPARE(Result,    QString("Line 2: A1 A2 A3"));

    // 5. language not found, fallback language found, EVENT_GLOBAL_UNKNOWN_STRING_ID string found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::French, TS4);
    QCOMPARE(Result,    QString("Unbekannter Text: 4. Argumente: \"A1\" \"A2\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("Unknown text: 4. Arguments: \"A1\" \"A2\" \"A3\""));

    // 6. language not found, fallback language found, EVENT_GLOBAL_UNKNOWN_STRING_ID string not found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithoutUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithoutUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::French, TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"A2\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"A2\" \"A3\""));

    // 7. language not found, fallback language not found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithoutUnknownStringID(), false, false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithoutUnknownStringID(),  false, false);
    Result = TheTranslator.TranslateToLanguage(QLocale::French, TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"A2\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"A2\" \"A3\""));

    // 8. language found, string found, no arglist provided
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS5);
    QCOMPARE(Result,    QString("Zeile 2: %1 %2 %3"));
    Result = TheTranslator.Translate(TS5);
    QCOMPARE(Result,    QString("Line 2: %1 %2 %3"));

    // 9. To many and to few arguments
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS90);
    QCOMPARE(Result,    QString("Zeile 2: A1 A2 %3"));
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS91);
    QCOMPARE(Result,    QString("Zeile 2: A1 A2 A3"));
    Result = TheTranslator.Translate(TS90);
    QCOMPARE(Result,    QString("Line 2: A1 A2 %3"));
    Result = TheTranslator.Translate(TS91);
    QCOMPARE(Result,    QString("Line 2: A1 A2 A3"));
}

/****************************************************************************/
void TestTranslator::utTestTranslateTranslatableArgs() {
    // create an argument list.
    tTranslatableStringList ArgListTr = tTranslatableStringList() << "A2";
    TranslatableString TS5(5, ArgListTr);
    TranslatableString TS6(6, ArgListTr);
    tTranslatableStringList ArgList = tTranslatableStringList() << "A1" << TS5 << "A3";
    tTranslatableStringList ArgList1 = tTranslatableStringList() << "A1" << TS6 << "A3";
    TranslatableString TS2(2, ArgList);
    TranslatableString TS4(4, ArgList);
    TranslatableString TS40(4, ArgList1);
    TranslatableString TS20(2);
    TranslatableString TS90(2, tTranslatableStringList() << "A1" << TS5);
    TranslatableString TS91(2, tTranslatableStringList() << "A1" << TS5 << "A3" << "A4");
    QString Result;

    // 1. language found, string found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS2);
    QCOMPARE(Result,    QString("Zeile 2: A1 ZA2 A3"));
    Result = TheTranslator.Translate(TS2);
    QCOMPARE(Result,    QString("Line 2: A1 LA2 A3"));
    // 2. language found, EVENT_GLOBAL_UNKNOWN_STRING_ID string found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS4);
    QCOMPARE(Result,    QString("Unbekannter Text: 4. Argumente: \"A1\" \"ZA2\" \"A3\""));
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS40);
    QCOMPARE(Result,    QString("Unbekannter Text: 4. Argumente: \"A1\" \"Unbekannter Text: 6. Argumente: \"A2\"\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("Unknown text: 4. Arguments: \"A1\" \"LA2\" \"A3\""));
    Result = TheTranslator.Translate(TS40);
    QCOMPARE(Result,    QString("Unknown text: 4. Arguments: \"A1\" \"Unknown text: 6. Arguments: \"A2\"\" \"A3\""));

    // 3. language found, EVENT_GLOBAL_UNKNOWN_STRING_ID string not found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithoutUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithoutUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"ZA2\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"LA2\" \"A3\""));

    // 4. language not found, fallback language found, string found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::French, TS2);
    QCOMPARE(Result,    QString("Zeile 2: A1 ZA2 A3"));
    Result = TheTranslator.Translate(TS2);
    QCOMPARE(Result,    QString("Line 2: A1 LA2 A3"));

    // 5. language not found, fallback language found, EVENT_GLOBAL_UNKNOWN_STRING_ID string found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::French, TS4);
    QCOMPARE(Result,    QString("Unbekannter Text: 4. Argumente: \"A1\" \"ZA2\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("Unknown text: 4. Arguments: \"A1\" \"LA2\" \"A3\""));

    // 6. language not found, fallback language found, EVENT_GLOBAL_UNKNOWN_STRING_ID string not found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithoutUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithoutUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::French, TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"ZA2\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"LA2\" \"A3\""));

    // 7. language not found, fallback language not found
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithoutUnknownStringID(), false, false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithoutUnknownStringID(),  false, false);
    Result = TheTranslator.TranslateToLanguage(QLocale::French, TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"\"5\": \"A2\"\" \"A3\""));
    Result = TheTranslator.Translate(TS4);
    QCOMPARE(Result,    QString("\"4\": \"A1\" \"\"5\": \"A2\"\" \"A3\""));

    // 8. language found, string found, no arglist provided
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS20);
    QCOMPARE(Result,    QString("Zeile 2: %1 %2 %3"));
    Result = TheTranslator.Translate(TS20);
    QCOMPARE(Result,    QString("Line 2: %1 %2 %3"));

    // 9. To many and to few arguments
    TheTranslator.Reset();
    TheTranslator.SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    TheTranslator.SetLanguageData(QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  false, true);
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS90);
    QCOMPARE(Result,    QString("Zeile 2: A1 ZA2 %3"));
    Result = TheTranslator.TranslateToLanguage(QLocale::German, TS91);
    QCOMPARE(Result,    QString("Zeile 2: A1 ZA2 A3"));
    Result = TheTranslator.Translate(TS90);
    QCOMPARE(Result,    QString("Line 2: A1 LA2 %3"));
    Result = TheTranslator.Translate(TS91);
    QCOMPARE(Result,    QString("Line 2: A1 LA2 A3"));
}

/****************************************************************************/
void TestTranslator::utTestEventAndUITranslator() {
    // test if EventTranslator and UITranslator are independent of each othe
    QString Result;

    // create an argument list.
    tTranslatableStringList ArgList = tTranslatableStringList() <<"A1"<<"A2"<<"A3";
    TranslatableString TS2(2, ArgList);
    // reset instances
    EventTranslator::TranslatorInstance().Reset();
    UITranslator::TranslatorInstance().Reset();
    // feed them with different languages
    EventTranslator::TranslatorInstance().SetLanguageData(QLocale::English, CreateLanguageStringsEnglishWithUnknownStringID(), true,  false);
    UITranslator::TranslatorInstance().SetLanguageData(   QLocale::German,  CreateLanguageStringsGermanWithUnknownStringID(),  true, false);

    // use EventTranslator for translations. It should translate correct into English but not German
    Result = EventTranslator::TranslatorInstance().TranslateToLanguage(QLocale::English, TS2);
    QCOMPARE(Result,    QString("Line 2: A1 A2 A3"));
    Result = EventTranslator::TranslatorInstance().TranslateToLanguage(QLocale::German, TS2);
    QCOMPARE(Result,    QString("\"2\": \"A1\" \"A2\" \"A3\""));

    // use UITranslator for translations. It should translate correct into German but not English
    Result = UITranslator::TranslatorInstance().TranslateToLanguage(QLocale::English, TS2);
    QCOMPARE(Result,    QString("\"2\": \"A1\" \"A2\" \"A3\""));
    Result = UITranslator::TranslatorInstance().TranslateToLanguage(QLocale::German, TS2);
    QCOMPARE(Result,    QString("Zeile 2: A1 A2 A3"));
}

} // end namespace Global

QTEST_MAIN(Global::TestTranslator)

#include "TestTranslator.moc"
