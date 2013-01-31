/****************************************************************************/
/*! \file DeviceExhaust.h
 *
 *  \brief  Definition file for class CDeviceExhaust
 *
 *  \version  0.1
 *  \date     2012-09-26
 *  \author   Torsten Scheck
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
#ifndef DEVICEEXHAUST_H
#define DEVICEEXHAUST_H

#include "DeviceBase.h"

namespace DeviceControl
{

#define EXHAUSTFAN_MIN_CURRENT      100

//#define EXHAUST_FAN_1       1
//#define EXHAUST_FAN_2       2
#define MAX_EXHAUST_FANS	2           ///< Number of Fans

class CAnalogInput;
class CDigitalInput;
class CDigitalOutput;

class CDeviceExhaust : public CDeviceBase
{
   Q_OBJECT

public:
    CDeviceExhaust(const DeviceProcessing &DeviceProc, const DeviceModuleList_t& FctModList, DevInstanceID_t InstanceID);
public slots:
    void SetFanOn(FanID_t FanID);
    void SetFanOff(FanID_t FanID);
signals:
    // Request Interface to DCP
    //void Activate();
   // void Deactivate();
   // void GetFlowStatus();

    // Response interface to DCP
    void ReportFlowStatus(ReturnCode_t HdlInfo, quint16 InputValue);
    void ReportSetFanOn(ReturnCode_t HdlInfo, FanID_t FanID);
    void ReportSetFanOff(ReturnCode_t HdlInfo, FanID_t FanID);

    // Independant Signal
    void FanStopped(ReturnCode_t HdlInfo, FanID_t FanIndex, qint16 CurrentValue);
    void FanStarted(ReturnCode_t HdlInfo, FanID_t FanIndex, qint16 CurrentValue);

protected:
    bool Trans_Configure(QEvent *p_Event);

protected slots:
    virtual void ThreadStarted();
    void SetValveAckn(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue);

    // Slots for each interface request
    //void ActivateSlot();
    //void DeactivateSlot();
   // void GetFlowStatusSlot();

    // Slots to catch FM signal
    void FlowSensor(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue);
    void Current1(quint32 InstanceID, ReturnCode_t HdlInfo, qint16 InputValue);
    void Current2(quint32 InstanceID, ReturnCode_t HdlInfo, qint16 InputValue);

private:
    quint8 GetIndexFromType(FanID_t FanID);
    FanID_t GetFanIDFromIndex(quint8);
    CBaseModule *mp_BaseModule;
    CDigitalInput *mp_FlowSensor;
    CAnalogInput *mp_CurrentConsumptionFan1;
    CAnalogInput *mp_CurrentConsumptionFan2;
    CDigitalOutput *mp_Fan[MAX_EXHAUST_FANS];


    bool m_DeviceStatus;
};

}
#endif // DEVICEEXHAUST_H

