/****************************************************************************/
/*! \file DataLogging/Source/Commands/CmdForceCaching.cpp
 *
 *  \brief Implementation file for class CmdForceCaching.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-04-28
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

#include <DataLogging/Include/Commands/CmdForceCaching.h>

namespace DataLogging {

QString CmdForceCaching::NAME = "DataLogging::CmdForceCaching";

/****************************************************************************/
CmdForceCaching::CmdForceCaching(int Timeout, bool ForceCaching) :
    Command(Timeout),
    m_ForceCaching(ForceCaching)
{
}

/****************************************************************************/
CmdForceCaching::~CmdForceCaching() {
}

} // end namespace DataLogging
