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

#include "DeviceControl/Include/Devices/DeviceXyz.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/Devices/ReadRackRfid.h"
#include "DeviceControl/Include/Devices/MoveXYZ.h"
#include "DeviceControl/Include/Devices/FmStepperMove.h"
#include "DeviceControl/Include/Devices/FmStepperInit.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include <QDebug>
#include <QFinalState>

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
    CDeviceBase(DeviceProc, ModuleList, InstanceID),
    mp_BaseModuleX(NULL),mp_BaseModuleY(NULL),mp_BaseModuleZ(NULL),
    mp_XAxisMotor(NULL), mp_YAxisMotor(NULL), mp_ZAxisMotor(NULL),
    mp_Rfid(NULL),
    mp_Adjustment(NULL),
    m_RackAttached(false)
{
    m_PrevState = m_CurrentState = XYZ_STATE_IDLE;

    if (DEVICE_INSTANCE_ID_XYZ_1 == InstanceID)
    {
        m_XyzType = DataManager::LEFT_XYZ;
    }
    else
    {
        m_XyzType = DataManager::RIGHT_XYZ;
    }

    // Initialize profiles
    m_Prof_X_Move_Empty = 1;
    m_Prof_X_Move_With_Rack = 2;

    m_Prof_Y_Move_Empty = 1;
    m_Prof_Y_Move_With_Rack = 2;
    m_Prof_Y_Attach_Detach_Rack = 2;

    m_Prof_Z_Move_Empty = 1;
    m_Prof_Z_Move_Down_With_Rack = 2;
    m_Prof_Z_Move_Up_Slow = 4;
    m_Prof_Z_Move_Up_Fast = 3;

    // Timer settings
    m_Timer.setSingleShot(true);
}

/****************************************************************************/
/*!
 *  \brief      Destructor of class CDeviceXyz
 */
/****************************************************************************/
CDeviceXyz::~CDeviceXyz()
{
    while (m_WayPoint.size())
    {
        delete m_WayPoint.first();

        m_WayPoint.removeFirst();
    }
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
        //! \todo Log Thread start error
        qDebug() << "Thread Start error";
        return false;
    }

    mp_BaseModuleX = m_DeviceProcessing.GetNodeFromID(GetModuleInstanceFromKey(CANObjectKeyLUT::m_BaseXAxisKey));
    mp_BaseModuleY = m_DeviceProcessing.GetNodeFromID(GetModuleInstanceFromKey(CANObjectKeyLUT::m_BaseYAxisKey));
    mp_BaseModuleZ = m_DeviceProcessing.GetNodeFromID(GetModuleInstanceFromKey(CANObjectKeyLUT::m_BaseZAxisKey));

    mp_XAxisMotor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorXAxisKey)));
    mp_YAxisMotor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorYAxisKey)));
    mp_ZAxisMotor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorZAxisKey)));

    mp_Rfid = static_cast<CRfid11785 *>(m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_XyzSystemRFIDKey)));
    mp_LevelSensor = static_cast<CAnalogInput *>(m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_XyzSystemLiquidLevelKey)));
    mp_LevelSensorTeach = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_XyzSystemSensorTeachKey)));

    // Get adjustment data container
    mp_Adjustment = m_DeviceProcessing.GetAdjustmentList();

    if (mp_XAxisMotor == NULL || mp_YAxisMotor == NULL || mp_ZAxisMotor == NULL || mp_Rfid == NULL ||
            mp_LevelSensor == NULL ||
            mp_Adjustment == NULL)
    {
        //! \todo Log Device configuration failed
        qDebug() << "Device configuration failed";
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

    // Read liquid level
    CState *p_LiquidLevel = new CState("LiquidLevel", p_Navigator);
    CState *p_LiquidLevelMove = new CState("Move", p_LiquidLevel);
    CState *p_LiquidLevelRead = new CState("Read", p_LiquidLevel);
    p_LiquidLevel->setInitialState(p_LiquidLevelMove);

    // Level sensor teach
    CState *p_Sensor = new CState("SensorTeach", p_Navigator);
    CState *p_SensorMove = new CState("Min", p_Sensor);
    CState *p_SensorTeach = new CState("Max", p_Sensor);
    p_Sensor->setInitialState(p_SensorMove);

    p_Navigator->setInitialState(p_Idle);

    // Connect interface signals to composite states
    connect(this, SIGNAL(Move(quint32,quint8,quint32,quint8,quint32,quint8)),
            mp_MoveXyz, SIGNAL(Move(quint32,quint8,quint32,quint8,quint32,quint8)));

    // Signal Transitions

    // Idle -> Move
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

    // Move -> Idle
    p_Move->addTransition(new CXyzTransition(mp_MoveXyz, SIGNAL(ReportMove(ReturnCode_t)),
                                                       *this, &CDeviceXyz::Trans_Active_Idle, p_Idle));

    // Read Liquid Level
    {
        p_Idle->addTransition(new CXyzTransition(this, SIGNAL(ReadLiquidLevel(quint16,quint16)),
                                                 *this, &CDeviceXyz::Trans_Idle_MoveLiquidLevelPos, p_LiquidLevel));

        p_LiquidLevelMove->addTransition(new CXyzTransition(mp_MoveXyz, SIGNAL(ReportMove(ReturnCode_t)),
                                                            *this, &CDeviceXyz::Trans_MoveLiquidLevel_Read, p_LiquidLevelRead));

        p_LiquidLevelRead->addTransition(new CXyzTransition(mp_LevelSensor, SIGNAL(ReportActInputValue(quint32,ReturnCode_t,qint16)),
                                                            *this, &CDeviceXyz::Trans_ReadLiquidLevel_Idle, p_Idle));

        // On any failure report and directly move to idle state
        p_LiquidLevel->addTransition(this, SIGNAL(ReportReadLiquidLevel(ReturnCode_t,qint16)), p_Idle);
    }

    // Teach Level sensor
    {
        p_Idle->addTransition(new CXyzTransition(this, SIGNAL(TeachLevelSensorMin(quint16,quint16)),
                                                 *this, &CDeviceXyz::Trans_Idle_MoveTeachSensorPos, p_Sensor));
        p_SensorMove->addTransition(new CXyzTransition(mp_MoveXyz, SIGNAL(ReportMove(ReturnCode_t)),
                                                       *this, &CDeviceXyz::Trans_Move_TeachSensor, p_SensorTeach));
        p_SensorTeach->addTransition(new CXyzTransition(mp_LevelSensorTeach, SIGNAL(ReportOutputValueAckn(quint32,ReturnCode_t,quint16)),
                                                        *this, &CDeviceXyz::Trans_TeachSensor_Idle, p_Idle));

        // On any failure report and directly move to idle state
        p_Sensor->addTransition(this, SIGNAL(ReportTeachSensor(ReturnCode_t)), p_Idle);
    }

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
    DataManager::CPositionXyz Position;

    if (!CXyzTransition::GetEventValue(p_Event, 0, StationColumn)) {
        return false;
    }
    if (!CXyzTransition::GetEventValue(p_Event, 1, StationRow)) {
        return false;
    }

    if (IsNewState(XYZ_STATE_MOVE_EMPTY, StationColumn, StationRow))
    {
        if (mp_Adjustment->GetXyzPosition(m_XyzType, m_StationRow, m_StationColumn, Position))
        {
            m_WayPoint.append(new CPoint(Position.m_PositionX, m_Prof_X_Move_Empty,
                                         Position.m_PositionY, m_Prof_Y_Move_Empty));
        }
        else
        {
//            emit ReportEvent(DCL_ERR_INVALID_PARAM);
            ReportStatus(DCL_ERR_INVALID_PARAM);
            return false;
        }
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
    DataManager::CPositionXyz Position;

    if (!CXyzTransition::GetEventValue(p_Event, 0, StationColumn)) {
        return false;
    }
    if (!CXyzTransition::GetEventValue(p_Event, 1, StationRow)) {
        return false;
    }

    if (IsNewState(XYZ_STATE_MOVE_RACK, StationColumn, StationRow))
    {
        if (mp_Adjustment->GetXyzPosition(m_XyzType, m_StationRow, m_StationColumn, Position))
        {
            Position.m_PositionY -= ATTACH_POSITION;

            m_WayPoint.append(new CPoint(Position.m_PositionX, m_Prof_X_Move_With_Rack,
                                         Position.m_PositionY, m_Prof_Y_Move_With_Rack));
        }
        else
        {
//            emit ReportEvent(DCL_ERR_INVALID_PARAM);
            ReportStatus(DCL_ERR_INVALID_PARAM);
            return false;
        }
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
    DataManager::CPositionXyz Position;

    if (IsNewState(XYZ_STATE_LET_DOWN_RACK, m_StationColumn, m_StationRow))
    {
        if (mp_Adjustment->GetXyzPosition(m_XyzType, m_StationRow, m_StationColumn, Position))
        {
            if (m_RackAttached)
            {
                m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                             NO_CHANGE, 0,
                                             Position.m_PositionZ, m_Prof_Z_Move_Down_With_Rack));
            }
            else
            {
                m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                             NO_CHANGE, 0,
                                             Position.m_PositionZ, m_Prof_Z_Move_Empty));
            }
        }
        else
        {
//            emit ReportEvent(DCL_ERR_INVALID_PARAM);
            ReportStatus(DCL_ERR_INVALID_PARAM);
            return false;
        }
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
        if (m_RackAttached)
        {
            // Move up slow
            m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                         NO_CHANGE, 0,
                                         Z_MOVE_UP_SLOW_WITH_RACK, m_Prof_Z_Move_Up_Slow));

            // Move up fast
            m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                         NO_CHANGE, 0,
                                         Z_MOVE_UP_FAST_WITH_RACK, m_Prof_Z_Move_Up_Fast));
        }
        else
        {
            m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                         NO_CHANGE, 0,
                                         Z_MOVE_UP_WITHOUT_RACK, m_Prof_Z_Move_Empty));
        }
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
    DataManager::CPositionXyz Position;

    if (IsNewState(XYZ_STATE_ATTACH, m_StationColumn, m_StationRow))
    {
        if (mp_Adjustment->GetXyzPosition(m_XyzType, m_StationRow, m_StationColumn, Position))
        {
            Position.m_PositionY -= ATTACH_POSITION;

            m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                         Position.m_PositionY, m_Prof_Y_Attach_Detach_Rack,
                                         NO_CHANGE, 0));
        }
        else
        {
//            emit ReportEvent(DCL_ERR_INVALID_PARAM);
            ReportStatus(DCL_ERR_INVALID_PARAM);
            return false;
        }
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
    DataManager::CPositionXyz Position;

    if (IsNewState(XYZ_STATE_DETACH, m_StationColumn, m_StationRow))
    {
        if (mp_Adjustment->GetXyzPosition(m_XyzType, m_StationRow, m_StationColumn, Position))
        {
            m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                         Position.m_PositionY, m_Prof_Y_Attach_Detach_Rack,
                                         NO_CHANGE, 0));
        }
        else
        {
//            emit ReportEvent(DCL_ERR_INVALID_PARAM);
            ReportStatus(DCL_ERR_INVALID_PARAM);
            return false;
        }
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
    DataManager::CPositionXyz Position;

    if (IsNewState(XYZ_STATE_COUNT_SLIDES, m_StationColumn, m_StationRow))
    {
        if (mp_Adjustment->GetXyzPosition(m_XyzType, m_StationRow, m_StationColumn, Position))
        {
            Position.m_PositionY += SLIDE_COUNT_MOVE_POS;

            m_WayPoint.append(new CPoint(NO_CHANGE, 0,
                                         Position.m_PositionY, m_Prof_Y_Move_With_Rack,
                                         NO_CHANGE, 0));
        }
        else
        {
//            emit ReportEvent(DCL_ERR_INVALID_PARAM);
            ReportStatus(DCL_ERR_INVALID_PARAM);
            return false;
        }
    }

    MoveNextStep();
    return true;
}

bool CDeviceXyz::Trans_Idle_MoveLiquidLevelPos(QEvent *p_Event)
{
    quint16 StationColumn;
    quint16 StationRow;
    DataManager::CPositionXyz Position;

    if (!CXyzTransition::GetEventValue(p_Event, 0, StationColumn)) {
        return false;
    }
    if (!CXyzTransition::GetEventValue(p_Event, 1, StationRow)) {
        return false;
    }

    if (IsNewState(XYZ_STATE_READ_LIQUID_LEVEL, StationColumn, StationRow))
    {
        if (mp_Adjustment->GetXyzPosition(m_XyzType, m_StationRow, m_StationColumn, Position))
        {
            m_LiquidLevel = 0;

            Position.m_PositionX += LIQUID_LEVEL_X_OFFSET;

            m_WayPoint.append(new CPoint(Position.m_PositionX, m_Prof_X_Move_Empty,
                                         Position.m_PositionY, m_Prof_Y_Move_Empty,
                                         0, m_Prof_Z_Move_Empty));
        }
        else
        {
//            emit ReportEvent(DCL_ERR_INVALID_PARAM);
            ReportStatus(DCL_ERR_INVALID_PARAM);
            return false;
        }
    }

    MoveNextStep();
    return true;
}

bool CDeviceXyz::Trans_MoveLiquidLevel_Read(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CXyzTransition::GetEventValue(p_Event, 0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        //! \todo To be decided whether to emit NeedInitialize() if axis movement failed
//        emit NeedInitialize(ReturnCode);
        // Report failure to DCP and transit to idle state
        ReportStatus(ReturnCode);
        return false;
    }

    // Delete previous step from way point list
    DeletePrevStep();

    // If list is empty, then last movement has been executed. Hence emit acknowledgement.
    // or emit with return code on any unsuccessful movement
    if(m_WayPoint.isEmpty() || (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode))
    {
        // Read level sensor
        ReturnCode = mp_LevelSensor->ReqActInputValue();
        if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
            emit NeedInitialize(ReturnCode);

            // Report failure to DCP and transit to idle state
            ReportStatus(ReturnCode);
            return false;
        }
    }
    else
    {
        MoveNextStep();
    }

    return true;
}

bool CDeviceXyz::Trans_ReadLiquidLevel_Idle(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CXyzTransition::GetEventValue(p_Event, 1);

    if (!CXyzTransition::GetEventValue(p_Event, 2, m_LiquidLevel)) {
        //! \todo log error
        //! \todo To be decided whether to emit NeedInitialize() if read failed
//        emit NeedInitialize(ReturnCode);
        return false;
    }

    ReportStatus(ReturnCode);

    return true;
}

bool CDeviceXyz::Trans_Idle_MoveTeachSensorPos(QEvent *p_Event)
{
    quint16 StationColumn;
    quint16 StationRow;
    DataManager::CPositionXyz Position;

    if (!CXyzTransition::GetEventValue(p_Event, 0, StationColumn)) {
        return false;
    }
    if (!CXyzTransition::GetEventValue(p_Event, 1, StationRow)) {
        return false;
    }

    if (IsNewState(XYZ_STATE_TEACH_SENSOR, StationColumn, StationRow))
    {
        if (mp_Adjustment->GetXyzPosition(m_XyzType, m_StationRow, m_StationColumn, Position))
        {
            m_LiquidLevel = 0;

            Position.m_PositionX += LIQUID_LEVEL_X_OFFSET;

            m_WayPoint.append(new CPoint(Position.m_PositionX, m_Prof_X_Move_Empty,
                                         Position.m_PositionY, m_Prof_Y_Move_Empty,
                                         0, m_Prof_Z_Move_Empty));
        }
        else
        {
//            emit ReportEvent(DCL_ERR_INVALID_PARAM);
            ReportStatus(DCL_ERR_INVALID_PARAM);
            return false;
        }
    }

    MoveNextStep();
    return true;
}

bool CDeviceXyz::Trans_Move_TeachSensor(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CXyzTransition::GetEventValue(p_Event, 0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        //! \todo To be decided whether to emit NeedInitialize() if axis movement failed
//        emit NeedInitialize(ReturnCode);
        // Report failure to DCP and transit to idle state
        ReportStatus(ReturnCode);
        return false;
    }

    // Delete previous step from way point list
    DeletePrevStep();

    // If list is empty, then last movement has been executed. Hence emit acknowledgement.
    // or emit with return code on any unsuccessful movement
    if(m_WayPoint.isEmpty() || (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode))
    {
        // Set level sensor teach
        ReturnCode = mp_LevelSensorTeach->SetOutputValue(1);
        if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
            emit NeedInitialize(ReturnCode);

            // Report failure to DCP and transit to idle state
            ReportStatus(ReturnCode);
            return false;
        }

        m_Timer.setInterval(SENSOR_TEACH_DURATION);
    }
    else
    {
        MoveNextStep();
    }

    return true;
}

bool CDeviceXyz::Trans_TeachSensor_Idle(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CXyzTransition::GetEventValue(p_Event, 1);

    ReportStatus(ReturnCode);

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
bool CDeviceXyz::Trans_Active_Idle(QEvent *p_Event)
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
        ReportStatus(ReturnCode);
        return true;
    }
    else
    {
        MoveNextStep();
        return false;
    }
}

void CDeviceXyz::ReportStatus(ReturnCode_t ReturnCode)
{
    switch(m_CurrentState)
    {
    case XYZ_STATE_MOVE_EMPTY:
        emit ReportMoveEmptyTo(ReturnCode);
        break;

    case XYZ_STATE_MOVE_RACK:
        emit ReportMoveRackTo(ReturnCode);
        break;

    case XYZ_STATE_ATTACH:
        if (DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
        {
            m_RackAttached = true;
        }
        emit ReportAttachRack(ReturnCode);
        break;

    case XYZ_STATE_DETACH:
        if (DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
        {
            m_RackAttached = false;
        }
        emit ReportDetachRack(ReturnCode);
        break;

    case XYZ_STATE_LET_DOWN_RACK:
        emit ReportLetDownRack(ReturnCode);
        break;

    case XYZ_STATE_PULL_UP_RACK:
        emit ReportPullUpRack(ReturnCode);
        break;

    case XYZ_STATE_COUNT_SLIDES:
        emit ReportCountSlides(ReturnCode);
        break;

    case XYZ_STATE_READ_LIQUID_LEVEL:
        emit ReportReadLiquidLevel(ReturnCode, m_LiquidLevel);
        break;

        // To suppress compiler warning
    case XYZ_STATE_IDLE:
        break;
    }

    m_CurrentState = XYZ_STATE_IDLE;
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

// Getters & Setters
/****************************************************************************/
/*!
 *  \brief      To get current state
 */
/****************************************************************************/
XyzState_t CDeviceXyz::GetCurrentstate()
{
    return m_CurrentState;
}

}
