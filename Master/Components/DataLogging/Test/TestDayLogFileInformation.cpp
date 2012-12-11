/****************************************************************************/
/*! \file TestDayLogFileInformation.cpp
 *
 *  \brief Implementation file for class TestDayLogFileInformation.
 *
 *  \b Description:
 *        This class tests all the important functions which are available
 *        in DayLogFileInformation class.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-10
 *  $Author:    $ Raju
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
#include <DataLogging/Include/DayLogFileInformation.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/SystemPaths.h>
#include <QProcess>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Test class for DayLogFileInformation class.
 */
/****************************************************************************/
class TestDayLogFileInformation : public QObject {
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
     * \brief Test constructor with additional data list
     */
    /****************************************************************************/
    void utConstructorWithData();
    /****************************************************************************/
    /**
     * \brief Test daily run log file names.
     */
    /****************************************************************************/
    void utDailyRunLogFileNames();
    /****************************************************************************/
    /**
     * \brief Test the log file data.
     */
    /****************************************************************************/
    void utDailyRunLogFileData();

    /****************************************************************************/
    /**
     * \brief Test the log files creation in a directory.
     */
    /****************************************************************************/
    void utDailyRunLogFileCreation();
}; // end class TestDayLogFileInformation

/****************************************************************************/
void TestDayLogFileInformation::initTestCase() {
}

/****************************************************************************/
void TestDayLogFileInformation::init() {
    Global::SystemPaths::Instance().SetLogfilesPath("../Logfiles");
    Global::SystemPaths::Instance().SetTranslationsPath("../Translations");
}

/****************************************************************************/
void TestDayLogFileInformation::cleanup() {
}

/****************************************************************************/
void TestDayLogFileInformation::cleanupTestCase() {
}

/****************************************************************************/
void TestDayLogFileInformation::utConstructorWithData() {
    DayLogFileInformation DayFile(QDir::currentPath());

    QCOMPARE(DayFile.m_LogFilePath, QDir::currentPath());

    DayLogFileInformation AnotherDayFile("/mnt");
    QCOMPARE(AnotherDayFile.m_LogFilePath, QString("/mnt"));

}

/****************************************************************************/
void TestDayLogFileInformation::utDailyRunLogFileNames() {
    DayLogFileInformation DayLogFileNames(Global::SystemPaths::Instance().GetLogfilesPath());

    QStringList FileNames;
    DayLogFileNames.CreateAndListDailyRunLogFileName(FileNames);

    QCOMPARE(FileNames.count(), 3);

}

/****************************************************************************/
void TestDayLogFileInformation::utDailyRunLogFileData() {    
    DayLogFileInformation DayLogFileData(Global::SystemPaths::Instance().GetLogfilesPath());

    QStringList FileNames;
    DayLogFileData.CreateAndListDailyRunLogFileName(FileNames);

    QCOMPARE(FileNames.count(), 3);

    QByteArray DataBytes;
    DataBytes.clear();
    DayLogFileData.CreateSpecificDailyRunLogFile(FileNames.value(0),
                                                 Global::SystemPaths::Instance().GetTranslationsPath()
                                                 + QDir::separator() + "EventStrings_en.xml", DataBytes);

}

/****************************************************************************/
void TestDayLogFileInformation::utDailyRunLogFileCreation() {
    DayLogFileInformation DayLogFileData(Global::SystemPaths::Instance().GetLogfilesPath());

    QStringList FileNames;
    DayLogFileData.CreateDailyRunLogFiles(Global::SystemPaths::Instance().GetTranslationsPath()
                                          + QDir::separator() + "EventStrings_en.xml", FileNames);

    QCOMPARE(FileNames.count(), 3);


    // remove all the files from the directory
    foreach (QString FileName, FileNames) {
        QFile::remove(FileName);
    }
    // get the directory name
    QString DirectoryName = FileNames.value(0);
    DirectoryName.chop(DirectoryName.length() - DirectoryName.lastIndexOf(QDir::separator()));
    QDir Directory;
    Directory.rmdir(DirectoryName);

}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestDayLogFileInformation)

#include "TestDayLogFileInformation.moc"
