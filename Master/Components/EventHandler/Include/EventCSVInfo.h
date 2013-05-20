/****************************************************************************/
/*! \file EventHandler/Include/EventCSVInfo.h
 *
 *  \brief Definition file for class EventCSVInfo.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-30-11
 *  $Author:    $ Aparna
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef EVENTHANDLER_EVENTCSVINFO_H
#define EVENTHANDLER_EVENTCSVINFO_H

#include <Global/Include/TranslatableString.h>
#include <Global/Include/LoggingSource.h>
#include <Global/Include/Utils.h>
#include <QStringList>


namespace EventHandler{

/****************************************************************************/
/**
 * \brief Class used to deliver the content of an DayOperation entry.
 *
 * This content consists of a time stamp a string ID and an argument list. It
 * has to be translatable.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class EventCSVInfo {

public:



protected:
    quint32                         m_EventCode;             ///< Event code/Event ID for event entry - Also used as String ID to get corresponding string for translation
    QString                         m_EventMacroName;             ///< Name of event macro
    Global::EventType               m_EventType;             ///< Event type/level of event entry.

    Global::ActionType              m_ResponseAction;          ///< Response Type
    Global::ActionType              m_RecoveryActions[4];       ///< 0 - Response fail, Positive Btn clicked; 1 -  Response fail, Negative Btn clicked; 2 - Response success, Positive Btn clicked; 3 -  Response success, Negative Btn clicked;
    Global::ActionType              m_FinalAction;                   /// < default action for event
    Global::GuiButtonType           m_Buttons[2];            ///< 0 - Response fail,1 - Response success,




    qint8                           m_NumberOfRetries;       ///< Retry attempts for the action
    Global::LogAuthorityType        m_LogAuthorityType;      ///< users in different authority, they can review the different content in logView of GUI
    Global::LoggingSource           m_Source;                ///< Source for event entry.
    Global::EventLogLevel           m_LogLevel;              ///< Various log levels
    QString                         m_MessageType;           ///< Error/Info/Warning/Reboot/Retry
    bool                            m_AckRequired;           ///< true - Needs acknowledge
    Global::AlarmPosType            m_AlarmPosType;          ///< No/device/local/remote site   
    bool                            m_StatusBarIcon;         ///< true - Display icon in status bar
    Global::EventSourceType         m_SourceComponent;       ///< SourceComponent that raises the event
    QString                         m_ErrorOutline;          ///< Error Outline
    QString                         m_DetailForRD;           ///< the detailed information for RD&Service
    Global::ResponseRecoveryType    m_ResponseRecoveryType;  ///< ResponseRecovery Type

    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const EventCSVInfo &rOther);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * Implemented due to signal / slot mechanism.
     */
    /****************************************************************************/
    EventCSVInfo();
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    EventCSVInfo(const EventCSVInfo &rOther);
    /****************************************************************************/


//    /****************************************************************************/
//    /**
//     * \brief Constructor with a translatable string.
//     *

//     * \param[in]   EventCode             EventID
//     * \param[in]   EventType             Fatal/Warning/Info
//     * \param[in]   ActionType            Action for the event
//     * \param[in]   Retries               Number of retries allowed
//     * \param[in]   ShowInRunLog          true - show , false - hide
//     * \param[in]   LoggingSource         Source -Scheduler/main controller etc
//     * \param[in]   LogLevel              Enabled, disabled, debug, console
//     * \param[in]   EventSource           Source Component for the Event
//     * \param[in]   MessageType           Error/Info/Warning/Reboot/Retry
//     * \param[in]   Ack                   true - Required, false - Not Required
//     * \param[in]   AlarmRequired         true - Required, false - Not Required
//     * \param[in]   GuiButtonType         List of GUI buttons for the event
//     * \param[in]   StatusBarIcon         true - Required, false - Not Required
//     * \param[in]   SourceComponent       Source of event as string, read from EventConf file
//     */
//    /****************************************************************************/
//    EventCSVInfo(const quint32 &EventCode, const QString &EventMacroName, const Global::EventType &EventType,  const Global::ActionType &ActionType,
//                                 qint8 NumofAttempts, Global::ActionType &ActionTypePositive,Global::ActionType &ActionTypeNegative,
//                                 Global::LogAuthorityType LogAuthorityType, const Global::LoggingSource &LoggingSource, const Global::EventLogLevel &LogLevel,
//                                 const QString &MessageType, const bool &AckRequired, Global::AlarmPosType AlarmPosType, const Global::GuiButtonType &ButtonType,
//                                 const bool &StatusBarIcon, const Global::EventSourceType &SourceComponent, const QString& ErrorOutline,
//                                 Global::ActionType ResponseType, const QString &DetailForRD, Global::ResponseRecoveryType& ResponseRecoveryType);

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~EventCSVInfo();
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
  //  const EventCSVInfo & operator = (const EventCSVInfo &rOther);
    /****************************************************************************/





    /****************************************************************************/
    /**
     * \brief Get the event type of event entry.
     *
     * \return      Event type.
     */
    /****************************************************************************/
    inline Global::EventType GetEventType() const {
        return m_EventType;
    }

    inline void SetEventType(Global::EventType EventType) {
        m_EventType = EventType;
    }

    inline  int GetRetryAttempts() const {
        return m_NumberOfRetries;
    }
    inline void SetRetryAttempts(int num){
        m_NumberOfRetries = num;
    }

    /****************************************************************************/
    /**
     * \brief Get/Set the action type for the event.
     *
     * \return      Action type.
     */
    /****************************************************************************/
    inline Global::ActionType GetDefaultAction() const {
        return (Global::ActionType)m_FinalAction;
    }
    inline void SetDefaultAction(Global::ActionType action) {
        m_FinalAction = action;
    }

    inline Global::ActionType GetResponseAction() const {
        return (Global::ActionType)m_ResponseAction;
    }
    inline void SetResponseAction(Global::ActionType action) {
        m_ResponseAction = action;
    }

    inline Global::ActionType GetRecoveryActionOnRspFailUsrPst() const {
        return (Global::ActionType)m_RecoveryActions[0];
    }
    inline Global::ActionType GetRecoveryActionOnRspFailUsrNeg() const {
        return (Global::ActionType)m_RecoveryActions[1];
    }
    inline Global::ActionType GetRecoveryActionOnRspSuccUsrPst() const {
        return (Global::ActionType)m_RecoveryActions[2];
    }
    inline Global::ActionType GetRecoveryActionOnRspSuccUsrNeg() const {
        return (Global::ActionType)m_RecoveryActions[3];
    }
    inline void SetRecoveryActionOnRspFailUsrPst(Global::ActionType action) {
        m_RecoveryActions[0] = action;
    }
    inline void SetRecoveryActionOnRspFailUsrNeg(Global::ActionType action) {
        m_RecoveryActions[1] = action;
    }
    inline void SetRecoveryActionOnRspSuccUsrPst(Global::ActionType action) {
        m_RecoveryActions[2] = action;
    }
    inline void SetRecoveryActionOnRspSuccUsrNeg(Global::ActionType action) {
        m_RecoveryActions[3] = action;
    }

    /****************************************************************************/
    /**
     * \brief Get the event code of event entry.
     *
     * \return      Event code.
     */
    /****************************************************************************/
    inline quint32 GetEventCode() const {
        return m_EventCode;
    }

    inline void SetEventCode(quint32 EventCode) {
        m_EventCode = EventCode;
    }

    /****************************************************************************/
    /**
     * \brief Get the button type of event entry.
     *
     * \return      Event code.
     */
    /****************************************************************************/
    inline Global::GuiButtonType GetButtonTypeOnRspFail() const {
        return m_Buttons[0];
    }
    inline Global::GuiButtonType GetButtonTypeOnRspSucc() const {
        return m_Buttons[1];
    }
    inline void SetButtonTypeOnRspFail(Global::GuiButtonType buttonType) {
        m_Buttons[0] = buttonType;
    }
    inline void SetButtonTypeOnRspSucc(Global::GuiButtonType buttonType) {
        m_Buttons[1] = buttonType;
    }
    /****************************************************************************/
    /**
     * \brief Get/Set the status icon flag of event entry.
     *
     * \return      Event code.
     */
    /****************************************************************************/
    inline bool GetStatusIcon() const {
        return m_StatusBarIcon;
    }

    inline void SetStatusIcon(bool statusIcon) {
        m_StatusBarIcon = statusIcon;
    }


    /****************************************************************************/
    /**
     * \brief Get the Name of the event.
     */
    /****************************************************************************/
    inline QString GetEventName() const {
        return m_EventMacroName;
    }

    inline void SetEventName(QString EventName) {
        m_EventMacroName = EventName;
    }

    /****************************************************************************/
    /**
     * \brief Get the source of the event.
     *
     * \return Event source.
     */
    /****************************************************************************/
    inline Global::EventSourceType GetSourceComponent() const {
        return m_SourceComponent;
    }
    inline void SetSourceComponent(Global::EventSourceType source) {
        m_SourceComponent = source;
    }

    inline Global::LoggingSource GetSource() const {
        return m_Source;
    }

    inline  void SetSource( Global::LoggingSource Source)  {
         m_Source = Source;
    }

    /****************************************************************************/
    /**
     * \brief Get log authority Type
     *
     * \return log authority Type.
     */
    /****************************************************************************/
    inline Global::LogAuthorityType GetLogAuthorityType() const {
        return m_LogAuthorityType;
    }

    inline void SetLogAuthorityType(Global::LogAuthorityType LogAuthorityType)  {
        m_LogAuthorityType = LogAuthorityType;
    }

    /****************************************************************************/
    /**
     * \brief Get the source of the event.
     *
     * \return Event source.
     */
    /****************************************************************************/

    inline Global::EventLogLevel GetLogLevel() const {
        return m_LogLevel;
    }

    inline void SetLogLevel(const Global::EventLogLevel & LogLevel) {
        m_LogLevel = LogLevel;
    }    


    /****************************************************************************/
    /**
     * \brief Get the Message Type for the Event
     *
     * \return Message Type as const ref.
     */
    /****************************************************************************/
    inline QString GetMessageType() const {
        return m_MessageType;
    }

    /****************************************************************************/
    /**
     * \brief Set Message Type for the Event
     *
     * param[in] const ref to Message Type
     */
    /****************************************************************************/

    inline void SetMessageType(const QString & MessageType) {
        m_MessageType = MessageType;
    }

    inline void SetAckRequired(const bool AckReq) {
        m_AckRequired = AckReq;
    }

    /****************************************************************************/
    /**
     * \brief Get the Ack Status
     *
     * \return Ack Requirement Status as const ref.
     */
    /****************************************************************************/
    inline bool GetAckReqStatus() const {
        return m_AckRequired;
    }



    /****************************************************************************/
    /**
     * \brief Get the Alarm position type
     *
     * \return Alarm position types as const ref.
     */
    /****************************************************************************/
    inline Global::AlarmPosType GetAlarmPosType() const {
        return m_AlarmPosType;
    }

    /****************************************************************************/
    /**
     * \brief Set Alarm position type
     *
     * param[in] ref to Alarm position type
     */
    /****************************************************************************/

    inline void SetAlarmPosType(Global::AlarmPosType AlarmPosType) {
        m_AlarmPosType = AlarmPosType;
    }

    /****************************************************************************/
    /**
     * \brief Set the outline of the error
     *
     */
    /****************************************************************************/
    inline void SetErrorOutline(const QString & ErrorOutline) {
        m_ErrorOutline = ErrorOutline;
    }

    /****************************************************************************/
    /**
     * \brief Set ResponseRecovery type
     *
     * param[in] const ref to ResponseRecovery type flag
     */
    /****************************************************************************/
    inline void SetResponseRecoveryType(Global::ResponseRecoveryType ResponseRecoveryType) {
        m_ResponseRecoveryType = ResponseRecoveryType;
    }
    inline Global::ResponseRecoveryType GetResponseRecoveryType() const {
        return m_ResponseRecoveryType;
    }

    /****************************************************************************/
    /**
     * \brief Set the detailed error information for RD&Service
     *
     * param[in] const ref to the detailed error information
     */
    /****************************************************************************/
    inline void SetDetailForRD(const QString & DetailInfor) {
        m_DetailForRD = DetailInfor;
    }
    inline QString GetDetailForRD() const{
        return m_DetailForRD;
    }

}; // end class EventCSVInfo




} // end namespace EVENTHANDLER

#endif // EVENTHANDLER_EVENTCSVINFO_H
