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
#include <DataLogging/Source/DayLogFileInformation.cpp>
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

    /****************************************************************************/
    /**
     * \brief Test the translations
     */
    /****************************************************************************/
    void utCheckTranslations();
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
    DayLogFileInformation DayFile(QDir::currentPath(), QDir::currentPath(), "Leica_ST");

    QCOMPARE(DayFile.m_LogFilePath, QDir::currentPath());

    DayLogFileInformation AnotherDayFile("/mnt", "/mnt", "Leica_ST");
    QCOMPARE(AnotherDayFile.m_LogFilePath, QString("/mnt"));

}

/****************************************************************************/
void TestDayLogFileInformation::utDailyRunLogFileNames() {
    DayLogFileInformation DayLogFileNames(Global::SystemPaths::Instance().GetLogfilesPath(),
                                          Global::SystemPaths::Instance().GetLogfilesPath(), "TestName_");

    QStringList FileNames;
    DayLogFileNames.CreateAndListDailyRunLogFileName(FileNames);

    QCOMPARE(FileNames.count(), 1);

}

/****************************************************************************/
void TestDayLogFileInformation::utDailyRunLogFileData() {    
    DayLogFileInformation DayLogFileData(Global::SystemPaths::Instance().GetLogfilesPath(),
                                         Global::SystemPaths::Instance().GetLogfilesPath(), "Leica_ST_");

    QStringList FileNames;
    DayLogFileData.CreateAndListDailyRunLogFileName(FileNames);

    QCOMPARE(FileNames.count(), 1);

    QByteArray DataBytes;
    DataBytes.clear();
    DayLogFileData.CreateSpecificDailyRunLogFile(FileNames.value(0), DataBytes);

}

/****************************************************************************/
void TestDayLogFileInformation::utDailyRunLogFileCreation() {
    DayLogFileInformation DayLogFileData(Global::SystemPaths::Instance().GetLogfilesPath(),
                                         Global::SystemPaths::Instance().GetLogfilesPath(), "ColoradoEvents_");

    QStringList FileNames;
    DayLogFileData.CreateDailyRunLogFiles(FileNames);

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

/****************************************************************************/
void TestDayLogFileInformation::utCheckTranslations() {
    DayLogFileInformation DayLogFileData(Global::SystemPaths::Instance().GetLogfilesPath(),
                                         Global::SystemPaths::Instance().GetLogfilesPath(), "ColoradoEvents_");

    Global::tTranslatableStringList TranlsateList;

    DayLogFileData.TranslateTheParameters (0, TranlsateList);

    QCOMPARE(TranlsateList.count(), 0);
    QString EmptyString("");
    QStringList EmptyNameList;

    QCOMPARE(DayLogFileData.CheckAndTranslateTheEventID(EmptyString,
                                         DayLogFileData.m_EventIDs, EmptyNameList), false);


    QString TestString(STRING_UNDEFINEDTYPE);
    DayLogFileData.TranslateEventType(TestString);
    QCOMPARE(TestString, STRING_UNDEFINEDTYPE);
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestDayLogFileInformation)

#include "TestDayLogFileInformation.moc"
