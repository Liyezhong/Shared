#include "ReadRackRfid.h"
#include "SignalTransition.h"

#include "MockFm.h"

namespace DeviceControl
{

typedef CSignalTransition<CReadRackRfid> CReadRackRfidTransition;

CReadRackRfid::CReadRackRfid(CMockFm *p_Rfid11785, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent), mp_Rfid11785(p_Rfid11785)
{
    CState *p_Idle = new CState("Idle", this);
    CState *p_Active = new CState("Active", this);
    setInitialState(p_Idle);

    CState *p_SetState = new CState("SetState", p_Active);
    CState *p_SendLogin = new CState("SendLogin", p_Active);
    CState *p_ReadUid = new CState("ReadUid", p_Active);
    CState *p_ReadData = new CState("ReadData", p_Active);
    p_Active->setInitialState(p_SetState);

    p_Idle->addTransition(new CReadRackRfidTransition(
        this, SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)),
        *this, &CReadRackRfid::SetState, 
        p_Active));

    p_SetState->addTransition(new CReadRackRfidTransition(
        mp_Rfid11785, SIGNAL(ReportStateAckn(quint32, ReturnCode_t)),
        *this, &CReadRackRfid::SendLogin, 
        p_SendLogin));
    p_SendLogin->addTransition(new CReadRackRfidTransition(
        mp_Rfid11785, SIGNAL(ReportLoginAckn(quint32, ReturnCode_t)),
        *this, &CReadRackRfid::ReadUid, 
        p_ReadUid));
    p_ReadUid->addTransition(new CReadRackRfidTransition(
        mp_Rfid11785, SIGNAL(ReportUID(quint32, ReturnCode_t, quint32)),
        *this, &CReadRackRfid::ReadData, 
        p_ReadData));
    p_ReadData->addTransition(new CReadRackRfidTransition(
        mp_Rfid11785, SIGNAL(ReportUserData(quint32, ReturnCode_t, quint8, quint32)),
        *this, &CReadRackRfid::ReportRead, 
        p_Idle));

    p_Active->addTransition(new CReadRackRfidTransition(
        this, SIGNAL(RfidError(ReturnCode_t)),
        *this, &CReadRackRfid::Nack, 
        p_Idle));

}

bool CReadRackRfid::SetState(QEvent *p_Event)
{
    // ReadRackRfid(LoaderRFIDChannel_t)
    if (!CReadRackRfidTransition::GetEventValue(p_Event, 0, m_Channel)) {
        emit RfidError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode_t ReturnCode = mp_Rfid11785->SetState(true, m_Channel);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit RfidError(ReturnCode);
        return false;
    }

    return true;
}

bool CReadRackRfid::SendLogin(QEvent *p_Event)
{
    // ReportStateAckn(quint32, ReturnCode_t)
    ReturnCode_t ReturnCode = CReadRackRfidTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit RfidError(ReturnCode);
        return false;
    }

    ReturnCode = mp_Rfid11785->SetLogin(0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit RfidError(ReturnCode);
        return false;
    }

    return true;
}

bool CReadRackRfid::ReadUid(QEvent *p_Event)
{
    // ReportLoginAckn(quint32, ReturnCode_t)
    ReturnCode_t ReturnCode = CReadRackRfidTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit RfidError(ReturnCode);
        return false;
    }

    ReturnCode = mp_Rfid11785->ReqUID();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit RfidError(ReturnCode);
        return false;
    }

    return true;
}

bool CReadRackRfid::ReadData(QEvent *p_Event)
{
    // ReportUID(quint32, ReturnCode_t, quint32)
    ReturnCode_t ReturnCode = CReadRackRfidTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit RfidError(ReturnCode);
        return false;
    }
    
    if (!CReadRackRfidTransition::GetEventValue(p_Event, 2, m_Uid)) {
        emit RfidError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode = mp_Rfid11785->ReqUserData(0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit RfidError(ReturnCode);
        return false;
    }

    return true;
}

bool CReadRackRfid::ReportRead(QEvent *p_Event)
{
    // ReportUserData(quint32, ReturnCode_t, quint8, quint32)
    ReturnCode_t ReturnCode = CReadRackRfidTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit RfidError(ReturnCode);
        return false;
    }

    if (!CReadRackRfidTransition::GetEventValue(p_Event, 3, m_Data)) {
        emit RfidError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    emit ReportReadRackRfid(ReturnCode, m_Channel, m_Uid, m_Data);
    return true;
}

bool CReadRackRfid::Nack(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CReadRackRfidTransition::GetEventValue(p_Event, 0);
    emit ReportReadRackRfid(ReturnCode, m_Channel, m_Uid, m_Data);
    return true;
}


} //namespace

// vi: set ts=4 sw=4 et:

