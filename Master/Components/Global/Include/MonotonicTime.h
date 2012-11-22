/****************************************************************************/
/*! \file Global/Include/MonotonicTime.h
 *
 *  \brief Definition file for class MonotonicTime.
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

#ifndef GLOBAL_MONOTONICTIME_H
#define GLOBAL_MONOTONICTIME_H

// do not remove following include since the operating system is defined in there!
#include <QtGlobal>

#if defined (Q_OS_LINUX)
    #include <Global/Include/MonotonicTimeLinux.h>
#elif defined (Q_OS_WIN32)
    #include <Global/Include/MonotonicTimeWindows.h>
#else
    #error Unsupported operating system
#endif

namespace Global {

#if defined (Q_OS_LINUX)
    typedef MonotonicTimeLinux MonotonicTime;
#elif defined (Q_OS_WIN32)
    typedef MonotonicTimeWindows MonotonicTime;
#else
    #error Unsupported operating system
#endif

} // end namespace Global

#endif // GLOBAL_MONOTONICTIME_H
