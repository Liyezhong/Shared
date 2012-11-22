#ifndef LITTLEGUI_H
#define LITTLEGUI_H

#include <QDialog>
#include <QPushButton>
#include <QBoxLayout>

class LittleGui : public QDialog
{
     Q_OBJECT

public:

    LittleGui(QWidget *pParent = NULL);
    ~LittleGui();
    bool Initialize();

signals:

    void GoDown();
    void SendAlarm();
    void SendEvent();
    void UploadFile();
    void UploadAnotherFile();
    void UploadAllFiles();

public slots:

    void Quit();

private:

    QPushButton   m_OkButton;
    QPushButton   m_AlarmButton;
    QPushButton   m_EventButton;
    QPushButton   m_UploadButton;
    QPushButton   m_Upload2Button;
    QPushButton   m_UploadAllButton;
    QBoxLayout   *m_Layout;
};

#endif // LITTLEGUI_H
