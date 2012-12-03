#include <QDebug>
#include "../Include/LittleGui.h"

LittleGui::LittleGui(QWidget *pParent) :
        QDialog(pParent)
{
}

LittleGui::~LittleGui()
{
}

bool LittleGui::Initialize()
{
    m_OkButton.setText("Quit");
    if (!QObject::connect(&m_OkButton, SIGNAL(clicked()), this, SLOT(Quit()))) {
        return false;
    }

    m_AlarmButton.setText("Send Alarm");
    if (!QObject::connect(&m_AlarmButton, SIGNAL(clicked()), this, SIGNAL(SendAlarm()))) {
        return false;
    }

    m_EventButton.setText("Send Event");
    if (!QObject::connect(&m_EventButton, SIGNAL(clicked()), this, SIGNAL(SendEvent()))) {
        return false;
    }

    m_UploadButton.setText("Upload File 1");
    if (!QObject::connect(&m_UploadButton, SIGNAL(clicked()), this, SIGNAL(UploadFile()))) {
        return false;
    }

    m_Upload2Button.setText("Upload File 2");
    if (!QObject::connect(&m_Upload2Button, SIGNAL(clicked()), this, SIGNAL(UploadAnotherFile()))) {
        return false;
    }

    m_UploadAllButton.setText("Upload 2 Files");
    if (!QObject::connect(&m_UploadAllButton, SIGNAL(clicked()), this, SIGNAL(UploadAllFiles()))) {
        return false;
    }

    try {
        m_Layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
        m_Layout->addWidget(&m_EventButton, 0, Qt::AlignHCenter|Qt::AlignVCenter);
        m_Layout->addWidget(&m_AlarmButton, 0, Qt::AlignHCenter|Qt::AlignVCenter);
        m_Layout->addWidget(&m_UploadButton, 0, Qt::AlignHCenter|Qt::AlignVCenter);
        m_Layout->addWidget(&m_Upload2Button, 0, Qt::AlignHCenter|Qt::AlignVCenter);
        m_Layout->addWidget(&m_UploadAllButton, 0, Qt::AlignHCenter|Qt::AlignVCenter);
        m_Layout->addSpacing(20);
        m_Layout->addWidget(&m_OkButton, 0, Qt::AlignHCenter|Qt::AlignVCenter);
        this->setLayout(m_Layout);
    }
    catch (std::bad_alloc) {
        qDebug() << "\nERROR: cannot allocate memory !\n";
        return false;
    }

    return true;
}

void LittleGui::Quit()
{
    emit GoDown();
    this->hide();
}
