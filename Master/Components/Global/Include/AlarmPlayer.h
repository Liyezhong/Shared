/****************************************************************************/
/*! \file Global/Include/AlarmPlayer.h
 *
 *  \brief Definition file for class AlarmPlayer
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
#ifndef GLOBAL_ALARMPLAYER_H
#define GLOBAL_ALARMPLAYER_H

#include <QObject>
#include <QProcess>
#include <QMutex>

#include <Global/Include/GlobalDefines.h>

namespace Global {
/****************************************************************************/
/**
 * \brief Alarm Player
 */
/****************************************************************************/
class AlarmPlayer : public QObject
{
    Q_OBJECT

public:
    /****************************************************************************/
    /**
     * \brief       Get reference to instance.
     *
     * \return      Reference to instance.
     */
    /****************************************************************************/
    static AlarmPlayer &Instance()
    {
        static AlarmPlayer m_Instance; //!< One and only instance of AlarmPlayer
        return m_Instance;
    }


    /****************************************************************************/
    /**
     * \brief       Specifies the alarm volume related to a specific alarm type
     *              1 = lowest
     *              6 = normal
     *              9 = highest
     *
     * \iparam      alarmType = type of alarm
     * \iparam      volume = alarm volume
     */
    /****************************************************************************/
    void setVolume(Global::AlarmType alarmType, quint8 volume);

    /****************************************************************************/
    /**
     * \brief       Specifies the sound file to be played when an alarm of type alarmType is raised
     *
     * \iparam      alarmType = type of alarm
     * \iparam      fileName = sound file name
     */
    /****************************************************************************/
    void setSoundFile(Global::AlarmType alarmType, QString fileName);

    /****************************************************************************/
    /**
     * \brief Specifies the sound-id related to a specific alarm type
     *
     * \iparam      alarmType = type of alarm
     * \iparam      number = sound number of alarm
     */
    /****************************************************************************/
    void setSoundNumber(Global::AlarmType alarmType, int number);


private:

    quint8 m_volumeError; //!< Error volume
    quint8 m_volumeWarning; //!< Warning volume
    quint8 m_volumeInfo; //!< info volume
    QProcess* m_processPlay; //!< Process to play tone
    QProcess* m_processSetVolume; //!< Process to set volume
    QHash<Global::AlarmType, QString> m_soundList; //!< List for storing alarm file names
    QMutex m_Mutex; //!< Lock for thread safety

    AlarmPlayer();
    /****************************************************************************/
    /**
     * \brief Disable copy and assignment
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(AlarmPlayer)
    void emitAlarm (Global::AlarmType alarmType, bool UsePresetValues = true, QString Filename = "", quint8 Volume = 1);

private slots:
    /****************************************************************************/
    /**
     * \brief plays alarm
     *
     * \iparam      AlarmTypeVal = type of alarm
     */
    /****************************************************************************/
    void PlayAlarm(Global::AlarmType AlarmTypeVal);

public slots:
    /****************************************************************************/
    /**
     * \brief plays test tone
     *
     * \iparam      alarmType = alarm type, it may be error, warning or infor
     * \iparam      Volume = volume
     * \iparam      AlarmNumber = alarm number
     */
    /****************************************************************************/
    bool playTestTone(Global::AlarmType alarmType, quint8 Volume, quint8 AlarmNumber);


}; //End of class AlarmPlayer

} // end of namespace Global

#endif // GLOBAL_ALARMPLAYER_H
