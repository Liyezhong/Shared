/****************************************************************************/
/*! \file	DeviceRackTransfer.cpp
 *
 *  \brief	Implementation file for class CDeviceRackTransfer
 *
 *  \version	0.1
 *  \date		2012-10-21
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
#include "DeviceControl/Include/Devices/DeviceRackTransfer.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/Devices/MoveOpenClosed.h"
#include "DeviceControl/Include/Devices/FmStepperMove.h"
#include "DeviceControl/Include/Devices/FmStepperInit.h"

#include <QFinalState>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CDeviceRackTransfer
 *
 */
/****************************************************************************/
typedef CSignalTransition<CDeviceRackTransfer> CRackTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceRackTransfer
 *
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              resuqest & response signals
 *
 * \iparam      ModuleList to invoke function module interface functions.
 *
 * \iparam      InstanceID Device Instance ID
 */
/****************************************************************************/
CDeviceRackTransfer::CDeviceRackTransfer(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                                         DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID)
{

}

/****************************************************************************/
/*!
 *  \brief      To configure device with appropriate function modules &
 *              definition of working state flow for RackTransfer Device.
 *
 *  \iparam     p_Event     Not used
 *
 *  \return     true, if configuration is successful, else false
 */
/****************************************************************************/
bool CDeviceRackTransfer::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    // Get function module instances
    mp_Motor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(
                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_RackTransferMotorKey)));

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


    /////////////////////////////////////////////////////////////////
    // Working composite states
    /////////////////////////////////////////////////////////////////
    mp_Working->setChildMode(QState::ParallelStates);

    // Rack Motion
    CMoveOpenClosed *p_RackMotion = new CMoveOpenClosed(mp_Motor, "RackTransferMotion", mp_Working);

    // Rack Stepper
    CFmStepperMove *p_RackStepper = new CFmStepperMove(mp_Motor, "RackTransferStepper", mp_Working);

    // command request interface to DCP
    connect(this,           SIGNAL(Transfer()),
            p_RackMotion,   SIGNAL(Open()));
    connect(this,           SIGNAL(Load()),
            p_RackMotion,   SIGNAL(Close()));

    // TODO: Device method for StepperMotor default connects
    connect(this,           SIGNAL(Stop()),
            p_RackStepper,  SIGNAL(Stop()));
    connect(this,           SIGNAL(Resume()),
            p_RackStepper,  SIGNAL(Resume()));
    connect(this,           SIGNAL(Abort()),
            p_RackStepper,  SIGNAL(Abort()));
    connect(this,           SIGNAL(Disable()),
            p_RackStepper,  SIGNAL(Disable()));

    // command response interface to DCP
    connect(p_RackMotion,   SIGNAL(ReportOpen(ReturnCode_t)),
            this,           SIGNAL(ReportTransfer(ReturnCode_t)));
    connect(p_RackMotion,   SIGNAL(ReportClose(ReturnCode_t)),
            this,           SIGNAL(ReportLoad(ReturnCode_t)));

    connect(p_RackStepper,  SIGNAL(ReportStop(ReturnCode_t)),
            this,           SIGNAL(ReportStop(ReturnCode_t)));
    connect(p_RackStepper,  SIGNAL(ReportResume(ReturnCode_t)),
            this,           SIGNAL(ReportResume(ReturnCode_t)));
    connect(p_RackStepper,  SIGNAL(ReportAbort(ReturnCode_t)),
            this,           SIGNAL(ReportAbort(ReturnCode_t)));
    connect(p_RackStepper,  SIGNAL(ReportDisable(ReturnCode_t)),
            this,           SIGNAL(ReportDisable(ReturnCode_t)));

    // signals for DrawerStepper and DrawerMotion interaction
    connect(p_RackMotion,   SIGNAL(Move(quint32, quint8)),
            p_RackStepper,  SIGNAL(Move(quint32, quint8)));
    connect(p_RackStepper,  SIGNAL(MoveAck()),
            p_RackMotion,   SIGNAL(MoveAck()));
    connect(p_RackStepper,  SIGNAL(ReportAbort(ReturnCode_t)),
            p_RackMotion,   SIGNAL(ReportAbort(ReturnCode_t)));
    connect(p_RackStepper,  SIGNAL(StepperError(ReturnCode_t)),
            p_RackMotion,   SIGNAL(StepperError(ReturnCode_t)));

    // Configure open close position & profile
    p_RackMotion->SetOpenPosition(2000);
    p_RackMotion->SetClosePosition(20);
    p_RackMotion->SetOpenProfile(0);
    p_RackMotion->SetCloseProfile(0);

    return true;
}


} //namespace
