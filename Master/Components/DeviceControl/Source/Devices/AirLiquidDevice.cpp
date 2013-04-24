

#include "DeviceControl/Include/Devices/AirLiquidDevice.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/PressureControl.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include <sys/stat.h>
#include <QtDebug>

namespace DeviceControl
{
#define UNDEFINED (999)
#define CHECK_SENSOR_TIME (200) // in msecs
const qint32 TOLERANCE = 10; //!< tolerance value for calculating inside and outside range
CAirLiquidDevice::CAirLiquidDevice(DeviceProcessing* pDeviceProcessing, QString Type) : CBaseDevice(pDeviceProcessing, Type),
        m_pPressureCtrl(0),  m_pFanDigitalOutput(0)
{
    Reset();
    FILE_LOG_L(laDEV, llINFO) << "Air-liquid device created";
    qDebug() <<  "airliquid device cons thread id is " << QThread::currentThreadId();
}

CAirLiquidDevice::~CAirLiquidDevice()
{
    Reset();
}
void CAirLiquidDevice::Reset()
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_ErrorTaskState   = AL_DEV_ERRTASK_STATE_FREE;

    m_PressureDrift = 0;
    m_instanceID = DEVICE_INSTANCE_ID_AIR_LIQUID;
    m_TargetPressure = UNDEFINED;
    m_CurrentPressure = UNDEFINED;

    m_WorkingPressurePositive = 30;
    m_WorkingPressureNegative = -30;
    if(!m_PIDDataList.isEmpty())
    {
        m_PIDDataList.clear();
    }
    m_pPressureCtrl = NULL;
    m_pFanDigitalOutput = NULL;
    memset( &m_LastGetTempTime, 0 , sizeof(m_LastGetTempTime));
    memset( &m_LastGetPressureTime, 0 , sizeof(m_LastGetPressureTime));
    memset( &m_SuckingTime, 0 , sizeof(m_SuckingTime));
    memset( &m_TargetTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_TargetTempCtrlStatus));
    memset( &m_CurrentTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_CurrentTempCtrlStatus));
    memset( &m_CurrentTemperatures, 0 , sizeof(m_CurrentTemperatures));
    memset( &m_TargetTemperatures, 0 , sizeof(m_TargetTemperatures));
    memset( &m_MainsVoltageStatus, 0 , sizeof(m_MainsVoltageStatus));
    memset( &m_pTempCtrls, 0 , sizeof(m_pTempCtrls));
    m_TargetPressureCtrlStatus = PRESSURECTRL_STATUS_UNDEF;
    m_CurrentPressureCtrlStatus = PRESSURECTRL_STATUS_UNDEF;
    m_TargetDOOutputValue = 0;

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
void CAirLiquidDevice::HandleTasks()
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
            m_ErrorTaskState = AL_DEV_ERRTASK_STATE_REPORT_IFACE;
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
            m_ErrorTaskState = AL_DEV_ERRTASK_STATE_REPORT_IFACE;
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
            m_ErrorTaskState = AL_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CAirLiquidDevice:handleTasks, new state: " << (int) m_MainState;
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
void CAirLiquidDevice::HandleIdleState()
{
    CheckSensorsData();
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
ReturnCode_t CAirLiquidDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    FILE_LOG_L(laDEV, llINFO) << "  CAirLiquidDevice::HandleInitializationState()";

    m_pPressureCtrl = (CPressureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_ALPressureCtrlKey));
    if(m_pPressureCtrl == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_PRESSURECTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AL_PRESSURECTRL not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pTempCtrls[AL_LEVELSENSOR] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_ALLevelSensorTempCtrlKey));
    if(m_pTempCtrls[AL_LEVELSENSOR] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AL_LEVELSENSORTEMPCTRL not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        m_InstTCTypeMap[((CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL & 0xF)] = AL_LEVELSENSOR;
    }

    m_pTempCtrls[AL_TUBE1] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_ALTube1TempCtrlKey));
    if(m_pTempCtrls[AL_TUBE1] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AL_TUBE1TEMPCTRL not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        m_InstTCTypeMap[ ((CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL & 0xF)] = AL_TUBE1;
    }

    m_pTempCtrls[AL_TUBE2] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_ALTube2TempCtrlKey));
    if(m_pTempCtrls[AL_TUBE2] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AL_TUBE2TEMPCTRL not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        m_InstTCTypeMap[((CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL & 0xF)] = AL_TUBE2;
    }

    m_pFanDigitalOutput = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_ALFanDOKey));
    if(m_pFanDigitalOutput == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_FANDO);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AL_FANDO not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
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
ReturnCode_t CAirLiquidDevice::HandleConfigurationState()
{
    if(!connect(m_pPressureCtrl, SIGNAL(ReportRefPressure(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetPressure(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_PRESSURECTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect pressure ctrl signal 'ReportRefPressure'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pPressureCtrl, SIGNAL(ReportRefValveState(quint32,ReturnCode_t,quint8,quint8)),
                this, SLOT(OnSetValve(quint32,ReturnCode_t,quint8,quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_PRESSURECTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect pressure ctrl signal 'ReportRefValveState'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pPressureCtrl, SIGNAL(ReportActPressure(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetPressure(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_PRESSURECTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect pressure ctrl signal 'ReportActPressure'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_LEVELSENSOR], SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportRefTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_TUBE1], SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportRefTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_TUBE2], SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportRefTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_LEVELSENSOR], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;

    }
    if(!connect(m_pTempCtrls[AL_TUBE1], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_TUBE2], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_LEVELSENSOR], SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_TUBE1], SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[AL_TUBE2], SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_LEVELSENSOR], SIGNAL(ReportLevelSensorState(quint32, ReturnCode_t, quint8)),
                this, SLOT(OnLevelSensorState(quint32, ReturnCode_t, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportLevelSensorState'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[AL_LEVELSENSOR], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[AL_TUBE1], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[AL_TUBE2], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pFanDigitalOutput, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_FANDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;

}

void CAirLiquidDevice::CheckSensorsData()
{

    //  qDebug() <<  "AL timer thread id is " << QThread::currentThreadId();

    if(m_pPressureCtrl )
    {
        GetPressureAsync(0);
    }
    if(m_pTempCtrls[AL_LEVELSENSOR])
    {
        GetTemperatureAsync(AL_LEVELSENSOR, 0);
    }
    if(m_pTempCtrls[AL_TUBE1])
    {
        GetTemperatureAsync(AL_TUBE1, 0);
    }
    if(m_pTempCtrls[AL_TUBE2])
    {
        GetTemperatureAsync(AL_TUBE2, 0);
    }
}


/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CAirLiquidDevice::ConfigureDeviceTasks()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

void CAirLiquidDevice::HandleErrorState()
{
    if(m_ErrorTaskState == AL_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == AL_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIOvenDev != 0)
    {
    m_pIOvenDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
    }*/
        m_ErrorTaskState = AL_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == AL_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = AL_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == AL_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pPressureCtrl = 0;
        m_pTempCtrls[AL_LEVELSENSOR] = 0;
        m_pTempCtrls[AL_TUBE1] = 0;
        m_pTempCtrls[AL_TUBE2] = 0;
        m_pFanDigitalOutput= 0;
    }
}

bool CAirLiquidDevice::SetPressure(quint8 flag, qreal NominalPressure)
{
    //m_TargetPressure = NominalPressure;
    m_TargetPressure = NominalPressure + m_PressureDrift;
    ReturnCode_t retCode =  m_pPressureCtrl->SetPressure(flag, m_TargetPressure);
    if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return false;
    }
    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_SET_PRESSURE);
    return (retCode == DCL_ERR_FCT_CALL_SUCCESS);
}

void CAirLiquidDevice::OnSetPressure(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Pressure)
{
    m_CurrentPressure = Pressure;
    if (m_CurrentPressure != m_TargetPressure)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_PRESSURE, DCL_ERR_FCT_CALL_FAILED);
        FILE_LOG_L(laDEVPROC, llWARNING) << " ERROR: Target pressure is not reached. ";
        qDebug() << "ERROR: Target pressure is not reached.";
    }
    else
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_PRESSURE, DCL_ERR_FCT_CALL_SUCCESS);
    }
    m_TargetPressure = 0;
}

bool CAirLiquidDevice::SetValve(quint8 ValveIndex, quint8 ValveState)
{
    ReturnCode_t retCode = m_pPressureCtrl->SetValve(ValveIndex, ValveState);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
        return false;
    }
    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_SET_VALVE);
    return (retCode == DCL_ERR_FCT_CALL_SUCCESS);
}

void CAirLiquidDevice::OnSetValve(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 ValveIndex, quint8 ValveState)
{
    Q_UNUSED(ValveIndex)
    Q_UNUSED(ValveState)
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_VALVE, ReturnCode);
}

qreal CAirLiquidDevice::GetPressure(quint8 Index)
{
    qreal RetValue;
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetPressureTime[Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        ReturnCode_t retCode = m_pPressureCtrl->ReqActPressure(Index);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            RetValue = UNDEFINED;
        }
        else
        {
            retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_GET_PRESSURE);
            //retCode =  (ReturnCode_t)m_LoopGetPressure.exec();
            if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                RetValue = UNDEFINED;
            }
            else
            {
                RetValue = m_CurrentPressure - m_PressureDrift;
            }
        }
            m_LastGetPressureTime[Index] = Now;
    }
    else
    {
        RetValue = m_CurrentPressure - m_PressureDrift;
    }
    return RetValue;
}

bool CAirLiquidDevice::GetPressureAsync(quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetPressureTime[Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        {
            m_LastGetPressureTime[Index] = Now;
        }
        return (DCL_ERR_FCT_CALL_SUCCESS == (m_pPressureCtrl->ReqActPressure(Index)));
    }
    return true;
}

void CAirLiquidDevice::OnGetPressure(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 Index, qreal Pressure)
{
    Q_UNUSED(Index)
    m_CurrentPressure = Pressure;
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_GET_PRESSURE, ReturnCode);
}

bool CAirLiquidDevice::SetPressureControlStatus(PressureCtrlStatus_t PressureCtrlStatus)
{
    m_TargetPressureCtrlStatus = PressureCtrlStatus;
    ReturnCode_t retCode = m_pPressureCtrl->SetStatus(PressureCtrlStatus);
    return (DCL_ERR_FCT_CALL_SUCCESS == retCode);
}

ReturnCode_t CAirLiquidDevice::SetPressureCtrlON()
{
    if(SetPressureControlStatus(PRESSURECTRL_STATUS_ON))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}

ReturnCode_t CAirLiquidDevice::SetPressureCtrlOFF()
{
    if(SetPressureControlStatus(PRESSURECTRL_STATUS_OFF))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}

void CAirLiquidDevice::StopCompressor(void)
{
    //Log(tr("Shut down compressor"));
    FILE_LOG_L(laDEVPROC, llINFO) << " INFO: Shut down compressor. ";
    SetPressure(0, 10);
}

ReturnCode_t CAirLiquidDevice::ReleasePressure(void)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start release pressure procedure";
    QTimer timer;
    quint32 TimeSlotPassed = 0;
    ReturnCode_t retCode= DCL_ERR_FCT_CALL_SUCCESS;


    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_PRESSURE, DCL_ERR_UNEXPECTED_BREAK);
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_DRAINING, DCL_ERR_UNEXPECTED_BREAK);
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_RELEASE_PRESSURE, DCL_ERR_UNEXPECTED_BREAK);
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, DCL_ERR_UNEXPECTED_BREAK);
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_VACCUM, DCL_ERR_UNEXPECTED_BREAK);

    connect(&timer, SIGNAL(timeout()), this, SLOT(ReleasePressureTimerCB()));
    //close both valve
    StopCompressor();
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Close Both Valves";
    SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
    SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
    qreal CurrentPressure = GetPressure(0);

    timer.setSingleShot(false);
    timer.start(500);
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Wait for current pressure get to ZERO";
    while((CurrentPressure > 2)||(CurrentPressure < (-2)))
    {
        CurrentPressure = GetPressure(0);
        //if (m_LoopReleasePressureTimer.exec() == RELEASE_PRESSURE_PROCEDURE_INTERRUPT)
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_RELEASE_PRESSURE);
        if (DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            timer.stop();
            return DCL_ERR_UNEXPECTED_BREAK;
        }
        TimeSlotPassed++;
        if(TimeSlotPassed*500 > RELEASE_PRESSURE_MAX_TIME)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING:  Release Pressure exceed maximum setup time, exit!";
            //stop compressor
            StopCompressor();
            //close both valve
            SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
            SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
            return DCL_ERR_DEV_AL_RELEASE_PRESSURE_TIMEOUT;
        }
    }
    TurnOffFan();
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t CAirLiquidDevice::Vaccum()
{

    ReturnCode_t RetValue = DCL_ERR_FCT_CALL_SUCCESS;
    if( DCL_ERR_FCT_CALL_SUCCESS != ReleasePressure())
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING:  Release pressure failed, exit now.";
        RetValue = DCL_ERR_DEV_AL_RELEASE_PRESSURE_FAILED;
        goto SORTIE;
    }
    TurnOnFan();
    if(!SetTargetPressure(m_WorkingPressureNegative))
    {
        RetValue = DCL_ERR_DEV_AL_SETUP_PRESSURE_FAILED;
        //stop compressor
        StopCompressor();
        //close both valve
        SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
        SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
        TurnOffFan();
    }
SORTIE:
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Pressure finished, exit";
    return RetValue;
}

ReturnCode_t CAirLiquidDevice::Pressure()
{
    ReturnCode_t RetValue = DCL_ERR_FCT_CALL_SUCCESS;
    if( DCL_ERR_FCT_CALL_SUCCESS != ReleasePressure())
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING:  Release pressure failed, exit now.";
        RetValue = DCL_ERR_DEV_AL_RELEASE_PRESSURE_FAILED;
        goto SORTIE;
    }
    TurnOnFan();
    if(!SetTargetPressure(m_WorkingPressurePositive))
    {
        RetValue = DCL_ERR_DEV_AL_SETUP_PRESSURE_FAILED;
        //stop compressor
        StopCompressor();
        //close both valve
        SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
        SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
        TurnOffFan();
    }
SORTIE:
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Pressure finished, exit";
    return RetValue;
}

ReturnCode_t CAirLiquidDevice::Draining(quint32 DelayTime)
{

    bool stop = false;
    ReturnCode_t RetValue = DCL_ERR_DEV_AL_DRAIN_SUCCESS;
    QTimer timer;
    QDateTime beforeDraining = QDateTime::currentDateTime();
    qreal CurrentPressure = 0;
    bool PressureHasBeenSetup = false;
    qint32 counter = 0;
    qint32 TimeSlotPassed = 0;
    bool WarnShowed = false;
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    connect(&timer, SIGNAL(timeout()), this, SLOT(DrainingTimerCB()));
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start Drainging procedure.";
    //release pressure
    if( DCL_ERR_FCT_CALL_SUCCESS != ReleasePressure())
    {
        RetValue = DCL_ERR_DEV_AL_RELEASE_PRESSURE_FAILED;
        goto SORTIE;
    }

    TurnOnFan();
    if(!SetTargetPressure(m_WorkingPressurePositive))
    {
        RetValue = DCL_ERR_DEV_AL_SETUP_PRESSURE_FAILED;
        goto SORTIE;
    }
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Set target pressure finished.";
    timer.setSingleShot(false);
    timer.start(DRAINGING_POLLING_TIME);

    while(!stop)
    {
        // if (m_LoopDrainingTimer.exec()== (-1))
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_DRAINING);
        if (DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            RetValue = DCL_ERR_DEV_AL_DRAIN_INTERRUPT;
            goto SORTIE;
        }
        CurrentPressure = GetPressure(0);

        if(!PressureHasBeenSetup)
        {
            if(CurrentPressure >= DRAINGING_TARGET_THRESHOLD_PRESSURE)
            {
                PressureHasBeenSetup = true;
                FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Pressure has been set up";
            }
        }
        else
        {
            if(CurrentPressure < DRAINGING_TARGET_FINISHED_PRESSURE)
            {
                counter++;
                if(counter > 6)
                {
                    FILE_LOG_L(laDEVPROC, llINFO) << "Drain finished!";
                    stop = true;
                }
            }
            else
            {
                counter = 0;
            }
        }
        TimeSlotPassed++;
        if(((TimeSlotPassed * DRAINGING_POLLING_TIME) > DRAINGING_SETUP_WARNING_TIME) && (!WarnShowed))
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "Warning: Draining do not finished in expected time";
            WarnShowed = true;
        }
        if((TimeSlotPassed * DRAINGING_POLLING_TIME) > DRAINGING_MAX_SETUP_TIME)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "Warning: Draining exceed maximum setup time(%1 seconds), exit!";
            RetValue = DCL_ERR_DEV_AL_DRAIN_SETUP_PRESSURE_TIMEOUT;
            goto SORTIE;
        }
    }

    //waiting for some time
    if(DelayTime > 0)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Finished. start hold for "<< DelayTime << " millisecond.";
        timer.setSingleShot(true);
        timer.start(DelayTime);
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_DRAINING);
        if (DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            RetValue = DCL_ERR_DEV_AL_DRAIN_INTERRUPT;
            goto SORTIE;
        }
    }

SORTIE:
    if(timer.isActive())
    {
        timer.stop();
    }
    //stop compressor
    StopCompressor();
    //close both valve
    SetValve(VALVE_1_INDEX,VALVE_STATE_CLOSE);
    SetValve(VALVE_2_INDEX,VALVE_STATE_CLOSE);
    TurnOffFan();

    return RetValue;
}

ReturnCode_t CAirLiquidDevice::Filling(quint32 DelayTime)
{
    ReturnCode_t RetValue = DCL_ERR_DEV_AL_FILL_SUCCESS;
    qint32 retCode = DCL_ERR_FCT_CALL_SUCCESS;
    QTimer timer;
    quint32 counter = 0;
    quint32 CounterStopValue = 0;
    QList<qreal> PressureBuf;
    int levelSensorState = 0xFF;
    bool stop = false;
    bool WarnShowed = false;
    connect(&timer, SIGNAL(timeout()), this, SLOT(SuckingTimerCB()));
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start Sucking procedure.";

    //release pressure
    if( DCL_ERR_FCT_CALL_SUCCESS != ReleasePressure())
    {
        RetValue = DCL_ERR_DEV_AL_RELEASE_PRESSURE_FAILED;
        goto SORTIE;
    }
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start Sucking now.";
    TurnOnFan();
    if(!SetTargetPressure(m_WorkingPressureNegative))
    {
        RetValue = DCL_ERR_DEV_AL_SETUP_PRESSURE_FAILED;
        goto SORTIE;
    }

    //set timeout to 2 minutes
    timer.setSingleShot(false);
    timer.start(SUCKING_POOLING_TIME);

    while(!stop)
    {
        // levelSensorState = m_LoopSuckingLevelSensor.exec();
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR);
        counter++;
        //if(levelSensorState == 1)
        if(DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_1 == retCode)
        {
            if(CounterStopValue == 0)
            {
                FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Hit target level. Sucking Finished.";
                if(DelayTime > 0)
                {
                    //waiting for some time
                    if(DelayTime < SUCKING_MAX_DELAY_TIME)
                    {
                        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Delay for " << DelayTime<<" milliseconds.";
                        CounterStopValue = counter + DelayTime / SUCKING_POOLING_TIME;
                    }
                    else
                    {
                        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Delay for " << SUCKING_MAX_DELAY_TIME<<" milliseconds.";
                        CounterStopValue = counter + SUCKING_MAX_DELAY_TIME / SUCKING_POOLING_TIME;
                    }
                }
                else
                {
                    stop = true;
                }
            }
        }
        //else if(levelSensorState == 0)
        else if(DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_0 == retCode)
        {
            FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Received level sensor signal 0 ";
        }
        //else if (levelSensorState == (-1))
        else if(DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            RetValue = DCL_ERR_DEV_AL_FILL_INTERRUPT;
            goto SORTIE;
        }
        //else if(levelSensorState == 3)
        else if(DCL_ERR_TIMER_TIMEOUT == retCode)
        {

            if((counter > (SUCKING_SETUP_WARNING_TIME / SUCKING_POOLING_TIME)) && ( !WarnShowed ))
            {
                FILE_LOG_L(laDEVPROC, llWARNING) << "Warning! Do not get level sensor data in" << (SUCKING_SETUP_WARNING_TIME / 1000)<<" seconds.";
                WarnShowed = true;
            }
            if(counter > (SUCKING_MAX_SETUP_TIME / SUCKING_POOLING_TIME))
            {
                FILE_LOG_L(laDEVPROC, llERROR) << "ERROR! Do not get level sensor data in" << (SUCKING_MAX_SETUP_TIME / 1000)<<" seconds, Time out! Exit!";
                RetValue = DCL_ERR_DEV_AL_FILL_TIMEOUT;
                goto SORTIE;
            }
            if((CounterStopValue!=0)&&(CounterStopValue <= counter))
            {
                FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Delay finished!";
                goto SORTIE;
            }
            //check pressure here
            qreal CurrentPressure = GetPressure(0);
            if(CurrentPressure != (-1))
            {
                PressureBuf.append(CurrentPressure);
            }
            if(PressureBuf.length() > SUCKING_OVERFLOW_SAMPLE_SIZE)
            {
                //after 2012.11.15, algorithm: at least one sample bigger than target, and two sample bigger than 3 samples' mean.
                PressureBuf.pop_front();
                quint32 ExceptPointNum = 0;
                bool biggerThanTarget = false;
                for(qint32 i = 0; i < PressureBuf.length(); i++)
                {
                    biggerThanTarget |= qAbs(PressureBuf.at(i)) > qAbs(SUCKING_TARGET_PRESSURE);

                    if(i < SUCKING_OVERFLOW_WINDOW_SIZE)
                    {   continue;   }
                    qreal sum = 0;
                    for(qint32 j = (i - SUCKING_OVERFLOW_WINDOW_SIZE); j < i ; j++)
                    {
                        sum += qAbs(PressureBuf.at(j));
                    }
                    qreal average = sum/SUCKING_OVERFLOW_WINDOW_SIZE;
                    if(( average + SUCKING_OVERFLOW_TOLERANCE) <= qAbs(PressureBuf.at(i)))
                    {
                        ExceptPointNum++;
                    }
                }

                if((ExceptPointNum > 1)&&(biggerThanTarget))
                {
                    FILE_LOG_L(laDEVPROC, llERROR) << "ERROR! Overflow occured! Exit now!";
                    RetValue = DCL_ERR_DEV_AL_FILL_OVERFLOW;
                    goto SORTIE;
                }
            }
        }
        else
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Unexpected level sensor state value: " << levelSensorState;
        }
    }


SORTIE:
    if(timer.isActive())
    {
        timer.stop();
    }
    ReleasePressure();
    TurnOffFan();
    return RetValue;
}

ReturnCode_t CAirLiquidDevice::PressureForBottoleCheck()
{
    qDebug()<< "Start setting up pressure for bottle check";
    QTimer timer;
    bool stop = false;
    quint32 TimeSlotPassed = 0;
    qreal CurrentPressure;
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    connect(&timer, SIGNAL(timeout()), this, SLOT(PressureTimerCB()));

    //release pressure
    if( DCL_ERR_FCT_CALL_SUCCESS != ReleasePressure())
    {
        retCode = DCL_ERR_DEV_AL_RELEASE_PRESSURE_FAILED;
        goto SORTIE;
    }

    //start compressor
    if(!SetTargetPressure(10))
    {
        retCode = DCL_ERR_DEV_AL_SETUP_PRESSURE_FAILED;
        goto SORTIE;
    }
    IsPIDDataSteady(0,  0,  0,  0, true);
    timer.setSingleShot(false);
    timer.start(200);

    while(!stop)
    {
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_PRESSURE);
        if ( DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            retCode = DCL_ERR_DEV_AL_FILL_INTERRUPT;
            goto SORTIE;
        }
        TimeSlotPassed++;
        CurrentPressure = GetRecentPressure(0);
        if (IsPIDDataSteady(10,  CurrentPressure,  \
                            2, 8, false))
        {
            qDebug()<<"Target pressure is getting steady now.";
            stop = true;
            timer.stop();
        }
        else
        {
            if((TimeSlotPassed * 200) > PRESSURE_MAX_SETUP_TIME)
            {
                timer.stop();
                qDebug()<<"Warning: Pressure exceed maximum setup time, exit!";
                //stop compressor
                StopCompressor();
                //close both valve
                SetValve(0,0);
                SetValve(1,0);
                retCode = DCL_ERR_DEV_BOTTLE_CHECK_TIMEOUT;
                goto SORTIE;
            }
        }
    }
    //stop compressor
    StopCompressor();
SORTIE:
    if(timer.isActive())
    {
        timer.stop();
    }
    return retCode;
}

bool CAirLiquidDevice::IsPIDDataSteady(qreal TargetValue, qreal CurrentValue, qreal Tolerance, qint32 Num, bool Init)
{
    bool ret = false;
    if(Init)
    {
        if(!m_PIDDataList.isEmpty())
        {
            m_PIDDataList.clear();
        }
    }
    else
    {
        qreal diff = (CurrentValue > TargetValue) ? (CurrentValue - TargetValue) : (TargetValue - CurrentValue );
        m_PIDDataList.append(diff);
        if(m_PIDDataList.size() >= Num)
        {
            ret = true;
            for(int idx =0; idx < Num; idx++)
            {
                if(m_PIDDataList.at(m_PIDDataList.size() - idx - 1) > Tolerance)
                {
                    ret = false;
                    break;
                }
            }
        }
    }

    return ret;
}

bool CAirLiquidDevice::SetTargetPressure(qreal pressure)
{
    if(pressure > 0)
    {
        //close valve 1
        SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
        //open valve 2
        SetValve(VALVE_2_INDEX, VALVE_STATE_OPEN);
        return  SetPressure(1, pressure);//should be 1
    }
    else if(pressure < 0)
    {
        //open valve 1
        SetValve(VALVE_1_INDEX, VALVE_STATE_OPEN);
        //close valve 2
        SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
        return SetPressure(9, pressure);//should be 9
    }
    return true;
}

void CAirLiquidDevice::VaccumTimerCB(void)
{
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_VACCUM, DCL_ERR_TIMER_TIMEOUT);
}

void CAirLiquidDevice::PressureTimerCB(void)
{
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_PRESSURE, DCL_ERR_TIMER_TIMEOUT);
}

void CAirLiquidDevice::DrainingTimerCB(void)
{
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_DRAINING, DCL_ERR_TIMER_TIMEOUT);
}

void CAirLiquidDevice::ReleasePressureTimerCB(void)
{
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_RELEASE_PRESSURE, DCL_ERR_TIMER_TIMEOUT);
}

void CAirLiquidDevice::SuckingTimerCB(void)
{
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, DCL_ERR_TIMER_TIMEOUT);
}

void CAirLiquidDevice::OnLevelSensorState(quint32, ReturnCode_t ReturnCode, quint8 State)
{
    qDebug() <<  "new level sensor state: " << State;

    Q_UNUSED(ReturnCode)
    if(State == 1)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_1);
    }
}

bool CAirLiquidDevice::SetTemperatureControlStatus(ALTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus)
{
    m_TargetTempCtrlStatus[Type] = TempCtrlStatus;
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        retCode = m_pTempCtrls[Type]->SetStatus(TempCtrlStatus);
        return (DCL_ERR_FCT_CALL_SUCCESS== retCode);
    }
    else
    {
        return false;
    }
}
ReturnCode_t CAirLiquidDevice::SetTempCtrlON(ALTempCtrlType_t Type)
{
    if (SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_ON))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}

ReturnCode_t CAirLiquidDevice::SetTempCtrlOFF(ALTempCtrlType_t Type)
{
    if(SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_OFF))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}

qreal CAirLiquidDevice::GetRecentPressure(quint8 Index)
{
   // QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetPressureTime[Index]) <= 500) // check if 200 msec has passed since last read
    {
        return (m_CurrentPressure - m_PressureDrift);
    }
    else
    {
        return UNDEFINED;
    }
}

qreal CAirLiquidDevice::GetRecentTemperature(ALTempCtrlType_t Type, quint8 Index)
{
  //  QMutexLocker Locker(&m_Mutex);
quint32 thisadd =(quint32)this;
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue;
    if((Now - m_LastGetTempTime[Type][Index]) <= 500) // check if 500 msec has passed since last read
    {
        RetValue = m_CurrentTemperatures[Type];
    }
    else
    {
        RetValue = UNDEFINED;
    }
    return RetValue;
}

ReturnCode_t CAirLiquidDevice::StartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
#ifndef PRE_ALFA_TEST
    Log(tr("StartTemperatureControl"));
#endif
    m_TargetTemperatures[Type] = NominalTemperature;
    m_TargetTempCtrlStatus[Type] = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState(Type) == TEMPCTRL_STATE_ERROR)
    {
        // Log(tr("Not able to read the temperature control status"));
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn(Type))
    {
        return DCL_ERR_DEV_TEMP_CTRL_ALREADY_ON;
    }
    if (IsTemperatureControlOff(Type))
    {
        //Set the nominal temperature
        if (!SetTemperature(Type, NominalTemperature, SlopeTempChange))
        {
            // Log(tr("Not able to set temperature"));
            return DCL_ERR_DEV_TEMP_CTRL_SET_TEMP_ERR;
        }
        //ON the temperature control
        if (!SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_ON))
        {
            // Log(tr("Not able to start temperature control"));
            return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
        }
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t CAirLiquidDevice::StartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    ReturnCode_t retCode;
    m_TargetTemperatures[Type] = NominalTemperature;
    m_TargetTempCtrlStatus[Type] = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState(Type) == TEMPCTRL_STATE_ERROR)
    {
        // Log(tr("Not able to read the temperature control status"));
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn(Type))
    {
        if(!SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_OFF))
        {
            return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
    }
    }

    retCode = SetTemperaturePid(Type, MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
    if(retCode != DCL_ERR_FCT_CALL_SUCCESS)
    {
         return retCode;
    }
    //Set the nominal temperature
    if (!SetTemperature(Type, NominalTemperature, SlopeTempChange))
    {
        // Log(tr("Not able to set temperature"));
        return DCL_ERR_DEV_TEMP_CTRL_SET_TEMP_ERR;
    }
    //ON the temperature control
    if (!SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_ON))
    {
        // Log(tr("Not able to start temperature control"));
        return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

TempCtrlState_t CAirLiquidDevice::GetTemperatureControlState(ALTempCtrlType_t Type)
{
    ReturnCode_t retCode = m_pTempCtrls[Type]->ReqStatus();
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
        m_CurrentTempCtrlStatus[Type] = TEMPCTRL_STATUS_UNDEF;
        return TEMPCTRL_STATE_ERROR;
    }
    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_GET_TEMP_CTRL_STATE);
    TempCtrlState_t controlstate;
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        controlstate = TEMPCTRL_STATE_ERROR;
    }
    else if (IsTemperatureControlOn(Type))
    {
        if (IsInsideRange(Type))
        {
            controlstate = TEMPCTRL_STATE_INSIDE_RANGE;
        }
        else if (IsOutsideRange(Type))
        {
            controlstate = TEMPCTRL_STATE_OUTSIDE_RANGE;
        }
    }
    else if (IsTemperatureControlOff(Type))
    {
        controlstate = TEMPCTRL_STATE_OFF;
    }
    return controlstate;
}

void CAirLiquidDevice::OnTempControlStatus(quint32 InstanceID, ReturnCode_t ReturnCode,
                                           TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        m_CurrentTempCtrlStatus[m_InstTCTypeMap[InstanceID]] = TempCtrlStatus;
        m_MainsVoltageStatus[m_InstTCTypeMap[InstanceID]] = MainsVoltage;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_GET_TEMP_CTRL_STATE, ReturnCode);
}

bool CAirLiquidDevice::IsInsideRange(ALTempCtrlType_t Type)
{
    if(GetTemperature(Type, 0) != UNDEFINED)
    {
        if((m_TargetTemperatures[Type] != UNDEFINED) || (m_CurrentTemperatures[Type] != UNDEFINED))
        {
            if ((m_CurrentTemperatures[Type] > m_TargetTemperatures[Type] - TOLERANCE)||
                            (m_CurrentTemperatures[Type] < m_TargetTemperatures[Type] + TOLERANCE))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    //Log(tr("Error"));
    return false;
}

bool CAirLiquidDevice::IsOutsideRange(ALTempCtrlType_t Type)
{
    if(GetTemperature(Type, 0) != UNDEFINED)
    {
        if((m_TargetTemperatures[Type] != UNDEFINED) || (m_CurrentTemperatures[Type] != UNDEFINED))
        {
            if ((m_CurrentTemperatures[Type] < m_TargetTemperatures[Type] - TOLERANCE)||
                            (m_CurrentTemperatures[Type] > m_TargetTemperatures[Type] + TOLERANCE))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    //   Log(tr("Error"));
    return false;
}

bool CAirLiquidDevice::IsTemperatureControlOn(ALTempCtrlType_t Type)
{
    return (m_CurrentTempCtrlStatus[Type] == TEMPCTRL_STATUS_ON);
}

bool CAirLiquidDevice::IsTemperatureControlOff(ALTempCtrlType_t Type)
{
    return (m_CurrentTempCtrlStatus[Type] == TEMPCTRL_STATUS_OFF);
}

bool CAirLiquidDevice::SetTemperature(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    m_TargetTemperatures[Type] = NominalTemperature;
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        retCode = m_pTempCtrls[Type]->SetTemperature(NominalTemperature, SlopeTempChange);
    }
    else
    {
        return false;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return false;
    }
    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_SET_TEMP);
    return (DCL_ERR_FCT_CALL_SUCCESS == retCode);
}

void CAirLiquidDevice::OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature)
{
    Q_UNUSED(Temperature)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: AL Set temperature successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL set temperature failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_TEMP, ReturnCode);
}

qreal CAirLiquidDevice::GetTemperature(ALTempCtrlType_t Type, quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue = m_CurrentTemperatures[Type];
    if((Now - m_LastGetTempTime[Type][Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        ReturnCode_t retCode = m_pTempCtrls[Type]->ReqActTemperature(Index);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode )
        {
            RetValue = UNDEFINED;
        }
        else
        {
            retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_GET_TEMP);
            if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                RetValue = UNDEFINED;
            }
            else
            {
                RetValue = m_CurrentTemperatures[Type];
            }
            m_LastGetTempTime[Type][Index] = Now;
        }
    }
    return RetValue;
}

bool CAirLiquidDevice::GetTemperatureAsync(ALTempCtrlType_t Type, quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetTempTime[Type][Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        m_LastGetTempTime[Type][Index] = Now;
        return ( DCL_ERR_FCT_CALL_SUCCESS== m_pTempCtrls[Type]->ReqActTemperature(Index));
    }
    return true;
}

void CAirLiquidDevice::OnGetTemp(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Index, qreal Temp)
{
    Q_UNUSED(Index)

    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: AL Get temperature successful! ";
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]] = Temp;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL get temperature failed! " << ReturnCode;
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]] = UNDEFINED;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_GET_TEMP, ReturnCode);

}

ReturnCode_t CAirLiquidDevice::SetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Set AL temperature PID, type: " << Type;
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        retCode = m_pTempCtrls[Type]->SetTemperaturePid(MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        return m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_SET_TEMP_PID);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

void CAirLiquidDevice::OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    Q_UNUSED(MaxTemperature)
    Q_UNUSED(ControllerGain)
    Q_UNUSED(ResetTime)
    Q_UNUSED(DerivativeTime)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: AL Set temperature PID successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL set temperature PID failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_TEMP_PID, ReturnCode);
}

bool CAirLiquidDevice::SetDOValue(quint16 OutputValue, quint16 Duration, quint16 Delay)
{
    m_TargetDOOutputValue = OutputValue;

    ReturnCode_t retCode = m_pFanDigitalOutput->SetOutputValue(m_TargetDOOutputValue, Duration, Delay);
    if (DCL_ERR_FCT_CALL_SUCCESS == retCode)
    {
        return false;
    }
    return (DCL_ERR_FCT_CALL_SUCCESS == m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_SET_DO_VALVE));
}

void CAirLiquidDevice::OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue)
{
    Q_UNUSED(OutputValue)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: AL Set DO output successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL set DO output failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_DO_VALVE, ReturnCode);

}

ReturnCode_t CAirLiquidDevice::TurnOnFan()
{
    if (SetDOValue(1, 0, 0))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}

ReturnCode_t CAirLiquidDevice::TurnOffFan()
{
    if(SetDOValue(0, 0, 0))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}

ReturnCode_t CAirLiquidDevice::BreakAllOperation(void)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start break all operation procedure";

    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_PRESSURE, DCL_ERR_UNEXPECTED_BREAK);
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_DRAINING, DCL_ERR_UNEXPECTED_BREAK);
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_RELEASE_PRESSURE, DCL_ERR_UNEXPECTED_BREAK);
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, DCL_ERR_UNEXPECTED_BREAK);
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_VACCUM, DCL_ERR_UNEXPECTED_BREAK);

    //close both valve
    m_pPressureCtrl->SetPressure(0, 10);

    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Close Both Valves";
    m_pPressureCtrl->SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
    m_pPressureCtrl->SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
    m_pFanDigitalOutput->SetOutputValue(0, 0, 0);
    return DCL_ERR_FCT_CALL_SUCCESS;
}
} //namespace
