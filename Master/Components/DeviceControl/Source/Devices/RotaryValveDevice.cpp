
#include "DeviceControl/Include/Devices/RotaryValveDevice.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "DeviceControl/Include/SlaveModules/ModuleConfig.h"
#include <sys/stat.h>
#include <unistd.h>
#include <QtDebug>
namespace DeviceControl
{

#define CHECK_SENSOR_TIME (200) // in msecs
const qint32 TOLERANCE = 10; //!< tolerance value for calculating inside and outside range

/****************************************************************************/
/*!
 *  \brief    Constructor of the CRotaryValveDevice class
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 */
/****************************************************************************/
CRotaryValveDevice::CRotaryValveDevice(DeviceProcessing* pDeviceProcessing, QString Type) : CBaseDevice(pDeviceProcessing, Type),
        m_pTempCtrl(0), m_pMotorRV(0), m_RVCurrentPosition(RV_UNDEF), m_RVPrevPosition(RV_UNDEF)
{
    Reset();
    FILE_LOG_L(laDEV, llINFO) << "rotary valve device created";
}//lint !e1566

/****************************************************************************/
/*!
 *  \brief  Destructor of CRotaryValveDevice
 */
/****************************************************************************/
CRotaryValveDevice::~CRotaryValveDevice()
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
 *  \brief  Clean up the error state
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if no error appears
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::ClearErrorState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    return RetVal;
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
void CRotaryValveDevice::HandleTasks()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

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
            m_ErrorTaskState = RV_DEV_ERRTASK_STATE_REPORT_IFACE;
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
            m_ErrorTaskState = RV_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_FCT_MOD_CFG)
    {
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = RV_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CRotaryValveDevice:handleTasks, new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *           m_pTempCtrl - Oven temperature control
 *           m_pMotorRV - Motor to move the oven cover
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 *
 ****************************************************************************/
ReturnCode_t CRotaryValveDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CBaseModule* pBaseModule = NULL;

    FILE_LOG_L(laDEV, llINFO) << "  CRotaryValveDevice::HandleInitializationState()";
    quint32 InstanceID;
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_RVTempCtrlKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pTempCtrl = (CTemperatureControl*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pTempCtrl == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RV_TEMPCONTROL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_RVMotorKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pMotorRV = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pMotorRV == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_MOTOR);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RV_MOTOR not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
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
 *
 ****************************************************************************/
ReturnCode_t CRotaryValveDevice::HandleConfigurationState()
{
    if(!connect(m_pMotorRV, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)),
                (CBaseDevice*)this, SLOT(OnFunctionModuleError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_MOTOR);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorRV, SIGNAL(ReportSetStateAckn(quint32, ReturnCode_t)),
                this, SLOT(OnSetMotorState(quint32, ReturnCode_t))))
    {

        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_MOTOR);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportSetStateAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorRV, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)),
                this, SLOT(OnReferenceRun(quint32, ReturnCode_t, qint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_MOTOR);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportReferenceMovementAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pMotorRV, SIGNAL(ReportPosition(quint32, ReturnCode_t, qint32, qint8)),
                this, SLOT(OnGetPosition(quint32, ReturnCode_t, qint32, qint8))))
    {

        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_MOTOR);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportPosition'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorRV, SIGNAL(ReportConfigureDone(quint32, ReturnCode_t)),
                this, SLOT(OnSetConfiguration(quint32, ReturnCode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_MOTOR);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportConfigureDone'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrl, SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrl, SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportSetTemperatureAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrl, SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrl, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrl, SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
            this, SLOT(OnTCGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
#if 0
    if(!connect(m_pTempCtrl, SIGNAL(ReportSetStatusAckn(quint32, ReturnCode_t, TempCtrlStatus_t)),
                this, SLOT(TempCtrlSetStatusAckn(quint32, ReturnCode_t, TempCtrlStatus_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportSetStatusAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrl, SIGNAL(ReportActOperatingMode(quint32, ReturnCode_t, TempCtrlOperatingMode_t)),
                this, SLOT(TempCtrlActOperatingMode(quint32, ReturnCode_t, TempCtrlOperatingMode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportActOperatingMode'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrl, SIGNAL(ReportSetOperatingModeAckn(quint32, ReturnCode_t, TempCtrlOperatingMode_t)),
                this, SLOT(TempCtrlSetOperatingModeAckn(quint32, ReturnCode_t, TempCtrlOperatingMode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportSetOperatingModeAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
#endif
    if(!connect(m_pTempCtrl, SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    Reset();

    return DCL_ERR_FCT_CALL_SUCCESS;

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
void CRotaryValveDevice::HandleIdleState()
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
 *  \brief   Read Rotary-valve device's sensors data asynchronizely
 */
/****************************************************************************/
void CRotaryValveDevice::CheckSensorsData()
{
    if(m_pTempCtrl)
    {
        (void)GetTemperatureAsync(0);
        (void)GetTemperatureAsync(1);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the error state of the class.
 *
 *   This task controls the error handling of the class.
 *   The function is called from HandleIdleState() if m_MainState == LOADER_DEV_MAIN_STATE_ERROR.
 *   This task consists of the following states:
 *
 *     OVEN_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     OVEN_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     OVEN_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state
 *
 *     OVEN_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CRotaryValveDevice::HandleErrorState()
{
    if(m_ErrorTaskState == RV_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == RV_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        m_ErrorTaskState = RV_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == RV_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = RV_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == RV_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pTempCtrl = 0;
        m_pMotorRV = 0;
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot receives the stepper motor error signal
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_MOTOR_SLV
 *  \iparam   ErrorCode  = from CAN-message
 *  \iparam   ErrorData  = from CAN-message
 *  \iparam   ErrorTime  = error message reception time on master side
 *
 ****************************************************************************/
void CRotaryValveDevice::StepperMotorError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = RV_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Handles the temperature control's error message
 *
 *            Activate error main state to forward the error information to
 *            the IOvenDevice and DeviceProcessing class
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_TEMP_SLV
 *  \iparam   ErrorCode  = from CAN-message
 *  \iparam   ErrorData  = from CAN-message
 *  \iparam   ErrorTime  = error message reception time on master side
 *
 ****************************************************************************/
void CRotaryValveDevice::TempCtrlError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = RV_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Set temperature control operating mode acknowledge notification
 *
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temp. control op. mode  was received,
 *                      otherwise the return value of the error detecting function
 *  \iparam   TempCtrlOpMode = the operating mode as set to the oven's temp. control
 *
 ****************************************************************************/
void CRotaryValveDevice::TempCtrlSetOperatingModeAckn(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlOperatingMode_t TempCtrlOpMode)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(TempCtrlOpMode);
}

/****************************************************************************/
/*!
 *  \brief Helper function, to set the temperature of Oven
 *
 *  \iparam NominalTemperature Temperature to be set
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::SetTemperature(qreal NominalTemperature, quint8 SlopeTempChange)
{
    m_TargetTemperature = NominalTemperature;
    ReturnCode_t retCode;
    if(m_pTempCtrl != NULL)
    {
        retCode = m_pTempCtrl->SetTemperature(NominalTemperature, SlopeTempChange);
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
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_SET_TEMP);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
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
void CRotaryValveDevice::OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature)
{
    Q_UNUSED(Temperature)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: RV Set temperature successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: RV set temperature failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_SET_TEMP, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   Request the actual oven temperature
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActTemperature() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam  Index = index of the temperature sensor
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
qreal CRotaryValveDevice::GetTemperature(quint32 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue = m_CurrentTemperature[Index];
    if((Now - m_LastGetTempTime[Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        ReturnCode_t retCode;
        if(m_pTempCtrl)
        {
            retCode = m_pTempCtrl->ReqActTemperature(Index);
        }
        else
        {
            retCode = DCL_ERR_NOT_INITIALIZED;
        }
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode )
        {
            RetValue = UNDEFINED_4_BYTE;
        }
        else
        {
            if(m_pDevProc)
            {
                retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_GET_TEMP);
            }
            else
            {
                retCode = DCL_ERR_NOT_INITIALIZED;
            }
            if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                RetValue = UNDEFINED_4_BYTE;
            }
            else
            {
                RetValue = m_CurrentTemperature[Index];
            }
            m_LastGetTempTime[Index] = Now;
        }
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief    Get actual temperature of Rotary valve device asynchronously.
 *
 *  \iparam  Index = Index of the target temperature control module.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::GetTemperatureAsync(quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    if((Now - m_LastGetTempTime[Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        m_LastGetTempTime[Index] = Now;
        if(m_pTempCtrl)
        {
            retCode = m_pTempCtrl->ReqActTemperature(Index);
        }
        else
        {
            retCode = DCL_ERR_NOT_INITIALIZED;
        }
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   Get the temperature sensor data captured in last 500 milliseconds.
 *
 *  \iparam  Index = Actual temperature sensor index.
 *
 *  \return  Actual temperature, UNDEFINED if failed.
 */
/****************************************************************************/
qreal CRotaryValveDevice::GetRecentTemperature(quint32 Index)
{
    QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue = UNDEFINED_4_BYTE;
    if((Now - m_LastGetTempTime[Index]) <= 500) // check if 500 msec has passed since last read
    {
        RetValue = m_CurrentTemperature[Index];
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief  slot for get temperature
 *
 *  This slot is connected to the signal ReportActTemperature
 *
 *  \iparam   ReturnCode    ReturnCode of function level Layer
 *  \iparam   Index         Index of the temperature sensor
 *  \iparam   Temp          Actual temperature
 *
 */
/****************************************************************************/
void CRotaryValveDevice::OnGetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 Index, qreal Temp)
{
    Q_UNUSED(Index)

    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: RV Get temperature successful! ";
        m_CurrentTemperature[Index] = Temp;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL get temperature failed! " << ReturnCode; //lint !e641
        m_CurrentTemperature[Index] = UNDEFINED_4_BYTE;
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_GET_TEMP, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief    Set temperature control's status.
 *
 *  \iparam  TempCtrlStatus = New pressure control status.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::SetTemperatureControlStatus(TempCtrlStatus_t TempCtrlStatus)
{
    m_TargetTempCtrlStatus = TempCtrlStatus;
    ReturnCode_t retCode;
    if(m_pTempCtrl != NULL)
    {
        retCode = m_pTempCtrl->SetStatus(TempCtrlStatus);
        return retCode;
    }
    else
    {
        return DCL_ERR_DEV_TEMP_CTRL_SET_TEMP_ERR;
    }
}

/****************************************************************************/
/*!
 *  \brief   Enable temperature control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::SetTempCtrlON()
{
    return SetTemperatureControlStatus( TEMPCTRL_STATUS_ON);
}

/****************************************************************************/
/*!
 *  \brief   Disable temperature control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::SetTempCtrlOFF()
{
    return SetTemperatureControlStatus(TEMPCTRL_STATUS_OFF);
}

/****************************************************************************/
/*!
 *  \brief   Set PID parameters for temperature control module.
 *
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::SetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Set RV temperature PID";
    ReturnCode_t retCode;
    if(m_pTempCtrl != NULL)
    {
        retCode = m_pTempCtrl->SetTemperaturePid(MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        if(m_pDevProc)
        {
            return m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_SET_TEMP_PID);
        }
        else
        {
            return DCL_ERR_NOT_INITIALIZED;
        }
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
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 */
/****************************************************************************/
void CRotaryValveDevice::OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    Q_UNUSED(MaxTemperature)
    Q_UNUSED(ControllerGain)
    Q_UNUSED(ResetTime)
    Q_UNUSED(DerivativeTime)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: RV Set temperature PID successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: RV set temperature PID failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_SET_TEMP_PID, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   Start temperature control.
 *
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::StartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange)
{
    ReturnCode_t retCode;
    m_TargetTemperature = NominalTemperature;
    m_TargetTempCtrlStatus = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState() == TEMPCTRL_STATE_ERROR)
    {
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn())
    {
        return DCL_ERR_DEV_TEMP_CTRL_ALREADY_ON;
    }
    if (IsTemperatureControlOff())
    {
        //Set the nominal temperature
        retCode = SetTemperature( NominalTemperature, SlopeTempChange);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        //ON the temperature control
        retCode = SetTemperatureControlStatus( TEMPCTRL_STATUS_ON);
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
ReturnCode_t CRotaryValveDevice::StartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    ReturnCode_t retCode;
    m_TargetTemperature = NominalTemperature;
    m_TargetTempCtrlStatus = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState() == TEMPCTRL_STATE_ERROR)
    {
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn())
    {
        retCode = SetTemperatureControlStatus(TEMPCTRL_STATUS_OFF);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
    }

    retCode = SetTemperaturePid(MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
    if(retCode != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return retCode;
    }
    //Set the nominal temperature
    retCode = SetTemperature(NominalTemperature, SlopeTempChange);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    //ON the temperature control
    if (DCL_ERR_FCT_CALL_SUCCESS != SetTemperatureControlStatus(TEMPCTRL_STATUS_ON))
    {
        return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Get temperature control module's status.
 *
 *  \return  Temperature control module status.
 */
/****************************************************************************/
TempCtrlState_t CRotaryValveDevice::GetTemperatureControlState()
{

    ReturnCode_t retCode;
    if(m_pTempCtrl)
    {
        retCode = m_pTempCtrl->ReqStatus();
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
        m_CurrentTempCtrlStatus = TEMPCTRL_STATUS_UNDEF;
        return TEMPCTRL_STATE_ERROR;
    }
    if(m_pDevProc)
    {
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_GET_TEMP_CTRL_STATE);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    TempCtrlState_t controlstate = TEMPCTRL_STATE_ERROR;
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        controlstate = TEMPCTRL_STATE_ERROR;
    }
    else if (IsTemperatureControlOn())
    {
        if (IsInsideRange(0))
        {
            controlstate = TEMPCTRL_STATE_INSIDE_RANGE;
        }
        else if (IsOutsideRange(0))
        {
            controlstate = TEMPCTRL_STATE_OUTSIDE_RANGE;
        }
    }
    else if (IsTemperatureControlOff())
    {
        controlstate =  TEMPCTRL_STATE_OFF;
    }
    return controlstate;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with get temperature control status.
 *
 *  This slot is connected to the signal, ReportActStatus
 *
 *  \iparam InstanceID = Instance ID of the device
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam TempCtrlStatus = Actual temperature control status
 *  \iparam MainsVoltage = Main voltage status.
 *
 */
/****************************************************************************/
void CRotaryValveDevice::OnTempControlStatus(quint32 InstanceID, ReturnCode_t ReturnCode,
                                             TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage)
{
    Q_UNUSED(InstanceID)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        m_CurrentTempCtrlStatus = TempCtrlStatus;
        m_MainsVoltageStatus = MainsVoltage;
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_GET_TEMP_CTRL_STATE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature control is enabled.
 *
 *  \return  True if temperature control is enabled, else not.
 */
/****************************************************************************/
bool CRotaryValveDevice::IsTemperatureControlOn()
{
    return (m_CurrentTempCtrlStatus == TEMPCTRL_STATUS_ON);
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature control is disabled.
 *
 *  \return  True if temperature control is disabled, else not.
 */
/****************************************************************************/
bool CRotaryValveDevice::IsTemperatureControlOff()
{
    return (m_CurrentTempCtrlStatus == TEMPCTRL_STATUS_OFF);
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature is inside the range.
 *
 *  \return  True if is inside the range, else not.
 */
/****************************************************************************/
bool CRotaryValveDevice::IsInsideRange(quint8 Index)
{
    if(GetTemperature(0) != UNDEFINED_4_BYTE)
    {
        if((m_TargetTemperature != UNDEFINED_4_BYTE) || (m_CurrentTemperature[Index] != UNDEFINED_4_BYTE))
        {
            if ((m_CurrentTemperature[Index] > m_TargetTemperature - TOLERANCE)||
                            (m_CurrentTemperature[Index] < m_TargetTemperature + TOLERANCE))
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
 *  \return  True if is outside the range, else not.
 */
/****************************************************************************/
bool CRotaryValveDevice::IsOutsideRange(quint8 Index)
{
    if(GetTemperature(0) != UNDEFINED_4_BYTE)
    {
        if((m_TargetTemperature != UNDEFINED_4_BYTE) || (m_CurrentTemperature[Index] != UNDEFINED_4_BYTE))
        {
            if ((m_CurrentTemperature[Index] < m_TargetTemperature - TOLERANCE)||
                            (m_CurrentTemperature[Index] > m_TargetTemperature + TOLERANCE))
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
 *  \brief   Request the rotary valve to move to its initial position.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::ReqMoveToInitialPosition()
{
    QString lsCode;
    ReturnCode_t RetValue = DCL_ERR_FCT_CALL_SUCCESS;
    bool ParaChange = false;
    //reconfig for original position
    if(GetConfigLS2Exists()==0)
    {
        SetConfigLS2Exists(1);
        SetConfigPos2Value(1);
        SetConfigPos3Value(1);
        ParaChange |= true; //lint !e514
    }
    if(GetConfigRefRunPosition() != 3)
    {
        SetConfigRefRunPosition(3);
        ParaChange |= true; //lint !e514
    }
    if(GetConfigPos1Stop() != 0)
    {
        SetConfigPos1Stop(0);
        ParaChange |= true; //lint !e514
    }
    if(GetConfigPos3Stop() != 1)
    {
        SetConfigPos3Stop(1);
        ParaChange |= true; //lint !e514
    }
    if(ParaChange)
    {
        (void)ApplyNewParameterSet();
    }
    else
    {
        (void)SetMotorState(true);
    }
    lsCode = GetLimitSwitchCode();
    if(lsCode == "3")
    {
        LogDebug(QString( "INFO: Already At Initial Position, No Need To Move!"));
        SetEDPosition(RV_TUBE_1);
        SetPrevEDPosition(RV_TUBE_1);
        return RetValue;
    }
    else if((lsCode == "request error")||(lsCode == "error"))
    {
        LogDebug(QString("ERROR: Error when read LS code"));
        RetValue = DCL_ERR_DEV_RV_MOTOR_CANNOTGET_ORIGINALPOSITION;
        return RetValue;
    }
    ReturnCode_t refRunRet = DoReferenceRunWithStepCheck(50, 7000);
    lsCode = GetLimitSwitchCode();
    if(refRunRet == DCL_ERR_FCT_CALL_SUCCESS)
    {
        quint32 retry = 0;
        while((lsCode != "3")&&(retry++ < 3))
        {
            lsCode = GetLimitSwitchCode();
        }
        if(lsCode == "3")
        {
            LogDebug(QString("INFO: Hit Initial Position"));
            SetEDPosition(RV_TUBE_1);
            SetPrevEDPosition(RV_TUBE_1);
        }
        else
        {
            SetEDPosition(RV_UNDEF);
            SetPrevEDPosition(RV_UNDEF);
            RetValue = DCL_ERR_DEV_RV_MOTOR_CANNOTGET_ORIGINALPOSITION;
            LogDebug(QString("WARNING: Hit unexpected position, please retry!"));
        }
    }
    else
    {
        SetEDPosition(RV_UNDEF);
        SetPrevEDPosition(RV_UNDEF);
        LogDebug(QString("WARNING: Hit unexpected position, please retry!"));
        RetValue = refRunRet;
    }
    return RetValue;
}
/****************************************************************************/
/*!
 *  \brief   Request the rotary valve to do reference run with step check.
 *
 *  \iparam  LowerLimit = The lower limit of the movement to initial position.
 *  \iparam  UpperLimit = The upper limit of the movement to initial position.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::DoReferenceRunWithStepCheck(quint32 LowerLimit, quint32 UpperLimit)
{
    ReturnCode_t ret = DCL_ERR_FCT_CALL_SUCCESS;

    bool stop = false;
    quint8 retry = 0;
    quint8 GetPositionRetry = 0;
    quint32 Step = 0;
    while(!stop)
    {
        stop = true;
        (void)DoReferenceRun();
        while((GetPositionRetry++) <5)
        {
            bool ok;
            Step = qAbs(GetPosition().toInt(&ok));
            if(Step > 0)
            {
                break;
            }
            (void)usleep(500*1000);//should sleep for 0.5 sec
        }
        if(Step < LowerLimit)
        {
            if((retry++) < 20)
            {
                stop = false;
                LogDebug(QString("WARNING: Motor running lower limit exception: retry time: %1, retry now.").arg(retry));
            }
            else
            {
                LogDebug(QString("ERROR: Motor moving retry time exceed 20, may be stucked!"));
                ret = DCL_ERR_DEV_RV_MOTOR_INTERNALSTEPS_RETRY;
            }
        }
        else if(Step > UpperLimit)
        {
            LogDebug(QString("WARNING: Motor moving steps exceed upper limit: %1").arg(UpperLimit));
            ret = DCL_ERR_DEV_RV_MOTOR_INTERNALSTEPS_EXCEEDUPPERLIMIT;
        }

    }

    return ret;
}

/****************************************************************************/
/*!
 *  \brief   Request the rotary valve to move to certain encoder disk's position.
 *
 *  \iparam  RVPosition = Target rotary valve encoder disk's position.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::ReqMoveToRVPosition( RVPosition_t RVPosition)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    RVPosition_t EDPosition = GetEDPosition();
    RVPosition_t PrevEDPosition = GetPrevEDPosition();
    quint32 MoveSteps = 0;
    bool cw = false;

    QString lsCode, lsCodeDup;
    quint8 retry = 0;

    if(RV_UNDEF == EDPosition)
    {
        lsCode = GetLimitSwitchCode();
        lsCodeDup = GetLimitSwitchCode();
        while(lsCode != lsCodeDup)
        {
            lsCode = GetLimitSwitchCode();
            lsCodeDup = GetLimitSwitchCode();
            if(retry++ > 3)
            {
                LogDebug(QString("WARNING: Limit Switch code are not stable!"));
                retCode = DCL_ERR_DEV_RV_MOTOR_INTERNALSTEPS_RETRY;
                return retCode;
            }
        }
        if(lsCode == "3")
        {
            EDPosition = RV_TUBE_1;
            SetEDPosition(EDPosition);
            SetPrevEDPosition(RV_TUBE_1);
        }
        else
        {
            LogDebug(QString("ERROR: Can't find current position, please run MoveToInitialPosition first!"));
            retCode = DCL_ERR_DEV_RV_MOTOR_LOSTCURRENTPOSITION;
            return retCode;
        }
    }
    if(((qint32)RVPosition < 1)||((qint32)RVPosition > 32))
    {
        LogDebug(QString("WARNING: The Tube Posotion %1 is Invalid").arg(RVPosition));    //lint !e641
        retCode = DCL_ERR_DEV_RV_MOTOR_LOSTCURRENTPOSITION;
        return retCode;
    }

    if(EDPosition == RVPosition)
    {
        LogDebug(QString("INFO: Already At Target Position, No Need To Move!"));       //lint !e641
        retCode = DCL_ERR_FCT_CALL_SUCCESS;
        return retCode;
    }
    else
    {
        MoveSteps = ((RVPosition > EDPosition)?(RVPosition - EDPosition):(EDPosition - RVPosition));//lint !e656 !e641
        if(MoveSteps > 15)
        {
            MoveSteps = 32 - MoveSteps;
            cw = (RVPosition > EDPosition ? true : false);
        }
        else
        {
            cw = (RVPosition > EDPosition ? false : true);
        }
        if((RV_SEAL_1 == EDPosition) && (RV_TUBE_2 == PrevEDPosition) && (!cw))
        {
            retCode = MoveToNextPortCW();
            if( DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                return retCode;
            }
            EDPosition = GetEDPosition();
            if(EDPosition != RV_TUBE_1)
            {
                LogDebug(QString("ERROR: Lost current position, need to run MoveToInitialPosition!"));
                retCode = DCL_ERR_DEV_RV_MOTOR_LOSTCURRENTPOSITION;
                return retCode;
            }
            MoveSteps = ((RVPosition > EDPosition)?(RVPosition - EDPosition):(EDPosition - RVPosition));//lint !e656 !e641
            if(MoveSteps > 15)
            {
                MoveSteps = 32 - MoveSteps;
                cw = (RVPosition > EDPosition ? true : false);
            }
            else
            {
                cw = (RVPosition > EDPosition ? false : true);
            }
        }
        if((RV_TUBE_2 == EDPosition) && (RV_SEAL_1 == PrevEDPosition) && (cw))
        {
            retCode = MoveToNextPortCCW();
            if( DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                return retCode;
            }
            EDPosition = GetEDPosition();
            if(EDPosition != RV_SEAL_2)
            {
                LogDebug(QString("ERROR: Lost current position, need to run MoveToInitialPosition!"));
                retCode = DCL_ERR_DEV_RV_MOTOR_LOSTCURRENTPOSITION;
                return retCode;
            }
            MoveSteps = ((RVPosition > EDPosition)?(RVPosition - EDPosition):(EDPosition - RVPosition)); //lint !e656 !e641
            if(MoveSteps > 15)
            {
                MoveSteps = 32 - MoveSteps;
                cw = (RVPosition > EDPosition ? true : false);
            }
            else
            {
                cw = (RVPosition > EDPosition ? false : true);
            }
        }
    }

    if(cw)
    {
        for(quint32 i = 0; i < MoveSteps; i++)
        {
            retCode = MoveToNextPortCW();
            if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                return retCode;
            }
        }
    }
    else
    {
        for(quint32 i = 0; i < MoveSteps; i++)
        {
            retCode = MoveToNextPortCCW();
            if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                return retCode;
            }
        }
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   Return rotary valve's encoder disk's position.
 *
 *  \return  Current encoder disk position.
 */
/****************************************************************************/
RVPosition_t CRotaryValveDevice::ReqActRVPosition()
{
    return m_RVCurrentPosition;
}

RVPosition_t CRotaryValveDevice::ReqAdjacentPosition(RVPosition_t position)
{
    RVPosition_t result = RV_UNDEF;
    if(position != RV_UNDEF)
    {
        quint8 remainder = (quint8)position % 2;
        quint8 left_position = (quint8)position / 2;
        result = (RVPosition_t)(left_position * 2 + remainder * 2 + remainder - 1);
    }
    return result;
}

bool CRotaryValveDevice::IsSealPosition(RVPosition_t position)
{
    quint8 remainder = (quint8)position % 2;
    return (remainder == 0);
}

/****************************************************************************/
/*!
 *  \brief   Request the rotary valve to move to next clockwise encoder disk position.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::MoveToNextPortCW()
{
    ReturnCode_t ret = DCL_ERR_FCT_CALL_SUCCESS;
    RVPosition_t EDPosition = GetEDPosition();

    if(RV_UNDEF == EDPosition)
    {
        LogDebug(QString("ERROR: Can't find current position, please run MoveToInitialPosition first!"));
        return DCL_ERR_DEV_RV_MOTOR_LOSTCURRENTPOSITION;
    }

    if( DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW == (DeviceControl::CANFctModuleStepperMotor::RotationDir_t)GetRotationDirection())
    {
        SetRotationDirection(DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW ); //lint !e641
        ret = MoveToNextPort(true, \
                             GetLowerLimit((quint32)EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, true), \
                             GetUpperLimit((quint32)EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, true));
    }
    else
    {
        ret = MoveToNextPort(false, \
                             GetLowerLimit((quint32)EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, false), \
                             GetUpperLimit((quint32)EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, false));
    }

    if((DCL_ERR_FCT_CALL_SUCCESS == ret) && (3 == (quint32)EDPosition))
    {
        ret = MoveToNextPort(false, \
                             GetLowerLimit(99, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, false), \
                             GetUpperLimit(99, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, false));
    }

    if((ret == DCL_ERR_FCT_CALL_SUCCESS) && (EDPosition == GetEDPosition()))
    {
        SetPrevEDPosition(EDPosition);
        EDPosition = (RVPosition_t)((quint32)EDPosition - 1);
        if(EDPosition == RV_UNDEF)
        {
            EDPosition = RV_SEAL_16;
        }
        SetEDPosition(EDPosition);
        LogDebug(QString("INFO: CW Hit Position: %1").arg(TranslateFromEDPosition(EDPosition))); //lint !e641
    }
    else
    {
        LogDebug(QString("ERROR: Unknown error happened, lost current position, please run MoveToInitialPosition"));
        SetEDPosition(RV_UNDEF);
        SetPrevEDPosition(RV_UNDEF);
    }

    return ret;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Get the name of certain encoder disk postion.
 *
 *  \iparam  EDPosition = Current encoder disk position.
 *
 *  \return The name of certain position.
 *
 */
/****************************************************************************/
QString CRotaryValveDevice::TranslateFromEDPosition(quint32 EDPosition)
{
    QString RetValue = "Invalid position";
    if((EDPosition > 0)&&(EDPosition < 33))
    {
        quint32 divide = EDPosition / 2;
        if((divide*2) == EDPosition)
        {
            RetValue = tr("Seal %1").arg(divide);
        }
        else
        {
            RetValue = tr("Tube %1").arg(divide + 1);
        }
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Get the upper limit for movement at certain encoder disk position .
 *
 *  \iparam  CurrentEDPosition = Current encoder disk position.
 *  \iparam  Direction = The current rotation direction.
 *  \iparam  ChangeDirection = If is to change direction.
 *
 *  \return the upper limit
 *
 */
/****************************************************************************/
quint32 CRotaryValveDevice::GetUpperLimit(quint32 CurrentEDPosition, DeviceControl::CANFctModuleStepperMotor::RotationDir_t Direction, bool ChangeDirection)
{
    quint32 UpperLimit = 500;
    if(CurrentEDPosition != 0)
    {
        if( DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW == Direction)
        {
            if(3 == CurrentEDPosition)
            {
                UpperLimit = 194;
            }
            else if(2 == CurrentEDPosition)
            {
                UpperLimit = 350;
            }
            else if(24 == CurrentEDPosition)
            {
                UpperLimit = 496;
            }
            else if(26 == CurrentEDPosition)
            {
                UpperLimit = 350;
            }
            else if(99 == CurrentEDPosition)
            {
                UpperLimit = 130;
            }
            else
            {
                UpperLimit = 350;
            }
        }
        else if(DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW == Direction)
        {
            if(2 == CurrentEDPosition)
            {
                UpperLimit = 130;
            }
            else if(1 == CurrentEDPosition)
            {
                UpperLimit = 350;
            }
            else if(23 == CurrentEDPosition)
            {
                UpperLimit = 496;
            }
            else if(25 == CurrentEDPosition)
            {
                UpperLimit = 350;
            }
            else if(99 == CurrentEDPosition)
            {
                UpperLimit = 194;
            }
            else
            {
                UpperLimit = 350;
            }
        }
    }
    if(ChangeDirection)
    {
        UpperLimit += 40;
    }
    return UpperLimit;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Get the lower limit for movement at certain encoder disk position .
 *
 *  \iparam  CurrentEDPosition = Current encoder disk position.
 *  \iparam  Direction = The current rotation direction.
 *  \iparam  ChangeDirection = If is to change direction.
 *
 *  \return the lower limit
 *
 */
/****************************************************************************/
quint32 CRotaryValveDevice::GetLowerLimit(quint32 CurrentEDPosition, DeviceControl::CANFctModuleStepperMotor::RotationDir_t Direction, bool ChangeDirection)
{
    quint32 LowerLimit = 100;
    if(CurrentEDPosition != 0)
    {
        if( DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW == Direction)
        {
            if(99 == CurrentEDPosition)
            {
                LowerLimit = 30;
            }
            else
            {
                LowerLimit = 100;
            }
        }
        else if(DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW == Direction)
        {
            if( 2 == CurrentEDPosition)
            {
                LowerLimit = 30;
            }
            else
            {
                LowerLimit = 100;
            }
        }
    }
    if(ChangeDirection)
    {
        LowerLimit += 50;
    }
    return LowerLimit;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Let the rotary valve move to next port counter-clockwisly.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::MoveToNextPortCCW()
{
    ReturnCode_t ret = DCL_ERR_FCT_CALL_SUCCESS;
    RVPosition_t EDPosition = GetEDPosition();

    if(RV_UNDEF == EDPosition)
    {
        LogDebug(QString("ERROR: Can't find current position, please run MoveToInitialPosition first!"));
        return DCL_ERR_DEV_RV_MOTOR_LOSTCURRENTPOSITION;
    }

    if( DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW == GetRotationDirection()) //lint !e641
    {
        SetRotationDirection(DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW ); //lint !e641
        ret = MoveToNextPort(true, \
                             GetLowerLimit((quint32)EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, true), \
                             GetUpperLimit((quint32)EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, true));
    }
    else
    {
        ret = MoveToNextPort(false, \
                             GetLowerLimit((quint32)EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, false), \
                             GetUpperLimit((quint32)EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, false));
    }
    if((DCL_ERR_FCT_CALL_SUCCESS == ret) && (2 == EDPosition)) //lint !e641
    {
        ret = MoveToNextPort(false, \
                             GetLowerLimit(99, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, false), \
                             GetUpperLimit(99, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, false));
    }

    if((ret == DCL_ERR_FCT_CALL_SUCCESS) && (EDPosition == GetEDPosition()))
    {
        SetPrevEDPosition(EDPosition);
        quint32 tempPosition = ((quint32)EDPosition + 1);
        if(tempPosition == 33)
        {
            tempPosition = (quint32)RV_TUBE_1;
        }
        SetEDPosition((RVPosition_t)tempPosition);
        LogDebug(QString("INFO: CCW Hit Position: %1").arg( TranslateFromEDPosition(tempPosition)));
    }
    else
    {
        LogDebug(QString("ERROR: Unknown error happened, lost current position, please run MoveToInitialPosition"));
        SetEDPosition(RV_UNDEF);
        SetPrevEDPosition(RV_UNDEF);
    }
    return ret;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Move to next port without change previous
 *                           direction configuration.
 *  \iparam changeParameter If to send can msg to change configuration.
 *  \iparam LowerLimit lower limit for the reference run
 *  \iparam UpperLimit upper limit for the reference run
 *
 *  \return The name of certain position.
 *
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::MoveToNextPort(bool changeParameter, quint32 LowerLimit, quint32 UpperLimit)
{
    bool ParaChange = changeParameter;
    QString lsCode;
    ReturnCode_t ret;
    if(GetConfigLS2Exists()==1)
    {
        SetConfigLS2Exists(0);
        SetConfigPos2Value(0);
        SetConfigPos3Value(0);
        ParaChange |= true;  //lint !e514
    }
    if(GetConfigRefRunPosition() != 1)
    {
        SetConfigRefRunPosition(1);
        ParaChange |= true;  //lint !e514
    }
    if(GetConfigPos1Stop() != 1)
    {
        SetConfigPos1Stop(1);
        ParaChange |= true;  //lint !e514
    }
    if(GetConfigPos3Stop() != 0)
    {
        SetConfigPos3Stop(0);
        ParaChange |= true;  //lint !e514
    }
    if(ParaChange)
    {
        (void)ApplyNewParameterSet();
    }
    RVPosition_t ED = GetEDPosition();
    LogDebug(QString("INFO: Last ED is: %1, lower limit is: %2, upper limit is %3.").arg(ED).arg(LowerLimit).arg(UpperLimit));    //lint !e641
    ret = DoReferenceRunWithStepCheck(LowerLimit, UpperLimit);

    lsCode = GetLimitSwitchCode();
    if((lsCode != "1")&&(lsCode != "3"))
    {
        quint32 Retry = 0;
        while(((Retry++) < 30)&&(lsCode != "1")&&(lsCode != "3"))
        {
            LogDebug(QString("WARNING: Get unexpected LS Code: %1, wait 0.5 sec to read again.").arg(lsCode));
            (void)usleep(500*1000);
            lsCode = GetLimitSwitchCode();
        }
        if(Retry >= 30)
        {
            LogDebug(QString("ERROR: Hit unexpected position! Already read LS code for 30 times"));
        }
    }
    return ret;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Return rotary-valve's rotation direction.
 *
 *  \return  The rotation direction.
 */
/****************************************************************************/
quint8 CRotaryValveDevice::GetRotationDirection()
{
    return m_Config.PosCode1.bStopDir;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set rotary-valve's rotation direction.
 *
 *  \iparam direction = Target direction.
 *
 *  \return void
 */
/****************************************************************************/
void CRotaryValveDevice::SetRotationDirection(quint8 direction)
{
    if(direction == DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW) //lint !e641
    {
        m_Config.PosCode1.bStopDir = 1;
        m_Config.PosCode2.bStopDir = 2;
    }
    else if(direction == DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW) //lint !e641
    {
        m_Config.PosCode1.bStopDir = 2;
        m_Config.PosCode2.bStopDir = 1;
    }
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Get rotary-valve's absolute position.
 *
 *  \return  The absolute position.
 */
/****************************************************************************/
QString CRotaryValveDevice::GetPosition()
{
    if(m_pMotorRV)
    {
        if (DCL_ERR_FCT_CALL_SUCCESS != m_pMotorRV->ReqActPosition())
        {
            return "request error";
        }
    }
    else
    {
        return "request error";
    }
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_FAILED;
    if(m_pDevProc)
    {
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_GET_MOTOR_POSITION);
    }
    QString position = "error";
    if (retCode == DCL_ERR_FCT_CALL_SUCCESS) {
        (void)position.setNum(m_CurrentPosition);
    }
    return position;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Get rotary-valve's limit switch code.
 *
 *  \return  The limit switch code.
 */
/****************************************************************************/
QString CRotaryValveDevice::GetLimitSwitchCode()
{

    ReturnCode_t retCode;
    if(m_pMotorRV)
    {
        retCode = m_pMotorRV->ReqActPosition();
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    if (retCode != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return "request error";
    }
    if(m_pDevProc)
    {
        retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_GET_MOTOR_POSITION);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    QString code = "error";
    if (retCode == DCL_ERR_FCT_CALL_SUCCESS) {
        (void)code.setNum(m_CurrentLimitSwitchCode);
    }
    return code;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with get position.
 *
 *  This slot is connected to the signal, OnGetPosition
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam Position = Actual position.
 *  \iparam PosCode = Actual limit switch code.
 *
 */
/****************************************************************************/
void CRotaryValveDevice::OnGetPosition(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qint32 Position, qint8 PosCode)
{
    m_CurrentPosition = Position;
    m_CurrentLimitSwitchCode = PosCode;
    if (ReturnCode != DCL_ERR_FCT_CALL_SUCCESS) {
        LogDebug(QString( "WRANING! Unexpected return code at OnGetPosition: %1").arg((quint32)ReturnCode));
    }

    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_GET_MOTOR_POSITION, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set if rotary-valve's limit switch 2 exist.
 *
 *  \iparam bExists = Actual limit switch code.
 *
 *  \return void
 */
/****************************************************************************/
void CRotaryValveDevice::SetConfigLS2Exists(quint8 bExists)
{
    m_Config.LimitSwitch2.bExists = bExists;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Check if rotary-valve's limit switch 2 exist.
 *
 *  \return The configuration of limit switch 2' existence.
 */
/****************************************************************************/
quint8 CRotaryValveDevice::GetConfigLS2Exists()
{
    return m_Config.LimitSwitch2.bExists;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set position 2's limit switch value.
 *
 *  \iparam bValid = Conifguration of position 2's limit switch value.
 *
 *  \return void
 */
/****************************************************************************/
void CRotaryValveDevice::SetConfigPos2Value(quint8 bValid)
{
    m_Config.PosCode2.bValid = bValid;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Check position 2's limit switch value.
 *
 *  \return Position 2's limit switch value.
 */
/****************************************************************************/
quint8 CRotaryValveDevice::GetConfigPos2Value()
{
    return m_Config.PosCode2.bValid;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set position 3's limit switch value.
 *
 *  \iparam bValid = Conifguration of position 3's limit switch value.
 *
 *  \return void
 */
/****************************************************************************/
void CRotaryValveDevice::SetConfigPos3Value(quint8 bValid)
{
    m_Config.PosCode3.bValid = bValid;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Check position 3's limit switch value.
 *
 *  \return Position 3's limit switch value.
 */
/****************************************************************************/
quint8 CRotaryValveDevice::GetConfigPos3Value()
{
    return m_Config.PosCode3.bValid;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Check reference run's stop position value.
 *
 *  \return Reference run's stop position value.
 */
/****************************************************************************/
qint8 CRotaryValveDevice::GetConfigRefRunPosition()
{
    return m_Config.refRunRefPos;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set reference run's stop position value.
 *
 *  \iparam refRunRefPos = Reference run position.
 *
 *  \return void
 */
/****************************************************************************/
void CRotaryValveDevice::SetConfigRefRunPosition(qint8 refRunRefPos)
{
    m_Config.refRunRefPos =  refRunRefPos;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set position 1's stop value.
 *
 *  \iparam bStop = Position 1's stop value.
 *
 *  \return void
 */
/****************************************************************************/
void CRotaryValveDevice::SetConfigPos1Stop(quint8 bStop)
{
    m_Config.PosCode1.bStop = bStop;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set position 2's stop value.
 *
 *  \iparam bStop = Position 2's stop value.
 *
 *  \return void
 */
/****************************************************************************/
void CRotaryValveDevice::SetConfigPos2Stop(quint8 bStop)
{
    m_Config.PosCode2.bStop = bStop;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Check position 2's stop value.
 *
 *  \return position 2's stop value.
 */
/****************************************************************************/
quint8 CRotaryValveDevice::GetConfigPos2Stop()
{
    return m_Config.PosCode2.bStop;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set position 3's stop value.
 *
 *  \iparam bStop = Position 3's stop value.
 *
 *  \return void
 */
/****************************************************************************/
void CRotaryValveDevice::SetConfigPos3Stop(quint8 bStop)
{
    m_Config.PosCode3.bStop = bStop;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Check position 3's stop value.
 *
 *  \return position 3's stop value.
 */
/****************************************************************************/
quint8 CRotaryValveDevice::GetConfigPos3Stop()
{
    return m_Config.PosCode3.bStop;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Check position 1's stop value.
 *
 *  \return position 1's stop value.
 */
/****************************************************************************/
quint8 CRotaryValveDevice::GetConfigPos1Stop()
{
    return m_Config.PosCode1.bStop;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Get current encoder disk's position.
 *
 *  \return Encoder disk position.
 */
/****************************************************************************/
RVPosition_t CRotaryValveDevice::GetEDPosition()
{
    if((quint32)m_RVCurrentPosition > 32)
    {
        m_RVCurrentPosition = RV_UNDEF;
    }

    return m_RVCurrentPosition;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set current encoder disk's position.
 *
 *  \iparam position = Encoder disk position.
 */
/****************************************************************************/
void CRotaryValveDevice::SetEDPosition(RVPosition_t position)
{

    if(((quint32)position <33) &&( (quint32)position >0))
    {
        m_RVCurrentPosition= position;
    }
    else
    {
        m_RVCurrentPosition = RV_UNDEF;
    }
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Get previous encoder disk's position.
 *
 *  \return Encoder disk position.
 */
/****************************************************************************/
RVPosition_t CRotaryValveDevice::GetPrevEDPosition()
{
    if((quint32)m_RVPrevPosition > 32)
    {
        m_RVPrevPosition = RV_UNDEF;
    }

    return m_RVPrevPosition;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set previous encoder disk's position.
 *
 *  \iparam position = Encoder disk position.
 */
/****************************************************************************/
void CRotaryValveDevice::SetPrevEDPosition(RVPosition_t position)
{

    if(((quint32)position <33) &&( (quint32)position >0))
    {
        m_RVPrevPosition = position;
    }
    else
    {
        m_RVPrevPosition = RV_UNDEF;
    }
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Set current encoder disk's position to initial
 *                           position.
 *
 */
/****************************************************************************/
void CRotaryValveDevice::InitEDPosition()
{
    m_RVCurrentPosition = RV_UNDEF;
}

/****************************************************************************/
/*!
 *  \brief  Request the slave board to get the configuration.
 *
 */
/****************************************************************************/
void CRotaryValveDevice::RestoreLastValidParameterSet()
{
    if(m_pMotorRV)
    {
        m_Config = *m_pMotorRV->GetConfiguration();
    }
}

/****************************************************************************/
/*!
 *  \brief  Reset class member variable
 */
/****************************************************************************/
void CRotaryValveDevice::Reset()
{
    RestoreLastValidParameterSet();
    m_CurrentPosition = 0;
    m_CurrentLimitSwitchCode = 0;
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_ErrorTaskState   = RV_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_ROTARY_VALVE;

    memset( &m_LastGetTempTime, 0 , sizeof(m_LastGetTempTime)); //lint !e545
    m_TargetTempCtrlStatus = TEMPCTRL_STATUS_UNDEF;
    m_CurrentTempCtrlStatus = TEMPCTRL_STATUS_UNDEF;
    memset(&m_CurrentTemperature, 0, sizeof(m_CurrentTemperature)); //lint !e545
    m_TargetTemperature = 0;
    memset( &m_MainsVoltageStatus, 0 , sizeof(m_MainsVoltageStatus)); //lint !e545
    memset( &m_TCHardwareStatus, 0 , sizeof(m_TCHardwareStatus)); //lint !e545
    m_LastGetTCCurrentTime = 0;
}

/****************************************************************************/
/*!
 *  \brief  Write the local configuration to slave board.
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::ApplyNewParameterSet()
{
   ReturnCode_t retCode = SetMotorState(false);
    // disable function module before reconfiguration
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
        return retCode;
    }
    if(m_pMotorRV)
    {
        retCode =  m_pMotorRV->DoReconfiguration(&m_Config);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    if ( retCode !=DCL_ERR_FCT_CALL_SUCCESS) {
        return retCode;
    }
    if(m_pDevProc)
    {
        retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_SET_MOTOR_CONFIG);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    (void)sleep(1);//in seconds

    if(DCL_ERR_FCT_CALL_SUCCESS == retCode)
    {
        retCode = SetMotorState(true);
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Request the motor to do reference run.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::DoReferenceRun()
{
    // enable function module before first use
    ReturnCode_t retCode = SetMotorState(true);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
        return retCode;
    }
    if(m_pMotorRV)
    {
        retCode = m_pMotorRV->ExecReferenceMovement();
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    if (retCode != DCL_ERR_FCT_CALL_SUCCESS) {
        return retCode;
    }
    if(m_pDevProc)
    {
        retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_REF_RUN);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with do reference run.
 *
 *  This slot is connected to the signal, OnReferenceRun
 *
 *  \iparam InstanceID = Instance ID of the module
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam Position = Motor's actual position.
 *
 */
/****************************************************************************/
void CRotaryValveDevice::OnReferenceRun(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position)
{
    Q_UNUSED(InstanceID)
    // exit from eventloop: 1 success, 0 timeout, -1 failure
    if(DCL_ERR_FCT_CALL_SUCCESS ==  ReturnCode)
    {
        m_CurrentPosition = Position;
    }
    else
    {
        LogDebug(QString( "WARNING! Unexpected return code at OnReferenceRun: %1").arg((quint32)ReturnCode));
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_REF_RUN, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set configuration.
 *
 *  This slot is connected to the signal, OnSetConfiguration
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *
 */
/****************************************************************************/
void CRotaryValveDevice::OnSetConfiguration(quint32 /*InstanceID*/, ReturnCode_t ReturnCode)
{
    // exit from eventloop: 1 success, 0 timeout, -1 failure
    if (ReturnCode != DCL_ERR_FCT_CALL_SUCCESS) {
        LogDebug(QString( "WARNING! Unexpected return code at OnSetConfiguration"));
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_SET_MOTOR_CONFIG, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Set the motor's state.
 *
 *  \iparam flag = New state.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 */
/****************************************************************************/
ReturnCode_t CRotaryValveDevice::SetMotorState(bool flag)
{


    ReturnCode_t retCode;
    if(m_pMotorRV)
    {
        retCode = m_pMotorRV->SetMotorState(flag);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    if (retCode != DCL_ERR_FCT_CALL_SUCCESS) {
        return retCode;
    }
    if(m_pDevProc)
    {
       retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_SET_MOTOR_STATE);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }

    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set motor state.
 *
 *  This slot is connected to the signal, OnSetMotorState
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *
 */
/****************************************************************************/
void CRotaryValveDevice::OnSetMotorState(quint32 /*InstanceID*/, ReturnCode_t ReturnCode)
{
    // exit from eventloop: 1 success, 0 timeout, -1 failure
    if ( DCL_ERR_FCT_CALL_SUCCESS  !=ReturnCode) {
        LogDebug(QString( "WARNING! Unexpected return code at OnSetMotorState: %1").arg((quint32)ReturnCode));
    }

    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_SET_MOTOR_STATE, ReturnCode);
    }
}
/****************************************************************************/
/*!
 *  \brief   Get actual current of device's heater.
 *
 *  \return  The current of heater in mA.
 */
/****************************************************************************/
quint16 CRotaryValveDevice::GetHeaterCurrent(void)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint16 RetValue = UNDEFINED_2_BYTE;
    if(m_pTempCtrl != NULL)
    {
        if((Now - m_LastGetTCCurrentTime) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
        {
            ReturnCode_t retCode = m_pTempCtrl->GetHardwareStatus();
            if (DCL_ERR_FCT_CALL_SUCCESS == retCode )
            {
                if(m_pDevProc)
                {
                    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_TC_GET_HW_STATUS);
                }
                if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
                {
                    RetValue = UNDEFINED_2_BYTE;
                }
                else
                {
                    RetValue = m_TCHardwareStatus.Current;
                }
                m_LastGetTCCurrentTime = Now;
            }
        }
        else
        {
            RetValue = m_TCHardwareStatus.Current;
        }
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function GetHeaterSwitchType
 *
 *  \return from Heater switch type
 */
/****************************************************************************/
quint8 CRotaryValveDevice::GetHeaterSwitchType(void)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint8 RetValue = UNDEFINED_1_BYTE;
    if(m_pTempCtrl != NULL)
    {
        if((Now - m_LastGetTCCurrentTime) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
        {
            ReturnCode_t retCode = m_pTempCtrl->GetHardwareStatus();
            if (DCL_ERR_FCT_CALL_SUCCESS == retCode )
            {
                if(m_pDevProc)
                {
                    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_TC_GET_HW_STATUS);
                }
                if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
                {
                    RetValue = UNDEFINED_1_BYTE;
                }
                else
                {
                    RetValue = m_TCHardwareStatus.HeaterSwitchType;
                }
                m_LastGetTCCurrentTime = Now;
            }
        }
        else
        {
            RetValue = m_TCHardwareStatus.HeaterSwitchType;
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
 *  \param InstanceID
 *  \param HeaterSwitchType
 */
/****************************************************************************/
void CRotaryValveDevice::OnTCGetHardwareStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Sensors, quint8 Fans,
                                               quint8 Heaters, quint8 Pids, quint16 Current, quint8 HeaterSwitchType)
{
    Q_UNUSED(InstanceID)
    if (DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        m_TCHardwareStatus.Sensors = Sensors;
        m_TCHardwareStatus.Fans = Fans;
        m_TCHardwareStatus.Heaters = Heaters;
        m_TCHardwareStatus.Pids = Pids;
        m_TCHardwareStatus.Current = Current;
        m_TCHardwareStatus.HeaterSwitchType = HeaterSwitchType;
    }
    else
    {
        m_TCHardwareStatus.Sensors = 0;
        m_TCHardwareStatus.Fans = 0;
        m_TCHardwareStatus.Heaters = 0;
        m_TCHardwareStatus.Pids = 0;
        m_TCHardwareStatus.Current = UNDEFINED_2_BYTE;
        m_TCHardwareStatus.HeaterSwitchType = 0;
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_TC_GET_HW_STATUS, ReturnCode);
    }
}

}
