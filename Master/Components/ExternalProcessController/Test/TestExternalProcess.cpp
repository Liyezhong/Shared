/****************************************************************************/
/*! \file TestExternalProcess.cpp
 *
 *  \brief Implementation file for class TestExternalProcess.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.02.2011
 *   $Author:  $ Y.Novak
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
#include <ExternalProcessController/Test/TestExternalProcess.h>

namespace ExternalProcessControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestExternalProcess::TestExternalProcess() :
            m_myExtProcess(NULL),
            m_myExtProcessPID(0),
            m_myProcessExitedString(""),
            m_myProcessExitedCode(-5),
            m_myProcessStartedString(""),
            m_myProcessErrorCode(-5)
{
    //qRegisterMetaType<DataLogging::EventEntry>("DataLogging::EventEntry");
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestExternalProcess::~TestExternalProcess()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcess::initTestCase()
{
    // create process:
    m_myExtProcess = new ExternalProcess(MY_PROCESS_NAME, this);
    // verify pointer value:
    QVERIFY(m_myExtProcess->m_myProcess == NULL);
    // verify name value:
    QVERIFY(m_myExtProcess->m_myName == MY_PROCESS_NAME);
    // initialize my process:
    QCOMPARE(m_myExtProcess->Initialize(), true);
    // check that object got created:
    QVERIFY(m_myExtProcess->m_myProcess != NULL);

    // connect ProcessManager's start/exit/error signals:
    if (!QObject::connect(m_myExtProcess, SIGNAL(ProcessExited(const QString &, int)),
                                    this, SLOT(TestProcessExited(const QString &, int)))) {
        QFAIL("TestExternalProcess: cannot connect \"ProcessExited\" signal !");
        return;
    }
    if (!QObject::connect(m_myExtProcess, SIGNAL(ProcessStarted(const QString &)),
                                    this, SLOT(TestProcessStarted(const QString &)))) {
        QFAIL("TestExternalProcess: cannot connect \"ProcessStarted\" signal !");
        return;
    }
    if (!QObject::connect(m_myExtProcess, SIGNAL(ProcessError(int)),
                                    this, SLOT(TestProcessError(int)))) {
        QFAIL("TestExternalProcess: cannot connect \"ProcessErrorOccured\" signal !");
        return;
    }
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcess::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcess::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcess::cleanupTestCase()
{
    // cleanup:
    delete m_myExtProcess;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestExternalProcess::CheckForNull()
{
    if (m_myExtProcess == NULL) {
        QFAIL("TestExternalProcess: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test process startup with command without parameters.
 */
/****************************************************************************/
void TestExternalProcess::utTestSimpleProcessStart()
{
    CheckForNull();

    // start process with one command:
    QCOMPARE(m_myExtProcess->StartProcess(QCoreApplication::applicationDirPath() + "/" + ProcessCmd1), true);
    QVERIFY(m_myExtProcessPID == 0);
    // check if process actually started:
    m_myExtProcessPID = m_myExtProcess->m_myProcess->pid();
    qDebug() << "TestExternalProcess: got Process PID: " << m_myExtProcessPID;
    QVERIFY(m_myExtProcessPID != 0);
    // wait to give process time to start:
    QTest::qWait(500);
    // check that startup signal was emitted:
    QCOMPARE(m_myProcessStartedString, MY_PROCESS_NAME);
    // reset the flag:
    m_myProcessStartedString = "";
    // run stdout-reading functions just to make sure they do not blowup anything:
    m_myExtProcess->ReadStdError();
    m_myExtProcess->ReadStdOut();
    //check the process is still alive:
    QCOMPARE(m_myExtProcess->m_myProcess->state(), QProcess::Running);
    // kill process:
    QCOMPARE(m_myExtProcess->TerminateProcess(), true);
    // wait to give process time to die:
    QTest::qWait(500);
    // check if process actually was killed:
    m_myExtProcessPID = m_myExtProcess->m_myProcess->pid();
    qDebug() << "TestExternalProcess: got killed Process PID: " << m_myExtProcessPID ;
    QVERIFY(m_myExtProcessPID == 0);
    // check that signal was emitted:
    QCOMPARE(m_myProcessExitedString, MY_PROCESS_NAME);
    QCOMPARE(m_myProcessExitedCode, 0);
    QCOMPARE(m_myProcessErrorCode, 1);
    // reset the flag:
    m_myProcessExitedString = "";
    m_myProcessExitedCode = -5;
    m_myProcessErrorCode = -5;
}

/****************************************************************************/
/**
 * \brief Test process startup with command parameters.
 */
/****************************************************************************/
void TestExternalProcess::utTestParameterizedProcessStart()
{
    CheckForNull();

    QStringList ParamList;
    ParamList << Param1 << Param2;
    // start process with one command:
    QCOMPARE(m_myExtProcess->StartProcess(QCoreApplication::applicationDirPath() + "/" + ProcessCmd2, ParamList), true);
    QVERIFY(m_myExtProcessPID == 0);
    // check if process actually started:
    m_myExtProcessPID = m_myExtProcess->m_myProcess->pid();
    qDebug() << "TestExternalProcess: got Process PID: " << m_myExtProcessPID;
    QVERIFY(m_myExtProcessPID != 0);
    // wait to give process time to start:
    QTest::qWait(500);
    // check that startup signal was emitted:
    QCOMPARE(m_myProcessStartedString, MY_PROCESS_NAME);
    // reset the flag:
    m_myProcessStartedString = "";
    // run stdout-reading functions just to make sure they do not blowup anything:
    m_myExtProcess->ReadStdError();
    m_myExtProcess->ReadStdOut();
    //check the process is still alive:
    QCOMPARE(m_myExtProcess->m_myProcess->state(), QProcess::Running);
    // kill process:
    QCOMPARE(m_myExtProcess->KillProcess(), true);
    // wait to give process time to die:
    QTest::qWait(500);
    // check if process actually was killed:
    m_myExtProcessPID = m_myExtProcess->m_myProcess->pid();
    qDebug() << "TestExternalProcess: got killed Process PID: " << m_myExtProcessPID ;
    QVERIFY(m_myExtProcessPID == 0);
    // check that signal was emitted:
    QCOMPARE(m_myProcessExitedString, MY_PROCESS_NAME);
    QCOMPARE(m_myProcessExitedCode, 0);
    QCOMPARE(m_myProcessErrorCode, 1);
    // reset the flag:
    m_myProcessExitedString = "";
    m_myProcessExitedCode = -5;
    m_myProcessErrorCode = -5;
}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestExternalProcess::utTestBadInputHandling()
{
    CheckForNull();

    // make sure that m_myExtProcess->m_myProcess == NULL:
    delete m_myExtProcess;
    m_myExtProcess = new ExternalProcess((QString)"Test", this);
    QVERIFY(m_myExtProcess->m_myProcess == NULL);
    // try to start non-existent process:
    QCOMPARE(m_myExtProcess->StartProcess(QCoreApplication::applicationDirPath() + "/" + ProcessCmd1), false);
    QStringList ParamList;
    ParamList << Param1 << Param2;
    // try to start non-existent process with parameters:
    QCOMPARE(m_myExtProcess->StartProcess(QCoreApplication::applicationDirPath() + "/" + ProcessCmd2, ParamList), false);
    // run stdout-reading functions just to make sure they do not blowup anything:
    m_myExtProcess->ReadStdError();
    m_myExtProcess->ReadStdOut();
    // try to kill non-existent process:
    QCOMPARE(m_myExtProcess->KillProcess(), false);
    QCOMPARE(m_myExtProcess->TerminateProcess(), false);
}

/****************************************************************************/
/**
 *  \brief Process exit Test slot.
 *
 *  \param Name = name of the exited process
 *  \param Code = code returned by exiting process
 */
/****************************************************************************/
void TestExternalProcess::TestProcessExited(const QString &Name, int Code)
{
    m_myProcessExitedString = Name;
    m_myProcessExitedCode = Code;
    //qDebug() << "TestExternalProcess: TestProcessExited " << Name << Code;
}

/****************************************************************************/
/**
 *  \brief Process start Test slot.
 *
 *  \param Name = name of the started process
 */
/****************************************************************************/
void TestExternalProcess::TestProcessStarted(const QString &Name)
{
    m_myProcessStartedString = Name;
    //qDebug() << "TestExternalProcess: TestProcessStarted " << Name;
}

/****************************************************************************/
/**
 *  \brief Process Error Test slot.
 *
 *  \param Code = error code returned by the process
 */
/****************************************************************************/
void TestExternalProcess::TestProcessError(int Code)
{
    m_myProcessErrorCode = Code;
    //qDebug() << "TestExternalProcess: TestProcessError " << Code;
}

} // end namespace ExternalProcessControl

QTEST_MAIN(ExternalProcessControl::TestExternalProcess)
