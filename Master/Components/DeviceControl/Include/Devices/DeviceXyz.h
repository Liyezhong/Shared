/****************************************************************************/
/*! \file   DeviceXyz.h
 *
 *  \brief  Definition file for class CDeviceXyz
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
#ifndef DEVICECONTROL_DEVICEXYZ_H
#define DEVICECONTROL_DEVICEXYZ_H

#include <QTimer>

#include "DeviceBase.h"
#include "MoveXYZ.h"

#include "DataManager/Containers/Adjustment/Include/Adjustment.h"
#include "DataManager/Containers/Adjustment/Include/AdjustmentVerifier.h"

namespace DeviceControl
{

class CRfid11785;
class CStepperMotor;
class CAnalogInput;
class CDigitalOutput;

#define ATTACH_POSITION             70

#define Z_MOVE_UP_SLOW_WITH_RACK    1000
#define Z_MOVE_UP_FAST_WITH_RACK    0
#define Z_MOVE_UP_WITHOUT_RACK      1450

#define LIQUID_LEVEL_X_OFFSET       200

#define SLIDE_COUNT_MOVE_POS        500

//!< Sensor teach duration in milli seconds
#define SENSOR_TEACH_DURATION       2500

class CPoint
{
public:
    quint32 m_PositionX;
    quint32 m_PositionY;
    quint32 m_PositionZ;

    quint8 m_ProfileX;
    quint8 m_ProfileY;
    quint8 m_ProfileZ;

    CPoint() : m_PositionX(0),
        m_PositionY(0),
        m_PositionZ(0),
        m_ProfileX(0),
        m_ProfileY(0),
        m_ProfileZ(0)
    {
    }

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

typedef enum {
    XYZ_STATE_IDLE = 0,
    XYZ_STATE_MOVE_EMPTY,
    XYZ_STATE_MOVE_RACK,
    XYZ_STATE_ATTACH,
    XYZ_STATE_DETACH,
    XYZ_STATE_LET_DOWN_RACK,
    XYZ_STATE_PULL_UP_RACK,
    XYZ_STATE_COUNT_SLIDES,
    XYZ_STATE_READ_LIQUID_LEVEL,
    XYZ_STATE_TEACH_SENSOR
} XyzState_t;

class CDeviceXyz : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceXyz(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList, DevInstanceID_t InstanceID);

    ~CDeviceXyz();

    // Getter
    XyzState_t GetCurrentstate();

signals:
    // Movement interface
    // Request interface to DCP
    /****************************************************************************/
    /*! \brief  Interface to DCP to move the arm when empty
     *
     *  \iparam StationColumn   Target station column
     *  \iparam StationRow      Target station row
     */
    /****************************************************************************/
    void MoveEmptyTo(quint16 StationColumn, quint16 StationRow);

    /****************************************************************************/
    /*! \brief Interface to DCP to move the arm when rack is attached
     *
     *  \iparam StationColumn   Target station column
     *  \iparam StationRow      Target station row
     */
    /****************************************************************************/
    void MoveRackTo(quint16 StationColumn, quint16 StationRow);

    /****************************************************************************/
    /*! \brief Interface to DCP to attach rack
     */
    /****************************************************************************/
    void AttachRack();

    /****************************************************************************/
    /*! \brief Interface to DCP to detach Rack
     */
    /****************************************************************************/
    void DetachRack();

    /****************************************************************************/
    /*! \brief Interface to DCP to let the rack down
     */
    /****************************************************************************/
    void LetDownRack();

    /****************************************************************************/
    /*! \brief Interface to DCP to pull up the rack
     */
    /****************************************************************************/
    void PullUpRack();

    /****************************************************************************/
    /*! \brief Interface to DCP to count slides
     */
    /****************************************************************************/
    void CountSlides();

    /****************************************************************************/
    /*! \brief Interface to DCP to Read rfid id & data of the attached rack
     */
    /****************************************************************************/
    void ReadRfid();

    /****************************************************************************/
    /*! \brief Interface to DCP to liquid level of currently pointing station
     */
    /****************************************************************************/
    void ReadLiquidLevel(quint16 StationColumn, quint16 StationRow);

    void TeachLevelSensor(quint16 MinStationColumn, quint16 MinStationRow,
                          quint16 MaxStationColumn, quint16 MaxStationRow);
    void TeachLevelSensorMin(quint16 StationColumn, quint16 StationRow);
    void TeachLevelSensorMax(quint16 StationColumn, quint16 StationRow);

    // Response interface to DCP
    /****************************************************************************/
    /*! \brief Response to MoveEmptyTo() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportMoveEmptyTo(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Response to MoveRackTo() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportMoveRackTo(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Response to AttachRack() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportAttachRack(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Response to DetachRack() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportDetachRack(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Response to LetDownRack() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportLetDownRack(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Response to PullUpRack() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportPullUpRack(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Response to CountSlides() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportCountSlides(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Response to ReadRfid() request.
     *
     *  \iparam ReturnCode result code sent by FM layer
     */
    /****************************************************************************/
    void ReportReadRfid(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Response to ReadLiquidLevel() request.
     *
     *  \iparam ReturnCode result code
     */
    /****************************************************************************/
    void ReportReadLiquidLevel(ReturnCode_t ReturnCode, qint16 LiquidLevel);

    /****************************************************************************/
    /*! \brief Response to TeachLevelSensor() request.
     *
     *  \iparam ReturnCode result code
     */
    /****************************************************************************/
    void ReportTeachSensor(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief Error Response to signal movement error
     *
     *  \iparam ReturnCode Reason for Failure / Error
     */
    /****************************************************************************/
    void MoveError(ReturnCode_t ReturnCode);

    // Internal signals
    /****************************************************************************/
    /*! \brief  Internal signal to initiate arm movement
     *
     *  \iparam m_PositionX   Target X position
     *  \iparam ProfileX    Motion profile for X axis
     *  \iparam PositionY   Target Y position
     *  \iparam ProfileY    Motion profile for Y axis
     *  \iparam PositionZ   Target Z position
     *  \iparam ProfileZ    Motion profile for Z axis
     */
    /****************************************************************************/
    void Move(quint32 m_PositionX, quint8 ProfileX,
              quint32 PositionY, quint8 ProfileY,
              quint32 PositionZ = NO_CHANGE, quint8 ProfileZ = 0);

    /****************************************************************************/
    /*! \brief  Internal signal to abort movement in progress
     */
    /****************************************************************************/
    void MoveAbort();

private:
    bool Trans_Configure(QEvent *p_Event);

    // Idle -> Active State
    bool Trans_Idle_MoveEmpty(QEvent *p_Event);
    bool Trans_Idle_MoveRack(QEvent *p_Event);
    bool Trans_Idle_LetdownRack(QEvent *p_Event);
    bool Trans_Idle_PullupRack(QEvent *p_Event);
    bool Trans_Idle_AttachRack(QEvent *p_Event);
    bool Trans_Idle_DetachRack(QEvent *p_Event);
    bool Trans_Idle_CountSlides(QEvent *p_Event);

    bool Trans_Idle_MoveLiquidLevelPos(QEvent *p_Event);
    bool Trans_MoveLiquidLevel_Read(QEvent *p_Event);
    bool Trans_ReadLiquidLevel_Idle(QEvent *p_Event);

    bool Trans_Idle_MoveTeachSensorPos(QEvent *p_Event);
    bool Trans_Move_TeachSensor(QEvent *p_Event);
    bool Trans_TeachSensor_Idle(QEvent *p_Event);

    // Active -> Idle State
    // On Movement completion
    bool Trans_Active_Idle(QEvent *p_Event);

    // On Abort request
    bool Trans_Abort_Idle(QEvent *p_Event);

    // Helper functions
    void MoveNextStep();
    void DeletePrevStep();

    bool IsNewState(XyzState_t NewState, quint16 StationColumn, quint16 StationRow);
    void ReportStatus(ReturnCode_t ReturnCode);

    // Function Modules
    CBaseModule *mp_BaseModuleX;
    CBaseModule *mp_BaseModuleY;
    CBaseModule *mp_BaseModuleZ;
    CStepperMotor *mp_XAxisMotor;
    CStepperMotor *mp_YAxisMotor;
    CStepperMotor *mp_ZAxisMotor;
    CRfid11785 *mp_Rfid;
    CAnalogInput *mp_LevelSensor;
    CDigitalOutput *mp_LevelSensorTeach;

    QTimer m_Timer;

    //
    CMoveXYZ *mp_MoveXyz;

    //
    XyzState_t m_PrevState;
    XyzState_t m_CurrentState;
    bool m_RackAttached;
    qint16 m_LiquidLevel;

    quint16 m_StationColumn;
    quint16 m_StationRow;

    // Way point list
    QList<CPoint*> m_WayPoint;

    // Adjustment data container
    DataManager::CAdjustment *mp_Adjustment;
    DataManager::XyzType_t m_XyzType;

    // Speed profiles & positions
    quint8 m_Prof_X_Move_Empty;
    quint8 m_Prof_X_Move_With_Rack;

    quint8 m_Prof_Y_Move_Empty;
    quint8 m_Prof_Y_Move_With_Rack;
    quint8 m_Prof_Y_Attach_Detach_Rack;

    quint8 m_Prof_Z_Move_Empty;
    quint8 m_Prof_Z_Move_Down_With_Rack;
    quint8 m_Prof_Z_Move_Up_Slow;
    quint8 m_Prof_Z_Move_Up_Fast;
};

}

#endif // DEVICECONTROL_DEVICEXYZ_H
