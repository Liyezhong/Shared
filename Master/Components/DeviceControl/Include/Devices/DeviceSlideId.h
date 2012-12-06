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

protected:
    bool Trans_Configure(QEvent *p_Event);

protected slots:
    virtual void ThreadStarted();

private slots:

private:
    CBaseModule *mp_BaseModule;         //!< Base module of the slide ID
    CAnalogInput *mp_PhotoDetector;     //!< Signal from the photoelectric detector
    CAnalogOutput *mp_TransmitControl;  //!< Pulse generator for the transmitter
    CAnalogOutput *mp_TransmitCurrent;  //!< Controls the amplitude of the transmitter
    CAnalogOutput *mp_ReceiveCurrent;   //!< Controls the amplitude of the receiver
};

} //namespace

#endif /* DEVICECONTROL_DEVICESLIDEID_H */

// vi: set ts=4 sw=4 et:

