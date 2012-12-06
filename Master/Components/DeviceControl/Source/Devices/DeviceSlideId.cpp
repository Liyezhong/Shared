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
    CDeviceBase(DeviceProc, ModuleList, InstanceID)
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
    //lint -esym(429, Init_Start)
    QFinalState *Init_Start = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(Init_Start);

    /////////////////////////////////////////////////////////////////
    // Working composite states
    /////////////////////////////////////////////////////////////////
    mp_Working->setChildMode(QState::ParallelStates);

    return true;
}

} //namespace
