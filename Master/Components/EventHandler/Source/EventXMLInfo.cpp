
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
EventXMLInfo::EventXMLInfo(const QStringList& eventXMLFileList)
    : m_eventXMLFileList(eventXMLFileList),
      m_pXMLReader(NULL),
      m_ParsingStatus(false)
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

    m_ParsingStatus = true;
    return true;
}

bool EventXMLInfo::ConstructXMLEvent(const QString& strSrcName)
{
    QSharedPointer<XMLEvent> pXMLEvent;
#if !defined(__arm__)
        QMap<quint32, QString> tmp_strings;
        quint32 tmp_stringid = 0;
        QString tmp_ErrorType = "";
        QString tmp_UserFlag = "FALSE";
#endif
    while (m_pXMLReader->name()!="Source" || !m_pXMLReader->isEndElement())
    {
        m_pXMLReader->readNextStartElement();
        if (m_pXMLReader->name() == "Event" && !m_pXMLReader->isEndElement())
        {
             quint32 code = 0;
            if (m_pXMLReader->attributes().hasAttribute("Code"))
            {
                bool ok = false;
                code = m_pXMLReader->attributes().value("Code").toString().toUInt(&ok);
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

            QString EventName = "";
            if (m_pXMLReader->attributes().hasAttribute("Name"))
            {
                EventName = m_pXMLReader->attributes().value("Name").toString();
            }
            Global::EventType errorType = EVTTYPE_UNDEFINED;
            if (m_pXMLReader->attributes().hasAttribute("ErrorType"))
            {
                QString strRet = m_pXMLReader->attributes().value("ErrorType").toString().toUpper();
#if !defined(__arm__)
                tmp_ErrorType = strRet;
#endif
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
                else if (strRet.trimmed() == "DEBUG")
                {
                    errorType = EVTTYPE_DEBUG;
                }
            }

            Global::GuiUserLevel authType = ADMIN;
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

            Global::AlarmPosType alarmType = ALARMPOS_NONE;
            if (m_pXMLReader->attributes().hasAttribute("AlarmType"))
            {
                QString strRet = m_pXMLReader->attributes().value("AlarmType").toString().toUpper();
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

            bool UserLog = false;
            if (m_pXMLReader->attributes().hasAttribute("UserLog"))
            {
                QString strUserLog = m_pXMLReader->attributes().value("UserLog").toString().toUpper();
                UserLog = ((strUserLog.toUpper() == "TRUE") || (strUserLog.toUpper() == "YES"));
            }
#if !defined(__arm__)
            if(UserLog)
            {
                tmp_UserFlag = QString("TRUE");
            }
            else
            {
                tmp_UserFlag = QString("FALSE");
            }
#endif
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

            // for service string id
            quint32 ServiceString = 0;
            if (m_pXMLReader->attributes().hasAttribute("ServiceString"))
            {
                bool ok;
                ServiceString = m_pXMLReader->attributes().value("ServiceString").toString().toUInt(&ok);
            }
#if !defined(__arm__)
            tmp_stringid = ServiceString;
            tmp_strings.insert(tmp_stringid,QString("%1,%2").arg(tmp_ErrorType).arg("FALSE"));
#endif
            Global::EventLogLevel logLevel = Global::LOGLEVEL_NONE;
            if (m_pXMLReader->attributes().hasAttribute("LogLevel"))
            {
                QString Level = m_pXMLReader->attributes().value("LogLevel").toString().toUpper();
                if (Level.trimmed().compare("LOW",Qt::CaseInsensitive) == 0)
                {
                    logLevel = Global::LOGLEVEL_LOW;
                }
                else if (Level.trimmed().compare("MEDIUM",Qt::CaseInsensitive) == 0)
                {
                    logLevel = Global::LOGLEVEL_MEDIUM;
                }
                else if (Level.trimmed().compare("HIGH",Qt::CaseInsensitive) == 0)
                {
                    logLevel = Global::LOGLEVEL_HIGH;
                }
            }

            Global::EventSourceType eventSource = EVENTSOURCE_NONE;
            if (m_pXMLReader->attributes().hasAttribute("EventSource"))
            {
                QString strRet = m_pXMLReader->attributes().value("EventSource").toString().toUpper();
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
            /*lint -e644 */
            pXMLEvent->m_AuthType = authType;
            /*lint -e644 */
            pXMLEvent->m_AlarmType = alarmType;
            pXMLEvent->m_RootStep = rootStep;
            pXMLEvent->m_EventSource = eventSource;
            pXMLEvent->m_LogLevel = logLevel;
            pXMLEvent->m_UserLog = UserLog;
            pXMLEvent->m_ServiceString = ServiceString;
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
                QString strRet = m_pXMLReader->attributes().value("ButtonType").toString().toUpper();
                if (strRet.trimmed().compare("OK",Qt::CaseInsensitive) == 0)
                {
                    ButtonType = OK;
                }
                else if (strRet.trimmed().compare("YES_NO",Qt::CaseInsensitive) == 0)
                {
                    ButtonType = YES_NO;
                }
                else if (strRet.trimmed().compare("YES_NO_CANCEL",Qt::CaseInsensitive) == 0)
                {
                    ButtonType = YES_NO_CANCEL;
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
                else if (strRet.trimmed().compare("RETRY", Qt::CaseInsensitive) == 0)
                {
                    ButtonType = RETRY;
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

            quint32 nextStepOnClickCancel = 0;
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnClickCANCEL"))
            {
                bool ok = false;
                nextStepOnClickCancel = m_pXMLReader->attributes().value("NextStepOnClickCANCEL").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

            quint32 nextStepOnClickRetry = 0;
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnClickRetry"))
            {
                bool ok = false;
                nextStepOnClickRetry = m_pXMLReader->attributes().value("NextStepOnClickRetry").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
            }

            bool StatusBar = true;
            if (m_pXMLReader->attributes().hasAttribute("StatusBar"))
            {
                QString strRet = m_pXMLReader->attributes().value("StatusBar").toString().toUpper();
                if (strRet.trimmed().compare("YES",Qt::CaseInsensitive) == 0)
                {
                    StatusBar = true;
                }
                else
                {
                    StatusBar = false;
                }
            }
#if !defined(__arm__)
            if(Type.compare("MSG") == 0)
            {
                tmp_stringid = StringId;
                if(ButtonType != NOT_SPECIFIED) //user can see
                {
                    tmp_strings.insert(tmp_stringid,QString("%1,%2").arg(tmp_ErrorType).arg("TRUE"));
                }
                else
                {
                    tmp_strings.insert(tmp_stringid,QString("%1,%2").arg(tmp_ErrorType).arg(tmp_UserFlag));
                }
            }
#endif
            QSharedPointer<EventStep> pEventStep(new EventStep(Id, Type));
            pEventStep->m_Action = Action;
            pEventStep->m_NextStepOnFail = NextStepOnFail;
            pEventStep->m_NextStepOnSuccess = NextStepOnSuccess;
            pEventStep->m_StringId = StringId;
            pEventStep->m_TimeOut = TimeOut;
            pEventStep->m_ButtonType = ButtonType;
            pEventStep->m_ButtonEnableConditon = ButtonEnableCondition;
            pEventStep->m_NextStepOnTimeOut = NextStepOnTimeOut;
            pEventStep->m_NextStepOnClickOk = nextStepOnClickOk;
            pEventStep->m_NextStepOnClickRetry = nextStepOnClickRetry;
            pEventStep->m_NextStepOnClickYES = nextStepOnClickYES;
            pEventStep->m_NextStepOnClickNO = nextStepOnClickNO;
            pEventStep->m_NextStepOnClickCancel = nextStepOnClickCancel;
            pEventStep->m_StatusBar = StatusBar;
            pXMLEvent->m_pEventStepList.insert(Id, pEventStep);
        }
    }
#if !defined(__arm__)
        QFile csv(strSrcName + ".csv");
        csv.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream csvout(&csv);
        for(QMap<quint32,QString>::const_iterator i = tmp_strings.constBegin(); i != tmp_strings.constEnd(); i++)
        {
            csvout << QString("%1,%2\n").arg(i.key()).arg(i.value());
        }
#endif
    return true;
}

const XMLEvent* EventXMLInfo::GetEvent(quint32 eventId) const
{
    // First check if XML parsing succeeds or not
    if (m_ParsingStatus  == false)
    {
        return NULL;
    }

    QHash< quint32, QSharedPointer<XMLEvent> >::const_iterator iter = m_pXMLEventList.find(eventId);
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

bool XMLEvent::IsLastStep(quint32 stepId) const
{
    bool ret = true;
    const EventStep *step =  GetStep(stepId);
    if(step)
    {
        ret = (step->GetNextStepOnClickNO() + step->GetNextStepOnClickOK() + step->GetNextStepOnClickYES()
               + step->GetNextStepOnFail() + step->GetNextStepOnSuccess() + step->GetNextStepOnTimeOut()
               + step->GetNextStepOnClickCancel()) <= 0 ;
    }
    return ret;
}

} // end namespace EventHandler
