/****************************************************************************/
/*! \file Global/Include/AlarmHandler.h
 *
 *  \brief Definition file for class AlarmHandler.
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

#ifndef GLOBAL_ALARMHANDLER_H
#define GLOBAL_ALARMHANDLER_H

#include <QObject>
#include <QHash>
#include <QTimer>
#include <QMutex>
#include <Global/Include/GlobalDefines.h>

namespace Platform {

/****************************************************************************/
/**
 * \brief Alarm handler for Colorado application.
 */
/****************************************************************************/
class AlarmHandler : public QObject
{
    Q_OBJECT

public:
    AlarmHandler(quint16 timeout, QString soundPath);
    ~AlarmHandler();

    /**
      @brief Includes or removes an alarm into the queue of raised alarms
      */
    void setAlarm(quint64 eventKey, Global::AlarmType alarmType, bool active);

    /**
      @brief Removes all alarms from alarm queue
      */
    void reset();

    /**
      @brief Specifies the timeout, i.e. the time interval inbetween two alarm sounds
      */
    void setTimeout(quint16 timeout);

    /**
      @brief Specifies the alarm volume related to a specific alarm type
      *1 = lowest
      *6 = normal
      *9 = highest
      */
    void setVolume(Global::AlarmType alarmType, quint8 volume);

    /**
      @brief Specifies the sound file to be played when an alarm of type alarmType is raised
      */
    void setSoundFile(Global::AlarmType alarmType, QString fileName);

    /**
      @brief Specifies the sound-id related to a specific alarm type
      */
    void setSoundNumber(Global::AlarmType alarmType, int number);

private:
    QHash<quint64, Global::AlarmType> m_errorList;
    QHash<Global::AlarmType, QString> m_soundList;
    QHash<Global::AlarmType, quint8> m_volumeList;
    QTimer* m_Timer;
    QMutex* m_mutex;
    void emitAlarm(Global::AlarmType alarmType);

    quint8 m_volume;
    QString m_soundPath;

private slots:
    void onTimeout();

}; // end class

} // end namespace Platform

#endif // GLOBAL_ALARMHANDLER_H
