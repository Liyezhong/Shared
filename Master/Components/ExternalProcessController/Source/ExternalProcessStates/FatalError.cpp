/****************************************************************************/
/*! \file FatalError.cpp
 *
 *  \brief Implementation file for class FatalError.
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
#include <ExternalProcessController/Include/ExternalProcessStates/FatalError.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    wObj = pointer to working object / parent
*
****************************************************************************/
FatalError::FatalError(ExternalProcessController *wObj) :
        State::State("FatalError", wObj),
        m_myController(wObj)
{
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
*
****************************************************************************/
FatalError::~FatalError()
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
bool FatalError::OnEntry(StateMachines::StateEvent et)
{
    qDebug() << "FatalErrorState entered.";

    if (m_myController == NULL) {
        /// \todo log error
        /// \todo we cannot switch further, do something here :)
        return false;
    }

    // stop device's powerup login timer if it is runnning
    m_myController->m_myDevice->StopLoginGuard();

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
bool FatalError::OnExit(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "FatalErrorState exited.";
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
bool FatalError::HandleEvent(StateMachines::StateEvent et)
{
    qDebug() << "FatalErrorState handles internal event " + QString::number(et.GetIndex(), 10);

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
