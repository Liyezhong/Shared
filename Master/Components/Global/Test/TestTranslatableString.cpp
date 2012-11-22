/****************************************************************************/
/*! \file TestTranslatableString.cpp
 *
 *  \brief Implementation file for class TestTranslatableString.
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
#include <Global/Include/TranslatableString.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Test class for Translator class.
 */
/****************************************************************************/
class TestTranslatableString : public QObject {
    Q_OBJECT
private:
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
     * \brief Test of constructors.
     */
    /****************************************************************************/
    void utTestConstructors();
    /****************************************************************************/
    /**
     * \brief Test of the assignment operator.
     */
    /****************************************************************************/
    void utTestAssignmentOperator();
    /****************************************************************************/
    /**
     * \brief Test of the assignment operator.
     */
    /****************************************************************************/
    void utTestGetMethods();
}; // end class TestTranslatableString

/****************************************************************************/
void TestTranslatableString::initTestCase() {
}

/****************************************************************************/
void TestTranslatableString::init() {
}

/****************************************************************************/
void TestTranslatableString::cleanup() {
}

/****************************************************************************/
void TestTranslatableString::cleanupTestCase() {
}

/****************************************************************************/
void TestTranslatableString::utTestConstructors() {
    // test default constructor
    TranslatableString TS1;
    QCOMPARE(TS1.m_oString,                     false);
    QCOMPARE(TS1.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QVERIFY(TS1.m_String.isEmpty());
    QCOMPARE(TS1.m_ArgumentList.size(),         0);

    // test constructor with a string ID
    TranslatableString TS2(2);
    QCOMPARE(TS2.m_oString,                     false);
    QCOMPARE(TS2.m_StringID,                    quint32(2));
    QVERIFY(TS2.m_String.isEmpty());
    QCOMPARE(TS2.m_ArgumentList.size(),         0);

    // test constructor with a QString
    TranslatableString TS3(QString("3"));
    QCOMPARE(TS3.m_oString,                     true);
    QCOMPARE(TS3.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QCOMPARE(TS3.m_String,                      QString("3"));
    QCOMPARE(TS3.m_ArgumentList.size(),         0);

    // test constructor with a const char *
    TranslatableString TS4("4");
    QCOMPARE(TS4.m_oString,                     true);
    QCOMPARE(TS4.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QCOMPARE(TS4.m_String,                      QString("4"));
    QCOMPARE(TS4.m_ArgumentList.size(),         0);

    // test constructor with a string ID and argument list
    TranslatableString TS5(5, tTranslatableStringList() << "51" << "52");
    QCOMPARE(TS5.m_oString,                     false);
    QCOMPARE(TS5.m_StringID,                    quint32(5));
    QCOMPARE(TS5.m_ArgumentList.size(),         2);
    QCOMPARE(TS5.m_ArgumentList[0].GetString(), QString("51"));
    QCOMPARE(TS5.m_ArgumentList[1].GetString(), QString("52"));

    // test copy constructor
    TranslatableString TS6(TS1);
    TranslatableString TS7(TS2);
    TranslatableString TS8(TS3);
    TranslatableString TS9(TS4);
    TranslatableString TS10(TS5);

    QCOMPARE(TS1.m_oString,                     false);
    QCOMPARE(TS1.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QVERIFY(TS1.m_String.isEmpty());
    QCOMPARE(TS1.m_ArgumentList.size(),         0);
    QCOMPARE(TS2.m_oString,                     false);
    QCOMPARE(TS2.m_StringID,                    quint32(2));
    QVERIFY(TS2.m_String.isEmpty());
    QCOMPARE(TS2.m_ArgumentList.size(),         0);
    QCOMPARE(TS3.m_oString,                     true);
    QCOMPARE(TS3.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QCOMPARE(TS3.m_String,                      QString("3"));
    QCOMPARE(TS3.m_ArgumentList.size(),         0);
    QCOMPARE(TS4.m_oString,                     true);
    QCOMPARE(TS4.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QCOMPARE(TS4.m_String,                      QString("4"));
    QCOMPARE(TS4.m_ArgumentList.size(),         0);
    QCOMPARE(TS5.m_oString,                     false);
    QCOMPARE(TS5.m_StringID,                    quint32(5));
    QCOMPARE(TS5.m_ArgumentList.size(),         2);
    QCOMPARE(TS5.m_ArgumentList[0].GetString(), QString("51"));
    QCOMPARE(TS5.m_ArgumentList[1].GetString(), QString("52"));
    QCOMPARE(TS6.m_oString,                     TS1.m_oString);
    QCOMPARE(TS6.m_StringID,                    TS1.m_StringID);
    QCOMPARE(TS6.m_String,                      TS1.m_String);
    QCOMPARE(TS6.m_ArgumentList.size(),         TS1.m_ArgumentList.size());
    QCOMPARE(TS7.m_oString,                     TS2.m_oString);
    QCOMPARE(TS7.m_StringID,                    TS2.m_StringID);
    QCOMPARE(TS7.m_String,                      TS2.m_String);
    QCOMPARE(TS7.m_ArgumentList.size(),         TS2.m_ArgumentList.size());
    QCOMPARE(TS8.m_oString,                     TS3.m_oString);
    QCOMPARE(TS8.m_StringID,                    TS3.m_StringID);
    QCOMPARE(TS8.m_String,                      TS3.m_String);
    QCOMPARE(TS8.m_ArgumentList.size(),         TS3.m_ArgumentList.size());
    QCOMPARE(TS9.m_oString,                     TS4.m_oString);
    QCOMPARE(TS9.m_StringID,                    TS4.m_StringID);
    QCOMPARE(TS9.m_String,                      TS4.m_String);
    QCOMPARE(TS9.m_ArgumentList.size(),         TS4.m_ArgumentList.size());
    QCOMPARE(TS10.m_oString,                    TS5.m_oString);
    QCOMPARE(TS10.m_StringID,                   TS5.m_StringID);
    QCOMPARE(TS10.m_String,                     TS5.m_String);
    QCOMPARE(TS10.m_ArgumentList.size(),        TS5.m_ArgumentList.size());
}

/****************************************************************************/
void TestTranslatableString::utTestAssignmentOperator() {

    TranslatableString TS1;
    QCOMPARE(TS1.m_oString,                     false);
    QCOMPARE(TS1.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QVERIFY(TS1.m_String.isEmpty());
    QCOMPARE(TS1.m_ArgumentList.size(),         0);

    TranslatableString TS2(2);
    QCOMPARE(TS2.m_oString,                     false);
    QCOMPARE(TS2.m_StringID,                    quint32(2));
    QVERIFY(TS2.m_String.isEmpty());
    QCOMPARE(TS2.m_ArgumentList.size(),         0);

    TranslatableString TS3(QString("3"));
    QCOMPARE(TS3.m_oString,                     true);
    QCOMPARE(TS3.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QCOMPARE(TS3.m_String,                      QString("3"));
    QCOMPARE(TS3.m_ArgumentList.size(),         0);

    TranslatableString TS4("4");
    QCOMPARE(TS4.m_oString,                     true);
    QCOMPARE(TS4.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QCOMPARE(TS4.m_String,                      QString("4"));
    QCOMPARE(TS4.m_ArgumentList.size(),         0);

    TranslatableString TS5(5, tTranslatableStringList() << "51" << "52");
    QCOMPARE(TS5.m_oString,                     false);
    QCOMPARE(TS5.m_StringID,                    quint32(5));
    QCOMPARE(TS5.m_ArgumentList.size(),         2);
    QCOMPARE(TS5.m_ArgumentList[0].GetString(), QString("51"));
    QCOMPARE(TS5.m_ArgumentList[1].GetString(), QString("52"));

    // test assignment operator
    TranslatableString TS6;
    TranslatableString TS7;
    TranslatableString TS8;
    TranslatableString TS9;
    TranslatableString TS10;
    TS6 = TS1;
    TS7 = TS2;
    TS8 = TS3;
    TS9 = TS4;
    TS10 = TS5;

    QCOMPARE(TS1.m_oString,                     false);
    QCOMPARE(TS1.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QVERIFY(TS1.m_String.isEmpty());
    QCOMPARE(TS1.m_ArgumentList.size(),         0);
    QCOMPARE(TS2.m_oString,                     false);
    QCOMPARE(TS2.m_StringID,                    quint32(2));
    QVERIFY(TS2.m_String.isEmpty());
    QCOMPARE(TS2.m_ArgumentList.size(),         0);
    QCOMPARE(TS3.m_oString,                     true);
    QCOMPARE(TS3.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QCOMPARE(TS3.m_String,                      QString("3"));
    QCOMPARE(TS3.m_ArgumentList.size(),         0);
    QCOMPARE(TS4.m_oString,                     true);
    QCOMPARE(TS4.m_StringID,                    EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QCOMPARE(TS4.m_String,                      QString("4"));
    QCOMPARE(TS4.m_ArgumentList.size(),         0);
    QCOMPARE(TS5.m_oString,                     false);
    QCOMPARE(TS5.m_StringID,                    quint32(5));
    QCOMPARE(TS5.m_ArgumentList.size(),         2);
    QCOMPARE(TS5.m_ArgumentList[0].GetString(), QString("51"));
    QCOMPARE(TS5.m_ArgumentList[1].GetString(), QString("52"));
    QCOMPARE(TS6.m_oString,                     TS1.m_oString);
    QCOMPARE(TS6.m_StringID,                    TS1.m_StringID);
    QCOMPARE(TS6.m_String,                      TS1.m_String);
    QCOMPARE(TS6.m_ArgumentList.size(),         TS1.m_ArgumentList.size());
    QCOMPARE(TS7.m_oString,                     TS2.m_oString);
    QCOMPARE(TS7.m_StringID,                    TS2.m_StringID);
    QCOMPARE(TS7.m_String,                      TS2.m_String);
    QCOMPARE(TS7.m_ArgumentList.size(),         TS2.m_ArgumentList.size());
    QCOMPARE(TS8.m_oString,                     TS3.m_oString);
    QCOMPARE(TS8.m_StringID,                    TS3.m_StringID);
    QCOMPARE(TS8.m_String,                      TS3.m_String);
    QCOMPARE(TS8.m_ArgumentList.size(),         TS3.m_ArgumentList.size());
    QCOMPARE(TS9.m_oString,                     TS4.m_oString);
    QCOMPARE(TS9.m_StringID,                    TS4.m_StringID);
    QCOMPARE(TS9.m_String,                      TS4.m_String);
    QCOMPARE(TS9.m_ArgumentList.size(),         TS4.m_ArgumentList.size());
    QCOMPARE(TS10.m_oString,                    TS5.m_oString);
    QCOMPARE(TS10.m_StringID,                   TS5.m_StringID);
    QCOMPARE(TS10.m_String,                     TS5.m_String);
    QCOMPARE(TS10.m_ArgumentList.size(),        TS5.m_ArgumentList.size());

    // assign to self
    TS6 = TS6;
    QCOMPARE(TS6.m_oString,                     TS1.m_oString);
    QCOMPARE(TS6.m_StringID,                    TS1.m_StringID);
    QCOMPARE(TS6.m_String,                      TS1.m_String);
    QCOMPARE(TS6.m_ArgumentList.size(),         TS1.m_ArgumentList.size());
}

/****************************************************************************/
void TestTranslatableString::utTestGetMethods() {
    TranslatableString TS1(1, tTranslatableStringList() << "11" << "12");
    QCOMPARE(TS1.m_oString,                         false);
    QCOMPARE(TS1.IsString(),                        TS1.m_oString);
    QCOMPARE(TS1.m_StringID,                        quint32(1));
    QCOMPARE(TS1.GetStringID(),                     TS1.m_StringID);
    QVERIFY(TS1.m_String.isEmpty());
    QCOMPARE(TS1.GetString(),                       TS1.m_String);
    QCOMPARE(TS1.m_ArgumentList.size(),             2);
    QCOMPARE(TS1.GetArgumentList().size(),          TS1.m_ArgumentList.size());
    QCOMPARE(TS1.GetArgumentList()[0].GetString(),  QString("11"));
    QCOMPARE(TS1.GetArgumentList()[1].GetString(),  QString("12"));

    TranslatableString TS2(QString("2"));
    QCOMPARE(TS2.m_oString,                         true);
    QCOMPARE(TS2.IsString(),                        TS2.m_oString);
    QCOMPARE(TS2.m_StringID,                        EVENT_GLOBAL_UNKNOWN_STRING_ID);
    QCOMPARE(TS2.GetStringID(),                     TS2.m_StringID);
    QCOMPARE(TS2.m_String,                          QString("2"));
    QCOMPARE(TS2.GetString(),                       TS2.m_String);
    QCOMPARE(TS2.m_ArgumentList.size(),             0);
    QCOMPARE(TS2.GetArgumentList().size(),          TS2.m_ArgumentList.size());
}

} // end namespace Global

QTEST_MAIN(Global::TestTranslatableString)

#include "TestTranslatableString.moc"
