#include "DeviceControl/Include/Devices/OvenDevice.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include <sys/stat.h>
#include <QtDebug>
#include "DeviceControl/Include/DeviceProcessing/DeviceLifeCycleRecord.h"

namespace DeviceControl
{
#define CHECK_SENSOR_TIME (200) // in msecs
const qint32 TOLERANCE = 10; //!< tolerance value for calculating inside and outside range

/****************************************************************************/
/*!
 *  \brief    Constructor of the COvenDevice class
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 */
/****************************************************************************/
COvenDevice::COvenDevice(DeviceProcessing* pDeviceProcessing, QString& Type,
                         const DeviceModuleList_t &ModuleList,
                         quint32 InstanceID) :
    CBaseDevice(pDeviceProcessing, Type, ModuleList, InstanceID)
{
    Reset();
    FILE_LOG_L(laDEV, llINFO) << "Oven device created";
}//lint !e1566

/****************************************************************************/
/*!
 *  \brief  Destructor of COvenDevice
 */
/****************************************************************************/
COvenDevice::~COvenDevice()
{
    try
    {
        Reset();
    }
    catch(...)
    {
        return;
    }
}//lint !e1579

/****************************************************************************/
/*!
 *  \brief  Reset class member variable
 */
/****************************************************************************/
void COvenDevice::Reset()
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_ErrorTaskState   = OVEN_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_OVEN;
    m_LastGetLidStatusTime = 0;
    memset( &m_LastGetTempTime, 0 , sizeof(m_LastGetTempTime)); //lint !e545
    memset( &m_LastGetTempCtrlStatus, 0 , sizeof(m_LastGetTempCtrlStatus)); //lint !e545
    memset( &m_TargetTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_TargetTempCtrlStatus)); //lint !e545 !e641
    memset( &m_CurrentTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_CurrentTempCtrlStatus)); //lint !e545 !e641
    memset( &m_CurrentTemperatures, 0 , sizeof(m_CurrentTemperatures)); //lint !e545 !e641
    memset( &m_TargetTemperatures, 0 , sizeof(m_TargetTemperatures)); //lint !e545
    memset( &m_MainsVoltageStatus, 0 , sizeof(m_MainsVoltageStatus)); //lint !e545
    memset( &m_pTempCtrls, 0 , sizeof(m_pTempCtrls));  //lint !e545
    memset( &m_TCHardwareStatus, 0 , sizeof(m_TCHardwareStatus)); //lint !e545
    memset( &m_LastGetTCCurrentTime, 0 , sizeof(m_LastGetTCCurrentTime)); //lint !e545
    m_pLidDigitalInput = NULL;
    m_LidStatus = 0;
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
void COvenDevice::HandleTasks()
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
            m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;
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
            m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   COvenDevice:handleTasks, new state: " << (int) m_MainState;
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
void COvenDevice::HandleIdleState()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if(now > (m_LastSensorCheckTime + MINIMUM_CHECK_SENSOR_T))
    {
        CheckSensorsData();
        m_LastSensorCheckTime = now;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CBaseModule* pBaseModule = NULL;

    FILE_LOG_L(laDEV, llINFO) << "  COvenDevice::HandleInitializationState()";
    quint32 InstanceID;
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_OvenTopTempCtrlKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pTempCtrls[OVEN_TOP] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pTempCtrls[OVEN_TOP] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_OVEN_TOPTEMPCTRL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstTCTypeMap[CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL] = OVEN_TOP;  //lint !e641
            if (m_ModuleLifeCycleRecord)
            {
                PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value("temp_oven_top");
                if (pPartLifeCycleRecord)
                {
                    m_pTempCtrls[OVEN_TOP]->SetPartLifeCycleRecord(pPartLifeCycleRecord);
                }
            }
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_OvenBottomTempCtrlKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pTempCtrls[OVEN_BOTTOM] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pTempCtrls[OVEN_BOTTOM] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_OVEN_BOTTOMTEMPCTRL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstTCTypeMap[CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL] = OVEN_BOTTOM;  //lint !e641
            if (m_ModuleLifeCycleRecord)
            {
                PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value("temp_oven_bottom");
                if (pPartLifeCycleRecord)
                {
                    m_pTempCtrls[OVEN_BOTTOM]->SetPartLifeCycleRecord(pPartLifeCycleRecord);
                }
            }
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_OvenLidDIKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pLidDigitalInput = (CDigitalInput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_OvenLidDIKey));
        if(m_pLidDigitalInput == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_LIDDI);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_LOCKDI not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        if (m_ModuleLifeCycleRecord)
        {
            PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value("Oven_Cover_Sensor");
            if (pPartLifeCycleRecord)
            {
                quint32 lifeCycle = pPartLifeCycleRecord->m_ParamMap.value(m_pLidDigitalInput->GetKey() + "_LifeCycle").toUInt();
                m_pLidDigitalInput->SetLifeCycle(lifeCycle);
                m_pLidDigitalInput->SetPartLifeCycleRecord(pPartLifeCycleRecord);
            }
        }
    }
    else
    {
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
 */
/****************************************************************************/
ReturnCode_t COvenDevice::HandleConfigurationState()
{

    if(!connect(m_pTempCtrls[OVEN_TOP], SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportRefTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[OVEN_BOTTOM], SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportRefTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[OVEN_TOP], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[OVEN_BOTTOM], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }


    if(!connect(m_pTempCtrls[OVEN_TOP], SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[OVEN_BOTTOM], SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[OVEN_TOP], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[OVEN_BOTTOM], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[OVEN_TOP], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[OVEN_BOTTOM], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[OVEN_TOP], SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
                this, SLOT(OnTCGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[OVEN_BOTTOM], SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
                this, SLOT(OnTCGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pLidDigitalInput, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
            this, SLOT(OnGetDIValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_LIDDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportActInputValue'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pLidDigitalInput, SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_OVEN_LIDDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Read Oven device's sensors data asynchronizely
 */
/****************************************************************************/
void COvenDevice::CheckSensorsData()
{
    if(m_pTempCtrls[OVEN_TOP])
    {
        (void)GetTemperatureAsync(OVEN_TOP, 0);
        (void)GetHeaterCurrentAsync(OVEN_TOP);
        (void)GetTemperatureControlStatusAsync(OVEN_TOP);
    }
    if(m_pTempCtrls[OVEN_BOTTOM])
    {
        (void)GetTemperatureAsync(OVEN_BOTTOM, 0);
        (void)GetTemperatureAsync(OVEN_BOTTOM, 1);
        (void)GetHeaterCurrentAsync(OVEN_BOTTOM);
        (void)GetTemperatureControlStatusAsync(OVEN_BOTTOM);
    }
    if(m_pLidDigitalInput)
    {
        (void)GetLidStatusAsync();
    }
}


/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::ConfigureDeviceTasks()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}
void COvenDevice::HandleErrorState()
{
    if(m_ErrorTaskState == OVEN_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == OVEN_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIOvenDev != 0)
    {
    m_pIOvenDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
    }*/
        m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == OVEN_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == OVEN_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pTempCtrls[OVEN_TOP] = 0;
        m_pTempCtrls[OVEN_BOTTOM] = 0;
    }
}

/****************************************************************************/
/*!
 *  \brief    Set temperature control's status.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  TempCtrlStatus = New temperature control status.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::SetTemperatureControlStatus(OVENTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus)
{
    m_TargetTempCtrlStatus[Type] = TempCtrlStatus;
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        retCode = m_pTempCtrls[Type]->SetStatus(TempCtrlStatus);
        return retCode;
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/*!
 *  \brief   Enable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::SetTempCtrlON(OVENTempCtrlType_t Type)
{
    return SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_ON);
}

/****************************************************************************/
/*!
 *  \brief   Disable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::SetTempCtrlOFF(OVENTempCtrlType_t Type)
{
    return SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_OFF);
}

/****************************************************************************/
/*!
 *  \brief   Get the temperature sensor data captured in last 500 milliseconds.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  Index = Actual temperature sensor index.
 *
 *  \return  Actual temperature, UNDEFINED if failed.
 */
/****************************************************************************/
qreal COvenDevice::GetRecentTemperature(OVENTempCtrlType_t Type, quint8 Index)
{
   // QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue;
    if((Now - m_LastGetTempTime[Type][Index]) <= 500) // check if 500 msec has passed since last read
    {
        RetValue = m_CurrentTemperatures[Type][Index];
    }
    else
    {
        RetValue = UNDEFINED_4_BYTE;
    }
    return RetValue;
}

bool COvenDevice::GetRecentHeatingStatus(OVENTempCtrlType_t Type)
{
    return m_CurrentTempCtrlStatus[Type] == TEMPCTRL_STATUS_ON;
}

/****************************************************************************/
/*!
 *  \brief   Get recent current of sensor captured in last 500 milliseconds.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  Actual Current, UNDEFINED if failed.
 */
/****************************************************************************/
quint32 COvenDevice::GetRecentCurrent(OVENTempCtrlType_t Type)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint32 RetValue;
    if((Now - m_LastGetTCCurrentTime[Type]) <= 500) // check if 500 msec has passed since last read
    {
        RetValue = m_TCHardwareStatus[Type].Current;
    }
    else
    {
        RetValue = UNDEFINED_4_BYTE;
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Start temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::StartTemperatureControl(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    m_TargetTemperatures[Type] = NominalTemperature;
    m_TargetTempCtrlStatus[Type] = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState(Type) == TEMPCTRL_STATE_ERROR)
    {
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn(Type))
    {
        return DCL_ERR_DEV_TEMP_CTRL_ALREADY_ON;
    }
    if (IsTemperatureControlOff(Type))
    {
        //Set the nominal temperature
        ReturnCode_t retCode = SetTemperature(Type, NominalTemperature, SlopeTempChange);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        //ON the temperature control
        retCode = SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_ON);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Start temperature control with PID parameters.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::StartTemperatureControlWithPID(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    ReturnCode_t retCode;
    m_TargetTemperatures[Type] = NominalTemperature;
    m_TargetTempCtrlStatus[Type] = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState(Type) == TEMPCTRL_STATE_ERROR)
    {
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn(Type))
    {
        if(DCL_ERR_FCT_CALL_SUCCESS != SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_OFF))
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
    retCode = SetTemperature(Type, NominalTemperature, SlopeTempChange);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    //ON the temperature control
//    retCode = SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_ON);
//    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
//    {
//        return retCode;
//    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Get temperature control module's status.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  Temperature control module status.
 */
/****************************************************************************/
TempCtrlState_t COvenDevice::GetTemperatureControlState(OVENTempCtrlType_t Type)
{
    TempCtrlState_t controlstate = TEMPCTRL_STATE_ERROR;
    if(m_pTempCtrls[Type] && m_pDevProc)
    {
        ReturnCode_t retCode = m_pTempCtrls[Type]->ReqStatus();
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
            m_CurrentTempCtrlStatus[Type] = TEMPCTRL_STATUS_UNDEF;
            return TEMPCTRL_STATE_ERROR;
        }
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_OVEN_GET_TEMP_CTRL_STATE);
        if (DCL_ERR_FCT_CALL_SUCCESS == retCode)
        {
            if (IsTemperatureControlOn(Type))
            {
                if (IsInsideRange(Type, 0))
                {
                    controlstate = TEMPCTRL_STATE_INSIDE_RANGE;
                }
                else if (IsOutsideRange(Type, 0))
                {
                    controlstate = TEMPCTRL_STATE_OUTSIDE_RANGE;
                }
            }
            else if (IsTemperatureControlOff(Type))
            {
                controlstate = TEMPCTRL_STATE_OFF;
            }
        }
    }
    return controlstate;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with get temperature control status.
 *
 *  This slot is connected to the signal, ReportActStatus
 *
 *  \iparam InstanceID = Instance ID of the function module
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam TempCtrlStatus = Actual temperature control status
 *  \iparam MainsVoltage = Main voltage status.
 *
 */
/****************************************************************************/
void COvenDevice::OnTempControlStatus(quint32 InstanceID, ReturnCode_t ReturnCode,
                                           TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        m_CurrentTempCtrlStatus[m_InstTCTypeMap[InstanceID]] = TempCtrlStatus;
        m_MainsVoltageStatus[m_InstTCTypeMap[InstanceID]] = MainsVoltage;
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_OVEN_GET_TEMP_CTRL_STATE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature is inside the range.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \param Index =  quint8 type parameter
 *
 *  \return  True if is inside the range, else not.
 */
/****************************************************************************/
bool COvenDevice::IsInsideRange(OVENTempCtrlType_t Type, quint8 Index)
{
    if(GetTemperature(Type, 0) != UNDEFINED_4_BYTE)
    {
        if((m_TargetTemperatures[Type] != UNDEFINED_4_BYTE) || (m_CurrentTemperatures[Type][Index] != UNDEFINED_4_BYTE))
        {
            if ((m_CurrentTemperatures[Type][Index] > m_TargetTemperatures[Type] - TOLERANCE)||
                            (m_CurrentTemperatures[Type][Index] < m_TargetTemperatures[Type] + TOLERANCE))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature is outside the range.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \param Index =  quint8 type parameter
 *
 *  \return  True if is outside the range, else not.
 */
/****************************************************************************/
bool COvenDevice::IsOutsideRange(OVENTempCtrlType_t Type, quint8 Index)
{
    if(GetTemperature(Type, 0) != UNDEFINED_4_BYTE)
    {
        if((m_TargetTemperatures[Type] != UNDEFINED_4_BYTE) || (m_CurrentTemperatures[Type][Index] != UNDEFINED_4_BYTE))
        {
            if ((m_CurrentTemperatures[Type][Index] < m_TargetTemperatures[Type] - TOLERANCE)||
                            (m_CurrentTemperatures[Type][Index] > m_TargetTemperatures[Type] + TOLERANCE))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature control is enabled.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  True if temperature control is enabled, else not.
 */
/****************************************************************************/
bool COvenDevice::IsTemperatureControlOn(OVENTempCtrlType_t Type)
{
    return (m_CurrentTempCtrlStatus[Type] == TEMPCTRL_STATUS_ON);
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature control is disabled.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  True if temperature control is disabled, else not.
 */
/****************************************************************************/
bool COvenDevice::IsTemperatureControlOff(OVENTempCtrlType_t Type)
{
    return (m_CurrentTempCtrlStatus[Type] == TEMPCTRL_STATUS_OFF);
}

/****************************************************************************/
/*!
 *  \brief   Start temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::SetTemperature(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    m_TargetTemperatures[Type] = NominalTemperature;
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        retCode = m_pTempCtrls[Type]->SetTemperature(NominalTemperature, SlopeTempChange);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    if(m_pDevProc)
    {
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_OVEN_SET_TEMP);
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set temperature.
 *
 *  This slot is connected to the signal, ReportRefTemperature
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam Temperature = Target temperature.
 *
 */
/****************************************************************************/
void COvenDevice::OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature)
{
    Q_UNUSED(Temperature)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Oven Set temperature successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Oven set temperature failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_OVEN_SET_TEMP, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   Get temperature synchronously.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  Index = Index of the target temperature control module.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
qreal COvenDevice::GetTemperature(OVENTempCtrlType_t Type, quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue = m_CurrentTemperatures[Type][Index];
    if((Now - m_LastGetTempTime[Type][Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        ReturnCode_t retCode = m_pTempCtrls[Type]->ReqActTemperature(Index);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode )
        {
            RetValue = UNDEFINED_4_BYTE;
        }
        else
        {
            if(m_pDevProc)
            {
                retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_OVEN_GET_TEMP);
            }
            if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                RetValue = UNDEFINED_4_BYTE;
            }
            else
            {
                RetValue = m_CurrentTemperatures[Type][Index];
            }
            m_LastGetTempTime[Type][Index] = Now;
        }
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief    Get actual temperature of Air-liquid device asynchronously.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  Index = Index of the target temperature control module.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::GetTemperatureAsync(OVENTempCtrlType_t Type, quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetTempTime[Type][Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        m_LastGetTempTime[Type][Index] = Now;
        return   m_pTempCtrls[Type]->ReqActTemperature(Index);
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t COvenDevice::GetTemperatureControlStatusAsync(OVENTempCtrlType_t Type)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetTempCtrlStatus[Type]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        m_LastGetTempCtrlStatus[Type] = Now;
        return   m_pTempCtrls[Type]->ReqStatus();
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}
/****************************************************************************/
/*!
 *  \brief   slot associated with get temperature.
 *
 *  This slot is connected to the signal, ReportActTemperature
 *
 *  \iparam InstanceID = Instance ID of the function module
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam Index = Index of the actual temperature control module.
 *  \iparam Temp = Actual temperature.
 *
 */
/****************************************************************************/
void COvenDevice::OnGetTemp(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Index, qreal Temp)
{
    Q_UNUSED(Index)

    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Oven Get temperature successful! ";
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]][Index] = Temp;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Oven get temperature failed! " << ReturnCode; //lint !e641
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]][Index] = UNDEFINED_4_BYTE;
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_OVEN_GET_TEMP, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   Set PID parameters for temperature control module.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::SetTemperaturePid(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Set Oven temperature PID, type: " << Type; //lint !e641
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        retCode = m_pTempCtrls[Type]->SetTemperaturePid(MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        if(m_pDevProc)
        {
            retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_OVEN_SET_TEMP_PID);
        }
        return retCode;
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set PID parameters.
 *
 *  This slot is connected to the signal, ReportSetPidAckn
 *
 *  \iparam  ReturnCode = ReturnCode of function level Layer
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 */
/****************************************************************************/
void COvenDevice::OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    Q_UNUSED(MaxTemperature)
    Q_UNUSED(ControllerGain)
    Q_UNUSED(ResetTime)
    Q_UNUSED(DerivativeTime)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Oven Set temperature PID successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Oven set temperature PID failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_OVEN_SET_TEMP_PID, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Get Oven Lid status.
 *
 *  \return  1: Open, 0: Closed
 */
/****************************************************************************/
quint16 COvenDevice::GetLidStatus()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_FAILED;
    if(m_pLidDigitalInput)
    {
        retCode = m_pLidDigitalInput->ReqActInputValue();
    }
    else
    {
        return UNDEFINED_2_BYTE;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED_2_BYTE;
    }
    if(m_pDevProc)
    {
        retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_OVEN_GET_DI_VALUE);
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED_2_BYTE;
    }
    return m_LidStatus;
}

/****************************************************************************/
/*!
 *  \brief  Get Oven Lid status asynchronously.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t COvenDevice::GetLidStatusAsync()
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetLidStatusTime) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        m_LastGetLidStatusTime = Now;
        if(m_pLidDigitalInput)
        {
            return m_pLidDigitalInput->ReqActInputValue();
        }
        else
        {
            return DCL_ERR_NOT_INITIALIZED;
        }
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with get digital input value.
 *
 *  This slot is connected to the signal, ReportActInputValue
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam InputValue = Actual digital input value.
 *
 */
/****************************************************************************/
void COvenDevice::OnGetDIValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 InputValue)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Oven Get DI value successful! ";
        m_LidStatus = InputValue;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Oven Get DI value failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_OVEN_GET_DI_VALUE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   Get the Oven lid sensor data captured in last 500 milliseconds.
 *
 *  \return  Actual lid status, UNDEFINED if failed.
 */
/****************************************************************************/
quint16 COvenDevice::GetRecentOvenLidStatus()
{
    // QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint16 RetValue;
    if((Now - m_LastGetLidStatusTime) <= 500) // check if 500 msec has passed since last read
    {
        RetValue = m_LidStatus;
    }
    else
    {
        RetValue = UNDEFINED_2_BYTE;
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Get actual current of device's heater.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  The current of heater in mA.
 */
/****************************************************************************/
ReturnCode_t COvenDevice::GetHeaterCurrentAsync(OVENTempCtrlType_t Type)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetTCCurrentTime[Type]) >= CHECK_SENSOR_TIME && m_pTempCtrls[Type] != NULL)
    {
        m_LastGetTCCurrentTime[Type] = Now;
        return m_pTempCtrls[Type]->GetHardwareStatus();
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function GetHeaterSwitchType
 *
 *  \return from Heater switch type
 */
/****************************************************************************/
quint8 COvenDevice::GetHeaterSwitchType()
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint8 RetValue = UNDEFINED_1_BYTE;
    if(m_pTempCtrls[OVEN_TOP] != NULL)
    {
        if((Now - m_LastGetTCCurrentTime[OVEN_TOP]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
        {
            ReturnCode_t retCode = m_pTempCtrls[OVEN_TOP]->GetHardwareStatus();
            if (DCL_ERR_FCT_CALL_SUCCESS == retCode )
            {
                if(m_pDevProc)
                {
                    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_OVEN_TC_GET_HW_STATUS);
                }
                if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
                {
                    RetValue = UNDEFINED_1_BYTE;
                }
                else
                {
                    RetValue = m_TCHardwareStatus[OVEN_TOP].HeaterSwitchType;
                }
                m_LastGetTCCurrentTime[OVEN_TOP] = Now;
            }
        }
        else
        {
            RetValue = m_TCHardwareStatus[OVEN_TOP].HeaterSwitchType;
        }
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief  slot for getting the hardware information
 *
 *  This slot is connected to the signal ReportHardwareStatus
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam Sensors = Number of temperature sensors connected to the board
 *  \iparam Fans = Number of ventilation fans connected to the board
 *  \iparam Heaters = Number of heating elements connected to the board
 *  \iparam Pids = Number of PID controllers in the control loop
 *  \iparam Current = Current through the heatinf circuit in milliamperes
 *  \param HeaterSwitchType
 *  \param InstanceID
 */
/****************************************************************************/
void COvenDevice::OnTCGetHardwareStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Sensors, quint8 Fans,
                                               quint8 Heaters, quint8 Pids, quint16 Current, quint8 HeaterSwitchType)
{
    if (DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Sensors = Sensors;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Fans = Fans;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Heaters = Heaters;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Pids = Pids;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Current = Current;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].HeaterSwitchType = HeaterSwitchType;
    }
    else
    {
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Sensors = 0;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Fans = 0;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Heaters = 0;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Pids = 0;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].Current = UNDEFINED_2_BYTE;
        m_TCHardwareStatus[m_InstTCTypeMap[InstanceID]].HeaterSwitchType = 0;
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_OVEN_TC_GET_HW_STATUS, ReturnCode);
    }
}

} //namespace
