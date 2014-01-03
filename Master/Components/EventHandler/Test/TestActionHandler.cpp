/****************************************************************************/
/*! \file TestActionHandler.cpp
 *
 *  \brief Implementation file for class TestActionHandler.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-03
 *  $Author:    $ Aparna
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
#include <Global/Include/Utils.h>
#include <EventHandler/Include/ActionHandler.h>
#include <DataLogging/Include/DayEventEntry.h>
#include <NetCommands/Include/CmdEventReport.h>
#include <EventHandler/Include/StateHandler.h>
#include <../../../Global/Include/Utils.h>
#include <../../../EventHandler/Include/EventHandlerThreadController.h>
#include <../../../NetCommands/Include/CmdSystemAction.h>

namespace EventHandler {

static QString FilePathEventConf;    ///< Path to csv.


/****************************************************************************/
/**
 * \brief Test class for XmlConfigFile class.
 */
/****************************************************************************/
class TestActionHandler : public QObject {
    Q_OBJECT

public:
    friend class EventHandler::ActionHandler;
private slots:
    /****************************************************************************/
    /**
     * \brief Receive Events Test Case.
     */
    /****************************************************************************/
    void ReceiveEventTestCase();
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



}; // end class TestActionHandler

/****************************************************************************/
void TestActionHandler::initTestCase() {
    // set paths
//    QString ExePath = QCoreApplication::applicationDirPath();
//    FilesPathErr = ExePath + "/../xmlErrFiles/";
//    FilesPathOK  = ExePath + "/../xmlOkFiles/";
//    FilesPathWrite = ExePath + "/";
//    // init languages
//    Global::InitSupportedLanguages();
}

/****************************************************************************/
void TestActionHandler::init() {
}

/****************************************************************************/
void TestActionHandler::cleanup() {
}

/****************************************************************************/
void TestActionHandler::cleanupTestCase() {
}

void TestActionHandler::ReceiveEventTestCase() {

    EventHandler::ActionHandler myActionHandler(NULL);
    DataLogging::DayEventEntry TheEvent;
    quint32 EventKey;
    myActionHandler.ReceiveEvent(TheEvent, EventKey);
}

 
 
 



///****************************************************************************/
//void TestActionHandler::utTestWriteFormatVersion() {
//    // write OK
//    QString FileName = FilesPathWrite + "dummy.xml";
//    try {
//        QXmlStreamWriter Writer;
//        QFile File;
//        XmlConfigFile Obj;
//        // call InitStreamWriter
//        Obj.InitStreamWriter(Writer, File, FileName);
//        // write start document
//        Writer.writeStartDocument("1.0");
//        // call WriteFormatVersion
//        Obj.WriteFormatVersion(Writer, "roottag", "42");
//        // write end document
//        Writer.writeEndDocument();
//        // close file
//        File.close();
//        // check data. take into account newlines.
//        QVERIFY((File.size() >= qint64(63)) && (File.size() <= qint64(65)));
//    } catch(...) {
//        QFAIL("Unknown exception caught!");
//    }
//    // remove dummy file again
//    QFile::remove(FileName);
//}

} // end namespace EventHandler

QTEST_MAIN(EventHandler::TestActionHandler)

#include "TestActionHandler.moc"
