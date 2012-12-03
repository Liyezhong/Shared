/****************************************************************************/
/*! \file   DeviceDrawer.h
 *
 *  \brief  Definition file for class CDeviceDrawer
 *
 *  \version  0.1
 *  \date     2012-10-24
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

#ifndef DEVICECONTROL_DRAWER_H
#define DEVICECONTROL_DRAWER_H

#include "DeviceBase.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"

namespace DeviceControl
{

class CDeviceDrawer : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceDrawer(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList, DevInstanceID_t InstanceID);

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*! \brief  Interface to DCP to read Rfid of racks
     *
     *  \iparam Channel Rfid channel of drawer
     */
    /****************************************************************************/
    void ReadRackRfid(LoaderRFIDChannel_t Channel);

    /****************************************************************************/
    /*! \brief  Interface to DCP to block drawer
     */
    /****************************************************************************/
    void Block();

    /****************************************************************************/
    /*! \brief  Interface to DCP to unblock drawer
     */
    /****************************************************************************/
    void Unblock();

    /****************************************************************************/
    /*! \brief  Interface to DCP to open drawer
     */
    /****************************************************************************/
    void Open();

    /****************************************************************************/
    /*! \brief  Interface to DCP to close drawer
     */
    /****************************************************************************/
    void Close();

    // command response interface to DCP
    /****************************************************************************/
    /*! \brief  Response to ReadRackRfid(..) request
     *
     *  \iparam Result  result code sent by FM layer
     *  \iparam Channel Rfid channel of drawer
     *  \iparam Uid     Unique Id of the rack
     *  \iparam Data    Data stored in rack rfid tag
     */
    /****************************************************************************/
    void ReportReadRackRfid(ReturnCode_t ReturnCode, LoaderRFIDChannel_t Channel, quint32 Uid, quint32 Data);

    /****************************************************************************/
    /*! \brief  Response to Block() request
     *
     *  \iparam Result result code sent by FM layer
     */
    /****************************************************************************/
    void ReportBlock(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  Response to Unblock() request
     *
     *  \iparam Result result code sent by FM layer
     */
    /****************************************************************************/
    void ReportUnblock(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  Response to Open() request
     *
     *  \iparam Result result code sent by FM layer
     */
    /****************************************************************************/
    void ReportOpen(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  Response to Close() request
     *
     *  \iparam Result result code sent by FM layer
     */
    /****************************************************************************/
    void ReportClose(ReturnCode_t ReturnCode);

    // Independent signals to DCP
    /****************************************************************************/
    /*! \brief  Independent signal sent to DCP on Open() event
     */
    /****************************************************************************/
    void Opened();

    /****************************************************************************/
    /*! \brief  Independent signal sent to DCP on Close() event
     */
    /****************************************************************************/
    void Closed();

protected:
    bool DefaultNackReadRackRfid(QEvent *p_Event);
    bool DefaultNackBlock(QEvent *p_Event);
    bool DefaultNackUnblock(QEvent *p_Event);

    bool Trans_Configure(QEvent *p_Event);

private:
    CStepperMotor *mp_Motor;
    CRfid11785 *mp_Rfid;
    CDigitalInput *mp_Button;
    CDigitalOutput *mp_LedFree;
    CDigitalOutput *mp_LedBlocked;
};

} //namespace

#endif /* DEVICECONTROL_DRAWER_H */

// vi: set ts=4 sw=4 et:

