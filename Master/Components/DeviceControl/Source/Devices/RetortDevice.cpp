#include "DeviceControl/Include/Devices/RetortDevice.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include <sys/stat.h>
#include <QtDebug>
#include <unistd.h>

namespace DeviceControl
{
#define UNDEFINED (999)
#define CHECK_SENSOR_TIME (200) // in msecs
const qint32 TOLERANCE = 10; //!< tolerance value for calculating inside and outside range
CRetortDevice::CRetortDevice(DeviceProcessing* pDeviceProcessing, QString Type) : CBaseDevice(pDeviceProcessing, Type)
{
    Reset();
    FILE_LOG_L(laDEV, llINFO) << "Retort device created";
}

CRetortDevice::~CRetortDevice()
{
    Reset();
}
void CRetortDevice::Reset()
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_ErrorTaskState   = RT_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_RETORT;

    m_pLockDigitalOutput = NULL;
    m_pLockDigitalInput = NULL;
    memset( &m_LastGetTempTime, 0 , sizeof(m_LastGetTempTime));
    memset( &m_TargetTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_TargetTempCtrlStatus));
    memset( &m_CurrentTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_CurrentTempCtrlStatus));
    memset( &m_CurrentTemperatures, 0 , sizeof(m_CurrentTemperatures));
    memset( &m_TargetTemperatures, 0 , sizeof(m_TargetTemperatures));
    memset( &m_MainsVoltageStatus, 0 , sizeof(m_MainsVoltageStatus));
    memset( &m_pTempCtrls, 0 , sizeof(m_pTempCtrls));
    m_TargetDOOutputValue = 0;
    m_LockStatus = 0;
    m_LastGetLockStatusTime = 0;
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
void CRetortDevice::HandleTasks()
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
            m_ErrorTaskState = RT_DEV_ERRTASK_STATE_REPORT_IFACE;
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
            m_ErrorTaskState = RT_DEV_ERRTASK_STATE_REPORT_IFACE;
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
            m_ErrorTaskState = RT_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CRetortDevice:handleTasks, new state: " << (int) m_MainState;
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
void CRetortDevice::HandleIdleState()
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
ReturnCode_t CRetortDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    FILE_LOG_L(laDEV, llINFO) << "  CRetortDevice::HandleInitializationState()";

    m_pTempCtrls[RT_BOTTOM] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_RetortBottomTempCtrlKey));
    if(m_pTempCtrls[RT_BOTTOM] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_BOTTOMTEMPCTRL not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
       // m_InstTCTypeMap[((CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL & 0xF)] = RT_BOTTOM;
        m_InstTCTypeMap[CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL] = RT_BOTTOM;
    }

    m_pTempCtrls[RT_SIDE] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_RetortSideTempCtrlKey));
    if(m_pTempCtrls[RT_SIDE] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_SIDETEMPCTRL not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
       // m_InstTCTypeMap[ ((CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL & 0xFFF0)<<4)|(CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL & 0xF)] = RT_SIDE;
        m_InstTCTypeMap[ CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL] = RT_SIDE;
    }

    m_pLockDigitalOutput = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_RetortLockDOKey));
    if(m_pLockDigitalOutput == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDO);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_LOCKDO not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pLockDigitalInput = (CDigitalInput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_RetortLockDIKey));
    if(m_pLockDigitalInput == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDI);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_LOCKDI not allocated.";
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
ReturnCode_t CRetortDevice::HandleConfigurationState()
{

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportRefTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportRefTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }


    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pLockDigitalOutput, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
            this, SLOT(OnGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
            this, SLOT(OnGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pLockDigitalInput, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
            this, SLOT(OnGetDIValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportActInputValue'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pLockDigitalInput, SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;

}

void CRetortDevice::CheckSensorsData()
{

    //  qDebug() <<  "AL timer thread id is " << QThread::currentThreadId();

    if(m_pTempCtrls[RT_BOTTOM])
    {
        GetTemperatureAsync(RT_BOTTOM, 0);
    }
    if(m_pTempCtrls[RT_SIDE])
    {
        GetTemperatureAsync(RT_SIDE, 0);
    }
    if(m_pLockDigitalInput)
    {
        GetLockStatusAsync();
    }
}


/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CRetortDevice::ConfigureDeviceTasks()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}
void CRetortDevice::HandleErrorState()
{
    if(m_ErrorTaskState == RT_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == RT_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIOvenDev != 0)
    {
    m_pIOvenDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
    }*/
        m_ErrorTaskState = RT_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == RT_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = RT_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == RT_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pTempCtrls[RT_BOTTOM] = 0;
        m_pTempCtrls[RT_SIDE] = 0;
        m_pLockDigitalOutput= 0;
    }
}


bool CRetortDevice::SetTemperatureControlStatus(RTTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus)
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
ReturnCode_t CRetortDevice::SetTempCtrlON(RTTempCtrlType_t Type)
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

ReturnCode_t CRetortDevice::SetTempCtrlOFF(RTTempCtrlType_t Type)
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
qreal CRetortDevice::GetRecentTemperature(RTTempCtrlType_t Type, quint8 Index)
{
   // QMutexLocker Locker(&m_Mutex);
//quint32 thisadd =(quint32)this;
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

ReturnCode_t CRetortDevice::StartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
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

ReturnCode_t CRetortDevice::StartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
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

TempCtrlState_t CRetortDevice::GetTemperatureControlState(RTTempCtrlType_t Type)
{
    ReturnCode_t retCode = m_pTempCtrls[Type]->ReqStatus();
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
        m_CurrentTempCtrlStatus[Type] = TEMPCTRL_STATUS_UNDEF;
        return TEMPCTRL_STATE_ERROR;
    }
    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_GET_TEMP_CTRL_STATE);
    TempCtrlState_t controlstate = TEMPCTRL_STATE_ERROR;
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
void CRetortDevice::OnTempControlStatus(quint32 InstanceID, ReturnCode_t ReturnCode,
                                           TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        m_CurrentTempCtrlStatus[m_InstTCTypeMap[InstanceID]] = TempCtrlStatus;
        m_MainsVoltageStatus[m_InstTCTypeMap[InstanceID]] = MainsVoltage;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_GET_TEMP_CTRL_STATE, ReturnCode);
}

bool CRetortDevice::IsInsideRange(RTTempCtrlType_t Type)
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

bool CRetortDevice::IsOutsideRange(RTTempCtrlType_t Type)
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

bool CRetortDevice::IsTemperatureControlOn(RTTempCtrlType_t Type)
{
    return (m_CurrentTempCtrlStatus[Type] == TEMPCTRL_STATUS_ON);
}

bool CRetortDevice::IsTemperatureControlOff(RTTempCtrlType_t Type)
{
    return (m_CurrentTempCtrlStatus[Type] == TEMPCTRL_STATUS_OFF);
}

bool CRetortDevice::SetTemperature(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
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
    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_SET_TEMP);
    return (DCL_ERR_FCT_CALL_SUCCESS == retCode);
}

void CRetortDevice::OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature)
{
    Q_UNUSED(Temperature)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Set temperature successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort set temperature failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_SET_TEMP, ReturnCode);
}

qreal CRetortDevice::GetTemperature(RTTempCtrlType_t Type, quint8 Index)
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
            retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_GET_TEMP);
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
bool CRetortDevice::GetTemperatureAsync(RTTempCtrlType_t Type, quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetTempTime[Type][Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        m_LastGetTempTime[Type][Index] = Now;
        return ( DCL_ERR_FCT_CALL_SUCCESS== m_pTempCtrls[Type]->ReqActTemperature(Index));
    }
    return true;
}


void CRetortDevice::OnGetTemp(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Index, qreal Temp)
{
    Q_UNUSED(Index)

    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Get temperature successful! ";
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]] = Temp;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort get temperature failed! " << ReturnCode;
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]] = UNDEFINED;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_GET_TEMP, ReturnCode);

}

ReturnCode_t CRetortDevice::SetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Set Retort temperature PID, type: " << Type;
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        retCode = m_pTempCtrls[Type]->SetTemperaturePid(MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        return m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_SET_TEMP_PID);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
void CRetortDevice::OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    Q_UNUSED(MaxTemperature)
    Q_UNUSED(ControllerGain)
    Q_UNUSED(ResetTime)
    Q_UNUSED(DerivativeTime)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Set temperature PID successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort set temperature PID failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_SET_TEMP_PID, ReturnCode);
}

bool CRetortDevice::SetDOValue(quint16 OutputValue, quint16 Duration, quint16 Delay)
{
    m_TargetDOOutputValue = OutputValue;

    ReturnCode_t retCode = m_pLockDigitalOutput->SetOutputValue(m_TargetDOOutputValue, Duration, Delay);
    if (DCL_ERR_FCT_CALL_SUCCESS == retCode)
    {
        return false;
    }
    return (DCL_ERR_FCT_CALL_SUCCESS == m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_SET_DO_VALUE));
}
void CRetortDevice::OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue)
{
    Q_UNUSED(OutputValue)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Set DO output successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL set DO output failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_SET_DO_VALUE, ReturnCode);

}
ReturnCode_t CRetortDevice::Lock()
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
ReturnCode_t CRetortDevice::Unlock()
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
void CRetortDevice::OnGetDIValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 InputValue)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Get DI value successful! ";
        m_LockStatus = InputValue;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort Get DI value failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_GET_DI_VALUE, ReturnCode);
}

quint16 CRetortDevice::GetRecentRetortLockStatus()
{
   // QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue;
    if((Now - m_LastGetLockStatusTime) <= 500) // check if 500 msec has passed since last read
    {
        RetValue = m_LockStatus;
    }
    else
    {
        RetValue = UNDEFINED;
    }
    return RetValue;
}

bool CRetortDevice::GetLockStatusAsync()
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetLockStatusTime) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        m_LastGetLockStatusTime = Now;
        return ( DCL_ERR_FCT_CALL_SUCCESS== m_pLockDigitalInput->ReqActInputValue());
    }
    return true;
}

quint16 CRetortDevice::GetLidStatus()
{
    //Log(tr("GetValue"));
    ReturnCode_t retCode = m_pLockDigitalInput->ReqActInputValue();
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED;
    }
    retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_GET_DI_VALUE);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED;
    }
    return m_LockStatus;
}

TempCtrlHardwareStatus_t *CRetortDevice::GetHardwareStatus(RTTempCtrlType_t Type)
{
    if(m_pTempCtrls[Type] != NULL)
    {
        ReturnCode_t retCode = m_pTempCtrls[Type]->GetHardwareStatus();
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return NULL;
        }
        retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_GET_HW_STATUS);
        if(DCL_ERR_FCT_CALL_SUCCESS == retCode)
        {
            return &m_HardwareStatus[Type];
        }
    }
    return NULL;
}

void CRetortDevice::OnGetHardwareStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Sensors, quint8 Fans,
                                               quint8 Heaters, quint8 Pids, quint16 Current, quint8 HeaterSwitchType)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Get DI value successful! ";
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Sensors = Sensors;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Fans = Fans;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Heaters = Heaters;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Pids = Pids;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Current = Current;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].HeaterSwitchType = HeaterSwitchType;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort Get DI value failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_GET_HW_STATUS, ReturnCode);
}
} //namespace
