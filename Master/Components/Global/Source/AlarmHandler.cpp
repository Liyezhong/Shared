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

namespace Platform {

/****************************************************************************/
AlarmHandler::AlarmHandler(quint16 timeout, QString soundPath)
    : m_volume(1)
    , m_soundPath(soundPath)
{
    m_mutex = new QMutex();

    m_Timer = new QTimer();
    m_Timer->setInterval(timeout);

    connect(m_Timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_Timer->start();
}

/****************************************************************************/
AlarmHandler::~AlarmHandler()
{
}

void AlarmHandler::onTimeout()
{
    m_Timer->stop();
    m_mutex->lock();

    Global::AlarmType alarmType = Global::ALARM_NONE;
    QHash<quint16, Global::AlarmType>::iterator i;
    for (i = m_errorList.begin(); i != m_errorList.end(); ++i)
    {
        if ((i.value() == Global::ALARM_ERROR) || ((i.value() == Global::ALARM_WARNING) && (alarmType != Global::ALARM_ERROR)))
        {
            alarmType = i.value();
        }
    }
    emitAlarm(alarmType);

    m_mutex->unlock();
    m_Timer->start();
}

void AlarmHandler::emitAlarm(Global::AlarmType alarmType)
{
    if (alarmType == Global::ALARM_NONE)
        return;

    QString soundFile = m_soundList.value(alarmType, QString(""));

    if (soundFile.length() == 0)
    {
        qDebug() << "AlarmHandler::emitAlarm" << "No sound file defined for alarm type " << alarmType;
        return;
    }

    double volumeLevel = 0.1 + 0.15*m_volume;   // m_volume=6 => volumeLevel=1

    QProcess *process = new QProcess(this);
//    qDebug() << "play -v " + QString::number(volumeLevel, 'g', 1) + " " + soundFile;
    QString program = "play -v " + QString::number(volumeLevel, 'g', 1) + " " + soundFile;
    process->start(program);
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
    Q_UNUSED(number);
    QString fileName = "";
    m_soundPath="/mnt/hgfs/SVN_HOME/trunk/Colorado/ColoradoMain/Master/Components/Main/Build/bin_dbg/";
    if (alarmType == Global::ALARM_ERROR)
        fileName = m_soundPath + "SoundE" + QString::number(number) + ".wav";
    else
        fileName = m_soundPath + "SoundW" + QString::number(number) + ".wav";

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

void AlarmHandler::setAlarm(quint16 eventId, Global::AlarmType alarmType, bool active)
{
    m_mutex->lock();
    if (active)
    {
        m_errorList.insert(eventId, alarmType);
    }
    else
    {
        m_errorList.remove(eventId);
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
