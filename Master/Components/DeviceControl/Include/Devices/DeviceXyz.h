#ifndef DEVICEXYZ_H
#define DEVICEXYZ_H

#include "DeviceBase.h"

#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"

namespace DeviceControl
{

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

signals:
    // Movement interface
    // Request interface to DCP
    void MoveEmptyTo(quint16 StationColumn, quint16 StationRow);
    void TransportRackTo(quint16 StationColumn, quint16 StationRow);
    void AttachRack();
    void DetachRack();
    void LetDownRack();
    void PullUpRack();

    void ReadRfid();
    void ReadLiquidLevel();

    // Response interface to DCP
    void ReportMoveEmptyTo(ReturnCode_t ReturnCode);
    void ReportTransportRackTo(ReturnCode_t ReturnCode);
    void ReportAttachRack(ReturnCode_t ReturnCode);
    void ReportDetachRack(ReturnCode_t ReturnCode);
    void ReportLetDownRack(ReturnCode_t ReturnCode);
    void ReportPullUpRack(ReturnCode_t ReturnCode);

    void ReportReadRfid(ReturnCode_t ReturnCode);
    void ReportReadLiquidLevel(ReturnCode_t ReturnCode);

    // Internal signals
    void Move(quint32 PositionX, quint8 ProfileX,
              quint32 PositionY, quint8 ProfileY,
              quint32 PositionZ, quint8 ProfileZ);

protected:
    bool Trans_Configure(QEvent *p_Event);

    bool Trans_Idle_MoveEmpty(QEvent *p_Event);
    bool Trans_Idle_TransportRack(QEvent *p_Event);

    bool Trans_MoveEmpty_Idle(QEvent *p_Event);
    bool Trans_TransportRack_Idle(QEvent *p_Event);

    // Helper functions
    quint32 ColumnToPosition(quint16 StationColumn);
    quint32 RowToPosition(quint16 StationRow);

protected slots:
    void ResetPositions(void);

private:
    CStepperMotor *mp_XAxisMotor;
    CStepperMotor *mp_YAxisMotor;
    CStepperMotor *mp_ZAxisMotor;

    CRfid11785 *mp_Rfid;

    quint8 m_MoveEmptyProfile[MAX_STEPPER_MOTORS];
    quint8 m_TransportRackProfile[MAX_STEPPER_MOTORS];

    quint32 m_CurrentPositionX;
    quint32 m_CurrentPositionY;
    quint32 m_CurrentPositionZ;
};

}

#endif // DEVICEXYZ_H
