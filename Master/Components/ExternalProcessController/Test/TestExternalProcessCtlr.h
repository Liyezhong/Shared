/****************************************************************************/
/*! \file TestExternalProcessCtlr.h
 *
 *  \brief Definition file for class TestExternalProcessCtlr.
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

#ifndef EXTERNALPROCESSCONTROL_TESTEXTERNALPROCESSCTLR_H
#define EXTERNALPROCESSCONTROL_TESTEXTERNALPROCESSCTLR_H

#include <ExternalProcessController/Test/DerivedController.h>


namespace ExternalProcessControl {

/// String for ProtocolHandler initialization:
const QString EPC_TEST_CLIENT     = "Colorado Device GUI";
/// Type of configuration document
const QString EPC_CONFIGFILE_TYPE = "netlayer_messages";
/// Path ending to find config files under Settings directory
const QString EPC_PATH_ADDITION = "/Communication";

/****************************************************************************/
/**
 * \brief Test class for ExternalProcessController class.
 */
/****************************************************************************/
class TestExternalProcessCtlr: public QObject
{
  Q_OBJECT

public:

    TestExternalProcessCtlr();
    ~TestExternalProcessCtlr();

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    /// \warning: Each test case builds on the output of the previous one.
    /// All tests shall be run exactly in the order as they are listed here:
    void utTestWorkingFunctions();          ///< Test N1
    void utTestBadInputHandling();          ///< Test N2

private:

    void CheckForNull();

private:

    DerivedController         *m_myExtProcessCtlr; ///< the external process controller to test
    ExternalProcessDevice     *m_myExtDevice;  ///< the external process to test
    Q_PID            m_myExtProcessPID;        ///< PID of the external process under test
    QString          m_myProcessExitedString;  ///< Name returned by exiting process
    int              m_myProcessExitedCode;    ///< Exit code returned by the process
    QString          m_myProcessStartedString; ///< Name returned by started process
    int              m_myProcessErrorCode;     ///< Error code returned by the process
};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_TESTEXTERNALPROCESSCTLR_H
