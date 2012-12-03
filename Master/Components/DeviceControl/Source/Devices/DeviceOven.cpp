/****************************************************************************/
/*! \file   DeviceOven.cpp
 *
 *  \brief  Implementation file for class CDeviceOven
 *
 *  \version  0.1
 *  \date     2012-10-26
 *  \author   Torsten Scheck, Stalin
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

#include "DeviceControl/Include/Devices/DeviceOven.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/Devices/MoveOpenClosed.h"
#include "DeviceControl/Include/Devices/FmStepperMove.h"
#include "DeviceControl/Include/Devices/FmStepperInit.h"
#include "DeviceControl/Include/Devices/TemperatureState.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"

#include <QFinalState>

namespace DeviceControl
{


/****************************************************************************/
/*!
 *  \brief  Signal transition for CDeviceHood
 */
/****************************************************************************/
typedef CSignalTransition<CDeviceOven> COvenTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceOven
 *
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              resuqest & response signals
 *
 *              FctModList Function module list of this device, to invoke
 *              function module interface functions.
 *
 *              InstanceID Device Instance ID
 */
/****************************************************************************/
CDeviceOven::CDeviceOven(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                         DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID)
{

}

/****************************************************************************/
/*!
 *  \brief      To configure device with appropriate function modules &
 *              definition of working state flow.
 *
 *  \iparam     p_Event unused
 *
 *  \return     true, if configuration is successful, else false
 */
/****************************************************************************/
bool CDeviceOven::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    mp_Motor = static_cast<CStepperMotor *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_OvenCoverMotorKey)));
    mp_TempControl = static_cast<CTemperatureControl *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_OvenTempCtrlKey)));

    if (mp_Motor == NULL || mp_TempControl == NULL)
    {
        return false;
    }

    connect(mp_TempControl, SIGNAL(ReportError(quint32, quint32, quint32, quint16, QDateTime)),
            this, SLOT(OnTempControlError(quint32, quint32, quint32, quint16, QDateTime)));

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

    // TemperatureState
    CTemperatureState *p_TemperatureState = new CTemperatureState(mp_TempControl, "TemperatureState", mp_Working);

    CMoveOpenClosed *p_OvenMotion = new CMoveOpenClosed(mp_Motor, "OvenMotion", mp_Working);

    CFmStepperMove *p_OvenStepper = new CFmStepperMove(mp_Motor, "OvenStepper", mp_Working);

    // command request interface to DCP
    connect(this,               SIGNAL(SetTemperature(bool, qreal)),
            p_TemperatureState, SIGNAL(SetState(bool, qreal)));
    connect(this,               SIGNAL(GetTemperature()),
            p_TemperatureState, SIGNAL(GetTemperature()));
    connect(this,           SIGNAL(CoverOpen()),
            p_OvenMotion,   SIGNAL(Open()));
    connect(this,           SIGNAL(CoverClose()),
            p_OvenMotion,   SIGNAL(Close()));

    connect(this,           SIGNAL(Stop()),
            p_OvenStepper,  SIGNAL(Stop()));
    connect(this,           SIGNAL(Resume()),
            p_OvenStepper,  SIGNAL(Resume()));
    connect(this,           SIGNAL(Abort()),
            p_OvenStepper,  SIGNAL(Abort()));
    connect(this,           SIGNAL(Disable()),
            p_OvenStepper,  SIGNAL(Disable()));

    // command response interface to DCP
    connect(p_TemperatureState, SIGNAL(ReportSetState(ReturnCode_t)),
            this,               SIGNAL(ReportSetTemperature(ReturnCode_t)));
    connect(p_TemperatureState, SIGNAL(ReportGetTemperature(ReturnCode_t,qreal)),
            this,               SIGNAL(ReportGetTemperature(ReturnCode_t,qreal)));
    connect(p_OvenMotion,   SIGNAL(ReportOpen(ReturnCode_t)),
            this,           SIGNAL(ReportCoverOpen(ReturnCode_t)));
    connect(p_OvenMotion,   SIGNAL(ReportClose(ReturnCode_t)),
            this,           SIGNAL(ReportCoverClose(ReturnCode_t)));

    connect(p_OvenStepper,  SIGNAL(ReportStop(ReturnCode_t)),
            this,           SIGNAL(ReportStop(ReturnCode_t)));
    connect(p_OvenStepper,  SIGNAL(ReportResume(ReturnCode_t)),
            this,           SIGNAL(ReportResume(ReturnCode_t)));
    connect(p_OvenStepper,  SIGNAL(ReportAbort(ReturnCode_t)),
            this,           SIGNAL(ReportAbort(ReturnCode_t)));
    connect(p_OvenStepper,  SIGNAL(ReportDisable(ReturnCode_t)),
            this,           SIGNAL(ReportDisable(ReturnCode_t)));

    // signals for DrawerStepper and DrawerMotion interaction
    connect(p_OvenMotion,   SIGNAL(Move(quint32, quint8)),
            p_OvenStepper,  SIGNAL(Move(quint32, quint8)));
    connect(p_OvenStepper,  SIGNAL(MoveAck()),
            p_OvenMotion,   SIGNAL(MoveAck()));
    connect(p_OvenStepper,  SIGNAL(ReportAbort(ReturnCode_t)),
            p_OvenMotion,   SIGNAL(ReportAbort(ReturnCode_t)));
    connect(p_OvenStepper,  SIGNAL(StepperError(ReturnCode_t)),
            p_OvenMotion,   SIGNAL(StepperError(ReturnCode_t)));

    // Send Nack for all requests by default:
    // This is done by adding targetless transitions to upper layer states,
    // which can be overridden by the child states.

    // Caution:
    // An event is distributed to all children of a parallel composite state.
    // If the event is not consumed by all children, the event is not overridden.
    // Workarounds
    // Exclude mp_Working state from source states for DefaultNack,
    // i.e. use mp_Init and only actual child state of mp_Working
    // Send Nacks from Device level for all commands.

    // Configure open close position & profile
    p_OvenMotion->SetOpenPosition(950);
    p_OvenMotion->SetClosePosition(20);
    p_OvenMotion->SetOpenProfile(1);
    p_OvenMotion->SetCloseProfile(1);

    // TODO Default Nacks for Stop Resume Abort in Base Device? plus helper method
    return true;
}

/****************************************************************************/
/*!
 *  \brief  This slot is called when an error was detected or received
 *
 *  \iparam InstanceID = Instance identifier of device
 *  \iparam ErrorGroup = Error group
 *  \iparam ErrorCode = Error code
 *  \iparam ErrorData = Additional error information
 *  \iparam ErrorTime = Error time
 */
/****************************************************************************/
void CDeviceOven::OnTempControlError(quint32 InstanceID, quint32 ErrorGroup, quint32 ErrorCode, quint16 ErrorData,
                                     QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID)
    emit ReportError(m_InstanceID, ErrorGroup, ErrorCode, ErrorData, ErrorTime);
}

} //namespace


// vi: set ts=4 sw=4 et:

