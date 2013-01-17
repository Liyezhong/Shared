/****************************************************************************/
/*! \file   DeviceHeatedCuvettes.h
 *
 *  \brief  Definition file for class CDeviceHeatedCuvettes
 *
 *  \version  0.1
 *  \date     2012-11-20
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

#ifndef DEVICECONTROL_DEVICEHEATEDCUVETTES_H
#define DEVICECONTROL_DEVICEHEATEDCUVETTES_H

#include "DeviceBase.h"
#include <QSignalMapper>

namespace DeviceControl
{

class CTemperatureControl;
class CTemperatureState;

/****************************************************************************/
/*!
 *  \brief  This is the main class of the heated cuvettes device
 */
/****************************************************************************/
class CDeviceHeatedCuvettes : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceHeatedCuvettes(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                          DevInstanceID_t InstanceID);

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Starts and stops temperature control
     *
     *      This method starts or stops the temperature control of a cuvette with
     *      a specified nominal temperature value.
     *
     *  \iparam Cuvette = Number of the heated cuvette
     *  \iparam Enable = Activates or deactivates the temperature control
     *  \iparam Temperature = Temperature of the oven to be set (°C)
     */
    /****************************************************************************/
    void SetTemperature(quint8 Cuvette, bool Enable, qreal Temperature);

    /****************************************************************************/
    /*!
     *  \brief  Reads the current temperature inside a cuvette
     *
     *  \iparam Cuvette = Number of the heated cuvette
     */
    /****************************************************************************/
    void GetTemperature(quint8 Cuvette);

    /****************************************************************************/
    /*!
     *  \brief  Sets the operating mode of the whole device to heat or hold mode
     *
     *  \iparam HoldMode = Hold (true) or heat (false) mode
     */
    /****************************************************************************/
    void SetOperatingMode(bool HoldMode);

    // command response interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Signals the result of the set temperature function
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     *  \iparam Cuvette = Number of the cuvette
     */
    /****************************************************************************/
    void ReportSetTemperature(ReturnCode_t ReturnCode, quint8 Cuvette);

    /****************************************************************************/
    /*!
     *  \brief  Signals the result of the get temperature function
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     *  \iparam Cuvette = Number of the cuvette
     *  \iparam Temperature = Current temperature in °C
     */
    /****************************************************************************/
    void ReportGetTemperature(ReturnCode_t ReturnCode, quint8 Cuvette, qreal Temperature);

    /****************************************************************************/
    /*!
     *  \brief  Signals the result of the set operating mode function
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     */
    /****************************************************************************/
    void ReportSetOperatingMode(ReturnCode_t ReturnCode);

    // asynchronous signals sent to DCP
    /****************************************************************************/
    /*!
     *  \brief  Signals that the temperature entered or left the range
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     *  \iparam Cuvette = Number of the cuvette
     *  \iparam InRange = Temperature is in range (true) or not (false)
     *  \iparam Temperature = Current temperature in °C
     */
    /****************************************************************************/
    void ReportTemperatureRange(ReturnCode_t ReturnCode, quint8 Cuvette, bool InRange, qreal Temperature);

protected:
    bool Trans_Configure(QEvent *p_Event);

protected slots:
    virtual void ThreadStarted();

private slots:
    // These slots are for internal use only
    void SwitchOn(int Cuvette);
    void OnTemperatureRange0(quint32 InstanceID, ReturnCode_t HdlInfo, bool InRange, qreal Temperature);
    void OnTemperatureRange1(quint32 InstanceID, ReturnCode_t HdlInfo, bool InRange, qreal Temperature);
    void OnTemperatureRange2(quint32 InstanceID, ReturnCode_t HdlInfo, bool InRange, qreal Temperature);
    void OnTemperatureRange3(quint32 InstanceID, ReturnCode_t HdlInfo, bool InRange, qreal Temperature);

private:
    bool TransSetTemperature(QEvent *p_Event);
    bool TransFilterSetTemperature(QEvent *p_Event);
    bool TransGetTemperature(QEvent *p_Event);
    bool TransNackOperatingMode(QEvent *p_Event);
    bool TransSetOperatingMode(QEvent *p_Event);
    bool TransSwitchOff(QEvent *p_Event);

    // Report temperature range transitions
    bool TransTemperatureInRange(QEvent *p_Event);
    bool TransTemperatureOutOfRange(QEvent *p_Event);
    bool TransTemperatureRange(QEvent *p_Event, bool InRangeTarget);

    // Report set temperature transitions
    bool TransReportSetTemperature0(QEvent *p_Event);
    bool TransReportSetTemperature1(QEvent *p_Event);
    bool TransReportSetTemperature2(QEvent *p_Event);
    bool TransReportSetTemperature3(QEvent *p_Event);
    bool TransReportSetTemperature(QEvent *p_Event, quint8 Cuvette);

    // Report get temperature transitions
    bool TransReportGetTemperature0(QEvent *p_Event);
    bool TransReportGetTemperature1(QEvent *p_Event);
    bool TransReportGetTemperature2(QEvent *p_Event);
    bool TransReportGetTemperature3(QEvent *p_Event);
    bool TransReportGetTemperature(QEvent *p_Event, quint8 Cuvette);

    // Report set operating mode transitions
    bool TransSwitchToHoldMode(QEvent *p_Event);
    bool TransSwitchToHeatMode(QEvent *p_Event);
    bool TransSwitchOperatingMode(QEvent *p_Event, TempCtrlOperatingMode_t TargetOperatingMode);

    CBaseModule *mp_BaseModule;                 //!< Base module of the heated cuvettes
    CTemperatureControl *mp_TempControl[4];     //!< Temperature control modules of the cuvettes
    CTemperatureState *mp_TemperatureState[4];  //!< Low level state machines for temperature control
    quint8 m_HeatModeHeatCounter;               //!< Number of cuvettes currently heated up
    quint8 m_LastHeatedCuvette;                 //!< The last cuvette that started to heat up
    QSignalMapper m_SignalMapper;               //!< Maps the On signal of the temperature states
};

} //namespace

#endif /* DEVICECONTROL_DEVICEHEATEDCUVETTES_H */

// vi: set ts=4 sw=4 et:

