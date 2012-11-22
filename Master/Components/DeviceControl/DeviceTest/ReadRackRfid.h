#ifndef DEVICECONTROL_READRACKRFID_H
#define DEVICECONTROL_READRACKRFID_H

#include "State.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

class CMockFm;

class CReadRackRfid : public CState
{
    Q_OBJECT

public:
    explicit CReadRackRfid(CMockFm *p_Rfid11785, const QString &Name, QState *p_Parent = 0);

signals:
    // command request interface to DCP
    void ReadRackRfid(LoaderRFIDChannel_t Channel);

    // command response interface to DCP
    void ReportReadRackRfid(ReturnCode_t ReturnCode, LoaderRFIDChannel_t Channel, quint32 Uid, quint32 Data);

    // to this
    void RfidError(ReturnCode_t ReturnCode);
    
private:
    bool SetState(QEvent *p_Event);
    bool SendLogin(QEvent *p_Event);
    bool ReadUid(QEvent *p_Event);
    bool ReadData(QEvent *p_Event);
    bool ReportRead(QEvent *p_Event);
    bool Nack(QEvent *p_Event);

    CMockFm *mp_Rfid11785;
    LoaderRFIDChannel_t m_Channel;
    quint32 m_Uid;
    quint32 m_Data;
};

} //namespace

#endif // DEVICECONTROL_READRACKRFID_H
