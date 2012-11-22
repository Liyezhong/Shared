/****************************************************************************/
/*! \file HeatedVesselsDevice.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    08.07.2010
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CHeatedVesselsDevice
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef HEATEDVESSELSDEVICE_H
#define HEATEDVESSELSDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"

namespace DeviceControl
{

class CTemperatureControl;

#define COUNT_HEATED_VESSELS 4  ///< Count of heated vessels

/****************************************************************************/
/*!
 *  \brief  This class provides functions to control the heated vessels
 */
/****************************************************************************/
class CHeatedVesselsDevice : public CBaseDevice
{
    Q_OBJECT

public:
    CHeatedVesselsDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    ~CHeatedVesselsDevice();

    void HandleTasks();

    ReturnCode_t SetStatusTempCtrl(TempCtrlStatus_t statusTempCtrl);
    ReturnCode_t ReqActStatusTempCtrl();
    ReturnCode_t SetTemperature(qint16 vesselTemperatur, HeatedVesselID_t vesselID);
    ReturnCode_t ReqActTemperature(HeatedVesselID_t vesselID);
    qint16 GetActTemperature(HeatedVesselID_t vesselID);

    ReturnCode_t SetOperatingMode(TempCtrlOperatingMode_t operatingMode);
    ReturnCode_t ReqOperatingMode();
    TempCtrlOperatingMode_t GetOperatingMode();

private slots:
    void TempCtrlError(quint32, quint16, quint16, quint16, QDateTime);
    void TempCtrlActTemperature(quint32, ReturnCode_t, quint8 Index, qreal Temperature);
    void TempCtrlActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t);
    void TempCtrlActOperatingMode(quint32, ReturnCode_t, TempCtrlOperatingMode_t);

private:
    ReturnCode_t HandleInitializationState();
    ReturnCode_t HandleConfigurationState();
    void HandleIdleState();
    void HandleErrorState();

    void HandleSetStatusTempCtrlTask();
    void HandleRequestActTempCtrlTask();
    void HandleSetOperatingModeTempCtrlTask();
    void HandleReqActOperatingModeTempCtrlTask();
    void HandleSetTemperatureTask();
    void HandleRequestActTemperatureTask();
    void ResetTask();

    quint8 GetVesselIdxFromID(HeatedVesselID_t vesselID);

    //! Task IDs
    typedef enum {
        HVESSELS_DEV_TASKID_INIT          = 0x00,   ///< initialization state, requests are not accepted
        HVESSELS_DEV_TASKID_FREE          = 0x01,   ///< free, a new request will be accepted
        HVESSELS_DEV_TASKID_STATUS_SET    = 0x02,   ///< set temperature control status
        HVESSELS_DEV_TASKID_OPMODE_SET    = 0x03,   ///< set temperature control operation mode
        HVESSELS_DEV_TASKID_ACTOPMODE_REQ = 0x04,   ///< request actual operating mode
        HVESSELS_DEV_TASKID_TEMP_SET      = 0x05,   ///< set target temperature
        HVESSELS_DEV_TASKID_ACTTEMP_REQ   = 0x06,   ///< request actual temperature
        HVESSELS_DEV_TASKID_ACTSTATUS_REQ = 0x07    ///< request actual temperature control status
    } HeatedVesselsDevTaskID_t;

    //! Task states
    typedef enum {
        HVESSELS_DEV_TASK_STATE_FREE     = 0x00,    ///< task state free, ready for action request
        HVESSELS_DEV_TASK_STATE_REQ      = 0x01,    ///< a task was requested, next step will be to forward the command via CAN
        HVESSELS_DEV_TASK_STATE_REQ_SEND = 0x02,    ///< command was forwarded via CAN, wait for CAN acknowledge, check timeout
        HVESSELS_DEV_TASK_STATE_REQ_ACKN = 0x03,    ///< command was acknowledged
        HVESSELS_DEV_TASK_STATE_ERROR    = 0x04     ///< error, e.g. timeout while waiting for acknowledge
    } HeatedVesselsDevTaskState_t;

    //! Error task states
    typedef enum {
        HVESSELS_DEV_ERRTASK_STATE_FREE           = 0x00,   ///< task state free, no error
        HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,   ///< reports the error to the interface class
        HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,   ///< reports the error to device processing
        HVESSELS_DEV_ERRTASK_STATE_IDLE           = 0x03,   ///< error idle state, do nothing, block anything
        HVESSELS_DEV_ERRTASK_STATE_RESET          = 0x04    ///< reset error state, change to initial state
    } HeatedVesselsDevErrTaskState_t;

    HeatedVesselsDevTaskID_t       m_taskID;        ///< task requested to the loader
    HeatedVesselsDevTaskState_t    m_taskState;     ///< task state
    HeatedVesselsDevErrTaskState_t m_errTaskState;  ///< the error task state

    CTemperatureControl* m_pTempCtrl[COUNT_HEATED_VESSELS];  ///< pointer to temp ctrl. function modules

    HeatedVesselID_t m_reqVesselID; ///< array index of a vessel at request task
    HeatedVesselID_t m_actVesselID; ///< array index of a vessel

    qint16 m_reqVesselTemperature;  ///< requested temperature
    qint16 m_actVesselTemperature[COUNT_HEATED_VESSELS];    ///< actual vessel temperature

    TempCtrlOperatingMode_t m_reqOperatingMode; ///< requested temp. ctrl. operation mode
    TempCtrlOperatingMode_t m_actOperatingMode[COUNT_HEATED_VESSELS];   ///< actual temp. ctrl. operation mode

    TempCtrlStatus_t m_reqStatusTempCtrl;   ///< requested temp. ctrl. status
    TempCtrlStatus_t m_actStatusTempCtrl[COUNT_HEATED_VESSELS]; ///< actual temp. ctrl. status

    quint8 m_vesselState[COUNT_HEATED_VESSELS];     ///< communcication state  (0 - free, 1 - read req. send, 2 - data received, 3 - error)
    qint16 m_vesselErrorCode[COUNT_HEATED_VESSELS]; ///< vessel error code
};

}  //namespace


#endif /* HEATEDVESSELSDEVICE_H */
