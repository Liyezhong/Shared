/****************************************************************************/
/*! \file TestExternalProcessDvc.h
 *
 *  \brief Definition file for class TestExternalProcessDvc.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 09.02.2011
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

#ifndef EXTERNALPROCESSCONTROL_TESTEXTERNALPROCESSDVC_H
#define EXTERNALPROCESSCONTROL_TESTEXTERNALPROCESSDVC_H


#include <ExternalProcessController/Include/ExternalProcessDevice.h>


namespace ExternalProcessControl {

/// String for ProtocolHandler initialization:
const QString EPD_TEST_CLIENT     = "Colorado Device GUI";
/// Type of configuration document
const QString EPD_CONFIGFILE_TYPE = "netlayer_messages";
/// Path ending to find config files under Settings directory
const QString EPD_PATH_ADDITION = "/Communication";

/****************************************************************************/
/**
 * \brief Test class for ExternalProcessDevice class.
 */
/****************************************************************************/
class TestExternalProcessDvc: public QObject
{
  Q_OBJECT

public:

    TestExternalProcessDvc();
    ~TestExternalProcessDvc();

public slots:

    void SlotLoginTimeout();

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

    ExternalProcessDevice     *m_myExtDevice;        ///< the external device to test
    bool                       m_myLoginTimeoutFlag; ///< flag to indicate the login timeout
};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_TESTEXTERNALPROCESSDVC_H
