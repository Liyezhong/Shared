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
#include <QProcess>

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/SystemPaths.h>

namespace Global {
//const quint32 DEFAULT_ALARM_TIMEOUT = 10000; //!< 10 seconds
const quint32 DEFAULT_ALARM_TIMEOUT = 5000; //!< 5 seconds for device alarm timeout
/****************************************************************************/
/**
 * \brief This class manages Alarms generated in response to request from
 *        event handler
 */
/****************************************************************************/
class AlarmHandler : public QObject
{
    Q_OBJECT

private:
    /****************************************************************************/
    /**
     * \brief constructor
     *
     * \iparam p_Parent = parent object
     * \iparam TimeOut = alarm timeout
     *
     */
    /****************************************************************************/
    AlarmHandler(QObject *p_Parent = NULL, quint16 TimeOut = DEFAULT_ALARM_TIMEOUT);
    ~AlarmHandler();

public:
    /****************************************************************************/
    /**
     * \brief Includes or removes an alarm into the queue of raised alarms
     *
     * \iparam eventKey = event key
     * \iparam alarmType = type of alarm
     * \iparam active = true if event is active, else false
     *
     */
    /****************************************************************************/
    void setAlarm(quint64 eventKey, Global::AlarmType alarmType, bool active);

    /****************************************************************************/
    /**
     * \brief Removes all alarms from alarm queue
     */
    /****************************************************************************/
    void reset();

    /****************************************************************************/
    /**
     * \brief Specifies the timeout, i.e. the time interval inbetween two alarm sounds
     *
     * \iparam timeout = time out for alarm
     */
    /****************************************************************************/
    void setTimeout(quint16 timeout);

    /****************************************************************************/
    /**
     * \brief Switch for warning alarm period
     *
     * \iparam onoff = on off flag
     */
    /****************************************************************************/
    void setWarnPeriod(bool onoff);

    /****************************************************************************/
    /**
     * \brief Switch for warning alarm period
     *
     * \iparam interval = period time, minutes
     */
    /****************************************************************************/
    void setWarnPeriodInterval(qint32 interval);


    /****************************************************************************/
    /**
     * \brief  Get reference to instance.
     *
     * \return Reference to instance.
     */
    /****************************************************************************/
    static AlarmHandler &Instance() {
        static AlarmHandler m_Instance;         ///< The instance.
        return m_Instance;
    }

private:
    QHash<quint64, Global::AlarmType> m_errorList;      ///< list of alarms for errors
    QHash<quint64, Global::AlarmType> m_warningList;    ///< list of alarms for warnings
    QTimer* m_Timer;                                    ///< timer

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(AlarmHandler)

private:
    bool m_WarnPeriodOn;   ///< warning period flag
    quint32 m_WarnPeriod;  ///< warning period interval

private slots:
    /****************************************************************************/
    /**
     * \brief slot called when time out occurs
     */
    /****************************************************************************/
    void onTimeout();

signals:
    /****************************************************************************/
    /**
     * \brief emit alarm signal
     */
    /****************************************************************************/
    void emitAlarm(Global::AlarmType);

}; // end class

} // end namespace Global

#endif // GLOBAL_ALARMHANDLER_H
