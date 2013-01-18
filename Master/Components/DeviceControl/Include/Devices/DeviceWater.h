/****************************************************************************/
/*! \file DeviceWater.h
 *
 *  \brief  Includes file for Device Water class
 *
 *  \version  0.1
 *  \date     2012-10-25
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
#ifndef DEVICECONTROL_WATER_H
#define DEVICECONTROL_WATER_H

#include "DeviceBase.h"

namespace DeviceControl
{

#define BLOCKAGE_LEVEL		1           ///< To Blockage Levle Input value

#define MAX_WATER_VALVES	6           ///< Number of Water Valves

class CDigitalInput;
class CDigitalOutput;

/****************************************************************************/
/*! \brief Device class to handle Water device tasks
 *
 *  This class signals upper layer (DCP) when Water Valve and Liquidl Status is changed. Also
 *  it emits appropriate signal when DCP requests status. \n
 *  Signals: \n
 *      Request interface to DCP: \n
 *          SetValveOn()\n
 *          SetValveOff()\n
 *
 *      Reponse Interface to DCP: \n
 *          ReportSetValveOn()\n
 *          ReportSetValveOff()\n
 *          ReportLiquidLevel()\n
 *
 *   Independent Signal: \n
 *          WaterBlockage()\n
 *
 */
 /****************************************************************************/
class CDeviceWater : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceWater(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList, DevInstanceID_t InstanceID);

public slots:
        // Request interface to DCP
        /****************************************************************************/
        /*! \brief  To Set the Valve On.
        *
        *          Triggers state change from Off to On
        */
        /****************************************************************************/
        void SetValveOn(WaterValveID_t ValveID);

        /****************************************************************************/
        /*! \brief  To Set the Valve Off.
        *
        *          Triggers state change from On to Off
        */
        /****************************************************************************/
        void SetValveOff(WaterValveID_t ValveID);

        /****************************************************************************/
        /*! \brief  To Request Liquid Level.
        *
        *          Triggers change in Liquid Level
        */
        /****************************************************************************/
        void ReadLiquidLevel();

signals:
    // Response Interface to DCP
    /****************************************************************************/
    /*! \brief  To acknowledge Valve On
     *
     *          Acknowledges SetValveOn(WaterValveID_t) command
	 *
	 *  \iparam     ReturnCode is DCL_ERR_FCT_CALL_SUCCESS if successfully executed
     *  \iparam     ValveIndex Valve ID
     */
    /****************************************************************************/
    void ReportSetValveOn(ReturnCode_t ReturnCode, quint8 ValveIndex);

    /****************************************************************************/
    /*! \brief  To acknowledge Valve Off
     *
     *          Acknowledges SetValveOff(WaterValveID_t) command
	 *
	 *  \iparam     ReturnCode is DCL_ERR_FCT_CALL_SUCCESS if successfully executed
     *  \iparam     ValveIndex Valve ID
     */
    /****************************************************************************/
    void ReportSetValveOff(ReturnCode_t ReturnCode, quint8 ValveIndex);

    /****************************************************************************/
    /*! \brief  To acknowledge Read Liquid Level
     *
     *          Acknowledges ReadLiquidLevel() command
	 *  \iparam     ReturnCode is DCL_ERR_FCT_CALL_SUCCESS if successfully executed
     *  \iparam     WaterLevel Level Indicator
     */
    /****************************************************************************/
    void ReportLiquidLevel(ReturnCode_t ReturnCode, bool WaterLevel);

    // Independent Signal
    /****************************************************************************/
    /*! \brief  To acknowledge Liquid Block
     *
     */
    /****************************************************************************/
    void WaterBlockage();

protected slots:
    void SetValveAckn(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue);
    void LiquidLevelStatus(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue);

protected:
    bool Trans_Configure(QEvent *p_Event);

private:
    WaterValveID_t GetValveIDFromIndex(quint8 valveID);
    quint8 GetValveIndexFromType(WaterValveID_t valveID);

    CBaseModule *mp_BaseModule;
    CDigitalOutput *m_pWaterValve[MAX_WATER_VALVES];    ///< Pointer for Six Water Valves.
    CDigitalInput *m_pLiquidLevel;						///< Pointer for Liquid Level.

    bool m_CommandActive;								///< For Condition Check for Block and Level indicator.
};

} //namespace

#endif // DEVICECONTROL_HOOD_H

// vi: set ts=4 sw=4 et:

