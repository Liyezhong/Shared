/****************************************************************************/
/*! \file TestExternalProcess.h
 *
 *  \brief Definition file for class TestExternalProcess.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 07.02.2011
 *  $Author:  $ Y.Novak
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

#ifndef EXTERNALPROCESSCONTROL_TESTEXTERNALPROCESS_H
#define EXTERNALPROCESSCONTROL_TESTEXTERNALPROCESS_H

#include <ExternalProcessController/Include/ExternalProcess.h>


namespace ExternalProcessControl {

/// Process start command
const QString ProcessCmd1 = "./CpuBandwidthEater 10 200";
/// Alternative process start command
const QString ProcessCmd2 = "./CpuBandwidthEater";
/// Process parameter N1
const QString Param1 = "10";
/// Process parameter N2
const QString Param2 = "200";
/// External Process name
const QString MY_PROCESS_NAME = "TestExtProcess";

/****************************************************************************/
/**
 * \brief Test class for ExternalProcess class.
 */
/****************************************************************************/
class TestExternalProcess: public QObject
{
  Q_OBJECT

public:

    TestExternalProcess();
    ~TestExternalProcess();

public slots:

    void TestProcessExited(const QString &, int);
    void TestProcessStarted(const QString &);
    void TestProcessError(int);

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    /// \warning: Each test case builds on the output of the previous one.
    /// All tests shall be run exactly in the order as they are listed here:
    void utTestSimpleProcessStart();        ///< Test N1
    void utTestParameterizedProcessStart(); ///< Test N2
    void utTestBadInputHandling();          ///< Test N3

private:

    void CheckForNull();

private:

    ExternalProcess *m_myExtProcess;           ///< the external process to test
    Q_PID            m_myExtProcessPID;        ///< PID of the external process under test
    QString          m_myProcessExitedString;  ///< Name returned by exiting process
    int              m_myProcessExitedCode;    ///< Exit code returned by the process
    QString          m_myProcessStartedString; ///< Name returned by started process
    int              m_myProcessErrorCode;     ///< Error code returned by the process
};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_TESTEXTERNALPROCESS_H
