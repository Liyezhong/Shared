/****************************************************************************/
/*! \file Final.cpp
 *
 *  \brief Implementation file for class Final.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.12.2010
 *   $Author:  $ Y.Novak
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

#include <QDebug>
#include <ExternalProcessController/Include/ExternalProcessStates/Final.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    wObj = pointer to working object / parent
*
****************************************************************************/
Final::Final(ExternalProcessController *wObj) :
        State::State("Final", wObj),
        m_myController(wObj)
{
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
Final::~Final()
{
    m_myController = NULL;
}

/****************************************************************************/
/*!
*  \brief    This function is called when the State is entered.
*
*  \param[in]    et = the event to process
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool Final::OnEntry(StateMachines::StateEvent et)
{
    qDebug() << "FinalState entered.";

    if (m_myController == NULL) {
        /// \todo log error
        /// \todo what here?
        return false;
    }

    return HandleEvent(et);
}

/****************************************************************************/
/*!
*  \brief    This function is called when the State is exited.
*
*  \param[in]    et = the event to process
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool Final::OnExit(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "FinalState exited.";
    return true;
}

/****************************************************************************/
/*!
*  \brief    This function processes state's internal event.
*
*  \param[in]    et = the event to process
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool Final::HandleEvent(StateMachines::StateEvent et)
{
    qDebug() << "FinalState handles internal event " + QString::number(et.GetIndex(), 10);

    if (m_myController == NULL) {
        /// \todo log error
        return false;
    }

    switch (et.GetIndex())
    {
    case EP_EXTPROCESS_CONNECTED:
        m_myController->m_myDevice->DisconnectPeer();
        break;
    default:
        break;
    }
    return true;
}

} // end namespace ExternalProcessControl
