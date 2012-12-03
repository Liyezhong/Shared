/****************************************************************************/
/*! \file Global/Include/AdjustedTime.h
 *
 *  \brief Definition file for class AdjustedTime.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-01-24
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

#ifndef GLOBAL_ADJUSTEDTIME_H
#define GLOBAL_ADJUSTEDTIME_H

#include <QDateTime>
#include <QReadWriteLock>

namespace Global {

/****************************************************************************/
/**
 * \brief AdjustedTime class.
 *
 * This class is used to get an adjusted date and time. Adjusted means that
 * there is an offset (in seconds) which is added to the system UTC time.
 * The offset may be negative!
 */
/****************************************************************************/
class AdjustedTime {
friend class TestAdjustedTime;
private:
    static AdjustedTime         m_Instance;         ///< The instance.
    int                         m_OffsetSeconds;    ///< Offset to add to current time [s].
    mutable QReadWriteLock      m_SyncObject;       ///< Synchronisation object.
    /****************************************************************************/
    AdjustedTime(const AdjustedTime &);                     ///< Not implemented.
    const AdjustedTime & operator = (const AdjustedTime &); ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * Sets offset to 0.
     */
    /****************************************************************************/
    AdjustedTime();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~AdjustedTime();
protected:
public:
    /****************************************************************************/
    /**
     * \brief Get reference to instance.
     *
     * \return      Reference to instance.
     */
    /****************************************************************************/
    static inline AdjustedTime &Instance() {
        return m_Instance;
    }
    /****************************************************************************/
    /**
     * \brief Set offset seconds.
     *
     * \param[in]   OffsetSeconds   Offset in seconds. May be negative.
     */
    /****************************************************************************/
    inline void SetOffsetSeconds(int OffsetSeconds) {
        QWriteLocker WL(&m_SyncObject);
        m_OffsetSeconds = OffsetSeconds;
    }
    /****************************************************************************/
    /**
     * \brief Get current offset seconds.
     *
     * \return  Current offset in seconds.
     */
    /****************************************************************************/
    inline int GetOffsetSeconds() const {
        QReadLocker RL(&m_SyncObject);
        return m_OffsetSeconds;
    }
    /****************************************************************************/
    /**
     * \brief Compute offset in seconds between current date time and another date time.
     *
     * \param[in]   DateTime    DateTime to which the offset has to be computed.
     * \return                  Offset in seconds.
     */
    /****************************************************************************/
    static inline int ComputeOffsetSeconds(const QDateTime &DateTime) {
        return QDateTime::currentDateTime().toUTC().secsTo(DateTime.toUTC());
    }
    /****************************************************************************/
    /**
     * \brief Get current datetime (adjusted)
     *
     * Gets the current datetime, transforms it to UC and adds the offset seconds.
     *
     * \return      Adjusted datetime.
     */
    /****************************************************************************/
    inline QDateTime GetCurrentDateTime() const {
        QReadLocker RL(&m_SyncObject);
        // get current datetime, transform it to UTC and add some seconds.
        return QDateTime::currentDateTime().toUTC().addSecs(m_OffsetSeconds);
    }
    /****************************************************************************/
    /**
     * \brief Get current date (adjusted)
     *
     * Gets the current datetime and extracts the date.
     *
     * \return      Adjusted date.
     */
    /****************************************************************************/
    inline QDate GetCurrentDate() const {
        return GetCurrentDateTime().date();
    }
    /****************************************************************************/
    /**
     * \brief Get current time (adjusted)
     *
     * Gets the current datetime and extracts the time.
     *
     * \return      Adjusted time.
     */
    /****************************************************************************/
    inline QTime GetCurrentTime() const {
        return GetCurrentDateTime().time();
    }
    /****************************************************************************/
    /**
     * \brief Adjust offset to fit to new datetime.
     *
     * Adjust m_OffsetSeconds so that the difference between current datetime and
     * NewDateTime fits again.
     *
     * \param[in]   NewDateTime     New datetime for which offset has to be computed.
     */
    /****************************************************************************/
    inline void AdjustToDateTime(const QDateTime &NewDateTime) {
        // compute number of seconds between current datetime and NewDateTime
        // and set it.
        SetOffsetSeconds(QDateTime::currentDateTime().toUTC().secsTo(NewDateTime.toUTC()));
    }
}; // end class AdjustedTime

} // end namespace Global

#endif // GLOBAL_ADJUSTEDTIME_H
