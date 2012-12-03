/****************************************************************************/
/*! \file TestEventFilter.cpp
 *
 *  \brief Implementation file for class TestEventFilter.
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
#include <DataLogging/Include/EventFilter.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Test class for EventFilter class.
 */
/****************************************************************************/
class TestEventFilter : public QObject {
    Q_OBJECT
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
     * \brief Test default constructor for FileInfo.
     */
    /****************************************************************************/
    void utDefaultConstructorFileInfo();
    /****************************************************************************/
    /**
     * \brief Test copy constructor for FileInfo.
     */
    /****************************************************************************/
    void utCopyConstructorFileInfo();
    /****************************************************************************/
    /**
     * \brief Test AddLine for FileInfo.
     */
    /****************************************************************************/
    void utAddLineFileInfo();
    /****************************************************************************/
    /**
     * \brief Test AddLineRange for FileInfo.
     */
    /****************************************************************************/
    void utAddLineRangeFileInfo();
    /****************************************************************************/
    /**
     * \brief Test Contains for FileInfo.
     */
    /****************************************************************************/
    void utContainsFileInfo();
    /****************************************************************************/
    /**
     * \brief Test Clear.
     */
    /****************************************************************************/
    void utClear();
    /****************************************************************************/
    /**
     * \brief Test RemovePath.
     */
    /****************************************************************************/
    void utRemovePath();
    /****************************************************************************/
    /**
     * \brief Test AddLine.
     */
    /****************************************************************************/
    void utAddLine();
    /****************************************************************************/
    /**
     * \brief Test AddLineRange.
     */
    /****************************************************************************/
    void utAddLineRange();
    /****************************************************************************/
    /**
     * \brief Test MustSend.
     */
    /****************************************************************************/
    void utMustSend();
}; // end class TestEventFilter

/****************************************************************************/
void TestEventFilter::initTestCase() {
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(), 0);
}

/****************************************************************************/
void TestEventFilter::init() {
    EventFilter::Instance().Clear();
}

/****************************************************************************/
void TestEventFilter::cleanup() {
}

/****************************************************************************/
void TestEventFilter::cleanupTestCase() {
}

/****************************************************************************/
void TestEventFilter::utDefaultConstructorFileInfo() {
    // test default constructor
    EventFilter::FileInfo TestObject;

    QCOMPARE(TestObject.m_Lines.size(), 0);
}

/****************************************************************************/
void TestEventFilter::utCopyConstructorFileInfo() {
    // test copy constructor
    EventFilter::FileInfo TestObject1;
    TestObject1.m_Lines.insert(33);
    TestObject1.m_Lines.insert(42);
    EventFilter::FileInfo TestObject2(TestObject1);

    QCOMPARE(TestObject1.m_Lines.size(),        2);
    QCOMPARE(TestObject1.m_Lines.contains(33),  true);
    QCOMPARE(TestObject1.m_Lines.contains(42),  true);
    QCOMPARE(TestObject2.m_Lines.size(),        2);
    QCOMPARE(TestObject2.m_Lines.contains(33),  true);
    QCOMPARE(TestObject2.m_Lines.contains(42),  true);
}

/****************************************************************************/
void TestEventFilter::utAddLineFileInfo() {
    EventFilter::FileInfo TestObject;
    QCOMPARE(TestObject.m_Lines.size(),         0);
    TestObject.AddLine(33);
    QCOMPARE(TestObject.m_Lines.size(),         1);
    QCOMPARE(TestObject.m_Lines.contains(33),   true);
    TestObject.AddLine(42);
    QCOMPARE(TestObject.m_Lines.size(),         2);
    QCOMPARE(TestObject.m_Lines.contains(33),   true);
    QCOMPARE(TestObject.m_Lines.contains(42),   true);
}

/****************************************************************************/
void TestEventFilter::utAddLineRangeFileInfo() {
    EventFilter::FileInfo TestObject;
    QCOMPARE(TestObject.m_Lines.size(),         0);
    // add first line range
    TestObject.AddLineRange(1, 3);
    QCOMPARE(TestObject.m_Lines.size(),         3);
    QCOMPARE(TestObject.m_Lines.contains(1),    true);
    QCOMPARE(TestObject.m_Lines.contains(2),    true);
    QCOMPARE(TestObject.m_Lines.contains(3),    true);
    // add disjoint line range
    TestObject.AddLineRange(5, 7);
    QCOMPARE(TestObject.m_Lines.size(),         6);
    QCOMPARE(TestObject.m_Lines.contains(1),    true);
    QCOMPARE(TestObject.m_Lines.contains(2),    true);
    QCOMPARE(TestObject.m_Lines.contains(3),    true);
    QCOMPARE(TestObject.m_Lines.contains(5),    true);
    QCOMPARE(TestObject.m_Lines.contains(6),    true);
    QCOMPARE(TestObject.m_Lines.contains(7),    true);
    // add overlapping range
    TestObject.AddLineRange(4, 6);
    QCOMPARE(TestObject.m_Lines.size(),         7);
    QCOMPARE(TestObject.m_Lines.contains(1),    true);
    QCOMPARE(TestObject.m_Lines.contains(2),    true);
    QCOMPARE(TestObject.m_Lines.contains(3),    true);
    QCOMPARE(TestObject.m_Lines.contains(4),    true);
    QCOMPARE(TestObject.m_Lines.contains(5),    true);
    QCOMPARE(TestObject.m_Lines.contains(6),    true);
    QCOMPARE(TestObject.m_Lines.contains(7),    true);
}

/****************************************************************************/
void TestEventFilter::utContainsFileInfo() {
    EventFilter::FileInfo TestObject;
    QCOMPARE(TestObject.m_Lines.size(),         0);
    // add first line range
    TestObject.AddLineRange(1, 3);
    TestObject.AddLineRange(7, 9);
    TestObject.AddLine(5);
    // now check contains
    QCOMPARE(TestObject.Contains(1),            true);
    QCOMPARE(TestObject.Contains(2),            true);
    QCOMPARE(TestObject.Contains(3),            true);
    QCOMPARE(TestObject.Contains(4),            false);
    QCOMPARE(TestObject.Contains(5),            true);
    QCOMPARE(TestObject.Contains(6),            false);
    QCOMPARE(TestObject.Contains(7),            true);
    QCOMPARE(TestObject.Contains(8),            true);
    QCOMPARE(TestObject.Contains(9),            true);
}

/****************************************************************************/
void TestEventFilter::utClear() {
    // add some data directly to members
    EventFilter::Instance().m_FileInfos["1"].m_Lines.insert(1);
    EventFilter::Instance().m_FileInfos["1"].m_Lines.insert(2);
    EventFilter::Instance().m_FileInfos["2"].m_Lines.insert(3);
    EventFilter::Instance().m_FileInfos["2"].m_Lines.insert(4);
    EventFilter::Instance().m_FileInfos["2"].m_Lines.insert(5);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),     2);
    // clear
    EventFilter::Instance().Clear();
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),    0);
}

/****************************************************************************/
void TestEventFilter::utRemovePath() {
    QCOMPARE(EventFilter::RemovePath(""),           QString(""));
    QCOMPARE(EventFilter::RemovePath("/"),          QString(""));
    QCOMPARE(EventFilter::RemovePath("1/"),         QString(""));
    QCOMPARE(EventFilter::RemovePath("/2"),         QString("2"));
    QCOMPARE(EventFilter::RemovePath("1/2"),        QString("2"));
    QCOMPARE(EventFilter::RemovePath("1/2/"),       QString(""));
    QCOMPARE(EventFilter::RemovePath("1/2/3"),      QString("3"));
}

/****************************************************************************/
void TestEventFilter::utAddLine() {
    // add some lines and check results
    EventFilter::Instance().AddLine("1", 11);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),                1);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(11),     true);
    EventFilter::Instance().AddLine("1", 12);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),                1);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(11),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(12),     true);
    EventFilter::Instance().AddLine("2", 21);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),                2);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(11),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(12),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(21),     true);
    EventFilter::Instance().AddLine("2", 22);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),                2);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(11),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(12),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(21),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(22),     true);
}

/****************************************************************************/
void TestEventFilter::utAddLineRange() {
    // add some line ranges and check results
    EventFilter::Instance().AddLineRange("1", 11, 12);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),                1);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(11),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(12),     true);
    EventFilter::Instance().AddLineRange("1", 14, 15);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),                1);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(11),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(12),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(14),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(15),     true);
    EventFilter::Instance().AddLineRange("2", 21, 22);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),                2);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(11),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(12),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(14),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(15),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(21),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(22),     true);
    EventFilter::Instance().AddLineRange("2", 24, 25);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),                2);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(11),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(12),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(14),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(15),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(21),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(22),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(24),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(25),     true);
    // LineStart > LineStop
    EventFilter::Instance().AddLineRange("3", 26, 25);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),                2);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(11),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(12),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(14),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["1"].Contains(15),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(0),      false);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(21),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(22),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(24),     true);
    QCOMPARE(EventFilter::Instance().m_FileInfos["2"].Contains(25),     true);
}

/****************************************************************************/
void TestEventFilter::utMustSend() {
    // add some line and line ranges and check results
    EventFilter::Instance().AddLineRange("1", 11, 12);
    EventFilter::Instance().AddLineRange("1", 16, 17);
    EventFilter::Instance().AddLineRange("2", 21, 22);
    EventFilter::Instance().AddLineRange("2", 26, 27);
    EventFilter::Instance().AddLine("1", 14);
    EventFilter::Instance().AddLine("2", 24);
    QCOMPARE(EventFilter::Instance().m_FileInfos.size(),    2);
    QCOMPARE(EventFilter::Instance().MustSend("0", 0),      false);
    QCOMPARE(EventFilter::Instance().MustSend("1", 0),      false);
    QCOMPARE(EventFilter::Instance().MustSend("1", 11),     true);
    QCOMPARE(EventFilter::Instance().MustSend("1", 12),     true);
    QCOMPARE(EventFilter::Instance().MustSend("1", 13),     false);
    QCOMPARE(EventFilter::Instance().MustSend("1", 14),     true);
    QCOMPARE(EventFilter::Instance().MustSend("1", 16),     true);
    QCOMPARE(EventFilter::Instance().MustSend("1", 15),     false);
    QCOMPARE(EventFilter::Instance().MustSend("1", 17),     true);
    QCOMPARE(EventFilter::Instance().MustSend("2", 0),      false);
    QCOMPARE(EventFilter::Instance().MustSend("2", 21),     true);
    QCOMPARE(EventFilter::Instance().MustSend("2", 22),     true);
    QCOMPARE(EventFilter::Instance().MustSend("2", 23),     false);
    QCOMPARE(EventFilter::Instance().MustSend("2", 24),     true);
    QCOMPARE(EventFilter::Instance().MustSend("2", 26),     true);
    QCOMPARE(EventFilter::Instance().MustSend("2", 25),     false);
    QCOMPARE(EventFilter::Instance().MustSend("2", 27),     true);
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestEventFilter)

#include "TestEventFilter.moc"
