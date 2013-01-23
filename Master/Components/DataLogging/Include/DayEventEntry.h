/****************************************************************************/
/*! \file DataLogging/Include/DayEventEntry.h
 *
 *  \brief Definition file for class DayEventEntry.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#ifndef DATALOGGING_DAYEVENTENTRY_H
#define DATALOGGING_DAYEVENTENTRY_H

#include <Global/Include/TranslatableString.h>
#include <Global/Include/LoggingSource.h>
#include <Global/Include/Utils.h>
#include <QStringList>
#include <Global/Include/Commands/AckOKNOK.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <EventHandler/Include/EventCSVInfo.h>
#include <QDateTime>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to deliver the content of an DayOperation entry.
 *
 * This content consists of a time stamp a string ID and an argument list. It
 * has to be translatable.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class DayEventEntry  {
    friend class TestDayOperationEntry;
private:

    EventHandler::EventCSVInfo m_EventCSVInfo;
    Global::tTranslatableStringList m_String;                ///< The translatable string.
    bool                            m_EventStatus;           ///< true/false - Set/Reset flag for the event
    Global::tRefType                m_Ref;                   ///< Ref for the acknowledgement received
    NetCommands::ClickedButton_t    m_AckType;               ///< Ack status received from GUI
//    Global::AckOKNOK                m_AckValue;              ///< Ack value received from GUI
    quint32                         m_EventKey;              ///< Event Key for every event raised. NULL until raised.
    QDateTime                       m_TimeStamp;             ///< TimeStamp for entry.
    quint8                          m_count;                 ///< Number of times the event has occured



    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \iparam   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const DayEventEntry &rOther);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * Implemented due to signal / slot mechanism.
     */
    /****************************************************************************/
    DayEventEntry();

    DayEventEntry(const QDateTime &TimeStamp,quint32 EventKey,bool &EventStatus,
                                const Global::tTranslatableStringList &String, quint8 count,
                                 NetCommands::ClickedButton_t ClickButton, /*Global::AckOKNOK AckValue,*/ Global::tRefType Ref, EventHandler::EventCSVInfo CSVInfo);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \iparam   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    DayEventEntry(const DayEventEntry &rOther);
    /****************************************************************************/
    /**
     * \brief Constructor with a translatable string.
     *
     * \iparam   TimeStamp       Timestamp.
     * \iparam   String          The complete translatable string.
     */
    /****************************************************************************/
    DayEventEntry(const QDateTime &TimeStamp, const Global::tTranslatableStringList &String);


    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~DayEventEntry();
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \iparam   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    const DayEventEntry & operator = (const DayEventEntry &rOther);
    /****************************************************************************/


    /****************************************************************************/
    /**
     * \brief Dump content to console
     */
    /****************************************************************************/
    void DumpToConsole() const;
    /**
       * \brief Get the timestamp.
       *
       * \return      Timestamp.
       */
      /****************************************************************************/
      inline QDateTime GetTimeStamp() const {
          return m_TimeStamp;
      }


      inline void SetDateTime(QDateTime DateTime) {
          m_TimeStamp = DateTime;
      }


      /****************************************************************************/
      /**
       * \brief Get the Event Key.
       *
       * \return      Event Key.
       */
      /****************************************************************************/
      inline quint32 GetEventKey() const {
          return m_EventKey;
      }

      inline void SetEventKey(quint32 EventKey) {
          m_EventKey = EventKey;
      }

  /****************************************************************************/
      /**
       * \brief Get the argument list as const reference.
       *
       * \return      Argument list  as const reference.
       */
      /****************************************************************************/
      inline const Global::tTranslatableStringList &GetString() const {
          return m_String;
      }

      inline void SetTranslatableStringList(Global::tTranslatableStringList  EventStringList) {
          m_String = EventStringList;
      }

       bool GetShowInRunLogStatus() const{
          return (m_EventCSVInfo.GetRunLogStatus());

      }

        Global::EventType GetEventType() const {
          return m_EventCSVInfo.GetEventType();
      }

       quint32 GetEventCode () const{
          return m_EventCSVInfo.GetEventCode();
      }

      inline QString GetEventName() const {
          return m_EventCSVInfo.GetEventName();
      }

      inline Global::EventLogLevel GetLogLevel() const {
          return m_EventCSVInfo.GetLogLevel();
      }
      inline void SetEventCode(qint32 EventCode){
          m_EventCSVInfo.SetEventCode(EventCode);
      }

      inline bool GetAlarmStatus() const {
          return m_EventCSVInfo.GetAlarmStatus();
      }

      inline int GetRetryAttempts() const {
          return m_EventCSVInfo.GetRetryAttempts();
      }

      inline Global::ActionType GetActionNegative() const {
          return m_EventCSVInfo.GetActionNegative();
      }
      inline Global::ActionType GetActionPositive() const {
          return m_EventCSVInfo.GetActionPositive();
      }
//      inline Global::ActionType GetNextAction() const {
//          return m_EventCSVInfo.GetNextAction();
//      }

      inline bool GetStatusIcon() const {
          return m_EventCSVInfo.GetStatusIcon();
      }

      inline bool GetAckReqStatus() const {
      return m_EventCSVInfo.GetAckReqStatus();
      }

      inline void SetAckReqStatus(bool Ack) {
          m_EventCSVInfo.SetAckRequired(Ack);
      }

      inline Global::ActionType GetActionType() const {
          return m_EventCSVInfo.GetActionType();
      }

      inline Global::GuiButtonType GetButtonType() const {
          return m_EventCSVInfo.GetButtonType();
      }

      inline Global::GuiButtonType GetGUIMessageBoxOptions() const {
          return m_EventCSVInfo.GetGUIMessageBoxOptions();
      }

      inline Global::EventSourceType GetSourceComponent() const {
          return m_EventCSVInfo.GetSourceComponent();
      }

      inline Global::LoggingSource GetSource() const {
          return m_EventCSVInfo.GetSource();
      }

      void SetEventCSVInfo(EventHandler::EventCSVInfo CSVInfo) ;

      /****************************************************************************/
      /**
       * \brief Get the Event Status
       *
       * \return Event Status as const ref.
       */
      /****************************************************************************/
      inline bool IsEventActive() const {
          return m_EventStatus;
      }

    /****************************************************************************/
      /**
       * \brief Set Event Status
       *
       * param[in] const ref to Event Status
       */
      /****************************************************************************/

      inline void SetEventStatus(const bool & EventStatus) {
          m_EventStatus = EventStatus;
      }



    /****************************************************************************/
      /**
       * \brief Get the Ack Status
       *
       * \return Ack Status as const ref.
       */
      /****************************************************************************/
      inline NetCommands::ClickedButton_t  GetAckValue() const {
          return m_AckType;
      }

      /****************************************************************************/
      /**
       * \brief Set Ack Status
       *
       * param[in] const ref to Ack Value
       */
      /****************************************************************************/

      inline void SetAckValue(const NetCommands::CmdAcknEventReport & AckVal) {
          m_AckType = AckVal.GetButtonClicked();

      }

}; // end class DayEventEntry




} // end namespace DataLogging

#endif // DATALOGGING_DAYEVENTENTRY_H
