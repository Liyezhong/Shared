/****************************************************************************/
/*! \file TestUtils.cpp
 *
 *  \brief Implementation file for utilities.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-01-11
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
#include <Global/Include/Utils.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/MonotonicTime.h>

#include <QDebug>

/****************************************************************************/
/**
 * \brief Test enum.
 */
/****************************************************************************/
enum TestEnum1 {
    TestEnum1_1 = 1,    ///< Value 1
    TestEnum1_2 = 2,    ///< value 2
    TestEnum1_3 = 3     ///< value 3
};

/****************************************************************************/
/**
 * \brief Test enum.
 */
/****************************************************************************/
enum TestEnum2 {
    TestEnum2_1 = 0x70000001,   ///< Value 1
    TestEnum2_2 = 0x70000002,   ///< Value 2
    TestEnum2_3 = 0x70000003    ///< Value 3
};

/****************************************************************************/
/**
 * \brief Test enum.
 */
/****************************************************************************/
enum TestEnum3 {
    TestEnum3_1 = 0xF0000001,   ///< Value 1
    TestEnum3_2 = 0xF0000002,   ///< Value 2
    TestEnum3_3 = 0xF0000003    ///< Value 3
};

namespace Global {

/****************************************************************************/
/**
 * \brief Test class for utilities.
 */
/****************************************************************************/
class TestUtils : public QObject {
    Q_OBJECT
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
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
     * \brief Test LanguageToString.
     */
    /****************************************************************************/
    void utLanguageToString();
    /****************************************************************************/
    /**
     * \brief Test StringToLanguage.
     */
    /****************************************************************************/
    void utStringToLanguage();
    /****************************************************************************/
    /**
     * \brief Test combination of LanguageToString and StringToLanguage.
     */
    /****************************************************************************/
    void utLanguageToStringToLanguage();
    /****************************************************************************/
    /**
     * \brief Test LanguageToLanguageCode.
     */
    /****************************************************************************/
    void utLanguageToLanguageCode();
    /****************************************************************************/
    /**
     * \brief Test GUIMessageTypeToString.
     */
    /****************************************************************************/
    void utGUIMessageTypeToString();
    /****************************************************************************/
    /**
     * \brief Test StringToGUIMessageType.
     */
    /****************************************************************************/
    void utStringToGUIMessageType();
    /****************************************************************************/
    /**
     * \brief Test TemperatureFormatToString.
     */
    /****************************************************************************/
    void utTemperatureFormatToString();
    /****************************************************************************/
    /**
     * \brief Test StringToTemperatureFormat.
     */
    /****************************************************************************/
    void utStringToTemperatureFormat();
    /****************************************************************************/
    /**
     * \brief Test combination of TemperatureFormatToString and StringToTemperatureFormat.
     */
    /****************************************************************************/
    void utTemperatureFormatToStringToTemperatureFormat();
    /****************************************************************************/
    /**
     * \brief Test OnOffStateToString.
     */
    /****************************************************************************/
    void utOnOffStateToString();
    /****************************************************************************/
    /**
     * \brief Test StringToOnOffState.
     */
    /****************************************************************************/
    void utStringToOnOffState();
    /****************************************************************************/
    /**
     * \brief Test combination of OnOffStateToString and StringToOnOffState.
     */
    /****************************************************************************/
    void utOnOffStateToStringToOnOffState();
    /****************************************************************************/
    /**
     * \brief Test DateFormatToString.
     */
    /****************************************************************************/
    void utDateFormatToString();
    /****************************************************************************/
    /**
     * \brief Test StringToDateFormat.
     */
    /****************************************************************************/
    void utStringToDateFormat();
    /****************************************************************************/
    /**
     * \brief Test combination of DateFormatToString and StringToDateFormat.
     */
    /****************************************************************************/
    void utDateFormatToStringToDateFormat();
    /****************************************************************************/
    /**
     * \brief Test TimeFormatToString.
     */
    /****************************************************************************/
    void utTimeFormatToString();
    /****************************************************************************/
    /**
     * \brief Test StringToTimeFormat.
     */
    /****************************************************************************/
    void utStringToTimeFormat();
    /****************************************************************************/
    /**
     * \brief Test combination of TimeFormatToString and StringToTimeFormat.
     */
    /****************************************************************************/
    void utTimeFormatToStringToTimeFormat();
    /****************************************************************************/
    /**
     * \brief Test OvenStartModeToString.
     */
    /****************************************************************************/
    void utOvenStartModeToString();
    /****************************************************************************/
    /**
     * \brief Test StringToOvenStartMode.
     */
    /****************************************************************************/
    void utStringToOvenStartMode();
    /****************************************************************************/
    /**
     * \brief Test combination of OvenStartModeToString and StringToOvenStartMode.
     */
    /****************************************************************************/
    void utOvenStartModeToStringToOvenStartMode();
    /****************************************************************************/
    /**
     * \brief Test AsInt.
     */
    /****************************************************************************/
    void utAsInt();
    /****************************************************************************/
    /**
     * \brief Test Asquint32.
     */
    /****************************************************************************/
    void utAsquint32();
}; // end class TestUtils

/****************************************************************************/
void TestUtils::initTestCase() {
    Global::InitSupportedLanguages();
}

/****************************************************************************/
void TestUtils::init() {
}

/****************************************************************************/
void TestUtils::cleanup() {
}

/****************************************************************************/
void TestUtils::cleanupTestCase() {
}

/****************************************************************************/
void TestUtils::utLanguageToString() {
    QCOMPARE(Global::LanguageToString(QLocale::C),          QString("C"));
    QCOMPARE(Global::LanguageToString(QLocale::German),     QString("German"));
    QCOMPARE(Global::LanguageToString(QLocale::English),    QString("English"));
    QCOMPARE(Global::LanguageToString(QLocale::French),     QString("French"));
    QCOMPARE(Global::LanguageToString(QLocale::Italian),    QString("Italian"));
    QCOMPARE(Global::LanguageToString(QLocale::Spanish),    QString("Spanish"));
    QCOMPARE(Global::LanguageToString(QLocale::Portuguese), QString("Portuguese"));
    QCOMPARE(Global::LanguageToString(QLocale::Dutch),      QString("Dutch"));
    QCOMPARE(Global::LanguageToString(QLocale::Swedish),    QString("Swedish"));
    QCOMPARE(Global::LanguageToString(QLocale::Norwegian),  QString("Norwegian"));
    QCOMPARE(Global::LanguageToString(QLocale::Danish),     QString("Danish"));
    QCOMPARE(Global::LanguageToString(QLocale::Polish),     QString("Polish"));
    QCOMPARE(Global::LanguageToString(QLocale::Czech),      QString("Czech"));
    QCOMPARE(Global::LanguageToString(QLocale::Russian),    QString("Russian"));
}

/****************************************************************************/
void TestUtils::utStringToLanguage() {
    QCOMPARE(StringToLanguage("c"),             QLocale::C);
    QCOMPARE(StringToLanguage("C"),             QLocale::C);
    QCOMPARE(StringToLanguage("German"),        QLocale::German);
    QCOMPARE(StringToLanguage("german"),        QLocale::German);
    QCOMPARE(StringToLanguage("GERMAN"),        QLocale::German);
    QCOMPARE(StringToLanguage("English"),       QLocale::English);
    QCOMPARE(StringToLanguage("english"),       QLocale::English);
    QCOMPARE(StringToLanguage("ENGLISH"),       QLocale::English);
    QCOMPARE(StringToLanguage("French"),        QLocale::French);
    QCOMPARE(StringToLanguage("french"),        QLocale::French);
    QCOMPARE(StringToLanguage("FRENCH"),        QLocale::French);
    QCOMPARE(StringToLanguage("Italian"),       QLocale::Italian);
    QCOMPARE(StringToLanguage("italian"),       QLocale::Italian);
    QCOMPARE(StringToLanguage("ITALIAN"),       QLocale::Italian);
    QCOMPARE(StringToLanguage("Spanish"),       QLocale::Spanish);
    QCOMPARE(StringToLanguage("spanish"),       QLocale::Spanish);
    QCOMPARE(StringToLanguage("SPANISH"),       QLocale::Spanish);
    QCOMPARE(StringToLanguage("Portuguese"),    QLocale::Portuguese);
    QCOMPARE(StringToLanguage("portuguese"),    QLocale::Portuguese);
    QCOMPARE(StringToLanguage("PORTUGUESE"),    QLocale::Portuguese);
    QCOMPARE(StringToLanguage("Dutch"),         QLocale::Dutch);
    QCOMPARE(StringToLanguage("dutch"),         QLocale::Dutch);
    QCOMPARE(StringToLanguage("DUTCH"),         QLocale::Dutch);
    QCOMPARE(StringToLanguage("Swedish"),       QLocale::Swedish);
    QCOMPARE(StringToLanguage("swedish"),       QLocale::Swedish);
    QCOMPARE(StringToLanguage("SWEDISH"),       QLocale::Swedish);
    QCOMPARE(StringToLanguage("Norwegian"),     QLocale::Norwegian);
    QCOMPARE(StringToLanguage("norwegian"),     QLocale::Norwegian);
    QCOMPARE(StringToLanguage("NORWEGIAN"),     QLocale::Norwegian);
    QCOMPARE(StringToLanguage("Danish"),        QLocale::Danish);
    QCOMPARE(StringToLanguage("danish"),        QLocale::Danish);
    QCOMPARE(StringToLanguage("DANISH"),        QLocale::Danish);
    QCOMPARE(StringToLanguage("Polish"),        QLocale::Polish);
    QCOMPARE(StringToLanguage("polish"),        QLocale::Polish);
    QCOMPARE(StringToLanguage("POLISH"),        QLocale::Polish);
    QCOMPARE(StringToLanguage("Czech"),         QLocale::Czech);
    QCOMPARE(StringToLanguage("czech"),         QLocale::Czech);
    QCOMPARE(StringToLanguage("CZECH"),         QLocale::Czech);
    QCOMPARE(StringToLanguage("Russian"),       QLocale::Russian);
    QCOMPARE(StringToLanguage("russian"),       QLocale::Russian);
    QCOMPARE(StringToLanguage("RUSSIAN"),       QLocale::Russian);
    QCOMPARE(StringToLanguage("gnagna"),        QLocale::C);
}

/****************************************************************************/
void TestUtils::utLanguageToStringToLanguage() {
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::C)),            QLocale::C);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::German)),       QLocale::German);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::English)),      QLocale::English);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::French)),       QLocale::French);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Italian)),      QLocale::Italian);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Spanish)),      QLocale::Spanish);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Portuguese)),   QLocale::Portuguese);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Dutch)),        QLocale::Dutch);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Swedish)),      QLocale::Swedish);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Norwegian)),    QLocale::Norwegian);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Danish)),       QLocale::Danish);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Polish)),       QLocale::Polish);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Czech)),        QLocale::Czech);
    QCOMPARE(StringToLanguage(Global::LanguageToString(QLocale::Russian)),      QLocale::Russian);
}

/****************************************************************************/
void TestUtils::utLanguageToLanguageCode() {
    QCOMPARE(LanguageToLanguageCode(QLocale::German),   QString("de"));
    QCOMPARE(LanguageToLanguageCode(QLocale::English),  QString("en"));
    QCOMPARE(LanguageToLanguageCode(QLocale::Greek),    QString("el"));
}

/****************************************************************************/
void TestUtils::utGUIMessageTypeToString() {
    QCOMPARE(GUIMessageTypeToString(GUIMSGTYPE_UNDEFINED),  QString("Undefined"));
    QCOMPARE(GUIMessageTypeToString(GUIMSGTYPE_ERROR),      QString("Error"));
    QCOMPARE(GUIMessageTypeToString(GUIMSGTYPE_WARNING),    QString("Warning"));
    QCOMPARE(GUIMessageTypeToString(GUIMSGTYPE_INFO),       QString("Info"));
}

/****************************************************************************/
void TestUtils::utStringToGUIMessageType() {
    // case sensitive
    QCOMPARE(StringToGUIMessageType("Undefined", true),     GUIMSGTYPE_UNDEFINED);
    QCOMPARE(StringToGUIMessageType("undefined", true),     GUIMSGTYPE_UNDEFINED);
    QCOMPARE(StringToGUIMessageType("Error", true),         GUIMSGTYPE_ERROR);
    QCOMPARE(StringToGUIMessageType("error", true),         GUIMSGTYPE_UNDEFINED);
    QCOMPARE(StringToGUIMessageType("Warning", true),       GUIMSGTYPE_WARNING);
    QCOMPARE(StringToGUIMessageType("warning", true),       GUIMSGTYPE_UNDEFINED);
    QCOMPARE(StringToGUIMessageType("Info", true),          GUIMSGTYPE_INFO);
    QCOMPARE(StringToGUIMessageType("info", true),          GUIMSGTYPE_UNDEFINED);
    QCOMPARE(StringToGUIMessageType("gnagna", true),        GUIMSGTYPE_UNDEFINED);

    // case insensitive
    QCOMPARE(StringToGUIMessageType("Undefined", false),    GUIMSGTYPE_UNDEFINED);
    QCOMPARE(StringToGUIMessageType("undefined", false),    GUIMSGTYPE_UNDEFINED);
    QCOMPARE(StringToGUIMessageType("Error", false),        GUIMSGTYPE_ERROR);
    QCOMPARE(StringToGUIMessageType("error", false),        GUIMSGTYPE_ERROR);
    QCOMPARE(StringToGUIMessageType("Warning", false),      GUIMSGTYPE_WARNING);
    QCOMPARE(StringToGUIMessageType("warning", false),      GUIMSGTYPE_WARNING);
    QCOMPARE(StringToGUIMessageType("Info", false),         GUIMSGTYPE_INFO);
    QCOMPARE(StringToGUIMessageType("info", false),         GUIMSGTYPE_INFO);
    QCOMPARE(StringToGUIMessageType("gnagna", false),       GUIMSGTYPE_UNDEFINED);
}
/****************************************************************************/
void TestUtils::utTemperatureFormatToString() {
    QCOMPARE(TemperatureFormatToString(TEMP_FORMAT_UNDEFINED),      QString("Undefined"));
    QCOMPARE(TemperatureFormatToString(TEMP_FORMAT_CELSIUS),        QString("C"));
    QCOMPARE(TemperatureFormatToString(TEMP_FORMAT_FAHRENHEIT),     QString("F"));
}

/****************************************************************************/
void TestUtils::utStringToTemperatureFormat() {
    // case sensitive
    QCOMPARE(StringToTemperatureFormat("C", true),      TEMP_FORMAT_CELSIUS);
    QCOMPARE(StringToTemperatureFormat("c", true),      TEMP_FORMAT_UNDEFINED);
    QCOMPARE(StringToTemperatureFormat("F", true),      TEMP_FORMAT_FAHRENHEIT);
    QCOMPARE(StringToTemperatureFormat("f", true),      TEMP_FORMAT_UNDEFINED);
    QCOMPARE(StringToTemperatureFormat("gnagna", true), TEMP_FORMAT_UNDEFINED);

    // case insensitive
    QCOMPARE(StringToTemperatureFormat("C", false),         TEMP_FORMAT_CELSIUS);
    QCOMPARE(StringToTemperatureFormat("c", false),         TEMP_FORMAT_CELSIUS);
    QCOMPARE(StringToTemperatureFormat("F", false),         TEMP_FORMAT_FAHRENHEIT);
    QCOMPARE(StringToTemperatureFormat("f", false),         TEMP_FORMAT_FAHRENHEIT);
    QCOMPARE(StringToTemperatureFormat("gnagna", false),    TEMP_FORMAT_UNDEFINED);
}

/****************************************************************************/
void TestUtils::utTemperatureFormatToStringToTemperatureFormat() {
    QCOMPARE(StringToTemperatureFormat(TemperatureFormatToString(TEMP_FORMAT_UNDEFINED), true),     TEMP_FORMAT_UNDEFINED);
    QCOMPARE(StringToTemperatureFormat(TemperatureFormatToString(TEMP_FORMAT_UNDEFINED), false),    TEMP_FORMAT_UNDEFINED);
    QCOMPARE(StringToTemperatureFormat(TemperatureFormatToString(TEMP_FORMAT_CELSIUS), true),       TEMP_FORMAT_CELSIUS);
    QCOMPARE(StringToTemperatureFormat(TemperatureFormatToString(TEMP_FORMAT_CELSIUS), false),      TEMP_FORMAT_CELSIUS);
    QCOMPARE(StringToTemperatureFormat(TemperatureFormatToString(TEMP_FORMAT_FAHRENHEIT), true),    TEMP_FORMAT_FAHRENHEIT);
    QCOMPARE(StringToTemperatureFormat(TemperatureFormatToString(TEMP_FORMAT_FAHRENHEIT), false),   TEMP_FORMAT_FAHRENHEIT);
}

/****************************************************************************/
void TestUtils::utOnOffStateToString() {
    QCOMPARE(OnOffStateToString(ONOFFSTATE_UNDEFINED),  QString("Undefined"));
    QCOMPARE(OnOffStateToString(ONOFFSTATE_ON),         QString("On"));
    QCOMPARE(OnOffStateToString(ONOFFSTATE_OFF),        QString("Off"));
}

/****************************************************************************/
void TestUtils::utStringToOnOffState() {
    // case sensitive
    QCOMPARE(StringToOnOffState("On", true),        ONOFFSTATE_ON);
    QCOMPARE(StringToOnOffState("on", true),        ONOFFSTATE_UNDEFINED);
    QCOMPARE(StringToOnOffState("ON", true),        ONOFFSTATE_UNDEFINED);
    QCOMPARE(StringToOnOffState("Off", true),       ONOFFSTATE_OFF);
    QCOMPARE(StringToOnOffState("off", true),       ONOFFSTATE_UNDEFINED);
    QCOMPARE(StringToOnOffState("OFF", true),       ONOFFSTATE_UNDEFINED);
    QCOMPARE(StringToOnOffState("gnagna", true),    ONOFFSTATE_UNDEFINED);

    // case insensitive
    QCOMPARE(StringToOnOffState("On", false),       ONOFFSTATE_ON);
    QCOMPARE(StringToOnOffState("on", false),       ONOFFSTATE_ON);
    QCOMPARE(StringToOnOffState("ON", false),       ONOFFSTATE_ON);
    QCOMPARE(StringToOnOffState("Off", false),      ONOFFSTATE_OFF);
    QCOMPARE(StringToOnOffState("off", false),      ONOFFSTATE_OFF);
    QCOMPARE(StringToOnOffState("OFF", false),      ONOFFSTATE_OFF);
    QCOMPARE(StringToOnOffState("gnagna", false),   ONOFFSTATE_UNDEFINED);
}

/****************************************************************************/
void TestUtils::utOnOffStateToStringToOnOffState() {
    QCOMPARE(StringToOnOffState(OnOffStateToString(ONOFFSTATE_UNDEFINED), true), ONOFFSTATE_UNDEFINED);
    QCOMPARE(StringToOnOffState(OnOffStateToString(ONOFFSTATE_UNDEFINED), false), ONOFFSTATE_UNDEFINED);
    QCOMPARE(StringToOnOffState(OnOffStateToString(ONOFFSTATE_ON), true), ONOFFSTATE_ON);
    QCOMPARE(StringToOnOffState(OnOffStateToString(ONOFFSTATE_ON), false), ONOFFSTATE_ON);
    QCOMPARE(StringToOnOffState(OnOffStateToString(ONOFFSTATE_OFF), true), ONOFFSTATE_OFF);
    QCOMPARE(StringToOnOffState(OnOffStateToString(ONOFFSTATE_OFF), false), ONOFFSTATE_OFF);
}

/****************************************************************************/
void TestUtils::utDateFormatToString() {
    QCOMPARE(DateFormatToString(DATE_UNDEFINED),        QString("Undefined"));
    QCOMPARE(DateFormatToString(DATE_INTERNATIONAL),    QString("International"));
    QCOMPARE(DateFormatToString(DATE_ISO),              QString("ISO"));
    QCOMPARE(DateFormatToString(DATE_US),               QString("US"));
}

/****************************************************************************/
void TestUtils::utStringToDateFormat() {
    // case sensitive
    QCOMPARE(StringToDateFormat("International", true), DATE_INTERNATIONAL);
    QCOMPARE(StringToDateFormat("international", true), DATE_UNDEFINED);
    QCOMPARE(StringToDateFormat("INTERNATIONAL", true), DATE_UNDEFINED);
    QCOMPARE(StringToDateFormat("ISO", true),           DATE_ISO);
    QCOMPARE(StringToDateFormat("iso", true),           DATE_UNDEFINED);
    QCOMPARE(StringToDateFormat("Iso", true),           DATE_UNDEFINED);
    QCOMPARE(StringToDateFormat("US", true),            DATE_US);
    QCOMPARE(StringToDateFormat("us", true),            DATE_UNDEFINED);
    QCOMPARE(StringToDateFormat("Us", true),            DATE_UNDEFINED);
    QCOMPARE(StringToDateFormat("gnagna", true),        DATE_UNDEFINED);

    // case insensitive
    QCOMPARE(StringToDateFormat("International", false),    DATE_INTERNATIONAL);
    QCOMPARE(StringToDateFormat("international", false),    DATE_INTERNATIONAL);
    QCOMPARE(StringToDateFormat("INTERNATIONAL", false),    DATE_INTERNATIONAL);
    QCOMPARE(StringToDateFormat("ISO", false),              DATE_ISO);
    QCOMPARE(StringToDateFormat("iso", false),              DATE_ISO);
    QCOMPARE(StringToDateFormat("Iso", false),              DATE_ISO);
    QCOMPARE(StringToDateFormat("US", false),               DATE_US);
    QCOMPARE(StringToDateFormat("us", false),               DATE_US);
    QCOMPARE(StringToDateFormat("Us", false),               DATE_US);
    QCOMPARE(StringToDateFormat("gnagna", false),           DATE_UNDEFINED);
}

/****************************************************************************/
void TestUtils::utDateFormatToStringToDateFormat() {
    QCOMPARE(StringToDateFormat(DateFormatToString(DATE_UNDEFINED), true),      DATE_UNDEFINED);
    QCOMPARE(StringToDateFormat(DateFormatToString(DATE_UNDEFINED), false),     DATE_UNDEFINED);
    QCOMPARE(StringToDateFormat(DateFormatToString(DATE_INTERNATIONAL), true),  DATE_INTERNATIONAL);
    QCOMPARE(StringToDateFormat(DateFormatToString(DATE_INTERNATIONAL), false), DATE_INTERNATIONAL);
    QCOMPARE(StringToDateFormat(DateFormatToString(DATE_ISO), true),            DATE_ISO);
    QCOMPARE(StringToDateFormat(DateFormatToString(DATE_ISO), false),           DATE_ISO);
    QCOMPARE(StringToDateFormat(DateFormatToString(DATE_US), true),             DATE_US);
    QCOMPARE(StringToDateFormat(DateFormatToString(DATE_US), false),            DATE_US);
}

/****************************************************************************/
void TestUtils::utTimeFormatToString() {
    QCOMPARE(TimeFormatToString(TIME_UNDEFINED),    QString("Undefined"));
    QCOMPARE(TimeFormatToString(TIME_24),           QString("24"));
    QCOMPARE(TimeFormatToString(TIME_12),           QString("12"));
}

/****************************************************************************/
void TestUtils::utStringToTimeFormat() {
    QCOMPARE(StringToTimeFormat("24", true),        TIME_24);
    QCOMPARE(StringToTimeFormat("12", true),        TIME_12);
    QCOMPARE(StringToTimeFormat("gnagna", true),    TIME_UNDEFINED);
}

/****************************************************************************/
void TestUtils::utTimeFormatToStringToTimeFormat() {
    QCOMPARE(StringToTimeFormat(TimeFormatToString(TIME_UNDEFINED), true),  TIME_UNDEFINED);
    QCOMPARE(StringToTimeFormat(TimeFormatToString(TIME_UNDEFINED), false), TIME_UNDEFINED);
    QCOMPARE(StringToTimeFormat(TimeFormatToString(TIME_24), true),         TIME_24);
    QCOMPARE(StringToTimeFormat(TimeFormatToString(TIME_24), false),        TIME_24);
    QCOMPARE(StringToTimeFormat(TimeFormatToString(TIME_12), true),         TIME_12);
    QCOMPARE(StringToTimeFormat(TimeFormatToString(TIME_12), false),        TIME_12);
}

/****************************************************************************/
void TestUtils::utOvenStartModeToString() {
    QCOMPARE(OvenStartModeToString(OVENSTART_UNDEFINED),        QString("Undefined"));
    QCOMPARE(OvenStartModeToString(OVENSTART_AFTER_STARTUP),    QString("AfterStartup"));
    QCOMPARE(OvenStartModeToString(OVENSTART_BEFORE_PROGRAM),   QString("BeforeProgram"));
}

/****************************************************************************/
void TestUtils::utStringToOvenStartMode() {
    // case sensitive
    QCOMPARE(StringToOvenStartMode("AfterStartup", true),   OVENSTART_AFTER_STARTUP);
    QCOMPARE(StringToOvenStartMode("afterstartup", true),   OVENSTART_UNDEFINED);
    QCOMPARE(StringToOvenStartMode("AFTERSTARTUP", true),   OVENSTART_UNDEFINED);
    QCOMPARE(StringToOvenStartMode("BeforeProgram", true),  OVENSTART_BEFORE_PROGRAM);
    QCOMPARE(StringToOvenStartMode("beforeprogram", true),  OVENSTART_UNDEFINED);
    QCOMPARE(StringToOvenStartMode("BEFOREPROGRAM", true),  OVENSTART_UNDEFINED);
    QCOMPARE(StringToOvenStartMode("gnagna", true),         OVENSTART_UNDEFINED);

    // case insensitive
    QCOMPARE(StringToOvenStartMode("AfterStartup", false),  OVENSTART_AFTER_STARTUP);
    QCOMPARE(StringToOvenStartMode("afterstartup", false),  OVENSTART_AFTER_STARTUP);
    QCOMPARE(StringToOvenStartMode("AFTERSTARTUP", false),  OVENSTART_AFTER_STARTUP);
    QCOMPARE(StringToOvenStartMode("BeforeProgram", false), OVENSTART_BEFORE_PROGRAM);
    QCOMPARE(StringToOvenStartMode("beforeprogram", false), OVENSTART_BEFORE_PROGRAM);
    QCOMPARE(StringToOvenStartMode("BEFOREPROGRAM", false), OVENSTART_BEFORE_PROGRAM);
    QCOMPARE(StringToOvenStartMode("gnagna", true),         OVENSTART_UNDEFINED);
}

/****************************************************************************/
void TestUtils::utOvenStartModeToStringToOvenStartMode() {
    QCOMPARE(StringToOvenStartMode(OvenStartModeToString(OVENSTART_UNDEFINED), true),       OVENSTART_UNDEFINED);
    QCOMPARE(StringToOvenStartMode(OvenStartModeToString(OVENSTART_UNDEFINED), false),      OVENSTART_UNDEFINED);
    QCOMPARE(StringToOvenStartMode(OvenStartModeToString(OVENSTART_AFTER_STARTUP), true),   OVENSTART_AFTER_STARTUP);
    QCOMPARE(StringToOvenStartMode(OvenStartModeToString(OVENSTART_AFTER_STARTUP), false),  OVENSTART_AFTER_STARTUP);
    QCOMPARE(StringToOvenStartMode(OvenStartModeToString(OVENSTART_BEFORE_PROGRAM), true),  OVENSTART_BEFORE_PROGRAM);
    QCOMPARE(StringToOvenStartMode(OvenStartModeToString(OVENSTART_BEFORE_PROGRAM), false), OVENSTART_BEFORE_PROGRAM);
}

/****************************************************************************/
void TestUtils::utAsInt() {
    QCOMPARE(AsInt<TestEnum1>(TestEnum1_1),     1);
    QCOMPARE(AsInt<TestEnum1>(TestEnum1_2),     2);
    QCOMPARE(AsInt<TestEnum1>(TestEnum1_3),     3);
    QCOMPARE(AsInt<TestEnum2>(TestEnum2_1),     0x70000001);
    QCOMPARE(AsInt<TestEnum2>(TestEnum2_2),     0x70000002);
    QCOMPARE(AsInt<TestEnum2>(TestEnum2_3),     0x70000003);
}

/****************************************************************************/
void TestUtils::utAsquint32() {
    QCOMPARE(Asquint32<TestEnum1>(TestEnum1_1),     quint32(1));
    QCOMPARE(Asquint32<TestEnum1>(TestEnum1_2),     quint32(2));
    QCOMPARE(Asquint32<TestEnum1>(TestEnum1_3),     quint32(3));
    QCOMPARE(Asquint32<TestEnum3>(TestEnum3_1),     0xF0000001);
    QCOMPARE(Asquint32<TestEnum3>(TestEnum3_2),     0xF0000002);
    QCOMPARE(Asquint32<TestEnum3>(TestEnum3_3),     0xF0000003);
}

} // end namespace Global

QTEST_MAIN(Global::TestUtils)

#include "TestUtils.moc"
