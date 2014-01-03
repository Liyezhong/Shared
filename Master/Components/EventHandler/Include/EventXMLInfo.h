/****************************************************************************/
/*! \file EventHandler/Include/EventXMLInfo.h
 *
 *  \brief Definition file for class EventXMLInfo.
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

#ifndef EVENTHANDLER_EVENTXMLINFO_H
#define EVENTHANDLER_EVENTXMLINFO_H

#include <Global/Include/TranslatableString.h>
#include <Global/Include/LoggingSource.h>
#include <Global/Include/Utils.h>
#include <QStringList>
#include <QXmlStreamReader>
#include <EventHandler/Include/EventScenarioErrorXMLInfo.h>
#include <QSharedPointer>

namespace EventHandler{

/****************************************************************************/
/**
 * \brief Class used to contain information on each XML Event Step 
 *
 */
/****************************************************************************/
class EventStep {
public:
    /****************************************************************************/
    /**
     * \brief explicit EventStep class constructor
     * \param[in] strId		Step Id	
     * \param[in] strType	Step Type (ACT or MSG)	
     */
    /****************************************************************************/
	explicit EventStep(const QString& strId, const QString& strType)
	: m_Id(strId),
	  m_Type(strType)	
	{ }

    /****************************************************************************/
    /**
     * \brief Get Step ID attribute 
     * 
     * \return Step Id	
     */
    /****************************************************************************/
    const QString& getId() const { return m_Id; }

    /****************************************************************************/
    /**
     * \brief Get Step Type attribute
     * 
     * \return Step Type	
     */
    /****************************************************************************/
    const QString& getType() const {return m_Type; }
private:
	QString		m_Id;					///< Step Id
	QString		m_Type;					///< Step Type 

	// For ACT type
	QString		m_Action;				///< Action Name
	QString		m_NextStepOnFail;		///< Next step when current action fails
	QString		m_NextStepOnSuccess;	///< Next step when current action succeeds

	//For MSg type
	QString		m_StringId;				///< Msg String Id
	QString		m_TimeOut;				///< Time Out
	QString		m_ButtonType;			///< Button Type
	QString		m_ButtonEnableConditon;	///< Condition on button enabled
	QString		m_NextStepOnTimeOut;	///< Next step on time out
	QString		m_StatusBar;			///< Status Bar
private:
    /****************************************************************************/
    /**
     * \brief Copy constructor of EventStep class
     * \param[in] rhs	const reference of EventStep object	
     * Disable the copy constructor
     */
    /****************************************************************************/
	EventStep(const EventStep& rhs);

    /****************************************************************************/
    /**
     * \brief assignment operator of EventStep class 
     * \param[in] rhs	const reference of EventStep object	
     * Disable the assignment operator function 
     */
    /****************************************************************************/
	EventStep& operator=(const EventStep& rhs);
    friend class EventXMLInfo;			///< declare EventXMLInfo class as friend class 
};

/****************************************************************************/
/**
 * \brief Class used to contain information on XML Event 
 *
 */
/****************************************************************************/
class XMLEvent {
public:
    /****************************************************************************/
    /**
     * \brief explicit XMLEvent class constructor
     * \param[in] strCode	Event Code Id	
     */
    /****************************************************************************/
	explicit XMLEvent(const QString& strCode)
	  : m_Code(strCode)
	{ }

    /****************************************************************************/
    /**
     * \brief Get XML Step list in the XMLEvent object 
     * 
     * \return	Const Reference of Event Step list 	
     */
    /****************************************************************************/
    const QHash< QString, QSharedPointer<EventStep> >& getEventStepList() const { return m_pEventStepList; }

    /****************************************************************************/
    /**
     * \brief Get Source attribute in XML Event  
     * 
     * \return Source attribute	
     */
    /****************************************************************************/
    const QString& getSource() const { return m_Source; }

    /****************************************************************************/
    /**
     * \brief Get Code attribute in XML Event  
     * 
     * \return Code attribute	
     */
    /****************************************************************************/
    const QString& getCode() const { return m_Code; }
private:
    QString                                 		m_Source;			///< Source Name
    QString                                 		m_Code;				///< Code
    QString                                 		m_ErrorId;			///< Error Code
    QString                                 		m_AuthType;			///< Authority Type
    QString                                 		m_AlarmType;		///< Alarm Type
    QString                                 		m_RootStep;			///< Root Step
    QHash< QString, QSharedPointer<EventStep> >		m_pEventStepList;	///< XML Step List
private:
    /****************************************************************************/
    /**
     * \brief Copy constructor of XMLEvent class
     * \param[in] rhs	const reference of XMLEvent object	
     * Disable the copy constructor
     */
    /****************************************************************************/
	XMLEvent(const XMLEvent& rhs);

    /****************************************************************************/
    /**
     * \brief assignment operator of XMLEvent class 
     * \param[in] rhs	const reference of XMLEvent object	
     * Disable the assignment operator function 
     */
    /****************************************************************************/
	XMLEvent& operator=(const XMLEvent& rhs);		

    friend class EventXMLInfo;			///< declare EventXMLInfo class as friend class 
};

/****************************************************************************/
/**
 * \brief Class used to contain information on EventXMLInfo 
 *
 */
/****************************************************************************/
class EventXMLInfo {
public:
    /****************************************************************************/
    /**
     * \brief explicit EventXMLInfo class constructor
     * \param[in] XMLFile	XML file 	
     */
    /****************************************************************************/
    explicit  EventXMLInfo(const QString& XMLFile);

    /****************************************************************************/
    /**
     * \brief Read XML file and parse it into XMLEvent list
     * 
     * \return true - success, false - failed 
     */
    /****************************************************************************/
    bool  initXMLInfo();

    /****************************************************************************/
    /**
     * \brief Get XML Event list in the EventXMLInfo object 
     * 
     * \return	Const Reference of XMLEvent list 	
     */
    /****************************************************************************/
    const QHash< QString, QSharedPointer<XMLEvent> >& getEventList() const { return m_pXMLEventList; }

    /****************************************************************************/
    /**
     * \brief Initalize ESE XML parser for later use
     * \param[in] ESEXMLFile    XML file containing ESE map info
     * \return true - success, false - failed
     */
    /****************************************************************************/
    bool  initESEXMLInfoParser(const QString& ESEXMLFile);

    /****************************************************************************/
    /**
     * \brief get Error code based on Event Id and Scenario ID.
     * \param[in] eventId		Event Id
     * \param[in] scenarioId	Scenario Id
     */
    /****************************************************************************/
    QString  getESEErrorCode(const QString& eventId, const QString& scenarioId="");
private:
    QString                                 		m_XMLFile;			///< XML file
    QSharedPointer<QXmlStreamReader>        		m_pXMLReader;		///< QT XML parser
    QHash< QString, QSharedPointer<XMLEvent> >		m_pXMLEventList;	///< XML Event list
    QSharedPointer<EventScenarioErrXMLInfo>       	m_pESEXMLInfo;		///< Event-Scenario-Error parser
    bool                                           	m_ESEXMLInfoStatus;	///< Event-Scenario-Error parser

    /****************************************************************************/
    /**
     * \brief  construct XMLEvent list and sort into m_pXMLEventList
     * \param[in] strSrcName	Source Name 	
     */
    /****************************************************************************/
	void  constructXMLEvent(const QString& strSrcName);
private:
    /****************************************************************************/
    /**
     * \brief Copy constructor of EventXMLInfo class
     * \param[in] rhs	const reference of EventXMLInfo object	
     * Disable the copy constructor
     */
    /****************************************************************************/
	EventXMLInfo(const EventXMLInfo& rhs);

    /****************************************************************************/
    /**
     * \brief assignment operator of EventXMLInfo class 
     * \param[in] rhs	const reference of EventStep object	
     * Disable the assignment operator function 
     */
    /****************************************************************************/
	EventXMLInfo& operator=(const EventXMLInfo& rhs);
}; // end class EventXMLInfo

} // end namespace EVENTHANDLER

#endif // EVENTHANDLER_EVENTXMLINFO_H
