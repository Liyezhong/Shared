/****************************************************************************/
/*! \file   DeviceSlideId.h
 *
 *  \brief  Definition file for class CDeviceSlideId
 *
 *  \version  0.1
 *  \date     2012-12-06
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

#ifndef DEVICECONTROL_DEVICESLIDEID_H
#define DEVICECONTROL_DEVICESLIDEID_H

#include "DeviceBase.h"
#include <QSignalMapper>

namespace DeviceControl
{

class CAnalogInput;
class CAnalogOutput;
class CDigitalOutput;

/****************************************************************************/
/*!
 *  \brief  This is the main class of the slide ID device
 */
/****************************************************************************/
class CDeviceSlideId : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceSlideId(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                   DevInstanceID_t InstanceID);

signals:
    // command request interface to DCP
    void StartCounting();
    void StopCounting();

    // command response interface to DCP
    void ReportStartCounting(ReturnCode_t ReturnCode);
    void ReportStopCounting(ReturnCode_t ReturnCode, quint8 Slides);

protected:
    bool Trans_Configure(QEvent *p_Event);

protected slots:
    virtual void ThreadStarted();

private:
    bool EnableSlideCounter(QEvent *p_Event);
    bool NormalOutputValueAckn(QEvent *p_Event);
    bool LastOutputValueAckn(QEvent *p_Event);
    bool OutputValueAckn(QEvent *p_Event, bool Last);
    bool OnActInputValue(QEvent *p_Event);
    bool NackStartCounting(QEvent *p_Event);
    bool NackStopCounting(QEvent *p_Event);
    bool EnableLaser(QEvent *p_Event);
    bool DisableLaser(QEvent *p_Event);
    bool OnEnableLaser(QEvent *p_Event);
    bool OnDisableLaser(QEvent *p_Event);
    bool CountSlide(QEvent *p_Event);

    CBaseModule *mp_BaseModule;         //!< Base module of the slide ID
    CAnalogInput *mp_PhotoDetector;     //!< Signal from the photoelectric detector
    CAnalogOutput *mp_TransmitControl;  //!< Pulse generator for the transmitter
    CAnalogOutput *mp_TransmitCurrent;  //!< Controls the amplitude of the transmitter
    CAnalogOutput *mp_ReceiveCurrent;   //!< Controls the amplitude of the receiver

    quint8 m_SlideCounter;
};

} //namespace

#endif /* DEVICECONTROL_DEVICESLIDEID_H */

// vi: set ts=4 sw=4 et:

