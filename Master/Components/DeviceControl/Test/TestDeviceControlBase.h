/****************************************************************************/
/*! \file TestDeviceControlBase.h
 *
 *  \brief Definition file for class TestExternalProcess.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 09.02.2011
 *  $Author:  $ Norbert Wiedmann
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef TESTDEVCTRLBASE_H
#define TESTDEVCTRLBASE_H
#include <QObject>
#include <QProcess>

namespace DeviceControl {

class DeviceProcessingResponse;

/****************************************************************************/
/**
 * \brief Test class for TestDeviceControlBase class.
 *
 *        This test starts, initialises and configures the DeviceControlLayer.
 *
 *        Important node!
 *        This class shall not be deleted before all other DeviceControlLayer unit test are done,
 *        otherwise the DeviceControlLayer shuts down and is not available for the other test.
 *        Therefor the QTEST_MAIN macro is not called, main() is impelemented at main.c
 *        Refer the documentation at main.c at this project for details.
 */
/****************************************************************************/
class TestDeviceControlBase: public QObject
{
  Q_OBJECT

public:

    TestDeviceControlBase();
    ~TestDeviceControlBase();

    //!< finishes the tests environment
    void CloseTest();

    DeviceProcessingResponse* GetDeviceProcessingResponse() { return m_pDeviceProcessingResponse; };

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    ///< \warning: Each test case builds on the output of the previous one.
    ///< All tests shall be run exactly in the order as they are listed here:
    void utTestStartDCL();            ///< Test 1
    void utTestDCLConfiguration();    ///< Test 2
    void utTestSerialKey();           ///< Test 3
    void utTestCANNodes();            ///< Test 4

private:
    //!< Register the meta types used by device control layer
    void RegisterMetaTypes();

    //!< virtual class connected to device processing layer
    DeviceProcessingResponse* m_pDeviceProcessingResponse;
    //!< ColoPiaSimulator process
    QProcess* m_pColoPiaProcess;

private:
};

} // end namespace ExternalProcessControl

#endif // TESTDEVCTRLBASE_H
