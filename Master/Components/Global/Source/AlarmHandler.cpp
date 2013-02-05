/****************************************************************************/
/*! \file ColoradoMasterThread/Source/AlarmHandler.cpp
 *
 *  \brief Implementation file for class AlarmHandler.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-08-07
 *  $Author:    $ Michael Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <Global/Include/AlarmHandler.h>
#include <QDebug>
#include <QProcess>
//#include <QSound>

namespace Global {

/****************************************************************************/
AlarmHandler::AlarmHandler(quint16 timeout)
    : m_volume(1)
    , m_processPlay(NULL)
    , m_processSetVolume(NULL)
    , m_alarmToneTimer(NULL)
{


    qDebug() << "AlarmHandler::emitAlarm" ;
    //moveToThread( this );
    m_mutex = new QMutex();

    m_Timer = new QTimer();
    m_Timer->setInterval(timeout);

    m_alarmToneTimer = new QTimer();
    connect(m_alarmToneTimer, SIGNAL(timeout()), this, SLOT(StopPlayAlarm()));

    connect(m_Timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_Timer->start();
}

/****************************************************************************/
AlarmHandler::~AlarmHandler()
{
    delete m_Timer;
    delete m_mutex;
    delete m_alarmToneTimer;
}

void AlarmHandler::onTimeout()
{
    m_mutex->lock();
    m_Timer->stop();

    if (!(m_errorList.size() == 0))
    {
        qDebug() << "AlarmHandler::emitAlarm" ;
        emitAlarm(Global::ALARM_ERROR);
    }
    else
    {
        if (!(m_warningList.size() == 0))
        {
            qDebug() << "AlarmHandler::emitAlarm" ;
            emitAlarm(Global::ALARM_WARNING);
        }
    }

    m_mutex->unlock();
    m_Timer->start();
}

void AlarmHandler::emitAlarm (Global::AlarmType alarmType, bool Active, QString Filename, quint8 AlarmVolume  )
{
    QString soundFile = ((Active) ? m_soundList.value(alarmType, NULL) : Filename);
    if (soundFile.length() == 0)
    {
        qDebug() << "AlarmHandler::emitAlarm" << "No sound file defined for alarm type " << alarmType;
        return;
    }

    double volumeLevel = ((Active) ? (( 0.1 + 0.15*m_volume )*100) : ((0.1 + 0.15*AlarmVolume )*100));   // m_volume=6 => volumeLevel=1

    if (!m_processPlay)
    {
        m_processPlay = new QProcess();
        qDebug() << "Creating a new Process";
    }
    else
    {
        if (m_processPlay->state() == QProcess::Running)
        {
            qDebug() << "Process Running";
            m_processPlay->kill();
            qDebug()<<"Process Killed";
            m_processPlay->waitForFinished();
        }
    }

    qDebug() << "ogg123" + /*QString::number(volumeLevel, 'g', 1) + " " +*/ soundFile;
    QString SetVolume= "amixer sset 'PCM',0 " + QString::number(volumeLevel) +"%";
    m_processSetVolume = new QProcess();
    m_processSetVolume->start(SetVolume);
    QString program = "ogg123 " + /*QString::number(volumeLevel, 'g', 1) + " " +*/ soundFile;
    m_processPlay->start(program);


}
bool AlarmHandler::playTestTone(bool AlarmTypeFlag, quint8 AlarmNumber, quint8 AlarmVolume)
{
    if ((!(m_errorList.size() == 0)) || (!(m_warningList.size() == 0)))
    {
        qDebug() << "Process is already running";
        return false;
    }

    else
    {   connect(m_processPlay,SIGNAL(finished(int)),this,SLOT(StopPlayAlarm()));
        m_alarmToneTimer->start(3000);

        quint8 number=AlarmNumber;
        QString FileName = "";

        if ( !AlarmTypeFlag )
        {
            QString FileName = Global::SystemPaths::Instance().GetSoundPath() + "/Alarm" + QString::number(number) + ".ogg";
            qDebug() << FileName;
            emitAlarm(Global::ALARM_ERROR, false, FileName, AlarmVolume);
        }
        else
        {
            QString FileName = Global::SystemPaths::Instance().GetSoundPath() + "/Note" + QString::number(number) + ".ogg";
            qDebug() << FileName;
            emitAlarm(Global::ALARM_WARNING, false, FileName, AlarmVolume);
        }
    }


    return true;
}

void AlarmHandler::StopPlayAlarm()
{
    if ( m_processPlay->state() == QProcess::NotRunning)
    {
        qDebug() << "Process completed...";
        m_alarmToneTimer->stop();
        disconnect(m_processPlay,SIGNAL(finished(int)),this,SLOT(StopPlayAlarm()));
    }
    else if (m_processPlay->state() == QProcess::Running)
    {

        qDebug() << "Process Running...";
        m_processPlay->kill();
        qDebug()<<"Process Killed....";
        m_processPlay->waitForFinished();
        m_alarmToneTimer->stop();
        disconnect(m_processPlay,SIGNAL(finished(int)),this,SLOT(StopPlayAlarm()));

    }

//    m_alarmToneTimer = NULL;


}

void AlarmHandler::setTimeout(quint16 timeout)
{
    m_Timer->stop();
    m_mutex->lock();

    m_Timer->setInterval(timeout);

    m_mutex->unlock();
    m_Timer->start();
}

void AlarmHandler::setVolume(Global::AlarmType alarmType, quint8 volume)
{
    m_mutex->lock();
    m_volumeList.insert(alarmType,volume);
    m_mutex->unlock();
}

void AlarmHandler::setSoundNumber(Global::AlarmType alarmType, int number)
{
    //Global::SystemPaths SysPath;
    Q_UNUSED(number);
    QString fileName = "";

    if (alarmType == Global::ALARM_ERROR)
    {
        fileName =  Global::SystemPaths::Instance().GetSoundPath() + "/Alarm" + QString::number(number) + ".ogg";
    }
    else
    {
        fileName =  Global::SystemPaths::Instance().GetSoundPath() + "/Note" + QString::number(number) + ".ogg";
    }
    if (fileName.length() > 0)
    {
        setSoundFile(alarmType, fileName);
        qDebug() << "AlarmHandler SoundFile is" << fileName;
    }

}

void AlarmHandler::setSoundFile(Global::AlarmType alarmType, QString fileName)
{
    if (fileName.length() == 0)
    {
        qDebug() << "AlarmHandler::setSoundFile, no file specified";
        return;
    }

    m_mutex->lock();
    m_soundList.insert(alarmType, fileName);
    m_mutex->unlock();
}

void AlarmHandler::setAlarm(quint64 eventKey, Global::AlarmType alarmType, bool active)
{
    m_mutex->lock();
    if (active)
    {
        if (alarmType == Global::ALARM_ERROR)
        {
           m_errorList.insert(eventKey, alarmType);
        }
        else if (alarmType == Global::ALARM_WARNING)
        {
          m_warningList.insert(eventKey, alarmType);
        }

    }
    else
    {
        if (alarmType == Global::ALARM_ERROR)
        {
            m_errorList.remove(eventKey);
        }
        else if (alarmType == Global::ALARM_WARNING)
        {
           m_warningList.remove(eventKey);
        }

    }
    m_mutex->unlock();
}

void AlarmHandler::reset()
{
    m_mutex->lock();
    m_errorList.clear();
    m_mutex->unlock();
}

} // end namespace Global
