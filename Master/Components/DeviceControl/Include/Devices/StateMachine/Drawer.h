#ifndef DEVICECONTROL_DRAWER_H
#define DEVICECONTROL_DRAWER_H

#include "DeviceControl/Include/Devices/StateMachine/Device.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include <QDateTime>

namespace DeviceControl
{

class CDrawerState;
class CMoveEndToEnd;
class CReadRackRfid;

/****************************************************************************/
class CDrawer : public CDevice
{
    Q_OBJECT

public:
    CDrawer(const DeviceProcessing &DeviceProcessing, const DeviceFctModList &SlaveModuleList, DevInstanceID_t InstanceId);
    ~CDrawer();

public slots:
    /*void Initialize();
    void SetBlockState(BlockState_t BlockState);
    void Stop();*/
    void ReadRfid(LoaderRFIDChannel_t Channel);

    /*void GetBlockState();
    void GetPosition();*/

signals:
    void ReportInitialize(ReturnCode_t Result);
    void ReportSetBlockState(ReturnCode_t Result, BlockState_t BlockState);
    void ReportStop(ReturnCode_t Result);
    void ReportReadRfid(ReturnCode_t Result, LoaderRFIDChannel_t Channel, quint32 Uid, quint32 Data);

    void ReportGetBlockState(ReturnCode_t Result, BlockState_t BlockState);
    void ReportGetPosition(ReturnCode_t Result, LoaderPosition_t Position);

    void ReportPositionReached(ReturnCode_t Result, LoaderPosition_t Position);

    void ReportError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime);

private:
    CStepperMotor* mp_Motor;        //!< Drawer motor function module
    CRfid11785* mp_Rfid;            //!< RFID function module
    CDigitalInput* mp_Button;       //!< Drawer button function module
    CDigitalOutput* mp_LedFree;     //!< Drawer status 'free' LED
    CDigitalOutput* mp_LedBlocked;  //!< Drawer status 'blocked' LED

    friend class CDrawerState;
    void ChangeState(CDrawerState *p_State);
    CDrawerState *mp_State;

    CMoveEndToEnd *mp_MoveEndToEnd;
    CReadRackRfid *mp_ReadRackRfid;
};

} //namespace

#endif /* DEVICECONTROL_DRAWER_H */
