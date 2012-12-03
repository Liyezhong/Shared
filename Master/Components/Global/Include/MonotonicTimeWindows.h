/****************************************************************************/
/*! \file Global/Include/MonotonicTimeWindows.h
 *
 *  \brief Definition file for class MonotonicTimeWindows.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-02-16
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

#ifndef GLOBAL_MONOTONICTIMEWINDOWS_H
#define GLOBAL_MONOTONICTIMEWINDOWS_H

// do not remove following include since the operating system is defined in there!
#include <QtGlobal>

#if defined (Q_OS_WIN32)

#include <windows.h>

namespace Global {

/****************************************************************************/
/**
 * \brief This class can compute elapsed time since creation or call of Trigger().
 *
 * No cycle time is wasted, nobody gets blocked. It is just a method to measure
 * the timespan elapsed since last call of Trigger().
 * <b>As soon as Qt 4.7 or above is used you can replace this class by QElapsedTimer.</b>
 * \warning This class is for Windows only!
 * \warning This does not work when system is run more than 49.7 days!
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class MonotonicTimeWindows {
friend class TestMonotonicTime;
friend bool operator == (const MonotonicTimeWindows &LeftTime, const MonotonicTimeWindows &RightTime);  ///< Operator ==
friend bool operator != (const MonotonicTimeWindows &LeftTime, const MonotonicTimeWindows &RightTime);  ///< Operator !=
private:
    DWORD   m_Ticks;     ///< Trigger time (monotonic clock). May be 0 or lie in future...
    /****************************************************************************/
    /**
     * \brief Copy other instance to this instance.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    void CopyFrom(const MonotonicTimeWindows &rOther);
protected:
public:
    /****************************************************************************/
    /**
     *  \brief Default constructor.
     *
     *  Trigger is called!
     */
    /****************************************************************************/
    MonotonicTimeWindows();
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    MonotonicTimeWindows(const MonotonicTimeWindows &rOther);
    /****************************************************************************/
    /**
     *  \brief Destructor.
     */
    /****************************************************************************/
    ~MonotonicTimeWindows();
    /****************************************************************************/
    /**
     *  \brief Assignment operator.
     *
     *  \param[in]  rOther  Instance to copy from.
     *  \return             Const reference to self.
     */
    /****************************************************************************/
    const MonotonicTimeWindows & operator = (const MonotonicTimeWindows &rOther);
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
}; // end class MonotonicTimeWindows

/****************************************************************************/
/**
 * \brief Comparison operator.
 *
 * \param[in]   LeftTime    Left argument.
 * \param[in]   RightTime   Right argument.
 * \return                  True if equal.
 */
/****************************************************************************/
inline bool operator == (const MonotonicTimeWindows &LeftTime, const MonotonicTimeWindows &RightTime) {
    return LeftTime.m_Ticks == RightTime.m_Ticks;
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
inline bool operator != (const MonotonicTimeWindows &LeftTime, const MonotonicTimeWindows &RightTime) {
    return !(LeftTime == RightTime);
}

} // end namespace Global

#endif // Q_OS_WIN32

#endif // GLOBAL_MONOTONICTIMEWINDOWS_H
