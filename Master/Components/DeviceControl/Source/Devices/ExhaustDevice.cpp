/******************************************************************/
/*! \file ExhaustDevice.cpp
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 08.07.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CExhaustDevice
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
/******************************************************************/

#include "DeviceControl/Include/Devices/ExhaustDevice.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

#define EXHAUSTDEVICE_SENSOR_REQUEST_TIMEOUT 5000  //!< timeout for sensor acknowledge

/****************************************************************************/
/*!
 *  \brief    Constructor of CExhaustDevice
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *
 *
 ***************************************************************************/
CExhaustDevice::CExhaustDevice(DeviceProcessing* pDeviceProcessing, QString Type)
    : CBaseDevice(pDeviceProcessing, Type),
    m_flowSensorStatus(0), m_fanCurrent1(0), m_fanCurrent2(0)
{
    m_pFlowSensor = 0;
    m_pCurrentConsumptionFan1 = 0;
    m_pCurrentConsumptionFan2 = 0;

    m_MainState = DEVICE_MAIN_STATE_START;
    m_MainStateOld = m_MainState;
    m_taskID = EXHAUST_DEV_TASKID_INIT;
    m_taskState = EXHAUST_DEV_TASK_STATE_FREE;
    m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_EXHAUST;

    FILE_LOG_L(laDEV, llINFO) << " create exhaust device";

}

/****************************************************************************/
/*!
 *  \brief    Destructor of CExhaustDevice
 *
 *
 *  \return
 *
 ****************************************************************************/
CExhaustDevice::~CExhaustDevice()
{
    m_pDevProc = 0;
    m_pFlowSensor = 0;
    m_pCurrentConsumptionFan1 = 0;
    m_pCurrentConsumptionFan2 = 0;
}

/****************************************************************************/
/*!
 *  \brief    Handles the internal state machine
 *
 *   This function should be called periodically, it handles the state machine of
 *   the class. Furthermore, the HandleTask-function of the interface class will be called.
 *   Each main state has a individual handling function, which will be called according to the
 *   currently active main state
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CExhaustDevice::HandleTasks()
{
    ReturnCode_t RetVal;

    if(m_MainState == DEVICE_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_MainState == DEVICE_MAIN_STATE_START)
    {
        m_MainState = DEVICE_MAIN_STATE_INIT;
    }
    else if(m_MainState == DEVICE_MAIN_STATE_INIT)
    {
        RetVal = HandleInitializationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_CONFIG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   CExhaustDevice:HandleInitializationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(DEVICE_MAIN_STATE_CONFIG == m_MainState)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
            m_taskID = EXHAUST_DEV_TASKID_FREE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   CExhaustDevice:HandleConfigurationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CExhaustDevice:handleTasks, new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialisation state.
 *
 *           This function attaches the function modules to the pointers, according to the loaderID,
 *           which differs loader and unlaoder
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CExhaustDevice::HandleInitializationState()
{
    FILE_LOG_L(laDEV, llINFO) << "  CExhaustDevice::HandleInitializationState()";

    m_pFlowSensor = (CDigitalInput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_FlowSensorKey));
    if(m_pFlowSensor == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DIN_EXHAUST_FLOW);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_DIN_EXHAUST_FLOW not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    m_pCurrentConsumptionFan1 = (CAnalogInput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_CurrentFan1Key));
    if(m_pCurrentConsumptionFan1 == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AIN_FAN1_CURRENT);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AIN_FAN1_CURRENT not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    m_pCurrentConsumptionFan2 = (CAnalogInput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_CurrentFan2Key));
    if(m_pCurrentConsumptionFan2 == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AIN_FAN2_CURRENT);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AIN_FAN2_CURRENT not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes configuration state.
 *
 *           This function connects each function module's signals to the internal slots.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if configuration was successfully executed
 *           otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
ReturnCode_t CExhaustDevice::HandleConfigurationState()
{    
    if(!connect(m_pFlowSensor, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(FlowSensorError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DIN_EXHAUST_FLOW);
        FILE_LOG_L(laDEV, llERROR) << "   Connect flow sensor signal 'ReportError' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pFlowSensor, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(FlowSensorStatusChanged(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DIN_EXHAUST_FLOW);
        FILE_LOG_L(laDEV, llERROR) << "   Connect flow sensor signal 'ReportActInputValue' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pCurrentConsumptionFan1, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(FanCurrentError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AIN_FAN1_CURRENT);
        FILE_LOG_L(laDEV, llERROR) << "   Connect current fan 1 signal 'ReportError' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pCurrentConsumptionFan1, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(FanCurrentChanged(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AIN_FAN1_CURRENT);
        FILE_LOG_L(laDEV, llERROR) << "   Connect current fan 1 signal 'ReportActInputValue' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pCurrentConsumptionFan2, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(FanCurrentError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AIN_FAN2_CURRENT);
        FILE_LOG_L(laDEV, llERROR) << "   Connect current fan 2 signal 'ReportActInputValue' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pCurrentConsumptionFan2, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(FanCurrentChanged(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AIN_FAN2_CURRENT);
        FILE_LOG_L(laDEV, llERROR) << "   Connect current fan 2 signal 'ReportActInputValue' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief    Internal function for idle state machine processing
 *
 *   The function handles the idle state, which is active if the class is 'ready for tasks'
 *   Depending on the pending task, which is stored in m_taskID, the task specific handling
 *   function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CExhaustDevice::HandleIdleState()
{
    if(m_taskID == EXHAUST_DEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_taskID == EXHAUST_DEV_TASKID_REQ_SENSOR_STATUS)
    {
        HandleSensorStatusRequestTask();
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_taskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_INVALID_STATE, (quint16) m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_REPORT_IFACE;

    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the sensor status request task
 *
 *
 *   The function is called from HandleIdleState() if m_taskID == EXHAUST_DEV_TASKID_REQ_SENSOR_STATUS.
 *   The three task states of the reference run task are handled here:
 *
 *     EXHAUST_DEV_TASK_STATE_REQ :
 *        forward the request to the CANDigitalInput class which is connected to the
 *        sensor
 *
 *     EXHAUST_DEV_TASK_STATE_REQ_SEND:
 *        Check timeout, wait until the acknowledge was received by
 *        the slot(TBD), by calling this slot, this state will be exited
 *
 *     EXHAUST_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class
 *        Send the task's acknowledge signal, hdlInfo is set from
 *        m_lastErrorHdlInfo, which contains the error code as returned by the
 *        failed code
 *
 *     All other states, which are unexpected, will result in a EXHAUST_DEV_TASK_STATE_ERROR
 *      state to forward the error
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CExhaustDevice::HandleSensorStatusRequestTask()
{
    if((m_pDevProc == 0) || (m_pFlowSensor == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == EXHAUST_DEV_TASK_STATE_REQ)
    {
        ReturnCode_t retval;

        retval = m_pFlowSensor->ReqActInputValue();
        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_taskState = EXHAUST_DEV_TASK_STATE_REQ_SEND;
            m_stateTimer.Trigger();
            m_stateTimespan = EXHAUSTDEVICE_SENSOR_REQUEST_TIMEOUT;
        }
        else
        {
            m_lastErrorHdlInfo = retval;
            m_taskState = EXHAUST_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_REQ_SENSOR_STATUS_FCTCALL_FAILED, (quint16) m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CExhaustDevice: error while ReqInputValue:" << (int) retval;
        }
    }
    else if(m_taskState == EXHAUST_DEV_TASK_STATE_REQ_SEND)
    {
        if(m_stateTimer.Elapsed() > m_stateTimespan)
        {
            m_lastErrorHdlInfo = DCL_ERR_TIMEOUT;
            m_taskState = EXHAUST_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_REQ_SENSOR_STATUS_TIMEOUT, (quint16)m_taskID);

            FILE_LOG_L(laDEV, llERROR) << "  CExhaustDevice: while ReqInputValue.. timeout";
        }
    }
    else if(m_taskState == EXHAUST_DEV_TASK_STATE_ERROR)
    {

        FILE_LOG_L(laDEV, llERROR) << " CExhaustDevice: error EXHAUST_DEV_TASKID_REQ_SENSOR_STATUS ";

        //m_pIExhaustDevice->RouteFlowSensorStatus(m_instanceID, m_lastErrorHdlInfo, 0);
        ResetTask();
        // its a exhaust error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = EXHAUST_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_EXHAUST_DEV, ERROR_DCL_EXHAUST_DEV_INVALID_STATE, (quint16) m_taskState);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the error state of the class.
 *
 *   This task controls the error handling of the class.
 *   The function is called from HandleIdleState() if m_MainState == DEVICE_MAIN_STATE_ERROR.
 *   This task consists of the following states:
 *
 *     EXHAUST_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     EXHAUST_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     EXHAUST_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state
 *
 *     EXHAUST_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CExhaustDevice::HandleErrorState()
{
    if(m_errTaskState == EXHAUST_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_errTaskState == EXHAUST_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIExhaustDevice != 0)
        {
            m_pIExhaustDevice->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }*/
        m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_errTaskState == EXHAUST_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_errTaskState == EXHAUST_DEV_ERRTASK_STATE_RESET)
    {
        ResetTask();
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references

        m_pFlowSensor = 0;
        m_pCurrentConsumptionFan1 = 0;
        m_pCurrentConsumptionFan2 = 0;

    }
}

/****************************************************************************/
/*!
 *  \brief   This slot receives the current sensors error signals
 *
 *           This function will be called from CANAnalogInput and forwards the
 *           error information.
 *           The function is called if an error was detected by the slave's function module.
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_AINP_SLV or EVENT_GRP_DCL_FCTMOD_AINP_DCL
 *  \iparam   ErrorCode  = from CAN-message
 *  \iparam   ErrorData  = from CAN-message
 *  \iparam   ErrorTime  = error message reception time on master side
 *
 *
 ****************************************************************************/
void CExhaustDevice::FlowSensorError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
     Q_UNUSED(InstanceID);

    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief   This slot receives the flow sensor value
 *
 *           This function will be called from CANAnalogInput and forwards the
 *           analog value.
 *           The function is called after the analog value was requested,
 *           and this command was respond by the slave's function module.
 *
 *  \iparam   instanceID = instance identifier of the device
 *  \iparam   hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 *  \iparam   inputValue = analog value
 *
 *  \return  void
 *
 ****************************************************************************/
void CExhaustDevice::FlowSensorStatusChanged(quint32 instanceID, ReturnCode_t hdlInfo, quint16 inputValue)
{
    FILE_LOG_L(laDEV, llINFO) << "  CExhaustDevice::FlowSensorStatusChanged received:" << std::hex << instanceID;
    if((m_pFlowSensor != 0) && (m_pFlowSensor->GetModuleHandle() == instanceID))
    {
        if((m_taskID == EXHAUST_DEV_TASKID_REQ_SENSOR_STATUS) &&
           ((m_taskState == EXHAUST_DEV_TASK_STATE_REQ) || (m_taskState == EXHAUST_DEV_TASK_STATE_REQ_SEND)))
        {
            ResetTask();
        }
        FILE_LOG_L(laDEV, llINFO) << "  CExhaustDevice::FlowSensorStatusChanged received:" << inputValue;
        /*if(m_pIExhaustDevice != 0)
        {
            m_pIExhaustDevice->RouteFlowSensorStatus(m_instanceID, hdlInfo, inputValue);
        }*/
    }
}

/****************************************************************************/
/*!
 *  \brief   This slot receives the current sensors error signals
 *
 *           This function will be called from CANAnalogInput and forwards the
 *           error information.
 *           The function is called if an error was detected by the slave's function module.
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_AINP_SLV or EVENT_GRP_DCL_FCTMOD_AINP_DCL
 *  \iparam   ErrorCode  = from CAN-message
 *  \iparam   ErrorData  = from CAN-message
 *  \iparam   ErrorTime  = error message reception time on master side
 *
 *  \return  void
 *
 ****************************************************************************/
void CExhaustDevice::FanCurrentError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_errTaskState = EXHAUST_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief   This slot receives the current value
 *
 *           This function will be called from CANAnalogInput and forwards the
 *           analog value.
 *           The function is called after the analog value was requested,
 *           and this command was respond by the slave's function module.
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 *  \iparam   InputValue = analog value
 *
 *  \return  void
 *
 ****************************************************************************/
void CExhaustDevice::FanCurrentChanged(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue)
{
    Q_UNUSED(InstanceID);

    if((m_taskID == EXHAUST_DEV_TASKID_REQ_FAN_CURRENT) &&
       ((m_taskState == EXHAUST_DEV_TASK_STATE_REQ) || (m_taskState == EXHAUST_DEV_TASK_STATE_REQ_SEND)))
    {
        ResetTask();
    }

    FILE_LOG_L(laDEV, llINFO) << "  CExhaustDevice::FanCurrentChanged received:" << InputValue;

    /*if(m_pIExhaustDevice != 0)
    {
        m_pIExhaustDevice->RouteFanCurrent(m_instanceID, HdlInfo, InputValue);
    }*/
}

/****************************************************************************/
/*!
 *  \brief  Request the actual status of the flow sensor
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted,
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CExhaustDevice::ReqFlowSensorStatus()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if((m_taskID == EXHAUST_DEV_TASKID_FREE) && (m_taskState == EXHAUST_DEV_TASK_STATE_FREE))
    {
        m_taskID = EXHAUST_DEV_TASKID_REQ_SENSOR_STATUS;
        m_taskState = EXHAUST_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "   CExhaustDevice::ReqFlowSensorStatus ";
    }
    else
    {
        retval = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laDEV, llINFO) << "   CExhaustDevice::ReqFlowSensorStatus invalid state: "
                                  << (int) m_taskID << ", " << (int) m_taskState;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Returns the status of the air flow sensor
 *
 *  \return Status of the air flow sensor
 */
/****************************************************************************/
quint16 CExhaustDevice::GetFlowSensorStatus()
{
    return m_flowSensorStatus;
}

/****************************************************************************/
/*!
 *  \brief  Free the taskID and the task state
 */
/****************************************************************************/
void CExhaustDevice::ResetTask()
{
    m_taskID = EXHAUST_DEV_TASKID_FREE;
    m_taskState = EXHAUST_DEV_TASK_STATE_FREE;
}

} //namespace
