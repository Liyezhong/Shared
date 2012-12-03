/****************************************************************************/
/*! \file   ReadRackRfid.cpp
 *
 *  \brief  Implementaion file for class CReadRackRfid
 *
 *  \version  0.1
 *  \date     2012-11-22
 *  \author   M.Scherer
 *
 *  \b Description:
 *
 *   N/A
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

#include "DeviceControl/Include/Devices/ReadRackRfid.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"

namespace DeviceControl
{

//! Signal transition for CReadRackRfid
typedef CSignalTransition<CReadRackRfid> CReadRackRfidTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CReadRackRfid
 *
 *  \iparam p_Rfid11785 = RFID ISO 11785 function module
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CReadRackRfid::CReadRackRfid(CRfid11785 *p_Rfid11785, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent), mp_Rfid11785(p_Rfid11785)
{
    //lint -esym(429, p_Idle)
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

/****************************************************************************/
/*!
 *  \brief  Sends the set state command
 *
 *  \iparam p_Event = Parameters of the signal ReadRackRfid
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CReadRackRfid::SetState(QEvent *p_Event)
{
    // ReadRackRfid(LoaderRFIDChannel_t)
    if (!CReadRackRfidTransition::GetEventValue(p_Event, 0, m_Channel)) {
        emit RfidError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode_t ReturnCode = mp_Rfid11785->SetState(true, (quint8)m_Channel - 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit RfidError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Sends the send login command
 *
 *  \iparam p_Event = Parameters of the signal ReportStateAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief  Sends the read UID command
 *
 *  \iparam p_Event = Parameters of the signal ReportLoginAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief  Sends the read data command
 *
 *  \iparam p_Event = Parameters of the signal ReportUID
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief  Acknowledges the read rack rfid operation
 *
 *  \iparam p_Event = Parameters of the signal ReportUserData
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief  Called when an error occurs during the transaction
 *
 *  \iparam p_Event = Parameters of the signal RfidError
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CReadRackRfid::Nack(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CReadRackRfidTransition::GetEventValue(p_Event, 0);
    emit ReportReadRackRfid(ReturnCode, m_Channel, m_Uid, m_Data);
    return true;
}


} //namespace

// vi: set ts=4 sw=4 et:
