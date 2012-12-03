/****************************************************************************/
/*! \file Global/Source/MonotonicTimeWindows.cpp
 *
 *  \brief Implementation file for class MonotonicTimeWindows.
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

// do not remove following include since the operating system is defined in there!
#include <QtGlobal>

#if defined (Q_OS_WIN32)

#if QT_VERSION >= 0x040700
    #warning You may consider using QElapsedTimer instead of this class...
#else
    #warning This does not work when system is run more than 49.7 days!
#endif

#include <Global/Include/MonotonicTimeWindows.h>
#include <Global/Include/Exception.h>

namespace Global {

/****************************************************************************/
MonotonicTimeWindows::MonotonicTimeWindows() :
    m_Ticks(0)
{
    Trigger();
}

/****************************************************************************/
MonotonicTimeWindows::MonotonicTimeWindows(const MonotonicTimeWindows &rOther) :
    m_Ticks(0)
{
    CopyFrom(rOther);
}

/****************************************************************************/
MonotonicTimeWindows::~MonotonicTimeWindows() {
}

/****************************************************************************/
const MonotonicTimeWindows & MonotonicTimeWindows::operator = (const MonotonicTimeWindows &rOther) {
    if(this != &rOther) {
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void MonotonicTimeWindows::CopyFrom(const MonotonicTimeWindows &rOther) {
    m_Ticks = rOther.m_Ticks;
}

/****************************************************************************/
void MonotonicTimeWindows::Trigger() {
    m_Ticks = GetTickCount();
}

/****************************************************************************/
qint64 MonotonicTimeWindows::Elapsed() const {
    DWORD CurrentTime = GetTickCount();
    // compute difference CurrentTime - m_Ticks
    qint64 Diff = CurrentTime - m_Ticks;
    return Diff;
}

} // end namespace Global

#endif // Q_OS_WIN32
