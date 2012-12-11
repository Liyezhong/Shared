#ifndef DEVICEXYZ_H
#define DEVICEXYZ_H

#include "DeviceBase.h"
#include "MoveXYZ.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"

namespace DeviceControl
{

typedef enum {
    XYZ_ROW1 = 0, XYZ_ROW2, XYZ_ROW3, XYZ_ROW4, XYZ_MAX_ROWS
} XyzRow_t;

typedef enum {
    XYZ_COL1 = 0, XYZ_COL2, XYZ_COL3, XYZ_COL4, XYZ_COL5, XYZ_COL6, XYZ_COL7, XYZ_COL8, XYZ_COL9,
    XYZ_COL10, XYZ_COL11, XYZ_COL12, XYZ_COL13, XYZ_COL14, XYZ_COL15, XYZ_COL16, XYZ_COL17, XYZ_COL18,
    XYZ_MAX_COLS
} XyzCol_t;

// General

#define X_MOTOR_ROLLING_CIRCUMFERENCE   90  // in mm
#define X_MOTOR_RESOLUTION              400 // in half-steps
#define Y_MOTOR_ROLLING_CIRCUMFERENCE   60  // in mm
#define Y_MOTOR_RESOLUTION              400 // in half-steps
#define Z_MOTOR_ROLLING_CIRCUMFERENCE   60  // in mm
#define Z_MOTOR_RESOLUTION              800 // in half-steps

#define ATTACH_POSITION         70
//#define ATTACH_POSITION         65

#define Z_UP_WITH_RACK          0
#define Z_UP_WITHOUT_RACK       1450

// Oven
#define XYZ_OVEN_ROW_START      XYZ_ROW1
#define XYZ_OVEN_ROW_END        XYZ_ROW2

#define XYZ_OVEN_COL_START      XYZ_COL1
#define XYZ_OVEN_COL_END        XYZ_COL3

#define XYZ_OVEN_X_START        20
//#define XYZ_OVEN_X_START        10
#define XYZ_OVEN_Y_START        0

#define XYZ_OVEN_X_DISTANCE     44          // in mm
#define XYZ_OVEN_X_OFFSET       XYZ_OVEN_X_DISTANCE * X_MOTOR_RESOLUTION / X_MOTOR_ROLLING_CIRCUMFERENCE
//#define XYZ_OVEN_X_OFFSET       190
#define XYZ_OVEN_Y_DISTANCE     143         // in mm
#define XYZ_OVEN_Y_OFFSET       XYZ_OVEN_Y_DISTANCE * Y_MOTOR_RESOLUTION / Y_MOTOR_ROLLING_CIRCUMFERENCE
//#define XYZ_OVEN_Y_OFFSET       855

#define XYZ_OVEN_Z_DOWN_POS     1850        //! Oven Stations

// Bath Layout
#define XYZ_BL_ROW_START        XYZ_ROW1
#define XYZ_BL_ROW_END          XYZ_ROW4

#define XYZ_BL_COL_START        XYZ_COL4
#define XYZ_BL_COL_END          XYZ_COL17

#define XYZ_BL_X_LEFT_DISTANCE  201         // in mm
#define XYZ_BL_X_START_LEFT     XYZ_BL_X_LEFT_DISTANCE * X_MOTOR_RESOLUTION / X_MOTOR_ROLLING_CIRCUMFERENCE
//#define XYZ_BL_X_START_LEFT     895
//#define XYZ_BL_X_RIGHT_DISTANCE  75          // in mm
//#define XYZ_BL_X_START_RIGHT    XYZ_BL_X_RIGHT_DISTANCE * X_MOTOR_RESOLUTION / X_MOTOR_ROLLING_CIRCUMFERENCE
#define XYZ_BL_X_START_RIGHT    110
#define XYZ_BL_Y_START          70
//#define XYZ_BL_Y_START          90

#define XYZ_BL_X_DISTANCE       55          // in mm
#define XYZ_BL_X_OFFSET         XYZ_BL_X_DISTANCE * X_MOTOR_RESOLUTION / X_MOTOR_ROLLING_CIRCUMFERENCE
//#define XYZ_BL_X_OFFSET         245
#define XYZ_BL_Y_DISTANCE       148         // in mm
#define XYZ_BL_Y_OFFSET         XYZ_BL_Y_DISTANCE * Y_MOTOR_RESOLUTION / Y_MOTOR_ROLLING_CIRCUMFERENCE
//#define XYZ_BL_Y_OFFSET         980

//#define XYZ_BL_Z_DOWN_POS       1900        //! Normal Station
#define XYZ_BL_Z_DOWN_POS       1870        //! Normal Station
#define XYZ_HS_Z_DOWN_POS       1850        //! Heated Cuvettes

#define ROW4_Y_DISTANCE         (156-148)   // in mm
#define ROW4_Y_OFFSET           ROW4_Y_DISTANCE * Y_MOTOR_RESOLUTION / Y_MOTOR_ROLLING_CIRCUMFERENCE
//#define ROW4_Y_OFFSET           60          //! Bath layout to Drawer Offset

#define ROW1_X_OFFSET           5
#define ROW1_Y_OFFSET           5

#define ROW2_X_OFFSET           10
#define ROW3_X_OFFSET           10
#define ROW4_X_OFFSET           5

// Rack Transfer
#define XYZ_RTS_ROW             XYZ_ROW2
#define XYZ_RTS_COL             XYZ_COL18

#define XYZ_RTS_X_POS           4315
#define XYZ_RTS_Y_POS           140
#define XYZ_RTS_Z_DOWN_POS      1600        //! Rack Transfer

// Slide counter
#define XYZ_SLIDE_COUNTER_ROW           XYZ_ROW3
#define XYZ_SLIDE_COUNTER_COL           XYZ_COL2

//#define XYZ_SLIDE_COUNTER_X_POS         240
#define XYZ_SLIDE_COUNTER_X_POS         255
//#define XYZ_SLIDE_COUNTER_Y_START       1800
#define XYZ_SLIDE_COUNTER_Y_START       2000
#define XYZ_SLIDE_COUNTER_Y_END         (XYZ_SLIDE_COUNTER_Y_START + 680)

#define XYZ_SLIDE_COUNTER_Z_DOWN_POS    1230        //! Slide counter

class CPoint
{
public:
    quint32 m_PositionX;
    quint32 m_PositionY;
    quint32 m_PositionZ;

    quint8 m_ProfileX;
    quint8 m_ProfileY;
    quint8 m_ProfileZ;

    CPoint(){}

    CPoint(quint32 PositionX, quint8 ProfileX,
           quint32 PositionY, quint8 ProfileY,
           quint32 PositionZ = NO_CHANGE, quint8 ProfileZ = 0)
    {
        m_PositionX = PositionX;
        m_PositionY = PositionY;
        m_PositionZ = PositionZ;

        m_ProfileX = ProfileX;
        m_ProfileY = ProfileY;
        m_ProfileZ = ProfileZ;
    }
};

struct PositionXYZ
{
    quint32 PositionX;
    quint32 PositionY;
    quint32 PositionZ;
    bool Active;
};

typedef enum {
    XYZ_STATE_IDLE = 0,
    XYZ_STATE_MOVE_EMPTY,
    XYZ_STATE_MOVE_RACK,
    XYZ_STATE_ATTACH,
    XYZ_STATE_DETACH,
    XYZ_STATE_LET_DOWN_RACK,
    XYZ_STATE_PULL_UP_RACK
} XyzState_t;

typedef enum {
    X_AXIS = 0, Y_AXIS, Z_AXIS, MAX_STEPPER_MOTORS
} StepperMotorAxis_t;

class CDeviceXyz : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceXyz(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList, DevInstanceID_t InstanceID);

    // Setter
    void SetMoveEmptyProfile(quint8 ProfileX, quint8 ProfileY, quint8 ProfileZ);
    void SetTransposrtRackProfile(quint8 ProfileX, quint8 ProfileY, quint8 ProfileZ);

    // Getter
    XyzState_t GetCurrentstate();

signals:
    // Movement interface
    // Request interface to DCP
    void MoveEmptyTo(quint16 StationColumn, quint16 StationRow);
    void MoveRackTo(quint16 StationColumn, quint16 StationRow);
    void AttachRack();
    void DetachRack();
    void LetDownRack();
    void PullUpRack();

    void ReadRfid();
    void ReadLiquidLevel();

    // Response interface to DCP
    void ReportMoveEmptyTo(ReturnCode_t ReturnCode);
    void ReportMoveRackTo(ReturnCode_t ReturnCode);
    void ReportAttachRack(ReturnCode_t ReturnCode);
    void ReportDetachRack(ReturnCode_t ReturnCode);
    void ReportLetDownRack(ReturnCode_t ReturnCode);
    void ReportPullUpRack(ReturnCode_t ReturnCode);

    void ReportReadRfid(ReturnCode_t ReturnCode);
    void ReportReadLiquidLevel(ReturnCode_t ReturnCode);

    void MoveError(ReturnCode_t ReturnCode);

    // Internal signals
    void Move(quint32 PositionX, quint8 ProfileX,
              quint32 PositionY, quint8 ProfileY,
              quint32 PositionZ = NO_CHANGE, quint8 ProfileZ = 0);

    void MoveAbort();

protected:
    bool Trans_Configure(QEvent *p_Event);

    // Idle -> Move State
    bool Trans_Idle_MoveEmpty(QEvent *p_Event);
    bool Trans_Idle_MoveRack(QEvent *p_Event);
    bool Trans_Idle_LetdownRack(QEvent *p_Event);
    bool Trans_Idle_PullupRack(QEvent *p_Event);
    bool Trans_Idle_AttachRack(QEvent *p_Event);
    bool Trans_Idle_DetachRack(QEvent *p_Event);

    // Move -> Idle State
    // On Movement completion
    bool Trans_Move_Idle(QEvent *p_Event);

    // On Abort request
    bool Trans_Abort_Idle(QEvent *p_Event);

    // Helper functions
    void MoveNextStep();
    void DeletePrevStep();
    void FillColumnRowPosition();
    bool IsNewState(XyzState_t NewState, quint16 StationColumn, quint16 StationRow);

private:
    // Function Modueles
    CStepperMotor *mp_XAxisMotor;
    CStepperMotor *mp_YAxisMotor;
    CStepperMotor *mp_ZAxisMotor;
    CRfid11785 *mp_Rfid;

    //
    CMoveXYZ *mp_MoveXyz;

    // Motion Profiles
    quint8 m_MoveEmptyProfile[MAX_STEPPER_MOTORS];
    quint8 m_TransportRackProfile[MAX_STEPPER_MOTORS];

    //
    XyzState_t m_PrevState;
    XyzState_t m_CurrentState;
    bool m_RackAttached;

    quint16 m_StationColumn;
    quint16 m_StationRow;

    //
    QList<CPoint*> m_WayPoint;

    //
    PositionXYZ m_StaionPos[XYZ_MAX_COLS][XYZ_MAX_ROWS];
};

}

#endif // DEVICEXYZ_H
