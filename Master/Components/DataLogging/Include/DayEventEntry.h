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
class DayEventEntry : public EventHandler::EventCSVInfo {
    friend class TestDayOperationEntry;
private:

    Global::tTranslatableStringList m_String;                ///< The translatable string.
    bool                            m_EventStatus;           ///< true/false - Set/Reset flag for the event
    bool                            m_Active;		     ///< true - Error status is active
    Global::tRefType                m_Ref;                   ///< Ref for the acknowledgement received
    NetCommands::ClickedButton_t    m_AckType;               ///< Ack status received from GUI
//    Global::AckOKNOK                m_AckValue;              ///< Ack value received from GUI
    quint32                         m_EventKey;              ///< Event Key for every event raised. NULL until raised.
    QDateTime                       m_TimeStamp;             ///< TimeStamp for entry.
    quint8                          m_count;                 ///< Number of times the event has occured
    bool                            m_ShowInRunLogStatus;


    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \param[in]   rOther  Const reference to other instance.
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
                                 bool Active,const Global::tTranslatableStringList &String, quint8 count,
                                 NetCommands::ClickedButton_t ClickButton, /*Global::AckOKNOK AckValue,*/ Global::tRefType Ref);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    DayEventEntry(const DayEventEntry &rOther);
    /****************************************************************************/
    /**
     * \brief Constructor with a translatable string.
     *
     * \param[in]   TimeStamp       Timestamp.
     * \param[in]   String          The complete translatable string.
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
     * \param[in]   rOther  Const reference to other instance.
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
          return (GetRunLogStatus());

      }



      void SetEventCSVInfo(EventCSVInfo CSVInfo) ;

      /****************************************************************************/
      /**
       * \brief Get the Event Status
       *
       * \return Event Status as const ref.
       */
      /****************************************************************************/
      inline bool IsEventActive() const {
          return m_Active;
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

   inline void SetActive(const bool & Active) {
          m_Active = Active;
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
