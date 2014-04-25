/****************************************************************************/
/*! \file EventHandler/Include/EventScenarioErrorXMLInfo.h
 *
 *  \brief Definition file for class EventXMLInfo.
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

#ifndef EVENTHANDLER_EVENTSCENARIOERRORXMLINFO_H
#define EVENTHANDLER_EVENTSCENARIOERRORXMLINFO_H

#include <Global/Include/TranslatableString.h>
#include <Global/Include/LoggingSource.h>
#include <Global/Include/Utils.h>
#include <QStringList>
#include <QXmlStreamReader>
#include <QVector>
#include <QSharedPointer>

namespace EventHandler{

/****************************************************************************/
/**
 * \brief Class used to contain information on Event-Scenaro-Error 
 *
 */
/****************************************************************************/
class ESEInfo {
public:
    /****************************************************************************/
    /**
     * \brief explicit ESEInfo class constructor
     * \param[in] strId   Event Id   
     */
    /****************************************************************************/
    explicit ESEInfo(quint32 id, const QString& strName, const QString& strGroup)
      : m_Id(id),
		m_Name(strName),
		m_Group(strGroup)
    { }

    /****************************************************************************/
    /**
     * \brief Get Error list in the ESEInfo object 
     * 
     * \return  Const Reference of Error list  
     */
    /****************************************************************************/
    const QHash<QString, quint32>& GetScenarioErrorList() const { return m_ScenarioErrorList; }

    /****************************************************************************/
    /**
     * \brief Get name attribute in the ESEInfo object 
     * 
     * \return  value of name class member  
     */
    /****************************************************************************/
    const QString& GetName() const { return m_Name; }
    /****************************************************************************/
    /**
     * \brief Get Id attribute in the ESEInfo object
     *
     * \return  value of Id class member
     */
    /****************************************************************************/
    quint32 GetId() const { return m_Id; }
private:
	quint32					m_Id;					///< Event Id
	QString					m_Name;					///< Event Name 
	QString					m_Group;				///< Event Group 
	QHash<QString, quint32>	m_ScenarioErrorList;	///< Error List, key:scenario,value:error
private:
	/****************************************************************************/
    /**
     * \brief Copy constructor of ESEInfo class
     * \param[in] rhs   const reference of ESEInfo object  
     * Disable the copy constructor
     */
    /****************************************************************************/
    ESEInfo(const ESEInfo& rhs);

    /****************************************************************************/
    /**
     * \brief assignment operator of ESEInfo class 
     * \param[in] rhs   const reference of ESEInfo object  
     * Disable the assignment operator function 
     */
    /****************************************************************************/
	ESEInfo& operator=(const ESEInfo& rhs);
	friend class EventScenarioErrXMLInfo;  ///< declare EventScenarioErrXMLInfo class as friend class
};


/****************************************************************************/
/**
 * \brief Class used to contain information on Event-Scenario-Error MAP XML
 *
 */
/****************************************************************************/
class EventScenarioErrXMLInfo {
public:
    /****************************************************************************/
    /**
     * \brief explicit EventScenarioErrXMLInfo class constructor
     * \param[in] XMLFile	XML file 	
     */
    /****************************************************************************/
    explicit  EventScenarioErrXMLInfo(const QString& XMLFile, bool genRetCode=false);

    /****************************************************************************/
    /**
     * \brief Read XML file and parse it into EventInfo list
     * 
     * \return true - success, false - failed 
     */
    /****************************************************************************/
    bool  InitXMLInfo();

    /****************************************************************************/
    /**
     * \brief Get Scenaro list in the EventScenarioErrXMLInfo object 
     * 
     * \return	Const Reference of QHash<QString, QString> type of Scenario List
     */
    /****************************************************************************/
    const QHash<QString, QString>& GetScenarioList() const { return m_ScenarioList; }

    /****************************************************************************/
    /**
     * \brief Get Event-Scenaro-Error list (with QVector format) in the EventScenarioErrXMLInfo object 
     * 
     * \return	Const Reference of Event-Scenaro-Error list 	
     */
    /****************************************************************************/
    const QVector< QSharedPointer<ESEInfo> >& GetESEInfoList() const { return m_ESEInfoList; }

    /****************************************************************************/
    /**
     * \brief Get Event-Scenaro-Error list (with QHash format) in the EventScenarioErrXMLInfo object 
     * 
     * \return	Const Reference of Event-Scenaro-Error list 	
     */
    /****************************************************************************/
    const QHash< quint32, QSharedPointer<ESEInfo> >& GetHashESEInfoList() const { return m_EventScenarioErrList; }

    /****************************************************************************/
    /**
     * \brief get Error code based on Event Id and Scenario ID.  
     * \param[in] eventId		Event Id 
     * \param[in] scenarioId	Scenario Id 
     * \return Error ID. If failed, return zero	
     */
    /****************************************************************************/
    quint32  GetErrorCode(quint32 eventId, quint32 scenarioId=0);
private:
    QString                                 		m_XMLFile;					///< XML file
    QSharedPointer<QXmlStreamReader>        		m_pXMLReader;				///< QT XML parser
    QString                                 		m_ScenarioPrefix;			///< Scenario Prefix 
    QHash<QString, QString>							m_ScenarioList;	    		///< Scenario list
    QHash< quint32, QSharedPointer<ESEInfo> >		m_EventScenarioErrList;    	///< Event-Scenaro-Error list
    QVector< QSharedPointer<ESEInfo> >              m_ESEInfoList;              ///< ESE list used to generate ReturnCode_t
    bool                                            m_GenRetCode;               ///< Generate Return Code or not

    /****************************************************************************/
    /**
     * \brief  construct ESEInfo objects list and sort into m_EventScenarioErrList
     * \return true - success, false - failed  
     */
    /****************************************************************************/
	bool  ConstructESEInfoList();

    /****************************************************************************/
    /**
     * \brief  construct m_ErrorList in each ESSInfo object 
     * \return true - success, false - failed  
     */
    /****************************************************************************/
	bool  ConstructSEMap4ESEInfo();
private:
    /****************************************************************************/
    /**
     * \brief Copy constructor of EventScenarioErrXMLInfo class
     * \param[in] rhs	const reference of EventScenarioErrXMLInfo object	
     * Disable the copy constructor
     */
    /****************************************************************************/
	EventScenarioErrXMLInfo(const EventScenarioErrXMLInfo& rhs);

    /****************************************************************************/
    /**
     * \brief assignment operator of EventScenarioErrXMLInfo class 
     * \param[in] rhs	const reference of EventScenarioErrXMLInfo object	
     * Disable the assignment operator function 
     */
    /****************************************************************************/
	EventScenarioErrXMLInfo& operator=(const EventScenarioErrXMLInfo& rhs);
}; // end class EventScenarioErrXMLInfo

} // end namespace EVENTHANDLER

#endif // EVENTHANDLER_EVENTSCENARIOERRORXMLINFO_H
