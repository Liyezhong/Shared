
#include "DeviceControl/Include/Devices/RotaryValveDevice.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include <sys/stat.h>
#include <QtDebug>
namespace DeviceControl
{

#define CHECK_SENSOR_TIME (200) // in msecs
#define UNDEFINED (999)
const qint32 TOLERANCE = 10; //!< tolerance value for calculating inside and outside range
CRotaryValveDevice::CRotaryValveDevice(DeviceProcessing* pDeviceProcessing, QString Type) : CBaseDevice(pDeviceProcessing, Type),
        m_pTempCtrl(0), m_pMotorRV(0), m_RVCurrentPosition(RV_UNDEF)
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    //m_TaskID         = RVDEV_TASKID_INIT;
    m_ErrorTaskState   = RV_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_ROTARY_VALVE;

    FILE_LOG_L(laDEV, llINFO) << "rotary valve device created";
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CRotaryValveDevice
 */
/****************************************************************************/
CRotaryValveDevice::~CRotaryValveDevice()
{/*
    try
    {
    m_pDevProc = 0;
    if (m_pTempCtrl)
    {
        delete m_pTempCtrl;
        m_pTempCtrl = 0;
    }
    if(m_pMotorRV)
    {
        delete m_pMotorRV;
        m_pMotorRV = 0;
    }
    }
    catch(...)
    {
    return;
    }*/
}

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

    // RetVal = m_pTempCtrl->ClearErrorTask();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        //  RetVal = m_pMotorRV->ClearErrorTask();
    }

    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        //m_TaskID = RVDEV_TASKID_FREE;

    }

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
            m_ErrorTaskState = RV_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
            //m_TaskID = RVDEV_TASKID_FREE;
            /// \todo maybe we need a state to ensure the reference run call!!
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
        //RetVal = ConfigureDeviceTasks();
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

    FILE_LOG_L(laDEV, llINFO) << "  CRotaryValveDevice::HandleInitializationState()";

    m_pTempCtrl = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_RVTempCtrlKey));
    if(m_pTempCtrl == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RV_TEMPCONTROL not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pMotorRV = (CStepperMotor*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_RVMotorKey));
    if(m_pMotorRV == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RV_MOTOR);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RV_MOTOR not allocated.";
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
                this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
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
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportActStatus'failed.";
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
    CheckSensorsData();
#if 0
    if(m_TaskID == RVDEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_TaskID == RVDEV_TASKID_COMMAND_HDL)
    {
        //HandleCommandRequestTask();
        //HandleDeviceTaskActions();
    }
    else if(m_TaskID == RVDEV_TASKID_ERROR)
    {
        //Just wait for 'ExitErrormode'
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_TaskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_RV_DEV, ERROR_DCL_OVEN_DEV_INVALID_STATE, (quint16) m_TaskID);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = RV_DEV_ERRTASK_STATE_REPORT_IFACE;
    }
#endif
}
void CRotaryValveDevice::CheckSensorsData()
{
    if(m_pTempCtrl)
    {
        GetTemperatureAsync(0);
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
        /*if(m_pIOvenDev != 0)
    {
    m_pIOvenDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
    }*/
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
 *
 *  \return true, if the temperature set succeeded, else false
 *
 */
/****************************************************************************/
bool CRotaryValveDevice::SetTemperature(qreal NominalTemperature, quint8 SlopeTempChange)
{
    m_TargetTemperature = NominalTemperature;
    ReturnCode_t retCode;
    if(m_pTempCtrl != NULL)
    {
        retCode = m_pTempCtrl->SetTemperature(NominalTemperature, SlopeTempChange);
    }
    else
    {
        return false;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return false;
    }
    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_SET_TEMP);
    return (DCL_ERR_FCT_CALL_SUCCESS == retCode);
}
void CRotaryValveDevice::OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature)
{
    Q_UNUSED(Temperature)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: RV Set temperature successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: RV set temperature failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_SET_TEMP, ReturnCode);
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
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
qreal CRotaryValveDevice::GetTemperature(quint32 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue = m_CurrentTemperature;
    if((Now - m_LastGetTempTime[Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        ReturnCode_t retCode = m_pTempCtrl->ReqActTemperature(Index);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode )
        {
            RetValue = UNDEFINED;
        }
        else
        {
            retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_GET_TEMP);
            if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                RetValue = UNDEFINED;
            }
            else
            {
                RetValue = m_CurrentTemperature;
            }
            m_LastGetTempTime[Index] = Now;
        }
    }
    return RetValue;
}
bool CRotaryValveDevice::GetTemperatureAsync(quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetTempTime[Index]) >= CHECK_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        m_LastGetTempTime[Index] = Now;
        return ( DCL_ERR_FCT_CALL_SUCCESS== m_pTempCtrl->ReqActTemperature(Index));
    }
    return true;
}
qreal CRotaryValveDevice::GetRecentTemperature(quint32 Index)
{
    QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue = UNDEFINED;
    if((Now - m_LastGetTempTime[Index]) <= 500) // check if 500 msec has passed since last read
    {
        RetValue = m_CurrentTemperature;
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
        m_CurrentTemperature = Temp;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL get temperature failed! " << ReturnCode;
        m_CurrentTemperature = UNDEFINED;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_GET_TEMP, ReturnCode);
}
bool CRotaryValveDevice::SetTemperatureControlStatus(TempCtrlStatus_t TempCtrlStatus)
{
    m_TargetTempCtrlStatus = TempCtrlStatus;
    ReturnCode_t retCode;
    if(m_pTempCtrl != NULL)
    {
        retCode = m_pTempCtrl->SetStatus(TempCtrlStatus);
        return (DCL_ERR_FCT_CALL_SUCCESS== retCode);
    }
    else
    {
        return false;
    }
}
ReturnCode_t CRotaryValveDevice::SetTempCtrlON()
{
    if( SetTemperatureControlStatus( TEMPCTRL_STATUS_ON))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}

ReturnCode_t CRotaryValveDevice::SetTempCtrlOFF()
{
    if(SetTemperatureControlStatus(TEMPCTRL_STATUS_OFF))
    {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }
    else
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
}

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
        return m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_SET_TEMP_PID);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
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
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: RV set temperature PID failed! " << ReturnCode;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_SET_TEMP_PID, ReturnCode);
}


ReturnCode_t CRotaryValveDevice::StartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange)
{
    m_TargetTemperature = NominalTemperature;
    m_TargetTempCtrlStatus = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState() == TEMPCTRL_STATE_ERROR)
    {
        // Log(tr("Not able to read the temperature control status"));
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn())
    {
        return DCL_ERR_DEV_TEMP_CTRL_ALREADY_ON;
    }
    if (IsTemperatureControlOff())
    {
        //Set the nominal temperature
        if (!SetTemperature( NominalTemperature, SlopeTempChange))
        {
            // Log(tr("Not able to set temperature"));
            return DCL_ERR_DEV_TEMP_CTRL_SET_TEMP_ERR;
        }
        //ON the temperature control
        if (!SetTemperatureControlStatus( TEMPCTRL_STATUS_ON))
        {
            // Log(tr("Not able to start temperature control"));
            return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
        }
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t CRotaryValveDevice::StartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    ReturnCode_t retCode;
    m_TargetTemperature = NominalTemperature;
    m_TargetTempCtrlStatus = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState() == TEMPCTRL_STATE_ERROR)
    {
        // Log(tr("Not able to read the temperature control status"));
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn())
    {
        if(!SetTemperatureControlStatus(TEMPCTRL_STATUS_OFF))
        {
            return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
    }
    }

    retCode = SetTemperaturePid(MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
    if(retCode != DCL_ERR_FCT_CALL_SUCCESS)
    {
         return retCode;
    }
    //Set the nominal temperature
    if (!SetTemperature(NominalTemperature, SlopeTempChange))
    {
        // Log(tr("Not able to set temperature"));
        return DCL_ERR_DEV_TEMP_CTRL_SET_TEMP_ERR;
    }
    //ON the temperature control
    if (!SetTemperatureControlStatus(TEMPCTRL_STATUS_ON))
    {
        // Log(tr("Not able to start temperature control"));
        return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

TempCtrlState_t CRotaryValveDevice::GetTemperatureControlState()
{
    ReturnCode_t retCode = m_pTempCtrl->ReqStatus();
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
        m_CurrentTempCtrlStatus = TEMPCTRL_STATUS_UNDEF;
        return TEMPCTRL_STATE_ERROR;
    }
    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_GET_TEMP_CTRL_STATE);
    TempCtrlState_t controlstate;
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        controlstate = TEMPCTRL_STATE_ERROR;
    }
    else if (IsTemperatureControlOn())
    {
        if (IsInsideRange())
        {
            controlstate = TEMPCTRL_STATE_INSIDE_RANGE;
        }
        else if (IsOutsideRange())
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
void CRotaryValveDevice::OnTempControlStatus(quint32 InstanceID, ReturnCode_t ReturnCode,
                                             TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage)
{
    Q_UNUSED(InstanceID)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        m_CurrentTempCtrlStatus = TempCtrlStatus;
        m_MainsVoltageStatus = MainsVoltage;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_GET_TEMP_CTRL_STATE, ReturnCode);
}

bool CRotaryValveDevice::IsTemperatureControlOn()
{
    return (m_CurrentTempCtrlStatus == TEMPCTRL_STATUS_ON);
}

bool CRotaryValveDevice::IsTemperatureControlOff()
{
    return (m_CurrentTempCtrlStatus == TEMPCTRL_STATUS_OFF);
}
bool CRotaryValveDevice::IsInsideRange()
{
    if(GetTemperature(0) != UNDEFINED)
    {
        if((m_TargetTemperature != UNDEFINED) || (m_CurrentTemperature != UNDEFINED))
        {
            if ((m_CurrentTemperature > m_TargetTemperature - TOLERANCE)||
                            (m_CurrentTemperature < m_TargetTemperature + TOLERANCE))
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

bool CRotaryValveDevice::IsOutsideRange()
{
    if(GetTemperature(0) != UNDEFINED)
    {
        if((m_TargetTemperature != UNDEFINED) || (m_CurrentTemperature != UNDEFINED))
        {
            if ((m_CurrentTemperature < m_TargetTemperature - TOLERANCE)||
                            (m_CurrentTemperature > m_TargetTemperature + TOLERANCE))
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
        ParaChange |= true;
    }
    if(GetConfigRefRunPosition() != 3)
    {
        SetConfigRefRunPosition(3);
        ParaChange |= true;
    }
    if(GetConfigPos1Stop() != 0)
    {
        SetConfigPos1Stop(0);
        ParaChange |= true;
    }
    if(GetConfigPos3Stop() != 1)
    {
        SetConfigPos3Stop(1);
        ParaChange |= true;
    }
    if(ParaChange)
    {
        ApplyNewParameterSet();
    }
    lsCode = GetLimitSwitchCode();
    if(lsCode == "3")
    {
        //Log(tr("Already At Initial Position, No Need To Move!"));
        //Log("Already At Initial Position, No Need To Move!");
        qDebug()<< "Already At Initial Position, No Need To Move!";
        SetEDPosition(RV_TUBE_1);
        return RetValue;
    }
    else if((lsCode == "request error")||(lsCode == "error"))
    {
        //Log(tr("Already At Initial Position, No Need To Move!"));
        //Log("Already At Initial Position, No Need To Move!");
        qDebug()<< "Error when read LS code";
        RetValue = DCL_ERR_FCT_CALL_FAILED;
        return RetValue;
    }
#if 1
    //RetValue = ReferenceRunWithTimeout(REFER_RUN_LOWER_LIMIT, 65000);
    DoReferenceRunWithStepCheck(50, 7000);
#else
    RetValue = DoReferenceRun();
#endif
    lsCode = GetLimitSwitchCode();
    if((lsCode == "3")&&(RetValue))
    {
        //Log(tr("Hit Initial Position!"));
        qDebug() << "Hit Initial Position";
        SetEDPosition(RV_TUBE_1);
    }
    else
    {
        SetEDPosition(RV_UNDEF);
        qDebug() << "Hit unexpected position, please retry!";
        // Log(tr("Hit unexpected position, please retry!"));
        // Log(tr("The Limit Switch code is: %1").arg(lsCode));
    }
    return RetValue;
}
//#define REFER_RUN_OK                  (1)
//#define REFER_RUN_DCL_ERR             (-1)
//#define REFER_RUN_TO_UPPER_LIMIT_ERR  (-2)
//#define REFER_RUN_TO_LOWER_LIMIT_ERR  (-3)
//#define REFER_RUN_LOWER_LIMIT         (600)
//#define REFER_RUN_TIMEROUT_IN_MS      (1100)
#define REFER_RUN_RETRY_TIME          (20)

bool CRotaryValveDevice::DoReferenceRunWithStepCheck(quint32 LowerLimit, quint32 UpperLimit)
{
    qint32 ret = true;

    bool stop = false;
    quint8 retry = 0;
    quint8 GetPositionRetry = 0;
    quint32 Step = 0;
    while(!stop)
    {
        stop = true;
        DoReferenceRun();
        while((GetPositionRetry++) <5)
        {
            bool ok;
            Step = qAbs(GetPosition().toInt(&ok));
            //Log(tr("Steps is: %1").arg(Step));
            if(Step > 0)
            {
                break;
            }
            usleep(500*1000);//should sleep for 0.5 sec
        }
        if(Step < LowerLimit)
        {
            if((retry++) < REFER_RUN_RETRY_TIME)
            {
                stop = false;
                //   Log(tr("Motor running lower limit exception: retry time: %1, retry now.").arg(retry));
            }
            else
            {
                //  Log(tr("Motor moving retry time exceed %1, may be stucked!").arg(REFER_RUN_RETRY_TIME));
                ret = false;
            }
        }
        else if(Step > UpperLimit)
        {
            //  Log(tr("Warning: Motor moving steps exceed upper limit: %1!").arg(UpperLimit));
            //ret = false;
        }

    }

    return ret;
}
ReturnCode_t CRotaryValveDevice::ReqMoveToRVPosition( RVPosition_t RVPosition)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    RVPosition_t EDPosition = GetEDPosition();
    float MoveSteps = 0;
    bool cw = false;

    QString lsCode, lsCodeDup;
    quint8 retry = 0;

    if(EDPosition == 0)
    {
        retCode = DCL_ERR_FCT_CALL_FAILED;
    }

    if(0 == EDPosition)
    {
        lsCode = GetLimitSwitchCode();
        lsCodeDup = GetLimitSwitchCode();
        while(lsCode != lsCodeDup)
        {
            lsCode = GetLimitSwitchCode();
            lsCodeDup = GetLimitSwitchCode();
            if(retry++ > 3)
            {
                //                 Log(tr("Limit Switch code are not stable!"));
                qDebug() << "Limit Switch code are not stable!";
                retCode = DCL_ERR_FCT_CALL_FAILED;
                return retCode;
            }
        }
        if(lsCode == "3")
        {
            EDPosition = RV_TUBE_1;
            SetEDPosition(EDPosition);
        }
        else
        {
            //Log(tr("Can't find current position, please run MoveToInitialPosition first!"));
            qDebug()<<"Can't find current position, please run MoveToInitialPosition first!";
            retCode = DCL_ERR_FCT_CALL_FAILED;
            return retCode;
        }
    }
    if((RVPosition < 1)||(RVPosition > 32))
    {
        //Log(tr("The Tube No You Input: %1 is Invalid").arg(Position));
        qDebug() << "The Tube Posotion No You Input: %1 is Invalid" << RVPosition;
        retCode = DCL_ERR_FCT_CALL_FAILED;
        return retCode;
    }

    if(EDPosition == RVPosition)
    {
        //Log(tr("Already At Target Position, No Need To Move!").arg(EDPosition));
        qDebug()<<"Already At Target Position, No Need To Move!" << RVPosition;
        retCode = DCL_ERR_FCT_CALL_SUCCESS;
        return retCode;
    }
    else
    {
        MoveSteps = ((RVPosition > EDPosition)?(RVPosition - EDPosition):(EDPosition - RVPosition));
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
    if(cw)
    {
        for(int i =0;i<MoveSteps;i++)
        {
            if(!MoveToNextPortCW())
            {
                retCode = DCL_ERR_FCT_CALL_FAILED;
                return retCode;
            }
        }
    }
    else
    {
        for(int i =0;i<MoveSteps;i++)
        {
            if (!MoveToNextPortCCW())
            {
                retCode = DCL_ERR_FCT_CALL_FAILED;
                return retCode;
            }
        }
    }
    return retCode;

}
RVPosition_t CRotaryValveDevice::ReqActRVPosition()
{
    return m_RVCurrentPosition;
}
bool CRotaryValveDevice::MoveToNextPortCW()
{
    bool ret = true;
    RVPosition_t EDPosition = GetEDPosition();

    if(0 == EDPosition)
    {
        //Log(tr("Can't find current position, please run MoveToInitialPosition first!"));
        qDebug()<<"Can't find current position, please run MoveToInitialPosition first!";
        return false;
    }

    if( DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW == GetRotationDirection())
    {
        SetRotationDirection(DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW );
        ret = MoveToNextPort(true, \
                             GetLowerLimit(EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, true), \
                             GetUpperLimit(EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, true));
    }
    else
    {
        ret = MoveToNextPort(false, \
                             GetLowerLimit(EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, false), \
                             GetUpperLimit(EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, false));
    }

    if(ret&&(3 == EDPosition))
    {
        ret = MoveToNextPort(false, \
                             GetLowerLimit(99, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, false), \
                             GetUpperLimit(99, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW, false));
    }

    if((ret)&&(EDPosition == GetEDPosition()))
    {
        EDPosition = (RVPosition_t)((quint32)EDPosition - 1);
        if(EDPosition == RV_UNDEF)
        {
            EDPosition = RV_SEAL_16;
        }
        SetEDPosition(EDPosition);
        //Log(tr("CW Hit Position: %1").arg(TranslateFromEDPosition(EDPosition)));
        qDebug()<<"CW Hit Position: %1" << TranslateFromEDPosition(EDPosition);
    }
    else
    {
        //Log(tr("Unknown error happened, lost current position, please run MoveToInitialPosition"));
        qDebug()<<"Unknown error happened, lost current position, please run MoveToInitialPosition";
        SetEDPosition(RV_UNDEF);
        ret = false;
    }
    return ret;
}
/****************************************************************************/
/*!
 *  \brief  Helper function: Get the name of certain encoder disk postion.
 *
 *
 *  \return The name of certain position.
 *
 */
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
 *  \brief  Helper function: Get the upper limit for movment at certain encoder disk position .
 *
 *
 *  \return the upper limit
 *
 */
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
                UpperLimit = 2000;
            }
            else if(24 == CurrentEDPosition)
            {
                UpperLimit = 496;
            }
            else if(26 == CurrentEDPosition)
            {
                UpperLimit = 2000;
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
                UpperLimit = 2000;
            }
            else if(23 == CurrentEDPosition)
            {
                UpperLimit = 496;
            }
            else if(25 == CurrentEDPosition)
            {
                UpperLimit = 2000;
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
        UpperLimit += 50;
    }
    return UpperLimit;
}

/****************************************************************************/
/*!
 *  \brief  Helper function: Get the lower limit for movment at certain encoder disk position .
 *
 *
 *  \return the lower limit
 *
 */
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
 *
 *  \return true, if the reference run succeeded, else false
 *
 */
bool CRotaryValveDevice::MoveToNextPortCCW()
{
    bool ret = true;
    RVPosition_t EDPosition = GetEDPosition();

    if(0 == EDPosition)
    {
        //Log(tr("Can't find current position, please run MoveToInitialPosition first!"));
        qDebug() << "Can't find current position, please run MoveToInitialPosition first!";
        return false;
    }

    if( DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW == GetRotationDirection())
    {
        SetRotationDirection(DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW );
        ret = MoveToNextPort(true, \
                             GetLowerLimit(EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, true), \
                             GetUpperLimit(EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, true));
    }
    else
    {
        ret = MoveToNextPort(false, \
                             GetLowerLimit(EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, false), \
                             GetUpperLimit(EDPosition, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, false));
    }
    if(2 == EDPosition)
    {
        ret = MoveToNextPort(false, \
                             GetLowerLimit(99, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, false), \
                             GetUpperLimit(99, DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW, false));
    }

    if((ret)&&(EDPosition == GetEDPosition()))
    {
        quint32 tempPosition = ((quint32)EDPosition + 1);
        if(tempPosition == 33)
        {
            tempPosition = (quint32)RV_TUBE_1;
        }
        SetEDPosition((RVPosition_t)tempPosition);
        //Log(tr("CCW Hit Position: %1").arg(TranslateFromEDPosition(EDPosition)));
        qDebug() << "CCW Hit Position: %1" << TranslateFromEDPosition(EDPosition);
    }
    else
    {
        //Log(tr("Unknown error happened, lost current position, please run MoveToInitialPosition"));
        qDebug() << "Unknown error happened, lost current position, please run MoveToInitialPosition";
        SetEDPosition(RV_UNDEF);
        ret = false;
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
bool CRotaryValveDevice::MoveToNextPort(bool changeParameter, quint32 LowerLimit, quint32 UpperLimit)
{
    bool ParaChange = changeParameter;
    QString lsCode;
    bool RetValue;
    if(GetConfigLS2Exists()==1)
    {
        //EnableLs2(0);
        SetConfigLS2Exists(0);
        SetConfigPos2Value(0);
        SetConfigPos3Value(0);
        ParaChange |= true;
    }
    if(GetConfigRefRunPosition() != 1)
    {
        SetConfigRefRunPosition(1);
        ParaChange |= true;
    }
    if(GetConfigPos1Stop() != 1)
    {
        SetConfigPos1Stop(1);
        ParaChange |= true;
    }
    if(GetConfigPos3Stop() != 0)
    {
        SetConfigPos3Stop(0);
        ParaChange |= true;
    }
    if(ParaChange)
    {
        ApplyNewParameterSet();
    }
#if 1
    //static quint32 LastED = 0;
    RVPosition_t ED = GetEDPosition();
    //Log(tr("last ED is: %1, lower limit is: %2, upper limit is %3 ").arg(ED).arg(LowerLimit).arg(UpperLimit));
    qDebug()<<"last ED is: %1, lower limit is: %2, upper limit is %3 " << ED << LowerLimit << UpperLimit;
    //RetValue = ReferenceRunWithTimeout(LowerLimit, UpperLimit);
    RetValue = DoReferenceRunWithStepCheck(LowerLimit, UpperLimit);
#else
    RetValue = DoReferenceRun();
#endif
    lsCode = GetLimitSwitchCode();
    if(lsCode != "1")
    {
        quint32 Retry = 0;
        //while(((Retry++) < 3)&&(lsCode != "1"))  // 2013.3.8 Frank's request
        while((Retry++) < 30)
        {
            //Log(tr("Warning: Get unexpected LS Code: %1, wait 0.5 sec to read again.").arg(lsCode));
            qDebug() << "Warning: Get unexpected LS Code: %1, wait 0.5 sec to read again." << lsCode;
            usleep(500*1000);
            lsCode = GetLimitSwitchCode();
        }
        if(Retry >= 30) // 2013.3.8 Frank's request
        {
            //Log(tr("Hit unexpected position! Already read LS code for 30 times"));
            qDebug() << "Hit unexpected position! Already read LS code for 30 times";
            //RetValue = false; // 2013.3.8 Frank's request
        }
    }
    return RetValue;
}

quint8 CRotaryValveDevice::GetRotationDirection()
{
    return m_Config.PosCode1.bStopDir;
}
bool CRotaryValveDevice::SetRotationDirection(quint8 direction)
{
    if(direction == DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CW)
    {
        m_Config.PosCode1.bStopDir = 1;
        m_Config.PosCode2.bStopDir = 2;
        //
    }
    else if(direction == DeviceControl::CANFctModuleStepperMotor::ROTATION_DIR_CCW)
    {
        m_Config.PosCode1.bStopDir = 2;
        m_Config.PosCode2.bStopDir = 1;
    }
    return true;
}

QString CRotaryValveDevice::GetPosition()
{
    //    bool ok = HandleErrorCode(m_pMotorRV->ReqActPosition());
    bool ok = true;
    m_pMotorRV->ReqActPosition();
    if (!ok) {
        // TODO: use const String from BaseWrapper
        return "request error";
    }
    ReturnCode_t ret =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_GET_MOTOR_POSITION);

    QString position = "error";
    if (ret == DCL_ERR_FCT_CALL_SUCCESS) {
        position.setNum(m_CurrentPosition);
    }
    return position;
}

QString CRotaryValveDevice::GetLimitSwitchCode()
{

    ReturnCode_t retCode;
    retCode = m_pMotorRV->ReqActPosition();
    if (retCode != DCL_ERR_FCT_CALL_SUCCESS)
    {
        // TODO: use const String from BaseWrapper
        return "request error";
    }
    ReturnCode_t ret = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_GET_MOTOR_POSITION);

    QString code = "error";
    if (ret == DCL_ERR_FCT_CALL_SUCCESS) {
        code.setNum(m_CurrentLimitSwitchCode);
    }
    return code;
}
void CRotaryValveDevice::OnGetPosition(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qint32 Position, qint8 PosCode)
{
    m_CurrentPosition = Position;
    m_CurrentLimitSwitchCode = PosCode;
    if (ReturnCode != DCL_ERR_FCT_CALL_SUCCESS) {
        qDebug()<< "Warning! Unexpected return code at OnGetPosition";
    } else {
        //m_CurrentPositionKnown = true;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_GET_MOTOR_POSITION, ReturnCode);
    // if (m_RVLoopGetPosition.isRunning()) {
    //     m_RVLoopGetPosition.exit(ret);
    // } else {
    //    // Log(tr("NOTICE: Unexpected action acknowledgement for GetPosition (p: %1, LS code: %2).").arg(m_CurrentPosition).arg(PosCode));
    // }
}


bool CRotaryValveDevice::SetConfigLS2Exists(quint8 bExists)
{
    m_Config.LimitSwitch2.bExists = bExists;
    return true;
}
quint8 CRotaryValveDevice::GetConfigLS2Exists()
{
    return m_Config.LimitSwitch2.bExists;
}
bool CRotaryValveDevice::SetConfigPos2Value(quint8 bValid)
{
    m_Config.PosCode2.bValid = bValid;
    return true;
}

quint8 CRotaryValveDevice::GetConfigPos2Value()
{
    return m_Config.PosCode2.bValid;
}

bool CRotaryValveDevice::SetConfigPos3Value(quint8 bValid)
{
    m_Config.PosCode3.bValid = bValid;
    return true;
}

quint8 CRotaryValveDevice::GetConfigPos3Value()
{
    return m_Config.PosCode3.bValid;
}
qint8 CRotaryValveDevice::GetConfigRefRunPosition()
{
    return m_Config.refRunRefPos;
}
bool CRotaryValveDevice::SetConfigRefRunPosition(qint8 refRunRefPos)
{
    m_Config.refRunRefPos =  refRunRefPos;
    return true;
}
bool CRotaryValveDevice::SetConfigPos1Stop(quint8 bStop)
{
    m_Config.PosCode1.bStop = bStop;
    return true;
}
bool CRotaryValveDevice::SetConfigPos2Stop(quint8 bStop)
{
    m_Config.PosCode2.bStop = bStop;
    return true;
}

quint8 CRotaryValveDevice::GetConfigPos2Stop()
{
    return m_Config.PosCode2.bStop;
}

bool CRotaryValveDevice::SetConfigPos3Stop(quint8 bStop)
{
    m_Config.PosCode3.bStop = bStop;
    return true;
}

quint8 CRotaryValveDevice::GetConfigPos3Stop()
{
    return m_Config.PosCode3.bStop;
}

quint8 CRotaryValveDevice::GetConfigPos1Stop()
{
    return m_Config.PosCode1.bStop;
}
RVPosition_t CRotaryValveDevice::GetEDPosition()
{
    if(m_RVCurrentPosition > 32)
    {
        m_RVCurrentPosition = RV_UNDEF;
    }

    return m_RVCurrentPosition;
}
void CRotaryValveDevice::SetEDPosition(RVPosition_t position)
{

    if((position <33) &&( position >0))
    {
        m_RVCurrentPosition= position;
    }
    else
    {
        m_RVCurrentPosition = RV_UNDEF;
    }
}
void CRotaryValveDevice::InitEDPosition()
{
    m_RVCurrentPosition =RV_UNDEF ;
}

void CRotaryValveDevice::RestoreLastValidParameterSet()
{
    m_Config = *m_pMotorRV->GetConfiguration();
}
void CRotaryValveDevice::Reset()
{
    RestoreLastValidParameterSet();
    // m_SetPositionActive = false;
    // m_RotationOn = false;
    // m_CurrentPositionKnown = false;
    m_CurrentPosition = 0;
    // m_TargetPosition  = 0;
    m_CurrentLimitSwitchCode = 0;
    //  m_CurrentSpeed = 0;
    //  m_TargetSpeed  = 0;
    // m_Speed  = 400;
    // m_Speed  = m_Config.listMotionProfiles[0].sSpeedMax;
    //  m_Profile = 0;
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_ErrorTaskState   = RV_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_ROTARY_VALVE;

    memset( &m_LastGetTempTime, 0 , sizeof(m_LastGetTempTime));
    m_TargetTempCtrlStatus = TEMPCTRL_STATUS_UNDEF;
    m_CurrentTempCtrlStatus = TEMPCTRL_STATUS_UNDEF;
    m_CurrentTemperature = 0;
    m_TargetTemperature = 0;
    memset( &m_MainsVoltageStatus, 0 , sizeof(m_MainsVoltageStatus));




}

bool CRotaryValveDevice::ApplyNewParameterSet()
{

    // disable function module before reconfiguration
    if (!SetMotorState(false)) {
        return false;
    }

    // bool ok = HandleErrorCode(m_pMotorRV->DoReconfiguration(&m_Config));
    ReturnCode_t retCode =  m_pMotorRV->DoReconfiguration(&m_Config);
    if ( retCode !=DCL_ERR_FCT_CALL_SUCCESS) {
        return false;
    }
    ReturnCode_t ret = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_SET_MOTOR_CONFIG);
    sleep(1);//in seconds

    if (!SetMotorState(true)) {
        return false;
    }

    return (ret == DCL_ERR_FCT_CALL_SUCCESS);
}
bool CRotaryValveDevice::DoReferenceRun()
{
    // enable function module before first use
    if (!SetMotorState(true)) {
        return false;
    }

    ReturnCode_t retCode = m_pMotorRV->ExecReferenceMovement();
    if (retCode != DCL_ERR_FCT_CALL_SUCCESS) {
        return false;
    }
    ReturnCode_t ret = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_REF_RUN);

    return (ret == DCL_ERR_FCT_CALL_SUCCESS);
}
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
        qDebug()<< "Warning! Unexpected return code at OnReferenceRun";
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_REF_RUN, ReturnCode);
}
void CRotaryValveDevice::OnSetConfiguration(quint32 /*InstanceID*/, ReturnCode_t ReturnCode)
{
    // exit from eventloop: 1 success, 0 timeout, -1 failure
    if (ReturnCode != DCL_ERR_FCT_CALL_SUCCESS) {
        qDebug()<< "Warning! Unexpected return code at OnSetConfiguration";
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_SET_MOTOR_CONFIG, ReturnCode);

}

bool CRotaryValveDevice::SetMotorState(bool flag)
{

    ReturnCode_t retCode = m_pMotorRV->SetMotorState(flag);

    if (retCode != DCL_ERR_FCT_CALL_SUCCESS) {
        return false;
    }
    ReturnCode_t ret =m_pDevProc->BlockingForSyncCall(SYNC_CMD_RV_SET_MOTOR_STATE);

    return (ret == DCL_ERR_FCT_CALL_SUCCESS);
}

void CRotaryValveDevice::OnSetMotorState(quint32 /*InstanceID*/, ReturnCode_t ReturnCode)
{
    // exit from eventloop: 1 success, 0 timeout, -1 failure
    if ( DCL_ERR_FCT_CALL_SUCCESS  !=ReturnCode) {
        qDebug()<< "Warning! Unexpected return code at OnSetMotorState";
    }

    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RV_SET_MOTOR_STATE, ReturnCode);
}

}
