/****************************************************************************/
/*! \file DeviceWater.cpp
 *
 *  \brief  Implementation file for class CDeviceWater
 *
 *  \version  0.1
 *  \date     2012-09-26
 *  \author   Thiru
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
#include <QFinalState>
#include "DeviceControl/Include/Devices/DeviceWater.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceWater
 *
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              resuqest & response signals
 *
 *  \iparam     ModuleList to invoke function module interface functions.
 *
 *  \iparam     InstanceID Device Instance ID.
 */
/****************************************************************************/
CDeviceWater::CDeviceWater(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                           DevInstanceID_t InstanceID) :
    CDeviceBase(DeviceProc, ModuleList, InstanceID), m_CommandActive(false)
{

}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules to the pointers,
 *           m_pWaterValve - Water valve of vessel 1 - 6
 *           m_pLiquidLevel - Digital liquid sensor
 *
 *  \iparam  p_Event Unused.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfully executed, otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
bool CDeviceWater::Trans_Configure(QEvent *p_Event)
{
    Q_UNUSED(p_Event);

    if (m_Thread.isRunning() == false)
    {
        return false;
    }

    // Get function module instances
    m_pWaterValve[0] = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                         GetModuleInstanceFromKey(CANObjectKeyLUT::m_WaterValve1Key)));

    m_pWaterValve[1] = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                         GetModuleInstanceFromKey(CANObjectKeyLUT::m_WaterValve2Key)));

    m_pWaterValve[2] = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                         GetModuleInstanceFromKey(CANObjectKeyLUT::m_WaterValve3Key)));

    m_pWaterValve[3] = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                         GetModuleInstanceFromKey(CANObjectKeyLUT::m_WaterValve4Key)));

    m_pWaterValve[4] = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                         GetModuleInstanceFromKey(CANObjectKeyLUT::m_WaterValve5Key)));

    m_pWaterValve[5] = static_cast<CDigitalOutput *>(m_DeviceProcessing.GetFunctionModule(
                                                         GetModuleInstanceFromKey(CANObjectKeyLUT::m_WaterValve6Key)));

    m_pLiquidLevel = static_cast<CDigitalInput *>(m_DeviceProcessing.GetFunctionModule(
                                                      GetModuleInstanceFromKey(CANObjectKeyLUT::m_WaterLiquidLevelKey)));

    quint8 ValveIndex = 0;
    for (ValveIndex = 0; ValveIndex < MAX_WATER_VALVES; ValveIndex++)
    {
        if (NULL == m_pWaterValve[ValveIndex])
        {
            return false;
        }
    }

    if(NULL == m_pLiquidLevel)
    {
        return false;
    }

    for (ValveIndex = 0; ValveIndex < MAX_WATER_VALVES; ValveIndex++)
    {
        connect(m_pWaterValve[ValveIndex], SIGNAL(ReportOutputValueAckn(quint32,ReturnCode_t,quint16)),
                this, SLOT(SetValveAckn(quint32,ReturnCode_t,quint16)));
    }

    connect(m_pLiquidLevel, SIGNAL(ReportActInputValue(quint32,ReturnCode_t,quint16)),
            this, SLOT(LiquidLevelStatus(quint32,ReturnCode_t,quint16)));

    // No Initialization needed.
    QFinalState *Init_Start = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(Init_Start);

    return true;
}

/****************************************************************************/
/*!
 *  \brief  The sets valve On when, SetValveOn Command Recieved.
 *
 *  \iparam ValveID = Valve ID.
 *
 */
/****************************************************************************/
void CDeviceWater::SetValveOn(WaterValveID_t ValveID)
{
    quint8 ValveIndex;

    ValveIndex = GetValveIndexFromType(ValveID);

    if (ValveIndex < MAX_WATER_VALVES)
    {
        m_pWaterValve[ValveIndex]->SetOutputValue(1);
    }
}

/****************************************************************************/
/*!
 *  \brief  The sets valve Off when, SetValveOff Command Recieved.
 *
 *  \iparam ValveID = Valve ID.
 *
 */
/****************************************************************************/
void CDeviceWater::SetValveOff(WaterValveID_t ValveID)
{
    quint8 ValveIndex;

    ValveIndex = GetValveIndexFromType(ValveID);

    if (ValveIndex < MAX_WATER_VALVES)
    {
        m_pWaterValve[ValveIndex]->SetOutputValue(0);
    }
}

/****************************************************************************/
/*!
 *  \brief  The Reads Liquid Level when, ReadLiquidLevel Command Recieved.
 *
 */
/****************************************************************************/
void CDeviceWater::ReadLiquidLevel()
{
    m_CommandActive = true;           ///< To Report Liquid Level.

    m_pLiquidLevel->ReqActInputValue();
}

/****************************************************************************/
/*!
 *  \brief   Repots Water valve set On or Off
 *
 *  \iparam   InstanceID    Device Instance ID
 *  \iparam   HdlInfo       ReturnCode is DCL_ERR_FCT_CALL_SUCCESS if successfully executed.
 *  \iparam   OutputValue   Indicates Valve On or Off.
 *
 */
/****************************************************************************/
void CDeviceWater::SetValveAckn(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
{
    quint8 ValveIndex;
    WaterValveID_t ValveID;
    quint8 FctModuleIndex = 0;

    for(ValveIndex = 0; ValveIndex < MAX_WATER_VALVES; ValveIndex++)
    {
        if((m_pWaterValve[ValveIndex] != 0) && (m_pWaterValve[ValveIndex]->GetModuleHandle() == InstanceID))
        {
            FctModuleIndex = ValveIndex;
            break;
        }
    }

    ValveID = GetValveIDFromIndex(FctModuleIndex);

    if(OutputValue)
    {
        emit ReportSetValveOn(HdlInfo, ValveID);        ///< Signal to report valve set On.
    }
    else
    {
        emit ReportSetValveOff(HdlInfo, ValveID);       ///< Signal to report valve set off.
    }
}

/****************************************************************************/
/*!
 *  \brief   Repots Lquid Level status
 *
 *  \iparam   InstanceID   Device Instance ID
 *  \iparam   HdlInfo      ReturnCode is DCL_ERR_FCT_CALL_SUCCESS if successfully executed.
 *  \iparam   InputValue   Indicates water block.
 *
 */
/****************************************************************************/
void CDeviceWater::LiquidLevelStatus(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue)
{
    Q_UNUSED(InstanceID);
    if (true == m_CommandActive)
    {
        emit ReportLiquidLevel(HdlInfo, InputValue);    ///< Signal to report Liquid Level.
    }

     if (InputValue == BLOCKAGE_LEVEL)
    {
        emit WaterBlockage();                           ///< Signal to report Water blockage.
    }

    m_CommandActive = false;
}

/****************************************************************************/
/*!
 *  \brief   Convert the valve type to function module array index
 *
 *  \iparam   ValveID = valvue identification
 *
 *  \return  requested array index
 *
 ****************************************************************************/
quint8 CDeviceWater::GetValveIndexFromType(WaterValveID_t ValveID)
{
    quint8 ValveIndex;

    switch(ValveID)
    {
    case (WATER_VALVE_ID_1):
        ValveIndex = 0;
        break;
    case (WATER_VALVE_ID_2):
        ValveIndex = 1;
        break;
    case (WATER_VALVE_ID_3):
        ValveIndex = 2;
        break;
    case (WATER_VALVE_ID_4):
        ValveIndex = 3;
        break;
    case (WATER_VALVE_ID_5):
        ValveIndex = 4;
        break;
    case (WATER_VALVE_ID_6):
        ValveIndex = 5;
        break;
    default:
        ValveIndex = 0;
        break;
    }

    return ValveIndex;

}

/****************************************************************************/
/*!
 *  \brief   Convert the function module array index to type
 *
 *  \iparam   ValveIndex valve index within the array
 *
 *  \return  WaterValveID_t with the requested array index
 *
 ****************************************************************************/
WaterValveID_t CDeviceWater::GetValveIDFromIndex(quint8 ValveIndex)
{
    WaterValveID_t ValveID;

    switch(ValveIndex)
    {
    case (0):
        ValveID = WATER_VALVE_ID_1;
        break;
    case (1):
        ValveID = WATER_VALVE_ID_2;
        break;
    case (2):
        ValveID = WATER_VALVE_ID_3;
        break;
    case (3):
        ValveID = WATER_VALVE_ID_4;
        break;
    case (4):
        ValveID = WATER_VALVE_ID_5;
        break;
    case (5):
        ValveID = WATER_VALVE_ID_6;
        break;
    default:
        ValveID = WATER_VALVE_ID_1;
        break;
    }

    return ValveID;
}


} //namespace

// vi: set ts=4 sw=4 et:
