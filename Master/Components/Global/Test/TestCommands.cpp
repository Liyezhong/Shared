/****************************************************************************/
/*! \file TestCommands.cpp
 *
 *  \brief Implementation file for utilities.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-11-20
 *  $Author:    $ Logan Yang
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
#include <Global/Include/Commands/Acknowledge.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <Global/Include/Commands/CmdByteArray.h>
#include <Global/Include/Commands/CmdDataChanged.h>
#include <Global/Include/Commands/CmdDateAndTime.h>
#include <Global/Include/Commands/CmdEventUpdate.h>
#include <Global/Include/Commands/CmdLCDPowerControl.h>
#include <Global/Include/Commands/CmdPowerFail.h>
#include <Global/Include/Commands/CmdShutDown.h>
#include <Global/Include/Commands/CmdSoftSwitchPressed.h>
#include <Global/Include/Commands/Command.h>
#include <Global/Include/Commands/PendingCmdDescriptor.h>
#include <Global/Include/MonotonicTime.h>

#include <QDebug>

namespace Global {

/****************************************************************************/
/**
 * \brief Test class for utilities.
 */
/****************************************************************************/
class TestCommands : public QObject {
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
     * \brief Test global commands
     */
    /****************************************************************************/
    void utTestCommands();
}; // end class TestUtils

/****************************************************************************/
void TestCommands::initTestCase() {
    //Global::InitSupportedLanguages();
}

/****************************************************************************/
void TestCommands::init() {
}

/****************************************************************************/
void TestCommands::cleanup() {
}

/****************************************************************************/
void TestCommands::utTestCommands() {

    QDataStream ds;
    AckOKNOK ackok;
    ackok.GetName();
    ackok.CopyToStream(ds);
    ackok.CopyFromStream(ds);
    ackok.GetStatus();
    ackok.GetText();
    ackok.GetType();

    CmdDataChanged cmdDC("HimalayaPrograms");
    cmdDC.GetName();
    cmdDC.GetDataContainerName();

    cmdDC.GetName();
    cmdDC.CopyToStream(ds);
    cmdDC.CopyFromStream(ds);
    cmdDC.GetTimeout();
//    cmdDC.setBusyStateAllowed(true);
//    cmdDC.setErrorStateAllowed(true);
//    cmdDC.setIdleStateAllowed(true);
//    cmdDC.setInitFailedStateAllowed(true);
//    cmdDC.setSoftSwitchMonitorStateAllowed(true);
//    cmdDC.setUserActionStateAllowed(true);
//    QVERIFY(cmdDC.isBusyStateAllowed());
//    QVERIFY(cmdDC.isErrorStateAllowed());
//    QVERIFY(cmdDC.isIdleStateAllowed());
//    QVERIFY(cmdDC.isInitFailedStateAllowed());
//    QVERIFY(cmdDC.isSoftSwitchStateAllowed());
//    QVERIFY(cmdDC.isStateAllowed("IdleState"));
//    QVERIFY(cmdDC.isUserActionAllowed());
    CmdDataChanged CmdDC1(cmdDC);
    CmdDC1 = cmdDC;
    CmdDC1.CopyFrom(cmdDC);

    CmdLCDPowerControl cmdlcd(true);
    cmdlcd.GetName();
    cmdlcd.CopyToStream(ds);
    cmdlcd.CopyFromStream(ds);
    CmdLCDPowerControl cmdlcd1(cmdlcd);
    cmdlcd1 = cmdlcd;
    cmdlcd1.CopyFrom(cmdlcd);
    cmdlcd.GetTimeout();
//    cmdlcd.setBusyStateAllowed(true);
//    cmdlcd.setErrorStateAllowed(true);
//    cmdlcd.setIdleStateAllowed(true);
//    cmdlcd.setInitFailedStateAllowed(true);
//    cmdlcd.setSoftSwitchMonitorStateAllowed(true);
//    cmdlcd.setUserActionStateAllowed(true);
//    QVERIFY(cmdlcd.isBusyStateAllowed());
//    QVERIFY(cmdlcd.isErrorStateAllowed());
//    QVERIFY(cmdlcd.isIdleStateAllowed());
//    QVERIFY(cmdlcd.isInitFailedStateAllowed());
//    QVERIFY(cmdlcd.isSoftSwitchStateAllowed());
//    QVERIFY(cmdlcd.isStateAllowed("IdleState"));
//    QVERIFY(cmdlcd.isUserActionAllowed());

    CmdPowerFail cmdpf(POWER_FAIL_STAGE_1);
    cmdpf.GetName();
    cmdpf.CopyToStream(ds);
    cmdpf.CopyFromStream(ds);
    CmdPowerFail cmdpf1(cmdpf);
    cmdpf1 = cmdpf;
    cmdpf1.CopyFrom(cmdpf);
    cmdpf.GetTimeout();
//    cmdpf.setBusyStateAllowed(true);
//    cmdpf.setErrorStateAllowed(true);
//    cmdpf.setIdleStateAllowed(true);
//    cmdpf.setInitFailedStateAllowed(true);
//    cmdpf.setSoftSwitchMonitorStateAllowed(true);
//    cmdpf.setUserActionStateAllowed(true);
//    QVERIFY(cmdpf.isBusyStateAllowed());
//    QVERIFY(cmdpf.isErrorStateAllowed());
//    QVERIFY(cmdpf.isIdleStateAllowed());
//    QVERIFY(cmdpf.isInitFailedStateAllowed());
//    QVERIFY(cmdpf.isSoftSwitchStateAllowed());
//    QVERIFY(cmdpf.isStateAllowed("IdleState"));
//    QVERIFY(cmdpf.isUserActionAllowed());

    CmdShutDown cmdsd;
    cmdsd.GetName();
    cmdsd.CopyToStream(ds);
    cmdsd.CopyFromStream(ds);
    CmdShutDown cmdsd1(cmdsd);
    cmdsd1 = cmdsd;
    cmdsd1.CopyFrom(cmdsd);
    cmdsd.GetTimeout();
//    cmdsd.setBusyStateAllowed(true);
//    cmdsd.setErrorStateAllowed(true);
//    cmdsd.setIdleStateAllowed(true);
//    cmdsd.setInitFailedStateAllowed(true);
//    cmdsd.setSoftSwitchMonitorStateAllowed(true);
//    cmdsd.setUserActionStateAllowed(true);
//    QVERIFY(cmdsd.isBusyStateAllowed());
//    QVERIFY(cmdsd.isErrorStateAllowed());
//    QVERIFY(cmdsd.isIdleStateAllowed());
//    QVERIFY(cmdsd.isInitFailedStateAllowed());
//    QVERIFY(cmdsd.isSoftSwitchStateAllowed());
//    QVERIFY(cmdsd.isStateAllowed("IdleState"));
//    QVERIFY(cmdsd.isUserActionAllowed());


    CmdSoftSwitchPressed cmdssp;
    cmdssp.GetName();
    cmdssp.CopyToStream(ds);
    cmdssp.CopyFromStream(ds);
    CmdSoftSwitchPressed cmdssp1;
    cmdssp1.CopyFrom(cmdssp);
    cmdssp.GetTimeout();
//    cmdssp.setBusyStateAllowed(true);
//    cmdssp.setErrorStateAllowed(true);
//    cmdssp.setIdleStateAllowed(true);
//    cmdssp.setInitFailedStateAllowed(true);
//    cmdssp.setSoftSwitchMonitorStateAllowed(true);
//    cmdssp.setUserActionStateAllowed(true);
//    QVERIFY(cmdssp.isBusyStateAllowed());
//    QVERIFY(cmdssp.isErrorStateAllowed());
//    QVERIFY(cmdssp.isIdleStateAllowed());
//    QVERIFY(cmdssp.isInitFailedStateAllowed());
//    QVERIFY(cmdssp.isSoftSwitchStateAllowed());
//    QVERIFY(cmdssp.isStateAllowed("IdleState"));
//    QVERIFY(cmdssp.isUserActionAllowed());

    PendingCmdDescriptor pcmdd(NULL,0,"TestCmd",5000);
    pcmdd.GetName();
}


void TestCommands::cleanupTestCase() {
}
} // end namespace Global

QTEST_MAIN(Global::TestCommands)

#include "TestCommands.moc"
