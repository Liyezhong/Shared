/****************************************************************************/
/*! \file   TemperatureState.h
 *
 *  \brief  Definition file for class CTemperatureState
 *
 *  \version  0.1
 *  \date     2012-11-21
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

#ifndef DEVICECONTROL_TEMPERATURESTATE_H
#define DEVICECONTROL_TEMPERATURESTATE_H

#include "DeviceControl/Include/Devices/DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControl.h"

namespace DeviceControl
{

class CTemperatureControl;

/****************************************************************************/
/*!
 *  \brief  State machine for common features of temperature control
 */
/****************************************************************************/
class CTemperatureState : public CState
{
    Q_OBJECT

public:
    explicit CTemperatureState(CTemperatureControl *p_TemperatureControl, const QString &Name, QState *p_Parent = 0);
    void EmitSetState(bool Enable, qreal Temperature);
    void EmitGetTemperature();

signals:
    // Request interface
    /****************************************************************************/
    /*!
     *  \brief  Starts and stops temperature control
     *
     *      This method starts or stops the temperature control with a specified
     *      nominal temperature value.
     *
     *  \iparam Enable = Activates or deactivates the temperature control
     *  \iparam Temperature = Temperature of the oven to be set (°C)
     */
    /****************************************************************************/
    void SetState(bool Enable, qreal Temperature);

    /****************************************************************************/
    /*!
     *  \brief  Reads the current temperature
     */
    /****************************************************************************/
    void GetTemperature();

    // Response interface
    /****************************************************************************/
    /*!
     *  \brief  Signals the result of the set state function
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     */
    /****************************************************************************/
    void ReportSetState(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*!
     *  \brief  Signals the result of the get temperature function
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     *  \iparam Temperature = Current temperature in °C
     */
    /****************************************************************************/
    void ReportGetTemperature(ReturnCode_t ReturnCode, qreal Temperature);

    // Helper signals
    /****************************************************************************/
    /*!
     *  \brief  Signals that temperature control is switched on
     */
    /****************************************************************************/
    void On();

    /****************************************************************************/
    /*!
     *  \brief  Signals that temperature control is switched off
     */
    /****************************************************************************/
    void Off();

private slots:
    void OnGetTemperature(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, qreal Temperature);

private:
    bool TransSetState(QEvent *p_Event);
    bool TransOffOn(QEvent *p_Event);
    bool TransOnOff(QEvent *p_Event);
    bool TransSwitch(QEvent *p_Event, TempCtrlStatus_t TargetStatus);
    bool TransGetTemperature(QEvent *p_Event);
    bool TransGetTemperatureError(QEvent *p_Event);

    CTemperatureControl *mp_TemperatureControl; //!< Temperature control function module
};

} //namespace

#endif // DEVICECONTROL_TEMPERATURESTATE_H

// vi: set ts=4 sw=4 et:

