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
    explicit EventStep(quint32 stepId, const QString& strType)
        : m_Id(stepId),m_Type(strType),m_NextStepOnFail(0),m_NextStepOnSuccess(0),m_NextStepOnTimeOut(0),
          m_NextStepOnClickOk(0),m_NextStepOnClickYES(0),m_NextStepOnClickNO(0)
	{ }

    /****************************************************************************/
    /**
     * \brief Get Step ID attribute 
     * 
     * \return Step Id	
     */
    /****************************************************************************/
    quint32 GetId() const { return m_Id; }

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
    quint32 GetNextStepOnFail() const { return m_NextStepOnFail; }

    /****************************************************************************/
    /**
     * \brief Get NextStepOnSuccess attribute
     * 
     * \return NextStepOnSuccess Type	
     */
    /****************************************************************************/
    quint32 GetNextStepOnSuccess() const { return m_NextStepOnSuccess; }

    /****************************************************************************/
    /**
     * \brief Get StringID attribute
     * 
     * \return StringID Type	
     */
    /****************************************************************************/
    const quint32 & GetStringID() const { return m_StringId; }

    /****************************************************************************/
    /**
     * \brief Get TimeOut attribute
     * 
     * \return TimeOut Type	
     */
    /****************************************************************************/
    QString GetTimeOut() const { return m_TimeOut; }
    /****************************************************************************/
    /**
     * \brief Set Timeout attribute
     *
     * \param  TimeOut Type
     */
    /****************************************************************************/
    void SetTimeOut(QString TimeOut) const {m_TimeOut = TimeOut;}


    /****************************************************************************/
    /**
     * \brief Get ButtonType attribute
     * 
     * \return ButtonType Type	
     */
    /****************************************************************************/
    Global::GuiButtonType GetButtonType() const { return m_ButtonType; }

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
    quint32 GetNextStepOnTimeOut() const { return m_NextStepOnTimeOut; }

    /****************************************************************************/
    /**
     * \brief Get NextStepOnClickOK attribute
     * 
     * \return NextStepOnClickOK Type	
     */
    /****************************************************************************/
    quint32 GetNextStepOnClickOK() const { return m_NextStepOnClickOk; }

    /****************************************************************************/
    /**
     * \brief Get NextStepOnClickYES attribute
     * 
     * \return NextStepOnClickYES Type	
     */
    /****************************************************************************/
    quint32 GetNextStepOnClickYES() const { return m_NextStepOnClickYES; }

    /****************************************************************************/
    /**
     * \brief Get NextStepOnClickNO attribute
     * 
     * \return NextStepOnClickNO Type	
     */
    /****************************************************************************/
    quint32 GetNextStepOnClickNO() const { return m_NextStepOnClickNO; }

    /****************************************************************************/
    /**
     * \brief Get StatusBar attribute
     * 
     * \return StatusBar Type	
     */
    /****************************************************************************/
    bool GetStatusBar() const { return m_StatusBar; }

    /****************************************************************************/
    /**
     * \brief Get UserLog attribute
     *
     * \return UserLog Flag
     */
    /****************************************************************************/
    bool GetUserLog() const { return m_UserLog; }

    /****************************************************************************/
    /**
     * \brief Get Log Level attribute
     *
     * \return Log level
     */
    /****************************************************************************/
    Global::EventLogLevel GetLogLevel() const { return m_LogLevel; }
private:
    quint32		m_Id;					///< Step Id
	QString		m_Type;					///< Step Type 

	// For ACT type
	QString		m_Action;				///< Action Name
    quint32		m_NextStepOnFail;		///< Next step when current action fails
    quint32		m_NextStepOnSuccess;	///< Next step when current action succeeds
    Global::EventLogLevel    m_LogLevel;     ///< Log level
    bool                m_UserLog;      ////< user log

	//For MSg type
    quint32                     m_StringId;				///< Msg String Id
    mutable QString                     m_TimeOut;				///< Time Out
    Global::GuiButtonType		m_ButtonType;			///< Button Type
    QString                     m_ButtonEnableConditon;	///< Condition on button enabled
    quint32                     m_NextStepOnTimeOut;	///< Next step on time out
    quint32                     m_NextStepOnClickOk;	///< Next step on clicking OK 
    quint32                     m_NextStepOnClickYES;	///< Next step on clicking YES 
    quint32                     m_NextStepOnClickNO;	///< Next step on clicking NO 
    bool                        m_StatusBar;			///< Status Bar
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
     * \param[in] errorID	Error Id
     */
    /****************************************************************************/
    explicit XMLEvent(quint32 errorID)
      : m_ErrorId(errorID)
	{ }

    /****************************************************************************/
    /**
     * \brief Get XML Step list in the XMLEvent object 
     * \param[in] stepId	Step Id	
     * \return Pointer to Eventstep object 
     */
    /****************************************************************************/
    const EventStep*  GetStep(quint32 stepId) const;


    bool IsLastStep(quint32 stepId) const;

    /****************************************************************************/
    /**
     * \brief Get ErrorId attribute in XML Event  
     * 
     * \return ErrorId attribute	
     */
    /****************************************************************************/
    quint32 GetErrorId() const { return m_ErrorId; }

    /****************************************************************************/
    /**
     * \brief Get Source attribute in XML Event  
     * 
     * \return Source attribute	
     */
    /****************************************************************************/
    const QString& GetSource() const { return m_Source; }

    /****************************************************************************/
    /**
     * \brief Get Code attribute in XML Event  
     * 
     * \return Code attribute	
     */
    /****************************************************************************/
    quint32 GetCode() const { return m_Code; }

    /****************************************************************************/
    /**
     * \brief ErrorType attribute in XML Event
     *
     * \return ErrorType attribute
     */
    /****************************************************************************/
    Global::EventType GetErrorType() const { return m_ErrorType; }


    /****************************************************************************/
    /**
     * \brief AuthorityType attribute in XML Event  
     * 
     * \return AuthorityType attribute	
     */
    /****************************************************************************/
    Global::GuiUserLevel GetAuthType() const { return m_AuthType; }

    /****************************************************************************/
    /**
     * \brief AlarmType attribute in XML Event  
     * 
     * \return AlarmType attribute	
     */
    /****************************************************************************/
    Global::AlarmPosType GetAlarmType() const { return m_AlarmType; }

    /****************************************************************************/
    /**
     * \brief RootStep attribute in XML Event  
     * 
     * \return RootStep attribute	
     */
    /****************************************************************************/
    quint32 GetRootStep() const { return m_RootStep; }


    /****************************************************************************/
    /**
     * \brief EventSource attribute in XML Event
     *
     * \return EventSource attribute
     */
    /****************************************************************************/
    Global::EventSourceType GetEventSource() const { return m_EventSource; }

    /****************************************************************************/
    /**
     * \brief Get Event Name (macro)
     *
     * \return event name (macro)
     */
    /****************************************************************************/
    const QString& GetEventName() const { return m_EventName; }
private:
    quint32                                 		m_ErrorId;			///< Error Code
    QString                                 		m_Source;			///< Source Name
    quint32                                 		m_Code;				///< Code
    QString                                         m_EventName;         ///< Event macro
    Global::EventType                          m_ErrorType;		///< Error Type
    Global::GuiUserLevel                          m_AuthType;			///< Authority Type
    Global::AlarmPosType                               m_AlarmType;		///< Alarm Type
    quint32                                 		m_RootStep;			///< Root Step
    Global::EventSourceType                         m_EventSource;		///< Event Source
    QHash< quint32, QSharedPointer<EventStep> >		m_pEventStepList;	///< XML Step List
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
    const QHash< quint32, QSharedPointer<XMLEvent> >& GetEventList() const { return m_pXMLEventList; }

    /****************************************************************************/
    /**
     * \brief get XMLEvent object based on Event Id and Scenario ID.
     * \param[in] eventId		Event Id
     * \param[in] scenarioId	Scenario Id
     * \return		pointer to the XML Event object
     */
    /****************************************************************************/
    const XMLEvent* GetEvent( quint32 eventId, quint32 scenarioId=0) const;
private:
    QStringList                                		m_eventXMLFileList;	///< XML file list containing all events
    QString                                         m_ESEXMLFile;       ///< XML file containing event-scenario-error map list
    QSharedPointer<QXmlStreamReader>        		m_pXMLReader;		///< QT XML parser
    bool											m_ParsingStatus;	///< XML Parsing status
    QHash< quint32, QSharedPointer<XMLEvent> >		m_pXMLEventList;	///< XML Event list
    QSharedPointer<EventScenarioErrXMLInfo>       	m_pESEXMLInfo;		///< Event-Scenario-Error parser

    /****************************************************************************/
    /**
     * \brief  construct XMLEvent list and sort into m_pXMLEventList
     * \param[in] strSrcName	Source Name 	
     * \return true - success, false - failed 
     */
    /****************************************************************************/
    bool  ConstructXMLEvent(const QString& strSrcName);
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
