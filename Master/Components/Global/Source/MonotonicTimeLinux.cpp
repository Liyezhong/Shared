/****************************************************************************/
/*! \file Global/Source/MonotonicTimeLinux.cpp
 *
 *  \brief Implementation file for class MonotonicTimeLinux.
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

// do not remove following include since the operating system is defined in there!
#include <QtGlobal>

#if defined (Q_OS_LINUX)

#if QT_VERSION >= 0x040700
    #warning You may consider using QElapsedTimer instead of this class...
#endif

#include <Global/Include/MonotonicTimeLinux.h>
#include <Global/Include/Exception.h>

namespace Global {

static timespec NULLTIME = {0, 0};  ///< For initialization

/****************************************************************************/
MonotonicTimeLinux::MonotonicTimeLinux() :
    m_Time(NULLTIME)
{
    Trigger();
}

/****************************************************************************/
MonotonicTimeLinux::MonotonicTimeLinux(const MonotonicTimeLinux &rOther) :
    m_Time(NULLTIME)
{
    CopyFrom(rOther);
}

/****************************************************************************/
MonotonicTimeLinux::~MonotonicTimeLinux() {
}

/****************************************************************************/
const MonotonicTimeLinux & MonotonicTimeLinux::operator = (const MonotonicTimeLinux &rOther) {
    if(this != &rOther) {
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void MonotonicTimeLinux::CopyFrom(const MonotonicTimeLinux &rOther) {
    m_Time = rOther.m_Time;
}

/****************************************************************************/
struct timespec MonotonicTimeLinux::GetClockTime() const {
    struct timespec Result;
    if(clock_gettime(CLOCK_MONOTONIC, &Result)) {
        // throw exception
        THROW(EVENT_GLOBAL_ERROR_MONOTONIC_CLOCK);
    }
    return Result;
}

/****************************************************************************/
void MonotonicTimeLinux::Trigger() {
    m_Time = GetClockTime();
}

/****************************************************************************/
qint64 MonotonicTimeLinux::Elapsed() const {
    struct timespec CurrentTime = GetClockTime();
    // compute difference CurrentTime - m_Time
    qint64 Diff = 1000000000*((qint64)CurrentTime.tv_sec - m_Time.tv_sec) + (CurrentTime.tv_nsec - m_Time.tv_nsec);
    Diff = (Diff / 1000) / 1000; // to ms
    return Diff;
}

} // end namespace Global

#endif // Q_OS_LINUX
