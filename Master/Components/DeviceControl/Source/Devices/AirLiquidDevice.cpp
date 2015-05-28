#include <QCoreApplication>
#include "DeviceControl/Include/Devices/AirLiquidDevice.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/PressureControl.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include <sys/stat.h>
#include <QtDebug>
#include "DeviceControl/Include/DeviceProcessing/DeviceLifeCycleRecord.h"

namespace DeviceControl
{
#define CHECK_SENSOR_TIME               (800) // in msecs
#define CHECK_PRESSURE_SENSOR_TIME      (200) // in msecs
#define CHECK_CURRENT_TIME              (900) // in msecs

//#define AL_TARGET_PRESSURE_POSITIVE     (30)
//#define AL_TARGET_PRESSURE_NEGATIVE     (-30)
#define AL_TARGET_PRESSURE_BOTTLECHECK  (10)
#define AL_PUMP_MODE_OFF                (0)
#define AL_PUMP_MODE_ON_OFF_POSITIVE    (1)
#define AL_PUMP_MODE_ON_OFF_NEGATIVE    (9)
#define AL_PUMP_MODE_PWM_POSITIVE       (17)
#define AL_PUMP_MODE_PWM_NEGATIVE       (25)
#define AL_RELEASE_PRESSURE_VALUE       (2)
const qint32 TOLERANCE = 10; //!< tolerance value for calculating inside and outside range

/****************************************************************************/
/*!
 *  \brief    Constructor of the CAirLiquidDevice class
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *  \param    ModuleList = Module List
 *  \param    InstanceID = instance  id
 */
/****************************************************************************/
CAirLiquidDevice::CAirLiquidDevice(DeviceProcessing* pDeviceProcessing,
                                   QString& Type,
                                   const DeviceModuleList_t &ModuleList,
                                   quint32 InstanceID) :
        CBaseDevice(pDeviceProcessing, Type, ModuleList, InstanceID),
        m_pPressureCtrl(0)/*  m_pFanDigitalOutput(0)*/
{
    Reset();
    FILE_LOG_L(laDEV, llINFO) << "Air-liquid device created";
    LogDebug(QString("Air-liquid device created"));
    CONNECTSIGNALSIGNAL(this,SigStopCommandExec(quint8),m_pDevProc,SigStopCommandExec(quint8));
} //lint !e1566

/****************************************************************************/
/*!
 *  \brief  Destructor of CAirLiquidDevice
 */
/****************************************************************************/
CAirLiquidDevice::~CAirLiquidDevice()
{
    try
    {
        Reset();
    }
    catch(...)
    {
        return;
    }
}    //lint !e1579

/****************************************************************************/
/*!
 *  \brief  Reset class member variable
 */
/****************************************************************************/
void CAirLiquidDevice::Reset()
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_ErrorTaskState   = AL_DEV_ERRTASK_STATE_FREE;

    m_PressureDrift = 0;
    m_instanceID = DEVICE_INSTANCE_ID_AIR_LIQUID;
    m_CurrentPressure = UNDEFINED_4_BYTE;

    m_WorkingPressurePositive = AL_TARGET_PRESSURE_POSITIVE;
    m_WorkingPressureNegative = AL_TARGET_PRESSURE_NEGATIVE;
    if(!m_PIDDataList.isEmpty())
    {
        m_PIDDataList.clear();
    }
    m_pPressureCtrl = NULL;
    memset( &m_LastGetTempTime, 0 , sizeof(m_LastGetTempTime)); //lint !e545
    memset( &m_LastGetTCCurrentTime, 0 , sizeof(m_LastGetTCCurrentTime)); //lint !e545
    m_LastGetPressureTime = 0;
    memset( &m_SuckingTime, 0 , sizeof(m_SuckingTime)); //lint !e545
    memset( &m_TargetTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_TargetTempCtrlStatus));  //lint !e545 !e641
    memset( &m_CurrentTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_CurrentTempCtrlStatus)); //lint !e545 !e641
    memset( &m_CurrentTemperatures, 0 , sizeof(m_CurrentTemperatures)); //lint !e545
    memset( &m_TargetTemperatures, 0 , sizeof(m_TargetTemperatures)); //lint !e545
    memset( &m_MainsVoltageStatus, 0 , sizeof(m_MainsVoltageStatus)); //lint !e545
    memset( &m_pTempCtrls, 0 , sizeof(m_pTempCtrls)); //lint !e545
    memset( &m_TCHardwareStatus, 0 , sizeof(m_TCHardwareStatus)); //lint !e545
    m_TargetPressureCtrlStatus = PRESSURECTRL_STATUS_UNDEF;
    m_CurrentPressureCtrlStatus = PRESSURECTRL_STATUS_UNDEF;
    m_TargetDOOutputValue = 0;
    m_DOLifeTime = 0;
    m_DOLifeCycles = 0;

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
ReturnCode_t CAirLiquidDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CBaseModule* pBaseModule = NULL;
    FILE_LOG_L(laDEV, llINFO) << "  CAirLiquidDevice::HandleInitializationState()";

    quint32 InstanceID;
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_ALPressureCtrlKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pPressureCtrl = (CPressureControl*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pPressureCtrl == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_PRESSURECTRL);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AL_PRESSURECTRL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        if (m_ModuleLifeCycleRecord)
        {
            PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value("AL_pressure_ctrl");
            if (pPartLifeCycleRecord)
            {
                quint32 valve1LifeCycle = pPartLifeCycleRecord->m_ParamMap.value("Valve1_LifeCycle").toUInt();
                quint32 valve2LifeCycle = pPartLifeCycleRecord->m_ParamMap.value("Valve2_LifeCycle").toUInt();
                quint32 activeCarbonFilterLifeTime = pPartLifeCycleRecord->m_ParamMap.value("ActiveCarbonFilter_LifeTime").toUInt();
                quint32 exhaustFanLifeTime = pPartLifeCycleRecord->m_ParamMap.value("Exhaust_Fan_LifeTime").toUInt();

                m_pPressureCtrl->SetValveLifeCycle(valve1LifeCycle, valve2LifeCycle);
                m_pPressureCtrl->SetActiveCarbonFilterLifeTime(activeCarbonFilterLifeTime);
                m_pPressureCtrl->SetExhaustFanLifeTime(exhaustFanLifeTime);
                m_pPressureCtrl->SetPartLifeCycleRecord(pPartLifeCycleRecord);
            }
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_ALLevelSensorTempCtrlKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    m_pTempCtrls[AL_LEVELSENSOR] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        if(m_pTempCtrls[AL_LEVELSENSOR] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AL_LEVELSENSORTEMPCTRL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstTCTypeMap[ CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL ] = AL_LEVELSENSOR; //lint !e641
            if (m_ModuleLifeCycleRecord)
            {
                PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value("AL_level_sensor_temp_ctrl");
                if (pPartLifeCycleRecord)
                {
                    quint32 levelSensorLifeCycle = pPartLifeCycleRecord->m_ParamMap.value(m_pTempCtrls[AL_LEVELSENSOR]->GetKey()+"_LifeCycle").toUInt();
                    m_pTempCtrls[AL_LEVELSENSOR]->SetLifeCycle(levelSensorLifeCycle);
                    m_pTempCtrls[AL_LEVELSENSOR]->SetPartLifeCycleRecord(pPartLifeCycleRecord);
                }
            }
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_ALTube1TempCtrlKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pTempCtrls[AL_TUBE1] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pTempCtrls[AL_TUBE1] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AL_TUBE1TEMPCTRL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstTCTypeMap[ CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL ] = AL_TUBE1; //lint !e641
            if (m_ModuleLifeCycleRecord)
            {
                PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value("temp_tube1");
                if (pPartLifeCycleRecord)
                {
                    m_pTempCtrls[AL_TUBE1]->SetPartLifeCycleRecord(pPartLifeCycleRecord);
                }
            }
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_ALTube2TempCtrlKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pTempCtrls[AL_TUBE2] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pTempCtrls[AL_TUBE2] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_AL_TUBE2TEMPCTRL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstTCTypeMap[ CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL ] = AL_TUBE2; //lint !e641
            if (m_ModuleLifeCycleRecord)
            {
                PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value("temp_tube2");
                if (pPartLifeCycleRecord)
                {
                    m_pTempCtrls[AL_TUBE2]->SetPartLifeCycleRecord(pPartLifeCycleRecord);
                }
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
ReturnCode_t CAirLiquidDevice::HandleConfigurationState()
{
    if(!connect(m_pPressureCtrl, SIGNAL(ReportRefPressure(quint32, ReturnCode_t, float)),
                this, SLOT(OnSetPressure(quint32, ReturnCode_t, float))))
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

    if(!connect(m_pPressureCtrl, SIGNAL(ReportActPressure(quint32, ReturnCode_t, quint8, float)),
                this, SLOT(OnGetPressure(quint32, ReturnCode_t, quint8, float))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_PRESSURECTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect pressure ctrl signal 'ReportActPressure'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pPressureCtrl, SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_PRESSURECTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect pressure ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pPressureCtrl, SIGNAL(ReportRefFanState(quint32, ReturnCode_t, quint8)),
                this, SLOT(OnSetFanStatus(quint32, ReturnCode_t, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_PRESSURECTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect pressure ctrl signal 'ReportError'failed.";
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
/*
    if(!connect(m_pFanDigitalOutput, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_FANDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
*/
    if(!connect(m_pTempCtrls[AL_LEVELSENSOR], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_TUBE1], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_TUBE2], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_LEVELSENSOR], SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
                this, SLOT(OnTCGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_TUBE1], SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
                this, SLOT(OnTCGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[AL_TUBE2], SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
                this, SLOT(OnTCGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

/*
    if(!connect(m_pFanDigitalOutput, SIGNAL(ReportLifeTimeData(quint32, ReturnCode_t, quint32, quint32)),
            this, SLOT(OnGetLifeTime(quint32,  ReturnCode_t, quint32, quint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AL_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_AL_FANDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportLifeTimeData' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
*/
    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief   Read Air-liquid device's sensors data asynchronizely
 */
/****************************************************************************/
void CAirLiquidDevice::CheckSensorsData()
{
    if (!CBaseDevice::m_SensorsDataCheckFlag) {
        return ;
    }

    if(m_pPressureCtrl )
    {
        (void)GetPressureAsync();
    }
    if(m_pTempCtrls[AL_LEVELSENSOR])
    {
        (void)GetTemperatureAsync(AL_LEVELSENSOR, 0);
        (void)GetHeaterCurrentAsync(AL_LEVELSENSOR);
    }
    if(m_pTempCtrls[AL_TUBE1])
    {
        (void)GetTemperatureAsync(AL_TUBE1, 0);
        (void)GetHeaterCurrentAsync(AL_TUBE1);
    }
    if(m_pTempCtrls[AL_TUBE2])
    {
        (void)GetTemperatureAsync(AL_TUBE2, 0);
        (void)GetHeaterCurrentAsync(AL_TUBE2);
    }
}

/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::ConfigureDeviceTasks()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief  Handles the error state of the class.
 */
/****************************************************************************/
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
    }
}

/****************************************************************************/
/*!
 *  \brief    Set specified pressure to Air-liquid device.
 *            This Function will only send the target pressure value to the
 *            slave board.
 *
 *  \param    flag = Pump's working mode
 *  \param    NominalPressure = Target pressure to set
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::SetPressure(quint8 flag, float NominalPressure)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    if(m_pPressureCtrl)
    {
        DCLEventLoop* event = m_pDevProc->CreateSyncCall(SYNC_CMD_AL_SET_PRESSURE);
        float targetPressure = NominalPressure + m_PressureDrift;
        retCode =  m_pPressureCtrl->SetPressure(flag, targetPressure);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        retCode =  m_pDevProc->BlockingForSyncCall(event);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set pressure
 *
 *  This slot is connected to the signal, ReportRefPressure
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam TargetPressure = Actual pressure
 *
 */
/****************************************************************************/
void CAirLiquidDevice::OnSetPressure(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, float TargetPressure)
{
    Q_UNUSED(ReturnCode)
    Q_UNUSED(TargetPressure)

    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_PRESSURE, DCL_ERR_FCT_CALL_SUCCESS);
    }
}

/****************************************************************************/
/*!
 *  \brief    Set  Air-liquid device's valve to specified state.
 *
 *
 *  \param    ValveIndex = Index of the target valve
 *  \param    ValveState = Valve's new state
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::SetValve(quint8 ValveIndex, quint8 ValveState)
{
    ReturnCode_t retCode;
    if(m_pPressureCtrl)
    {
        DCLEventLoop* event = m_pDevProc->CreateSyncCall(SYNC_CMD_AL_SET_VALVE);
        retCode = m_pPressureCtrl->SetValve(ValveIndex, ValveState);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
            return retCode;
        }
        retCode =  m_pDevProc->BlockingForSyncCall(event);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set valve
 *
 *  This slot is connected to the signal, ReportRefValveState
 *
 *  \iparam ReturnCode  = ReturnCode of function level Layer
 *  \iparam ValveIndex  = Actual valve index
 *  \iparam ValveState  = Actual valve state
 *
 */
/****************************************************************************/
void CAirLiquidDevice::OnSetValve(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 ValveIndex, quint8 ValveState)
{
    Q_UNUSED(ValveIndex)
    Q_UNUSED(ValveState)
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_VALVE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief    Get actual pressure of Air-liquid device. This functin will
 *            block caller's thread until slave answered.
 *
 *
 *  \return  The actual pressure of the Air-liquid system. UNDEFINED if any
 *           problem happens.
 */
/****************************************************************************/
qreal CAirLiquidDevice::GetPressure(void)
{
    qreal RetValue = m_CurrentPressure - m_PressureDrift;
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetPressureTime) >= CHECK_PRESSURE_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        if(m_pPressureCtrl)
        {
            ReturnCode_t retCode = m_pPressureCtrl->ReqActPressure(0); //lint -esym(526, DeviceControl::ReqActPressure) -esym(628, DeviceControl::ReqActPressure)
            if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                RetValue = UNDEFINED_4_BYTE;
            }
            else
            {
                retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_GET_PRESSURE);
                if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
                {
                    RetValue = UNDEFINED_4_BYTE;
                }
                else
                {
                    RetValue = m_CurrentPressure - m_PressureDrift;
                }
            }
        }
        else
        {
            RetValue = UNDEFINED_4_BYTE;
        }
        m_LastGetPressureTime = Now;
    }

    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief    Get actual pressure of Air-liquid device asynchronously.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::GetPressureAsync(void)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetPressureTime) >= CHECK_PRESSURE_SENSOR_TIME) // check if 200 msec has passed since last read
    {
        {
            m_LastGetPressureTime = Now;
        }
        if(m_pPressureCtrl)
        {
            return m_pPressureCtrl->ReqActPressure(0);
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
 *  \brief   slot associated with Get pressure
 *
 *  This slot is connected to the signal, ReportActPressure
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam Index = Actual pressure sensor index
 *  \iparam ActPressure = Actual pressure
 *
 */
/****************************************************************************/
void CAirLiquidDevice::OnGetPressure(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 Index, float ActPressure)
{
    Q_UNUSED(Index)
    m_CurrentPressure = ActPressure;
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_GET_PRESSURE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief    Set pressure control's status.
 *
 *  \iparam  PressureCtrlStatus = New pressure control status
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::SetPressureControlStatus(PressureCtrlStatus_t PressureCtrlStatus)
{
    m_TargetPressureCtrlStatus = PressureCtrlStatus;
    ReturnCode_t retCode;
    if(m_pPressureCtrl)
    {
        retCode = m_pPressureCtrl->SetStatus(PressureCtrlStatus);//lint -esym(526, DeviceControl::SetStatus) -esym(628, DeviceControl::SetStatus)
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   Enable pressure control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::SetPressureCtrlON()
{
    return SetPressureControlStatus(PRESSURECTRL_STATUS_ON);
}

/****************************************************************************/
/*!
 *  \brief   Disable pressure control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::SetPressureCtrlOFF()
{
    return SetPressureControlStatus(PRESSURECTRL_STATUS_OFF);
}

/****************************************************************************/
/*!
 *  \brief   Turn off the pump.
 */
/****************************************************************************/
void CAirLiquidDevice::StopCompressor(void)
{
    FILE_LOG_L(laDEVPROC, llINFO) << " INFO: Shut down compressor. ";
    (void)SetPressure(AL_PUMP_MODE_OFF, 1);
}

/****************************************************************************/
/*!
 *  \brief   Release pressure/vaccum in the system, this function will also
 *           break any on-going pump operation related function.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::ReleasePressure(void)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start release pressure procedure";
    LogDebug(QString("INFO: Start release pressure procedure"));
    quint32 TimeSlotPassed = 0;
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_PRESSURE, DCL_ERR_UNEXPECTED_BREAK);
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_DRAINING, DCL_ERR_UNEXPECTED_BREAK);
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_RELEASE_PRESSURE, DCL_ERR_UNEXPECTED_BREAK);
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, DCL_ERR_UNEXPECTED_BREAK);
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_VACCUM, DCL_ERR_UNEXPECTED_BREAK);
    }
    //close both valve
    LogDebug(QString("INFO: Stop Pump"));
    StopCompressor();
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Close Both Valves";
    LogDebug(QString("INFO: Close Both Valves"));
    (void)SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
    (void)SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
    qreal CurrentPressure = GetPressure();

    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Wait for current pressure get to ZERO";
    while(qAbs(CurrentPressure) > AL_RELEASE_PRESSURE_VALUE)
    {
        CurrentPressure = GetPressure();
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_RELEASE_PRESSURE, 500);
        if (DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            LogDebug(QString("WARNING: Current procedure has been interrupted, exit now."));
            return DCL_ERR_UNEXPECTED_BREAK;
        }
        TimeSlotPassed++;
        if(TimeSlotPassed*500 > RELEASE_PRESSURE_MAX_TIME)
        {
            LogDebug(QString("ERROR:  Release Pressure exceed maximum setup time, exit!"));
            //stop compressor
            StopCompressor();
            //close both valve
            (void)SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
            (void)SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
            //(void)TurnOffFan();
            return DCL_ERR_DEV_LA_RELEASING_TIMEOUT;
        }
    }
    //(void)TurnOffFan();
    LogDebug(QString("INFO: Pressure released, exit now."));
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Set-up pre-defied pressure enviroment in the system.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::Vaccum(float targetPressure)
{
    ReturnCode_t RetValue = DCL_ERR_FCT_CALL_SUCCESS;
    if( DCL_ERR_FCT_CALL_SUCCESS != ReleasePressure())
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING:  Release pressure failed, exit now.";
        LogDebug(QString("WARNING:  Release pressure failed, exit now."));
        goto SORTIE;
    }
    //(void)TurnOnFan();
    RetValue = SetTargetPressure(AL_PUMP_MODE_ON_OFF_NEGATIVE, targetPressure);
    if(DCL_ERR_FCT_CALL_SUCCESS != RetValue)
    {
        //stop compressor
        StopCompressor();
        //close both valve
        (void)SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
        (void)SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
       // (void)TurnOffFan();
    }
SORTIE:
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Pressure finished, exit";
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Set-up pre-defied vaccum enviroment in the system.
 *  \param   targetPressure target Pressure
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::Pressure(float targetPressure)
{
    ReturnCode_t RetValue = DCL_ERR_FCT_CALL_SUCCESS;
    if( DCL_ERR_FCT_CALL_SUCCESS != ReleasePressure())
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING:  Release pressure failed, exit now.";
        goto SORTIE;
    }
    //(void)TurnOnFan();
    RetValue = SetTargetPressure(AL_PUMP_MODE_ON_OFF_POSITIVE, targetPressure);
    if(DCL_ERR_FCT_CALL_SUCCESS != RetValue)
    {
        //stop compressor
        StopCompressor();
        //close both valve
        (void)SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
        (void)SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
       // (void)TurnOffFan();
    }
SORTIE:
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Pressure finished, exit";
    return RetValue;
}

ReturnCode_t CAirLiquidDevice::SealingCheckPressure()
{
    ReturnCode_t RetValue = DCL_ERR_FCT_CALL_SUCCESS;
    if( DCL_ERR_FCT_CALL_SUCCESS != ReleasePressure())
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING:  Release pressure failed, exit now.";
        goto SORTIE;
    }
   // (void)TurnOnFan();
    RetValue = SetTargetPressure(AL_PUMP_MODE_ON_OFF_POSITIVE, 15);
    if(DCL_ERR_FCT_CALL_SUCCESS != RetValue)
    {
        //stop compressor
        StopCompressor();
        //close both valve
        (void)SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
        (void)SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
       // (void)TurnOffFan();
    }
SORTIE:
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Pressure finished, exit";
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Drain the system automatically.
 *
 *  \iparam  DelayTime = Delay a small period(in milliseconds) before turn-off
 *                       the pump when retort has been detected empty.
 *  \iparam targetPressure target Pressure
 *  \iparam IgnorePressure Ignore Pressure
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::Draining(quint32 DelayTime, float targetPressure, bool IgnorePressure)
{

    bool stop = false;
    ReturnCode_t RetValue = DCL_ERR_FCT_CALL_SUCCESS;
    qreal CurrentPressure = 0;
    bool PressureHasBeenSetup = false;
    qint32 counter = 0;
    bool WarnShowed = false;
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    qint64 TimeNow = 0;
    qint64 TimeStartPressure = 0;
    qint64 TimeStartDraining = 0;
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start Drainging procedure.";
    LogDebug(QString("INFO: Start Drainging procedure."));
    //release pressure
    RetValue = ReleasePressure();
    if( DCL_ERR_FCT_CALL_SUCCESS != RetValue )
    {
        goto SORTIE;
    }

   // (void)TurnOnFan();
    RetValue = SetTargetPressure(AL_PUMP_MODE_PWM_POSITIVE, targetPressure);
    if(DCL_ERR_FCT_CALL_SUCCESS != RetValue)
    {
        goto SORTIE;
    }
    TimeStartPressure = QDateTime::currentMSecsSinceEpoch();
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Set target pressure finished.";
    LogDebug(QString("INFO: Set target pressure finished."));

    if (false == IgnorePressure)
    {
        PressureHasBeenSetup = false;
    }
    else
    {
        PressureHasBeenSetup = true;
        TimeStartDraining = QDateTime::currentMSecsSinceEpoch();
    }

    while(!stop)
    {
        TimeNow = QDateTime::currentMSecsSinceEpoch();
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_DRAINING, DRAINGING_POLLING_TIME);
        if (DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            LogDebug(QString("WARNING: Current procedure has been interrupted, exit now."));
            RetValue = DCL_ERR_UNEXPECTED_BREAK;
            goto SORTIE;
        }
        CurrentPressure = GetPressure();

        if(!PressureHasBeenSetup)
        {
            if(CurrentPressure >= DRAINGING_TARGET_THRESHOLD_PRESSURE)
            {
                PressureHasBeenSetup = true;
                TimeStartDraining = TimeNow;
                FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Pressure has been set up";
                LogDebug(QString("INFO: Pressure has been set up"));
            }
            else
            {
                if(TimeNow > (TimeStartPressure + DRAINGING_PRESSURE_BUILD_TIME))
                {
                    LogDebug(QString("ERROR: Pressure can't be built up in 2 minutes."));
                    RetValue = DCL_ERR_DEV_LA_DRAINING_TIMEOUT_BULIDPRESSURE;
                    goto SORTIE;
                }
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
                    LogDebug(QString("INFO: Drain finished!"));
                    stop = true;
                }
            }
            else
            {
                counter = 0;
            }

            if((TimeNow > (TimeStartDraining + DRAINGING_SETUP_WARNING_TIME)) && (!WarnShowed))
            {
                FILE_LOG_L(laDEVPROC, llWARNING) << "Warning: Draining do not finished in expected time";
                LogDebug(QString("WARNING: Draining do not finished in expected time"));
                WarnShowed = true;
                m_pDevProc->OnReportDrainingTimeOut2Min();
            }
            if(TimeNow > (TimeStartDraining + DRAINGING_MAX_SETUP_TIME))
            {
                FILE_LOG_L(laDEVPROC, llWARNING) << "Warning: Draining exceed maximum setup time(%1 seconds), exit!";
                LogDebug(QString("WARNING: Draining exceed maximum setup time(%1 seconds), exit!"));
                RetValue = DCL_ERR_DEV_LA_DRAINING_TIMEOUT_EMPTY_4MIN;
                goto SORTIE;
            }
        }
    }

    //waiting for some time
    if(DelayTime > 0)
    {
        LogDebug(QString("INFO: Draining Finished. start hold for %1 millisecond.").arg(DelayTime));
        m_pDevProc->BlockingForSyncCall(SYNC_CMD_TOTAL_DELAY, DelayTime);
    }

SORTIE:
    ReleasePressure();
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Fill the system automatically.
 *
 *  \iparam  DelayTime = Delay a small period (in Millisecodns) before turn-off
 *                       the pump when retort has been detected full.
 *  \iparam  EnableInsufficientCheck = Enable Insufficient Check
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::Filling(quint32 DelayTime, bool EnableInsufficientCheck)
{
    ReturnCode_t RetValue = DCL_ERR_FCT_CALL_SUCCESS;
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    QList<qreal> PressureBuf;
    QList<qreal> AllPressureBuf;
    int levelSensorState = 0xFF;
    bool NeedOverflowChecking = true;
    bool stop = false;
    bool InsufficientCheckFlag = EnableInsufficientCheck;
    bool TimeOutInsufficient = false;
    QList<qreal> InsufficientPressureBuf;
    bool WarnShowed = false;
    qint64 TimeNow, TimeStartPressure, TimeStopFilling;
    TimeStopFilling = 0;
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start Sucking procedure.";
    LogDebug(QString("INFO: Start Filling procedure."));

    //release pressure
    RetValue = ReleasePressure();
    if( DCL_ERR_FCT_CALL_SUCCESS != RetValue )
    {
        goto SORTIE;
    }
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start Sucking now.";
   // (void)TurnOnFan();
    RetValue = SetTargetPressure(AL_PUMP_MODE_PWM_NEGATIVE, m_WorkingPressureNegative);
    if(DCL_ERR_FCT_CALL_SUCCESS != RetValue)
    {
        goto SORTIE;
    }
    TimeStartPressure = QDateTime::currentMSecsSinceEpoch();
    // For Paraffin reagent, we need NOT insufficient check all the time. For Non-paraffin ones, we need
    // check insufficient before FM_TEMP_LEVEL_SENSOR_STATE_1, and NOT need this after STATE_1
    //set timeout to 2 minutes
    while(!stop)
    {
        TimeNow = QDateTime::currentMSecsSinceEpoch();
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, SUCKING_POOLING_TIME);
        if(DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_1 == retCode)
        {
            m_pDevProc->OnReportLevelSensorStatus1();
            if(TimeStopFilling == 0)
            {
                m_pTempCtrls[AL_LEVELSENSOR]->OnLevelSensorStateChanged();
                FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Hit target level. Filling Finished.";
                LogDebug(QString("INFO: Hit target level. Filling Finished."));
                if(DelayTime > 0)
                {
                    NeedOverflowChecking = false;
                    //waiting for some time
                    if(DelayTime < SUCKING_MAX_DELAY_TIME)
                    {
                        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Delay for " << DelayTime<<" milliseconds.";
                        LogDebug(QString("INFO: Delay for %1 milliseconds.").arg(DelayTime));
                        TimeStopFilling = TimeNow + DelayTime;
                    }
                    else
                    {
                        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Delay for " << SUCKING_MAX_DELAY_TIME<<" milliseconds.";
                        LogDebug(QString("INFO: Delay for %1 milliseconds.").arg(SUCKING_MAX_DELAY_TIME));
                        TimeStopFilling = TimeNow + SUCKING_MAX_DELAY_TIME;
                    }
                }
                else
                {
                    stop = true;
                    StopCompressor();
                    return DCL_ERR_FCT_CALL_SUCCESS;
                }
            }
            InsufficientCheckFlag = false;
        }
        else if(DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_0 == retCode)
        {
            FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Received level sensor signal 0 ";
            LogDebug(QString("INFO: Received level sensor signal 0 "));
        }
        else if(DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            LogDebug(QString("WARNING: Current procedure has been interrupted, exit now."));
            RetValue = DCL_ERR_UNEXPECTED_BREAK;
            goto SORTIE;
        }
        else if(DCL_ERR_TIMER_TIMEOUT == retCode)
        {

            if((TimeNow > (TimeStartPressure + SUCKING_SETUP_WARNING_TIME)) && ( !WarnShowed ))
            {
                FILE_LOG_L(laDEVPROC, llWARNING) << "Warning! Do not get level sensor data in" << (SUCKING_SETUP_WARNING_TIME / 1000)<<" seconds.";
                LogDebug(QString("WARNING! Do not get level sensor data in %1 seconds").arg(SUCKING_SETUP_WARNING_TIME / 1000));
                WarnShowed = true;
                m_pDevProc->OnReportFillingTimeOut2Min();
            }
            //check pressure here
            qreal CurrentPressure = GetPressure();
            if(CurrentPressure != (UNDEFINED_4_BYTE))
            {
                PressureBuf.append(CurrentPressure);
                AllPressureBuf.append(CurrentPressure);
            }
            if(PressureBuf.length() >= SUCKING_OVERFLOW_SAMPLE_SIZE)
            {
                //with 4 wire pump
                qreal Sum = 0;
                qreal DeltaSum = 0;
                qreal lastValue = PressureBuf.at(0);
                for(qint32 i = 0; i < PressureBuf.length(); i++)
                {
                     Sum += PressureBuf.at(i);
                     DeltaSum += PressureBuf.at(i) - lastValue;
                     lastValue = PressureBuf.at(i);
                }

                if(((Sum/ PressureBuf.length()) < SUCKING_OVERFLOW_PRESSURE)&&(DeltaSum < SUCKING_OVERFLOW_4SAMPLE_DELTASUM) && NeedOverflowChecking)
                {
                    LogDebug(QString("ERROR: Overflow occured! Exit now"));
                    for(qint32 i = 0; i < PressureBuf.length(); i++)
                    {
                        LogDebug(QString("INFO: Pressur buf: %1").arg(PressureBuf.at(i)));
                    }
                    RetValue = DCL_ERR_DEV_LA_FILLING_OVERFLOW;
                    goto SORTIE;
                }
                if (((Sum/PressureBuf.length())<SUCKING_INSUFFICIENT_PRESSURE) && (DeltaSum<SUCKING_INSUFFICIENT_4SAMPLE_DELTASUM) && InsufficientCheckFlag && !TimeOutInsufficient)
                {
                    // Just mark flag here, and return it until 4 minutes timeout
                    TimeOutInsufficient = true;
                    for (qint32 i=0; i< PressureBuf.length(); i++)
                    {
                        InsufficientPressureBuf.append(PressureBuf.at(i));
                    }
                }

                PressureBuf.pop_front();
            }

            if(TimeNow > (TimeStartPressure + SUCKING_MAX_SETUP_TIME))
            {
                //Firstly, check Insufficient
                if (TimeOutInsufficient)
                {
                    LogDebug(QString("ERROR: Insufficient reagent in the station! Exit now"));
                    for (qint32 i=0; i<InsufficientPressureBuf.length(); i++)
                    {
                        LogDebug(QString("INFO: Insufficient Pressure %1 is: %2").arg(i).arg(InsufficientPressureBuf.at(i)));
                    }

                    RetValue = DCL_ERR_DEV_LA_FILLING_INSUFFICIENT;
                    goto SORTIE;
                }
                else
                {
                    for (qint32 i=0; i<AllPressureBuf.length(); i++)
                    {
                        LogDebug(QString("INFO: Pressure when 4 min error %1 is: %2").arg(i).arg(AllPressureBuf.at(i)));
                    }
                }
                FILE_LOG_L(laDEVPROC, llERROR) << "ERROR! Do not get level sensor data in" << (SUCKING_MAX_SETUP_TIME / 1000)<<" seconds, Time out! Exit!";
                LogDebug(QString("ERROR! Do not get level sensor data in %1 seconds, Timeout! Exit!").arg(SUCKING_MAX_SETUP_TIME / 1000));
                RetValue = DCL_ERR_DEV_LA_FILLING_TIMEOUT_4MIN;
                goto SORTIE;
            }
            if((TimeStopFilling!=0)&&(TimeNow >= TimeStopFilling))
            {
                FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Delay finished!";
                LogDebug(QString("INFO: Delay finished"));
                StopCompressor();
                return DCL_ERR_FCT_CALL_SUCCESS;
            }
        }
        else
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Unexpected level sensor state value: " << levelSensorState;
            LogDebug(QString("WARNING: Unexpected level sensor state value: %1").arg(levelSensorState));
        }
    }


SORTIE:
    (void)ReleasePressure();
   // (void)TurnOffFan();
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Stop command execution
 *
 *  \iparam  CmdType command type
 */
/****************************************************************************/
void CAirLiquidDevice::StopCommandExec(quint8 CmdType)
{
    emit SigStopCommandExec(CmdType);
}

/****************************************************************************/
/*!
 *  \brief   Fill the system automatically.
 *
 *  \iparam  DelayTime = Delay a small period (in Millisecodns) before turn-off
 *                       the pump when retort has been detected full.
 *  \iparam  EnableInsufficientCheck = Enable Insufficient Check
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/

ReturnCode_t CAirLiquidDevice::FillingForService(quint32 DelayTime, bool EnableInsufficientCheck)
{
    ReturnCode_t RetValue = DCL_ERR_FCT_CALL_SUCCESS;
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    QList<qreal> PressureBuf;
    int levelSensorState = 0xFF;
    bool NeedOverflowChecking = true;
    bool stop = false;
    bool InsufficientCheckFlag = EnableInsufficientCheck;
    qint64 TimeNow, TimeStartPressure, TimeStopFilling;
    TimeStopFilling = 0;
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start Sucking procedure.";
    LogDebug(QString("INFO: Start Filling procedure."));

    //release pressure
    RetValue = ReleasePressure();
    if( DCL_ERR_FCT_CALL_SUCCESS != RetValue )
    {
        goto SORTIE;
    }
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Start Sucking now.";
   // (void)TurnOnFan();
    RetValue = SetTargetPressure(AL_PUMP_MODE_PWM_NEGATIVE, m_WorkingPressureNegative);
    if(DCL_ERR_FCT_CALL_SUCCESS != RetValue)
    {
        goto SORTIE;
    }
    TimeStartPressure = QDateTime::currentMSecsSinceEpoch();
    // For Paraffin reagent, we need NOT insufficient check all the time. For Non-paraffin ones, we need
    // check insufficient before FM_TEMP_LEVEL_SENSOR_STATE_1, and NOT need this after STATE_1
    //set timeout to 2 minutes
    while(!stop)
    {
        TimeNow = QDateTime::currentMSecsSinceEpoch();
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, SUCKING_POOLING_TIME);
        if(DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_1 == retCode)
        {
            m_pDevProc->OnReportLevelSensorStatus1();
            if(TimeStopFilling == 0)
            {
                FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Hit target level. Sucking Finished.";
                LogDebug(QString("INFO: Hit target level. Sucking Finished."));
                if(DelayTime > 0)
                {
                    //NeedOverflowChecking = false;
                    //waiting for some time
                    quint32 MaxDelayTime = 70*1000;
                    if(DelayTime <= MaxDelayTime)
                    {
                        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Delay for " << DelayTime<<" milliseconds.";
                        LogDebug(QString("INFO: Delay for %1 milliseconds.").arg(DelayTime));
                        TimeStopFilling = TimeNow + DelayTime;
                    }
                    else
                    {
                        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Delay for " << MaxDelayTime<<" milliseconds.";
                        LogDebug(QString("INFO: Delay for %1 milliseconds.").arg(MaxDelayTime));
                        TimeStopFilling = TimeNow + MaxDelayTime;
                    }
                }
                else
                {
                    stop = true;
                    return DCL_ERR_FCT_CALL_SUCCESS;
                }
            }
            InsufficientCheckFlag = false;
        }
        else if(DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_0 == retCode)
        {
            FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Received level sensor signal 0 ";
            LogDebug(QString("INFO: Received level sensor signal 0 "));
        }
        else if(DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            LogDebug(QString("WARNING: Current procedure has been interrupted, exit now."));
            RetValue = DCL_ERR_UNEXPECTED_BREAK;
            goto SORTIE;
        }
        else if(DCL_ERR_TIMER_TIMEOUT == retCode)
        {
            if(TimeNow > (TimeStartPressure + 120*1000))
            {
                FILE_LOG_L(laDEVPROC, llERROR) << "ERROR! Do not get level sensor data in" << (SUCKING_MAX_SETUP_TIME / 1000)<<" seconds, Time out! Exit!";
                LogDebug(QString("ERROR! Do not get level sensor data in %1 seconds, Timeout! Exit!").arg(SUCKING_SETUP_WARNING_TIME / 1000));
                RetValue = DCL_ERR_DEV_LA_FILLING_TIMEOUT_2MIN;
                goto SORTIE;
            }
            if((TimeStopFilling!=0)&&(TimeNow >= TimeStopFilling))
            {
                FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Delay finished!";
                LogDebug(QString("INFO: Delay finished"));
                goto SORTIE;
            }
            //check pressure here
            qreal CurrentPressure = GetPressure();
            if(CurrentPressure != (UNDEFINED_4_BYTE))
            {
                PressureBuf.append(CurrentPressure);
            }
            if(PressureBuf.length() >= SUCKING_OVERFLOW_SAMPLE_SIZE)
            {
                //with 4 wire pump
                qreal Sum = 0;
                qreal DeltaSum = 0;
                qreal lastValue = PressureBuf.at(0);
                for(qint32 i = 0; i < PressureBuf.length(); i++)
                {
                     Sum += PressureBuf.at(i);
                     DeltaSum += PressureBuf.at(i) - lastValue;
                     lastValue = PressureBuf.at(i);
                }

                if(((Sum/ PressureBuf.length()) < SUCKING_OVERFLOW_PRESSURE)&&(DeltaSum < SUCKING_OVERFLOW_4SAMPLE_DELTASUM) && NeedOverflowChecking)
                {
                    LogDebug(QString("ERROR: Overflow occured! Exit now"));
                    for(qint32 i = 0; i < PressureBuf.length(); i++)
                    {
                        LogDebug(QString("INFO: Pressur buf: %1").arg(PressureBuf.at(i)));
                    }
                    RetValue = DCL_ERR_DEV_LA_FILLING_OVERFLOW;
                    goto SORTIE;
                }
                else if(((Sum/ PressureBuf.length()) < SUCKING_INSUFFICIENT_PRESSURE)&&(DeltaSum > SUCKING_INSUFFICIENT_4SAMPLE_DELTASUM)&&(InsufficientCheckFlag))
                {
                    LogDebug(QString("ERROR: Insufficient reagent in the station! Exit now"));
                    for(qint32 i = 0; i < PressureBuf.length(); i++)
                    {
                        LogDebug(QString("INFO: Insufficient Pressure %1 is: %2").arg(i).arg(PressureBuf.at(i)));
                    }
                    RetValue = DCL_ERR_DEV_LA_FILLING_INSUFFICIENT;
                    goto SORTIE;
                }
                PressureBuf.pop_front();
            }
        }
        else
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Unexpected level sensor state value: " << levelSensorState;
            LogDebug(QString("WARNING: Unexpected level sensor state value: %1").arg(levelSensorState));
        }
    }


SORTIE:
    (void)ReleasePressure();
    //(void)TurnOffFan();
    return RetValue;

}


/****************************************************************************/
/*!
 *  \brief   Build-up a lower pressure in system for Bottle-check function.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::PressureForBottoleCheck()
{
    LogDebug(QString( "INFO: Start setting up pressure for bottle check"));
    bool stop = false;
    qint64 startTime = 0;
    qreal CurrentPressure;
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    //release pressure
    retCode = ReleasePressure();
    if( DCL_ERR_FCT_CALL_SUCCESS != retCode )
    {
        goto SORTIE;
    }

    //start compressor
    retCode = SetTargetPressure(AL_PUMP_MODE_ON_OFF_POSITIVE, AL_TARGET_PRESSURE_BOTTLECHECK);
    if(DCL_ERR_FCT_CALL_SUCCESS != retCode )
    {

        goto SORTIE;
    }
    (void)IsPIDDataSteady(0,  0,  0,  0, true);

    startTime = QDateTime::currentMSecsSinceEpoch();
    while(!stop)
    {
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_PROCEDURE_PRESSURE, 200);
        if ( DCL_ERR_UNEXPECTED_BREAK == retCode)
        {
            FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Current procedure has been interrupted, exit now.";
            retCode = DCL_ERR_UNEXPECTED_BREAK;
            goto SORTIE;
        }
        CurrentPressure = GetRecentPressure();
        if (IsPIDDataSteady(10,  CurrentPressure,  \
                            1, 8, false))
        {
            LogDebug(QString("INFO: Target pressure is getting steady now."));
            retCode = DCL_ERR_FCT_CALL_SUCCESS;
            stop = true;
        }
        else
        {
            if((QDateTime::currentMSecsSinceEpoch() - startTime) > 30*1000)
            {
                LogDebug(QString("WARNING: Pressure exceed maximum setup time, exit!"));
                //stop compressor
                (void)ReleasePressure();
                retCode = DCL_ERR_DEV_LA_BOTTLECHECK_PRESSUREBUILD_FAILED;
                goto SORTIE;
            }
        }
    }
    //stop compressor
    StopCompressor();
SORTIE:
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   Judge if the control target's is stable under PID method.
 *
 *  \iparam  TargetValue = Target value to compare.
 *  \iparam  CurrentValue = Current value to compare.
 *  \iparam  Tolerance = Tolerance value used to compare.
 *  \iparam  Num = Length of the internal buffer to store the data.
 *  \iparam  Init = Whether it is to start a new comparsion.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief    Set specified pressure to Air-liquid device.
 *            This function will firstly operate the valves and then use
 *            SetPressure to operate the pump.
 *
 *  \param    flag = Pump's working mode
 *  \param    pressure = Target pressure to set
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::SetTargetPressure(quint8 flag, float pressure)
{
    if(qFuzzyIsNull(pressure)) //equal to float zero
    {
        //close valve 1
        (void)SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
        //close valve 2
        (void)SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
        return  SetPressure(flag, pressure);
    }
    else if(pressure > 0.0) //larger than float zero
    {
        //close valve 1
        (void)SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
        //open valve 2
        (void)SetValve(VALVE_2_INDEX, VALVE_STATE_OPEN);
        return  SetPressure(flag, pressure);//should be 1 or 17
    }
    else if (pressure < 0.0) //less than zero
    {
        //open valve 1
        (void)SetValve(VALVE_1_INDEX, VALVE_STATE_OPEN);
        //close valve 2
        (void)SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
        return SetPressure(flag, pressure);//should be 9 or 25
    }
    return DCL_ERR_FCT_CALL_FAILED;
}

/****************************************************************************/
/*!
 *  \brief    Callback function for timer used in Vaccum.
 */
/****************************************************************************/
void CAirLiquidDevice::VaccumTimerCB(void)
{
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_VACCUM, DCL_ERR_TIMER_TIMEOUT);
    }
}

/****************************************************************************/
/*!
 *  \brief    Callback function for timer used in Pressure.
 */
/****************************************************************************/
void CAirLiquidDevice::PressureTimerCB(void)
{
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_PRESSURE, DCL_ERR_TIMER_TIMEOUT);
    }
}

/****************************************************************************/
/*!
 *  \brief    Callback function for timer used in Draining.
 */
/****************************************************************************/
void CAirLiquidDevice::DrainingTimerCB(void)
{
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_DRAINING, DCL_ERR_TIMER_TIMEOUT);
    }
}

/****************************************************************************/
/*!
 *  \brief    Callback function for timer used in ReleasePressure.
 */
/****************************************************************************/
void CAirLiquidDevice::ReleasePressureTimerCB(void)
{
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_RELEASE_PRESSURE, DCL_ERR_TIMER_TIMEOUT);
    }
}

/****************************************************************************/
/*!
 *  \brief    Callback function for timer used in Sucking.
 */
/****************************************************************************/
void CAirLiquidDevice::SuckingTimerCB(void)
{
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, DCL_ERR_TIMER_TIMEOUT);
    }
}

/****************************************************************************/
/*!
 *  \brief   slot associated with level sensor state changing.
 *
 *  This slot is connected to the signal, ReportLevelSensorState
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam State = Current state of the level sensor
 *
 */
/****************************************************************************/
void CAirLiquidDevice::OnLevelSensorState(quint32, ReturnCode_t ReturnCode, quint8 State)
{
    LogDebug(QString( "INFO: New level sensor state: %1").arg(State));

    Q_UNUSED(ReturnCode)
    if(State == 1)
    {
        if(m_pDevProc)
        {
            m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_1);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Set temperature control's status.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  TempCtrlStatus = New pressure control status.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::SetTemperatureControlStatus(ALTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus)
{
    m_TargetTempCtrlStatus[Type] = TempCtrlStatus;
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        retCode = m_pTempCtrls[Type]->SetStatus(TempCtrlStatus);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    return  retCode;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with get fan's life-cycle data.
 *
 *  This slot is connected to the signal, ReportLifeTimeData
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam LifeTime = Life time of the fan.
 *  \iparam LifeCycles = Life cycles of the fan.
 *
 */
/****************************************************************************/
void CAirLiquidDevice::OnGetLifeTime(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint32 LifeTime, quint32 LifeCycles)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        m_DOLifeTime = LifeTime;
        m_DOLifeCycles = LifeCycles;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_GET_DO_LIFE_TIME, ReturnCode);
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
ReturnCode_t CAirLiquidDevice::SetTempCtrlON(ALTempCtrlType_t Type)
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
ReturnCode_t CAirLiquidDevice::SetTempCtrlOFF(ALTempCtrlType_t Type)
{
    return SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_OFF);
}

/****************************************************************************/
/*!
 *  \brief   Get the pressure sensor data captured in last 500 milliseconds.
 *
 *  \return  Actual pressure, UNDEFINED if failed.
 */
/****************************************************************************/
qreal CAirLiquidDevice::GetRecentPressure(void)
{
   // QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetPressureTime) <= 500) // check if 500 msec has passed since last read
    {
        if(!qFuzzyCompare(m_CurrentPressure, UNDEFINED_4_BYTE))
        {
            return (m_CurrentPressure - m_PressureDrift);
        }
        else
        {
            return UNDEFINED_4_BYTE;
        }
    }
    else
    {
        return UNDEFINED_4_BYTE;
    }
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
qreal CAirLiquidDevice::GetRecentTemperature(ALTempCtrlType_t Type, quint8 Index)
{
  //  QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue;
    if((Now - m_LastGetTempTime[Type][Index]) <= 1000) // check if 1000 msec has passed since last read
    {
        RetValue = m_CurrentTemperatures[Type][Index];
    }
    else
    {
        //LogDebug(QString("In AirLiquid device, invalid temperature. Current state is: %1").arg(m_MainState));
        RetValue = UNDEFINED_4_BYTE;
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Get recent current of sensor captured in last 500 milliseconds.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  Actual current, UNDEFINED if failed.
 */
/****************************************************************************/
quint32 CAirLiquidDevice::GetRecentCurrent(ALTempCtrlType_t Type)
{
  //  QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint32 RetValue;
    if((Now - m_LastGetTCCurrentTime[Type]) <= 1000) // check if 1000 msec has passed since last read
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
ReturnCode_t CAirLiquidDevice::StartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
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
        return DCL_ERR_DEV_TEMP_CTRL_ALREADY_ON;
    }
    if (IsTemperatureControlOff(Type))
    {
        //Set the nominal temperature
        retCode = SetTemperature(Type, NominalTemperature, SlopeTempChange);
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
ReturnCode_t CAirLiquidDevice::StartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    ReturnCode_t retCode;
    m_TargetTemperatures[Type] = NominalTemperature;
    m_TargetTempCtrlStatus[Type] = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState(Type) == TEMPCTRL_STATE_ERROR)
    {
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
#if 0
    if (IsTemperatureControlOn(Type))
    {
        if( DCL_ERR_FCT_CALL_SUCCESS != SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_OFF))
        {
            return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
        }
    }
#endif
    if( DCL_ERR_FCT_CALL_SUCCESS != SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_OFF))
    {
        return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
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
//    if ( DCL_ERR_FCT_CALL_SUCCESS != retCode)
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
TempCtrlState_t CAirLiquidDevice::GetTemperatureControlState(ALTempCtrlType_t Type)
{
    ReturnCode_t retCode = m_pTempCtrls[Type]->ReqStatus();
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
        m_CurrentTempCtrlStatus[Type] = TEMPCTRL_STATUS_UNDEF;
        return TEMPCTRL_STATE_ERROR;
    }
    retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_GET_TEMP_CTRL_STATE);
    TempCtrlState_t controlstate = TEMPCTRL_STATE_ERROR;
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        controlstate = TEMPCTRL_STATE_ERROR;
    }
    else if (IsTemperatureControlOn(Type))
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
bool CAirLiquidDevice::IsInsideRange(ALTempCtrlType_t Type, quint8 Index)
{
    if(!qFuzzyCompare(GetTemperature(Type, 0), UNDEFINED_4_BYTE))
    {
        if(!qFuzzyCompare(m_TargetTemperatures[Type],UNDEFINED_4_BYTE)
                || !qFuzzyCompare(m_CurrentTemperatures[Type][Index],UNDEFINED_4_BYTE))
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
bool CAirLiquidDevice::IsOutsideRange(ALTempCtrlType_t Type, quint8 Index)
{
    if(!qFuzzyCompare(GetTemperature(Type, 0),UNDEFINED_4_BYTE))
    {
        if(!qFuzzyCompare(m_TargetTemperatures[Type],UNDEFINED_4_BYTE)
                || !qFuzzyCompare(m_CurrentTemperatures[Type][Index],UNDEFINED_4_BYTE))
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
bool CAirLiquidDevice::IsTemperatureControlOn(ALTempCtrlType_t Type)
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
bool CAirLiquidDevice::IsTemperatureControlOff(ALTempCtrlType_t Type)
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
ReturnCode_t CAirLiquidDevice::SetTemperature(ALTempCtrlType_t Type, float NominalTemperature, quint8 SlopeTempChange)
{
    m_TargetTemperatures[Type] = NominalTemperature;
    ReturnCode_t retCode;

    DCLEventLoop* event = NULL;
    if(m_pTempCtrls[Type] != NULL)
    {
        event = m_pDevProc->CreateSyncCall(SYNC_CMD_AL_SET_TEMP);
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
    retCode =  m_pDevProc->BlockingForSyncCall(event);
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
void CAirLiquidDevice::OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature)
{
    Q_UNUSED(Temperature)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: AL Set temperature successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL set temperature failed! " << ReturnCode; //lint !e641
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_TEMP, ReturnCode);
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
qreal CAirLiquidDevice::GetTemperature(ALTempCtrlType_t Type, quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue = m_CurrentTemperatures[Type][Index];
    if((Now - m_LastGetTempTime[Type][Index]) >= CHECK_SENSOR_TIME) // check if 800 msec has passed since last read
    {
        ReturnCode_t retCode = m_pTempCtrls[Type]->ReqActTemperature(Index);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode )
        {
            RetValue = UNDEFINED_4_BYTE;
        }
        else
        {
            retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_GET_TEMP);
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
ReturnCode_t CAirLiquidDevice::GetTemperatureAsync(ALTempCtrlType_t Type, quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetTempTime[Type][Index]) >= CHECK_SENSOR_TIME) // check if 800 msec has passed since last read
    {
        m_LastGetTempTime[Type][Index] = Now;
        return m_pTempCtrls[Type]->ReqActTemperature(Index);
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Get actual current of Air-liquid's heater.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  The current of heater in mA.
 */
/****************************************************************************/
quint16 CAirLiquidDevice::GetHeaterCurrent(ALTempCtrlType_t Type)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint16 RetValue = UNDEFINED_2_BYTE;
    if(m_pTempCtrls[Type] != NULL)
    {
        if((Now - m_LastGetTCCurrentTime[Type]) >= CHECK_CURRENT_TIME) // check if 900 msec has passed since last read
        {
            ReturnCode_t retCode = m_pTempCtrls[Type]->GetHardwareStatus();
            if (DCL_ERR_FCT_CALL_SUCCESS == retCode )
            {
                retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_AL_TC_GET_HW_STATUS);
                if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
                {
                    RetValue = UNDEFINED_2_BYTE;
                }
                else
                {
                    RetValue = m_TCHardwareStatus[Type].Current;
                }
                m_LastGetTCCurrentTime[Type] = Now;
            }
        }
        else
        {

            RetValue = m_TCHardwareStatus[Type].Current;
        }
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Get actual current of Air-liquid's heater asynchronously.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::GetHeaterCurrentAsync(ALTempCtrlType_t Type)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetTCCurrentTime[Type]) >= CHECK_CURRENT_TIME && m_pTempCtrls[Type] != NULL) // check if 900 msec has passed since last read
    {
        m_LastGetTCCurrentTime[Type] = Now;
        return m_pTempCtrls[Type]->GetHardwareStatus();
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
void CAirLiquidDevice::OnGetTemp(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Index, qreal Temp)
{
    Q_UNUSED(Index)

    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: AL Get temperature successful! ";
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]][Index] = Temp;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL get temperature failed! " << ReturnCode; //lint !e641
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]][Index] = UNDEFINED_4_BYTE;
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_GET_TEMP, ReturnCode);
}

/****************************************************************************/
/*!
 *  \brief  slot for getting the hardware information
 *
 *  This slot is connected to the signal ReportHardwareStatus
 *
 *  \iparam InstanceID
 *  \iparam HeaterSwitchType
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam Sensors = Number of temperature sensors connected to the board
 *  \iparam Fans = Number of ventilation fans connected to the board
 *  \iparam Heaters = Number of heating elements connected to the board
 *  \iparam Pids = Number of PID controllers in the control loop
 *  \iparam Current = Current through the heatinf circuit in milliamperes
 */
/****************************************************************************/
void CAirLiquidDevice::OnTCGetHardwareStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Sensors, quint8 Fans,
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
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_TC_GET_HW_STATUS, ReturnCode);
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
ReturnCode_t CAirLiquidDevice::SetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Set AL temperature PID, type: " << Type; //lint !e641
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        DCLEventLoop* event = m_pDevProc->CreateSyncCall(SYNC_CMD_AL_SET_TEMP_PID);
        retCode = m_pTempCtrls[Type]->SetTemperaturePid(MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        return m_pDevProc->BlockingForSyncCall(event);
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
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL set temperature PID failed! " << ReturnCode; //lint !e641
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_TEMP_PID, ReturnCode);
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set FAN status.
 *
 *  This slot is connected to the signal, ReportRefFanState
 *
 *  \iparam  InstanceID = Instance ID.
 *  \iparam  ReturnCode = ReturnCode of function level Layer
 *  \iparam  FanStatus = Fan Status.
 *
 */
/****************************************************************************/
void CAirLiquidDevice::OnSetFanStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 FanStatus)
{
    Q_UNUSED(FanStatus)
    Q_UNUSED(InstanceID)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: AL Set Fan status successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL set fan status failed! " << ReturnCode; //lint !e641
    }
    m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_SET_FAN_STATUS, ReturnCode);
}

/****************************************************************************/
/*!
 *  \brief  Turn on the fan.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::TurnOnFan()
{
    ReturnCode_t retCode = DCL_ERR_NOT_INITIALIZED;
    if(m_pPressureCtrl)
    {
        DCLEventLoop* event = m_pDevProc->CreateSyncCall(SYNC_CMD_AL_SET_FAN_STATUS);
        retCode = m_pPressureCtrl->SetFan(1);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        retCode = m_pDevProc->BlockingForSyncCall(event);
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Turn off the fan.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::TurnOffFan()
{
    ReturnCode_t retCode = DCL_ERR_NOT_INITIALIZED;
    if(m_pPressureCtrl)
    {
        DCLEventLoop* event = m_pDevProc->CreateSyncCall(SYNC_CMD_AL_SET_FAN_STATUS);
        retCode = m_pPressureCtrl->SetFan(0);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        retCode = m_pDevProc->BlockingForSyncCall(event);
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Break all ongoing pump-related operation.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::BreakAllOperation(void)
{
    LogDebug(QString("INFO: Start break all operation procedure"));
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_PRESSURE, DCL_ERR_UNEXPECTED_BREAK);
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_DRAINING, DCL_ERR_UNEXPECTED_BREAK);
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_RELEASE_PRESSURE, DCL_ERR_UNEXPECTED_BREAK);
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR, DCL_ERR_UNEXPECTED_BREAK);
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_AL_PROCEDURE_VACCUM, DCL_ERR_UNEXPECTED_BREAK);
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief  This function would turn-off the pump, valves and fan,
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::AllStop(void)
{
    LogDebug(QString("INFO: Stop pump"));
    StopCompressor();
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Close Both Valves";
    LogDebug(QString("INFO: Device Airliquid: Close both valves"));
    if(m_pPressureCtrl)
    {
        (void)m_pPressureCtrl->SetValve(VALVE_1_INDEX, VALVE_STATE_CLOSE);
        (void)m_pPressureCtrl->SetValve(VALVE_2_INDEX, VALVE_STATE_CLOSE);
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t CAirLiquidDevice::ControlValve(quint8 ValveIndex, quint8 ValveState)
{
    LogDebug(QString("INFO:Control valve valveIndex:%1,valveState:%2").arg(ValveIndex).arg(ValveState));
    return SetValve(ValveIndex, ValveState);
}

/****************************************************************************/
/*!
 *  \brief  Set pressure drift value of the pressure control module.
 *
 *  \iparam pressureDrift = Pressure drift value
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CAirLiquidDevice::SetPressureDrift(float pressureDrift)
{
    m_PressureDrift = pressureDrift;
    return DCL_ERR_FCT_CALL_SUCCESS;
}
/****************************************************************************/
/*!
 *  \brief  Reset the life time of ActiveCarbonFilter
 */
/****************************************************************************/
void CAirLiquidDevice::ResetActiveCarbonFilterLifeTime()
{
    if (m_pPressureCtrl)
    {
        m_pPressureCtrl->SetActiveCarbonFilterLifeTime(0);
    }

}

} //namespace
