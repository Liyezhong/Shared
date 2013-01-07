/****************************************************************************/
/*! \file DeviceAgitation.cpp
 *
 *  \brief  Implementation file for class CDeviceAgitation
 *
 *  \version  0.1
 *  \date     2012-09-24
 *  \author   Thiru
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
#include "DeviceControl/Include/Devices/DeviceAgitation.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/Devices/MoveOpenClosed.h"
#include "DeviceControl/Include/Devices/FmStepperMove.h"
#include "DeviceControl/Include/Devices/FmStepperInit.h"
#include <QFinalState>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CDeviceAgitation
 *
 */
/****************************************************************************/
typedef CSignalTransition<CDeviceAgitation> CAgitationTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceAgitation
 *
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              request & response signals
 *
 *  \iparam     DeviceModuleList of this device, to invoke
 *              function module interface functions.
 *
 *  \iparam     InstanceID Device Instance ID.
 */
/****************************************************************************/
CDeviceAgitation::CDeviceAgitation(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                                   DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID)
{

}

/****************************************************************************/
/*!
 *  \brief      To configure device with appropriate function modules &
 *              definition of working state flow for Agitation Device.
 *
 *  \iparam     p_Event     Not used
 *
 *  \return     true, if configuration is successful, else false
 */
/****************************************************************************/
bool CDeviceAgitation::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    mp_Motor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(
                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorAgitationKey)));

    if (mp_Motor == NULL)
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////
    // Initializing
    /////////////////////////////////////////////////////////////////
    CFmStepperInit *p_InitStepper =
        new CFmStepperInit(mp_Motor, "InitStepper", mp_Initializing);
    QFinalState *p_InitEnd = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(p_InitStepper);

    p_InitStepper->addTransition( p_InitStepper, SIGNAL(finished()), p_InitEnd );

    connect(p_InitStepper, SIGNAL(NeedInitialize(ReturnCode_t)),
            this,          SIGNAL(NeedInitialize(ReturnCode_t)));

    // AgitationStepper
    CFmStepperMove *p_AgitationStepper = new CFmStepperMove(mp_Motor, "DrawerStepper", mp_Working);

    mp_Working->setInitialState(p_AgitationStepper);

    connect(this, SIGNAL(Rotate(quint32, quint8)),
            p_AgitationStepper,               SIGNAL(Rotate(quint32, quint8)));
    connect(this, SIGNAL(Move(quint32, quint8)),
            p_AgitationStepper,               SIGNAL(Move(quint32, quint8)));
    connect(this, SIGNAL(Stop()),
            p_AgitationStepper,               SIGNAL(Stop()));
    connect(this, SIGNAL(Resume()),
            p_AgitationStepper,               SIGNAL(Resume()));
    connect(this, SIGNAL(Abort()),
            p_AgitationStepper,               SIGNAL(Abort()));
    connect(p_AgitationStepper,               SIGNAL(RotateAck()),
            this,                             SIGNAL(RotateAck()));
    connect(p_AgitationStepper,               SIGNAL(MoveAck()),
            this,                             SIGNAL(RotateAck()));

	return true;
}


} //namespace


// vi: set ts=4 sw=4 et:

