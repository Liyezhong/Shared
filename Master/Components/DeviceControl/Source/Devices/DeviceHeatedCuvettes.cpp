/****************************************************************************/
/*! \file   DeviceHeatedCuvettes.cpp
 *
 *  \brief  Implementaion file for class CDeviceHeatedCuvettes
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

#include "DeviceControl/Include/Devices/DeviceHeatedCuvettes.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/Devices/TemperatureState.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"

#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CDeviceHeatedCuvettes
typedef CSignalTransition<CDeviceHeatedCuvettes> CHeatedCuvettesTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CDeviceHeatedCuvettes
 *
 *  \iparam DeviceProc = Device processing layer
 *  \iparam ModuleList = List of slave modules used by the device
 *  \iparam InstanceID = Numerical instance ID of the device
 */
/****************************************************************************/
CDeviceHeatedCuvettes::CDeviceHeatedCuvettes(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                                             DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID), m_HeatModeHeatCounter(0), m_LastHeatedCuvette(0)
{

}

/****************************************************************************/
/*!
 *  \brief  Called when the internal thread of the device is started
 *
 *      This function adds the configuration states to the main state machine
 *      of the device. It also starts the state machine.
 */
/****************************************************************************/
void CDeviceHeatedCuvettes::ThreadStarted()
{
    /////////////////////////////////////////////////////////////////
    // Configuring composite states
    /////////////////////////////////////////////////////////////////

    CState *ConfigStart = new CState("ConfigStart", mp_Configuring);
    QFinalState *ConfigEnd = new QFinalState(mp_Configuring);
    mp_Configuring->setInitialState(ConfigStart);

    ConfigStart->addTransition(new CHeatedCuvettesTransition(ConfigStart, SIGNAL(entered()),
                                                             *this, &CDeviceHeatedCuvettes::Trans_Configure, ConfigEnd));

    m_machine.start();
}

/****************************************************************************/
/*!
 *  \brief  Performs the configuration of the device state machine
 *
 *      This function constructs the main state machine of the device.
 *
 *  \iparam p_Event = Signal parameters (unused)
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    // Mapping of heated cuvette stations to the temperature control modules
    // Station 1: m_TempCtrlVessel2Key, Station 2: m_TempCtrlVessel1Key
    // Station 3: m_TempCtrlVessel4Key, Station 4: m_TempCtrlVessel3Key
    mp_BaseModule = m_DeviceProcessing.GetNodeFromID(GetModuleInstanceFromKey(CANObjectKeyLUT::m_BaseHeatedCuvettesKey));
    mp_TempControl[0] = static_cast<CTemperatureControl *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_TempCtrlVessel2Key)));
    mp_TempControl[1] = static_cast<CTemperatureControl *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_TempCtrlVessel1Key)));
    mp_TempControl[2] = static_cast<CTemperatureControl *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_TempCtrlVessel4Key)));
    mp_TempControl[3] = static_cast<CTemperatureControl *>(
                m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(CANObjectKeyLUT::m_TempCtrlVessel3Key)));

    if (mp_BaseModule == NULL || mp_TempControl[0] == NULL || mp_TempControl[1] == NULL ||
            mp_TempControl[2] == NULL || mp_TempControl[3] == NULL)
    {
        return false;
    }

    connect(mp_TempControl[0], SIGNAL(ReportError(quint32, quint32, quint32, quint16, QDateTime)),
            this, SLOT(OnTempControlError(quint32, quint32, quint32, quint16, QDateTime)));
    connect(mp_TempControl[1], SIGNAL(ReportError(quint32, quint32, quint32, quint16, QDateTime)),
            this, SLOT(OnTempControlError(quint32, quint32, quint32, quint16, QDateTime)));
    connect(mp_TempControl[2], SIGNAL(ReportError(quint32, quint32, quint32, quint16, QDateTime)),
            this, SLOT(OnTempControlError(quint32, quint32, quint32, quint16, QDateTime)));
    connect(mp_TempControl[3], SIGNAL(ReportError(quint32, quint32, quint32, quint16, QDateTime)),
            this, SLOT(OnTempControlError(quint32, quint32, quint32, quint16, QDateTime)));

    connect(mp_TempControl[0], SIGNAL(ReportTemperatureRange(quint32, ReturnCode_t, bool, qreal)),
            this, SLOT(OnTemperatureRange0(quint32, ReturnCode_t, bool, qreal)));
    connect(mp_TempControl[1], SIGNAL(ReportTemperatureRange(quint32, ReturnCode_t, bool, qreal)),
            this, SLOT(OnTemperatureRange1(quint32, ReturnCode_t, bool, qreal)));
    connect(mp_TempControl[2], SIGNAL(ReportTemperatureRange(quint32, ReturnCode_t, bool, qreal)),
            this, SLOT(OnTemperatureRange2(quint32, ReturnCode_t, bool, qreal)));
    connect(mp_TempControl[3], SIGNAL(ReportTemperatureRange(quint32, ReturnCode_t, bool, qreal)),
            this, SLOT(OnTemperatureRange3(quint32, ReturnCode_t, bool, qreal)));

    /////////////////////////////////////////////////////////////////
    // Initializing
    /////////////////////////////////////////////////////////////////
    //lint -esym(429, Init_Start)
    QFinalState *Init_Start = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(Init_Start);

    /////////////////////////////////////////////////////////////////
    // Working composite states
    /////////////////////////////////////////////////////////////////
    mp_Working->setChildMode(QState::ParallelStates);

    // State machines of the temperature control function modules
    for (qint32 i = 0; i < 4; i++) {
        mp_TemperatureState[i] = new CTemperatureState(
                    mp_TempControl[i], QString("TemperatureState%1").arg(i), mp_Working);
        connect(mp_TemperatureState[i], SIGNAL(On()), &m_SignalMapper, SLOT(map()));
        m_SignalMapper.setMapping(mp_TemperatureState[i], i);
    }
    connect(&m_SignalMapper, SIGNAL(mapped(int)), this, SLOT(SwitchOn(int)));

    // Report set temperature transitions
    mp_Working->addTransition(new CHeatedCuvettesTransition(
        mp_TemperatureState[0], SIGNAL(ReportSetState(ReturnCode_t)),
        *this, &CDeviceHeatedCuvettes::TransReportSetTemperature0));

    mp_Working->addTransition(new CHeatedCuvettesTransition(
        mp_TemperatureState[1], SIGNAL(ReportSetState(ReturnCode_t)),
        *this, &CDeviceHeatedCuvettes::TransReportSetTemperature1));

    mp_Working->addTransition(new CHeatedCuvettesTransition(
        mp_TemperatureState[2], SIGNAL(ReportSetState(ReturnCode_t)),
        *this, &CDeviceHeatedCuvettes::TransReportSetTemperature2));

    mp_Working->addTransition(new CHeatedCuvettesTransition(
        mp_TemperatureState[3], SIGNAL(ReportSetState(ReturnCode_t)),
        *this, &CDeviceHeatedCuvettes::TransReportSetTemperature3));

    // Report get temperature transitions
    mp_Working->addTransition(new CHeatedCuvettesTransition(
        mp_TemperatureState[0], SIGNAL(ReportGetTemperature(ReturnCode_t, qreal)),
        *this, &CDeviceHeatedCuvettes::TransReportGetTemperature0));

    mp_Working->addTransition(new CHeatedCuvettesTransition(
        mp_TemperatureState[1], SIGNAL(ReportGetTemperature(ReturnCode_t, qreal)),
        *this, &CDeviceHeatedCuvettes::TransReportGetTemperature1));

    mp_Working->addTransition(new CHeatedCuvettesTransition(
        mp_TemperatureState[2], SIGNAL(ReportGetTemperature(ReturnCode_t, qreal)),
        *this, &CDeviceHeatedCuvettes::TransReportGetTemperature2));

    mp_Working->addTransition(new CHeatedCuvettesTransition(
        mp_TemperatureState[3], SIGNAL(ReportGetTemperature(ReturnCode_t, qreal)),
        *this, &CDeviceHeatedCuvettes::TransReportGetTemperature3));

    // Global heated cuvettes state machine
    CState *p_Control = new CState("Control", mp_Working);
    CState *p_HeatModeHold = new CState("HeatModeHold", p_Control);
    CState *p_HeatModeHeat = new CState("HeatModeHeat", p_Control);
    CState *p_HoldModeHold = new CState("HoldModeHold", p_Control);
    CState *p_HoldModeHeat = new CState("HoldModeHeat", p_Control);
    p_Control->setInitialState(p_HeatModeHold);

    // Set temperature transition
    p_Control->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(SetTemperature(quint8, bool, qreal)),
        *this, &CDeviceHeatedCuvettes::TransSetTemperature));

    // Get temperature transition
    p_Control->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(GetTemperature(quint8)),
        *this, &CDeviceHeatedCuvettes::TransGetTemperature));

    // Set operating mode default nack transition
    p_Control->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(SetOperatingMode(bool)),
        *this, &CDeviceHeatedCuvettes::TransNackOperatingMode));

    // Heat mode and no cuvette is heating up
    for (qint32 i = 0; i < 4; i++) {
        p_HeatModeHold->addTransition(mp_TemperatureState[i], SIGNAL(On()), p_HeatModeHeat);
    }

    p_HeatModeHold->addTransition(new CHeatedCuvettesTransition(
        mp_TempControl[0], SIGNAL(ReportSetOperatingModeAckn(quint32, ReturnCode_t, TempCtrlOperatingMode_t)),
        *this, &CDeviceHeatedCuvettes::TransSwitchToHoldMode,
        p_HoldModeHold));

    p_HeatModeHold->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(SetOperatingMode(bool)),
        *this, &CDeviceHeatedCuvettes::TransSetOperatingMode));

    p_HeatModeHold->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(ReportTemperatureRange(ReturnCode_t, quint8, bool, qreal)),
        *this, &CDeviceHeatedCuvettes::TransTemperatureOutOfRange,
        p_HeatModeHeat));

    // Heat mode and one or more cuvettes is heating up
    for (qint32 i = 0; i < 4; i++) {
        p_HeatModeHeat->addTransition(new CHeatedCuvettesTransition(
            mp_TemperatureState[i], SIGNAL(Off()),
            *this, &CDeviceHeatedCuvettes::TransSwitchOff,
            p_HeatModeHold));
    }

    p_HeatModeHeat->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(ReportTemperatureRange(ReturnCode_t, quint8, bool, qreal)),
        *this, &CDeviceHeatedCuvettes::TransTemperatureInRange,
        p_HeatModeHold));

    // Hold mode and no cuvette is heating up
    for (qint32 i = 0; i < 4; i++) {
        p_HoldModeHold->addTransition(mp_TemperatureState[i], SIGNAL(On()), p_HoldModeHeat);
    }

    p_HoldModeHold->addTransition(new CHeatedCuvettesTransition(
        mp_TempControl[0], SIGNAL(ReportSetOperatingModeAckn(quint32, ReturnCode_t, TempCtrlOperatingMode_t)),
        *this, &CDeviceHeatedCuvettes::TransSwitchToHeatMode,
        p_HeatModeHold));

    p_HoldModeHold->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(SetOperatingMode(bool)),
        *this, &CDeviceHeatedCuvettes::TransSetOperatingMode));

    p_HoldModeHold->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(ReportTemperatureRange(ReturnCode_t, quint8, bool, qreal)),
        *this, &CDeviceHeatedCuvettes::TransTemperatureOutOfRange,
        p_HoldModeHeat));

    // Hold mode and a single cuvette is heating up
    p_HoldModeHeat->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(SetTemperature(quint8, bool, qreal)),
        *this, &CDeviceHeatedCuvettes::TransFilterSetTemperature));

    for (qint32 i = 0; i < 4; i++) {
        p_HoldModeHeat->addTransition(new CHeatedCuvettesTransition(
            mp_TemperatureState[i], SIGNAL(Off()),
            *this, &CDeviceHeatedCuvettes::TransSwitchOff,
            p_HoldModeHold));
    }

    p_HoldModeHeat->addTransition(new CHeatedCuvettesTransition(
        this, SIGNAL(ReportTemperatureRange(ReturnCode_t, quint8, bool, qreal)),
        *this, &CDeviceHeatedCuvettes::TransTemperatureInRange,
        p_HoldModeHold));

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Called when a heated cuvette is switched on
 *
 *  \iparam Cuvette = Number of the heated cuvette
 */
/****************************************************************************/
void CDeviceHeatedCuvettes::SwitchOn(int Cuvette)
{
    m_HeatModeHeatCounter++;
    m_LastHeatedCuvette = Cuvette;
}

/****************************************************************************/
/*!
 *  \brief  Called when the set temperature interface signal is emitted
 *
 *  \iparam p_Event = Parameters of the signal SetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransSetTemperature(QEvent *p_Event)
{
    quint8 Cuvette;
    bool Enable;
    qreal Temperature;

    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 0, Cuvette)) {
        emit ReportSetTemperature(DCL_ERR_INVALID_PARAM, Cuvette);
        return false;
    }
    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 1, Enable)) {
        emit ReportSetTemperature(DCL_ERR_INVALID_PARAM, Cuvette);
        return false;
    }
    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 2, Temperature)) {
        emit ReportSetTemperature(DCL_ERR_INVALID_PARAM, Cuvette);
        return false;
    }

    if (Cuvette >= 4) {
        emit ReportSetTemperature(DCL_ERR_INVALID_PARAM, Cuvette);
        return false;
    }

    mp_TemperatureState[Cuvette]->EmitSetState(Enable, Temperature);
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when the set temperature interface signal is emitted
 *
 *      This function is called when the set temperature signal is received
 *      and the device is in hold mode and another cuvette is heated up.
 *
 *  \iparam p_Event = Parameters of the signal SetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransFilterSetTemperature(QEvent *p_Event)
{
    quint8 Cuvette;
    bool Enable;

    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 0, Cuvette)) {
        emit ReportSetTemperature(DCL_ERR_INVALID_PARAM, Cuvette);
        return true;
    }
    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 1, Enable)) {
        emit ReportSetTemperature(DCL_ERR_INVALID_PARAM, Cuvette);
        return true;
    }

    if (true == Enable) {
        emit ReportSetTemperature(DCL_ERR_INVALID_STATE, Cuvette);
        return true;
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when the get temperature interface signal is emitted
 *
 *  \iparam p_Event = Parameters of the signal GetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransGetTemperature(QEvent *p_Event)
{
    quint8 Cuvette;

    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 0, Cuvette)) {
        emit ReportGetTemperature(DCL_ERR_INVALID_PARAM, Cuvette, 0);
        return false;
    }

    if (Cuvette >= 4) {
        emit ReportGetTemperature(DCL_ERR_INVALID_PARAM, Cuvette, 0);
        return false;
    }

    mp_TemperatureState[Cuvette]->EmitGetTemperature();
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when a cuvette is heating up
 *
 *      When a cuvette is heated up a change of the operating mode is not
 *      allowed.
 *
 *  \iparam p_Event = Parameters of the signal SetOperatingMode (unused)
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransNackOperatingMode(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    emit ReportSetOperatingMode(DCL_ERR_INVALID_STATE);
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when the set operating mode signal is emitted
 *
 *  \iparam p_Event = Parameters of the signal SetOperatingMode
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransSetOperatingMode(QEvent *p_Event)
{
    bool HoldMode;

    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 0, HoldMode)) {
        emit ReportSetOperatingMode(DCL_ERR_INVALID_PARAM);
        return true;
    }

    TempCtrlOperatingMode_t OperatingMode = HoldMode ? TEMPCTRL_OPMODE_HOLD : TEMPCTRL_OPMODE_FULL;
    mp_TempControl[0]->SetOperatingMode(OperatingMode);
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Called when the a heated cuvette is switched off
 *
 *  \iparam p_Event = Parameters of the signal Off (empty)
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransSwitchOff(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    if(--m_HeatModeHeatCounter == 0) {
        return true;
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief  Used when an in range signal is expected
 *
 *  \iparam p_Event = Parameters of the signal ReportTemperatureRange
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransTemperatureInRange(QEvent *p_Event)
{
    return TransTemperatureRange(p_Event, true);
}

/****************************************************************************/
/*!
 *  \brief  Used when an in range signal is expected
 *
 *  \iparam p_Event = Parameters of the signal ReportTemperatureRange
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransTemperatureOutOfRange(QEvent *p_Event)
{
    return TransTemperatureRange(p_Event, false);
}

/****************************************************************************/
/*!
 *  \brief  Evaluates if the desired range is entered
 *
 *  \iparam p_Event = Parameters of the signal ReportTemperatureRange
 *  \iparam InRangeTarget = Expected temperature range flag
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransTemperatureRange(QEvent *p_Event, bool InRangeTarget)
{
    ReturnCode_t ReturnCode;
    quint8 Cuvette;
    bool InRange;

    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 0, ReturnCode)) {
        return false;
    }
    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 1, Cuvette)) {
        return false;
    }
    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 2, InRange)) {
        return false;
    }

    if (InRangeTarget == InRange) {
        if (InRange) {
            m_HeatModeHeatCounter--;
        }
        else {
            m_HeatModeHeatCounter++;
        }
        return true;
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set temperature signal is emitted
 *
 *  \iparam p_Event = Parameters of the signal ReportSetTemperature
 *  \iparam Cuvette = Number of the cuvette
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportSetTemperature(QEvent *p_Event, quint8 Cuvette)
{
    ReturnCode_t ReturnCode;

    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 0, ReturnCode)) {
        ReturnCode = DCL_ERR_INVALID_PARAM;
    }

    emit ReportSetTemperature(ReturnCode, Cuvette);
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set temperature signal is emitted
 *
 *      This function is called, when cuvette 0 emits the signal.
 *
 *  \iparam p_Event = Parameters of the signal ReportSetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportSetTemperature0(QEvent *p_Event)
{
    return TransReportSetTemperature(p_Event, 0);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set temperature signal is emitted
 *
 *      This function is called, when cuvette 1 emits the signal.
 *
 *  \iparam p_Event = Parameters of the signal ReportSetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportSetTemperature1(QEvent *p_Event)
{
    return TransReportSetTemperature(p_Event, 1);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set temperature signal is emitted
 *
 *      This function is called, when cuvette 2 emits the signal.
 *
 *  \iparam p_Event = Parameters of the signal ReportSetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportSetTemperature2(QEvent *p_Event)
{
    return TransReportSetTemperature(p_Event, 2);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set temperature signal is emitted
 *
 *      This function is called, when cuvette 3 emits the signal.
 *
 *  \iparam p_Event = Parameters of the signal ReportSetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportSetTemperature3(QEvent *p_Event)
{
    return TransReportSetTemperature(p_Event, 3);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report get temperature signal is emitted
 *
 *  \iparam p_Event = Parameters of the signal ReportGetTemperature
 *  \iparam Cuvette = Number of the cuvette
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportGetTemperature(QEvent *p_Event, quint8 Cuvette)
{
    ReturnCode_t ReturnCode;
    qreal Temperature;

    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 0, ReturnCode)) {
        emit ReportGetTemperature(DCL_ERR_INVALID_PARAM, Cuvette, 0);
        return false;
    }
    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 1, Temperature)) {
        emit ReportGetTemperature(DCL_ERR_INVALID_PARAM, Cuvette, 0);
        return false;
    }

    emit ReportGetTemperature(ReturnCode, Cuvette, Temperature);
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Called when the report get temperature signal is emitted
 *
 *      This function is called, when cuvette 0 emits the signal.
 *
 *  \iparam p_Event = Parameters of the signal ReportGetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportGetTemperature0(QEvent *p_Event)
{
    return TransReportGetTemperature(p_Event, 0);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report get temperature signal is emitted
 *
 *      This function is called, when cuvette 1 emits the signal.
 *
 *  \iparam p_Event = Parameters of the signal ReportGetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportGetTemperature1(QEvent *p_Event)
{
    return TransReportGetTemperature(p_Event, 1);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report get temperature signal is emitted
 *
 *      This function is called, when cuvette 2 emits the signal.
 *
 *  \iparam p_Event = Parameters of the signal ReportGetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportGetTemperature2(QEvent *p_Event)
{
    return TransReportGetTemperature(p_Event, 2);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report get temperature signal is emitted
 *
 *      This function is called, when cuvette 3 emits the signal.
 *
 *  \iparam p_Event = Parameters of the signal ReportGetTemperature
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransReportGetTemperature3(QEvent *p_Event)
{
    return TransReportGetTemperature(p_Event, 3);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set operating mode signal is emitted
 *
 *      This function is called, when a switch to hold mode is expected.
 *
 *  \iparam p_Event = Parameters of the signal ReportSetOperatingModeAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransSwitchToHoldMode(QEvent *p_Event)
{
    return TransSwitchOperatingMode(p_Event, TEMPCTRL_OPMODE_HOLD);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set operating mode signal is emitted
 *
 *      This function is called, when a switch to heat mode is expected.
 *
 *  \iparam p_Event = Parameters of the signal ReportSetOperatingModeAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransSwitchToHeatMode(QEvent *p_Event)
{
    return TransSwitchOperatingMode(p_Event, TEMPCTRL_OPMODE_FULL);
}

/****************************************************************************/
/*!
 *  \brief  Called when the report set operating mode signal is emitted
 *
 *  \iparam p_Event = Parameters of the signal ReportSetOperatingModeAckn
 *  \iparam TargetOperatingMode = Intended operating mode
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CDeviceHeatedCuvettes::TransSwitchOperatingMode(QEvent *p_Event, TempCtrlOperatingMode_t TargetOperatingMode)
{
    ReturnCode_t ReturnCode;
    TempCtrlOperatingMode_t OperatingMode;

    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 1, ReturnCode)) {
        emit ReportSetOperatingMode(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CHeatedCuvettesTransition::GetEventValue(p_Event, 2, OperatingMode)) {
        emit ReportSetOperatingMode(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportSetOperatingMode(ReturnCode);
        return false;
    }
    if (OperatingMode != TargetOperatingMode) {
        emit ReportSetOperatingMode(ReturnCode);
        return false;
    }

    emit ReportSetOperatingMode(ReturnCode);
    return true;
}

/****************************************************************************/
/*!
 *  \brief  This slot is called when an error was detected or received
 *
 *  \iparam InstanceID = Instance identifier of device
 *  \iparam ErrorGroup = Error group
 *  \iparam ErrorCode = Error code
 *  \iparam ErrorData = Additional error information
 *  \iparam ErrorTime = Error time
 */
/****************************************************************************/
void CDeviceHeatedCuvettes::OnTempControlError(quint32 InstanceID, quint32 ErrorGroup, quint32 ErrorCode,
                                               quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID)
    emit ReportError(m_InstanceID, ErrorGroup, ErrorCode, ErrorData, ErrorTime);
}

/****************************************************************************/
/*!
 *  \brief  Connects the ReportTemperatureRange signal to the interface
 *
 *      This function is used for cuvette 0.
 *
 *  \iparam InstanceID = Instance identifier of the function module
 *  \iparam HdlInfo = Return code of the signal
 *  \iparam InRange = Indicates if the temperature is in range or not
 *  \iparam Temperature = Current temperature in °C
 */
/****************************************************************************/
void CDeviceHeatedCuvettes::OnTemperatureRange0(quint32 InstanceID, ReturnCode_t HdlInfo,
                                                bool InRange, qreal Temperature)
{
    Q_UNUSED(InstanceID)
    emit ReportTemperatureRange(HdlInfo, 0, InRange, Temperature);
}

/****************************************************************************/
/*!
 *  \brief  Connects the ReportTemperatureRange signal to the interface
 *
 *      This function is used for cuvette 1.
 *
 *  \iparam InstanceID = Instance identifier of the function module
 *  \iparam HdlInfo = Return code of the signal
 *  \iparam InRange = Indicates if the temperature is in range or not
 *  \iparam Temperature = Current temperature in °C
 */
/****************************************************************************/
void CDeviceHeatedCuvettes::OnTemperatureRange1(quint32 InstanceID, ReturnCode_t HdlInfo,
                                                bool InRange, qreal Temperature)
{
    Q_UNUSED(InstanceID)
    emit ReportTemperatureRange(HdlInfo, 1, InRange, Temperature);
}

/****************************************************************************/
/*!
 *  \brief  Connects the ReportTemperatureRange signal to the interface
 *
 *      This function is used for cuvette 2.
 *
 *  \iparam InstanceID = Instance identifier of the function module
 *  \iparam HdlInfo = Return code of the signal
 *  \iparam InRange = Indicates if the temperature is in range or not
 *  \iparam Temperature = Current temperature in °C
 */
/****************************************************************************/
void CDeviceHeatedCuvettes::OnTemperatureRange2(quint32 InstanceID, ReturnCode_t HdlInfo,
                                                bool InRange, qreal Temperature)
{
    Q_UNUSED(InstanceID)
    emit ReportTemperatureRange(HdlInfo, 2, InRange, Temperature);
}

/****************************************************************************/
/*!
 *  \brief  Connects the ReportTemperatureRange signal to the interface
 *
 *      This function is used for cuvette 3.
 *
 *  \iparam InstanceID = Instance identifier of the function module
 *  \iparam HdlInfo = Return code of the signal
 *  \iparam InRange = Indicates if the temperature is in range or not
 *  \iparam Temperature = Current temperature in °C
 */
/****************************************************************************/
void CDeviceHeatedCuvettes::OnTemperatureRange3(quint32 InstanceID, ReturnCode_t HdlInfo,
                                                bool InRange, qreal Temperature)
{
    Q_UNUSED(InstanceID)
    emit ReportTemperatureRange(HdlInfo, 3, InRange, Temperature);
}

} //namespace
