
/****************************************************************************/
/*! \file EventHandler/Source/EventScenarioErrorXMLInfo.cpp
 *
 *  \brief Implementation file for class EventScenarioErrorXMLInfo.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-31-12
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

#include <EventHandler/Include/EventScenarioErrorXMLInfo.h>
#include <QFile>

namespace EventHandler {

/****************************************************************************/
EventScenarioErrXMLInfo::EventScenarioErrXMLInfo(const QString& XMLFile, bool genRetCode)
    : m_XMLFile(XMLFile),
      m_pXMLReader(NULL),
      m_GenRetCode(genRetCode)
{
}

bool EventScenarioErrXMLInfo::InitXMLInfo()
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

		// Insert all the scenarios into m_ScenarioList
        if (m_pXMLReader->isStartElement() && m_pXMLReader->name()== "scenariolist")
        {
			m_ScenarioPrefix = m_pXMLReader->attributes().value("prefixId").toString(); 
			while (m_pXMLReader->name() != "scenariolist" || !m_pXMLReader->isEndElement())
			{
				m_pXMLReader->readNextStartElement();
				if (m_pXMLReader->name() == "scenario" && !m_pXMLReader->isEndElement())
				{
                    QString id = m_ScenarioPrefix  + m_pXMLReader->attributes().value("id").toString();
					QString name = m_pXMLReader->attributes().value("name").toString();	
					m_ScenarioList.insert(id, name);
				}
			}
        }
		//Insert all the events into m_EventScenarioErrList
        if (m_pXMLReader->isStartElement() && m_pXMLReader->name()== "eventlist")
		{
            if (false ==ConstructESEInfoList())
            {
                return false;
            }
		}

		// Get eventId-errorID-scenarioID map for each ESSInfo object
        if (m_pXMLReader->isStartElement() && m_pXMLReader->name()== "correspondinglist")
		{
            if (ConstructSEMap4ESEInfo() == false)
			{
				return false;
			}	
		} 
    }

    return true;
}

bool EventScenarioErrXMLInfo::ConstructESEInfoList()
{
    QSharedPointer<ESEInfo> pESEObj;
    while (m_pXMLReader->name()!="eventlist" || !m_pXMLReader->isEndElement())
    {
        m_pXMLReader->readNextStartElement();
		QString group="";
        if (m_pXMLReader->name() == "group" && !m_pXMLReader->isEndElement())
        {
            if (m_pXMLReader->attributes().hasAttribute("name"))
            {
                group = m_pXMLReader->attributes().value("name").toString();
            }
        }
        else if (m_pXMLReader->name() == "event" && !m_pXMLReader->isEndElement())
        {
            quint32 id = 0;
			if (m_pXMLReader->attributes().hasAttribute("id"))
			{
                bool ok = false;
                id = m_pXMLReader->attributes().value("id").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
			}

			QString name="";
			if (m_pXMLReader->attributes().hasAttribute("name"))
			{
                name = m_pXMLReader->attributes().value("name").toString();
			}
			pESEObj = QSharedPointer<ESEInfo>(new ESEInfo(id, name, group));
            m_EventScenarioErrList.insert(id, pESEObj);
            if (true == m_GenRetCode)
            {
                m_ESEInfoList.push_back(pESEObj);
            }
        }
    }
    return true;
}

bool EventScenarioErrXMLInfo::ConstructSEMap4ESEInfo()
{
    quint32 errorId = 0;
    QHash< quint32, QSharedPointer<ESEInfo> >::iterator iter = m_EventScenarioErrList.end();

    while (m_pXMLReader->name()!="correspondinglist" || !m_pXMLReader->isEndElement())
    {
        m_pXMLReader->readNextStartElement();

        if (m_pXMLReader->name() == "event" && !m_pXMLReader->isEndElement())
        {
			if ( m_pXMLReader->attributes().hasAttribute("eventid") )
			{
                bool ok = false;
                quint32 eventId = m_pXMLReader->attributes().value("eventid").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
                iter = m_EventScenarioErrList.find(eventId);

				// Check if the eventid is in the m_EventScenarioErrList 
				if (iter == m_EventScenarioErrList.end())
				{
					return false;
				}
				
			}
        }
        else if (m_pXMLReader->name() == "error" && !m_pXMLReader->isEndElement())
		{
			if ( m_pXMLReader->attributes().hasAttribute("errorid") )
			{
                bool ok = false;
                errorId = m_pXMLReader->attributes().value("errorid").toString().toInt(&ok);
                if (false == ok)
                {
                    return false;
                }
			}
        }
        else if (m_pXMLReader->name() == "scenario" && !m_pXMLReader->isEndElement())
		{
			QString type = "";
			if (m_pXMLReader->attributes().hasAttribute("type"))
			{
				type = m_pXMLReader->attributes().value("type").toString();
			}
			else
			{
				return false;
			}

			if (type == "single")
			{
				QString scenarioId = "";	
				if (m_pXMLReader->attributes().hasAttribute("id"))
				{
					scenarioId = m_ScenarioPrefix + m_pXMLReader->attributes().value("id").toString();
				}
				// Check if scenario ID is in m_ScenarioList
				if (m_ScenarioList.find(scenarioId) == m_ScenarioList.end())
				{
					return false;
				}
                iter.value()->m_ScenarioErrorList.insert(scenarioId, errorId);
			}
			else if (type == "range")
			{
				int startId = 0, endId = 0;
				if (m_pXMLReader->attributes().hasAttribute("startid") && m_pXMLReader->attributes().hasAttribute("endid"))
				{
                    startId =  m_pXMLReader->attributes().value("startid").toString().toInt();
                    endId = m_pXMLReader->attributes().value("endid").toString().toInt();
					for (int i= startId; i<=endId; i++)
					{
                        // Check if scenario ID is in m_ScenarioList. If yes, insert it. Otherwise, just ignore it.
						if (m_ScenarioList.find(m_ScenarioPrefix+QString::number(i)) == m_ScenarioList.end())
						{
                            continue;
						}

                        iter.value()->m_ScenarioErrorList.insert(m_ScenarioPrefix+QString::number(i), errorId);
					}
				}
			}
			else if (type == "all")
			{
                iter.value()->m_ScenarioErrorList.insert("all", errorId);
			}
		}
	}

	return true;
}

quint32 EventScenarioErrXMLInfo::GetErrorCode(quint32 eventId, quint32 scenarioId)
{
    QHash< quint32, QSharedPointer<ESEInfo> >::iterator iter = m_EventScenarioErrList.find(eventId);
	if (iter == m_EventScenarioErrList.end())
	{
        return 0;
	}

    QHash<QString, quint32> scenarioErrList = iter.value()->m_ScenarioErrorList;

	int size = scenarioErrList.size();
    QHash<QString, quint32>::iterator sceErrIter = scenarioErrList.begin();
	// If scenarioId is empty, we suppose scenario type is "all" and there is only one element in the list
    if (scenarioId == 0 && size == 1 && sceErrIter.key() == "all")
	{
		return sceErrIter.value();
	}

    sceErrIter = scenarioErrList.find(m_ScenarioPrefix + QString::number(scenarioId));
	if (sceErrIter != scenarioErrList.end())
	{
		return sceErrIter.value();
	}

    return 0;
}

} // end namespace EventHandler
