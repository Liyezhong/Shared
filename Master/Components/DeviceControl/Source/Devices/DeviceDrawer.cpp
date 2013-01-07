/****************************************************************************/
/*! \file   DeviceDrawer.cpp
 *
 *  \brief  Implementation file for class CDeviceDrawer
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

#include <QFinalState>
#include <QDebug>

#include "DeviceControl/Include/Devices/DeviceDrawer.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/Devices/SetDrawerLed.h"
#include "DeviceControl/Include/Devices/ReadRackRfid.h"
#include "DeviceControl/Include/Devices/MoveOpenClosed.h"
#include "DeviceControl/Include/Devices/FmStepperMove.h" //TODO
#include "DeviceControl/Include/Devices/FmStepperInit.h"

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CDeviceDrawer
 */
/****************************************************************************/
typedef CSignalTransition<CDeviceDrawer> CDrawerTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceDrawer
 *
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              request & response signals
 *
 *  \iparam     FctModList Function module list of this device, to invoke
 *              function module interface functions.
 *
 *  \iparam     InstanceID Device Instance ID
 */
/****************************************************************************/
CDeviceDrawer::CDeviceDrawer(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                             DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID)
{

}

/****************************************************************************/
/*!
 *  \brief      To send Nack if read Rfid is requested in wrong state
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CDeviceDrawer::DefaultNackReadRackRfid(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportReadRackRfid(DCL_ERR_INVALID_STATE, LOADER_ID_RFID_UNDEF, 0, 0);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To send NACK if Block is requested in wrong state
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CDeviceDrawer::DefaultNackBlock(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportBlock(DCL_ERR_INVALID_STATE);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To send NACK if Unblock is requested in wrong state
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CDeviceDrawer::DefaultNackUnblock(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportUnblock(DCL_ERR_INVALID_STATE);
    return true;
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
bool CDeviceDrawer::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    mp_Motor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(
                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorDrawerKey)));
    mp_Rfid = static_cast<CRfid11785 *>(m_DeviceProcessing.GetFunctionModule(
                                            GetModuleInstanceFromKey(CANObjectKeyLUT::m_DrawerRFIDKey)));
    mp_Button = static_cast<CDigitalInput *>(m_DeviceProcessing.GetFunctionModule(
                                                 GetModuleInstanceFromKey(CANObjectKeyLUT::m_DrawerButtonKey)));
    mp_LedFree = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                   GetModuleInstanceFromKey(CANObjectKeyLUT::m_DrawerLEDFreeKey)));
    mp_LedBlocked = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                      GetModuleInstanceFromKey(CANObjectKeyLUT::m_DrawerLEDBlockedKey)));

    if (mp_Motor == NULL || mp_Rfid == NULL || mp_Button == NULL || mp_LedFree == NULL || mp_LedBlocked == NULL)
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////
    // Initializing
    /////////////////////////////////////////////////////////////////

    CFmStepperInit *p_InitStepper =
            new CFmStepperInit(mp_Motor, "InitStepper", mp_Initializing);
    CSetDrawerLed *p_InitLed =
            new CSetDrawerLed(mp_LedFree, true, mp_LedBlocked, false, "InitLed", mp_Initializing);
    QFinalState *p_InitEnd = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(p_InitStepper);

    p_InitStepper->addTransition( p_InitStepper, SIGNAL(finished()), p_InitLed );
    p_InitLed->addTransition( p_InitLed, SIGNAL(finished()), p_InitEnd );

    connect(p_InitStepper, SIGNAL(NeedInitialize(ReturnCode_t)),
            this,          SIGNAL(NeedInitialize(ReturnCode_t)));

    connect(p_InitLed,     SIGNAL(LedError(ReturnCode_t)),
            this,          SIGNAL(NeedInitialize(ReturnCode_t)));

    /////////////////////////////////////////////////////////////////
    // Working composite states
    /////////////////////////////////////////////////////////////////

    mp_Working->setChildMode(QState::ParallelStates);

    // ReadRackRfid
    CReadRackRfid *p_ReadRackRfid = new CReadRackRfid(mp_Rfid, "ReadRackRfid", mp_Working);

    // DrawerMotion
    CMoveOpenClosed *p_DrawerMotion = new CMoveOpenClosed(mp_Motor, mp_Button, mp_LedFree,
                                                          mp_LedBlocked, "DrawerMotion", mp_Working);

    // DrawerStepper
    CFmStepperMove *p_DrawerStepper = new CFmStepperMove(mp_Motor, "DrawerStepper", mp_Working);

    // command request interface to DCP
    connect(this,            SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)),
            p_ReadRackRfid,  SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)));
    connect(this,            SIGNAL(Block()),
            p_DrawerMotion,  SIGNAL(Block()));
    connect(this,            SIGNAL(Unblock()),
            p_DrawerMotion,  SIGNAL(Unblock()));
    connect(this,            SIGNAL(Open()),
            p_DrawerMotion,  SIGNAL(Open()));
    connect(this,            SIGNAL(Close()),
            p_DrawerMotion,  SIGNAL(Close()));
    // TODO: Device method for StepperMotor default connects
    connect(this,            SIGNAL(Stop()),
            p_DrawerStepper, SIGNAL(Stop()));
    connect(this,            SIGNAL(Resume()),
            p_DrawerStepper, SIGNAL(Resume()));
    connect(this,            SIGNAL(Abort()),
            p_DrawerStepper, SIGNAL(Abort()));
    connect(this,            SIGNAL(Disable()),
            p_DrawerStepper, SIGNAL(Disable()));

    // command response interface to DCP
    connect(p_ReadRackRfid,  SIGNAL(ReportReadRackRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32)),
            this,            SIGNAL(ReportReadRackRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32)));
    connect(p_DrawerMotion,  SIGNAL(ReportBlock(ReturnCode_t)),
            this,            SIGNAL(ReportBlock(ReturnCode_t)));
    connect(p_DrawerMotion,  SIGNAL(ReportUnblock(ReturnCode_t)),
            this,            SIGNAL(ReportUnblock(ReturnCode_t)));
    connect(p_DrawerMotion,  SIGNAL(Opened()),
            this,            SIGNAL(Opened()));
    connect(p_DrawerMotion,  SIGNAL(Closed()),
            this,            SIGNAL(Closed()));
    connect(p_DrawerMotion,  SIGNAL(ReportOpen(ReturnCode_t)),
            this,            SIGNAL(ReportOpen(ReturnCode_t)));
    connect(p_DrawerMotion,  SIGNAL(ReportClose(ReturnCode_t)),
            this,            SIGNAL(ReportClose(ReturnCode_t)));

    connect(p_DrawerStepper, SIGNAL(ReportStop(ReturnCode_t)),
            this,            SIGNAL(ReportStop(ReturnCode_t)));
    connect(p_DrawerStepper, SIGNAL(ReportResume(ReturnCode_t)),
            this,            SIGNAL(ReportResume(ReturnCode_t)));
    connect(p_DrawerStepper, SIGNAL(ReportAbort(ReturnCode_t)),
            this,            SIGNAL(ReportAbort(ReturnCode_t)));
    connect(p_DrawerStepper, SIGNAL(ReportDisable(ReturnCode_t)),
            this,            SIGNAL(ReportDisable(ReturnCode_t)));

    // signals for DrawerStepper and DrawerMotion interaction
    connect(p_DrawerMotion,  SIGNAL(Move(quint32, quint8)),
            p_DrawerStepper, SIGNAL(Move(quint32, quint8)));
    connect(p_DrawerStepper, SIGNAL(MoveAck()),
            p_DrawerMotion,  SIGNAL(MoveAck()));
    connect(p_DrawerStepper, SIGNAL(ReportAbort(ReturnCode_t)),
            p_DrawerMotion,  SIGNAL(ReportAbort(ReturnCode_t)));
    connect(p_DrawerStepper, SIGNAL(StepperError(ReturnCode_t)),
            p_DrawerMotion,  SIGNAL(StepperError(ReturnCode_t)));

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

    mp_Init->addTransition( new CDrawerTransition(
                                p_ReadRackRfid, SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)),
                                *this, &CDeviceDrawer::DefaultNackReadRackRfid) );
    p_ReadRackRfid->addTransition( new CDrawerTransition(
                                       p_ReadRackRfid, SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)),
                                       *this, &CDeviceDrawer::DefaultNackReadRackRfid) );

    mp_Init->addTransition( new CDrawerTransition(
                                p_DrawerMotion, SIGNAL(Block()),
                                *this, &CDeviceDrawer::DefaultNackBlock) );
    p_DrawerMotion->addTransition( new CDrawerTransition(
                                       p_DrawerMotion, SIGNAL(Block()),
                                       *this, &CDeviceDrawer::DefaultNackBlock) );

    mp_Init->addTransition( new CDrawerTransition(
                                p_DrawerMotion, SIGNAL(Unblock()),
                                *this, &CDeviceDrawer::DefaultNackUnblock) );
    p_DrawerMotion->addTransition( new CDrawerTransition(
                                       p_DrawerMotion, SIGNAL(Unblock()),
                                       *this, &CDeviceDrawer::DefaultNackUnblock) );

    // TODO Default Nacks for Stop Resume Abort in Base Device? plus helper method

    // Configure open close position & profile
    p_DrawerMotion->SetOpenPosition(2000);
    p_DrawerMotion->SetClosePosition(0);
    p_DrawerMotion->SetOpenProfile(0);
    p_DrawerMotion->SetCloseProfile(0);

    return true;
}

} //namespace


// vi: set ts=4 sw=4 et:

