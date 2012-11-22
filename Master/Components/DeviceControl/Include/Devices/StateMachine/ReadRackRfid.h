#ifndef DEVICECONTROL_READRACKRFID_H
#define DEVICECONTROL_READRACKRFID_H

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include <QDateTime>

namespace DeviceControl
{

class CReadRackRfidState;
class CRfid11785;

/****************************************************************************/
class CReadRackRfid : public QObject
{
    Q_OBJECT

public:
    CReadRackRfid(CRfid11785 *p_Rfid11785);
    void ReadRackRfid(LoaderRFIDChannel_t Channel);

public slots:
    void OnReportSetChannel(ReturnCode_t ReturnCode);
    void OnReportSendLogin(ReturnCode_t ReturnCode);
    void OnReportReadUid(ReturnCode_t ReturnCode, quint32 Uid);
    void OnReportReadData(ReturnCode_t ReturnCode, quint32 Data);

signals:
    void ReportReadRfid(ReturnCode_t Result, LoaderRFIDChannel_t Channel, quint32 Uid, quint32 Data);
    void ReportError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime);

private:
    friend class CReadRackRfidState;
    void ChangeState(CReadRackRfidState *p_State);
    void ReportReadRfid(ReturnCode_t ReturnCode);

    CReadRackRfidState *mp_State;
    CRfid11785 *mp_Rfid11785;
    LoaderRFIDChannel_t m_Channel;
    quint32 m_Uid;
    quint32 m_Data;
};

} //namespace

#endif /* DEVICECONTROL_READRACKRFID_H */
