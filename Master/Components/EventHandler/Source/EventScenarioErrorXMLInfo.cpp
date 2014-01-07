
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
EventScenarioErrXMLInfo::EventScenarioErrXMLInfo(const QString& XMLFile)
    : m_XMLFile(XMLFile),
      m_pXMLReader(NULL)
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
            constructESEInfoList();
		}

		// Get eventId-errorID-scenarioID map for each ESSInfo object
        if (m_pXMLReader->isStartElement() && m_pXMLReader->name()== "correspondinglist")
		{
			if (constructSEMap4ESEInfo() == false)
			{
				return false;
			}	
		} 
    }

    return true;
}

void EventScenarioErrXMLInfo::constructESEInfoList()
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
			QString id="";
			if (m_pXMLReader->attributes().hasAttribute("id"))
			{
				id = m_pXMLReader->attributes().value("id").toString();
			}

			QString name="";
			if (m_pXMLReader->attributes().hasAttribute("name"))
			{
                name = m_pXMLReader->attributes().value("name").toString();
			}
			pESEObj = QSharedPointer<ESEInfo>(new ESEInfo(id, name, group));
            m_EventScenarioErrList.insert(id, pESEObj);
        }
    }
}

bool EventScenarioErrXMLInfo::constructSEMap4ESEInfo()
{
	QString strErrId = "";
	QHash< QString, QSharedPointer<ESEInfo> >::iterator iter = m_EventScenarioErrList.end();

    while (m_pXMLReader->name()!="correspondinglist" || !m_pXMLReader->isEndElement())
    {
        m_pXMLReader->readNextStartElement();

        if (m_pXMLReader->name() == "event" && !m_pXMLReader->isEndElement())
        {
			if ( m_pXMLReader->attributes().hasAttribute("eventid") )
			{
				QString strEventId = m_pXMLReader->attributes().value("eventid").toString();
				iter = m_EventScenarioErrList.find(strEventId);

				// Check if the eventid is in the m_EventScenarioErrList 
				if (iter == m_EventScenarioErrList.end())
				{
                    qDebug()<<"I come here false - event";
					return false;
				}
				
			}
        }
        else if (m_pXMLReader->name() == "error" && !m_pXMLReader->isEndElement())
		{
			if ( m_pXMLReader->attributes().hasAttribute("errorid") )
			{
				strErrId = m_pXMLReader->attributes().value("errorid").toString();
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
				iter.value()->m_ScenarioErrorList.insert(scenarioId, strErrId);
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
						// Check if scenario ID is in m_ScenarioList
						if (m_ScenarioList.find(m_ScenarioPrefix+QString::number(i)) == m_ScenarioList.end())
						{
							return false;
						}

						iter.value()->m_ScenarioErrorList.insert(m_ScenarioPrefix+QString::number(i), strErrId);
					}
				}
			}
			else if (type == "all")
			{
				iter.value()->m_ScenarioErrorList.insert("all", strErrId);
			}
		}
	}

	return true;
}

QString EventScenarioErrXMLInfo::GetErrorCode(const QString& eventId, const QString& scenarioId)
{
	QHash< QString, QSharedPointer<ESEInfo> >::iterator iter = m_EventScenarioErrList.find(eventId);
	if (iter == m_EventScenarioErrList.end())
	{
		return "";
	}

	QHash<QString, QString> scenarioErrList = iter.value()->m_ScenarioErrorList;

	int size = scenarioErrList.size();
	QHash<QString, QString>::iterator sceErrIter = scenarioErrList.begin();
	// If scenarioId is empty, we suppose scenario type is "all" and there is only one element in the list
    if (scenarioId.isEmpty() && size == 1 && sceErrIter.key() == "all")
	{
		return sceErrIter.value();
	}

	sceErrIter = scenarioErrList.find(scenarioId);
	if (sceErrIter != scenarioErrList.end())
	{
		return sceErrIter.value();
	}

	return "";
}

} // end namespace EventHandler
