/****************************************************************************/
/*! \file PlatformEventCodes.h
 *
 *  \brief Definition of event groups for event codes.
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

#ifndef PLATFORMEVENTCODES_H
#define PLATFORMEVENTCODES_H

#include <QtGlobal>
const quint32 EVENT_GROUP_SOURCENAME                        = 0x0000;
#include "../../../Himalaya/HimalayaMain/Master/Include/HimalayaEventCodes.h"


// Note : Add event groups for your platform components here
/// Event group for platform
const quint32 EVENT_GROUP_PLATFORM                          = 0x01;

/// Event group for global stuff
const quint32 EVENT_GROUP_PLATFORM_GLOBAL                   = ((EVENT_GROUP_PLATFORM << 8) + 0x01) << 16;
/// Event group for thread stuff
const quint32 EVENT_GROUP_PLATFORM_THREADS                  = ((EVENT_GROUP_PLATFORM << 8) + 0x02) << 16;
/// Event group for data logging
const quint32 EVENT_GROUP_PLATFORM_DATALOGGING              = ((EVENT_GROUP_PLATFORM << 8) + 0x03) << 16;
/// Event group for event handler.
const quint32 EVENT_GROUP_PLATFORM_EVENTHANDLER             = ((EVENT_GROUP_PLATFORM << 8) + 0x04) << 16;



const quint32 EVENT_INIT_FAILED                     = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0005; ///< Event for Init failed
const quint32 EVENT_INIT_SUCCESS                    = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0006; ///< Event for Init Success


#endif // namespace PLATFORMEVENTCODES_H
