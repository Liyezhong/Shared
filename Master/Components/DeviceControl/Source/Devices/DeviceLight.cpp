/****************************************************************************/
/*! \file	DeviceLight.cpp
 *
 *  \brief	Implementation file for class CDeviceLight
 *
 *  \version	0.1
 *  \date		2012-10-25
 *  \author		Thiru
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

#include "DeviceControl/Include/Devices/DeviceLight.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/Devices/SetDrawerLed.h"
#include "DeviceControl/Include/Global/DeviceControl.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include <QDebug>
#include <QFinalState>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CDeviceLight
 *
 */
/****************************************************************************/
typedef CSignalTransition<CDeviceLight> CLightTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceLight
 *
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              request & response signals
 *
 *  \iparam     ModuleList to invoke function module interface functions.
 *
 *  \iparam     InstanceID Device Instance ID
 */
/****************************************************************************/
CDeviceLight::CDeviceLight(const DeviceProcessing &DeviceProc,const DeviceModuleList_t &ModuleList, DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID)
{

}

/****************************************************************************/
/*!
 *  \brief      Invoked when its thread is started. All child objects of this
 *              devices shall instantiated after this event.
 */
/****************************************************************************/
void CDeviceLight::ThreadStarted()
{
    // Configure step might be needed for ProtoTest in order to
    // allow reconfiguration (FM housekeeping and connects).
    // => automatically trigger configuration from constructor
    //    in order to keep interface lean at first.

    /////////////////////////////////////////////////////////////////
    // Configuring composite states
    /////////////////////////////////////////////////////////////////

    CState *ConfigStart = new CState("ConfigStart", mp_Configuring);
    QFinalState *ConfigEnd = new QFinalState(mp_Configuring);
    mp_Configuring->setInitialState(ConfigStart);
    // TODO: add configuration work
    ConfigStart->addTransition(new CLightTransition(ConfigStart, SIGNAL(entered()),
                                                     *this, &CDeviceLight::Trans_Configure, ConfigEnd));

    m_machine.start();
}
/****************************************************************************/
/*!
 *  \brief      To start
 *
 *  \iparam     p_Event     Not used
 *
 *  \return     true,
 */
/****************************************************************************/
bool CDeviceLight::Start(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    return true;
}
/****************************************************************************************/
/*!
 *  \brief      To Set the Led On when Light Device changes state from Off to SwitchingOn.
 *
 *  \iparam     p_Event     Not used.
 *
 *  \return     true, if Led setOuputvalue is sucess, else false.
 */
/*****************************************************************************************/
bool CDeviceLight::Trans_LightOff_SwitchingOn(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    ReturnCode_t ReturnCode = mp_FmDoLed->SetOutputValue(1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit LedError(ReturnCode);
        return false;
    }
    return true;
}

/******************************************************************************************/
/*!
 *  \brief      To emit LightOn when Light Device changes state from SwitchingOn to LightOn.
 *
 *  \iparam     p_Event     Refers to the arguements sent by signal.
 *
 *  \return     true, if Get Event value is sucess, else false.
 */
/******************************************************************************************/
bool CDeviceLight::Trans_SwitchingOn_LightOn(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CLightTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    emit LightOn(ReturnCode);
    return true;
}

/********************************************************************************************/
/*!
 *  \brief      To Set the Led Off when Light Device changes state from LightOn to SwitchingOn.
 *
 *  \iparam     p_Event     Refers to the arguements sent by signal.
 *
 *  \return     true, if Led setOuputvalue is sucess, else false.
 */
/********************************************************************************************/
bool CDeviceLight::Trans_LightOn_SwitchingOff(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    ReturnCode_t ReturnCode = mp_FmDoLed->SetOutputValue(0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit  LedError(ReturnCode);;
        return false;
    }
    return true;
}

/********************************************************************************************/
/*!
 *  \brief      To emit LightOff when Light Device changes state from SwitchingOff to LightOff.
 *
 *  \iparam     p_Event     Refers to the arguements sent by signal.
 *
 *  \return     true, if Get Event value is sucess, else false.
 */
/********************************************************************************************/
bool CDeviceLight::Trans_SwitchingOff_LightOff(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CLightTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    emit LightOff(ReturnCode);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To configure device with appropriate function modules &
 *              definition of working state flow for Light Device.
 *
 *  \iparam     p_Event     Not used
 *
 *  \return     true, if configuration is successful, else false
 */
/****************************************************************************/
bool CDeviceLight::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false) {
        return false;
    }

    // Get function module instances
    mp_BaseModule = m_DeviceProcessing.GetNodeFromID(GetModuleInstanceFromKey(CANObjectKeyLUT::m_BaseLightKey));
    mp_FmDoLed = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                   GetModuleInstanceFromKey(CANObjectKeyLUT::m_LightControlKey)));

    if (mp_FmDoLed == NULL) {
        return false;
    }

    // No Initialization needed.
    QFinalState *InitStart = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(InitStart);

    /////////////////////////////////////////////////////////////////
    // Working states
    /////////////////////////////////////////////////////////////////
    CState *p_Start = new CState("Start", mp_Working);
    CState *p_LightOff = new CState("LightOff", mp_Working);
    CState *p_SwitchingOn = new CState("SwitchingOn", mp_Working);
    CState *p_LightOn = new CState("LightOn", mp_Working);
    CState *p_SwitchingOff = new CState("SwitchingOff", mp_Working);

    mp_Working->setInitialState(p_Start);
    //p_Start->addTransition(p_LightOff);
    p_Start->addTransition( new CLightTransition (
           p_Start, SIGNAL(entered()),
           *this, &CDeviceLight::Start,
           p_LightOff ));
   p_LightOff->addTransition( new CLightTransition(
                                    this, SIGNAL(SetLightOn()),
                                    *this, &CDeviceLight::Trans_LightOff_SwitchingOn,
                                    p_SwitchingOn ));
    p_SwitchingOn->addTransition( new CLightTransition(
                                    mp_FmDoLed, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                                    *this, &CDeviceLight::Trans_SwitchingOn_LightOn,
                                    p_LightOn ));
    p_LightOn->addTransition( new CLightTransition(
                                    this, SIGNAL(SetLightOff()),
                                    *this, &CDeviceLight::Trans_LightOn_SwitchingOff,
                                    p_SwitchingOff ));
    p_SwitchingOff->addTransition(new CLightTransition(
                                     mp_FmDoLed, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                                     *this, &CDeviceLight::Trans_SwitchingOff_LightOff,
                                     p_LightOff ));
    return true;
}

} //namespace

