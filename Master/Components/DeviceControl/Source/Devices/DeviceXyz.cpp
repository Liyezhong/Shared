/****************************************************************************/
/*! \file   DeviceXyz.cpp
 *
 *  \brief  Implementation file for class CDeviceXyz
 *
 *  \version  0.1
 *  \date     2012-11-19
 *  \author   Stalin
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

#include "DeviceControl/Include/Devices/DeviceXyz.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/Devices/ReadRackRfid.h"
#include "DeviceControl/Include/Devices/MoveXYZ.h"
#include "DeviceControl/Include/Devices/FmStepperMove.h"
#include "DeviceControl/Include/Devices/FmStepperInit.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CDeviceGrappler
 */
/****************************************************************************/
typedef CSignalTransition<CDeviceXyz> CXyzTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceXyz
 *
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              request & response signals
 *
 *  \iparam     FctModList Function module list of this device, to invoke
 *              function module interface functions.
 *
 *  \iparam     InstanceID Device Instance ID.
 */
/****************************************************************************/
CDeviceXyz::CDeviceXyz(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                                 DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID)
{
    quint8 Index;

    for (Index = 0; Index < MAX_STEPPER_MOTORS; Index++)
    {
        m_MoveEmptyProfile[Index] = 1;
        m_TransportRackProfile[Index] = 2;
    }

    m_PrevState = m_CurrentState = XYZ_STATE_IDLE;

    m_RackAttached = false;

    FillColumnRowPosition();
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
bool CDeviceXyz::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    mp_XAxisMotor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(
                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorXAxisKey)));
    mp_YAxisMotor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(
                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorYAxisKey)));
    mp_ZAxisMotor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(
                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorZAxisKey)));
    mp_Rfid = static_cast<CRfid11785 *>(m_DeviceProcessing.GetFunctionModule(
                                            GetModuleInstanceFromKey(CANObjectKeyLUT::m_GrapplerRFIDKey)));

    if (mp_XAxisMotor == NULL || mp_YAxisMotor == NULL || mp_ZAxisMotor == NULL || mp_Rfid == NULL)
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////
    // Initializing
    /////////////////////////////////////////////////////////////////
    CFmStepperInit *p_InitStepperZ = new CFmStepperInit(mp_ZAxisMotor, "Z", mp_Initializing);

    CState *p_ParallelInit = new CState("XY", mp_Initializing);
    p_ParallelInit->setChildMode(QState::ParallelStates);

    CFmStepperInit *p_InitStepperX = new CFmStepperInit(mp_XAxisMotor, "X", p_ParallelInit);
    CFmStepperInit *p_InitStepperY = new CFmStepperInit(mp_YAxisMotor, "Y", p_ParallelInit);

    QFinalState *p_InitEnd = new QFinalState(mp_Initializing);

    mp_Initializing->setInitialState(p_InitStepperZ);

//    p_InitStepperZ->addTransition(p_InitStepperZ, SIGNAL(finished()), p_InitEnd);z
    p_InitStepperZ->addTransition(p_InitStepperZ, SIGNAL(finished()), p_ParallelInit);
    p_ParallelInit->addTransition(p_ParallelInit, SIGNAL(finished()), p_InitEnd);

    connect(p_InitStepperX, SIGNAL(NeedInitialize(ReturnCode_t)),
            this,           SIGNAL(NeedInitialize(ReturnCode_t)));
    connect(p_InitStepperY, SIGNAL(NeedInitialize(ReturnCode_t)),
            this,           SIGNAL(NeedInitialize(ReturnCode_t)));
    connect(p_InitStepperZ, SIGNAL(NeedInitialize(ReturnCode_t)),
            this,           SIGNAL(NeedInitialize(ReturnCode_t)));

    /////////////////////////////////////////////////////////////////
    // Working composite states
    /////////////////////////////////////////////////////////////////

    mp_Working->setChildMode(QState::ParallelStates);

    // Xyz Movement
    mp_MoveXyz = new CMoveXYZ(*mp_XAxisMotor, *mp_YAxisMotor, *mp_ZAxisMotor, "MoveXyz", mp_Working);

    // ReadRackRfid
    CReadRackRfid *p_ReadRackRfid = new CReadRackRfid(mp_Rfid, "ReadRackRfid", mp_Working);

    // Navigation
    CState *p_Navigator = new CState("Navigator", mp_Working);
    CState *p_Idle = new CState("Idle", p_Navigator);
    CState *p_Move = new CState("Move", p_Navigator);

    p_Navigator->setInitialState(p_Idle);

    // Connect interface signals to composite states
    connect(this, SIGNAL(Move(quint32,quint8,quint32,quint8,quint32,quint8)),
            mp_MoveXyz, SIGNAL(Move(quint32,quint8,quint32,quint8,quint32,quint8)));

    // Signal Transitions
//    p_Navigator->addTransition(this, SIGNAL(Abort()), p_Idle);

    // Idle -> Active
    p_Idle->addTransition(new CXyzTransition(this, SIGNAL(MoveEmptyTo(quint16,quint16)),
                                                  *this, &CDeviceXyz::Trans_Idle_MoveEmpty, p_Move));

    p_Idle->addTransition(new CXyzTransition(this, SIGNAL(MoveRackTo(quint16,quint16)),
                                                  *this, &CDeviceXyz::Trans_Idle_MoveRack, p_Move));

    p_Idle->addTransition(new CXyzTransition(this, SIGNAL(LetDownRack()),
                                                  *this, &CDeviceXyz::Trans_Idle_LetdownRack, p_Move));

    p_Idle->addTransition(new CXyzTransition(this, SIGNAL(PullUpRack()),
                                                  *this, &CDeviceXyz::Trans_Idle_PullupRack, p_Move));

    p_Idle->addTransition(new CXyzTransition(this, SIGNAL(AttachRack()),
                                                  *this, &CDeviceXyz::Trans_Idle_AttachRack, p_Move));

    p_Idle->addTransition(new CXyzTransition(this, SIGNAL(DetachRack()),
                                                  *this, &CDeviceXyz::Trans_Idle_DetachRack, p_Move));

    p_Idle->addTransition(new CXyzTransition(this, SIGNAL(CountSlides()),
                                                  *this, &CDeviceXyz::Trans_Idle_CountSlides, p_Move));

    // Active -> Idle
    p_Move->addTransition(new CXyzTransition(mp_MoveXyz, SIGNAL(ReportMove(ReturnCode_t)),
                                                       *this, &CDeviceXyz::Trans_Move_Idle, p_Idle));

//    p_Move->addTransition(new CGrapplerTransition(mp_MoveXyz, SIGNAL(ReportMove(ReturnCode_t)),
//                                                       *this, &CDeviceXyz::Trans_Move));

    // Abort Active states
    p_Move->addTransition(new CXyzTransition(mp_MoveXyz, SIGNAL(ReportAbort(ReturnCode_t)),
                                             *this, &CDeviceXyz::Trans_Abort_Idle, p_Idle));

    connect(this, SIGNAL(Abort()), mp_MoveXyz, SIGNAL(Abort()));

    return true;
}


/****************************************************************************/
/*!
 *  \brief      To initiate arm movement without rack to target station when
 *              MoveEmptyTo() is triggered
 *
 *  \iparam     p_Event Refers to the arguements list sent by signal.
 *              Arg 0 - Target Station Column
 *              Arg 1 - Target Station Row
 *
 *  \return     true, if sucessful, else false
 */
/****************************************************************************/
bool CDeviceXyz::Trans_Idle_MoveEmpty(QEvent *p_Event)
{
    quint16 StationColumn;
    quint16 StationRow;
    quint32 TargetPositionX;
    quint32 TargetPositionY;

    if (!CXyzTransition::GetEventValue(p_Event, 0, StationColumn)) {
        return false;
    }
    if (!CXyzTransition::GetEventValue(p_Event, 1, StationRow)) {
        return false;
    }

    if (IsNewState(XYZ_STATE_MOVE_EMPTY, StationColumn, StationRow))
    {
        TargetPositionX = m_StaionPos[m_StationColumn][m_StationRow].PositionX;
        TargetPositionY = m_StaionPos[m_StationColumn][m_StationRow].PositionY;

        m_WayPoint.append(new CPoint(TargetPositionX, m_MoveEmptyProfile[X_AXIS],
                                     TargetPositionY, m_MoveEmptyProfile[Y_AXIS]));
    }

    MoveNextStep();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To initiate arm movement with rack to target station when
 *              MoveRackTo() is triggered
 *
 *  \iparam     p_Event Refers to the arguements list sent by signal.
 *              Arg 0 - Target Station Column
 *              Arg 1 - Target Station Row
 *
 *  \return     true, if sucessful, else false
 */
/****************************************************************************/
bool CDeviceXyz::Trans_Idle_MoveRack(QEvent *p_Event)
{
    quint16 StationColumn;
    quint16 StationRow;
    quint32 TargetPositionX;
    quint32 TargetPositionY;

    if (!CXyzTransition::GetEventValue(p_Event, 0, StationColumn)) {
        return false;
    }
    if (!CXyzTransition::GetEventValue(p_Event, 1, StationRow)) {
        return false;
    }

    if (IsNewState(XYZ_STATE_MOVE_RACK, StationColumn, StationRow))
    {
        TargetPositionX = m_StaionPos[m_StationColumn][m_StationRow].PositionX;
        TargetPositionY = m_StaionPos[m_StationColumn][m_StationRow].PositionY - ATTACH_POSITION;

        m_WayPoint.append(new CPoint(TargetPositionX, m_TransportRackProfile[X_AXIS],
                                     TargetPositionY, m_TransportRackProfile[Y_AXIS]));
    }

    MoveNextStep();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To let rack down to preset depth of the station when LetDownRacK()
 *              is triggered
 *
 *  \iparam     p_Event unused
 *
 *  \return     true, if successful, else false
 */
/****************************************************************************/
bool CDeviceXyz::Trans_Idle_LetdownRack(QEvent *p_Event)
{
    Q_UNUSED(p_Event);
    quint32 TargetPositionZ;

    if (IsNewState(XYZ_STATE_LET_DOWN_RACK, m_StationColumn, m_StationRow))
    {
        TargetPositionZ = m_StaionPos[m_StationColumn][m_StationRow].PositionZ;

        m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                     NO_CHANGE, 0,
                                     TargetPositionZ, m_TransportRackProfile[Z_AXIS]));
    }

    MoveNextStep();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To pull rack up to preset height when LetDownRacK() is triggered
 *
 *  \iparam     p_Event unused
 *
 *  \return     true, if successful, else false
 */
/****************************************************************************/
bool CDeviceXyz::Trans_Idle_PullupRack(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (IsNewState(XYZ_STATE_PULL_UP_RACK, m_StationColumn, m_StationRow))
    {
        m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                     NO_CHANGE, 0,
                                     Z_UP_WITH_RACK, m_TransportRackProfile[Z_AXIS]));
    }

    MoveNextStep();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To attach rack
 *
 *  \iparam     p_Event unused
 *
 *  \return     true, if successful, else false
 */
/****************************************************************************/
bool CDeviceXyz::Trans_Idle_AttachRack(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (IsNewState(XYZ_STATE_ATTACH, m_StationColumn, m_StationRow))
    {
        quint32 AttachPositionY = m_StaionPos[m_StationColumn][m_StationRow].PositionY - ATTACH_POSITION;
        quint32 AttachPositionZ = m_StaionPos[m_StationColumn][m_StationRow].PositionZ;

//        m_WayPoint.append(new CPoint(NO_CHANGE, 0,
//                                     NO_CHANGE, 0,
//                                     AttachPositionZ, m_MoveEmptyProfile[Z_AXIS]));

        m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                     AttachPositionY, m_TransportRackProfile[Y_AXIS],
                                     NO_CHANGE, 0));

//        m_WayPoint.append(new CPoint(NO_CHANGE, 0,
//                                     NO_CHANGE, 0,
//                                     Z_UP_WITH_RACK, m_TransportRackProfile[Z_AXIS]));
    }

    MoveNextStep();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To Detach rack
 *
 *  \iparam     p_Event unused
 *
 *  \return     true, if successful, else false
 */
/****************************************************************************/
bool CDeviceXyz::Trans_Idle_DetachRack(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (IsNewState(XYZ_STATE_DETACH, m_StationColumn, m_StationRow))
    {
        quint32 DetachPositionY = m_StaionPos[m_StationColumn][m_StationRow].PositionY;
        quint32 DetachPositionZ = m_StaionPos[m_StationColumn][m_StationRow].PositionZ;

//        m_WayPoint.append(new CPoint(NO_CHANGE, 0,
//                                     NO_CHANGE, 0,
//                                     DetachPositionZ, m_MoveEmptyProfile[Z_AXIS]));

        m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                     DetachPositionY, m_TransportRackProfile[Y_AXIS],
                                     NO_CHANGE, 0));

//        m_WayPoint.append(new CPoint(NO_CHANGE, 0,
//                                     NO_CHANGE, 0,
//                                     Z_UP_WITHOUT_RACK, m_TransportRackProfile[Z_AXIS]));
    }

    MoveNextStep();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To move to slide counter position and perform movement
 *              for slide counting
 *
 *  \iparam     p_Event unused
 *
 *  \return     true, if successful, else false
 */
/****************************************************************************/
bool CDeviceXyz::Trans_Idle_CountSlides(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (IsNewState(XYZ_STATE_COUNT_SLIDES, m_StationColumn, m_StationRow))
    {
        quint32 Y = m_StaionPos[m_StationColumn][m_StationRow].PositionY + 500;

        m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                     Y, m_TransportRackProfile[Y_AXIS],
                                     NO_CHANGE, 0));
    }

    MoveNextStep();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To move from active state (i.e. any movement state) to idle
*               state if previous step was the last step of movement
 *
 *  \iparam     p_Event Refers to the arguements list sent by signal.
 *              Arg 0 - Return Code
 *
 *  \return     true, if successful, else false
 */
/****************************************************************************/
bool CDeviceXyz::Trans_Move_Idle(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CXyzTransition::GetEventValue(p_Event, 0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        //! \todo To be decided whether to emit NeedInitialize() if axis movement failed
//        emit NeedInitialize(ReturnCode);
    }

    // Delete previous step from way point list
    DeletePrevStep();

    // If list is empty, then last movement has been executed. Hence emit acknowledgement.
    // or emit with return code on any unsuccessful movement
    if(m_WayPoint.isEmpty() || (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode))
    {
        switch(m_CurrentState)
        {
        case XYZ_STATE_MOVE_EMPTY:
            ReportMoveEmptyTo(ReturnCode);
            break;

        case XYZ_STATE_MOVE_RACK:
            ReportMoveRackTo(ReturnCode);
            break;

        case XYZ_STATE_ATTACH:
            m_RackAttached = true;
            ReportAttachRack(ReturnCode);
            break;

        case XYZ_STATE_DETACH:
            m_RackAttached = false;
            ReportDetachRack(ReturnCode);
            break;

        case XYZ_STATE_LET_DOWN_RACK:
            ReportLetDownRack(ReturnCode);
            break;

        case XYZ_STATE_PULL_UP_RACK:
            ReportPullUpRack(ReturnCode);
            break;

        case XYZ_STATE_COUNT_SLIDES:
            ReportCountSlides(ReturnCode);
            break;
        }

        m_CurrentState = XYZ_STATE_IDLE;

        return true;
    }
    else
    {
        MoveNextStep();

        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge & report aborted when movement abort is triggered
 *
 *  \iparam     p_Event Refers to the arguements list sent by signal.
 *              Arg 0 - Return Code
 *
 *  \return     true, if successful, else false
 */
/****************************************************************************/
bool CDeviceXyz::Trans_Abort_Idle(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CXyzTransition::GetEventValue(p_Event, 0);

    m_CurrentState = XYZ_STATE_IDLE;

    emit ReportAbort(ReturnCode);

    return true;
}

/****************************************************************************/
/*!
 *  \brief      To fetch and trigger next movement if any
 */
/****************************************************************************/
void CDeviceXyz::MoveNextStep()
{
    CPoint *pTargetPoint;

    if(m_WayPoint.size())
    {
        pTargetPoint = m_WayPoint.first();

        emit Move(pTargetPoint->m_PositionX, pTargetPoint->m_ProfileX,
                  pTargetPoint->m_PositionY, pTargetPoint->m_ProfileY,
                  pTargetPoint->m_PositionZ, pTargetPoint->m_ProfileZ);
    }
}

/****************************************************************************/
/*!
 *  \brief      To delete previous step
 */
/****************************************************************************/
void CDeviceXyz::DeletePrevStep()
{
    if(m_WayPoint.size())
    {
        delete m_WayPoint.first();
        m_WayPoint.removeFirst();
    }
}

// Getters & Setters
/****************************************************************************/
/*!
 *  \brief      To set motion profile for movement without rack
 *
 *  \iparam     ProfileX X Axis motion profile
 *  \iparam     ProfileY Y Axis motion profile
 *  \iparam     ProfileZ Z Axis motion profile
 */
/****************************************************************************/
void CDeviceXyz::SetMoveEmptyProfile(quint8 ProfileX, quint8 ProfileY, quint8 ProfileZ)
{
    m_MoveEmptyProfile[X_AXIS] = ProfileX;
    m_MoveEmptyProfile[Y_AXIS] = ProfileY;
    m_MoveEmptyProfile[Z_AXIS] = ProfileZ;
}

/****************************************************************************/
/*!
 *  \brief      To set motion profile for movement with rack
 *
 *  \iparam     ProfileX X Axis motion profile
 *  \iparam     ProfileY Y Axis motion profile
 *  \iparam     ProfileZ Z Axis motion profile
 */
/****************************************************************************/
void CDeviceXyz::SetTransposrtRackProfile(quint8 ProfileX, quint8 ProfileY, quint8 ProfileZ)
{
    m_TransportRackProfile[X_AXIS] = ProfileX;
    m_TransportRackProfile[Y_AXIS] = ProfileY;
    m_TransportRackProfile[Z_AXIS] = ProfileZ;
}

/****************************************************************************/
/*!
 *  \brief      To get current state
 */
/****************************************************************************/
XyzState_t CDeviceXyz::GetCurrentstate()
{
    return m_CurrentState;
}

/****************************************************************************/
/*!
 *  \brief      To check if requested state (i.e. movement) is new or is is
 *              resumed after stop
 *
 *  \iparam     NewState        Requested new state (movement)
 *  \iparam     StationColumn   Target Station Column (Optional)
 *  \iparam     StationRow      Target Station Row (Optional)
 */
/****************************************************************************/
bool CDeviceXyz::IsNewState(XyzState_t NewState, quint16 StationColumn, quint16 StationRow)
{
    bool isNewState = true;

    // if previous movement was not completed and new command is same as previous command,
    // then continue executing from where it was aborted previously.
    // if not, clear way point list and treat as new command

    if (m_WayPoint.size())
    {
        // Check if previous state & parameters are same as new state
        if ((m_PrevState == NewState) &&
                (m_StationColumn == StationColumn) &&
                (m_StationRow == StationRow))
        {
            isNewState = false;
        }
        else
        {
            m_WayPoint.clear();
        }
    }

    m_PrevState = m_CurrentState = NewState;
    m_StationColumn = StationColumn;
    m_StationRow = StationRow;

    return isNewState;
}

/****************************************************************************/
/*!
 *  \brief      To statically fill position for each station. Temporary
 *              arrangment for testing purpose only. Will be replaced with
 *              adjusment data container
 */
/****************************************************************************/
void CDeviceXyz::FillColumnRowPosition()
{
    quint8 StationColumn;
    quint8 StationRow;

    quint32 PositionX;
    quint32 PositionY;

    for (StationRow = XYZ_ROW1; StationRow <= XYZ_ROW4; StationRow++)
    {
        for (StationColumn = XYZ_COL1; StationColumn <= XYZ_COL18; StationColumn++)
        {
            m_StaionPos[StationColumn][StationRow].PositionX = 0;
            m_StaionPos[StationColumn][StationRow].PositionY = 0;
            m_StaionPos[StationColumn][StationRow].PositionZ = 0;
        }
    }

    // Oven Layout
    //if (DEVICE_INSTANCE_ID_GRAPPLER_1 == m_InstanceID)
    if (DEVICE_INSTANCE_ID_XYZ == m_InstanceID)
    {
        PositionY = XYZ_OVEN_Y_START;
        for (StationRow = XYZ_OVEN_ROW_START; StationRow <= XYZ_OVEN_ROW_END; StationRow++)
        {
            PositionX = XYZ_OVEN_X_START;

            for (StationColumn = XYZ_OVEN_COL_START; StationColumn <= XYZ_OVEN_COL_END; StationColumn++)
            {

                m_StaionPos[StationColumn][StationRow].PositionX = PositionX;
                m_StaionPos[StationColumn][StationRow].PositionY = PositionY;
                m_StaionPos[StationColumn][StationRow].PositionZ = XYZ_OVEN_Z_DOWN_POS;

                PositionX += XYZ_OVEN_X_OFFSET;
            }

            PositionY += XYZ_OVEN_Y_OFFSET;
        }

        // Slide Counter
        m_StaionPos[XYZ_SLIDE_COUNTER_COL][XYZ_SLIDE_COUNTER_ROW].PositionX = XYZ_SLIDE_COUNTER_X_POS;
        m_StaionPos[XYZ_SLIDE_COUNTER_COL][XYZ_SLIDE_COUNTER_ROW].PositionY = XYZ_SLIDE_COUNTER_Y_START;
        m_StaionPos[XYZ_SLIDE_COUNTER_COL][XYZ_SLIDE_COUNTER_ROW].PositionZ = XYZ_SLIDE_COUNTER_Z_DOWN_POS;
    }

    // Bath Layout + Drawers
    PositionY = XYZ_BL_Y_START;
    for (StationRow = XYZ_BL_ROW_START; StationRow <= XYZ_BL_ROW_END; StationRow++)
    {
        PositionX = XYZ_BL_X_START_LEFT;

//        if (XYZ_ROW4 == StationRow)
//        {
//            PositionY += ROW4_Y_OFFSET;
//        }
        switch (StationRow)
        {
        case XYZ_ROW1:
            PositionX += ROW1_X_OFFSET;
            PositionY += ROW1_Y_OFFSET;
            break;
        case XYZ_ROW2:
            PositionX += ROW2_X_OFFSET;
            break;
        case XYZ_ROW3:
            PositionX += ROW3_X_OFFSET;
            break;
        case XYZ_ROW4:
            PositionX += ROW4_X_OFFSET;
            PositionY += ROW4_Y_OFFSET;
            break;
        default:
            break;
        }

        for (StationColumn = XYZ_BL_COL_START; StationColumn <= XYZ_BL_COL_END; StationColumn++)
        {
            m_StaionPos[StationColumn][StationRow].PositionX = PositionX;
            m_StaionPos[StationColumn][StationRow].PositionY = PositionY;

            // Check if Heated stations
            if (((XYZ_ROW1 == StationRow) || (XYZ_ROW2 == StationRow)) &&
                    ((XYZ_COL4 == StationColumn) || (XYZ_COL5 == StationColumn)))
            {
                m_StaionPos[StationColumn][StationRow].PositionZ = XYZ_HS_Z_DOWN_POS;
            }
            else
            {
                m_StaionPos[StationColumn][StationRow].PositionZ = XYZ_BL_Z_DOWN_POS;
            }

            PositionX += XYZ_BL_X_OFFSET;
        }

        PositionY += XYZ_BL_Y_OFFSET;
    }

    // Rack Transfer
    //if (DEVICE_INSTANCE_ID_GRAPPLER_2 == m_InstanceID)
    if (DEVICE_INSTANCE_ID_XYZ_2 == m_InstanceID)
    {
        m_StaionPos[XYZ_RTS_COL][XYZ_RTS_ROW].PositionX = XYZ_RTS_X_POS;
        m_StaionPos[XYZ_RTS_COL][XYZ_RTS_ROW].PositionY = XYZ_RTS_Y_POS;
        m_StaionPos[XYZ_RTS_COL][XYZ_RTS_ROW].PositionZ = XYZ_RTS_Z_DOWN_POS;
    }

    for (StationRow = XYZ_ROW1; StationRow <= XYZ_ROW4; StationRow++)
    {
        for (StationColumn = XYZ_COL1; StationColumn <= XYZ_COL18; StationColumn++)
        {
            // adjust bath layout coordinates for right grappler
            //if (DEVICE_INSTANCE_ID_GRAPPLER_2 == m_InstanceID)
            if (DEVICE_INSTANCE_ID_XYZ_2 == m_InstanceID)
            {
                if (m_StaionPos[StationColumn][StationRow].PositionX != 0)
                {
                    m_StaionPos[StationColumn][StationRow].PositionX = (4315 + XYZ_BL_X_START_RIGHT) -
                            m_StaionPos[StationColumn][StationRow].PositionX;
                }
            }

//            qDebug() << StationRow << StationColumn << ": "
//                     << m_StaionPos[StationColumn][StationRow].PositionX
//                     << m_StaionPos[StationColumn][StationRow].PositionY
//                     << m_StaionPos[StationColumn][StationRow].PositionZ;
        }
    }
}

}
