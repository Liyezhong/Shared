/****************************************************************************/
/*! \file Global/Source/Commands/CmdPowerFail.cpp
 *
 *  \brief Implementation file for class CmdPowerFail.
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

#include <Global/Include/Commands/CmdPowerFail.h>

namespace Global {

QString CmdPowerFail::NAME = "Global::CmdPowerFail";

/****************************************************************************/
CmdPowerFail::CmdPowerFail() :
    Command(NOTIMEOUT)
{
}

/****************************************************************************/
CmdPowerFail::CmdPowerFail(const CmdPowerFail &rOther) :
    Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
CmdPowerFail::~CmdPowerFail() {
}

/****************************************************************************/
const CmdPowerFail & CmdPowerFail::operator = (const CmdPowerFail &rOther) {
    if(this != &rOther) {
        Command::operator =(rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void CmdPowerFail::CopyFrom(const CmdPowerFail &/*rOther*/) {
}

} // end namespace Global
