/****************************************************************************/
/*! \file Global/Source/AlarmPlayer.cpp
 *
 *  \brief Implementation file for class AlarmPlayer
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-08-17
 *  $Author:    $ N.Kamath
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

#include <Global/Include/AlarmPlayer.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/EventObject.h>


namespace Global {

const quint32  MAX_VOLUME = 9;       //!< MAX volume of alarm

AlarmPlayer::AlarmPlayer()
    : QObject(0)
    , m_volumeError(2)
    , m_volumeWarning(0)
    , m_processPlay(NULL)
    , m_processSetVolume(NULL)
    , m_Mutex(QMutex::Recursive)
{
    qRegisterMetaType<Global::AlarmType>("Global::AlarmType");
}



bool AlarmPlayer::playTestTone(bool AlarmTypeFlag, quint8 AlarmVolume, quint8 AlarmNumber)
{
    if ( AlarmTypeFlag ) {
        QString FileName = Global::SystemPaths::Instance().GetSoundPath() + "/Alarm" + QString::number(AlarmNumber) + ".ogg";
        if (QFile::exists(FileName)) {
            emitAlarm(Global::ALARM_ERROR, false, FileName, AlarmVolume);
        }
    }
    else {
        QString FileName = Global::SystemPaths::Instance().GetSoundPath() + "/Note" + QString::number(AlarmNumber) + ".ogg";
        if (QFile::exists(FileName)) {
            emitAlarm(Global::ALARM_WARNING, false, FileName, AlarmVolume);
        }
    }
    return true;
}

void AlarmPlayer::setVolume(Global::AlarmType alarmType, quint8 volume)
{
    QMutexLocker Lock(&m_Mutex);
    if (alarmType == Global::ALARM_ERROR) {
        m_volumeError = volume;
    }
    else if (alarmType == Global::ALARM_WARNING) {
        m_volumeWarning = volume;
    }
}

void AlarmPlayer::setSoundFile(Global::AlarmType alarmType, QString fileName)
{
    if (fileName.length() == 0) {
        return;
    }

    QMutexLocker Lock(&m_Mutex);
    m_soundList.insert(alarmType, fileName);
}

void AlarmPlayer::setSoundNumber(Global::AlarmType alarmType, int number)
{
    QString fileName;

    if (alarmType == Global::ALARM_ERROR) {
        fileName =  Global::SystemPaths::Instance().GetSoundPath() + "/Alarm" + QString::number(number) + ".ogg";
    }
    else {
        fileName =  Global::SystemPaths::Instance().GetSoundPath() + "/Note" + QString::number(number) + ".ogg";
    }
    if (QFile::exists(fileName)) {
        setSoundFile(alarmType, fileName);
        qDebug() << "AlarmHandler SoundFile is" << fileName;
    }
}

void AlarmPlayer::emitAlarm(Global::AlarmType alarmType, bool UsePresetValues, QString Filename, quint8 Volume)
{
    qDebug()<<"Alarm Player Thread" << this->thread();
    quint8 ActiveVolume;
    { //Mutex scope start
        QMutexLocker Lock(&m_Mutex);
        if (!m_soundList.contains(alarmType)) {
            Global::EventObject::Instance().RaiseEvent(EVENT_ALARM_SOUND_MISSING);
            return;
        }

        if (alarmType == Global::ALARM_ERROR) {
            ActiveVolume = m_volumeError;
        }
        else {
            ActiveVolume = m_volumeWarning;
        }
    }//Mutex scope ends

    quint8 volume = 0;
    if (UsePresetValues)
        volume = ActiveVolume;
    else
        volume = Volume;

    if (volume > MAX_VOLUME)
        volume = MAX_VOLUME;

    double volumeRatio = ((float)volume + 1.0) * 0.04000 + 0.60000;
    double volumeLevel = volumeRatio * 100.0;
    //double volumeLevel = ((UsePresetValues) ? (((float)ActiveVolume/(float)MAX_VOLUME) * 100) : ((Volume/(float)MAX_VOLUME ) * 100));

    QString SetVolume= "amixer set PCM " + QString::number(volumeLevel) +"%";
    if (!m_processSetVolume) {
        m_processSetVolume = new QProcess();
    }
    m_processSetVolume->start(SetVolume);
    m_processSetVolume->waitForFinished();

    QString soundFile = ((UsePresetValues) ? m_soundList.value(alarmType) : Filename);

    if (!m_processPlay)
    {
        m_processPlay = new QProcess();
    }
    else
    {
        if (m_processPlay->state() == QProcess::Running)
        {
            m_processPlay->kill();
            m_processPlay->waitForFinished();
        }
    }

    QString program = "ogg123 " + soundFile;
    m_processPlay->start(program);
    m_processPlay->waitForFinished();
}

void AlarmPlayer::PlayAlarm(Global::AlarmType AlarmTypeVal)
{
    emitAlarm(AlarmTypeVal);
}

} //End of namespace Global
