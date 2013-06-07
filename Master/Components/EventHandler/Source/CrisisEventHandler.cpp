/****************************************************************************/
/*! \file CrisisEventHandler.cpp
 *
 *  \brief Definition file for class CrisisEventHandler
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-28
 *  $Author:    $ Abe Yang
 *
 *  \b Description:
 *
 *       In Himalaya project, when some events happenning, we should consider 
 *       the current system state in different sates, we have 
 *       different processing.
 *       In this design, we apply this pattern to get an error code:
 *       CrisisEventId + StateId = ErrorId 
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <EventHandler/Include/CrisisEventHandler.h>
#include <QFile>
#include <QTextStream>
#include <qstringlist.h>
#include <Global/Include/EventObject.h>


namespace EventHandler {

CrisisEventHandler CrisisEventHandler::m_EventHandlerInstance; //!< The instance

CrisisEventHandler::CrisisEventHandler():
  //m_currentSysState(520000293)
    m_currentSysState(520000001)
{
}

CrisisEventHandler::~CrisisEventHandler()
{
}

void CrisisEventHandler::currentSysState(quint32 stateId)
{
    m_currentSysState = stateId;
}

bool CrisisEventHandler::readEventStateConf(const QString filename)
{
    QFile file(filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
        return false;

    m_EventStateErrorHash.clear();

    QTextStream stream ( &file );
    QString line;

    //required once the event is in table, even if nothing is specified in the required column
    while( !stream.atEnd() )
    {
        line = stream.readLine();
        if (line.left(1) != "#")//ignor the first line(header line)
        {
            QStringList textList = line.split(",");
            if (textList.count() > 0)
            {
                if(!textList.at(0).isEmpty())
                {
                    quint32 eventId = textList.at(0).toInt();
                    QStringList stateList = textList.at(2).trimmed().toUpper().split(";");
                    quint32 errorId = textList.at(3).trimmed().toInt();
                    m_EventStateErrorHash.insert(eventId,qMakePair(stateList, errorId));
                }
            } 
        }
    }
    return true;
}
    
quint32 CrisisEventHandler::findErrorCode(quint32 eventId)
{
    QHash< quint32, QPair<QStringList,quint32> >::iterator i = m_EventStateErrorHash.find(eventId);
    while (i != m_EventStateErrorHash.end() && i.key() == eventId) {
        QPair<QStringList,quint32> value = i.value();
        QStringList stateList = value.first;
        QString strCurrentSysState = QString::number(m_currentSysState);
        if (stateList.contains(strCurrentSysState))
        {
            return value.second;
        }
        ++i;
    }
    return 0;
}

quint32 CrisisEventHandler::findErrorCode(quint32 eventId, quint32 Scenario)
{
    QHash< quint32, QPair<QStringList,quint32> >::iterator i = m_EventStateErrorHash.find(eventId);
    while (i != m_EventStateErrorHash.end() && i.key() == eventId) {
        QPair<QStringList,quint32> value = i.value();
        QStringList stateList = value.first;
        QString strCurrentSysState = QString::number(Scenario);
        if (stateList.contains(strCurrentSysState))
        {
            return value.second;
        }
        ++i;
    }
    return 0;
}


QStringList& CrisisEventHandler::RaiseLog()
{
    if(OneLogInfo.size() >0 )
    {
        QString info("");
        for(QStringList::const_iterator it = OneLogInfo.constBegin(); it != OneLogInfo.constEnd(); it++)
        {
            info += " " + *it;
        }
        Global::EventObject::Instance().RaiseLog(info);
        OneLogInfo.clear();
    }
    return OneLogInfo;
}
} // end namespace EventHandler

