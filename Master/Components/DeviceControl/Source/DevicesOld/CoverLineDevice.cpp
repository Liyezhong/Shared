/******************************************************************/
/*! \file CoverLineDevice.cpp
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 14.01.2011
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains hte implemtation of the class CCoverLineDevice
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

// -esym(534, QMap*::insert)
// -esym(534, QMap::insert)

#include "DeviceControl/Include/Devices/CoverLineDevice.h"
#include "DeviceControl/Include/Devices/CoverLineNavigator.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief    Constructor for the CCoverLineDevice
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *  \param    InstanceID = unique Device instacne id
 *
 *  \return
 *
 ****************************************************************************/
 CCoverLineDevice::CCoverLineDevice(DeviceProcessing* pDeviceProcessing, QString Type, DevInstanceID_t InstanceID)
     : CBaseDevice(pDeviceProcessing, Type),
    m_pCoverLineNavigator(0), m_pDigInpSlideDetection(0), m_pDigInpEDMLevel(0),
    m_pDigOutpVacuumPump(0), m_pDigOutpVacuumValve1(0), m_pDigOutpVacuumValve2(0),
    m_pAnaInpPressureSensor(0), m_pAnaInpHallSensor1(0), m_RackType(RACK_TYPE_UNDEF)
{
    m_MainState    = DEVICE_MAIN_STATE_START;
    m_MainStateOld = m_MainState;
    m_taskID       = COVERLINE_DEV_TASKID_INIT;
    m_taskState    = COVERLINE_DEV_TASK_STATE_FREE;
    m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_FREE;

    m_instanceID = InstanceID;

    for(quint8 idx = 0; idx < COVERLINE_MOTOR_MAX; idx++)
    {
        m_pMotorCoverLine[idx] = 0;
    }

    m_pCoverLineNavigator = new CCoverLineNavigator();

    FILE_LOG_L(laDEV, llINFO) << " create CCoverLineDevice device";
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CCoverLineDevice
 *
 *
 *  \return
 *
 ****************************************************************************/
CCoverLineDevice::~CCoverLineDevice()
{
    try
    {
        m_pDevProc = 0;

        for(quint8 idx = 0; idx < COVERLINE_MOTOR_MAX; idx++)
        {
            m_pMotorCoverLine[idx] = 0;
        }

        if(m_pCoverLineNavigator)
        {
            delete m_pCoverLineNavigator;
            m_pCoverLineNavigator = 0;
        }

        m_pDigInpSlideDetection = 0;
        m_pDigInpEDMLevel = 0;
        m_pDigOutpVacuumPump = 0;
        m_pDigOutpVacuumValve1 = 0;
        m_pDigOutpVacuumValve2 = 0;
        m_pAnaInpPressureSensor = 0;
        m_pAnaInpHallSensor1 = 0;
    }
    catch (...)
    {
        // and exit
        return;
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
void CCoverLineDevice::HandleTasks()
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
            FILE_LOG_L(laDEV, llERROR) << "   CCoverLineDevice:HandleInitializationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_DEV_TASK_CFG;
            m_taskID = COVERLINE_DEV_TASKID_FREE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   CCoverLineDevice:HandleConfigurationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_DEV_TASK_CFG)
    {
        RetVal = HandleDeviceTasksConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
            m_taskID = COVERLINE_DEV_TASKID_FREE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   HandleDeviceTasksConfigurationState:HandleConfigurationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE;  /// \todo define not correct
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_FCT_MOD_CFG)
    {
        HandleMotorConfigurationState();
        m_MainState = DEVICE_MAIN_STATE_IDLE;
        m_taskID    = COVERLINE_DEV_TASKID_FREE;
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CCoverLineDevice:handleTasks, new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *           m_pMotorAgitation - Motor to move the agitation
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CCoverLineDevice::HandleInitializationState()
{
    quint32 InstanceID;

    FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice::HandleInitializationState()";

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorCoverLineZAxisKey);
    m_pMotorCoverLine[CL_MOTOR_ELEVATOR] = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorCoverLine[CL_MOTOR_ELEVATOR] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorCoverLineSlideKey);
    m_pMotorCoverLine[CL_MOTOR_SLIDE] = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorCoverLine[CL_MOTOR_SLIDE] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorCoverLineNeedleKey);
    m_pMotorCoverLine[CL_MOTOR_NEEDLE] = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorCoverLine[CL_MOTOR_NEEDLE] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorCoverLineClampKey);
    m_pMotorCoverLine[CL_MOTOR_CLAMP] = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorCoverLine[CL_MOTOR_CLAMP] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorCoverSlipYKey);
    m_pMotorCoverLine[CL_MOTOR_CS_Y] = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorCoverLine[CL_MOTOR_CS_Y] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorCoverSlipZKey);
    m_pMotorCoverLine[CL_MOTOR_CS_Z] = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorCoverLine[CL_MOTOR_CS_Z] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorEDMPump);
    m_pMotorCoverLine[CL_MOTOR_EDM_PUMP] = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorCoverLine[CL_MOTOR_EDM_PUMP] == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, m_MotorEDMPump not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }


    //slide detection
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_DigInpCoverLineSlideDetect);
    m_pDigInpSlideDetection = (CDigitalInput*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pDigInpSlideDetection == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //EDM level
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_DigInpEDMLevel);
    m_pDigInpEDMLevel = (CDigitalInput*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pDigInpEDMLevel == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //Vacuum pump
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_DigOutpVacuumPump);
    m_pDigOutpVacuumPump = (CDigitalOutput*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pDigOutpVacuumPump == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, m_DigOutpVacuumPump not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }


    //Vacuum valve 1
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_DigOutpVacuumValve1);
    m_pDigOutpVacuumValve1 = (CDigitalOutput*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pDigOutpVacuumValve1 == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //Vacuum valve 2
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_DigOutpVacuumValve2);
    m_pDigOutpVacuumValve2 = (CDigitalOutput*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pDigOutpVacuumValve2 == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_COVERLINE not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //pressure sensor
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_AnaInpPressureSensor);
    m_pAnaInpPressureSensor = (CAnalogInput*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pAnaInpPressureSensor == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, 'pressure sensor' not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //pressure sensor
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_AnaInpHallSensor1);
    m_pAnaInpHallSensor1 = (CAnalogInput*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pAnaInpHallSensor1 == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, 'pressure sensor' not allocated.";
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
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CCoverLineDevice::HandleConfigurationState()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    for(quint8 idx = 0; idx < COVERLINE_MOTOR_MAX; idx++)
    {
        if(m_pMotorCoverLine[idx] == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        if(!connect(m_pMotorCoverLine[idx], SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
        {
            SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_AGITATION);
            FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportReferenceMovementAckn' failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
        if(!connect(m_pMotorCoverLine[idx], SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16 )), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
        {
            SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_AGITATION);
            FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportMovementAckn' failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
        if(!connect(m_pMotorCoverLine[idx], SIGNAL(ReportEncoderPosition(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorPosition(quint32, ReturnCode_t, qint32))))
        {
            SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_AGITATION);
            FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportEncoderPosition' failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
        if(!connect(m_pMotorCoverLine[idx], SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
        {
            SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_AGITATION);
            FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportError' failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }

    if((m_pDigInpSlideDetection == 0) || (m_pDigInpEDMLevel == 0) || (m_pDigOutpVacuumValve1 == 0) ||
       (m_pDigOutpVacuumValve2 == 0) || (m_pAnaInpPressureSensor == 0) || (m_pAnaInpHallSensor1 == 0))
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }


    //slide detection
    if(!connect(m_pDigInpSlideDetection, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(DigInputChanged(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pDigInpSlideDetection->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportActInputValue' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigInpSlideDetection, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(DigInputError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pDigInpSlideDetection->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportError' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //EDM level sensor
    if(!connect(m_pDigInpEDMLevel, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(DigInputChanged(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pDigInpEDMLevel->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportActInputValue' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigInpEDMLevel, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(DigInputError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pDigInpEDMLevel->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportError' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //vacuum valve 1
    if(!connect(m_pDigOutpVacuumValve1, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t)), this, SLOT(DigOutputValue(quint32, ReturnCode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pDigOutpVacuumValve1->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigOutpVacuumValve1, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(DigOutputError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pDigOutpVacuumValve1->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportError' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //vacuum valve 2
    if(!connect(m_pDigOutpVacuumValve2, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t)), this, SLOT(DigOutputValue(quint32, ReturnCode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pDigOutpVacuumValve2->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigOutpVacuumValve2, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(DigOutputError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pDigOutpVacuumValve2->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportError' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //pressure sensor
    if(!connect(m_pAnaInpPressureSensor, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(AnalogInputChanged(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pAnaInpPressureSensor->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect analog input signal 'ReportActOutputValue' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pAnaInpPressureSensor, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(AnalogInputError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pAnaInpPressureSensor->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect analog input signal 'ReportError' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //hall sensor 1
    if(!connect(m_pAnaInpHallSensor1, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(AnalogInputChanged(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pAnaInpHallSensor1->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect analog input signal 'ReportActOutputValue' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pAnaInpHallSensor1, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(AnalogInputError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED, (quint16) m_pAnaInpHallSensor1->GetModuleHandle());
        FILE_LOG_L(laDEV, llERROR) << "   Connect analog input signal 'ReportError' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return retval;

}

/****************************************************************************/
/*!
 *  \brief   Handles the device tasks configuration state.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CCoverLineDevice::HandleDeviceTasksConfigurationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    DeviceTask* pDeviceTask;

    if(m_pCoverLineNavigator == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, DEVICE_TASK_REFRUN);
    m_DeviceTaskMap[DEVICE_TASK_REFRUN] = pDeviceTask;
    RetVal = m_pCoverLineNavigator->CreateReferenceRunFctModTasks(this, pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, DEVICE_TASK_BASIS_POS);
    m_DeviceTaskMap[DEVICE_TASK_BASIS_POS] = pDeviceTask;
    RetVal = m_pCoverLineNavigator->CreateBasisPosFctModTasks(this, pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, DEVICE_TASK_RACK_POS);
    m_DeviceTaskMap[DEVICE_TASK_RACK_POS] = pDeviceTask;
    RetVal = m_pCoverLineNavigator->CreateRackPosTasks(this,  pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, DEVICE_TASK_SLIDE_SHIFT_OUT);
    m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT] = pDeviceTask;
    RetVal = m_pCoverLineNavigator->CreateSlideShiftOutTasks(this,  pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, DEVICE_TASK_COVER_SLIP_EXEC_COVER);
    m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER] = pDeviceTask;
    RetVal = m_pCoverLineNavigator->CreateCoverSlipCoverTasks(this,  pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    //m_pCoverLineNavigator->CreateReturnSildeTasks(this,  m_DeviceTask[DEVICE_TASK_REFRUN].m_FctModTaskList);

    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, DEVICE_TASK_SLIDE_SHIFT_BACK);
    m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK] = pDeviceTask;
    RetVal = m_pCoverLineNavigator->CreateSlideShiftBackTasks(this, pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, DEVICE_TASK_COVER_SLIP_GRAB);
    m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB] = pDeviceTask;
    RetVal = m_pCoverLineNavigator->CreateCoverSlipGrabTasks(this, pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, DEVICE_TASK_COVER_SLIP_DISCARD);
    m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD] = pDeviceTask;
    RetVal = m_pCoverLineNavigator->CreateCoverSlipDiscardTasks(this, pDeviceTask);

    return RetVal;
}


/****************************************************************************/
/*!
 *  \brief   Handles the classes motor configuration state.
 *
 *           This function handles the assigned motors configuration procedure.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
void CCoverLineDevice::HandleMotorConfigurationState()
{
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
void CCoverLineDevice::HandleIdleState()
{
    if(m_taskID == COVERLINE_DEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_taskID == COVERLINE_DEV_TASKID_REFRUN)
    {
        HandleReferenceRunTask();
    }
    else if(m_taskID == COVERLINE_DEV_TASKID_COVER)
    {
        HandleCoverTask();
    }
    else if(m_taskID == COVERLINE_DEV_TASKID_REQ_ACTPOS)
    {
        ;
    }
    else if(m_taskID == COVERLINE_DEV_TASKID_REQ_ACTSPEED)
    {
        ;
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_taskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INVALID_STATE, (quint16) m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the reference run task
 *
 *   This task controls the reference run procedure.
 *   The function is called from HandleIdleState() if m_taskID == COVERLINE_DEV_TASKID_REFRUN.
 *   It handles the reference run task, which consists of the following states:
 *
 *     COVERLINE_DEV_TASK_STATE_REQ :
 *        Start the device task DEVICE_TASK_REFRUN by setting its' m_TaskState variable
 *
 *     COVERLINE_DEV_TASK_STATE_PROGRESS_ACTION_STEPS:
 *        Calls the functionmodule tasks if the are ready to be started and finishes the
 *        reference run task after completion, the acknowlege is forwarded to the interface class
 *
 *     COVERLINE_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class by switching to error main state to
 *        Send the task's acknowledge signal, hdlInfo is set from
 *        m_lastErrorHdlInfo, which contains the error code as returned by the
 *        failed code
 *
 *     All other states, which are unexpected, will result in a COVERLINE_DEV_TASK_STATE_ERROR
 *      state to forward the error

 *  \return   void
 */
/****************************************************************************/
void CCoverLineDevice::HandleReferenceRunTask()
{
    if((m_pDevProc == 0) || (m_pCoverLineNavigator == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == COVERLINE_DEV_TASK_STATE_REQ)
    {
        ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_REFRUN])->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
        m_taskState = COVERLINE_DEV_TASK_STATE_PROGRESS_ACTION_STEPS;
        FILE_LOG_L(laDEV, llINFO) << "##########################################################";
        FILE_LOG_L(laDEV, llINFO) << "  set device task 'Reference run'' to 'progress'.";
    }
    else if (m_taskState == COVERLINE_DEV_TASK_STATE_PROGRESS_ACTION_STEPS)
    {
        bool bActionStateInProgress = false;
        bool bActionStateWithErrorDetect = false;

        DeviceTask* pDeviceTask;

        pDeviceTask = m_DeviceTaskMap[DEVICE_TASK_REFRUN];

        CheckTasksForStartup(bActionStateInProgress,
                             bActionStateWithErrorDetect);

        if(bActionStateWithErrorDetect == true)
        {
            m_taskState = COVERLINE_DEV_TASK_STATE_ERROR;
        }
        else if(bActionStateInProgress == false)
        {
            FunctionModuleTask* pFunctionModuleTask1;
            QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);

            // set all tasks to state 'unused'
            while (iterFctModTask.hasNext())
            {
                pFunctionModuleTask1 = iterFctModTask.next().value();
                pFunctionModuleTask1->m_TaskState = FunctionModuleTask::FCTMOD_STATE_UNUSED;
            }

            // reset the device's state
            pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task 'Reference run'' to 'finished'.";

            // route the information to interface class
            //m_pICoverLineDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo);

            //reset the states
            ResetTask();
        }
    }
    else if(m_taskState == COVERLINE_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << "  CCoverLineDevice: error COVERLINE_DEV_TASKID_REFRUN ";

        //m_pICoverLineDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo);

        ResetTask();

        // its a cover line error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = COVERLINE_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INVALID_STATE, (quint16) m_taskState);
    }

}

/****************************************************************************/
/*!
 *  \brief    Handles the cover task
 *
 *   This task controls the cover procedure of a rack
 *   The function is called from HandleIdleState() if m_taskID == COVERLINE_DEV_TASKID_COVER.
 *   It handles the reference run task, which consists of the following states:
 *
 *     COVERLINE_DEV_TASK_STATE_REQ :
 *        Start the device task DEVICE_TASK_REFRUN by setting its' m_TaskState variable
 *
 *     COVERLINE_DEV_TASK_STATE_PROGRESS_ACTION_STEPS:
 *        Starts the tasks, check timeout to observe the actions, and forwards the acknowledge to
 *        the interface class.
 *
 *     COVERLINE_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class by switching to error main state to
 *        Send the task's acknowledge signal, hdlInfo is set from
 *        m_lastErrorHdlInfo, which contains the error code as returned by the
 *        failed code
 *
 *     All other states, which are unexpected, will result in a COVERLINE_DEV_TASK_STATE_ERROR
 *      state to forward the error

 *  \return   void
 */
/****************************************************************************/
void CCoverLineDevice::HandleCoverTask()
{
    if((m_pDevProc == 0) || (m_pCoverLineNavigator == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == COVERLINE_DEV_TASK_STATE_REQ)
    {
        m_CoverSlipControl.m_ConditionClampOK = false;
        m_CoverSlipControl.m_ConditionCoverslipFailed = false;
        m_CoverSlipControl.m_ConditionCoverslipOK = false;

        //start the first task of the cover procedure, which is the 'basis position' task
        ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_RACK_POS])->m_TaskState = DeviceTask::DEV_TASK_STATE_INIT;
        ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT])->m_TaskState = DeviceTask::DEV_TASK_STATE_INIT;
        ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_TaskState = DeviceTask::DEV_TASK_STATE_INIT;
        ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK])->m_TaskState = DeviceTask::DEV_TASK_STATE_INIT;

        ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_BASIS_POS])->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
        ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_BASIS_POS])->m_timeAction.Trigger();
        ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_BASIS_POS])->m_aktionTimespan = 20000;
        FILE_LOG_L(laDEV, llINFO) << "##########################################################";
        FILE_LOG_L(laDEV, llINFO) << "  set device task 'BasisPos' to 'progress'.";
        m_taskState = COVERLINE_DEV_TASK_STATE_PROGRESS_ACTION_STEPS;

        m_CoverSlipControl.m_SubStateCover = 1;
    }
    else if (m_taskState == COVERLINE_DEV_TASK_STATE_PROGRESS_ACTION_STEPS)
    {
        bool bActionStateInProgress = false;
        bool bActionStateWithErrorDetect = false;
        QMapIterator<quint16, DeviceTask*> iterDeviceTask(m_DeviceTaskMap);
        DeviceTask*         pDeviceTask;

        while(iterDeviceTask.hasNext())
        {
            pDeviceTask = iterDeviceTask.next().value();
            if((pDeviceTask) && (pDeviceTask->m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS))
            {
                if(pDeviceTask->m_timeAction.Elapsed() > pDeviceTask->m_aktionTimespan)
                {
                    m_taskState = COVERLINE_DEV_TASK_STATE_ERROR;
                    FILE_LOG_L(laDEV, llERROR) << "  CCoverLineDevice: device task timeout: " << (int) pDeviceTask->m_Key;
                    return;
                }
            }
        }

        //check if any tasks pending for beeing started
        CheckTasksForStartup(bActionStateInProgress, bActionStateWithErrorDetect);

        //handle the cover process device tasks
        m_pCoverLineNavigator->HandleCoverLineDeviceTasks(this, m_DeviceTaskMap);

        //check cover process for having finished
        if(m_CoverSlipControl.m_SubStateCover == 99)
        {
            m_taskState = COVERLINE_DEV_TASK_STATE_ACTION_STEPS_ACKN;
        }

    }
    else if (m_taskState == COVERLINE_DEV_TASK_STATE_ACTION_STEPS_ACKN)
    {
        //reset the device and function module tasks
        QMapIterator<quint16, DeviceTask*> iterDeviceTask(m_DeviceTaskMap);
        DeviceTask*         pDeviceTask;
        //FunctionModuleTask* pFunctionModuleTask;

        while(iterDeviceTask.hasNext())
        {
            pDeviceTask = iterDeviceTask.next().value();
            m_pCoverLineNavigator->ResetDeviceTask(pDeviceTask);
            // reset the device's state
            pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
        }

        // route the information to interface class
        //m_pICoverLineDev->RouteCoverAckn(m_instanceID, m_lastErrorHdlInfo);

        //reset the states
        ResetTask();
    }
    else if(m_taskState == COVERLINE_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << "  CCoverLineDevice: error COVERLINE_DEV_TASKID_COVER ";

        //m_pICoverLineDev->RouteCoverAckn(m_instanceID, m_lastErrorHdlInfo);

        ResetTask();

        // its a cover line error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = COVERLINE_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INVALID_STATE, (quint16) m_taskState);
    }
}

/****************************************************************************/
/*!
 *  \brief    Checks, if there are function module task pending for beeing started, and start them
 *
 *  \iparam   bActionStateInProgress =
 *  \iparam   bActionStateWithErrorDetect =
 */
/****************************************************************************/
void CCoverLineDevice::CheckTasksForStartup(bool& bActionStateInProgress, bool& bActionStateWithErrorDetect)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    //search for function module tasks which are ready to be started
    QMapIterator<quint16, DeviceTask*> iterDeviceTask(m_DeviceTaskMap);
    DeviceTask*         pDeviceTask;

    //loop thru the device task map
    while(iterDeviceTask.hasNext())
    {
        pDeviceTask = iterDeviceTask.next().value();

        //only tasks with state 'progress' have function module tasks to startup
        if(pDeviceTask->m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {

            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(pDeviceTask->m_FctModTaskMap,
                                                                                             bActionStateInProgress,
                                                                                             bActionStateWithErrorDetect);
            while(pFunctionModuleTask)
            {
                ReturnCode_t RetVal;

                if(bActionStateWithErrorDetect == true)
                {
                    m_taskState = COVERLINE_DEV_TASK_STATE_ERROR;
                    FILE_LOG_L(laDEV, llERROR) << "  CCoverLineDevice: task with error state:" << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
                }


                FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice: task for startup found.";
                RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    FILE_LOG_L(laDEV, llERROR) << "  CCoverLineDevice: failed to start task.";
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(pDeviceTask->m_FctModTaskMap,
                                                                                                 bActionStateInProgress,
                                                                                                 bActionStateWithErrorDetect);
            }
        }
    }
}


/****************************************************************************/
/*!
 *  \brief    Handle the device task states
 *
 *  \return   void
 */
/****************************************************************************/
/*void CCoverLineDevice::HandleCoverDeviceTasks()
{
    //FILE_LOG_L(laDEV, llINFO) << " SubState ==  " << (int) m_SubStateCover;
    if(m_SubStateCover == 1)
    {
        if(m_DeviceTaskMap[DEVICE_TASK_BASIS_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            FILE_LOG_L(laDEV, llINFO) << " device task DEVICE_TASK_RACK_POS == " << (int) m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState;
            if(m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_INIT)
            {
                //move the rack to the topmost position
                m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
                ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_RACK_POS])->m_timeAction.Trigger();
                ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_RACK_POS])->m_aktionTimespan = 8000;

                FILE_LOG_L(laDEV, llINFO) << "##########################################################";
                FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_RACK_POS to 'progress'.";

                //grab a coverslip from depot
                m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
                ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_timeAction.Trigger();
                ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_aktionTimespan = 12000;
                FILE_LOG_L(laDEV, llINFO) << "##########################################################";
                FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_GRAB to 'progress'.";

                m_SubStateCover = 2;
            }
        }
    }
    else if(m_SubStateCover == 2)
    {
        /// \todo Move the rack to topmost position. If a slide was detected while move up, the coverslib grab task
        //      will be started.

        //rack is in topmost position and a coverslip was grabbed
        if((m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED) &&
           (m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED))
        {
            //shift out the slide
            m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT])->m_timeAction.Trigger();
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT])->m_aktionTimespan = 15000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_SLIDE_SHIFT_OUT to 'progress'.";

            //move coverslip to 'coverslip prepare position'
            //m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_PREP_COV]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            //((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_PREP_COV])->m_timeAction.Trigger();
            //((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_PREP_COV])->m_aktionTimespan = 8000;
            //FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            //FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_PREP_COV to 'progress'.";

            m_SubStateCover = 3;
            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 3";
        }
        else if (m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED_WITH_INFO)
        {
            m_taskState = COVERLINE_DEV_TASK_STATE_ACTION_STEPS_ACKN;
            m_SubStateCover = 99;
            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 99";
        }
        //coverslip grabbing has an unattractive result
        else if (m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED_WITH_INFO)
        {
            //the grabbed coverslip is brocken
            if(m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskInfo == DeviceTask::DEV_TASK_STATE_CS_GRAB_COVERSLIP_BROCKEN)
            {
                // set the coverslip grab tastto 'ready'
                m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState = DeviceTask::DEV_TASK_STATE_INIT;
                // start the coverslip engage task
                FILE_LOG_L(laDEV, llINFO) << "##########################################################";
                FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_DISCARD to 'progress'.";
                m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
                ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD])->m_timeAction.Trigger();
                ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD])->m_aktionTimespan = 15000;

                m_SubStateCover = 10;
                FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 10";
            }
        }
        //rack positoning task has an unattractive result
        else if (m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED_WITH_INFO)
        {
            //there was nor slide detected at the rack
            if(m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskInfo == DeviceTask::DEVICE_TASK_STATE_RACK_POS_NO_SLIDE)
            {
                //finish the stuff
                m_SubStateCover = 99;
                FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 99";
            }
        }
    }
    else if(m_SubStateCover == 3)
    {
        //slide was shift out, clamped and ready to cover, and a coverslip is at the cover 'coverslip prepare position'
        if((m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED))
        {
            //cover the slide
            m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER])->m_timeAction.Trigger();
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER])->m_aktionTimespan = 20000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_EXEC_COVER to 'progress'.";

            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 3";
            m_SubStateCover = 4;
        }
    }
    else if(m_SubStateCover == 4)
    {
        //slide was covered
        if(m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            //shift the slide back into the rack
            m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK])->m_timeAction.Trigger();
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK])->m_aktionTimespan = 6000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_SLIDE_SHIFT_BACK to 'progress'.";

            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 5";
            m_SubStateCover = 5;
        }
    }
    else if(m_SubStateCover == 5)
    {
        if(m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            FunctionModuleTask*  pFunctionModuleTask;
            CANStepperMotorTask* pCANStepperMotorTask;

            //the cover procedure of a slide was finished
            ResetDeviceTask(m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT]);
            ResetDeviceTask(m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]);
            ResetDeviceTask(m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER]);
            ResetDeviceTask(m_DeviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK]);
            ResetDeviceTask(m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD]);

            // restart rack positioning to enable the coverage of the next slide
            m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_RACK_POS])->m_timeAction.Trigger();
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_RACK_POS])->m_aktionTimespan = 6000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_RACK_POS to 'progress'.";

            //grab a coverslip from depot
            m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_timeAction.Trigger();
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_aktionTimespan = 12000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_GRAB to 'progress'.";


            pFunctionModuleTask = m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_FctModTaskMap[1];
            pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
            if(pCANStepperMotorTask)
            {
                if(pCANStepperMotorTask->m_TargetPos > 50)
                {
                    //move rack to the next slide position
                    pCANStepperMotorTask->m_TargetPos -= 50;
                    FILE_LOG_L(laDEV, llINFO) << "  TargetPosition: " <<  pCANStepperMotorTask->m_TargetPos;
                    m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_FctModTaskMap[1]->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                    m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_FctModTaskMap[2]->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                }
                else
                {
                    //move rack to the transfer position, device task will finish there
                    pCANStepperMotorTask->m_TargetPos = -20;
                    m_DeviceTaskMap[DEVICE_TASK_RACK_POS]->m_FctModTaskMap[1]->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                    FILE_LOG_L(laDEV, llINFO) << "  finish, all slides covered.";

                    //Set anything to finish the processing!!
                }
            }
            m_SubStateCover = 2;
            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 2";
        }
    }
    else if(m_SubStateCover == 10)
    {
        //coverslip was engaged
        if(m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            ResetDeviceTask(m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]);
            ResetDeviceTask(m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD]);

            m_SubStateCover = 2;
            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 2";

            //try next coverslip
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_GRAB to 'progress'.";
            m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) m_DeviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_timeAction.Trigger();
        }
    }
}*/

/****************************************************************************/
/*!
 *  \brief    Handles the error state of the class.
 *
 *   This task controls the error handling of the class.
 *   The function is called from HandleIdleState() if m_MainState == DEVICE_MAIN_STATE_ERROR.
 *   This task consists of the following states:
 *
 *     COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     COVERLINE_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     COVERLINE_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state
 *
 *     COVERLINE_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CCoverLineDevice::HandleErrorState()
{
    if(m_errTaskState == COVERLINE_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_errTaskState == COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pICoverLineDev != 0)
        {
            m_pICoverLineDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }*/
        m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_errTaskState == COVERLINE_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_errTaskState == COVERLINE_DEV_ERRTASK_STATE_RESET)
    {
        ResetTask();
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;

        // reset the function module references
        /*m_pMotorXAxis = 0;
        m_pMotorYAxis = 0;
        m_pMotorZAxis = 0;
        m_pRFID = 0;
        m_pLiquidLevel = 0;*/
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot will be called from CANStepperMotor if a reference run was acknowledged
 *
 *
 *  \iparam   instanceID = instance identifier of the CANStepperMotor
 *  \iparam   hdlInfo = return code
 *  \iparam   position = The actual motor position
 *
 */
/****************************************************************************/
void CCoverLineDevice::StepperMotorReferenceMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    CANStepperMotorTask* pCANStepperMotorTask;
    int DeviceTaskIndex;

    FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice InstanceID 0x" << std::hex << instanceID;

    pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    if(pFunctionModuleTask == 0)
    {
        FILE_LOG_L(laDEV, llINFO) << " no task found.";
    }
    while(pFunctionModuleTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState;
        Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);

        if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                if(pCANStepperMotorTask)
                {
                    pCANStepperMotorTask->m_ActPos = position;

                    ForwardTaskToDeviceTaskHandler(DeviceTaskIndex, pFunctionModuleTask);
                }
            }
            else
            {
                // error was detected from motor side
                FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while driveToPosition.. error";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
            }
            break;
        }
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    }
}

/****************************************************************************/
/*!
 *  \brief    Forwards the slot information of a function module's task to the device task handler
 *
 *
 *  \iparam   DeviceTaskIndex = device task index
 *  \iparam   pFunctionModuleTask = function module task
 *
 */
/****************************************************************************/
void CCoverLineDevice::ForwardTaskToDeviceTaskHandler(int DeviceTaskIndex, FunctionModuleTask* pFunctionModuleTask)
{
    DeviceTask* pDeviceTask;

    pDeviceTask = m_DeviceTaskMap[DeviceTaskIndex];

    if(m_pCoverLineNavigator)
    {
        if(DeviceTaskIndex == DEVICE_TASK_REFRUN)
        {
            m_pCoverLineNavigator->HandleRefRunTaskEvent(pFunctionModuleTask, pDeviceTask);
        }
        else if(DeviceTaskIndex == DEVICE_TASK_BASIS_POS)
        {
            m_pCoverLineNavigator->HandleBasisPositionTaskEvent(pFunctionModuleTask, pDeviceTask);
        }
        else if(DeviceTaskIndex == DEVICE_TASK_RACK_POS)
        {
            m_pCoverLineNavigator->HandleRackPositionTaskEvent(pFunctionModuleTask, pDeviceTask);
        }
        else if(DeviceTaskIndex == DEVICE_TASK_COVER_SLIP_GRAB)
        {
            m_pCoverLineNavigator->HandleCoverslipGrabTaskEvent(pFunctionModuleTask, pDeviceTask, this);
        }
        else if(DeviceTaskIndex == DEVICE_TASK_SLIDE_SHIFT_OUT)
        {
            m_pCoverLineNavigator->HandleSlideShiftOutTaskEvent(pFunctionModuleTask, pDeviceTask, this);
        }
        else if(DeviceTaskIndex == DEVICE_TASK_COVER_SLIP_EXEC_COVER)
        {
            m_pCoverLineNavigator->HandleCoverTaskEvent(pFunctionModuleTask, pDeviceTask, this);
        }
        else if(DeviceTaskIndex == DEVICE_TASK_SLIDE_SHIFT_BACK)
        {
            m_pCoverLineNavigator->HandleSlideShiftBackTaskEvent(pFunctionModuleTask, pDeviceTask, this);
        }
        else if(DeviceTaskIndex == DEVICE_TASK_COVER_SLIP_DISCARD)
        {
            m_pCoverLineNavigator->HandleCoverslipDiscardTaskEvent(pFunctionModuleTask, pDeviceTask, this);
        }

    }
}

/****************************************************************************/
/*!
 *  \brief    This slot will be called from CANStepperMotor if a movement was acknowledged
 *
 *
 *  \iparam   instanceID = instance identifier of the CANStepperMotor
 *  \iparam   hdlInfo = return code
 *  \iparam   position = The actual motor position
 *  \iparam   speed = The actual motor speed
 *
 */
/****************************************************************************/
void CCoverLineDevice::StepperMotorMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position, qint16 speed)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    CANStepperMotorTask* pCANStepperMotorTask;
    int DeviceTaskIndex;

    FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice, instanceID 0x" << std::hex << instanceID;

    pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    if(pFunctionModuleTask == 0)
    {
        FILE_LOG_L(laDEV, llINFO) << " no task found.";
    }

    while(pFunctionModuleTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState << " key: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
        Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);

        if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                if(pCANStepperMotorTask)
                {
                    pCANStepperMotorTask->m_ActPos = position;
                    pCANStepperMotorTask->m_ActSpeed = speed;

                    ForwardTaskToDeviceTaskHandler(DeviceTaskIndex, pFunctionModuleTask);
                }
            }
            else
            {
                // error was detected from motor side
                FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while driveToPosition.. error";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
            }
            //break; no, there are multible task, maybe
        }
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
        if(!pFunctionModuleTask)
        {
            FILE_LOG_L(laDEV, llDEBUG) << " GetTaskFromInstanceID " << std::hex << instanceID << " return 0 -> exit";
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot receives the actual position from CANStepperMotor
 *
 *
 *  \iparam   instanceID = instance identifier of the CANStepperMotor
 *  \iparam   hdlInfo = return code
 *  \iparam   position = The actual motor position
 *
 */
/****************************************************************************/
void CCoverLineDevice::StepperMotorPosition(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position)
{
    CANStepperMotorTask* pCANStepperMotorTask;
    FunctionModuleTask* pFunctionModuleTask = 0;
    int DeviceTaskIndex;

    Q_UNUSED(position);

    FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice, InstanceID 0x" << std::hex << instanceID;

    pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    if(pFunctionModuleTask == 0)
    {
        FILE_LOG_L(laDEV, llINFO) << " no task found.";
    }

    while(pFunctionModuleTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState << " key: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
        Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

        if((pCANStepperMotorTask != 0) &&
           (pCANStepperMotorTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS))
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pCANStepperMotorTask->m_TaskIndex << " state to finished.";
                pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
            }
            else
            {
                // error was detected from motor side
                FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while driveToPosition.. error";
                pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
            }
            break;
        }
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    }

}

/****************************************************************************/
/*!
 *  \brief   This slot receives the motor's error signal
 *
 *           This function will be called from CANStepperMotor and forwards the
 *           motor's error information.
 *           The function is called if an error was detected by the slave's function module.
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_MOTOR_SLV
 *  \iparam   ErrorCode  = from CAN-message
 *  \iparam   ErrorData  = from CAN-message
 *  \iparam   ErrorTime  = error message reception time on master side
 *
 *  \return  void
 *
 ****************************************************************************/
void CCoverLineDevice::StepperMotorError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE;
}


/****************************************************************************/
/*!
*  \brief    This slot receives the analog input error signal
*
*  \iparam   InstanceID = instance identifier of the device
*  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_ANA_INP_SLV or EVENT_GRP_DCL_FCTMOD_ANA_INP_DLC
*  \iparam   ErrorCode  = from CAN-message
*  \iparam   ErrorData  = from CAN-message
*  \iparam   ErrorTime  = error message reception time on master side
*
****************************************************************************/
void CCoverLineDevice::AnalogInputError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_errTaskState = COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief  Handles the data signal of the analog input function modul
 *
 *
 *  \iparam InstanceID = instance identifier of the function module
 *  \iparam hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if valid data,
 *                    otherwise the return value of the error detecting function
 *  \iparam InputValue = input value of the analog channel
 */
/****************************************************************************/
void CCoverLineDevice::AnalogInputChanged(quint32 InstanceID, ReturnCode_t hdlInfo, quint16 InputValue)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    CANAnalogInputTask* pCANAnalogInputTask;
    int DeviceTaskIndex;

    FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice instanceID: 0x" << std::hex << InstanceID;

    pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, InstanceID, pFunctionModuleTask, DeviceTaskIndex);
    while(pFunctionModuleTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState << " key: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();

        if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                pCANAnalogInputTask = dynamic_cast<CANAnalogInputTask*>(pFunctionModuleTask);
                if(pCANAnalogInputTask != 0)
                {
                    pCANAnalogInputTask->m_Value = InputValue;
                }

                ForwardTaskToDeviceTaskHandler(DeviceTaskIndex, pFunctionModuleTask);
            }
            else
            {
                // error was detected from motor side
                FILE_LOG_L(laDEV, llERROR) << " CANDigitalInput: while DigInputChanged.. error";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
            }
            break;
        }
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, InstanceID, pFunctionModuleTask, DeviceTaskIndex);
    }
}

/****************************************************************************/
/*
 *  \brief    Handles the data signal of the analog output function modul
 *
 *
 *  \iparam   instanceID = instance identifier of the function module
 *  \iparam   hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if valid data,
 *                      otherwise the return value of the error detecting function
 *  \iparam   outputValue = output value of the analog channel
 *
 *  \return  void
 *
 ****************************************************************************/
/*void CCoverLineDevice::AnalogOutputValue(quint32, ReturnCode_t, quint16)
{
}*/

/****************************************************************************/
/*
*  \brief    This slot receives the analog output error signal
*
*  \iparam   instanceID = instance identifier of the device
*  \iparam   errorGroup = EVENT_GRP_DCL_FCTMOD_ANA_OUTP_SLV or EVENT_GRP_DCL_FCTMOD_ANA_OUTP_DLC
*  \iparam   errorCode  = from CAN-message
*  \iparam   errorData  = from CAN-message
*  \iparam   errorTime  = error message reception time on master side
*
****************************************************************************/

/*void CCoverLineDevice::AnalogOutputError(quint32, quint16, quint16, quint16, QDateTime)
{
}*/

/****************************************************************************/
/*!
*  \brief    This slot receives the digital input error signal
*
*  \iparam   InstanceID = instance identifier of the device
*  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_DIG_INP_SLV or EVENT_GRP_DCL_FCTMOD_DIG_INP_DLC
*  \iparam   ErrorCode  = from CAN-message
*  \iparam   ErrorData  = from CAN-message
*  \iparam   ErrorTime  = error message reception time on master side
*
****************************************************************************/
void CCoverLineDevice::DigInputError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    //m_errTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Handles the data signal of the digital input function modul
 *
 *
 *  \iparam   instanceID = instance identifier of the function module
 *  \iparam   hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if valid data,
 *                      otherwise the return value of the error detecting function
 *  \iparam   InputValue = input value of the dig. channel
 *
 *  \return  void
 *
 ****************************************************************************/
void CCoverLineDevice::DigInputChanged(quint32 instanceID, ReturnCode_t hdlInfo, quint16 InputValue)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    CANDigitalInputTask* pCANDigitalInputTask;
    int DeviceTaskIndex;

    FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice, InstanceID 0x" << std::hex << instanceID;

    pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    while(pFunctionModuleTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState << " key: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
        Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT);

        if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                pCANDigitalInputTask = dynamic_cast<CANDigitalInputTask*>(pFunctionModuleTask);
                if(pCANDigitalInputTask != 0)
                {
                    pCANDigitalInputTask->m_Value = InputValue;
                }

                ForwardTaskToDeviceTaskHandler(DeviceTaskIndex, pFunctionModuleTask);
            }
            else
            {
                // error was detected from motor side
                FILE_LOG_L(laDEV, llERROR) << " CANDigitalInput: while DigInputChanged.. error";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
            }
            break;
        }
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    }
}

/****************************************************************************/
/*!
*  \brief    This slot receives the digital output error signal
*
*  \iparam   InstanceID = instance identifier of the device
*  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_DIG_OUTP_SLV or EVENT_GRP_DCL_FCTMOD_DIG_OUTP_DLC
*  \iparam   ErrorCode  = from CAN-message
*  \iparam   ErrorData  = from CAN-message
*  \iparam   ErrorTime  = error message reception time on master side
*
****************************************************************************/
void CCoverLineDevice::DigOutputError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(ErrorGroup);
    Q_UNUSED(ErrorCode);
    Q_UNUSED(ErrorData);
    Q_UNUSED(ErrorTime);
}

/****************************************************************************/
/*!
 *  \brief    Handles the data signal of the digital output function modul
 *
 *
 *  \iparam   instanceID = instance identifier of the function module
 *  \iparam   hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if valid data,
 *                      otherwise the return value of the error detecting function
 *
 *  \return  void
 *
 ****************************************************************************/
void CCoverLineDevice::DigOutputValue(quint32 instanceID, ReturnCode_t hdlInfo)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    CANDigitalOutputTask* pCANDigitalOutputTask;
    int DeviceTaskIndex;

    FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice InstanceID 0x" << std::hex << instanceID;

    pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    while(pFunctionModuleTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState << " key: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
        Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_DIG_OUTPUT);

        if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                pCANDigitalOutputTask = dynamic_cast<CANDigitalOutputTask*>(pFunctionModuleTask);
                if(pCANDigitalOutputTask != 0)
                {
                    //this is just to keep it konsistent
                    //pCANDigitalOutputTask->m_Value = InputValue;
                }

                ForwardTaskToDeviceTaskHandler(DeviceTaskIndex, pFunctionModuleTask);
            }
            else
            {
                // error was detected from motor side
                FILE_LOG_L(laDEV, llERROR) << " CANDigitalInput: while DigInputChanged.. error";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
            }
            break;
        }
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    }
}

/****************************************************************************/
/*!
 *  \brief    Request all motors reference run
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted, otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CCoverLineDevice::ReqReferenceRun()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if((m_taskID == COVERLINE_DEV_TASKID_FREE) && (m_taskState == COVERLINE_DEV_TASK_STATE_FREE))
    {
        m_taskID = COVERLINE_DEV_TASKID_REFRUN;
        m_taskState = COVERLINE_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "    CCoverLineDevice::ReqReferenceRun ";
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "   CCoverLineDevice::ReqReferenceRun failed, taskID:" << (int) m_taskID <<" taskState:" << (int) m_taskState;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}


/****************************************************************************/
/*!
 *  \brief  Request the cover procedure of a whole rack
 *
 *  \iparam RackType = Rack type
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted,
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CCoverLineDevice::ReqCoverProcedure(RackType_t RackType)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if((m_taskID == COVERLINE_DEV_TASKID_FREE) && (m_taskState == COVERLINE_DEV_TASK_STATE_FREE))
    {
        m_taskID = COVERLINE_DEV_TASKID_COVER;
        m_taskState = COVERLINE_DEV_TASK_STATE_REQ;
        m_RackType = RackType;
        FILE_LOG_L(laDEV, llINFO) << "    CCoverLineDevice::ReqCoverProcedure ";
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "   CCoverLineDevice::ReqCoverProcedure failed, taskID:" << (int) m_taskID <<" taskState:" << (int) m_taskState;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief   Free the taskID and the task state
 *
 *
 *  \return
 *
 ****************************************************************************/
void CCoverLineDevice::ResetTask()
{
    m_taskID = COVERLINE_DEV_TASKID_FREE;
    m_taskState = COVERLINE_DEV_TASK_STATE_FREE;
}

}
