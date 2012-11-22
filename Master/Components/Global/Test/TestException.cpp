/****************************************************************************/
/*! \file TestException.cpp
 *
 *  \brief Implementation file for class TestException.
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
#include <Global/Include/Exception.h>

namespace Global {

static const int ExceptionSleepTime = 10;   ///< Sleep time to ensure different time stamps[ms].

/****************************************************************************/
/**
 * \brief Test class for Exception class.
 */
/****************************************************************************/
class TestException : public QObject {
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
     * \brief Test constructors.
     */
    /****************************************************************************/
    void utConstructors();
    /****************************************************************************/
    /**
     * \brief Test assignment operator.
     */
    /****************************************************************************/
    void utAssignmentOperator();
    /****************************************************************************/
    /**
     * \brief Test Get methods.
     */
    /****************************************************************************/
    void utGetMethods();
}; // end class TestException

/****************************************************************************/
void TestException::initTestCase() {
}

/****************************************************************************/
void TestException::init() {
}

/****************************************************************************/
void TestException::cleanup() {
}

/****************************************************************************/
void TestException::cleanupTestCase() {
}

/****************************************************************************/
void TestException::utConstructors() {
    // test constructors

    // Exception(const QString & File, int Line, const QDateTime & TimeStamp, const QString & ErrorCode)
    // create
    QTest::qSleep(ExceptionSleepTime);
    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Exception Ex2("File2", 2, TimeStamp2, 22);
    // test
    QCOMPARE(Ex2.m_File,                    QString("File2"));
    QCOMPARE(Ex2.m_Line,                    2);
    QCOMPARE(Ex2.m_TimeStamp,               TimeStamp2);
    QCOMPARE(Ex2.m_ErrorCode,               quint32(22));
    QCOMPARE(Ex2.m_AdditionalData.size(),   0);

    // Exception(const QString & File, int Line, const QDateTime & TimeStamp, const QString & ErrorCode, const QStringList & AdditionalData)
    // create
    QTest::qSleep(ExceptionSleepTime);
    QDateTime TimeStamp3 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Exception Ex3("File3", 3, TimeStamp3, 33, tTranslatableStringList() << "31" << "32" << "33");
    // test
    QCOMPARE(Ex3.m_File,                            QString("File3"));
    QCOMPARE(Ex3.m_Line,                            3);
    QCOMPARE(Ex3.m_TimeStamp,                       TimeStamp3);
    QCOMPARE(Ex3.m_ErrorCode,                       quint32(33));
    QCOMPARE(Ex3.m_AdditionalData.size(),           3);
    QCOMPARE(Ex3.m_AdditionalData[0].GetString(),   QString("31"));
    QCOMPARE(Ex3.m_AdditionalData[1].GetString(),   QString("32"));
    QCOMPARE(Ex3.m_AdditionalData[2].GetString(),   QString("33"));

    // Exception(const Exception & r_Other)
    Exception Ex4(Ex3);
    // test
    QCOMPARE(Ex4.m_File,                                QString("File3"));
    QCOMPARE(Ex4.m_Line,                                3);
    QCOMPARE(Ex4.m_TimeStamp,                           TimeStamp3);
    QCOMPARE(Ex4.m_ErrorCode,                           quint32(33));
    QCOMPARE(Ex4.m_AdditionalData.size(),               3);
    QCOMPARE(Ex4.m_AdditionalData[0].GetString(),       QString("31"));
    QCOMPARE(Ex4.m_AdditionalData[1].GetString(),       QString("32"));
    QCOMPARE(Ex4.m_AdditionalData[2].GetString(),       QString("33"));
}

/****************************************************************************/
void TestException::utAssignmentOperator() {
    // const Exception & operator = (const Exception & r_Other)
    QTest::qSleep(ExceptionSleepTime);
    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Exception Ex1("File1", 1, TimeStamp1, 11, tTranslatableStringList() << "11" << "12" << "13");
    // test
    QCOMPARE(Ex1.m_File,                                QString("File1"));
    QCOMPARE(Ex1.m_Line,                                1);
    QCOMPARE(Ex1.m_TimeStamp,                           TimeStamp1);
    QCOMPARE(Ex1.m_ErrorCode,                           quint32(11));
    QCOMPARE(Ex1.m_AdditionalData.size(),               3);
    QCOMPARE(Ex1.m_AdditionalData[0].GetString(),       QString("11"));
    QCOMPARE(Ex1.m_AdditionalData[1].GetString(),       QString("12"));
    QCOMPARE(Ex1.m_AdditionalData[2].GetString(),       QString("13"));

    // create
    QTest::qSleep(ExceptionSleepTime);
    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Exception Ex2("File2", 2, TimeStamp2, 22);
    QTest::qSleep(ExceptionSleepTime);
    QDateTime TimeStamp3 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Exception Ex3("File3", 3, TimeStamp3, 33);
    QCOMPARE(Ex2.m_File,                    QString("File2"));
    QCOMPARE(Ex2.m_Line,                    2);
    QCOMPARE(Ex2.m_TimeStamp,               TimeStamp2);
    QCOMPARE(Ex2.m_ErrorCode,               quint32(22));
    QCOMPARE(Ex2.m_AdditionalData.size(),   0);
    QCOMPARE(Ex3.m_File,                    QString("File3"));
    QCOMPARE(Ex3.m_Line,                    3);
    QCOMPARE(Ex3.m_TimeStamp,               TimeStamp3);
    QCOMPARE(Ex3.m_ErrorCode,               quint32(33));
    QCOMPARE(Ex3.m_AdditionalData.size(),   0);
    // assignment operator
    Ex3 = Ex2 = Ex1;
    // test
    QCOMPARE(Ex2.m_File,                                QString("File1"));
    QCOMPARE(Ex2.m_Line,                                1);
    QCOMPARE(Ex2.m_TimeStamp,                           TimeStamp1);
    QCOMPARE(Ex2.m_ErrorCode,                           quint32(11));
    QCOMPARE(Ex2.m_AdditionalData.size(),               3);
    QCOMPARE(Ex2.m_AdditionalData[0].GetString(),       QString("11"));
    QCOMPARE(Ex2.m_AdditionalData[1].GetString(),       QString("12"));
    QCOMPARE(Ex2.m_AdditionalData[2].GetString(),       QString("13"));
    QCOMPARE(Ex3.m_File,                                QString("File1"));
    QCOMPARE(Ex3.m_Line,                                1);
    QCOMPARE(Ex3.m_TimeStamp,                           TimeStamp1);
    QCOMPARE(Ex3.m_ErrorCode,                           quint32(11));
    QCOMPARE(Ex3.m_AdditionalData.size(),               3);
    QCOMPARE(Ex3.m_AdditionalData[0].GetString(),       QString("11"));
    QCOMPARE(Ex3.m_AdditionalData[1].GetString(),       QString("12"));
    QCOMPARE(Ex3.m_AdditionalData[2].GetString(),       QString("13"));

    // assign to self
    Ex3 = Ex3;
    // test
    QCOMPARE(Ex3.m_Line,                                1);
    QCOMPARE(Ex3.m_TimeStamp,                           TimeStamp1);
    QCOMPARE(Ex3.m_ErrorCode,                           quint32(11));
    QCOMPARE(Ex3.m_AdditionalData.size(),               3);
    QCOMPARE(Ex3.m_AdditionalData[0].GetString(),       QString("11"));
    QCOMPARE(Ex3.m_AdditionalData[1].GetString(),       QString("12"));
    QCOMPARE(Ex3.m_AdditionalData[2].GetString(),       QString("13"));
}

/****************************************************************************/
void TestException::utGetMethods() {
    QTest::qSleep(ExceptionSleepTime);
    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Exception Ex1("File1", 1, TimeStamp1, 11, tTranslatableStringList() << "11" << "12" << "13");
    // test
    QCOMPARE(Ex1.GetFile(),                                 QString("File1"));
    QCOMPARE(Ex1.GetLine(),                                 1);
    QCOMPARE(Ex1.GetTimeStamp(),                            TimeStamp1);
    QCOMPARE(Ex1.GetErrorCode(),                            quint32(11));
    QCOMPARE(Ex1.GetAdditionalData().size(),                3);
    QCOMPARE(Ex1.GetAdditionalData()[0].GetString(),        QString("11"));
    QCOMPARE(Ex1.GetAdditionalData()[1].GetString(),        QString("12"));
    QCOMPARE(Ex1.GetAdditionalData()[2].GetString(),        QString("13"));
}

} // end namespace Global

QTEST_MAIN(Global::TestException)

#include "TestException.moc"
