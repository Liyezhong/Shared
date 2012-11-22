#ifndef DEVICECONTROL_READRACKRFIDSTATE_H
#define DEVICECONTROL_READRACKRFIDSTATE_H

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

class CReadRackRfid;
class CRfid11785;

/****************************************************************************/
class CReadRackRfidState
{
public:
    virtual ~CReadRackRfidState() {}
    virtual void ReadRackRfid(CReadRackRfid *, CRfid11785 *, LoaderRFIDChannel_t) {}
    virtual void OnReportSetChannel(CReadRackRfid *, CRfid11785 *, ReturnCode_t) {}
    virtual void OnReportSendLogin(CReadRackRfid *, CRfid11785 *, ReturnCode_t) {}
    virtual void OnReportReadUid(CReadRackRfid *, CRfid11785 *, ReturnCode_t, quint32) {}
    virtual void OnReportReadData(CReadRackRfid *, ReturnCode_t, quint32) {}

protected:
    void ChangeState(CReadRackRfid *p_Context, CReadRackRfidState *p_State);
    void ReportReadRfid(CReadRackRfid *p_Context, ReturnCode_t ReturnCode);
    void SetChannel(CReadRackRfid *p_Context, LoaderRFIDChannel_t Channel);
    void SetUid(CReadRackRfid *p_Context, quint32 Uid);
    void SetData(CReadRackRfid *p_Context, quint32 Data);
};

/****************************************************************************/
class CIdleState : public CReadRackRfidState
{
public:
    void ReadRackRfid(CReadRackRfid *p_Context, CRfid11785 *p_Rfid11785, LoaderRFIDChannel_t Channel);
    static CReadRackRfidState *Instance() { return &m_Instance; }

private:
    static quint8 GetRfidIndexFromChannel(LoaderRFIDChannel_t Channel);
    static CIdleState m_Instance;
};

/****************************************************************************/
class CSetChannelState : public CReadRackRfidState
{
public:
    void OnReportSetChannel(CReadRackRfid *p_Context, CRfid11785 *p_Rfid11785, ReturnCode_t ReturnCode);
    static CReadRackRfidState *Instance() { return &m_Instance; }

private:
    static CSetChannelState m_Instance;
};

/****************************************************************************/
class CSendLoginState : public CReadRackRfidState
{
public:
    void OnReportSendLogin(CReadRackRfid *p_Context, CRfid11785 *p_Rfid11785, ReturnCode_t ReturnCode);
    static CReadRackRfidState *Instance() { return &m_Instance; }

private:
    static CSendLoginState m_Instance;
};

/****************************************************************************/
class CReadUidState : public CReadRackRfidState
{
public:
    void OnReportReadUid(CReadRackRfid *p_Context, CRfid11785 *p_Rfid11785, ReturnCode_t ReturnCode, quint32 Uid);
    static CReadRackRfidState *Instance() { return &m_Instance; }

private:
    static CReadUidState m_Instance;
};

/****************************************************************************/
class CReadDataState : public CReadRackRfidState
{
public:
    void OnReportReadData(CReadRackRfid *p_Context, ReturnCode_t ReturnCode, quint32 Data);
    static CReadRackRfidState *Instance() { return &m_Instance; }

private:
    static CReadDataState m_Instance;
};

} //namespace

#endif /* DEVICECONTROL_READRACKRFIDSTATE_H */
