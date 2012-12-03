#ifndef AGENTCONTROLLER_H
#define AGENTCONTROLLER_H

#include <QObject>
#include <QThread>
#include "AgentWrapper.h"

class AgentController : public QObject
{
    Q_OBJECT

public:

    AgentController(QObject *pParent = NULL);
    ~AgentController();
    bool Initialize(int argc, char *argv[]);

signals:

    void Finished();

public slots:

    void Start();
    void Quit();
    void SendAlarm();
    void SendEvent();
    void UploadFile();
    void UploadAnotherFile();
    void UploadTwoFiles();

private slots:

    void Exit();

private:

    AgentWrapper   *m_Agent;
    QThread         m_AgentThread;
};

#endif // AGENTCONTROLLER_H
