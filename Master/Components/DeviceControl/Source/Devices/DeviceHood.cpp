/****************************************************************************/
/*! \file DeviceHood.cpp
 *
 *  \brief  Implementation file for class CDeviceHood
 *
 *  \version  0.1
 *  \date     2012-09-26
 *  \author   Torsten Scheck
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

#include "DeviceControl/Include/Devices/DeviceHood.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"

#include <QFinalState>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CDeviceHood
 *
 */
/****************************************************************************/
typedef CSignalTransition<CDeviceHood> CHoodTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceHood
 *
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              resuqest & response signals
 *
 *              FctModList Function module list of this device, to invoke
 *              function module interface functions.
 *
 *              InstanceID Device Instance ID.
 */
/****************************************************************************/
CDeviceHood::CDeviceHood(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                         DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID)
{

}

/****************************************************************************/
/*!
 *  \brief      To request current state of the hood.
 *
 *  \iparam     p_Event     Not used
 *
 *  \return     true, on success
 *              false, if function module fails
 */
/****************************************************************************/
bool CDeviceHood::Start(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    ReturnCode_t ReturnCode = mp_HoodSensor->ReqActInputValue();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To emit Opened signal when hood changes state from Closed to Open
 *
 *  \iparam     p_Event     Refers to the arguements sent by signal.
 *
 *  \return     true, if active hood state is opened, else false
 */
/****************************************************************************/
bool CDeviceHood::GuardCheckOpen(QEvent *p_Event)
{
    if (! CheckSignalForValue(p_Event, 1)) {
        return false;
    }
    emit Opened();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To emit Closed signal when hood changes state from Open to Close
 *
 *  \iparam     p_Event     refers to the arguements sent by signal
 *
 *  \return     true, if active hood state is closed, else false
 */
/****************************************************************************/
bool CDeviceHood::GuardCheckClosed(QEvent *p_Event)
{
    if (! CheckSignalForValue(p_Event, 0)) {
        return false;
    }
    //TODO always emit?
    emit Closed();
    return true;
}

/****************************************************************************/
/*!
 *  \brief Helper Function, to verify signal arguement. Verifies actual hood
 *          state against expected value.
 *
 *  \iparam     p_Event refers to the arguements sent by signal (ReportActInputValue)
 *              0 - Device Instance
 *              1 - Return Code
 *              2 - Input Value
 *
 *  \iparam     CheckValue  Expected value (Hood State)
 *
 *  \return     true,
 */
/****************************************************************************/
bool CDeviceHood::CheckSignalForValue(QEvent *p_Event, quint16 CheckValue)
{
    // ReportActInputValue(quint32 InstanceID, ReturnCode_t ReturnCode, quint16 InputValue)
    ReturnCode_t ReturnCode = CHoodTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    quint16 ActualValue;
    if (!CHoodTransition::GetEventValue(p_Event, 2, ActualValue)) {
        emit NeedInitialize(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (CheckValue != ActualValue) {
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To respond to IsClosed() as true, as current state is Closed.
 *
 *  \iparam     p_Event     Not Used
 *
 *  \return     true
 */
/****************************************************************************/
bool CDeviceHood::AckIsClosedTrue(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportIsClosed(DCL_ERR_FCT_CALL_SUCCESS, true);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To respond to IsClosed() as true, as current state is Open.
 *
 *  \iparam     p_Event     Not used
 *
 *  \return     true
 */
/****************************************************************************/
bool CDeviceHood::AckIsClosedFalse(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportIsClosed(DCL_ERR_FCT_CALL_SUCCESS, false);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To Report invalid state, if signal is emitted in inappropriate state
 *
 *  \iparam     p_Event     Not used
 *
 *  \return     true
 */
/****************************************************************************/
bool CDeviceHood::DefaultNackIsClosed(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportIsClosed(DCL_ERR_INVALID_STATE, false);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To configure device with appropriate function modules &
 *              definition of working state flow for Hood Device.
 *
 *  \iparam     p_Event     Not used
 *
 *  \return     true, if configuration is successful, else false
 */
/****************************************************************************/
bool CDeviceHood::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    // Get function module instances
    mp_HoodSensor = static_cast<CDigitalInput *>(m_DeviceProcessing.GetFunctionModule(
                                                 GetModuleInstanceFromKey(CANObjectKeyLUT::m_HoodSensorKey)));

    if (mp_HoodSensor == NULL)
    {
        return false;
    }

    // No Initialization needed.
    QFinalState *Init_Start = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(Init_Start);

    /////////////////////////////////////////////////////////////////
    // Working states
    /////////////////////////////////////////////////////////////////
    CState *p_Start = new CState("Start", mp_Working);
    CState *p_Checking = new CState("Checking", mp_Working);
    CState *p_Open = new CState("Open", mp_Working);
    CState *p_Closed = new CState("Closed", mp_Working);
    mp_Working->setInitialState(p_Start);

    p_Start->addTransition( new CHoodTransition (
        p_Start, SIGNAL(entered()),
        *this, &CDeviceHood::Start,
        p_Checking ));

    p_Checking->addTransition( new CHoodTransition (
        mp_HoodSensor, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
        *this, &CDeviceHood::GuardCheckOpen,
        p_Open ));
    p_Checking->addTransition( new CHoodTransition (
        mp_HoodSensor, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
        *this, &CDeviceHood::GuardCheckClosed,
        p_Closed ));

    p_Open->addTransition( new CHoodTransition (
        mp_HoodSensor, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
        *this, &CDeviceHood::GuardCheckClosed,
        p_Closed ));
    p_Closed->addTransition( new CHoodTransition (
        mp_HoodSensor, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
        *this, &CDeviceHood::GuardCheckOpen,
        p_Open ));

    p_Open->addTransition( new CHoodTransition (
        this, SIGNAL(IsClosed()),
        *this, &CDeviceHood::AckIsClosedFalse ));
    p_Closed->addTransition( new CHoodTransition (
        this, SIGNAL(IsClosed()),
        *this, &CDeviceHood::AckIsClosedTrue ));

    mp_All->addTransition( new CHoodTransition (
        this, SIGNAL(IsClosed()),
        *this, &CDeviceHood::DefaultNackIsClosed ));

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

