/****************************************************************************/
/*! \file Global/Include/MonotonicTimeLinux.h
 *
 *  \brief Definition file for class MonotonicTimeLinux.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-09-20
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

#ifndef GLOBAL_MONOTONICTIMELINUX_H
#define GLOBAL_MONOTONICTIMELINUX_H

// do not remove following include since the operating system is defined in there!
#include <QtGlobal>

#if defined (Q_OS_LINUX)

#include <time.h>

namespace Global {

/****************************************************************************/
/**
 * \brief This class can compute elapsed time since creation or call of Trigger().
 *
 * No cycle time is wasted, nobody gets blocked. It is just a method to measure
 * the timespan elapsed since last call of Trigger().
 * <b>As soon as Qt 4.7 or above is used you can replace this class by QElapsedTimer.</b>
 * \warning This class is for Linux only!
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class MonotonicTimeLinux {
friend class TestMonotonicTime;
friend bool operator == (const MonotonicTimeLinux &LeftTime, const MonotonicTimeLinux &RightTime);  ///< Operator ==
friend bool operator != (const MonotonicTimeLinux &LeftTime, const MonotonicTimeLinux &RightTime);  ///< Operator !=
private:
    struct timespec     m_Time;     ///< Trigger time (monotonic clock). May be 0 or lie in future...
    /****************************************************************************/
    /**
     * \brief Copy other instance to this instance.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    void CopyFrom(const MonotonicTimeLinux &rOther);
    /****************************************************************************/
    /**
     * \brief Get monotonic clock.
     *
     * \return  Monotonic clock time.
     */
    /****************************************************************************/
    struct timespec GetClockTime() const;
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * Trigger is called!
     */
    /****************************************************************************/
    MonotonicTimeLinux();
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    MonotonicTimeLinux(const MonotonicTimeLinux &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~MonotonicTimeLinux();
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Instance to copy from.
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const MonotonicTimeLinux & operator = (const MonotonicTimeLinux &rOther);
    /****************************************************************************/
    /**
     * \brief Trigger.
     *
     * Sets \ref m_Time to current time.
     **/
    /****************************************************************************/
    void Trigger();
    /****************************************************************************/
    /**
     * \brief Get difference between now and \ref m_Time in ms.
     *
     * \return  Difference in milliseconds. May be < 0 if \ref m_Time lies in future.
     */
    /****************************************************************************/
    qint64 Elapsed() const;
}; // end class MonotonicTimeLinux

/****************************************************************************/
/**
 * \brief Comparison operator.
 *
 * \param[in]   LeftTime    Left argument.
 * \param[in]   RightTime   Right argument.
 * \return                  True if equal.
 */
/****************************************************************************/
inline bool operator == (const MonotonicTimeLinux &LeftTime, const MonotonicTimeLinux &RightTime) {
    return (LeftTime.m_Time.tv_sec == RightTime.m_Time.tv_sec) && (LeftTime.m_Time.tv_nsec == RightTime.m_Time.tv_nsec);
}

/****************************************************************************/
/**
 * \brief Comparison operator.
 *
 * \param[in]   LeftTime    Left argument.
 * \param[in]   RightTime   Right argument.
 * \return                  True if not equal.
 */
/****************************************************************************/
inline bool operator != (const MonotonicTimeLinux &LeftTime, const MonotonicTimeLinux &RightTime) {
    return !(LeftTime == RightTime);
}

} // end namespace Global

#endif // Q_OS_LINUX

#endif // GLOBAL_MONOTONICTIMELINUX_H
