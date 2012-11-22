/****************************************************************************/
/*! \file DataLogging/Include/EventEntry.h
 *
 *  \brief Definition file for class EventEntry.
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

#ifndef DATALOGGING_EVENTENTRY_H
#define DATALOGGING_EVENTENTRY_H

#include <QDateTime>

#include <Global/Include/LoggingSource.h>
#include <Global/Include/TranslatableString.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to deliver the content of an event entry.
 *
 * This class can be used to deliver the content for an event entry using the
 * signal / slot mechanism. It contains timestamp, source, event type, event code
 * and additional event data.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class EventEntry {
friend class TestEventEntry;
private:
    QDateTime                       m_TimeStamp;            ///< TimeStamp for event entry.
    Global::LoggingSource           m_Source;               ///< Source for event entry.
    Global::EventType               m_EventType;            ///< Event type of event entry.
    Global::EventStatus             m_EventStatus;          ///< Status of event: ON / OFF.
    quint32                         m_EventCode;            ///< Event code for event entry.
    Global::tTranslatableStringList m_AdditionalData;       ///< Additional data for event entry.
    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const EventEntry &rOther);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Overloaded assignment operator.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    const EventEntry & operator = (const EventEntry &);
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * Implemented due to signal / slot mechanism.
     */
    /****************************************************************************/
    EventEntry();
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * Implemented due to signal / slot mechanism.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    EventEntry(const EventEntry &rOther);
    /****************************************************************************/
    /**
     * \brief Constructor without an additional data list.
     *
     * This constructor is usefull for event entries without additional data.
     *
     * \param[in]   TimeStamp       Timestamp of event.
     * \param[in]   Source          Source of the event.
     * \param[in]   TheEventType    Type of event.
     * \param[in]   TheEventStatus  Event status.
     * \param[in]   EventCode       Event code.
     */
    /****************************************************************************/
    EventEntry(const QDateTime &TimeStamp, const Global::LoggingSource &Source, Global::EventType TheEventType,
               Global::EventStatus TheEventStatus, quint32 EventCode);
    /****************************************************************************/
    /**
     * \brief Constructor with an additional data list.
     *
     * This constructor is usefull for event entries with additional data.
     *
     * \param[in]   TimeStamp       Timestamp of event.
     * \param[in]   Source          Source of the event.
     * \param[in]   TheEventType    Type of event.
     * \param[in]   TheEventStatus  Event status.
     * \param[in]   EventCode       Event code.
     * \param[in]   AdditionalData  Additional data for event.
     */
    /****************************************************************************/
    EventEntry(const QDateTime &TimeStamp, const Global::LoggingSource &Source, Global::EventType TheEventType,
               Global::EventStatus TheEventStatus, quint32 EventCode,
               const Global::tTranslatableStringList &AdditionalData);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~EventEntry();
    /****************************************************************************/
    /**
     * \brief Get the timestamp of event entry.
     *
     * \return      Timestamp.
     */
    /****************************************************************************/
    inline QDateTime GetTimeStamp() const {
        return m_TimeStamp;
    }
    /****************************************************************************/
    /**
     * \brief Get the source of the event.
     *
     * \return Event source.
     */
    /****************************************************************************/
    inline Global::LoggingSource GetSource() const {
        return m_Source;
    }
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
    /****************************************************************************/
    /**
     * \brief Get the event status.
     *
     * \return      Event status.
     */
    /****************************************************************************/
    inline Global::EventStatus GetEventStatus() const {
        return m_EventStatus;
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
    /****************************************************************************/
    /**
     * \brief Get the additional event data of event entry as const reference.
     *
     * \return      Additional event data as const reference.
     */
    /****************************************************************************/
    inline const Global::tTranslatableStringList &GetAdditionalData() const {
        return m_AdditionalData;
    }
    /****************************************************************************/
    /**
     * \brief Dump content to console
     */
    /****************************************************************************/
    void DumpToConsole() const;
}; // end class EventEntry

} // end namespace DataLogging

#endif // _EVENTENTRY_H_
