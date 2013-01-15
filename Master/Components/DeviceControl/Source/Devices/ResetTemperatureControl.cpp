/****************************************************************************/
/*! \file   ResetTemperatureControl.cpp
 *
 *  \brief  Implementaion file for class CResetTemperatureControl
 *
 *  \version  0.1
 *  \date     2012-01-14
 *  \author   M.Scherer
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include "DeviceControl/Include/Devices/ResetTemperatureControl.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CResetTemperatureControl
typedef CSignalTransition<CResetTemperatureControl> CResetTemperatureControlTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CResetTemperatureControl
 *
 *  \iparam p_TemperatureControl = Module whose data is reset
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CResetTemperatureControl::CResetTemperatureControl(CTemperatureControl *p_TemperatureControl,
                                                   const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent), mp_TemperatureControl(p_TemperatureControl)
{
    CState *p_Init = new CState("Init", this);
    CState *p_ResetHeaterOperatingTime = new CState("ResetHeaterOperatingTime", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CResetTemperatureControlTransition(
        p_Init, SIGNAL(entered()),
        *this, &CResetTemperatureControl::ResetHeaterOperatingTime,
        p_ResetHeaterOperatingTime));
    p_ResetHeaterOperatingTime->addTransition(new CResetTemperatureControlTransition(
        mp_TemperatureControl, SIGNAL(ReportResetHeaterOperatingTime(quint32, ReturnCode_t, quint8)),
        *this, &CResetTemperatureControl::ReportResetHeaterOperatingTime,
        p_Final));
}

/****************************************************************************/
/*!
 *  \brief  Sends a data reset request command to the module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CResetTemperatureControl::ResetHeaterOperatingTime(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_TemperatureControl->ResetHeaterOperatingTime(0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Receives a data reset acknowledgement
 *
 *  \iparam p_Event = Parameters of the signal ReportResetHeaterOperatingTime
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CResetTemperatureControl::ReportResetHeaterOperatingTime(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = CResetTemperatureControlTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
