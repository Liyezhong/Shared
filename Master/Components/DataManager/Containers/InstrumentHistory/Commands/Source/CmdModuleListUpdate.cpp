/****************************************************************************/
/*! \file CmdModuleListUpdate.cpp
 *
 *  \brief CmdModuleListUpdate command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 14.01.2013
 *   $Author:  $ Soumya. D
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

#include <QBuffer>

#include <Global/Include/Exception.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/Utils.h>

#include "DataManager/Containers/InstrumentHistory/Commands/Include/CmdModuleListUpdate.h"

namespace MsgClasses {

QString CmdModuleListUpdate::NAME = "MsgClasses::CmdModuleListUpdate";

/****************************************************************************/
/*!
 * \brief   Constructor for sending
 *
 * \iparam   Timeout             Timeout for command.
 * \iparam   Module
 * \iparam   DeviceName          Name of the device for which information is
 *                               being sent
 * \iparam   Error               set to true if retrieving module information
 *                               had failed ,else false.
 */
/****************************************************************************/
CmdModuleListUpdate::CmdModuleListUpdate(int Timeout, const DataManager::CModule& Module, const QString& DeviceType, const bool Error) :
    Command(Timeout), m_DeviceType(DeviceType), m_Error(Error), mp_ModuleData(NULL)
{   
    mp_ModuleData = new DataManager::CModule();
    *mp_ModuleData = Module;
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdModuleListUpdate::CmdModuleListUpdate() : Command(0), mp_ModuleData(NULL)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdModuleListUpdate::~CmdModuleListUpdate()
{
    if (mp_ModuleData != NULL) {
        try {
             delete mp_ModuleData;
        }
        CATCHALL_DTOR();
        mp_ModuleData = NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdModuleListUpdate::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses
