#include <QApplication>
#include <QDebug>
#include "../Include/AgentController.h"
#include "../Include/LittleGui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LittleGui gui;
    if (!gui.Initialize()) {
        qDebug() << "ERROR: LittleGui init failed !";
        return false;
    }

    AgentController aController;
    if (!aController.Initialize(argc, argv)) {
        qDebug() << "ERROR: AgentController init failed !";
        return false;
    }

    if (!QObject::connect(&gui, SIGNAL(GoDown()), &aController, SLOT(Quit()))) {
        qDebug() << "ERROR: Cannot connect AgentController and Gui quit pair !";
        return false;
    }
    if (!QObject::connect(&gui, SIGNAL(SendAlarm()), &aController, SLOT(SendAlarm()))) {
        qDebug() << "ERROR: Cannot connect AgentController and Gui alarm pair !";
        return false;
    }
    if (!QObject::connect(&gui, SIGNAL(SendEvent()), &aController, SLOT(SendEvent()))) {
        qDebug() << "ERROR: Cannot connect AgentController and Gui event pair !";
        return false;
    }
    if (!QObject::connect(&gui, SIGNAL(UploadFile()), &aController, SLOT(UploadFile()))) {
        qDebug() << "ERROR: Cannot connect AgentController and Gui upload pair !";
        return false;
    }
    if (!QObject::connect(&gui, SIGNAL(UploadAnotherFile()), &aController, SLOT(UploadAnotherFile()))) {
        qDebug() << "ERROR: Cannot connect AgentController and Gui upload pair !";
        return false;
    }
    if (!QObject::connect(&gui, SIGNAL(UploadAllFiles()), &aController, SLOT(UploadTwoFiles()))) {
        qDebug() << "ERROR: Cannot connect AgentController and Gui upload pair !";
        return false;
    }
    if (!QObject::connect(&aController, SIGNAL(Finished()), &a, SLOT(quit()))) {
        qDebug() << "ERROR: Cannot connect AgentController and App !";
        return false;
    }

    aController.Start();
    gui.show();
    int result = a.exec();
    qDebug() << "Application exited with code :  " << result << "\n";
    return result;
}
