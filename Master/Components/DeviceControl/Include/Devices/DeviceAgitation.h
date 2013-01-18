/****************************************************************************/
/*! \file DeviceAgitation.h
 *
 *  \brief  Definition file for class CDeviceAgitation
 *
 *  \version  0.1
 *  \date     2012-09-24
 *  \author   Thiru
 *
 *  \b Description:
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
#ifndef DEVICECONTROL_DEVICEAGITATION_H
#define DEVICECONTROL_DEVICEAGITATION_H

#include "DeviceControl/Include/Devices/DeviceBase.h"

namespace DeviceControl
{

class CStepperMotor;

/****************************************************************************/
/*! \brief Device class to handle Agitation device tasks
 *
 *  This class signals upper layer (DCP) when Agitation Status is changed. Also
 *  it emits appropriate signal when DCP requests status. \n
 *  Signals: \n
 *      Request interface to DCP: \n
 *          IsClosed()\n
 *
 *      Reponse Interface to DCP: \n
 *          ReportIsClosed()\n
 *          Opened()\n
 *          Closed()\n
 *
 */
 /****************************************************************************/
class CDeviceAgitation: public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceAgitation(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                     DevInstanceID_t InstanceID);

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*! \brief Interface to DCP to start Agitation
     */
    /****************************************************************************/
    void Move(quint32, quint8);
    void Rotate(quint32 TargetSpeed, quint8 MotionProfileIdx = 0);

    // command response interface to DCP
	/****************************************************************************/
    /*! \brief Response to Rotate request.
     *
     */
    /****************************************************************************/
    void RotateAck();

protected:
    bool Trans_Configure(QEvent *p_Event);

private:
    CBaseModule *mp_BaseModule;
    CStepperMotor *mp_Motor;

};

} //namespace

#endif /* DEVICECONTROL_DEVICEAGITATION_H */
