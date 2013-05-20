
#include "DeviceControl/Include/Devices/PeripheryDevice.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include <sys/stat.h>
#include <QtDebug>

namespace DeviceControl
{
CPeripheryDevice::CPeripheryDevice(DeviceProcessing* pDeviceProcessing, QString Type) : CBaseDevice(pDeviceProcessing, Type)
{
    Reset();
    FILE_LOG_L(laDEV, llINFO) << "Retort device created";
    qDebug() <<  "Retort device cons thread id is " << QThread::currentThreadId();
}

CPeripheryDevice::~CPeripheryDevice()
{
    Reset();
}
void CPeripheryDevice::Reset()
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_ErrorTaskState   = PER_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_MAIN_CONTROL;

    memset( &m_pDigitalOutputs, 0 , sizeof(m_pDigitalOutputs));
    memset( &m_TargetDOOutputValues, 0 , sizeof(m_TargetDOOutputValues));

}
/****************************************************************************/
/*!
 *  \brief  Handles the internal state machine
 *
 *      This function should be called periodically, it handles the state
 *      machine of the class. Furthermore, the HandleTask-function of the
 *      interface class will be called. Each main state has a individual
 *      handling function, which will be called according to the currently
 *      active main state.
 */
/****************************************************************************/
void CPeripheryDevice::HandleTasks()
{
    ReturnCode_t RetVal;

    if(m_MainState == DEVICE_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_MainState == DEVICE_MAIN_STATE_START)
    {
        //fall through
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
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = PER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
            /// \todo maybe we need a state to ensure the reference run call!!
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = PER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_FCT_MOD_CFG)
    {
        RetVal = ConfigureDeviceTasks();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = PER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CPeripheryDevice:handleTasks, new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}
/****************************************************************************/
/*!
 *  \brief    Internal function for idle state machine processing
 *
 *   The function handles the idle state, which is active if the class is 'ready for tasks'
 *   Depending on the pending task, which is stored in m_TaskID, the task specific handling
 *   function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CPeripheryDevice::HandleIdleState()
{
}


/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 *
 ****************************************************************************/
ReturnCode_t CPeripheryDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    FILE_LOG_L(laDEV, llINFO) << "  CPeripheryDevice::HandleInitializationState()";

    m_pDigitalOutputs[PER_MAIN_RELAY] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerMainRelayDOKey));
    if(m_pDigitalOutputs[PER_MAIN_RELAY] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_MAINRELAYDO);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_BOTTOMTEMPCTRL not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        m_InstDOTypeMap[((CANObjectKeyLUT::FCTMOD_PER_MAINRELAYDO & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_PER_MAINRELAYDO & 0xF)] = PER_MAIN_RELAY;
    }

/*    m_pDigitalOutputs[PER_REMOTE_ALARM_SET] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerRemoteAlarmSetDOKey));
    if(m_pDigitalOutputs[PER_REMOTE_ALARM_SET] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMSETDO);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_BOTTOMTEMPCTRL not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        m_InstDOTypeMap[((CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMSETDO & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMSETDO & 0xF)] = PER_REMOTE_ALARM_SET;
    }
*/

    m_pDigitalOutputs[PER_REMOTE_ALARM_CTRL] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerRemoteAlarmCtrlDOKey));
    if(m_pDigitalOutputs[PER_REMOTE_ALARM_CTRL] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCTRLDO);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_PER_REMOTEALARMCTRLDO not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        m_InstDOTypeMap[((CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCTRLDO & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCTRLDO & 0xF)] = PER_REMOTE_ALARM_CTRL;
    }

    m_pDigitalOutputs[PER_LOCAL_ALARM_CTRL] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerLocalAlarmCtrlDOKey));
    if(m_pDigitalOutputs[PER_LOCAL_ALARM_CTRL] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_LOCALALARMCTRLDO);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_PER_REMOTEALARMCTRLDO not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        m_InstDOTypeMap[((CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCTRLDO & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_PER_LOCALALARMCTRLDO & 0xF)] = PER_LOCAL_ALARM_CTRL;
    }
/*    m_pDigitalOutputs[PER_REMOTE_ALARM_CLEAR] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerRemoteAlarmClearDOKey));
    if(m_pDigitalOutputs[PER_REMOTE_ALARM_CLEAR] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCLEARDO);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_PER_REMOTEALARMCLEARDO not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        m_InstDOTypeMap[((CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCLEARDO & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCLEARDO & 0xF)] = PER_REMOTE_ALARM_CLEAR;
    }
*/
    return RetVal;

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
ReturnCode_t CPeripheryDevice::HandleConfigurationState()
{

    if(!connect(m_pDigitalOutputs[PER_MAIN_RELAY], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_MAINRELAYDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigitalOutputs[PER_REMOTE_ALARM_CTRL], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCTRLDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigitalOutputs[PER_LOCAL_ALARM_CTRL], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_LOCALALARMCTRLDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
/*
    if(!connect(m_pDigitalOutputs[PER_REMOTE_ALARM_SET], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMSETDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigitalOutputs[PER_REMOTE_ALARM_CLEAR], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCLEARDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
*/

    return DCL_ERR_FCT_CALL_SUCCESS;

}




/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CPeripheryDevice::ConfigureDeviceTasks()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}
void CPeripheryDevice::HandleErrorState()
{
    if(m_ErrorTaskState == PER_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == PER_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIOvenDev != 0)
    {
    m_pIOvenDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
    }*/
        m_ErrorTaskState = PER_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == PER_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = PER_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == PER_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pDigitalOutputs[PER_MAIN_RELAY] = 0;
        //m_pDigitalOutputs[PER_REMOTE_ALARM_CLEAR] = 0;
        m_pDigitalOutputs[PER_REMOTE_ALARM_CTRL] = 0;
       // m_pDigitalOutputs[PER_REMOTE_ALARM_SET] = 0;
    }
}



bool CPeripheryDevice::SetDOValue(PerDOType_t Type, quint16 OutputValue, quint16 Duration, quint16 Delay)
{
    m_TargetDOOutputValues[Type] = OutputValue;

    ReturnCode_t retCode = m_pDigitalOutputs[Type]->SetOutputValue(m_TargetDOOutputValues[Type], Duration, Delay);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return false;
    }
    return (DCL_ERR_FCT_CALL_SUCCESS == m_pDevProc->BlockingForSyncCall(SYNC_CMD_PER_SET_DO_VALVE));
}
void CPeripheryDevice::OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue)
{
    Q_UNUSED(OutputValue)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Periphery Set DO output successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Periphery set DO output failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_PER_SET_DO_VALVE, ReturnCode);

}
ReturnCode_t CPeripheryDevice::TurnOnMainRelay()
{
    if (SetDOValue(PER_MAIN_RELAY,1, 0, 0))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}
ReturnCode_t CPeripheryDevice::TurnOffMainRelay()
{
    if(SetDOValue(PER_MAIN_RELAY, 0, 0, 0))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}


} //namespace
