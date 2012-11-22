#include "DeviceControl/Include/Devices/StateMachine/ReadRackRfid.h"
#include "DeviceControl/Include/Devices/StateMachine/ReadRackRfidState.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"

namespace DeviceControl
{

/****************************************************************************/
void CReadRackRfidState::ChangeState(CReadRackRfid *p_Context, CReadRackRfidState *p_State)
{
    p_Context->ChangeState(p_State);
}

void CReadRackRfidState::ReportReadRfid(CReadRackRfid *p_Context, ReturnCode_t ReturnCode)
{
    p_Context->ReportReadRfid(ReturnCode);
}

void CReadRackRfidState::SetChannel(CReadRackRfid *p_Context, LoaderRFIDChannel_t Channel)
{
    p_Context->m_Channel = Channel;
}

void CReadRackRfidState::SetUid(CReadRackRfid *p_Context, quint32 Uid)
{
    p_Context->m_Uid = Uid;
}

void CReadRackRfidState::SetData(CReadRackRfid *p_Context, quint32 Data)
{
    p_Context->m_Data = Data;
}

/****************************************************************************/
CIdleState CIdleState::m_Instance;

void CIdleState::ReadRackRfid(CReadRackRfid *p_Context, CRfid11785 *p_Rfid11785, LoaderRFIDChannel_t Channel)
{
    ReturnCode_t Result = p_Rfid11785->SetState(true, GetRfidIndexFromChannel(Channel));
    if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
        SetChannel(p_Context, Channel);
        ChangeState(p_Context, CSetChannelState::Instance());
    }
    else {
        ReportReadRfid(p_Context, Result);
    }
}

quint8 CIdleState::GetRfidIndexFromChannel(LoaderRFIDChannel_t Channel)
{
    int Index = 0;

    switch (Channel) {
        case LOADER_ID_RFID_1:
            Index = 0;
            break;
        case LOADER_ID_RFID_2:
            Index = 1;
            break;
        case LOADER_ID_RFID_3:
            Index = 2;
            break;
        case LOADER_ID_RFID_4:
            Index = 3;
            break;
        case LOADER_ID_RFID_5:
            Index = 4;
            break;
        default:
            Index = 5;
            break;
    }

    return Index;
}

/****************************************************************************/
CSetChannelState CSetChannelState::m_Instance;

void CSetChannelState::OnReportSetChannel(CReadRackRfid *p_Context, CRfid11785 *p_Rfid11785, ReturnCode_t ReturnCode)
{
    if (ReturnCode == DCL_ERR_FCT_CALL_SUCCESS) {
        ReturnCode_t Result = p_Rfid11785->SetLogin(0);
        if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
            ChangeState(p_Context, CSendLoginState::Instance());
        }
        else {
            ReportReadRfid(p_Context, Result);
            ChangeState(p_Context, CIdleState::Instance());
        }
    }
    else {
        ReportReadRfid(p_Context, ReturnCode);
        ChangeState(p_Context, CIdleState::Instance());
    }
}

/****************************************************************************/
CSendLoginState CSendLoginState::m_Instance;

void CSendLoginState::OnReportSendLogin(CReadRackRfid *p_Context, CRfid11785 *p_Rfid11785, ReturnCode_t ReturnCode)
{
    if (ReturnCode == DCL_ERR_FCT_CALL_SUCCESS) {
        ReturnCode_t Result = p_Rfid11785->ReqUID();
        if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
            ChangeState(p_Context, CReadUidState::Instance());
        }
        else {
            ReportReadRfid(p_Context, Result);
            ChangeState(p_Context, CIdleState::Instance());
        }
    }
    else {
        ReportReadRfid(p_Context, ReturnCode);
        ChangeState(p_Context, CIdleState::Instance());
    }
}

/****************************************************************************/
CReadUidState CReadUidState::m_Instance;

void CReadUidState::OnReportReadUid(CReadRackRfid *p_Context, CRfid11785 *p_Rfid11785, ReturnCode_t ReturnCode, quint32 Uid)
{
    if (ReturnCode == DCL_ERR_FCT_CALL_SUCCESS) {
        ReturnCode_t Result = p_Rfid11785->ReqUserData(0);
        if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
            SetUid(p_Context, Uid);
            ChangeState(p_Context, CReadDataState::Instance());
        }
        else {
            ReportReadRfid(p_Context, Result);
            ChangeState(p_Context, CIdleState::Instance());
        }
    }
    else {
        ReportReadRfid(p_Context, ReturnCode);
        ChangeState(p_Context, CIdleState::Instance());
    }
}

/****************************************************************************/
CReadDataState CReadDataState::m_Instance;

void CReadDataState::OnReportReadData(CReadRackRfid *p_Context, ReturnCode_t ReturnCode, quint32 Data)
{
    SetData(p_Context, Data);
    ReportReadRfid(p_Context, ReturnCode);
    ChangeState(p_Context, CIdleState::Instance());
}

} //namespace
