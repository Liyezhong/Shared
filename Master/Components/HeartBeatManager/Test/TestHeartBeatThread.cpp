#include <QtTest/QTest>
#include <QDebug>
#include <QFile>
#include <QProcess>


#include "HeartBeatManager/Include/HeartBeatThread.h"
#include "Threads/Include/ThreadController.h"
#include <HeartBeatManager//Include/HeartBeatCSVInfo.h>
#include "HeartBeatManager/Include/Commands/CmdAddControllerForHeartBeatCheck.h"
#include "HeartBeatManager/Include/Commands/CmdHeartBeat.h"
#include "HeartBeatManager/Include/Commands/CmdRemoveControllerForHeartBeatCheck.h"

/****************************************************************************/
/**
 * \brief class for testing HeartBeatThread class.
 */
/****************************************************************************/
class ControllerTest : public QObject {

    Q_OBJECT
    friend class TestHeartBeatThread;
public:

    quint16 m_HeartBeatSource;

    ControllerTest():m_HeartBeatSource(234)
    {

    }
    void SendSignal()
    {
        emit Timeout();
    }

signals:
    void Timeout();
 };

namespace HeartBeatManager {

/****************************************************************************/
/**
 * \brief Test class for HeartBeatThread class.
 */
/****************************************************************************/
class TestHeartBeatThread : public QObject {
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
     * \brief Test data of HeartBeatThreadController.
     */
    /****************************************************************************/
    void utTestHeartbeat();

    /****************************************************************************/
    /**
     * \brief Test data of HeartBeat EventCSVInfo.
     */
    /****************************************************************************/
    void utTestHeartbeatCSVInfo();

    /****************************************************************************/
    /**
     * \brief Test data of HeartBeat Commands.
     */
    /****************************************************************************/
    void utTestHeartbeatCommands();


}; // end class TestDataModuleList

/****************************************************************************/
void TestHeartBeatThread::initTestCase() {
}

/****************************************************************************/
void TestHeartBeatThread::init() {
}

/****************************************************************************/
void TestHeartBeatThread::cleanup() {
}

/****************************************************************************/
void TestHeartBeatThread::cleanupTestCase() {
}

void TestHeartBeatThread::utTestHeartbeat()
{  

    /// \todo uncomment the below section and write proper test cases
    HeartBeatThreadController *p_HeartBeat = new HeartBeatThreadController(32);
    p_HeartBeat->CreateAndInitializeObjects();
    p_HeartBeat->CleanupAndDestroyObjects();

    //p_HeartBeat->OnGoReceived();
    QCOMPARE(p_HeartBeat->m_HeartbeatCheckTimeout, 5000);
    QCOMPARE(p_HeartBeat->m_ControllerHeartbeatTimeout, 3000);
    HeartBeatManager::CmdAddControllerForHeartBeatCheck *p_CmdForHeartBeatCheck = new HeartBeatManager::CmdAddControllerForHeartBeatCheck(5000, 19);
    p_HeartBeat->AddControllerForHeartBeatCheck(19);
    //p_HeartBeat->StartHeartBeatCheckTimer();
    QCOMPARE(p_HeartBeat->m_HeartbeatCheckTimer.isSingleShot(), false);
    QCOMPARE(p_HeartBeat->m_HeartbeatCheckTimer.isActive(), false);
    p_HeartBeat->SetHeartBeatCheckTimerValue(5000);
    HeartBeatManager::CmdHeartBeat *p_CmdHeartBeat = new HeartBeatManager::CmdHeartBeat(5000, 23);
    p_HeartBeat->HeartBeatSlot(23);
    ControllerTest *p_Cont= new ControllerTest();
    CONNECTSIGNALSLOT(p_Cont, Timeout(),p_HeartBeat, HeartbeatCheck());
    p_Cont->SendSignal();
    HeartBeatManager::CmdRemoveControllerForHeartBeatCheck *p_CmdRemoveHeartBeatCheck= new HeartBeatManager::CmdRemoveControllerForHeartBeatCheck(5000, 20);
    p_HeartBeat->RemoveControllerForHeartBeatCheck(20);
    p_HeartBeat->OnGoReceived();
    p_HeartBeat->OnStopReceived();
    p_HeartBeat->OnPowerFail(Global::POWER_FAIL_STAGE_1);
    p_HeartBeat->StartHeartBeatCheckTimer();
    p_HeartBeat->StopHeartBeatCheckTimer();
    p_HeartBeat->OnMissingHeartBeats(32);

    delete p_CmdForHeartBeatCheck;
    delete p_CmdHeartBeat;
    delete p_CmdRemoveHeartBeatCheck;

}

void TestHeartBeatThread::utTestHeartbeatCSVInfo(){

    HeartBeatCSVInfo *p_EventCSVInfo = new HeartBeatCSVInfo();
    p_EventCSVInfo->SetAction(Global::HEARTBEATACNTYPE_RESTART);
    p_EventCSVInfo->SetWaitDurationInHeartBeatTick(100);
    p_EventCSVInfo->SetThreadName("HeartBeat");
    p_EventCSVInfo->SetThreadMacroName("HB");
    p_EventCSVInfo->SetThreadId(21);

    QCOMPARE(p_EventCSVInfo->GetAction(), Global::HEARTBEATACNTYPE_RESTART);
    QCOMPARE(p_EventCSVInfo->GetWaitDurationInHeartBeatTick(),quint8(100));
    QCOMPARE(p_EventCSVInfo->GetThreadName(), QString("HeartBeat"));
    QCOMPARE(p_EventCSVInfo->GetThreadMacroName(),QString("HB"));
    QCOMPARE(p_EventCSVInfo->GetThreadId(), quint32(21));

    HeartBeatCSVInfo *p_EventCSVInfonew = new HeartBeatCSVInfo(23, "HB", "HeartBeat", Global::HEARTBEATACNTYPE_RESTART, 100);
    HeartBeatCSVInfo EventCSV(*p_EventCSVInfonew);

    delete p_EventCSVInfonew;
    delete p_EventCSVInfo;
}

void TestHeartBeatThread::utTestHeartbeatCommands()
{
    CmdAddControllerForHeartBeatCheck AddController(5000, 5);

    QVERIFY(AddController.GetName() == "HeartBeatManager::CmdAddControllerForHeartBeatCheck");
    QVERIFY(AddController.GetThreadId() == 5);


    CmdHeartBeat HeartBeat(5000, 5);

    QVERIFY(HeartBeat.GetName() == "HeartBeatManager::CmdHeartBeat");
    QVERIFY(HeartBeat.GetThreadId() == 5);


    CmdRemoveControllerForHeartBeatCheck RmController(5000, 5);

    QVERIFY(RmController.GetName() == "HeartBeatManager::CmdRemoveControllerForHeartBeatCheck");
    QVERIFY(RmController.GetThreadId() == 5);
}
}
QTEST_MAIN(HeartBeatManager::TestHeartBeatThread)

#include "TestHeartBeatThread.moc"

