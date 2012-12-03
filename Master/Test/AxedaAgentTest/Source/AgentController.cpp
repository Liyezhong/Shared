#include <QDebug>
#include "../Include/AgentController.h"

AgentController::AgentController(QObject *pParent) :
        QObject(pParent),
        m_Agent(NULL)
{
}

AgentController::~AgentController()
{
    if (m_Agent != NULL) {
        delete m_Agent;
        m_Agent = NULL;
    }
}

bool AgentController::Initialize(int argc, char *argv[])
{
    try {
        m_Agent = new AgentWrapper();
        if (!m_Agent->Initialize(argc, argv)) {
            qDebug() << "ERROR: AgentWrapper init failed !";
            return false;
        }

        if (!QObject::connect(&m_AgentThread, SIGNAL(started()), m_Agent, SLOT(run()))) {
            qDebug() << "AgentController ERROR: Cannot connect Thread and Agent !";
            return false;
        }
        if (!QObject::connect(m_Agent, SIGNAL(Finished()), &m_AgentThread, SLOT(quit()))) {
            qDebug() << "AgentController ERROR: Cannot connect Agent and Thread !";
            return false;
        }
        if (!QObject::connect(m_Agent, SIGNAL(Finished()), this, SLOT(Exit()))) {
            qDebug() << "AgentController ERROR: Cannot connect Agent's exit signal !";
            return false;
        }

        m_Agent->moveToThread(&m_AgentThread);
    }
    catch(const std::bad_alloc &) {
        qDebug() << "ERROR: Cannot allocate memory !";
        return false;
    }

    return true;
}

void AgentController::Start()
{
    m_AgentThread.start();
}

void AgentController::Quit()
{
    m_Agent->stop();
    //qDebug() << "AgentController: Quit is called.";
}

void AgentController::Exit()
{
    //qDebug() << "AgentController: Exit is called.";
    m_AgentThread.wait();
    emit Finished();
}

void AgentController::SendAlarm()
{
    m_Agent->SetupAlarm();
}

void AgentController::SendEvent()
{
    m_Agent->SetupEvent();
}

void AgentController::UploadFile()
{
    m_Agent->SetupUpload();
}

void AgentController::UploadAnotherFile()
{
    m_Agent->SetupAnotherUpload();
}

void AgentController::UploadTwoFiles()
{
    m_Agent->SetupTwoUploads();
}
