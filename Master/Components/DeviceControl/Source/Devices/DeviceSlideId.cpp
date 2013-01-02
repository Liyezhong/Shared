/****************************************************************************/
/*! \file   DeviceSlideId.cpp
 *
 *  \brief  Implementaion file for class CDeviceSlideId
 *
 *  \version  0.1
 *  \date     2012-12-06
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

#include "DeviceControl/Include/Devices/DeviceSlideId.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"

#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CDeviceSlideId
typedef CSignalTransition<CDeviceSlideId> CSlideIdTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CDeviceSlideId
 *
 *  \iparam DeviceProc = Device processing layer
 *  \iparam ModuleList = List of slave modules used by the device
 *  \iparam InstanceID = Numerical instance ID of the device
 */
/****************************************************************************/
CDeviceSlideId::CDeviceSlideId(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                               DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID), m_SlideCounter(0)
{

}

/****************************************************************************/
/*!
 *  \brief  Called when the internal thread of the device is started
 *
 *      This function adds the configuration states to the main state machine
 *      of the device. It also starts the state machine.
 */
/****************************************************************************/
void CDeviceSlideId::ThreadStarted()
{
    /////////////////////////////////////////////////////////////////
    // Configuring composite states
    /////////////////////////////////////////////////////////////////

    CState *ConfigStart = new CState("ConfigStart", mp_Configuring);
    QFinalState *ConfigEnd = new QFinalState(mp_Configuring);
    mp_Configuring->setInitialState(ConfigStart);

    ConfigStart->addTransition(new CSlideIdTransition(ConfigStart, SIGNAL(entered()), *this,
                                                      &CDeviceSlideId::Trans_Configure, ConfigEnd));

    m_machine.start();
}

/****************************************************************************/
/*!
 *  \brief  Performs the configuration of the device state machine
 *
 *      This function constructs the main state machine of the device.
 *
 *  \iparam p_Event = Signal parameters (unused)
 */
/****************************************************************************/
bool CDeviceSlideId::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    mp_BaseModule = m_DeviceProcessing.GetNodeFromID(GetModuleInstanceFromKey(CANObjectKeyLUT::m_BaseSlideIdKey));
    mp_PhotoDetector = static_cast<CAnalogInput *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_SlideIdPhotoDetectorKey)));
    mp_TransmitControl = static_cast<CAnalogOutput *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_SlideIdTransmitControlKey)));
    mp_TransmitCurrent = static_cast<CAnalogOutput *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_SlideIdTransmitCurrentKey)));
    mp_ReceiveCurrent = static_cast<CAnalogOutput *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_SlideIdReceiveCurrentKey)));

    if (mp_BaseModule == NULL || mp_PhotoDetector == NULL || mp_TransmitControl == NULL
            || mp_TransmitCurrent == NULL || mp_ReceiveCurrent == NULL)
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////
    // Initializing
    /////////////////////////////////////////////////////////////////
    CState *p_InitStart = new CState("InitStart", mp_Initializing);
    CState *p_WaitForAck1 = new CState("WaitForAck1", mp_Initializing);
    CState *p_WaitForAck2 = new CState("WaitForAck2", mp_Initializing);
    CState *p_WaitForAck3 = new CState("WaitForAck3", mp_Initializing);
    CState *p_WaitForValue = new CState("WaitForValue", mp_Initializing);
    CState *p_WaitForAckOff = new CState("WaitForAckOff", mp_Initializing);
    QFinalState *p_InitEnd = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(p_InitStart);

    // Enable all analog outputs needed fo the photoelectric detector
    p_InitStart->addTransition(new CSlideIdTransition(
            p_InitStart, SIGNAL(entered()),
            *this, &CDeviceSlideId::EnableSlideCounter, p_WaitForAck1));

    // Any of the analog outputs is able to activate the transition
    p_WaitForAck1->addTransition(new CSlideIdTransition(
            mp_TransmitControl, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::NormalOutputValueAckn, p_WaitForAck2));
    p_WaitForAck1->addTransition(new CSlideIdTransition(
            mp_TransmitCurrent, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::NormalOutputValueAckn, p_WaitForAck2));
    p_WaitForAck1->addTransition(new CSlideIdTransition(
            mp_ReceiveCurrent, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::NormalOutputValueAckn, p_WaitForAck2));

    // Any of the analog outputs is able to activate the transition
    p_WaitForAck2->addTransition(new CSlideIdTransition(
            mp_TransmitControl, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::NormalOutputValueAckn, p_WaitForAck3));
    p_WaitForAck2->addTransition(new CSlideIdTransition(
            mp_TransmitCurrent, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::NormalOutputValueAckn, p_WaitForAck3));
    p_WaitForAck2->addTransition(new CSlideIdTransition(
            mp_ReceiveCurrent, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::NormalOutputValueAckn, p_WaitForAck3));

    // Any of the analog outputs is able to activate the transition
    p_WaitForAck3->addTransition(new CSlideIdTransition(
            mp_TransmitControl, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::LastOutputValueAckn, p_WaitForValue));
    p_WaitForAck3->addTransition(new CSlideIdTransition(
            mp_TransmitCurrent, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::LastOutputValueAckn, p_WaitForValue));
    p_WaitForAck3->addTransition(new CSlideIdTransition(
            mp_ReceiveCurrent, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::LastOutputValueAckn, p_WaitForValue));

    // Read the value at the photoelectric detector
    p_WaitForValue->addTransition(new CSlideIdTransition(
            mp_PhotoDetector, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, qint16)),
            *this, &CDeviceSlideId::OnActInputValue, p_WaitForAckOff));

    // Disable the laser again
    p_WaitForAckOff->addTransition(new CSlideIdTransition(
            mp_TransmitControl, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::NormalOutputValueAckn, p_InitEnd));

    /////////////////////////////////////////////////////////////////
    // Working composite states
    /////////////////////////////////////////////////////////////////
    mp_Working->setChildMode(QState::ParallelStates);
    CState *p_SlideCounter = new CState("SlideCounter", mp_Working);
    CState *p_Stopped = new CState("Stopped", p_SlideCounter);
    CState *p_Started = new CState("Started", p_SlideCounter);
    CState *p_Stopping = new CState("Stopping", p_SlideCounter);
    CState *p_Starting = new CState("Starting", p_SlideCounter);
    p_SlideCounter->setInitialState(p_Stopped);

    // Default NACKs
    p_SlideCounter->addTransition(new CSlideIdTransition(
            this, SIGNAL(StartCounting()),
            *this, &CDeviceSlideId::NackStartCounting));
    p_SlideCounter->addTransition(new CSlideIdTransition(
            this, SIGNAL(StopCounting()),
            *this, &CDeviceSlideId::NackStopCounting));

    // Start switching the state
    p_Stopped->addTransition(new CSlideIdTransition(
            this, SIGNAL(StartCounting()),
            *this, &CDeviceSlideId::EnableLaser, p_Starting));
    p_Started->addTransition(new CSlideIdTransition(
            this, SIGNAL(StopCounting()),
            *this, &CDeviceSlideId::DisableLaser, p_Stopping));

    // Switch the state
    p_Stopping->addTransition(new CSlideIdTransition(
            mp_TransmitControl, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::OnDisableLaser, p_Stopped));
    p_Stopping->addTransition(new CSlideIdTransition(
            mp_TransmitControl, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::ErrorSwitchLaser, p_Started));
    p_Starting->addTransition(new CSlideIdTransition(
            mp_TransmitControl, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::OnEnableLaser, p_Started));
    p_Starting->addTransition(new CSlideIdTransition(
            mp_TransmitControl, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
            *this, &CDeviceSlideId::ErrorSwitchLaser, p_Stopped));

    // Increment the slide counter
    p_Started->addTransition(new CSlideIdTransition(
            mp_PhotoDetector, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, qint16)),
            *this, &CDeviceSlideId::CountSlide));

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Enables all outputs required to switch on the laser
 *
 *  \iparam p_Event = Signal parameters (unused)
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::EnableSlideCounter(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_TransmitControl->SetOutputValue(0x7FFF);
    if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    ReturnCode = mp_TransmitCurrent->SetOutputValue(0);
    if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    ReturnCode = mp_ReceiveCurrent->SetOutputValue(0xFFFF);
    if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Confirms that the output was enabled
 *
 *  \iparam p_Event = Parameters of the signal ReportOutputValueAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::NormalOutputValueAckn(QEvent *p_Event)
{
    return OutputValueAckn(p_Event, false);
}

/****************************************************************************/
/*!
 *  \brief  Confirms that the last output was enabled
 *
 *  \iparam p_Event = Parameters of the signal ReportOutputValueAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::LastOutputValueAckn(QEvent *p_Event)
{
    return OutputValueAckn(p_Event, true);
}

/****************************************************************************/
/*!
 *  \brief  Confirms that the output was enabled and requests the state
 *
 *  \iparam p_Event = Parameters of the signal ReportOutputValueAckn
 *  \iparam Last = Indicates if this is the last acknowledge signal
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::OutputValueAckn(QEvent *p_Event, bool Last)
{
    ReturnCode_t ReturnCode = CSlideIdTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    if (true == Last) {
        ReturnCode = mp_PhotoDetector->ReqActInputValue();
        if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
            emit NeedInitialize(ReturnCode);
            return false;
        }
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Called when an input value is received and checks it
 *
 *  \iparam p_Event = Parameters of the signal ReportActInputValue
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::OnActInputValue(QEvent *p_Event)
{
    qint16 Value;

    ReturnCode_t ReturnCode = CSlideIdTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    if (!CSlideIdTransition::GetEventValue(p_Event, 2, Value)) {
        emit NeedInitialize(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (100 > Value) {
        emit NeedInitialize(DCL_ERR_INTERNAL_ERR);
        return false;
    }
    ReturnCode = mp_TransmitControl->SetOutputValue(0xFFFF);
    if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Default transition for negative acks for start requests
 *
 *  \iparam p_Event = Signal parameters (unused)
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::NackStartCounting(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportStartCounting(DCL_ERR_INVALID_STATE);

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Default transition for negative acks for stop requests
 *
 *  \iparam p_Event = Signal parameters (unused)
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::NackStopCounting(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportStopCounting(DCL_ERR_INVALID_STATE, 0);

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Sends an enable request to the laser control output
 *
 *  \iparam p_Event = Signal parameters (unused)
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::EnableLaser(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_TransmitControl->SetOutputValue(0x7FFF);
    if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportStartCounting(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Sends a disable request to the laser control output
 *
 *  \iparam p_Event = Signal parameters (unused)
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::DisableLaser(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_TransmitControl->SetOutputValue(0xFFFF);
    if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportStopCounting(ReturnCode, 0);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Receives the result of the activation of the laser
 *
 *  \iparam p_Event = Parameters of the signal ReportOutputValueAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::OnEnableLaser(QEvent *p_Event)
{
    bool Success = true;

    ReturnCode_t ReturnCode = CSlideIdTransition::GetEventValue(p_Event, 1);
    if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        Success = false;
    }
    else {
        m_SlideCounter = 0;
    }
    emit ReportStartCounting(ReturnCode);

    return Success;
}

/****************************************************************************/
/*!
 *  \brief  Receives the result of the deactivation of the laser
 *
 *  \iparam p_Event = Parameters of the signal ReportOutputValueAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::OnDisableLaser(QEvent *p_Event)
{
    bool Success = true;

    ReturnCode_t ReturnCode = CSlideIdTransition::GetEventValue(p_Event, 1);
    if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        Success = false;
    }
    emit ReportStopCounting(ReturnCode, m_SlideCounter);

    return Success;
}

/****************************************************************************/
/*!
 *  \brief  Performed when a negative ack is received from the laser output
 *
 *  \iparam p_Event = Parameters of the signal ReportOutputValueAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::ErrorSwitchLaser(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CSlideIdTransition::GetEventValue(p_Event, 1);
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode) {
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Increases the slide counter, when something hits the laser
 *
 *  \iparam p_Event = Parameters of the signal ReportActInputValue
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceSlideId::CountSlide(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CSlideIdTransition::GetEventValue(p_Event, 1);
    if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        return false;
    }
    m_SlideCounter++;

    return true;
}

} //namespace
