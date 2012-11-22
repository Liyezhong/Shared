/******************************************************************/
/*! \file HeatedVesselsDevice.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.09.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CHeatedVesselsDevice
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

#include "DeviceControl/Include/Devices/HeatedVesselsDevice.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor of CHeatedVesselsDevice
 *
 *  \iparam pDeviceProcessing = pointer to DeviceProcessing
 *  \iparam Type = Device type string
 */
/****************************************************************************/
CHeatedVesselsDevice::CHeatedVesselsDevice(DeviceProcessing* pDeviceProcessing, QString Type) :
    CBaseDevice(pDeviceProcessing, Type)
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;

    // normal operation tasks and states
    m_taskID       = HVESSELS_DEV_TASKID_INIT;
    m_taskState    = HVESSELS_DEV_TASK_STATE_FREE;
    m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_HVESSELS;

    for(quint8 idx = 0; idx < COUNT_HEATED_VESSELS; idx++)
    {
        m_pTempCtrl[idx] = 0;
        m_actVesselTemperature[idx] = 0;
        m_actOperatingMode[idx] = TEMPCTRL_OPMODE_UNDEF;
        m_actStatusTempCtrl[idx] = TEMPCTRL_STATUS_UNDEF;
        m_vesselState[idx] = 0;
        m_vesselErrorCode[idx] = 0;
    }
    m_reqVesselID = HVESSELS_ID_VESSEL_1;
    m_actVesselID = HVESSELS_ID_VESSEL_1;

    m_reqVesselTemperature = 0;

    m_reqOperatingMode = TEMPCTRL_OPMODE_UNDEF;
    m_reqStatusTempCtrl = TEMPCTRL_STATUS_UNDEF;

    FILE_LOG_L(laDEV, llINFO) << " create heated vessels device";
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CHeatedVesselsDevice
 *
 *
 *  \return
 *
 ****************************************************************************/
CHeatedVesselsDevice::~CHeatedVesselsDevice()
{
    m_pDevProc = 0;

    for(quint8 idx = 0; idx < COUNT_HEATED_VESSELS; idx++)
    {
        m_pTempCtrl[idx] = 0;
    }
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
void CHeatedVesselsDevice::HandleTasks()
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
            m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
            m_taskID    = HVESSELS_DEV_TASKID_FREE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CHeatedVesselsDevice:handleTasks, new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules to the pointers,
 *
 *
 *  \return  void
 *
 ****************************************************************************/
ReturnCode_t CHeatedVesselsDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::HandleInitializationState()";

    m_pTempCtrl[0] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_TempCtrlVessel1Key));
    if(m_pTempCtrl[0] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_VESSEL1);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_TEMPCTRL_VESSEL1 not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pTempCtrl[1] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_TempCtrlVessel2Key));
    if(m_pTempCtrl[1] == 0)
    {
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_VESSEL2);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_TEMPCTRL_VESSEL2 not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pTempCtrl[2] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_TempCtrlVessel3Key));
    if(m_pTempCtrl[2] == 0)
    {
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_VESSEL3);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_TEMPCTRL_VESSEL3 not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pTempCtrl[3] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_TempCtrlVessel4Key));
    if(m_pTempCtrl[3] == 0)
    {
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_VESSEL4);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_TEMPCTRL_VESSEL4 not allocated.";
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
ReturnCode_t CHeatedVesselsDevice::HandleConfigurationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    for(int idx = 0; idx < COUNT_HEATED_VESSELS; idx++)
    {
        if (!connect(m_pTempCtrl[idx], SIGNAL(ReportError(quint32, quint16 , quint16 , quint16, QDateTime)),
                     this, SLOT(TempCtrlError(quint32, quint16 , quint16 , quint16, QDateTime))))
        {
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_VESSEL1);
            FILE_LOG_L(laDEV, llERROR) << "   Connect temp. ctrl." << (int) idx << "  signal 'ReportError' failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
        if (!connect(m_pTempCtrl[idx], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                     this, SLOT(TempCtrlActTemperature(quint32, ReturnCode_t, quint8, qreal))))
        {
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_VESSEL1);
            FILE_LOG_L(laDEV, llERROR) << "   Connect temp. ctrl." << (int) idx << "  signal 'ReportActTemperature' failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }

        if (!connect(m_pTempCtrl[idx], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                     this, SLOT(TempCtrlActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
        {
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_VESSEL1);
            FILE_LOG_L(laDEV, llERROR) << "   Connect temp. ctrl." << (int) idx << "  signal 'TempCtrlActStatus' failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }

        if (!connect(m_pTempCtrl[idx], SIGNAL(ReportActOperatingMode(quint32, ReturnCode_t, TempCtrlOperatingMode_t)),
                     this, SLOT(TempCtrlActOperatingMode(quint32, ReturnCode_t, TempCtrlOperatingMode_t))))
        {
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_VESSEL1);
            FILE_LOG_L(laDEV, llERROR) << "   Connect temp. ctrl." << (int) idx << "  signal 'ReportActOperatingMode' failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }

    return RetVal;

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
void CHeatedVesselsDevice::HandleIdleState()
{
    if(m_taskID == HVESSELS_DEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_taskID == HVESSELS_DEV_TASKID_STATUS_SET)
    {
        HandleSetStatusTempCtrlTask();
    }
    else if(m_taskID == HVESSELS_DEV_TASKID_OPMODE_SET)
    {
        HandleSetOperatingModeTempCtrlTask();
    }
    else if(m_taskID == HVESSELS_DEV_TASKID_ACTOPMODE_REQ)
    {
        HandleReqActOperatingModeTempCtrlTask();
    }
    else if(m_taskID == HVESSELS_DEV_TASKID_TEMP_SET)
    {
        HandleSetTemperatureTask();
    }
    else if(m_taskID == HVESSELS_DEV_TASKID_ACTTEMP_REQ)
    {
        HandleRequestActTemperatureTask();        
    }
    else if(m_taskID == HVESSELS_DEV_TASKID_ACTSTATUS_REQ)
    {
        HandleRequestActTempCtrlTask();
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_taskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INVALID_STATE, (quint16) m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
    }}

/****************************************************************************/
/*!
 *  \brief    Handle the task to set the status of temperature control
 *
 *   The function is called from HandleIdleState() if m_taskID == HVESSELS_DEV_TASKID_STATUS_SET.
 *   The three task states of the reference run task are handled here:
 *
 *     HVESSELS_DEV_TASK_STATE_REQ :
 *        Forward the status request to the temperature control object
 *
 *     HVESSELS_DEV_TASK_STATE_REQ_SEND:
 *        Check timeout, wait until the request was confirmed by receiving
 *        the signal
 *
 *     HVESSELS_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class
 *
 *     All other states, which are unexpected, will result in a HVESSELS_DEV_TASK_STATE_ERROR
 *     state to forward the error
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CHeatedVesselsDevice::HandleSetStatusTempCtrlTask()
{
    if((m_pDevProc == 0) || (m_pTempCtrl[0] == 0) || (m_pTempCtrl[1] == 0) || (m_pTempCtrl[2] == 0) || (m_pTempCtrl[3] == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ)
    {
        ReturnCode_t retval;
        quint8 vesselIdx = 0;
        bool errorMarker = false;

        if(m_reqVesselID == HVESSELS_ID_VESSEL_ALL)
        {
            for(vesselIdx = 0; vesselIdx < COUNT_HEATED_VESSELS; vesselIdx++)
            {
                retval = m_pTempCtrl[vesselIdx]->SetStatus(m_reqStatusTempCtrl);
                if(retval != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while SetStatus at vessel " << vesselIdx << " : " << (int) retval;
                    errorMarker = true;
                }
            }
            // Return the status of the first vessel
            vesselIdx = 0;
        }
        else
        {
            vesselIdx = GetVesselIdxFromID(m_reqVesselID);
            retval = m_pTempCtrl[vesselIdx]->SetStatus(m_reqStatusTempCtrl);
        }

        if((retval == DCL_ERR_FCT_CALL_SUCCESS) && (errorMarker == true))
        {
            //m_pIHeatedVesselsDev->RouteActStatus(m_instanceID, m_lastErrorHdlInfo, m_actStatusTempCtrl[vesselIdx]);
            ResetTask();
        }
        else
        {
            m_lastErrorHdlInfo = retval;
            m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, EVENT_DCL_HVESSELS_DEV_SET_STATUS_FCT_CALL_FAILED, (quint16) m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while SetStatus: " << (int) retval;
        }
    }
    else if(m_taskState == HVESSELS_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error at task HVESSELS_DEV_TASKID_STATUS_SET ";

        //m_pIHeatedVesselsDev->RouteActStatus(m_instanceID, m_lastErrorHdlInfo, TEMPCTRL_STATUS_UNDEF);

        ResetTask();

        // its a heated vessels error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INVALID_STATE, (quint16)m_taskState);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to set the operating mode of one or all assigned
 *            temperature control classes
 *
 *   The function is called from HandleIdleState() if m_taskID == HVESSELS_DEV_TASKID_OPMODE_SET.
 *   The three task states of the reference run task are handled here:
 *
 *     HVESSELS_DEV_TASK_STATE_REQ :
 *        Forward the status request to the temperature control object
 *
 *     HVESSELS_DEV_TASK_STATE_REQ_SEND:
 *        Check timeout, wait until the request was confirmed by receiving
 *        the signal
 *
 *     HVESSELS_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class
 *
 *     All other states, which are unexpected, will result in a HVESSELS_DEV_TASK_STATE_ERROR
 *     state to forward the error
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CHeatedVesselsDevice::HandleSetOperatingModeTempCtrlTask()
{
    if((m_pDevProc == 0) || (m_pTempCtrl[0] == 0) || (m_pTempCtrl[1] == 0) || (m_pTempCtrl[2] == 0) || (m_pTempCtrl[3] == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ)
    {
        ReturnCode_t retval;
        quint8 vesselIdx = 0;

        if(m_reqVesselID == HVESSELS_ID_VESSEL_ALL)
        {
            for(vesselIdx = 0; vesselIdx < COUNT_HEATED_VESSELS; vesselIdx++)
            {
                retval = m_pTempCtrl[vesselIdx]->SetOperatingMode(m_reqOperatingMode);
                if(retval != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while SetOperatingMode at vessel " << vesselIdx << " : " << (int) retval;
                    break;
                }
            }
            // Return the status of the first vessel
            vesselIdx = 0;
        }
        else
        {
            vesselIdx = GetVesselIdxFromID(m_reqVesselID);
            retval = m_pTempCtrl[vesselIdx]->SetOperatingMode(m_reqOperatingMode);
        }

        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            //m_pIHeatedVesselsDev->RouteActOperatingMode(m_instanceID, m_lastErrorHdlInfo, m_actOperatingMode[vesselIdx]);
            ResetTask();
        }
        else
        {
            m_lastErrorHdlInfo = retval;
            m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, EVENT_DCL_HVESSELS_DEV_SET_OPMODE_FCT_CALL_FAILED, (quint16) m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while SetOperatingMode: " << (int) retval;
        }
    }
    else if(m_taskState == HVESSELS_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error HVESSELS_DEV_TASKID_OPMODE_SET";

        //m_pIHeatedVesselsDev->RouteActOperatingMode(m_instanceID, m_lastErrorHdlInfo, TEMPCTRL_OPMODE_UNDEF);

        ResetTask();

        // its a heated vessels error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INVALID_STATE, (quint16)m_taskState);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to set the target temperature of one or all assigned
 *            temperature control classes
 *
 *   The function is called from HandleIdleState() if m_taskID == HVESSELS_DEV_TASKID_TEMP_SET.
 *   The three task states of the reference run task are handled here:
 *
 *     HVESSELS_DEV_TASK_STATE_REQ :
 *        Forward the target temperature to the temperature control object(s)
 *
 *     HVESSELS_DEV_TASK_STATE_REQ_SEND:
 *        Check timeout, wait until the request was confirmed by receiving
 *        the signal
 *
 *     HVESSELS_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class
 *
 *     All other states, which are unexpected, will result in a HVESSELS_DEV_TASK_STATE_ERROR
 *     state to forward the error
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CHeatedVesselsDevice::HandleSetTemperatureTask()
{
    if((m_pDevProc == 0) || (m_pTempCtrl[0] == 0) || (m_pTempCtrl[1] == 0) || (m_pTempCtrl[2] == 0) || (m_pTempCtrl[3] == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ)
    {
        ReturnCode_t retval;
        quint8 vesselIdx = 0;

        if(m_reqVesselID == HVESSELS_ID_VESSEL_ALL)
        {
            for(vesselIdx = 0; vesselIdx < COUNT_HEATED_VESSELS; vesselIdx++)
            {
                FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice: SetTemperature " << m_reqVesselTemperature;
                retval = m_pTempCtrl[vesselIdx]->SetTemperature(m_reqVesselTemperature);
                if(retval != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    FILE_LOG_L(laDEV, llERROR) << "  CHeatedVesselsDevice: error while SetTemperature at vessel " << vesselIdx << " : " << (int) retval;
                    break;
                }
            }
            // Return the status of the first vessel
            vesselIdx = 0;
        }
        else
        {
            vesselIdx = GetVesselIdxFromID(m_reqVesselID);
            retval = m_pTempCtrl[vesselIdx]->SetTemperature(m_reqVesselTemperature);
        }

        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            //m_pIHeatedVesselsDev->RouteActTemperature(m_instanceID, m_lastErrorHdlInfo, m_reqVesselID, m_actVesselTemperature[vesselIdx]);
            ResetTask();
        }
        else
        {
            m_lastErrorHdlInfo = retval;
            m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, EVENT_DCL_HVESSELS_DEV_SET_TEMP_FCT_CALL_FAILED, (quint16) m_taskID);
            FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while SetTemperature: " << (int) retval;
        }
    }
    else if(m_taskState == HVESSELS_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error HVESSELS_DEV_TASKID_TEMP_SET ";
        //m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        //m_pIHeatedVesselsDev->RouteError(m_instanceID, m_lastErrorHdlInfo);
        //m_pIHeatedVesselsDev->RouteActTemperature(m_instanceID, m_lastErrorHdlInfo, m_reqVesselID, NULL);
        ResetTask();
        // its a heated vessels error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INVALID_STATE, (quint16)m_taskState);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to request the actual temperature of one or all assigned
 *            temperature control classes
 *
 *   The function is called from HandleIdleState() if m_taskID == HVESSELS_DEV_TASKID_TEMP_REQ.
 *   The three task states of the reference run task are handled here:
 *
 *     HVESSELS_DEV_TASK_STATE_REQ :
 *        Forward the temperature request to the temperature control object(s)
 *
 *     HVESSELS_DEV_TASK_STATE_REQ_SEND:
 *        Check timeout, wait until the request was confirmed by receiving
 *        the signal
 *
 *     HVESSELS_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class
 *
 *     All other states, which are unexpected, will result in a HVESSELS_DEV_TASK_STATE_ERROR
 *     state to forward the error
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CHeatedVesselsDevice::HandleRequestActTemperatureTask()
{
    if((m_pDevProc == 0) || (m_pTempCtrl[0] == 0) || (m_pTempCtrl[1] == 0) || (m_pTempCtrl[2] == 0) || (m_pTempCtrl[3] == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ)
    {
        ReturnCode_t retval;
        quint8 vesselIdx = 0;

        if(m_reqVesselID == HVESSELS_ID_VESSEL_ALL)
        {
            for(vesselIdx = 0; vesselIdx < COUNT_HEATED_VESSELS; vesselIdx++)
            {
                retval = m_pTempCtrl[vesselIdx]->ReqActTemperature(0);
                m_vesselState[vesselIdx] = 1;      // 0 - free, 1 - read req. send, 2 - data received, 3 - error
                m_vesselErrorCode[vesselIdx] = 0;  // clear
                if(retval != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_vesselErrorCode[vesselIdx] = 3;
                    FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while ReqTemperature at vessel " << vesselIdx << " : " << (int) retval;
                    break;
                }
            }
        }
        else
        {
            vesselIdx = GetVesselIdxFromID(m_reqVesselID);
            retval = m_pTempCtrl[vesselIdx]->ReqActTemperature(0);
        }

        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_stateTimespan = 5000; /// \todo
            m_stateTimer.Trigger();

            m_taskState = HVESSELS_DEV_TASK_STATE_REQ_SEND;
        }
        else
        {
            m_lastErrorHdlInfo = retval;
            m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, EVENT_DCL_HVESSELS_DEV_REQ_ACTTEMP_FCT_CALL_FAILED, (quint16) m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while SetOperatingMode: " << (int) retval;
        }
    }
    else if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ_SEND)
    {
        /// \todo Timer, muss nicht jedesmal gecheckt werden.
        bool bVesselsResultsComplete = true;
        for(int idx=0; idx < COUNT_HEATED_VESSELS; idx++)
        {
            /*if(m_pTempCtrl[idx]->GetActionState() == CANTemperatureControl::FM_TEMP_SUB_STATE_ACTION_ACTTEMP_REQ_ERROR)
            {
                m_vesselErrorCode[idx] = m_pTempCtrl[idx]->GetLastErrorCode();
                m_vesselState[idx] = 3;
                /// \todo festlegen
                // damit waere der ganze Heizkram hin! sonst koennte man mit fuenf Kuevetten weiterarbeiten.
                m_subStateAction = HVESSELS_DEV_ACTION_STATE_ACTTEMP_REQ_ERROR;
            }*/

            if(m_vesselState[idx] < 2)
            {
                bVesselsResultsComplete = false;
            }
        }
        if(bVesselsResultsComplete == true)
        {
            m_taskState = HVESSELS_DEV_TASK_STATE_REQ_ACKN;
        }

        if(m_stateTimer.Elapsed() > m_stateTimespan)
        {
            m_lastErrorHdlInfo = DCL_ERR_TIMEOUT;
            m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, EVENT_DCL_HVESSELS_DEV_REQ_ACTTEMP_TIMEOUT, (quint16)m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: while ReqTemperature.. timeout";
        }
    }
    else if(m_taskState == HVESSELS_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error HVESSELS_DEV_TASKID_ACTTEMP_REQ ";

        //m_pIHeatedVesselsDev->RouteActTemperature(m_instanceID, m_lastErrorHdlInfo, m_reqVesselID, 0);

        ResetTask();
        // its a heated vessels error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INVALID_STATE, (quint16)m_taskState);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to request the actual operationmode of one or all assigned
 *            temperature control classes
 *
 *   The function is called from HandleIdleState() if m_taskID == HVESSELS_DEV_TASKID_ACTOPMODE_REQ.
 *   The three task states of the reference run task are handled here:
 *
 *     HVESSELS_DEV_TASK_STATE_REQ :
 *        Forward the operating request to the temperature control object(s)
 *
 *     HVESSELS_DEV_TASK_STATE_REQ_SEND:
 *        Check timeout, wait until the request was confirmed by receiving
 *        the signal
 *
 *     HVESSELS_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class
 *
 *     All other states, which are unexpected, will result in a HVESSELS_DEV_TASK_STATE_ERROR
 *     state to forward the error
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CHeatedVesselsDevice::HandleReqActOperatingModeTempCtrlTask()
{
    if((m_pDevProc == 0) || (m_pTempCtrl[0] == 0) || (m_pTempCtrl[1] == 0) || (m_pTempCtrl[2] == 0) || (m_pTempCtrl[3] == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ)
    {
        ReturnCode_t retval;
        quint8 vesselIdx = 0;

        if(m_reqVesselID == HVESSELS_ID_VESSEL_ALL)
        {
            for(vesselIdx = 0; vesselIdx < COUNT_HEATED_VESSELS; vesselIdx++)
            {
                retval = m_pTempCtrl[vesselIdx]->ReqOperatingMode();
                m_vesselState[vesselIdx] = 1;      // 0 - free, 1 - read req. send, 2 - data received, 3 - error
                m_vesselErrorCode[vesselIdx] = 0;  // clear
                if(retval != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_vesselErrorCode[vesselIdx] = 3;
                    FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while ReqOperationMode at vessel " << vesselIdx << " : " << (int) retval;
                    break;
                }
            }
        }
        else
        {
            vesselIdx = GetVesselIdxFromID(m_reqVesselID);
            retval = m_pTempCtrl[vesselIdx]->ReqOperatingMode();
        }

        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_stateTimespan = 5000; /// \todo
            m_stateTimer.Trigger();

            m_taskState = HVESSELS_DEV_TASK_STATE_REQ_SEND;
        }
        else
        {
            m_lastErrorHdlInfo = retval;
            m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, EVENT_DCL_HVESSELS_DEV_REQ_OPMODE_FCT_CALL_FAILED, (quint16) m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while ReqOperatingMode: " << (int) retval;
        }
    }
    else if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ_SEND)
    {
        /// \todo Timer, muss nicht jedesmal gecheckt werden.
        bool bVesselsResultsComplete = true;
        for(int idx=0; idx < COUNT_HEATED_VESSELS; idx++)
        {
            /*if(m_pTempCtrl[idx]->GetActionState() == CANTemperatureControl::FM_TEMP_SUB_STATE_ACTION_ACTOPMODE_REQ_ERROR)
            {
                m_vesselErrorCode[idx] = m_pTempCtrl[idx]->GetLastErrorCode();
                m_vesselState[idx] = 3;
                /// \todo festlegen
                // damit waere der ganze Heizkram hin! sonst koennte man mit fuenf Kuevetten weiterarbeiten.
                m_subStateAction = HVESSELS_DEV_ACTION_STATE_ACTOPMODE_REQ_ERROR;
            }*/

            if(m_vesselState[idx] < 2)
            {
                bVesselsResultsComplete = false;
            }
        }
        if(bVesselsResultsComplete == true)
        {
            m_taskState = HVESSELS_DEV_TASK_STATE_REQ_ACKN;
        }

        if(m_stateTimer.Elapsed() > m_stateTimespan)
        {
            m_lastErrorHdlInfo = DCL_ERR_TIMEOUT;
            m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, EVENT_DCL_HVESSELS_DEV_REQ_OPMODE_TIMEOUT, (quint16)m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: while ReqOperationMode.. timeout";
        }
    }
    else if(m_taskState == HVESSELS_DEV_TASK_STATE_ERROR)
    {
        TempCtrlOperatingMode_t operatingMode = TEMPCTRL_OPMODE_UNDEF;
        FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error HVESSELS_DEV_TASKID_ACTOPMODE_REQ ";

        //m_pIHeatedVesselsDev->RouteActOperatingMode(m_instanceID, m_lastErrorHdlInfo, operatingMode);

        ResetTask();
        // its a heated vessels error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INVALID_STATE, (quint16)m_taskState);
    }
}

void CHeatedVesselsDevice::HandleRequestActTempCtrlTask()
{
    if((m_pDevProc == 0) || (m_pTempCtrl[0] == 0) || (m_pTempCtrl[1] == 0) || (m_pTempCtrl[2] == 0) || (m_pTempCtrl[3] == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }
    if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ)
    {
        ReturnCode_t retval;
        quint8 vesselIdx = 0;

        if(m_reqVesselID == HVESSELS_ID_VESSEL_ALL)
        {
            for(vesselIdx = 0; vesselIdx < COUNT_HEATED_VESSELS; vesselIdx++)
            {
                retval = m_pTempCtrl[vesselIdx]->ReqStatus();
                m_vesselState[vesselIdx] = 1;      // 0 - free, 1 - read req. send, 2 - data received, 3 - error
                m_vesselErrorCode[vesselIdx] = 0;  // clear
                if(retval != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_vesselErrorCode[vesselIdx] = 3;
                    FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while ReqTemperature at vessel " << vesselIdx << " : " << (int) retval;
                    break;
                }
            }
        }
        else
        {
            vesselIdx = GetVesselIdxFromID(m_reqVesselID);
            retval = m_pTempCtrl[vesselIdx]->ReqStatus();
        }

        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_stateTimespan = 5000; /// \todo
            m_stateTimer.Trigger();

            m_taskState = HVESSELS_DEV_TASK_STATE_REQ_SEND;
        }
        else
        {
            m_lastErrorHdlInfo = retval;
            m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, EVENT_DCL_HVESSELS_DEV_REQ_STATUS_FCT_CALL_FAILED, (quint16) m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error while GetTempCtrl: " << (int) retval;
        }
    }
    else if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ_SEND)
    {
        /// \todo Timer, muss nicht jedesmal gecheckt werden.
        bool bVesselsResultsComplete = true;
        for(int idx=0; idx < COUNT_HEATED_VESSELS; idx++)
        {
            /*if(m_pTempCtrl[idx]->GetActionState() == CANTemperatureControl::FM_TEMP_SUB_STATE_ACTION_ACTTEMP_REQ_ERROR)
            {
                m_vesselErrorCode[idx] = m_pTempCtrl[idx]->GetLastErrorCode();
                m_vesselState[idx] = 3;
                /// \todo festlegen
                // damit waere der ganze Heizkram hin! sonst koennte man mit fuenf Kuevetten weiterarbeiten.
                m_subStateAction = HVESSELS_DEV_ACTION_STATE_ACTTEMP_REQ_ERROR;
            }*/

            if(m_vesselState[idx] < 2)
            {
                bVesselsResultsComplete = false;
            }
        }
        if(bVesselsResultsComplete == true)
        {
            m_taskState = HVESSELS_DEV_TASK_STATE_REQ_ACKN;
        }

        if(m_stateTimer.Elapsed() > m_stateTimespan)
        {
            m_lastErrorHdlInfo = DCL_ERR_TIMEOUT;
            m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, EVENT_DCL_HVESSELS_DEV_REQ_STATUS_TIMEOUT, (quint16)m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: while ReqTemperature.. timeout";
        }
    }
    else if(m_taskState == HVESSELS_DEV_TASK_STATE_ERROR)
    {
        TempCtrlStatus_t tempctrlstatus = TEMPCTRL_STATUS_UNDEF;
        FILE_LOG_L(laDEV, llERROR) << " CHeatedVesselsDevice: error HVESSELS_DEV_TASKID_ACTOPMODE_REQ ";

        //m_pIHeatedVesselsDev->RouteActStatus(m_instanceID, m_lastErrorHdlInfo, tempctrlstatus);

        ResetTask();
        // its a heated vessels error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = HVESSELS_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_HVESSELS_DEV, ERROR_DCL_HVESSELS_DEV_INVALID_STATE, (quint16)m_taskState);
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
 *     HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     HVESSELS_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state
 *
 *     HVESSELS_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CHeatedVesselsDevice::HandleErrorState()
{
    if(m_errTaskState == HVESSELS_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_errTaskState == HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIHeatedVesselsDev != 0)
        {
            m_pIHeatedVesselsDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }*/
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_errTaskState == HVESSELS_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_errTaskState == HVESSELS_DEV_ERRTASK_STATE_RESET)
    {
        ResetTask();
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        for(quint8 idx = 0; idx < COUNT_HEATED_VESSELS; idx++)
        {
            m_pTempCtrl[idx] = 0;
            m_actVesselTemperature[idx] = 0;
            m_actOperatingMode[idx] = TEMPCTRL_OPMODE_UNDEF;
            m_vesselState[idx] = 0;
            m_vesselErrorCode[idx] = 0;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot receives the stepper motor error signal
 *
 *  \iparam   instanceID = instance identifier of the device
 *  \iparam   errorGroup = EVENT_GRP_DCL_FCTMOD_TEMP_SLV
 *  \iparam   errorCode  = from CAN-message
 *  \iparam   errorData  = from CAN-message
 *  \iparam   errorTime  = error message reception time on master side
 *
 ****************************************************************************/
void CHeatedVesselsDevice::TempCtrlError(quint32 instanceID, quint16 errorGroup, quint16 errorCode, quint16 errorData, QDateTime errorTime)
{
    Q_UNUSED(instanceID);

    SetErrorParameter(errorGroup, errorCode, errorData);
    m_lastErrorTime = errorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_errTaskState = HVESSELS_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief  Receives the actual temperature of a cuvette
 *
 *      This slot will be called if one of the assigned temperature controls
 *      emits its 'ReportActTemperature' signal.
 *
 *  \iparam instanceID = instance identifier of the device
 *  \iparam hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temp. control op. mode  was received,
 *                    otherwise the return value of the error detecting function
 *  \iparam Temperature = temperature in °C
 */
/****************************************************************************/
void CHeatedVesselsDevice::TempCtrlActTemperature(quint32 instanceID, ReturnCode_t hdlInfo, quint8 Index, qreal Temperature)
{
    FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::TempCtrlActTemperature received";
    for(int idx = 0; idx < COUNT_HEATED_VESSELS; idx++)
    {
        FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::TempCtrlActTemperature, check :0x" << m_pTempCtrl[idx]->GetModuleHandle() << " = " << instanceID;
        if((m_pTempCtrl[idx] != 0) && (m_pTempCtrl[idx]->GetModuleHandle() == instanceID))
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_actVesselTemperature[idx] = Temperature;
                m_vesselState[idx] = 2;
                FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::TempCtrlActTemperature ok, idx: " << idx;
            }
            else
            {
                m_lastErrorHdlInfo = hdlInfo;
                //SetTaskStateToError();
                m_lastErrorGroup = EVENT_GRP_DCL_FCTMOD_TEMP_DCL;

                /// \todo m_vesselErrorCode[idx] = m_pTempCtrl[idx]->GetLastErrorCode();
                m_actVesselTemperature[idx] = 0;  /// \todo maybe its not a good idea
                m_vesselState[idx] = 3;
            }
            break;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the temperature control operating mode information signal
 *
 *      The internal state machine will be reset, and the temperature
 *      control's operating mode information is forwarded to the
 *      IHeatedVesselsDevice class.
 *
 *  \iparam instanceID = instance identifier of the function module
 *  \iparam hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temp. control op. mode  was received,
 *                    otherwise the return value of the error detecting function
 *  \iparam operatingMode = the actual operating mode of then heated cuvettes's temp. control
 */
/****************************************************************************/
void CHeatedVesselsDevice::TempCtrlActOperatingMode(quint32 instanceID, ReturnCode_t hdlInfo, TempCtrlOperatingMode_t operatingMode)
{
    FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::TempCtrlOperatingMode received";
    for(int idx = 0; idx < COUNT_HEATED_VESSELS; idx++)
    {
        FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::TempCtrlOperatingMode, check :0x" << m_pTempCtrl[idx]->GetModuleHandle() << " = " << instanceID;
        if((m_pTempCtrl[idx] != 0) && (m_pTempCtrl[idx]->GetModuleHandle() == instanceID))
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_actOperatingMode[idx] = operatingMode;
                m_vesselState[idx] = 2;
                FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::TempCtrlOperatingMode ok, idx: " << idx;
            }
            else
            {
                m_lastErrorHdlInfo = hdlInfo;
                //SetTaskStateToError();
                m_lastErrorGroup = EVENT_GRP_DCL_FCTMOD_TEMP_DCL;

                /// \todo m_vesselErrorCode[idx] = m_pTempCtrl[idx]->GetLastErrorCode();
                m_vesselState[idx] = 0;  /// \todo set it unklnown
                m_vesselState[idx] = 3;
            }
            break;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the temperature control actual status information signal
 *
 *      The internal state machine will be reset, and the temperature
 *      control's actual status information is forwarded to the
 *      IHeatedVesselsDevice class.
 *
 *  \iparam instanceID = instance identifier of the function module
 *  \iparam hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temp. control op. mode  was received,
 *                    otherwise the return value of the error detecting function
 *  \iparam status = the actual status of the heated cuvettes's temp. control
 */
/****************************************************************************/
void CHeatedVesselsDevice::TempCtrlActStatus(quint32 instanceID, ReturnCode_t hdlInfo, TempCtrlStatus_t status, TempCtrlMainsVoltage_t)
{
    FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::TempCtrlActStatus received";
    for(int idx = 0; idx < COUNT_HEATED_VESSELS; idx++)
    {
        FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::TempCtrlActStatus, check :0x" << m_pTempCtrl[idx]->GetModuleHandle() << " = " << instanceID;
        if((m_pTempCtrl[idx] != 0) && (m_pTempCtrl[idx]->GetModuleHandle() == instanceID))
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_actStatusTempCtrl[idx] = status;
                m_vesselState[idx] = 2;
                FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::TempCtrlActStatus ok, idx: " << idx;
            }
            else
            {
                m_lastErrorHdlInfo = hdlInfo;
                //SetTaskStateToError();
                m_lastErrorGroup = EVENT_GRP_DCL_FCTMOD_TEMP_DCL;

                /// \todo m_vesselErrorCode[idx] = m_pTempCtrl[idx]->GetLastErrorCode();
                m_vesselState[idx] = 0;  /// \todo set it unklnown
                m_vesselState[idx] = 3;
            }
            break;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Returns the temperature of a cuvette
 *
 *  \iparam vesselID = instance identifier of the function module
 *
 *  \return Temperature of a cuvette
 */
/****************************************************************************/
qint16 CHeatedVesselsDevice::GetActTemperature(HeatedVesselID_t vesselID)
{
    quint8 idx;

    idx = GetVesselIdxFromID(vesselID);

    if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ_ACKN)
    {
        ResetTask();
    }

    return m_actVesselTemperature[idx];
}

/****************************************************************************/
/*!
 *  \brief  Returns the operating mode of the heated cuvettes
 *
 *  \return Operating mode
 */
/****************************************************************************/
TempCtrlOperatingMode_t CHeatedVesselsDevice::GetOperatingMode()
{
    quint8 idx;

    idx = GetVesselIdxFromID(HVESSELS_ID_VESSEL_1);

    if(m_taskState == HVESSELS_DEV_TASK_STATE_REQ_ACKN)
    {
        ResetTask();
    }

    return m_actOperatingMode[idx];
}

/****************************************************************************/
/*!
 *  \brief   Set the status of the temperature control
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActStatus() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam  statusTempCtrl = status of all vessels temperature control
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CHeatedVesselsDevice::SetStatusTempCtrl(TempCtrlStatus_t statusTempCtrl)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    // the action can be started exclusively from those states
    if(m_taskID == HVESSELS_DEV_TASKID_FREE)
    {
        m_reqStatusTempCtrl = statusTempCtrl;
        //m_reqVesselID = HVESSELS_ID_VESSEL_1;
        m_reqVesselID = HVESSELS_ID_VESSEL_ALL;
        m_taskID    = HVESSELS_DEV_TASKID_STATUS_SET;
        m_taskState = HVESSELS_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::SetStatusTempCtrl ";
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "  CHeatedVesselsDevice::SetStatusTempCtrl, invalid task state " << (int) m_taskID;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief   Set the operting mode of the temperature control
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActOperatingMode() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam  operatingMode = operating mode of all vessels temperature control
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CHeatedVesselsDevice::SetOperatingMode(TempCtrlOperatingMode_t operatingMode)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    // the action can be started exclusively from those states
    if(m_taskID == HVESSELS_DEV_TASKID_FREE)
    {
        m_reqOperatingMode = operatingMode;
        m_reqVesselID = HVESSELS_ID_VESSEL_1;
        m_taskID    = HVESSELS_DEV_TASKID_OPMODE_SET;
        m_taskState = HVESSELS_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::SetOperatingMode";
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "  CHeatedVesselsDevice::SetOperatingMode, invalid task state " << (int) m_taskID;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief   Request the actual temperature control status
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActStatus() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CHeatedVesselsDevice::ReqActStatusTempCtrl()
{
    //ReturnCode_t retval = DCL_ERR_FCT_NOT_IMPLEMENTED;
    //FILE_LOG_L(laDEV, llERROR) << "  CHeatedVesselsDevice::ReqActStatusTempCtrl is not implemented so far...";

    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    // the action can be started exclusively from those states
    if(m_taskID == HVESSELS_DEV_TASKID_FREE)
    {
        m_reqVesselID = HVESSELS_ID_VESSEL_1;
        m_taskID    = HVESSELS_DEV_TASKID_ACTSTATUS_REQ;
        m_taskState = HVESSELS_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "CHeatedVesselsDevice::ReqActStatusTempCtrl ";
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "  CHeatedVesselsDevice::ReqActStatusTempCtrl, invalid task state " << (int) m_taskID;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief   Request the actual temperature control operating mode
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActOperatingMode() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CHeatedVesselsDevice::ReqOperatingMode()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    // the action can be started exclusively from those states
    if(m_taskID == HVESSELS_DEV_TASKID_FREE)
    {
        m_reqVesselID = HVESSELS_ID_VESSEL_1;
        m_taskID    = HVESSELS_DEV_TASKID_ACTOPMODE_REQ;
        m_taskState = HVESSELS_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "CHeatedVesselsDevice::ReqOperatingMode ";
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "  CHeatedVesselsDevice::ReqOperatingMode, invalid task state " << (int) m_taskID;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief   Set the vessel temperature
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActTemperature() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam  vesselTemperatur = vessel target temperature
 *  \iparam  vesselID = vessel indentifier type
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CHeatedVesselsDevice::SetTemperature(qint16 vesselTemperatur, HeatedVesselID_t vesselID)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    // the action can be started exclusively from those states
    if(m_taskID == HVESSELS_DEV_TASKID_FREE)
    {
        m_reqVesselTemperature = vesselTemperatur;
        m_reqVesselID = vesselID;
        m_taskID    = HVESSELS_DEV_TASKID_TEMP_SET;
        m_taskState = HVESSELS_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::SetTemperature " << m_reqVesselTemperature << " vesselID: " << (int) m_reqVesselID;
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "  CHeatedVesselsDevice::SetTemperature, invalid task state " << (int) m_taskID;
        retval = DCL_ERR_INVALID_STATE;
    }
    return retval;
}

/****************************************************************************/
/*!
 *  \brief   Request the actual vessel temperature
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActTemperature() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam   vesselID = vessel indentifier type
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CHeatedVesselsDevice::ReqActTemperature(HeatedVesselID_t vesselID)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    // the action can be started exclusively from those states
    if(m_taskID == HVESSELS_DEV_TASKID_FREE)
    {
        m_reqVesselID = vesselID;
        m_taskID    = HVESSELS_DEV_TASKID_ACTTEMP_REQ;
        m_taskState = HVESSELS_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "  CHeatedVesselsDevice::ReqActTemperature ";
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "  CHeatedVesselsDevice::ReqActTemperature, invalid task id " << (int) m_taskID;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief Converts a heated cuvette ID into a number
 *
 *  \iparam vesselID = vessel indentifier type
 *
 *  \return Number of the heated cuvette
 */
/****************************************************************************/
quint8 CHeatedVesselsDevice::GetVesselIdxFromID(HeatedVesselID_t vesselID)
{
    quint8 vesselIdx = 0;

    switch (vesselID)
    {
        case HVESSELS_ID_VESSEL_1:
            vesselIdx = 0;
            break;
        case HVESSELS_ID_VESSEL_2:
            vesselIdx = 1;
            break;
        case HVESSELS_ID_VESSEL_3:
            vesselIdx = 2;
            break;
        case HVESSELS_ID_VESSEL_4:
            vesselIdx = 3;
            break;
        default:
            vesselIdx = 0;
            break;
    }

    return vesselIdx;
}

/****************************************************************************/
/*!
 *  \brief   Free the taskID and the task state
 *
 *
 *  \return
 *
 ****************************************************************************/
void CHeatedVesselsDevice::ResetTask()
{
    m_taskID = HVESSELS_DEV_TASKID_FREE;
    m_taskState = HVESSELS_DEV_TASK_STATE_FREE;
}

} //namespace
