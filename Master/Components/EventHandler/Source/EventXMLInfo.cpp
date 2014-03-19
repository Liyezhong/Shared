
/****************************************************************************/
/*! \file EventHandler/Source/EventXMLInfo.cpp
 *
 *  \brief Implementation file for class EventXMLInfo.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-26-12
 *  $Author:    $ Songtao Yu 
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2013 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <EventHandler/Include/EventXMLInfo.h>
#include <Global/Include/GlobalDefines.h>
#include <QFile>
using namespace Global;

namespace EventHandler {

/****************************************************************************/
EventXMLInfo::EventXMLInfo(const QStringList& eventXMLFileList, const QString& ESEXMLFile)
    : m_eventXMLFileList(eventXMLFileList),
      m_ESEXMLFile(ESEXMLFile),
      m_pXMLReader(NULL),
      m_pESEXMLInfo(NULL)
{
}

bool EventXMLInfo::InitXMLInfo()
{
    m_ParsingStatus = false; // set parsing status false

    // For EventList configuration file list, we store them into m_pXMLEventList
    QStringList::iterator iter = m_eventXMLFileList.begin();
    for (; iter !=m_eventXMLFileList.end(); ++iter)
    {
        QFile xmlFile(*iter);
        if (xmlFile.exists())
        {
            if (xmlFile.open(QIODevice::ReadOnly))
            {
                m_pXMLReader = QSharedPointer<QXmlStreamReader>(new QXmlStreamReader(xmlFile.readAll()));
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        while (!m_pXMLReader->atEnd())
        {
            m_pXMLReader->readNextStartElement();

            if (m_pXMLReader->isStartElement() && m_pXMLReader->name()== "Source")
            {
                if (!m_pXMLReader->attributes().hasAttribute("Name"))
                {
                    return false;
                }
                else
                {
                    QString strSrcName = m_pXMLReader->attributes().value("Name").toString();
                    if (false == this->ConstructXMLEvent(strSrcName))
                    {
                        return false;
                    }
                }

            }
        }
    }

    //For Event-Scenaro-Error map list, we store it into m_pXMLEventList
    m_pESEXMLInfo = QSharedPointer<EventScenarioErrXMLInfo>(new EventScenarioErrXMLInfo(m_ESEXMLFile));
    if (m_pESEXMLInfo->InitXMLInfo() == false)
    {
        return false;
    }

    m_ParsingStatus = true;
    return true;
}

bool EventXMLInfo::ConstructXMLEvent(const QString& strSrcName)
{
    QSharedPointer<XMLEvent> pXMLEvent;
    while (m_pXMLReader->name()!="Source" || !m_pXMLReader->isEndElement())
    {
        m_pXMLReader->readNextStartElement();
        if (m_pXMLReader->name() == "Event" && !m_pXMLReader->isEndElement())
        {
             QString code = "";
            if (m_pXMLReader->attributes().hasAttribute("Code"))
            {
                code = m_pXMLReader->attributes().value("Code").toString();
            }

            quint32 errorId = 0;
            if (m_pXMLReader->attributes().hasAttribute("ErrorID"))
            {
                bool ok = false;
                errorId = m_pXMLReader->attributes().value("ErrorID").toString().toInt(&ok);
                if (ok == false)
                {
                    return false;
                }
            }

            Global::EventType errorType = EVTTYPE_UNDEFINED;
            if (m_pXMLReader->attributes().hasAttribute("ErrorType"))
            {
                QString strRet = m_pXMLReader->attributes().value("ErrorType").toString();
                if (strRet.trimmed() == "ERROR")
                {
                    errorType = EVTTYPE_ERROR;
                }
                else if (strRet.trimmed() == "WARNING")
                {
                    errorType = EVTTYPE_WARNING;
                }
                else if (strRet.trimmed() == "INFO")
                {
                   errorType = EVTTYPE_INFO;
                }
            }

            Global::GuiUserLevel authType;
            if (m_pXMLReader->attributes().hasAttribute("AuthorityType"))
            {
                QString strRet = m_pXMLReader->attributes().value("AuthorityType").toString();
                if (strRet.trimmed() == "Auth_ADMIN")
                {
                    authType = ADMIN;
                }
                else if (strRet.trimmed() == "Auth_SERVICE")
                {
                    authType = SERVICE;
                }
                else
                {
                    authType = OPERATOR;
                }
            }

            Global::AlarmPosType alarmType;
            if (m_pXMLReader->attributes().hasAttribute("AlarmType"))
            {
                QString strRet = m_pXMLReader->attributes().value("AlarmType").toString();
                if (strRet.trimmed() == "AL_LOCAL")
                {
                    alarmType = ALARMPOS_LOCAL;
                }
                else if (strRet.trimmed() == "AL_DEVICE")
                {
                    alarmType = ALARMPOS_DEVICE;
                }
                else if (strRet.trimmed() == "AL_REMOTE")
                {
                    alarmType = ALARMPOS_REMOTE;
                }
                else
                {
                    alarmType = ALARMPOS_NONE;
                }
            }

            quint32 rootStep = 0;
            if (m_pXMLReader->attributes().hasAttribute("RootStep"))
            {
                bool ok = false;
                rootStep = m_pXMLReader->attributes().value("RootStep").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

            QString EventName = "";
            if (m_pXMLReader->attributes().hasAttribute("Name"))
            {
                EventName = m_pXMLReader->attributes().value("Name").toString();
            }

            Global::EventSourceType eventSource = EVENTSOURCE_NONE;
            if (m_pXMLReader->attributes().hasAttribute("EventSource"))
            {
                QString strRet = m_pXMLReader->attributes().value("EventSource").toString();
                if (strRet.trimmed() == "MAIN")
                {
                    eventSource = EVENTSOURCE_MAIN;
                }
                else if (strRet.trimmed() == "DEVICECOMMANDPROCESSOR")
                {
                    eventSource = EVENTSOURCE_DEVICECOMMANDPROCESSOR;
                }
                else if (strRet.trimmed() == "SCHEDULER")
                {
                    eventSource = EVENTSOURCE_SCHEDULER;
                }
                else if (strRet.trimmed() == "EXPORT")
                {
                    eventSource = EVENTSOURCE_EXPORT;
                }
                else if (strRet.trimmed() == "IMPORTEXPORT")
                {
                    eventSource = EVENTSOURCE_IMPORTEXPORT;
                }
                else if (strRet.trimmed() == "BLG")
                {
                    eventSource = EVENTSOURCE_BLG;
                }
                else if (strRet.trimmed() == "EVENTHANDLER")
                {
                    eventSource = EVENTSOURCE_EVENTHANDLER;
                }
                else if (strRet.trimmed() == "SEPIA")
                {
                    eventSource = EVENTSOURCE_SEPIA;
                }
                else if (strRet.trimmed() == "NONE")
                {
                    eventSource = EVENTSOURCE_NONE;
                }
                else if (strRet.trimmed() == "DATALOGGER")
                {
                    eventSource = EVENTSOURCE_DATALOGGER;
                }
                else if (strRet.trimmed() == "NOTEXIST")
                {
                    eventSource = EVENTSOURCE_NOTEXIST;
                }
                else if (strRet.trimmed() == "COLORADO")
                {
                    eventSource = EVENTSOURCE_COLORADO;
                }
            }

            // construct XMLEvent object
            pXMLEvent = QSharedPointer<XMLEvent>(new XMLEvent(errorId));
            pXMLEvent->m_Source = strSrcName;
            pXMLEvent->m_Code = code;
            pXMLEvent->m_ErrorId = errorId;
            pXMLEvent->m_EventName = EventName;
            pXMLEvent->m_ErrorType = errorType;
            pXMLEvent->m_AuthType = authType;
            pXMLEvent->m_AlarmType = alarmType;
            pXMLEvent->m_RootStep = rootStep;
            pXMLEvent->m_EventSource = eventSource;
        }
        else if (m_pXMLReader->name() == "Event" && m_pXMLReader->isEndElement())
        {
            // Check if the event has been in the list
            if (m_pXMLEventList.find(pXMLEvent->m_ErrorId) == m_pXMLEventList.end())
            {
                m_pXMLEventList.insert(pXMLEvent->m_ErrorId, pXMLEvent);
            }
        }
        else if (m_pXMLReader->name() == "Step"&& !m_pXMLReader->isEndElement()) // For the "Step" elements
        {
            quint32 Id = 0;
            if (m_pXMLReader->attributes().hasAttribute("ID"))
            {
                bool ok = false;
                Id = m_pXMLReader->attributes().value("ID").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

            QString Type = "";
            if (m_pXMLReader->attributes().hasAttribute("Type"))
            {
                Type = m_pXMLReader->attributes().value("Type").toString();
            }

            Global::EventLogLevel LogLevel = Global::LOGLEVEL_NONE;
            if (m_pXMLReader->attributes().hasAttribute("LogLevel"))
            {
                QString Level = m_pXMLReader->attributes().value("LogLevel").toString();
                if (Level.trimmed().compare("LOW",Qt::CaseInsensitive) == 0)
                {
                    LogLevel = Global::LOGLEVEL_LOW;
                }
                else if (Level.trimmed().compare("MEDIUM",Qt::CaseInsensitive) == 0)
                {
                    LogLevel = Global::LOGLEVEL_MEDIUM;
                }
                else if (Level.trimmed().compare("HIGH",Qt::CaseInsensitive) == 0)
                {
                    LogLevel = Global::LOGLEVEL_HIGH;
                }
            }

            bool UserLog = false;
            if (m_pXMLReader->attributes().hasAttribute("UserLog"))
            {
                QString strUserLog = m_pXMLReader->attributes().value("UserLog").toString();
                UserLog = ((strUserLog.toUpper() == "TRUE") || (strUserLog.toUpper() == "YES"));
            }

            QString Action = "";
            if (m_pXMLReader->attributes().hasAttribute("Action"))
            {
                Action = m_pXMLReader->attributes().value("Action").toString();
            }

			// for "ACT" type
            quint32 NextStepOnFail = 0;
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnFail"))
            {
                bool ok = false;
                NextStepOnFail = m_pXMLReader->attributes().value("NextStepOnFail").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

            quint32 NextStepOnSuccess = 0;
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnSuccess"))
            {
                bool ok = false;
                NextStepOnSuccess = m_pXMLReader->attributes().value("NextStepOnSuccess").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

			// for "MSG" type
            quint32 StringId = Global::EVENT_GLOBAL_UNKNOWN_STRING_ID;
            if (m_pXMLReader->attributes().hasAttribute("StringID"))
            {
                bool ok;
                StringId = m_pXMLReader->attributes().value("StringID").toString().toUInt(&ok);
            }

            QString TimeOut="";
            if (m_pXMLReader->attributes().hasAttribute("TimeOut"))
            {
                TimeOut = m_pXMLReader->attributes().value("TimeOut").toString();
            }

            Global::GuiButtonType ButtonType = NOT_SPECIFIED;
            if (m_pXMLReader->attributes().hasAttribute("ButtonType"))
            {
                QString strRet = m_pXMLReader->attributes().value("ButtonType").toString();
                if (strRet.trimmed().compare("OK",Qt::CaseInsensitive) == 0)
                {
                    ButtonType = OK;
                }
                else if (strRet.trimmed().compare("YES_NO",Qt::CaseInsensitive) == 0)
                {
                    ButtonType = YES_NO;
                }
                else if (strRet.trimmed().compare("CONTINUE_STOP",Qt::CaseInsensitive) == 0)
                {
                    ButtonType = CONTINUE_STOP;
                }
                else if (strRet.trimmed().compare("OK_CANCEL",Qt::CaseInsensitive) == 0)
                {
                    ButtonType = OK_CANCEL;
                }
                else if (strRet.trimmed().compare("WITHOUT_BUTTONS",Qt::CaseInsensitive) == 0)
                {
                    ButtonType = WITHOUT_BUTTONS;
                }
                else if (strRet.trimmed().compare("INVISIBLE",Qt::CaseInsensitive) == 0)
                {
                    ButtonType = INVISIBLE;
                }
            }

            QString ButtonEnableCondition = "";
            if (m_pXMLReader->attributes().hasAttribute("ButtonEnableCondition"))
            {
                ButtonEnableCondition = m_pXMLReader->attributes().value("ButtonEnableCondition").toString();
            }

            quint32 NextStepOnTimeOut = 0;
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnTimeOut"))
            {
                bool ok = false;
                NextStepOnTimeOut = m_pXMLReader->attributes().value("NextStepOnTimeOut").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

            quint32 nextStepOnClickOk = 0;
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnClickOK"))
            {
                bool ok = false;
                nextStepOnClickOk = m_pXMLReader->attributes().value("NextStepOnClickOK").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

            quint32 nextStepOnClickYES = 0;
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnClickYES"))
            {
                bool ok = false;
                nextStepOnClickYES = m_pXMLReader->attributes().value("NextStepOnClickYES").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

            quint32 nextStepOnClickNO = 0;
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnClickNO"))
            {
                bool ok = false;
                nextStepOnClickNO = m_pXMLReader->attributes().value("NextStepOnClickNO").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

            bool StatusBar = true;
            if (m_pXMLReader->attributes().hasAttribute("StatusBar"))
            {
                QString strRet = m_pXMLReader->attributes().value("StatusBar").toString();
                if (strRet.trimmed().compare("YES",Qt::CaseInsensitive))
                {
                    StatusBar = true;
                }
                else
                {
                    StatusBar = false;
                }
            }

            QSharedPointer<EventStep> pEventStep(new EventStep(Id, Type));
            pEventStep->m_Action = Action;
            pEventStep->m_LogLevel = LogLevel;
            pEventStep->m_UserLog = UserLog;
            pEventStep->m_NextStepOnFail = NextStepOnFail;
            pEventStep->m_NextStepOnSuccess = NextStepOnSuccess;
            pEventStep->m_StringId = StringId;
            pEventStep->m_TimeOut = TimeOut;
            pEventStep->m_ButtonType = ButtonType;
            pEventStep->m_ButtonEnableConditon = ButtonEnableCondition;
            pEventStep->m_NextStepOnTimeOut = NextStepOnTimeOut;
            pEventStep->m_NextStepOnClickOk = nextStepOnClickOk;
            pEventStep->m_NextStepOnClickYES = nextStepOnClickYES;
            pEventStep->m_NextStepOnClickNO = nextStepOnClickNO;
            pEventStep->m_StatusBar = StatusBar;

            pXMLEvent->m_pEventStepList.insert(Id, pEventStep);
        }
    }
    return true;
}

const XMLEvent* EventXMLInfo::GetEvent(quint32 eventId, quint32 scenarioId) const
{
    // First check if XML parsing succeeds or not
    if (m_ParsingStatus  == false)
    {
        return NULL;
    }

    //In this case, strErrid is the same as ErrorID in EventConfigure.xml
    quint32 errorId = m_pESEXMLInfo->GetErrorCode(eventId, scenarioId);

    if(scenarioId == 0){
        errorId = eventId;
    }
    if (0 == errorId)
    {
        return NULL;
    }
    QHash< quint32, QSharedPointer<XMLEvent> >::const_iterator iter = m_pXMLEventList.find(errorId);
    if (iter == m_pXMLEventList.end())
    {
        return NULL;
    }

    return iter.value().data();
}

const EventStep* XMLEvent::GetStep(quint32 stepId) const
{
    QHash< quint32, QSharedPointer<EventStep> >::const_iterator iter = m_pEventStepList.find(stepId);
    if (iter == m_pEventStepList.end())
    {
        return NULL;
    }

    return iter.value().data();
}

} // end namespace EventHandler
