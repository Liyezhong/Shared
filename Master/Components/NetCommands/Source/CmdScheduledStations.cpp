/****************************************************************************/
/*! \file CmdScheduledStations.cpp
 *
 *  \brief Implementation file for class CmdScheduledStations.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-06
 *  $Author:    $ N.Kamath
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <NetCommands/Include/CmdScheduledStations.h>

namespace NetCommands {

QString CmdScheduledStations::NAME           = "NetCommands::CmdScheduledStations";

/****************************************************************************/
CmdScheduledStations::CmdScheduledStations(int TimeOut) :
    Global::Command(TimeOut)
{
}

/****************************************************************************/
CmdScheduledStations::CmdScheduledStations():
    Global::Command(0)
{

}

/****************************************************************************/
CmdScheduledStations::~CmdScheduledStations() {
}

} // end namespace NetCommands


