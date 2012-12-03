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
typedef CSignalTransition<CDeviceXyz> CGrapplerTransition;

CDeviceXyz::CDeviceXyz(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
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
//bool CDeviceGrappler::Trans_Configure(QEvent *p_Event)
//{
//    Q_UNUSED(p_Event);

//    if (m_Thread.isRunning() == false)
//    {
//        return false;
//    }

//    mp_XAxisMotor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(
//                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorXAxisKey)));
//    mp_YAxisMotor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(
//                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorYAxisKey)));
//    mp_ZAxisMotor = static_cast<CStepperMotor *>(m_DeviceProcessing.GetFunctionModule(
//                                                GetModuleInstanceFromKey(CANObjectKeyLUT::m_MotorZAxisKey)));
//    mp_Rfid = static_cast<CRfid11785 *>(m_DeviceProcessing.GetFunctionModule(
//                                            GetModuleInstanceFromKey(CANObjectKeyLUT::m_GrapplerRFIDKey)));

//    if (mp_XAxisMotor == NULL || mp_YAxisMotor == NULL || mp_ZAxisMotor == NULL || mp_Rfid == NULL)
//    {
//        return false;
//    }

//    /////////////////////////////////////////////////////////////////
//    // Initializing
//    /////////////////////////////////////////////////////////////////
//    CFmStepperInit *p_InitStepperX =
//            new CFmStepperInit(mp_XAxisMotor, "XAxis", mp_Initializing);
//    CFmStepperInit *p_InitStepperY =
//            new CFmStepperInit(mp_YAxisMotor, "YAxis", mp_Initializing);
//    CFmStepperInit *p_InitStepperZ =
//            new CFmStepperInit(mp_ZAxisMotor, "ZAxis", mp_Initializing);
//    QFinalState *p_InitEnd = new QFinalState(mp_Initializing);
//    mp_Initializing->setInitialState(p_InitStepperX);

//    p_InitStepperX->addTransition(p_InitStepperX, SIGNAL(finished()), p_InitStepperY);
//    p_InitStepperY->addTransition(p_InitStepperY, SIGNAL(finished()), p_InitStepperZ);
//    p_InitStepperZ->addTransition(p_InitStepperZ, SIGNAL(finished()), p_InitEnd);

//    connect(p_InitStepperX, SIGNAL(NeedInitialize(ReturnCode_t)),
//            this,           SIGNAL(NeedInitialize(ReturnCode_t)));
//    connect(p_InitStepperY, SIGNAL(NeedInitialize(ReturnCode_t)),
//            this,           SIGNAL(NeedInitialize(ReturnCode_t)));
//    connect(p_InitStepperZ, SIGNAL(NeedInitialize(ReturnCode_t)),
//            this,           SIGNAL(NeedInitialize(ReturnCode_t)));

//    /////////////////////////////////////////////////////////////////
//    // Working composite states
//    /////////////////////////////////////////////////////////////////

//    mp_Working->setChildMode(QState::ParallelStates);

//    // XYZ Stepper Motor
//    CFmStepperMove *p_XStepper = new CFmStepperMove(mp_XAxisMotor, "XAxisStepper", mp_Working);
//    CFmStepperMove *p_YStepper = new CFmStepperMove(mp_YAxisMotor, "YAxisStepper", mp_Working);
//    CFmStepperMove *p_ZStepper = new CFmStepperMove(mp_ZAxisMotor, "ZAxisStepper", mp_Working);

//    // ReadRackRfid
//    CReadRackRfid *p_ReadRackRfid = new CReadRackRfid(mp_Rfid, "ReadRackRfid", mp_Working);

//    // Movement
//    CState *p_Navigator = new CState("Navigator", mp_Working);
//    CState *p_Idle = new CState("Idle", p_Navigator);
//    CState *p_MoveEmpty = new CState("MoveEmpty", p_Navigator);
//    CState *p_TransportRack = new CState("TransportRack", p_Navigator);

//    p_Navigator->setInitialState(p_Idle);

//    // Connect internal signals to FM States
//    connect(this, SIGNAL(MoveX(quint32,quint8)), p_XStepper, SIGNAL(Move(quint32,quint8)));
//    connect(this, SIGNAL(MoveY(quint32,quint8)), p_YStepper, SIGNAL(Move(quint32,quint8)));
//    connect(this, SIGNAL(MoveZ(quint32,quint8)), p_ZStepper, SIGNAL(Move(quint32,quint8)));

//    // Signal Transitions
//    p_Idle->addTransition(new CGrapplerTransition(this, SIGNAL(MoveEmptyTo(quint16,quint16)),
//                                                  *this, &CDeviceGrappler::Trans_Idle_MoveEmpty, p_MoveEmpty));

//    p_Idle->addTransition(new CGrapplerTransition(this, SIGNAL(MoveEmptyTo(quint16,quint16)),
//                                                  *this, &CDeviceGrappler::Trans_Idle_TransportRack, p_TransportRack));

//    p_MoveEmpty->addTransition(new CGrapplerTransition(p_XStepper, SIGNAL(MoveAck()),
//                                                       *this, &CDeviceGrappler::Trans_MoveEmpty_Idle, p_Idle));
//    p_MoveEmpty->addTransition(new CGrapplerTransition(p_YStepper, SIGNAL(MoveAck()),
//                                                       *this, &CDeviceGrappler::Trans_MoveEmpty_Idle, p_Idle));
//    p_MoveEmpty->addTransition(new CGrapplerTransition(p_ZStepper, SIGNAL(MoveAck()),
//                                                       *this, &CDeviceGrappler::Trans_MoveEmpty_Idle, p_Idle));

//    p_MoveEmpty->addTransition(new CGrapplerTransition(p_XStepper, SIGNAL(MoveAck()),
//                                                       *this, &CDeviceGrappler::Trans_TransportRack_Idle, p_Idle));
//    p_MoveEmpty->addTransition(new CGrapplerTransition(p_YStepper, SIGNAL(MoveAck()),
//                                                       *this, &CDeviceGrappler::Trans_TransportRack_Idle, p_Idle));
//    p_MoveEmpty->addTransition(new CGrapplerTransition(p_ZStepper, SIGNAL(MoveAck()),
//                                                       *this, &CDeviceGrappler::Trans_TransportRack_Idle, p_Idle));
//}

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
    CFmStepperInit *p_InitStepperX =
            new CFmStepperInit(mp_XAxisMotor, "XAxis", mp_Initializing);
    CFmStepperInit *p_InitStepperY =
            new CFmStepperInit(mp_YAxisMotor, "YAxis", mp_Initializing);
    CFmStepperInit *p_InitStepperZ =
            new CFmStepperInit(mp_ZAxisMotor, "ZAxis", mp_Initializing);
    QFinalState *p_InitEnd = new QFinalState(mp_Initializing);

    mp_Initializing->setInitialState(p_InitStepperZ);

    p_InitStepperZ->addTransition(p_InitStepperZ, SIGNAL(finished()), p_InitStepperY);
    p_InitStepperY->addTransition(p_InitStepperY, SIGNAL(finished()), p_InitStepperX);
    p_InitStepperX->addTransition(p_InitStepperX, SIGNAL(finished()), p_InitEnd);

    connect(p_InitStepperX, SIGNAL(NeedInitialize(ReturnCode_t)),
            this,           SIGNAL(NeedInitialize(ReturnCode_t)));
    connect(p_InitStepperY, SIGNAL(NeedInitialize(ReturnCode_t)),
            this,           SIGNAL(NeedInitialize(ReturnCode_t)));
    connect(p_InitStepperZ, SIGNAL(NeedInitialize(ReturnCode_t)),
            this,           SIGNAL(NeedInitialize(ReturnCode_t)));

    connect(mp_Initializing, SIGNAL(finished()), this, SLOT(ResetPositions()));

    /////////////////////////////////////////////////////////////////
    // Working composite states
    /////////////////////////////////////////////////////////////////

    mp_Working->setChildMode(QState::ParallelStates);

    // Xyz Movement
    CMoveXYZ *p_MoveXyz = new CMoveXYZ(*mp_XAxisMotor, *mp_YAxisMotor, *mp_ZAxisMotor, "MoveXyz", mp_Working);

    // ReadRackRfid
    CReadRackRfid *p_ReadRackRfid = new CReadRackRfid(mp_Rfid, "ReadRackRfid", mp_Working);

    // Navigation
    CState *p_Navigator = new CState("Navigator", mp_Working);
    CState *p_Idle = new CState("Idle", p_Navigator);
    CState *p_MoveEmpty = new CState("MoveEmpty", p_Navigator);
    CState *p_TransportRack = new CState("TransportRack", p_Navigator);

    p_Navigator->setInitialState(p_Idle);

    // Connect interface signals to composite states
    connect(this, SIGNAL(Move(quint32,quint8,quint32,quint8,quint32,quint8)),
            p_MoveXyz, SIGNAL(Move(quint32,quint8,quint32,quint8,quint32,quint8)));

    //
    connect(this, SIGNAL(Abort()), p_MoveXyz, SIGNAL(Abort()));
    connect(p_MoveXyz, SIGNAL(ReportAbort(ReturnCode_t)),
            this, SIGNAL(ReportAbort(ReturnCode_t)));

    // Signal Transitions
//    p_Navigator->addTransition(this, SIGNAL(Abort()), p_Idle);

    p_Idle->addTransition(new CGrapplerTransition(this, SIGNAL(MoveEmptyTo(quint16,quint16)),
                                                  *this, &CDeviceXyz::Trans_Idle_MoveEmpty, p_MoveEmpty));

    p_Idle->addTransition(new CGrapplerTransition(this, SIGNAL(TransportRackTo(quint16,quint16)),
                                                  *this, &CDeviceXyz::Trans_Idle_TransportRack, p_TransportRack));

    p_MoveEmpty->addTransition(new CGrapplerTransition(p_MoveXyz, SIGNAL(ReportMove(ReturnCode_t)),
                                                       *this, &CDeviceXyz::Trans_MoveEmpty_Idle, p_Idle));

    p_MoveEmpty->addTransition(this, SIGNAL(Abort()), p_Idle);

    p_TransportRack->addTransition(new CGrapplerTransition(p_MoveXyz, SIGNAL(ReportMove(ReturnCode_t)),
                                                           *this, &CDeviceXyz::Trans_TransportRack_Idle, p_Idle));

    p_TransportRack->addTransition(this, SIGNAL(Abort()), p_Idle);

    return true;
}

bool CDeviceXyz::Trans_Idle_MoveEmpty(QEvent *p_Event)
{
    quint16 StationColumn;
    quint16 StationRow;

    if (!CGrapplerTransition::GetEventValue(p_Event, 0, StationColumn)) {
        return false;
    }
    if (!CGrapplerTransition::GetEventValue(p_Event, 1, StationRow)) {
        return false;
    }

    m_CurrentPositionX = ColumnToPosition(StationColumn);
    m_CurrentPositionY = RowToPosition(StationRow);

    emit Move(m_CurrentPositionX, m_MoveEmptyProfile[X_AXIS],
              m_CurrentPositionY, m_MoveEmptyProfile[Y_AXIS],
              m_CurrentPositionZ, m_MoveEmptyProfile[Z_AXIS]);

    return true;
}

bool CDeviceXyz::Trans_Idle_TransportRack(QEvent *p_Event)
{
    quint16 StationColumn;
    quint16 StationRow;

    if (!CGrapplerTransition::GetEventValue(p_Event, 0, StationColumn)) {
        return false;
    }
    if (!CGrapplerTransition::GetEventValue(p_Event, 1, StationRow)) {
        return false;
    }

    m_CurrentPositionX = ColumnToPosition(StationColumn);
    m_CurrentPositionY = RowToPosition(StationRow);

    emit Move(m_CurrentPositionX, m_TransportRackProfile[X_AXIS],
              m_CurrentPositionY, m_TransportRackProfile[Y_AXIS],
              m_CurrentPositionZ, m_TransportRackProfile[Z_AXIS]);

    return true;
}

bool CDeviceXyz::Trans_MoveEmpty_Idle(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    ReportMoveEmptyTo(DCL_ERR_FCT_CALL_SUCCESS);

    return true;
}

bool CDeviceXyz::Trans_TransportRack_Idle(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    ReportTransportRackTo(DCL_ERR_FCT_CALL_SUCCESS);

    return true;
}

quint32 CDeviceXyz::ColumnToPosition(quint16 StationColumn)
{
    return StationColumn * 100;
}

quint32 CDeviceXyz::RowToPosition(quint16 StationRow)
{
    return StationRow * 100;
}

void CDeviceXyz::SetMoveEmptyProfile(quint8 ProfileX, quint8 ProfileY, quint8 ProfileZ)
{
    m_MoveEmptyProfile[X_AXIS] = ProfileX;
    m_MoveEmptyProfile[Y_AXIS] = ProfileY;
    m_MoveEmptyProfile[Z_AXIS] = ProfileZ;
}

void CDeviceXyz::SetTransposrtRackProfile(quint8 ProfileX, quint8 ProfileY, quint8 ProfileZ)
{
    m_TransportRackProfile[X_AXIS] = ProfileX;
    m_TransportRackProfile[Y_AXIS] = ProfileY;
    m_TransportRackProfile[Z_AXIS] = ProfileZ;
}

void CDeviceXyz::ResetPositions()
{
    m_CurrentPositionX = 0;
    m_CurrentPositionY = 0;
    m_CurrentPositionZ = 0;
}

}
