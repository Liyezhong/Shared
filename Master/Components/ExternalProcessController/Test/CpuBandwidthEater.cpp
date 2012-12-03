#include <CpuBandwidthEater.h>
#include <QCoreApplication>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

Calculator::Calculator() :
    m_Calculate(true)
{
}

Calculator::~Calculator() {
}

void Calculator::Calculate() {
    double squareroot = 0;

    while (m_Calculate) {
        squareroot = sqrt(rand());
    }
}

void Calculator::Stop() {
    m_Calculate = false;
}

/// \brief Constructor
/// \param runt   = time to run
/// \param sleept = time to sleep idly
CpuBandwidthEater::CpuBandwidthEater(int runt, int sleept) :
        m_myRunTime(runt),
        m_mySleepTime(sleept),
        m_myCalculator(NULL),
        m_Thread(NULL)
{
    printf("\nCpuBandwidthEater: ============== started ! ============== ");
}

CpuBandwidthEater::~CpuBandwidthEater()
{
    if (m_Thread != NULL) {
        delete m_Thread;
    }
    if (m_myCalculator != NULL) {
        delete m_myCalculator;
    }
    printf("\nCpuBandwidthEater: ============== destroyed ! ============== ");
}

void CpuBandwidthEater::Start() {
    OnSleepTimeout();
}

void CpuBandwidthEater::OnRunTimeout()
{
    m_myCalculator->Stop();
    m_Thread->exit();
    m_Thread->wait();
    if (m_Thread != NULL) {
        delete m_Thread;
    }
    if (m_myCalculator != NULL) {
        delete m_myCalculator;
    }

    QTimer::singleShot(m_mySleepTime, this, SLOT(OnSleepTimeout()));
}

void CpuBandwidthEater::OnSleepTimeout()
{
    m_myCalculator = new Calculator();
    m_Thread = new QThread();
    m_myCalculator->moveToThread(m_Thread);

    if (!QObject::connect(m_Thread, SIGNAL(started()), m_myCalculator, SLOT(Calculate()))) {
        return;
    }

    m_Thread->start();
    QTimer::singleShot(m_myRunTime, this, SLOT(OnRunTimeout()));
}

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    if (argc < 3) {
        printf("\n>> CpuBandwidthEater: not enough arguments!");
        printf("\n>> I need two: ");
        printf("\n    1). time to run (in ms)");
        printf("\n    2). time to sleep (in ms)\n\n");

        return -1;
    }

    int arg1 = atoi(argv[1]);
    int arg2 = atoi(argv[2]);

    printf("\n>>>>>> CpuBandwidthEater: ====== creating class... ====== \n");
    CpuBandwidthEater *eater = new CpuBandwidthEater(arg1, arg2);

    QThread mythread;

    if (!QObject::connect(&mythread, SIGNAL(started()), eater, SLOT(Start()))) {
        printf("\n>>>>>> CpuBandwidthEater: ====== cannot connect signal, exiting... ====== \n");
        return -1;
    }

    eater->moveToThread(&mythread);
    mythread.start();

    return a.exec();
}
