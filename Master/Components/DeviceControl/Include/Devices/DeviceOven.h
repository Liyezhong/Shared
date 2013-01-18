/****************************************************************************/
/*! \file   DeviceOven.h
 *
 *  \brief  Definition file for class CDeviceOven
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

#ifndef DEVICECONTROL_DEVICEOVEN_H
#define DEVICECONTROL_DEVICEOVEN_H

#include "DeviceControl/Include/Devices/DeviceBase.h"

namespace DeviceControl
{

class CStepperMotor;
class CTemperatureControl;

class CDeviceOven : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceOven(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList, DevInstanceID_t InstanceID);

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*! \brief  Interface to DCP to set temperature of the oven.
     *
     *  \iparam Enable      Turn on temperature control of oven
     *  \iparam Temperature Target temperature of oven
     */
    /****************************************************************************/
    void SetTemperature(bool Enable, qreal Temperature);

    /****************************************************************************/
    /*! \brief  Interface to DCP to get oven temperatur
     */
    /****************************************************************************/
    void GetTemperature();

    /****************************************************************************/
    /*! \brief  Interface to DCP to open oven cover
     */
    /****************************************************************************/
    void CoverOpen();

    /****************************************************************************/
    /*! \brief  Interface to DCP to close oven cover
     */
    /****************************************************************************/
    void CoverClose();

    // command response interface to DCP
    /****************************************************************************/
    /*! \brief  Response to SetTemperature(..) request
     *
     *  \iparam Result result code sent by FM layer
     */
    /****************************************************************************/
    void ReportSetTemperature(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  Response to GetTemperature(..) request
     *
     *  \iparam Result result code sent by FM layer
     *  \iparam The current temperature in °C
     */
    /****************************************************************************/
    void ReportGetTemperature(ReturnCode_t ReturnCode, qreal Temperature);

    /****************************************************************************/
    /*! \brief  Aynchronous message signaling the temperature tolerance state
     *
     *  \iparam Result  result code sent by FM layer
     *  \iparam InRange Indicates if the temperature is in the tolerance range
     */
    /****************************************************************************/
    void ReportRange(ReturnCode_t ReturnCode, bool InRange);

    /****************************************************************************/
    /*! \brief  Response to CoverOpen(..) request
     *
     *  \iparam Result result code sent by FM layer
     */
    /****************************************************************************/
    void ReportCoverOpen(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*! \brief  Response to CoverClose(..) request
     *
     *  \iparam Result result code sent by FM layer
     */
    /****************************************************************************/
    void ReportCoverClose(ReturnCode_t ReturnCode);

protected:
    bool Trans_Configure(QEvent *p_Event);

private:
    // Function Modules
    CBaseModule *mp_BaseModule;
    CStepperMotor *mp_Motor;                //!< Oven cover stepper motor function module
    CTemperatureControl *mp_TempControl;    //!< Temperature control function module
};

} //namespace

#endif /* DEVICECONTROL_DEVICEOVEN_H */

// vi: set ts=4 sw=4 et:

