/****************************************************************************/
/*! \file DeviceRackTransfer.h
 *
 *  \brief  Definition file for class CDeviceRackTransfer
 *
 *  \version  0.1
 *  \date     2012-10-21
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
#ifndef DEVICECONTROL_DEVICERACKTRANSFER_H
#define DEVICECONTROL_DEVICERACKTRANSFER_H

#include "DeviceControl/Include/Devices/DeviceBase.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"

namespace DeviceControl
{

/****************************************************************************/
/*! \brief Device class to handle RackTransfer device tasks
 *
 *  This class signals upper layer (DCP) when RackTransfer Status is changed. Also
 *  it emits appropriate signal when DCP requests status. \n
 *  Signals: \n
 *      Request interface to DCP: \n
 *          Transfer()\n
 *			Load()\n
 *
 *      Reponse Interface to DCP: \n
 *          ReportTransfer(ReturnCode_t ReturnCode)\n
 *          ReportLoad(ReturnCode_t ReturnCode)\n
 *
 */
 /****************************************************************************/
class CDeviceRackTransfer : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceRackTransfer(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                        DevInstanceID_t InstanceID);

signals:
    // command request interface to DCP
	/****************************************************************************/
    /*! \brief Interface to DCP to Transfer Rack.
     */
    /****************************************************************************/
    void Transfer();
	
	/****************************************************************************/
    /*! \brief Interface to DCP to Load Rack.
     */
    /****************************************************************************/
    void Load();

    // command response interface to DCP
	/****************************************************************************/
    /*! \brief Response to Transfer() request.
     *
     *  \iparam ReturnCode code sent by FM layer.
     */
    /****************************************************************************/
    void ReportTransfer(ReturnCode_t ReturnCode);
	
	/****************************************************************************/
    /*! \brief Response to Load() request.
     *
     *  \iparam ReturnCode code sent by FM layer.
     */
    /****************************************************************************/
    void ReportLoad(ReturnCode_t ReturnCode);

protected:
    bool Trans_Configure(QEvent *p_Event);

private:
    CStepperMotor *mp_Motor;			 ///< pointer to StepperMotor

};

} //namespace

#endif // DEVICECONTROL_DEVICERACKTRANSFER_H
