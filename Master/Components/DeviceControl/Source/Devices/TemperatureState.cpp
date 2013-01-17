/****************************************************************************/
/*! \file   TemperatureState.cpp
 *
 *  \brief  Implementaion file for class CTemperatureState
 *
 *  \version  0.1
 *  \date     2012-11-21
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

#include "DeviceControl/Include/Devices/DeviceBase.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/Devices/TemperatureState.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"

#include <QStateMachine>

namespace DeviceControl
{

//! Signal transition for CTemperatureState
typedef CSignalTransition<CTemperatureState> CTemperatureTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CTemperatureState
 *
 *  \iparam p_TemperatureControl = Temperature control function module
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CTemperatureState::CTemperatureState(CTemperatureControl *p_TemperatureControl, const QString &Name,
                                     QState *p_Parent) :
    CState(Name, p_Parent), mp_TemperatureControl(p_TemperatureControl)
{
    qRegisterMetaType<TempCtrlStatus_t>("TempCtrlStatus_t");

    //lint -esym(1524, p_Off)
    CState *p_Off = new CState("Off", this);
    CState *p_On = new CState("On", this);
    setInitialState(p_Off);

    addTransition(new CTemperatureTransition(
            this, SIGNAL(SetState(bool, qreal)),
            *this, &CTemperatureState::TransSetState,
            this));

    p_Off->addTransition(new CTemperatureTransition(
        mp_TemperatureControl, SIGNAL(ReportSetStatusAckn(quint32, ReturnCode_t, TempCtrlStatus_t, qreal)),
        *this, &CTemperatureState::TransOffOn,
        p_On));

    p_On->addTransition(new CTemperatureTransition(
        mp_TemperatureControl, SIGNAL(ReportSetStatusAckn(quint32, ReturnCode_t, TempCtrlStatus_t, qreal)),
        *this, &CTemperatureState::TransOnOff,
        p_Off));

    p_On->addTransition(mp_TemperatureControl, SIGNAL(ReportEvent(quint32, quint16, QDateTime)), p_Off);

    p_On->addTransition(new CTemperatureTransition(
        this, SIGNAL(GetTemperature()),
        *this, &CTemperatureState::TransGetTemperature,
        p_On));

    p_Off->addTransition(new CTemperatureTransition(
        this, SIGNAL(GetTemperature()),
        *this, &CTemperatureState::TransGetTemperatureError,
        p_Off));

    connect(mp_TemperatureControl, SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
            this, SLOT(OnGetTemperature(quint32, ReturnCode_t, quint8, qreal)));
    connect(p_On, SIGNAL(entered()), this, SIGNAL(On()));
    connect(p_Off, SIGNAL(entered()), this, SIGNAL(Off()));
}

/****************************************************************************/
/*!
 *  \brief  Emits the set state signal
 *
 *  \iparam Enable = Activates or deactivates the temperature control
 *  \iparam Temperature = Temperature of the oven to be set (°C)
 */
/****************************************************************************/
void CTemperatureState::EmitSetState(bool Enable, qreal Temperature)
{
    emit SetState(Enable, Temperature);
}

/****************************************************************************/
/*!
 *  \brief  Emits the get state signal
 */
/****************************************************************************/
void CTemperatureState::EmitGetTemperature()
{
    emit GetTemperature();
}

/****************************************************************************/
/*!
 *  \brief  Called when the set state interface signal is emitted
 *
 *  \iparam p_Event = Parameters of the signal SetState
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CTemperatureState::TransSetState(QEvent *p_Event)
{
    bool Enable;
    qreal Temperature;

    if (!CTemperatureTransition::GetEventValue(p_Event, 0, Enable)) {
        emit ReportSetState(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CTemperatureTransition::GetEventValue(p_Event, 1, Temperature)) {
        emit ReportSetState(DCL_ERR_INVALID_PARAM);
        return false;
    }

    TempCtrlStatus_t Status = Enable ? TEMPCTRL_STATUS_ON : TEMPCTRL_STATUS_OFF;
    ReturnCode_t ReturnCode = mp_TemperatureControl->SetStatus(Status, Temperature);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportSetState(ReturnCode);
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set status signal is emitted
 *
 *      This function is called, when the module should be switched from off
 *      to on.
 *
 *  \iparam p_Event = Parameters of the signal ReportSetStateAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CTemperatureState::TransOffOn(QEvent *p_Event)
{
    return TransSwitch(p_Event, TEMPCTRL_STATUS_ON);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set status signal is emitted
 *
 *      This function is called, when the module should be switched from on
 *      to off.
 *
 *  \iparam p_Event = Parameters of the signal ReportSetStateAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CTemperatureState::TransOnOff(QEvent *p_Event)
{
    return TransSwitch(p_Event, TEMPCTRL_STATUS_OFF);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set status signal is emitted
 *
 *      This function is called, when the module should be switched to the
 *      target status.
 *
 *  \iparam p_Event = Parameters of the signal ReportSetStateAckn
 *  \iparam TargetStatus = Tells if the module should be switched on or off
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CTemperatureState::TransSwitch(QEvent *p_Event, TempCtrlStatus_t TargetStatus)
{
    ReturnCode_t ReturnCode;
    TempCtrlStatus_t Status;

    if (!CTemperatureTransition::GetEventValue(p_Event, 1, ReturnCode)) {
        emit ReportSetState(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CTemperatureTransition::GetEventValue(p_Event, 2, Status)) {
        emit ReportSetState(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportSetState(ReturnCode);
        return false;
    }
    if (Status != TargetStatus) {
        emit ReportSetState(ReturnCode);
        return false;
    }

    emit ReportSetState(ReturnCode);
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Called when the report get temperature signal is emitted
 *
 *  \iparam p_Event = Parameters of the signal GetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CTemperatureState::TransGetTemperature(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_TemperatureControl->ReqActTemperature(0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportGetTemperature(ReturnCode, 0);
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when the report get temperature signal is emitted
 *
 *      This is the default transition which is called when getting the
 *      temperature is not allowed, e.g. the module is in off state.
 *
 *  \iparam p_Event = Parameters of the signal GetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CTemperatureState::TransGetTemperatureError(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportGetTemperature(DCL_ERR_INVALID_STATE, 0);
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when the report report temperature signal is emitted
 *
 *  \iparam p_Event = Parameters of the signal ReportActTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
void CTemperatureState::OnGetTemperature(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, qreal Temperature)
{
    Q_UNUSED(InstanceID)
    Q_UNUSED(Index)

    emit ReportGetTemperature(HdlInfo, Temperature);
}

} //namespace
