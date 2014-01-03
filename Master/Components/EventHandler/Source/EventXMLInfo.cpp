
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
#include <QFile>

namespace EventHandler {

/****************************************************************************/
EventXMLInfo::EventXMLInfo(const QString& XMLFile)
    : m_XMLFile(XMLFile),
      m_pXMLReader(NULL)
{
}

bool EventXMLInfo::initXMLInfo()
{
	QFile xmlFile(m_XMLFile);
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
                this->constructXMLEvent(strSrcName);
            }

        }
    }

    return true;
}

void EventXMLInfo::constructXMLEvent(const QString& strSrcName)
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

            QString errorId = "";
            if (m_pXMLReader->attributes().hasAttribute("ErrorID"))
            {
                errorId = m_pXMLReader->attributes().value("ErrorID").toString();
            }

            QString errorType = "";
            if (m_pXMLReader->attributes().hasAttribute("ErrorType"))
            {
                errorType = m_pXMLReader->attributes().value("ErrorType").toString();
            }

            QString authType = "";
            if (m_pXMLReader->attributes().hasAttribute("AuthorityType"))
            {
                authType = m_pXMLReader->attributes().value("AuthorityType").toString();
            }

            QString alarmType = "";
            if (m_pXMLReader->attributes().hasAttribute("AlarmType"))
            {
                alarmType = m_pXMLReader->attributes().value("AlarmType").toString();
            }

            QString rootStep = "";
            if (m_pXMLReader->attributes().hasAttribute("RootStep"))
            {
                rootStep = m_pXMLReader->attributes().value("RootStep").toString();
            }

            // construct XMLEvent object
            pXMLEvent = QSharedPointer<XMLEvent>(new XMLEvent(code));
            pXMLEvent->m_Source = strSrcName;
            pXMLEvent->m_ErrorId = errorId;
            pXMLEvent->m_AuthType = authType;
            pXMLEvent->m_AlarmType = alarmType;
            pXMLEvent->m_RootStep = rootStep;
        }
        else if (m_pXMLReader->name() == "Event" && m_pXMLReader->isEndElement())
        {
            m_pXMLEventList.insert(pXMLEvent->m_Code, pXMLEvent);
        }
        else if (m_pXMLReader->name() == "Step"&& !m_pXMLReader->isEndElement()) // For the "Step" elements
        {
            QString Id = "";
            if (m_pXMLReader->attributes().hasAttribute("ID"))
            {
                Id = m_pXMLReader->attributes().value("ID").toString();
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
            QString NextStepOnFail = "";
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnFail"))
            {
                NextStepOnFail = m_pXMLReader->attributes().value("NextStepOnFail").toString();
            }

            QString NextStepOnSuccess = "";
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnSuccess"))
            {
                NextStepOnSuccess = m_pXMLReader->attributes().value("NextStepOnSuccess").toString();
            }

			// for "MSG" type
            QString StringId = "";
            if (m_pXMLReader->attributes().hasAttribute("StringID"))
            {
                StringId = m_pXMLReader->attributes().value("StringID").toString();
            }

            QString TimeOut = "";
            if (m_pXMLReader->attributes().hasAttribute("TimeOut"))
            {
                TimeOut = m_pXMLReader->attributes().value("TimeOut").toString();
            }

            QString ButtonType = "";
            if (m_pXMLReader->attributes().hasAttribute("ButtonType"))
            {
                ButtonType = m_pXMLReader->attributes().value("ButtonType").toString();
            }

            QString ButtonEnableCondition = "";
            if (m_pXMLReader->attributes().hasAttribute("ButtonEnableCondition"))
            {
                ButtonEnableCondition = m_pXMLReader->attributes().value("ButtonEnableCondition").toString();
            }

            QString NextStepOnTimeOut = "";
            if (m_pXMLReader->attributes().hasAttribute("NextStepOnTimeOut"))
            {
                NextStepOnTimeOut = m_pXMLReader->attributes().value("NextStepOnTimeOut").toString();
            }

            QString StatusBar = "";
            if (m_pXMLReader->attributes().hasAttribute("StatusBar"))
            {
                StatusBar = m_pXMLReader->attributes().value("StatusBar").toString();
            }

            QSharedPointer<EventStep> pEventStep(new EventStep(Id, Type));
            pEventStep->m_Action = Action;
            pEventStep->m_NextStepOnFail = NextStepOnFail;
            pEventStep->m_NextStepOnSuccess = NextStepOnSuccess;
            pEventStep->m_StringId = StringId;
            pEventStep->m_TimeOut = TimeOut;
            pEventStep->m_ButtonType = ButtonType;
            pEventStep->m_ButtonEnableConditon = ButtonEnableCondition;
            pEventStep->m_NextStepOnTimeOut = NextStepOnTimeOut;
            pEventStep->m_StatusBar = StatusBar;

            pXMLEvent->m_pEventStepList.insert(Id, pEventStep);
        }
    }

}

} // end namespace EventHandler
