/****************************************************************************/
/*! \file Global/Source/Commands/CmdDataChanged.cpp
 *
 *  \brief Implementation file for class CmdDataChanged.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-03-29
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

#include <Global/Include/Commands/CmdDataChanged.h>

namespace Global {

QString CmdDataChanged::NAME = "Global::CmdDataChanged";

/****************************************************************************/
CmdDataChanged::CmdDataChanged(const QString &DataContainerName) :
    Command(NOTIMEOUT),
    m_DataContainerName(DataContainerName)
{
}

/****************************************************************************/
CmdDataChanged::CmdDataChanged(const CmdDataChanged &rOther) :
    Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
CmdDataChanged::~CmdDataChanged() {
}

/****************************************************************************/
const CmdDataChanged & CmdDataChanged::operator = (const CmdDataChanged &rOther) {
    if(this != &rOther) {
        Command::operator =(rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void CmdDataChanged::CopyFrom(const CmdDataChanged &rOther) {
    m_DataContainerName = rOther.m_DataContainerName;
}

} // end namespace Global
