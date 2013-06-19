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
    quint32                         m_EventCodeFromCom;            /// Event code from RaiseEvent call
    Global::tTranslatableStringList m_StringForRd;             ///< Argument list for event string for R&d
    quint32                         m_Scenario;             /// Event Scenario. m_EventCode + m_Scenario <=> m_EventCSVInfo.GetEventCode()
    QDateTime                       m_TimeStamp;             ///< TimeStamp for entry.
    quint8                          m_CountRetires;                 ///< Number of times the event has been retried to fixed by user from Gui.
    Global::AlternateEventStringUsage m_AltEventStringUsage; ///< Alternate Event string type
    Global::EventStatus             m_CurrentStatus;                         ///< current status of event.
    bool                            m_IsHWParameter;            ///< used to mark if this is hardware parameter
    QString                         m_HWParameter;              /// HW parameter



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
    DayEventEntry(bool isHWPar, QString& HWInfo);

    DayEventEntry(const QDateTime &TimeStamp,quint32 EventKey,bool &EventStatus,
                                const Global::tTranslatableStringList &String, quint8 count,
                                 NetCommands::ClickedButton_t ClickButton, 
                                /*Global::AckOKNOK AckValue,*/ Global::tRefType Ref, 
                                EventHandler::EventCSVInfo CSVInfo);
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

    inline void SetAltStringUsage(const Global::AlternateEventStringUsage AltStringUsage) {
        m_AltEventStringUsage = AltStringUsage;
    }

    inline Global::AlternateEventStringUsage GetAltStringUsageType() const {
        return m_AltEventStringUsage;
    }

    /****************************************************************************/
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

      inline bool GetIsHWParameter() const
      {
          return m_IsHWParameter;
      }
      inline void SetIsHWParameter(bool val)
      {
          m_IsHWParameter = val;
      }

      inline QString GetHWParameter() const
      {
          return m_HWParameter;
      }

      /****************************************************************************/
      /**
         * \brief Get the EventCode.
         *
         * \return      EventCode.
         */
        /****************************************************************************/
        inline quint32 GetEventCodeFromCom() const {
            return m_EventCodeFromCom;
        }


        inline void SetEventCodeFromCom(quint32 EventCode) {
            m_EventCodeFromCom = EventCode;
        }
        /****************************************************************************/
        /**
           * \brief Get the Event Scenario.
           *
           * \return      Scenario.
           */
          /****************************************************************************/
          inline quint32 GetScenario() const {
              return m_Scenario;
          }


          inline void SetScenario(quint32 Scenario) {
              m_Scenario = Scenario;
          }


          /****************************************************************************/
          /**
             * \brief Get/Set the Event Current Status.
             *
             * \return      Scenario.
             */
            /****************************************************************************/
            inline Global::EventStatus GetCurrentStatus() const {
                return m_CurrentStatus;
            }


            inline void SetCurrentStatus(Global::EventStatus CurrentStatus) {
                m_CurrentStatus = CurrentStatus;
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

      /****************************************************************************/
          /**
           * \brief Get the argument list as for event string for RD
           *
           * \return
           */
          /****************************************************************************/
          inline Global::tTranslatableStringList &GetStringArgForRd() {
              return m_StringForRd;
          }

          inline void SetStringArgForRd(Global::tTranslatableStringList  EventStringList) {
              m_StringForRd = EventStringList;
          }

          QString GetStringForRd() const;

      Global::LogAuthorityType GetLogAuthorityType() const{
          return (m_EventCSVInfo.GetLogAuthorityType());

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

      inline Global::AlarmPosType GetAlarmPosType() const {
          return m_EventCSVInfo.GetAlarmPosType();
      }

      inline int GetRetryAttempts() const {
          return m_EventCSVInfo.GetRetryAttempts();
      }
      inline quint8 GetAndIncRetryCount() {
          m_CountRetires ++;
          return m_CountRetires - 1;
      }

      inline Global::ActionType GetRecoveryActionOnRspFailUsrPst() const {
          return m_EventCSVInfo.GetRecoveryActionOnRspFailUsrPst();
      }
      inline Global::ActionType GetRecoveryActionOnRspFailUsrNeg() const {
          return m_EventCSVInfo.GetRecoveryActionOnRspFailUsrNeg();
      }
      inline Global::ActionType GetRecoveryActionOnRspSuccUsrPst() const {
          return m_EventCSVInfo.GetRecoveryActionOnRspSuccUsrPst();
      }
      inline Global::ActionType GetRecoveryActionOnRspSuccUsrNeg() const {
          return m_EventCSVInfo.GetRecoveryActionOnRspSuccUsrNeg();
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

      inline Global::ActionType GetDefaultAction() const {
          return m_EventCSVInfo.GetDefaultAction();
      }

      inline Global::ResponseRecoveryType GetResponseRecoveryType() const {
          return m_EventCSVInfo.GetResponseRecoveryType();
      }

      inline Global::ActionType GetResponseAction() const {
          return m_EventCSVInfo.GetResponseAction();
      }

      inline Global::GuiButtonType GetButtonTypeOnRspFail() const {
          return m_EventCSVInfo.GetButtonTypeOnRspFail();
      }
      inline Global::GuiButtonType GetButtonTypeOnRspSucc() const {
          return m_EventCSVInfo.GetButtonTypeOnRspSucc();
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

      inline void SetAckValue(NetCommands::ClickedButton_t & AckVal) {
          m_AckType = AckVal;
      }

}; // end class DayEventEntry




} // end namespace DataLogging

#endif // DATALOGGING_DAYEVENTENTRY_H
