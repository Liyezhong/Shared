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
    const QString& GetId() const { return m_Id; }

    /****************************************************************************/
    /**
     * \brief Get Step Type attribute
     * 
     * \return Step Type	
     */
    /****************************************************************************/
    const QString& GetType() const { return m_Type; }

    /****************************************************************************/
    /**
     * \brief Get Action attribute
     * 
     * \return Action Type	
     */
    /****************************************************************************/
    const QString& GetAction() const { return m_Action; }

    /****************************************************************************/
    /**
     * \brief Get NextStepOnFail attribute
     * 
     * \return NextStepOnFail Type	
     */
    /****************************************************************************/
    const QString& GetNextStepOnFail() const { return m_NextStepOnFail; }

    /****************************************************************************/
    /**
     * \brief Get NextStepOnSuccess attribute
     * 
     * \return NextStepOnSuccess Type	
     */
    /****************************************************************************/
    const QString& GetNextStepOnSuccess() const { return m_NextStepOnSuccess; }

    /****************************************************************************/
    /**
     * \brief Get StringID attribute
     * 
     * \return StringID Type	
     */
    /****************************************************************************/
    const QString& GetStringID() const { return m_StringId; }

    /****************************************************************************/
    /**
     * \brief Get TimeOut attribute
     * 
     * \return TimeOut Type	
     */
    /****************************************************************************/
    const QString& GetTimeOut() const { return m_TimeOut; }

    /****************************************************************************/
    /**
     * \brief Get ButtonType attribute
     * 
     * \return ButtonType Type	
     */
    /****************************************************************************/
    const QString& GetButtonType() const { return m_ButtonType; }

    /****************************************************************************/
    /**
     * \brief Get ButtonEnableConditon attribute
     * 
     * \return ButtonEnableConditon Type	
     */
    /****************************************************************************/
    const QString& GetButtonEnableConditon() const { return m_ButtonEnableConditon; }

    /****************************************************************************/
    /**
     * \brief Get NextStepOnTimeOut attribute
     * 
     * \return NextStepOnTimeOut Type	
     */
    /****************************************************************************/
    const QString& GetNextStepOnTimeOut() const { return m_NextStepOnTimeOut; }

    /****************************************************************************/
    /**
     * \brief Get StatusBar attribute
     * 
     * \return StatusBar Type	
     */
    /****************************************************************************/
    const QString& GetStatusBar() const { return m_StatusBar; }
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
     * \param[in] strErrID	Error Id	
     */
    /****************************************************************************/
	explicit XMLEvent(const QString& strErrID)
	  : m_ErrorId(strErrID)
	{ }

    /****************************************************************************/
    /**
     * \brief Get XML Step list in the XMLEvent object 
     * \param[in] stepId	Step Id	
     * \return Pointer to Eventstep object 
     */
    /****************************************************************************/
    const EventStep*  GetStep(const QString& stepId) const;

    /****************************************************************************/
    /**
     * \brief Get ErrorId attribute in XML Event  
     * 
     * \return ErrorId attribute	
     */
    /****************************************************************************/
    const QString& GetErrorId() const { return m_ErrorId; }

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

    /****************************************************************************/
    /**
     * \brief AuthorityType attribute in XML Event  
     * 
     * \return AuthorityType attribute	
     */
    /****************************************************************************/
    const QString& getAuthType() const { return m_AuthType; }

    /****************************************************************************/
    /**
     * \brief AlarmType attribute in XML Event  
     * 
     * \return AlarmType attribute	
     */
    /****************************************************************************/
    const QString& getAlarmType() const { return m_AlarmType; }

    /****************************************************************************/
    /**
     * \brief RootStep attribute in XML Event  
     * 
     * \return RootStep attribute	
     */
    /****************************************************************************/
    const QString& getRootStep() const { return m_RootStep; }
private:
    QString                                 		m_ErrorId;			///< Error Code
    QString                                 		m_Source;			///< Source Name
    QString                                 		m_Code;				///< Code
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
     * \param[in]   eventXMLFileList    XML file list containing all events
     ** \param[in]  ESEXMLFile          XML file containing event-scenario-error map list
     */
    /****************************************************************************/
    explicit  EventXMLInfo(const QStringList& eventXMLFileList, const QString& ESEXMLFile="");

    /****************************************************************************/
    /**
     * \brief Read XML file and parse it into XMLEvent list
     * 
     * \return true - success, false - failed 
     */
    /****************************************************************************/
    bool  InitXMLInfo();

    /****************************************************************************/
    /**
     * \brief Get XML Event list in the EventXMLInfo object 
     * 
     * \return	Const Reference of XMLEvent list 	
     */
    /****************************************************************************/
    const QHash< QString, QSharedPointer<XMLEvent> >& GetEventList() const { return m_pXMLEventList; }

    /****************************************************************************/
    /**
     * \brief get XMLEvent object based on Event Id and Scenario ID.
     * \param[in] eventId		Event Id
     * \param[in] scenarioId	Scenario Id
     * \return		pointer to the XML Event object
     */
    /****************************************************************************/
    const XMLEvent* GetEvent( const QString& eventId, const QString& scenarioId="");
private:
    QStringList                                		m_eventXMLFileList;	///< XML file list containing all events
    QString                                         m_ESEXMLFile;       ///< XML file containing event-scenario-error map list
    QSharedPointer<QXmlStreamReader>        		m_pXMLReader;		///< QT XML parser
    bool											m_ParsingStatus;	///< XML Parsing status
    QHash< QString, QSharedPointer<XMLEvent> >		m_pXMLEventList;	///< XML Event list
    QSharedPointer<EventScenarioErrXMLInfo>       	m_pESEXMLInfo;		///< Event-Scenario-Error parser

    /****************************************************************************/
    /**
     * \brief  construct XMLEvent list and sort into m_pXMLEventList
     * \param[in] strSrcName	Source Name 	
     */
    /****************************************************************************/
    void  ConstructXMLEvent(const QString& strSrcName);
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
