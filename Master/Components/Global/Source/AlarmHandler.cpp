/****************************************************************************/
/*! \file Global/Source/AlarmHandler.cpp
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
#include <Global/Include/EventObject.h>
#include <Global/Include/AlarmPlayer.h>
#include <QDebug>
#include <QProcess>

namespace Global {

/****************************************************************************/
AlarmHandler::AlarmHandler(QObject *p_Parent, quint16 TimeOut)
    : QObject(p_Parent)
    , m_WarnPeriodOn(true)
    , m_WarnPeriod(0)

{
    m_Timer = new QTimer(this);
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_Timer->start(TimeOut);
    qRegisterMetaType<Global::AlarmType>("Global::AlarmType");
    connect(this, SIGNAL(emitAlarm(Global::AlarmType)),
            &Global::AlarmPlayer::Instance(), SLOT(PlayAlarm(Global::AlarmType)));
}

/****************************************************************************/
AlarmHandler::~AlarmHandler()
{
}

void AlarmHandler::onTimeout()
{
    if (!(m_errorList.size() == 0))
    {
        emitAlarm(Global::ALARM_ERROR);
    }
    else if (!(m_warningList.size() == 0))
    {
        emitAlarm(Global::ALARM_WARNING);
    }
}

void AlarmHandler::setTimeout(quint16 timeout)
{
    qDebug() << "AlarmHandler::setTimeout: alarm interval now is: " << timeout << " mseconds.";
    if (timeout <= 0)
        return ;

    m_Timer->stop();
    m_Timer->setInterval(timeout);
    m_Timer->start();
}

void AlarmHandler::setWarnPeriod(bool onoff)
{
    this->m_WarnPeriodOn = onoff;
}

void AlarmHandler::setWarnPeriodInterval(qint32 interval)
{
    qDebug() << "AlarmHandler::setWarnPeriodInterval: interval=" << interval;
    this->m_WarnPeriod = interval;
    if (m_WarnPeriodOn && m_warningList.size() > 0) {
        if (m_errorList.size() < 1)
            this->setTimeout(m_WarnPeriod * 60 * 1000);
    }
}

void AlarmHandler::setAlarm(quint64 eventKey, Global::AlarmType alarmType, bool active)
{
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
}

void AlarmHandler::reset()
{
    m_errorList.clear();
    m_warningList.clear();
}

} // end namespace Global
