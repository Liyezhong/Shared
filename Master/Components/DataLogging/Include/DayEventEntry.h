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
class DayEventEntry {
friend class TestDayOperationEntry;
private:
    QDateTime                       m_TimeStamp;    ///< TimeStamp for entry.
    quint32                         m_EventCode;    ///< Event code/Event ID for event entry - Also used as String ID to get corresponding string for translation
    Global::EventType               m_EventType;    ///< Event type/level of event entry.
    Global::tTranslatableStringList m_String;       ///< The translatable string.
    QString                         m_NumericData;  ///< Any numeric data that needs to be stored
    bool                            m_ShowInRunLog; ///< true -show in GUI its and daily run log, else dont show its error log.
    Global::LoggingSource           m_Source;       ///< Source for event entry.
    Global::LogLevel                m_LogLevel;     ///< Various log levels
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
     * \brief Constructor with a translatable string.
     *
     * \param[in]   TimeStamp       Timestamp.
     * \param[in]   EventCode       EventID
     * \param[in]   EventType       Fatal/Warning/Info
     * \param[in]   String          The complete translatable string.
     * \param[in]   NumericData     Numeric data for logging
     * \param[in]   ShowInRunLog    true - show , false - hide
     * \param[in]   LoggingSource   Source -Scheduler/main controller etc
     */
    /****************************************************************************/
    DayEventEntry(const QDateTime &TimeStamp, const quint32 &EventCode,
                                         const Global::EventType &EventType, const Global::tTranslatableStringList &String,
                                         const QString &NumericData, const bool &ShowInRunLog,
                                         const Global::LoggingSource &LoggingSource, const Global::LogLevel &LogLevel);
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
    /**
     * \brief Get the timestamp.
     *
     * \return      Timestamp.
     */
    /****************************************************************************/
    inline QDateTime GetTimeStamp() const {
        return m_TimeStamp;
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
     * \brief Get status of ShowInRunLogStatus
     *
     * \return ShowInRunLog flag.
     */
    /****************************************************************************/
    inline bool GetShowInRunLogStatus() const {
        return m_ShowInRunLog;
    }

    /****************************************************************************/
    /**
     * \brief Get the source of the event.
     *
     * \return Event source.
     */
    /****************************************************************************/
    inline QString GetNumericData() const{
        return m_NumericData;
    }

    /****************************************************************************/
    /**
     * \brief Dump content to console
     */
    /****************************************************************************/
    void DumpToConsole() const;

}; // end class DayEventEntry

} // end namespace DataLogging

#endif // DATALOGGING_DAYEVENTENTRY_H
