#include "DeviceControl/Include/Devices/StateMachine/ReadRackRfid.h"
#include "DeviceControl/Include/Devices/StateMachine/ReadRackRfidState.h"
#include <iostream>

namespace DeviceControl
{

/****************************************************************************/
CReadRackRfid::CReadRackRfid(CRfid11785 *p_Rfid11785) : mp_Rfid11785(p_Rfid11785)
{
    mp_State = CIdleState::Instance();
}

void CReadRackRfid::ReadRackRfid(LoaderRFIDChannel_t Channel)
{
    mp_State->ReadRackRfid(this, mp_Rfid11785, Channel);
}

void CReadRackRfid::OnReportSetChannel(ReturnCode_t ReturnCode)
{
    mp_State->OnReportSetChannel(this, mp_Rfid11785, ReturnCode);
}

void CReadRackRfid::OnReportSendLogin(ReturnCode_t ReturnCode)
{
    mp_State->OnReportSendLogin(this, mp_Rfid11785, ReturnCode);
}

void CReadRackRfid::OnReportReadUid(ReturnCode_t ReturnCode, quint32 Uid)
{
    mp_State->OnReportReadUid(this, mp_Rfid11785, ReturnCode, Uid);
}

void CReadRackRfid::OnReportReadData(ReturnCode_t ReturnCode, quint32 Data)
{
    mp_State->OnReportReadData(this, ReturnCode, Data);
}

void CReadRackRfid::ChangeState(CReadRackRfidState *p_State)
{
    mp_State = p_State;
}

void CReadRackRfid::ReportReadRfid(ReturnCode_t ReturnCode)
{
    emit ReportReadRfid(ReturnCode, m_Channel, m_Uid, m_Data);
}

} //namespace
