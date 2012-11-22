/****************************************************************************/
/*! \file ExhaustDevice.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    08.07.2010
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CExhaustDevice
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

#ifndef EXHAUSTDEVICE_H
#define EXHAUSTDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"

namespace DeviceControl
{

class CDigitalInput;
class CAnalogInput;

/****************************************************************************/
/*! \brief This is the class to control the exhaust
 *         The class provides functionality to notify the application whenever a malfunction regarding the exhaust appears
 *         - flow sensor
 *         - fan current supervision
 *
*/
/****************************************************************************/
class CExhaustDevice : public CBaseDevice
{
    Q_OBJECT

public:
    CExhaustDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    virtual ~CExhaustDevice();

    void HandleTasks();

    ReturnCode_t ReqFlowSensorStatus();
    quint16 GetFlowSensorStatus();

private slots:
    void FlowSensorError(quint32, quint16, quint16, quint16, QDateTime);
    void FlowSensorStatusChanged(quint32 instanceID, ReturnCode_t hdlInfo, quint16 inputValue);
    void FanCurrentError(quint32, quint16, quint16, quint16, QDateTime);
    void FanCurrentChanged(quint32 instanceID, ReturnCode_t hdlInfo, quint16 inputValue);

private:
    ReturnCode_t HandleInitializationState();
    ReturnCode_t HandleConfigurationState();
    void HandleIdleState();
    void HandleErrorState();

    void HandleSensorStatusRequestTask();
    void ResetTask();

    /*! Task id definition */
    typedef enum {
        EXHAUST_DEV_TASKID_INIT              = 0x00,    ///< initialization state, requests are not accepted
        EXHAUST_DEV_TASKID_FREE              = 0x01,    ///< free, a new request will be accepted
        EXHAUST_DEV_TASKID_REQ_SENSOR_STATUS = 0x02,    ///< request the flow sensor status
        EXHAUST_DEV_TASKID_REQ_FAN_CURRENT   = 0x03     ///< request one of the fan current values
    } ExhaustDevTaskID_t;

    /*! Task state definition */
    typedef enum {
        EXHAUST_DEV_TASK_STATE_FREE     = 0x00, ///< task state free, ready for action request
        EXHAUST_DEV_TASK_STATE_REQ      = 0x01, ///< an action was requested, next step will be to forward the command via CAN
        EXHAUST_DEV_TASK_STATE_REQ_SEND = 0x02, ///< command was forwarded via CAN, wait for CAN acknowledge, check timeout
        EXHAUST_DEV_TASK_STATE_ERROR    = 0x03  ///< error, e.g. timeout while waiting for acknowledge, or error CAN-message received
    } ExhaustDevTaskState_t;

    /*! error task states */
    typedef enum {
        EXHAUST_DEV_ERRTASK_STATE_FREE           = 0x00,    ///< task state free, no error
        EXHAUST_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,    ///< reports the error to the interface class
        EXHAUST_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,    ///< reports the error to device processing
        EXHAUST_DEV_ERRTASK_STATE_IDLE           = 0x03,    ///< error idle state, do nothing, block anything
        EXHAUST_DEV_ERRTASK_STATE_RESET          = 0x04     ///< reset error state, change to initial state
    } ExhaustDevErrTaskState_t;

    ExhaustDevTaskID_t       m_taskID;        ///< task requested to the loader
    ExhaustDevTaskState_t    m_taskState;     ///< task state
    ExhaustDevErrTaskState_t m_errTaskState;  ///< the error task state

    CDigitalInput* m_pFlowSensor;                ///< pointer to the flow sensor function module
    CAnalogInput*  m_pCurrentConsumptionFan1;    ///< pointer to the fan 1 current sensor function module
    CAnalogInput*  m_pCurrentConsumptionFan2;    ///< pointer to the fan 2 current sensor function module

    quint16 m_flowSensorStatus;   ///< flow sensor value
    quint16 m_fanCurrent1;        ///< fan1 current value
    quint16 m_fanCurrent2;        ///< fan2 current value
};

}  //namespace

#endif /* EXHAUSTDEVICE_H */
