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

namespace Platform {

/****************************************************************************/
AlarmHandler::AlarmHandler(quint16 timeout, QString soundPath)
    : m_volume(1)
    , m_soundPath(soundPath)
    , m_process(NULL)
{
    //moveToThread( this );
    m_mutex = new QMutex();

    m_Timer = new QTimer();
    m_Timer->setInterval(timeout);

    connect(m_Timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_Timer->start();
}

/****************************************************************************/
AlarmHandler::~AlarmHandler()
{
    delete m_Timer;
    delete m_mutex;
}

void AlarmHandler::onTimeout()
{
    m_Timer->stop();
    m_mutex->lock();
    //quint8 Volume = 1;
    //QString Filename = QString("");

    //Global::AlarmType alarmType = Global::ALARM_NONE;
    Global::AlarmType alarmType;
    if (!(m_errorList.size() == 0))
    {
        emitAlarm(alarmType);
    }
    else
    {
        if (!(m_warningList.size() == 0))
        {
            emitAlarm(alarmType);
        }
    }

    m_mutex->unlock();
    m_Timer->start();
}

void AlarmHandler::emitAlarm (Global::AlarmType alarmType, bool Active, QString Filename, quint8 Volume  )
{
    // alarmType = ((AlarmTypeFlag) ? Global::ALARM_WARNING : Global::ALARM_ERROR );

    QString soundFile = ((Active) ? m_soundList.value(alarmType, QString("")) : Filename);
    if (soundFile.length() == 0)
    {
        qDebug() << "AlarmHandler::emitAlarm" << "No sound file defined for alarm type " << alarmType;
        return;
    }

    double volumeLevel = ((Active) ? ( 0.1 + 0.15*m_volume ) : (0.1 + 0.15*Volume ));   // m_volume=6 => volumeLevel=1

    if (!m_process)
    {
        m_process = new QProcess();
        qDebug() << "Creating a new Process";
    }
    else
    {
        if (m_process->state() == QProcess::Running)
        {
            qDebug() << (m_process->state() == QProcess::Running);
            m_process->terminate();
        }
    }

    qDebug() << "play -v " + QString::number(volumeLevel, 'g', 1) + " " + soundFile;
    QString program = "play -v " + QString::number(volumeLevel, 'g', 1) + " " + soundFile;
    m_process->start(program);


}
void AlarmHandler::playTestTone(bool AlarmTypeFlag, quint8 Volume, quint8 Sound)
{
    //Q_UNUSED(number);
    quint8 number=Sound;
    QString FileName = QString("");
    Global::AlarmType alarmType;
    if ( !AlarmTypeFlag )
    {
        alarmType == Global::ALARM_ERROR;
        QString FileName = Global::SystemPaths::Instance().GetSoundPath() + "/Alarm" + QString::number(number) + ".wav";
        qDebug() << FileName;
        emitAlarm(Global::ALARM_ERROR, false, FileName, Volume);
    }
    else
    {
        alarmType == Global::ALARM_WARNING;
        QString FileName = Global::SystemPaths::Instance().GetSoundPath() + "/Note" + QString::number(number) + ".wav";
        qDebug() << FileName;
        emitAlarm(Global::ALARM_WARNING, false, FileName, Volume);
    }
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
   // m_soundPath="";
   //m_soundPath="/mnt/hgfs/SVN_HOME/trunk/Colorado/ColoradoMain/Master/Components/Main/Build/bin_dbg/";
    if (alarmType == Global::ALARM_ERROR)
        fileName = Global::SystemPaths::Instance().GetSoundPath() + "SoundE" + QString::number(number) + ".wav";
    else
        fileName = Global::SystemPaths::Instance().GetSoundPath() + "SoundW" + QString::number(number) + ".wav";

    if (fileName.length() > 0)
        setSoundFile(alarmType, fileName);
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
        else
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
        else
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

} // end namespace Colorado
