/****************************************************************************/
/*! \file ExternalProcessControlEventCodes.h
 *
 *  \brief All event codes used by external process controller.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-06-15
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

#ifndef DATAMANAGEMENT_EXTERNALPROCESSCONTROLEVENTCODES_H
#define DATAMANAGEMENT_EXTERNALPROCESSCONTROLEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

namespace ExternalProcessControl {

const quint32 EVENT_EXTERNALPROCESSCONTROL_ERROR_LOADING_SETTINGS                   = EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL + 0x0001; ///< Error loading settings.
const quint32 EVENT_EXTERNALPROCESSCONTROL_ERROR_INITIALIZE_EXTERNAL_PROCESS_DEVICE = EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL + 0x0002; ///< Error initializing external process device.
const quint32 EVENT_EXTERNALPROCESSCONTROL_ERROR_INITIALIZE_SIMPLE_STATE_MACHINE    = EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL + 0x0003; ///< Error initializing simple state machine.
const quint32 EVENT_EXTERNALPROCESSCONTROL_ERROR_INITIALIZE_FULL_STATE_MACHINE      = EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL + 0x0004; ///< Error initializing full state machine.
const quint32 EVENT_EXTERNALPROCESSCONTROL_ERROR_INITIALIZE_PROCESS_MANAGER         = EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL + 0x0005; ///< Error initializing process manager.
const quint32 EVENT_EXTERNALPROCESSCONTROL_ERROR_CREATOR_FUNCTOR_ALREADY_REGISTERED = EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL + 0x0006; ///< Creator functor already registered for: '%1'.
const quint32 EVENT_EXTERNALPROCESSCONTROL_ERROR_NO_CREATOR_FUNCTOR_REGISTERED      = EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL + 0x0007; ///< Error no creator functor registered for class: '%1'.
const quint32 EVENT_EXTERNALPROCESSCONTROL_ERROR_REFERENCE_ALREADY_REGISTERED       = EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL + 0x0008; ///< Error reference already registered '%1' for command '%2'.
const quint32 EVENT_EXTERNALPROCESSCONTROL_ERROR_UNKNOWN_REFERENCE                  = EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL + 0x0009; ///< Error unknown reference '%1' for acknowledge '%2'.

} // end namespace ExternalProcessControl

#endif // DATAMANAGEMENT_EXTERNALPROCESSCONTROLEVENTCODES_H
