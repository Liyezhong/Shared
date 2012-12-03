/******************************************************************/
/*! \file WaterDevice.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CWaterDevice
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

#include "DeviceControl/Include/Devices/WaterDevice.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

#define WATER_DEV_TIMEOUT_VALVE_STATUS_REQ 2000   //!< timeout for output value request
#define WATER_DEV_TIMEOUT_ACT_LIQUID_REQ   2000   //!< timeout for input value request

/****************************************************************************/
/*!
 *  \brief    Constructor of CWaterDevice
 *
 *
 *  \param    pDeviceProcessing = Pointer to DeviceProcessing
 *  \param    Type = Device type string
 *
 *  \return
 *
 ***************************************************************************/
CWaterDevice::CWaterDevice(DeviceProcessing* pDeviceProcessing, QString Type)
        : CBaseDevice(pDeviceProcessing, Type)
    //m_reqValveID(WATER_VALVE_ID_UNDEF),
    //m_reqValveStatus(0), m_valveID(WATER_VALVE_ID_UNDEF), m_actValvesStatusMask(0), m_liquidLevel(0)
{

    m_MainState    = DEVICE_MAIN_STATE_START;
    m_MainStateOld = m_MainState;
    m_TaskID       = WATERDEV_TASKID_INIT;
    //m_taskState    = WATER_DEV_TASK_STATE_FREE;
    m_ErrorTaskState = WATER_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_WATER;

    //function module pointer initialisation
    for(int idx = 0; idx < MAX_WATER_VALVE_COUNT; idx++)
    {
        m_pWaterValve[idx] = 0;
    }
    m_pLiquidLevel = 0;

    //module command array initialisation
    for(quint8 idx = 0; idx < WD_MAX_MODULE_CMD_IDX; idx++)
    {
        m_ModuleCommand[idx].State = WATERDEV_MODULE_CMD_STATE_FREE;
    }

    FILE_LOG_L(laDEV, llDEBUG3) << " water device created";
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CWaterDevice
 *
 *
 *  \return
 *
 ****************************************************************************/
CWaterDevice::~CWaterDevice()
{
    for(int idx = 0; idx < MAX_WATER_VALVE_COUNT; idx++)
    {
        m_pWaterValve[idx] = 0;
    }
    m_pLiquidLevel = 0;
    m_pDevProc = 0;
}

/****************************************************************************/
/*!
 *  \brief    clean up the error state
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if no error appears
 *
 ****************************************************************************/
ReturnCode_t CWaterDevice::ClearErrorState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    for(int idx = 0; idx < MAX_WATER_VALVE_COUNT; idx++)
    {
        //RetVal = m_pWaterValve[idx]->ClearErrorTask();
        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
            break;
    }

    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        //RetVal = m_pLiquidLevel->ClearErrorTask();
    }

    return RetVal;
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
void CWaterDevice::HandleTasks()
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
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = WATER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
            m_TaskID = WATERDEV_TASKID_FREE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = WATER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llDEBUG3) << "   CWaterDevice:handleTasks, new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules to the pointers,
 *           m_pWaterValve - Water valve of vessel 1 - 6
 *           m_pLiquidLevel - Digital liquid sensor
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfully executed, otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
ReturnCode_t CWaterDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    FILE_LOG_L(laDEV, llINFO) << "  CWaterDevice::HandleInitializationState()";

    m_pWaterValve[0] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_WaterValve1Key));
    if(m_pWaterValve[0] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DOUT_WATER_1);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_DOUT_WATER_1 not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pWaterValve[1] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_WaterValve2Key));
    if(m_pWaterValve[1] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DOUT_WATER_2);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_DOUT_WATER_2 not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pWaterValve[2] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_WaterValve3Key));
    if(m_pWaterValve[2] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DOUT_WATER_3);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_DOUT_WATER_3 not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pWaterValve[3] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_WaterValve4Key));
    if(m_pWaterValve[3] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DOUT_WATER_4);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_DOUT_WATER_4 not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pWaterValve[4] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_WaterValve5Key));
    if(m_pWaterValve[4] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DOUT_WATER_5);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_DOUT_WATER_5 not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pWaterValve[5] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_WaterValve6Key));
    if(m_pWaterValve[5] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DOUT_WATER_6);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_DOUT_WATER_6 not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pLiquidLevel = (CDigitalInput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_WaterLiquidLevelKey));
    if(m_pLiquidLevel == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DIN_COLL_TRAY_LIMIT);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_DIN_COLL_TRAY_LIMIT not allocated.";
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
ReturnCode_t CWaterDevice::HandleConfigurationState()
{
    for(int idx=0; idx < MAX_WATER_VALVE_COUNT; idx++)
    {
        if(!connect(m_pWaterValve[idx], SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(WaterValveError(quint32, quint16, quint16, quint16, QDateTime))))
        {
            SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DOUT_WATER_1);
            FILE_LOG_L(laDEV, llERROR) << "   Connect water valve " << (int) idx << " signal 'ReportError'failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
        if(!connect(m_pWaterValve[idx], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)), this, SLOT(WaterValveOutputValueAckn(quint32, ReturnCode_t, quint16))))
        {
            SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DOUT_WATER_1);
            FILE_LOG_L(laDEV, llERROR) << "   Connect water valve " << (int) idx << " signal 'ReportOutputValueAckn'failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
        if(!connect(m_pWaterValve[idx], SIGNAL(ReportActOutputValue(quint32, ReturnCode_t, quint16)), this, SLOT(WaterValveActOutputValue(quint32, ReturnCode_t, quint16))))
        {
            SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_DOUT_WATER_1);
            FILE_LOG_L(laDEV, llERROR) << "   Connect water valve " << (int) idx << " signal 'ReportActOutputValue'failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }

    if(!connect(m_pLiquidLevel, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(LiquidLevelSensorError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_OVEN_COVER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect dig. input signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pLiquidLevel, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(LiquidLevelSensorStatus(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_OVEN_COVER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect dig. input signal 'ReportActInputValue'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief    Internal function for idle state processing
 *
 *   The function handles the idle state, which is active if the class is 'ready for tasks'
 *   Depending on the pending task, which is stored in m_taskID, the task specific handling
 *   function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CWaterDevice::HandleIdleState()
{

    if(m_TaskID == WATERDEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_TaskID == WATERDEV_TASKID_COMMAND_HDL)
    {
        HandleCommandRequestTask();
    }
    else if(m_TaskID == WATERDEV_TASKID_ERROR)
    {
        //Just wait for 'ExitErrormode'
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_TaskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_INVALID_STATE, (quint16) m_TaskID);
        //ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = WATER_DEV_ERRTASK_STATE_REPORT_IFACE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to execute the module commands
 *
 *   The function is called from HandleTasks() if m_TaskID == WATERDEV_TASKID_COMMAND_HDL.
 *   The function loops thru the m_ModuleCommand array and prodesses the commands pending for execution.
 *
 *   If a module command state is *DEVICE*_MODULE_CMD_STATE_REQ, the command request will be
 *   forwarded by calling the function module's function. Timeout supervision will be started.
 *   if the state is *DEVICE*_MODULE_CMD_STATE_REQ_SEND, the timeout will be checked, and,
 *   if elapsed, an error will be thrown. In this case the command's response function
 *   will be called with the error information.
 *
 *   Usually, a module command will be confirmed and closed if the notification slot is called
 *   from function module (e.g. LiquidLevelSensorStatus(..))
 *
 *  \return   void
 */
/****************************************************************************/
void CWaterDevice::HandleCommandRequestTask()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_NOT_FOUND;
    quint16 ErrorCode = 0;
    bool ActiveCommandFound = false;

    if((m_pDevProc == 0) || (m_pLiquidLevel == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ERROR_DCL_WATER_DEV_NULL_PTR_ACCESS, (quint16) m_TaskID);

        //forward the error information. The error relevant datas should already have been set.
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = WATER_DEV_ERRTASK_STATE_REPORT_IFACE;

        return;
    }

    for(quint8 idx = 0; idx < WD_MAX_MODULE_CMD_IDX; idx++)
    {
        if(m_ModuleCommand[idx].State == WATERDEV_MODULE_CMD_STATE_REQ)
        {
            //General:
            // Forward the module command request to the slave side by sending
            // the corresponding CAN-message
            ActiveCommandFound = true;

            FILE_LOG_L(laFCT, llDEBUG) << " CWaterDevice: forward request of command type " << (int) m_ModuleCommand[idx].Type;
            if(m_ModuleCommand[idx].Type == WATERDEV_CMD_REQ_LIQUID_LEVEL)
            {
                RetVal = m_pLiquidLevel->ReqActInputValue();
                m_ModuleCommand[idx].Timeout = WATER_DEV_TIMEOUT_ACT_LIQUID_REQ;
                ErrorCode = EVENT_DCL_WATER_DEV_REQ_LIQUID_LEVEL_FCT_CALL_FAILED;
            }
            else if(m_ModuleCommand[idx].Type == WATERDEV_CMD_SET_VALVE_STATE)
            {
                /*if(m_ModuleCommand[idx].m_ValveID == WATER_VALVE_ID_ALL)
                {
                    FILE_LOG_L(laDEV, llINFO) << "    set all the valves...";
                    for(int idx = 0; idx < MAX_WATER_VALVE_COUNT; idx++)
                    {
                        RetVal = m_pWaterValve[idx]->SetOutputValue(m_ModuleCommand[idx].m_ValveStatus);
                        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                        {
                            ErrorCode = EVENT_DCL_WATER_DEV_SET_VALVE_FCT_CALL_FAILED;
                            /// \todo what shall I do if any of the valves is not accessibly?
                            // I think we need a safety mechanism, because:
                            // if the valves are on, and CAN-Communication breaks down, the people can swim thru their office
                            //break;
                        }
                    }
                }
                else*/
                {
                    quint8 FctModIdx;

                    FctModIdx = GetValveIndexFromType(m_ModuleCommand[idx].m_ValveID);
                    RetVal = m_pWaterValve[FctModIdx]->SetOutputValue(m_ModuleCommand[idx].m_ValveStatus);

                }
                m_ModuleCommand[idx].Timeout = WATER_DEV_TIMEOUT_ACT_LIQUID_REQ;
                m_ModuleCommand[idx].m_AcknMask = 0;
                ErrorCode = EVENT_DCL_WATER_DEV_SET_VALVE_FCT_CALL_FAILED;
            }
            else if(m_ModuleCommand[idx].Type == WATERDEV_CMD_REQ_ACTVALVE_STATE)
            {
                quint8 FctModIdx;

                FctModIdx = GetValveIndexFromType(m_ModuleCommand[idx].m_ValveID);
                RetVal = m_pWaterValve[FctModIdx]->ReqOutputValue();
                m_ModuleCommand[idx].Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                ErrorCode = EVENT_DCL_WATER_DEV_REQ_VALVE_STATE_FCT_CALL_FAILED;
            }

            //check for success
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                //trigger timeout supervision
                m_ModuleCommand[idx].ReqSendTime.Trigger();
                m_ModuleCommand[idx].State = WATERDEV_MODULE_CMD_STATE_REQ_SEND;
            }
            else
            {
                //set error information and error state
                m_lastErrorHdlInfo = RetVal;                ///< Return value of the failed function call
                SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, ErrorCode, (quint16) m_ModuleCommand[idx].Type);
                m_ModuleCommand[idx].State = WATERDEV_MODULE_CMD_STATE_ERR;
            }
        }
        else if(m_ModuleCommand[idx].State == WATERDEV_MODULE_CMD_STATE_REQ_SEND)
        {
            // check avtive device commands for timeout
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].ReqSendTime.Elapsed() > m_ModuleCommand[idx].Timeout)
            {
                //timeout detected, forward the error information to logging and emit the command signal with error information.
                m_lastErrorHdlInfo = DCL_ERR_TIMEOUT;
                m_ModuleCommand[idx].State = WATERDEV_MODULE_CMD_STATE_ERR;
                if(m_ModuleCommand[idx].Type == WATERDEV_CMD_REQ_LIQUID_LEVEL)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  WaterDevice : ReqLiquidLevel timeout error.";
                    SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, EVENT_DCL_WATER_DEV_REQ_LIQUID_LEVEL_TIMEOUT, (quint16) m_TaskID);
                }
                else if(m_ModuleCommand[idx].Type == WATERDEV_CMD_SET_VALVE_STATE)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  WaterDevice: SetValveState timeout error.";
                    SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, EVENT_DCL_WATER_DEV_SET_VALVE_STATE_TIMEOUT, (quint16) m_TaskID);
                }
                else if(m_ModuleCommand[idx].Type == WATERDEV_CMD_REQ_ACTVALVE_STATE)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  WaterDevice: ReqActValveState timeout error.";
                    SetErrorParameter(EVENT_GRP_DCL_WATER_DEV, EVENT_DCL_WATER_DEV_REQ_VALVE_STATE_TIMEOUT, (quint16) m_TaskID);
                }
            }
        }
        else if(m_ModuleCommand[idx].State == WATERDEV_MODULE_CMD_STATE_ERR)
        {
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].Type == WATERDEV_CMD_REQ_LIQUID_LEVEL)
            {
                //m_pIWaterDev->RouteLiquidLevel(m_instanceID, m_lastErrorHdlInfo, 0);
            }
            else if(m_ModuleCommand[idx].Type == WATERDEV_CMD_SET_VALVE_STATE)
            {
                //m_pIWaterDev->RouteValveStatus(m_instanceID, m_lastErrorHdlInfo, m_ModuleCommand[idx].m_ValveID, (quint8) 0);
            }
            else if(m_ModuleCommand[idx].Type == WATERDEV_CMD_REQ_ACTVALVE_STATE)
            {
                //m_pIWaterDev->RouteValveStatus(m_instanceID, m_lastErrorHdlInfo, m_ModuleCommand[idx].m_ValveID, (quint8) 0);
            }
            //forward the error information. The error relevant datas should already have been set.
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

            //Free the mocule command
            m_ModuleCommand[idx].State = WATERDEV_MODULE_CMD_STATE_FREE;
            //set state machine to error mode
            m_TaskID = WATERDEV_TASKID_ERROR;
        }
    }

    if(ActiveCommandFound == false)
    {
        m_TaskID = WATERDEV_TASKID_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the error state of the class.
 *
 *   This task controls the error handling of the class.
 *   The function is called from HandleStates() if m_MainState == DEVICE_MAIN_STATE_ERROR.
 *   This task consists of the following states:
 *
 *     WATER_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     WATER_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     WATER_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state
 *
 *     OVEN_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CWaterDevice::HandleErrorState()
{
    if(m_ErrorTaskState == WATER_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == WATER_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIWaterDev != 0)
        {
            m_pIWaterDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }*/
        m_ErrorTaskState = WATER_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == WATER_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = WATER_DEV_ERRTASK_STATE_IDLE;
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot receives the error signal of the liquid sensor
 *
 *  \iparam   instanceID = instance identifier of the device
 *  \iparam   errorGroup = EVENT_GRP_DCL_FCTMOD_DIG_INP_SLV
 *  \iparam   errorCode  = from CAN-message
 *  \iparam   errorData  = from CAN-message
 *  \iparam   errorTime  = error message reception time on master side
 *
 ****************************************************************************/
void CWaterDevice::LiquidLevelSensorError(quint32 /*instanceID*/, quint16 errorGroup, quint16 errorCode, quint16 errorData, QDateTime errorTime)
{
    SetErrorParameter(errorGroup, errorCode, errorData);
    m_lastErrorTime = errorTime;
    m_TaskID = WATERDEV_TASKID_ERROR;
    m_ErrorTaskState = WATER_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Handles the new data signal of the digital input function modul
 *
 *
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if valid data
 *                      otherwise the return value of the error detecting function
 *  \iparam   InputValue = input value of the dig. channel
 *
 *  \return  void
 *
 ****************************************************************************/
void CWaterDevice::LiquidLevelSensorStatus(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue)
{
    Q_UNUSED(InstanceID);
    FILE_LOG_L(laDEV, llINFO) << "   CWaterDevice::LiquidLevelSensorStatus received:" << InputValue;

    if(m_TaskID == WATERDEV_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(WATERDEV_CMD_REQ_LIQUID_LEVEL);
    }

    /*if(m_pIWaterDev != 0)
    {
        m_pIWaterDev->RouteLiquidLevel(m_instanceID, HdlInfo, InputValue);
    }*/
}

/****************************************************************************/
/*!
 *  \brief    This slot receives the error signal of the digital output modules
 *
 *  \iparam   instanceID = instance identifier of the device
 *  \iparam   errorGroup = EVENT_GRP_DCL_FCTMOD_DIG_OUTP_SLV
 *  \iparam   errorCode  = from CAN-message
 *  \iparam   errorData  = from CAN-message
 *  \iparam   errorTime  = error message reception time on master side
 *
 ****************************************************************************/
void CWaterDevice::WaterValveError(quint32 /*instanceID*/, quint16 errorGroup, quint16 errorCode, quint16 errorData, QDateTime errorTime)
{
    SetErrorParameter(errorGroup, errorCode, errorData);
    m_lastErrorTime = errorTime;
    m_TaskID = WATERDEV_TASKID_ERROR;
    m_ErrorTaskState = WATER_DEV_ERRTASK_STATE_REPORT_IFACE;
}


/****************************************************************************/
/*!
 *  \brief    This slot handles the digital output value acknowledge
 *
 *
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if valid data
 *                      otherwise the return value of the error detecting function
 *  \iparam   OutputValue = actual output value of the dig. channel
 *
 *  \return  void
 *
 ****************************************************************************/
void CWaterDevice:: WaterValveOutputValueAckn(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
{
    FILE_LOG_L(laDEV, llINFO) << "   CWaterDevice received, " << (int) InstanceID;

    quint8 idx2, idx1;

    WaterValveID_t ValveID;
    quint8 FctModuleIndex = 0;

    for(idx2 = 0; idx2 < MAX_WATER_VALVE_COUNT; idx2++)
    {
        if((m_pWaterValve[idx2] != 0) && (m_pWaterValve[idx2]->GetModuleHandle() == InstanceID))
        {
            FctModuleIndex = idx2;
            break;
        }
    }

    ValveID = GetValveIDFromIndex(FctModuleIndex);

    if(m_TaskID == WATERDEV_TASKID_COMMAND_HDL)
    {
        for(idx1 = 0; idx1 < WD_MAX_MODULE_CMD_IDX; idx1++)
        {
            if((m_ModuleCommand[idx1].Type == WATERDEV_CMD_SET_VALVE_STATE) &&
               (m_ModuleCommand[idx1].State == WATERDEV_MODULE_CMD_STATE_REQ_SEND))
            {
                if(m_ModuleCommand[idx1].m_ValveID == ValveID)
                {
                     m_ModuleCommand[idx1].State = WATERDEV_MODULE_CMD_STATE_FREE;
                    break;
                }
            }
        }
    }

    /*if(m_pIWaterDev != 0)
    {
        m_pIWaterDev->RouteValveStatus(m_instanceID, HdlInfo, ValveID, (quint8) OutputValue);
    }*/
}

/****************************************************************************/
/*!
 *  \brief    This slot handles the answer of an actual output value request of the digital output function modul
 *
 *
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if valid data
 *                      otherwise the return value of the error detecting function
 *  \iparam   OutputValue = actual output value of the dig. channel
 *
 *  \return  void
 *
 ****************************************************************************/
void CWaterDevice::WaterValveActOutputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
{
    FILE_LOG_L(laDEV, llINFO) << "   CWaterDevice::WaterValveActOutputValue received:" << OutputValue;

    if(m_TaskID == WATERDEV_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(WATERDEV_CMD_REQ_ACTVALVE_STATE);
    }

    quint8 FctModIdx = 0;
    WaterValveID_t ValveID;

    // Get the internal index of the function module and convert it to the WaterValveID_t type
    for(int idx = 0; idx < MAX_WATER_VALVE_COUNT; idx++)
    {
        if((m_pWaterValve[idx] != 0) && (m_pWaterValve[idx]->GetModuleHandle() == InstanceID))
        {
            FctModIdx = idx;
            break;
        }
    }

    ValveID = GetValveIDFromIndex(FctModIdx);
    /*if(m_pIWaterDev != 0)
    {
        m_pIWaterDev->RouteValveStatus(m_instanceID, HdlInfo, ValveID, (quint8) OutputValue);
    }*/
}

/****************************************************************************/
/*!
 *  \brief    Request the valve state of one of the water valves
 *
 *            The function starts an asynchronous request to forward the digital output value to the valve.
 *            The request is handled by the task's handle function, it will be forwarded there
 *            to the CANDigitalOutput class.
 *            When finished, the slot WaterValveOutputValue() will be called by
 *            CANDigitalOutput, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam   ValveID = valve identification
 *  \iparam   ValveStatus = valve status
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CWaterDevice::SetValveStatus(WaterValveID_t ValveID, quint8 ValveStatus)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8  CmdIndex;

    if(ValveID != WATER_VALVE_ID_UNDEF)
    {
        if(SetModuleTask(WATERDEV_CMD_SET_VALVE_STATE, &CmdIndex))
        {
            m_ModuleCommand[CmdIndex].m_ValveID = ValveID;
            m_ModuleCommand[CmdIndex].m_ValveStatus = ValveStatus;
            FILE_LOG_L(laDEV, llINFO) << " CWaterDevice, valve: " << (int) ValveID;
        }
        else
        {
            RetVal = DCL_ERR_INVALID_STATE;
            FILE_LOG_L(laFCT, llERROR) << " CWaterDevice invalid state: " << (int) m_TaskID;
        }
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Request the actual valve state of one of the water valves
 *
 *           This fuction is called from IWaterDevice to request the valve state state.
 *           By calling this function an asynchronous request will be started, the request closes by
 *           calling the RouteValveStatus(..) function which is implemented by the IWaterDevice class.
 *
 *           The request will be forwarded to the CANDigitalOutput class which is connected to the output
 *           function module on slave side. By calling the CANDigitalOutput::ReqOutputValue function
 *           this class determines the value and, when finished, sending its 'ReportActOutputValue' signal
 *           for acknowledge.
 *           This signal is connected to the WaterValveActOutputValue slot of this class. The command execution waits
 *           until the acknowledge was received by this slot.
 *           By calling this slot the acknowledge is forwarded to the interface class and this command will be freed.
 *
 *           A timeout supervision is started when the request is forwarded to ReqOutputValue(), it observes
 *           the acknowledge signal from CANDigitalOutput.
 *
 *           The request will be accepted if at least one free module command structure can be found.
 *
 *  \param   ValveID = Vavle to set
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is no free module command, or the error state is active, DCL_ERR_INVALID_STATE is returned.
 *
 */
/****************************************************************************/
ReturnCode_t CWaterDevice::ReqValveStatus(WaterValveID_t ValveID)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8  CmdIndex;

    if(ValveID != WATER_VALVE_ID_UNDEF)
    {
        if(SetModuleTask(WATERDEV_CMD_SET_VALVE_STATE, &CmdIndex))
        {
            m_ModuleCommand[CmdIndex].m_ValveID = ValveID;
            FILE_LOG_L(laDEV, llINFO) << " CWaterDevice, valve: " << (int) ValveID;
        }
        else
        {
            RetVal = DCL_ERR_INVALID_STATE;
            FILE_LOG_L(laFCT, llERROR) << " CWaterDevice invalid state: " << (int) m_TaskID;
        }
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Request the liquid sensor state
 *
 *           This fuction is called from IWaterDevice to request the liquid sensor state.
 *           By calling this function an asynchronous request will be started, the request closes by
 *           calling the RouteLiquidLevel(..) function which is implemented by the IWaterDevice class.
 *
 *           The request will be forwarded to the CANDigitalInput class which is connected to the input
 *           function module on slave side. By calling the CANDigitalInput::ReqActInputValue function
 *           this class determines the value and, when finished, sending its 'ReportActInputValue' signal
 *           for acknowledge.
 *           This signal is connected to the LiquidLevelSensorStatus slot of this class.
 *           The command execution waits until the acknowledge was received by the slot 'LiquidLevelSensorStatus'.
 *           By calling this slot the acknowledge is forwarded to the interface class and this command will be freed.
 *
 *           A timeout supervision is started when the request is forwarded to ReqActInputValue(), it observes
 *           the input's acknowledge signal.
 *
 *           The request will be accepted if at least one free module command structure can be found.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is no free module command, or the error state is active, DCL_ERR_INVALID_STATE is returned.
 *
 */
/****************************************************************************/
ReturnCode_t CWaterDevice::ReqLiquidLevel()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(WATERDEV_CMD_REQ_LIQUID_LEVEL))
    {
        FILE_LOG_L(laDEV, llINFO) << "    CWaterDevice::ReqLiquidLevel, valve";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CWaterDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Convert the valve type to function module array index
 *
  *  \iparam   valveID = valvue identification
 *
 *  \return  requested array index
 *
 ****************************************************************************/
quint8 CWaterDevice::GetValveIndexFromType(WaterValveID_t valveID)
{
    quint8 FctModIdx;

    switch(valveID)
    {
        case (WATER_VALVE_ID_1):
            FctModIdx = 0;
            break;
        case (WATER_VALVE_ID_2):
            FctModIdx = 1;
            break;
        case (WATER_VALVE_ID_3):
            FctModIdx = 2;
            break;
        case (WATER_VALVE_ID_4):
            FctModIdx = 3;
            break;
        case (WATER_VALVE_ID_5):
            FctModIdx = 4;
            break;
        case (WATER_VALVE_ID_6):
            FctModIdx = 5;
            break;
        default:
            FctModIdx = 0;
            break;
    }

    return FctModIdx;

}

/****************************************************************************/
/*!
 *  \brief   Convert the function module array index to type
 *
 *  \iparam   valveID = valvue index within the array
 *
 *  \return  WaterValveID_t with the requested array index
 *
 ****************************************************************************/
WaterValveID_t CWaterDevice::GetValveIDFromIndex(quint8 valveID)
{
    WaterValveID_t ValveID;

    switch(valveID)
    {
        case (0):
            ValveID = WATER_VALVE_ID_1;
            break;
        case (1):
            ValveID = WATER_VALVE_ID_2;
            break;
        case (2):
            ValveID = WATER_VALVE_ID_3;
            break;
        case (3):
            ValveID = WATER_VALVE_ID_4;
            break;
        case (4):
            ValveID = WATER_VALVE_ID_5;
            break;
        case (5):
            ValveID = WATER_VALVE_ID_6;
            break;
        default:
            ValveID = WATER_VALVE_ID_1;
            break;
    }

    return ValveID;

}

/****************************************************************************/
/*!
 *  \brief   Helper function, sets a free module command to the given command type
 *
 *  \iparam   CommandType = command type to set
 *  \iparam   pCmdIndex = pointer to index within the command array the command is set to (optional parameter, default 0)
 *
 *  \return  true, if the command type can be placed, otherwise false
 *
 *
 ****************************************************************************/
bool CWaterDevice::SetModuleTask(WaterDevCmdType_t CommandType, quint8* pCmdIndex)
{
    bool CommandAdded = false;

    if((m_TaskID == WATERDEV_TASKID_FREE) || (m_TaskID == WATERDEV_TASKID_COMMAND_HDL))
    {
        for(quint8 idx = 0; idx < WD_MAX_MODULE_CMD_IDX; idx++)
        {
            if(m_ModuleCommand[idx].State == WATERDEV_MODULE_CMD_STATE_FREE)
            {
                m_ModuleCommand[idx].State = WATERDEV_MODULE_CMD_STATE_REQ;
                m_ModuleCommand[idx].Type = CommandType;

                m_TaskID = WATERDEV_TASKID_COMMAND_HDL;
                CommandAdded  = true;
                if(pCmdIndex)
                {
                    *pCmdIndex = idx;
                }

                FILE_LOG_L(laFCT, llINFO) << " CWaterDevice:  task " << (int) idx << " request.";
                break;
            }
        }
    }

    return CommandAdded;
}

/****************************************************************************/
/**
 *  \brief      set the ModuleCommands with the specified command type to 'FREE'
 *
 *  \param      ModuleCommandType = ModuleCommands having this command type will be set to free

 *  \return     void
 */
/****************************************************************************/
void CWaterDevice::ResetModuleCommand(WaterDevCmdType_t ModuleCommandType)
{
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < WD_MAX_MODULE_CMD_IDX; idx++)
    {
        if((m_ModuleCommand[idx].Type == ModuleCommandType) &&
           (m_ModuleCommand[idx].State == WATERDEV_MODULE_CMD_STATE_REQ_SEND))
        {
            m_ModuleCommand[idx].State = WATERDEV_MODULE_CMD_STATE_FREE;
        }

        if(m_ModuleCommand[idx].State != WATERDEV_MODULE_CMD_STATE_FREE)
        {
            ActiveCommandFound = true;
        }
    }

    if(ActiveCommandFound == false)
    {
        m_TaskID = WATERDEV_TASKID_FREE;
    }
}

} //namespace
