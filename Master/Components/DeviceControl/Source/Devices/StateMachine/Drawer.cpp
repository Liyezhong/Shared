#include "DeviceControl/Include/Devices/StateMachine/Drawer.h"
#include "DeviceControl/Include/Devices/StateMachine/DrawerState.h"
#include "DeviceControl/Include/Devices/StateMachine/MoveEndToEnd.h"
#include "DeviceControl/Include/Devices/StateMachine/ReadRackRfid.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

namespace DeviceControl
{

CDrawer::CDrawer(const DeviceProcessing &DeviceProcessing, const DeviceFctModList &SlaveModuleList, DevInstanceID_t InstanceId) :
    CDevice(DeviceProcessing, SlaveModuleList, InstanceId)
{   
    mp_State = CBlockedState::Instance();

    if (InstanceId != DEVICE_INSTANCE_ID_LOADER && InstanceId != DEVICE_INSTANCE_ID_UNLOADER) {
        THROWARG(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR, InstanceId);
    }

    mp_Motor = static_cast<CStepperMotor *>(DeviceProcessing.GetFunctionModule(
                                                GetSlaveModuleInstanceIdFromKey(CANObjectKeyLUT::m_MotorDrawerKey)));
    if (mp_Motor == NULL) {
        THROWARG(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR, CANObjectKeyLUT::m_MotorDrawerKey);
    }

    mp_Rfid = static_cast<CRfid11785 *>(DeviceProcessing.GetFunctionModule(
                                            GetSlaveModuleInstanceIdFromKey(CANObjectKeyLUT::m_DrawerRFIDKey)));
    if (mp_Rfid == NULL) {
        THROWARG(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR, CANObjectKeyLUT::m_DrawerRFIDKey);
    }

    mp_Button = static_cast<CDigitalInput *>(DeviceProcessing.GetFunctionModule(
                                                 GetSlaveModuleInstanceIdFromKey(CANObjectKeyLUT::m_DrawerButtonKey)));
    if(mp_Button == NULL) {
        THROWARG(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR, CANObjectKeyLUT::m_DrawerButtonKey);
    }

    mp_LedFree = static_cast<CDigitalOutput *>(DeviceProcessing.GetFunctionModule(
                                                   GetSlaveModuleInstanceIdFromKey(CANObjectKeyLUT::m_DrawerLEDFreeKey)));
    if (mp_LedFree == NULL) {
        THROWARG(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR, CANObjectKeyLUT::m_DrawerLEDFreeKey);
    }

    mp_LedBlocked = static_cast<CDigitalOutput *>(DeviceProcessing.GetFunctionModule(
                                                      GetSlaveModuleInstanceIdFromKey(CANObjectKeyLUT::m_DrawerLEDBlockedKey)));
    if (mp_LedBlocked == NULL) {
        THROWARG(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR, CANObjectKeyLUT::m_DrawerLEDBlockedKey);
    }

    mp_MoveEndToEnd = new CMoveEndToEnd(mp_Motor);
    mp_ReadRackRfid = new CReadRackRfid(mp_Rfid);

    CONNECTSIGNALSIGNAL(mp_ReadRackRfid, ReportReadRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32),
                        this, ReportReadRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32));
}

CDrawer::~CDrawer()
{
    delete mp_ReadRackRfid;
    delete mp_MoveEndToEnd;
}

void CDrawer::ReadRfid(LoaderRFIDChannel_t Channel)
{
    mp_ReadRackRfid->ReadRackRfid(Channel);
}

void CDrawer::ChangeState(CDrawerState *p_State)
{
    mp_State = p_State;
}

} //namespace
