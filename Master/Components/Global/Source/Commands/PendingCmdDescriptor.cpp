/****************************************************************************/
/*! \file Global/Source/Commands/PendingCmdDescriptor.cpp
 *
 *  \brief Implementation file for class PendingCmdDescriptor.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-09
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

#include <Global/Include/Commands/PendingCmdDescriptor.h>
#include <Global/Include/Utils.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>

namespace Global {

/****************************************************************************/
PendingCmdDescriptor::PendingCmdDescriptor(QObject *pParent, tRefType Ref, const QString &Name, int Timeout):
    QObject(pParent),
    m_Ref(Ref),
    m_Name(Name),
    m_Timer(this)
{
    // arm timer
    m_Timer.setSingleShot(true);
    m_Timer.setInterval(Timeout);
}

/****************************************************************************/
void PendingCmdDescriptor::ConnectAndStartTimer() {
    CONNECTSIGNALSLOT(&m_Timer, timeout(), this, OnTimeout());
    // start timer
    m_Timer.start();
}

} // end namespace Global
