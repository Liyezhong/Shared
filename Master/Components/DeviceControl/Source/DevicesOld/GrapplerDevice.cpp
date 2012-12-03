/******************************************************************/
/*! \file GrapplerDevice.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CGrapplerDevice
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

#include "DeviceControl/Include/Devices/GrapplerDevice.h"
#include "DeviceControl/Include/Devices/GrapplerNavigator.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/SystemPaths.h"

#include <QStringList>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief    Constructor of CGrapplerDevice
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *  \param    instanceID = identifier of the class instance
 *
 *  \return
 *
 ***************************************************************************/
CGrapplerDevice::CGrapplerDevice(DeviceProcessing* pDeviceProcessing, QString Type, DevInstanceID_t instanceID)
    : CBaseDevice(pDeviceProcessing, Type),
    m_pGrapplerNavigator(0), m_pMotorXAxis(0), m_pMotorYAxis(0), m_pMotorZAxis(0), m_pRFID(0), m_pLiquidLevel(0),
    m_stationPosX(0), m_stationPosY(0), m_nextStationRow(0), m_nextStationColumn(0), m_nextStationPosX(0), m_nextStationPosY(0),
    m_ActStationAction(STATION_ACTION_UNDEF), m_actRfidDataMSB(0), m_actRfidDataLSB(0), m_actLiquidLevelValue(0),
    m_PositionZAxisTransportUnload(0), m_PositionZAxisTransportLoad(0), m_PositionZAxisCuvette(0),
    m_ProfileZAxisUpUnload(0), m_ProfileZAxisUpLoadSlow(0), m_ProfileZAxisUpLoadFast(0), m_CmdDelayZAxisUpLoadFast(0), m_PositionZAxisUpAcc(0),
    m_ProfileZAxisDownUnload(0), m_ProfileZAxisDownLoad(0),
    m_ProfileXAxisUnload(0), m_ProfileXAxisLoad(0), m_ProfileYAxisUnload(0), m_ProfileYAxisLoad(0),
    m_PositionX1AxisPark(0), m_PositionX2AxisPark(0), m_PositionY1AxisPark(0), m_PositionY2AxisPark(0), m_PositionZ1AxisPark(0), m_PositionZ2AxisPark(0),
    m_OffsetYAxisLoad(0), m_OffsetYAxisUnload(0), m_ProfileYAxisGrabLoad(0),
    m_TransportState(TRANSP_STATE_UNKNOWN)
{
    m_MainState = DEVICE_MAIN_STATE_START;
    m_MainStateOld = m_MainState;

    // normal operation tasks and states, error state
    m_taskID       = GRAPPLER_DEV_TASKID_INIT;
    m_taskState    = GRAPPLER_DEV_TASK_STATE_FREE;
    m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_FREE;

    m_instanceID = instanceID;

    Q_ASSERT( ((m_instanceID == DEVICE_INSTANCE_ID_GRAPPLER_1) ||
               (m_instanceID == DEVICE_INSTANCE_ID_GRAPPLER_2)) );

    for(int idxCol = 0; idxCol < 18; idxCol++)
    {
        for(int idxRow = 0; idxRow < 4; idxRow++)
        {
            m_stationPosition[idxRow][idxCol].accessible = 0;
            m_stationPosition[idxRow][idxCol].positionX = 0;
            m_stationPosition[idxRow][idxCol].positionY = 0;
            m_stationAdjust[idxRow][idxCol].positionX = 0;
            m_stationAdjust[idxRow][idxCol].positionY = 0;
        }
    }

    m_actRfidDataMSB = 0;
    m_actRfidDataLSB = 0;

    //actual liquid level
    m_actLiquidLevelValue = 0;

    m_pGrapplerNavigator = new CGrapplerNavigator();

    FILE_LOG_L(laDEV, llINFO) << " create station device grappler " << (int) m_instanceID;
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CGrapplerDevice
 *
 *
 *  \return
 *
 ****************************************************************************/
CGrapplerDevice::~CGrapplerDevice()
{
    m_pDevProc = 0;
    m_pMotorXAxis = 0;
    m_pMotorYAxis = 0;
    m_pMotorZAxis = 0;
    m_pRFID = 0;
    m_pLiquidLevel = 0;

    try
    {
        if(m_pGrapplerNavigator)
        {
            delete m_pGrapplerNavigator;
            m_pGrapplerNavigator = 0;
        }
    }
    catch (...)
    {
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
 */
/****************************************************************************/
void CGrapplerDevice::HandleTasks()
{
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
        ReturnCode_t RetVal;

        RetVal = HandleInitializationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_CONFIG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   CGrapplerDevice:HandleInitializationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        ReturnCode_t RetVal;

        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_DEV_TASK_CFG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   CGrapplerDevice:HandleConfigurationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_DEV_TASK_CFG)
    {
        ReturnCode_t RetVal;

        RetVal = HandleDeviceTasksConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
            m_taskID = GRAPPLER_DEV_TASKID_FREE;
        }
    }

    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CGrapplerDevice:handleTasks: new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialisation state.
 *
 *           This function attaches the function modules to the pointers, according to the loaderID,
 *           which differs loader and unlaoder
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CGrapplerDevice::HandleInitializationState()
{
    FILE_LOG_L(laDEV, llINFO) << "  CGrapplerDevice::Initialize(): Grappler " << (int) m_instanceID;

    m_pMotorXAxis  = (CStepperMotor*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorXAxisKey));
    m_pMotorYAxis  = (CStepperMotor*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorYAxisKey));
    m_pMotorZAxis  = (CStepperMotor*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorZAxisKey));
    m_pRFID        = (CRfid11785*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_GrapplerRFIDKey));
    m_pLiquidLevel = (CAnalogInput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_GrapplerLiquidLevelKey));

    // check if the function module could be allocated
    if(m_pMotorXAxis == 0)
    {
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_X_AXIS_GRAP1/2 not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pMotorYAxis == 0)
    {
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Y_AXIS_GRAP1);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_Y_AXIS_GRAP1/2 not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pMotorZAxis == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_Z_AXIS_GRAP1/2 not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRFID == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RFID_GRAP1);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RFID_GRAP1/2 not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pLiquidLevel == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_LIQUIDLEV_GRAP1);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_LIQUIDLEV_GRAP1/2 not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    FILE_LOG_L(laDEV, llINFO) << "  Grabber 1: ";
    FILE_LOG_L(laDEV, llINFO) << "             motor " << m_pMotorXAxis->GetName().toStdString();
    FILE_LOG_L(laDEV, llINFO) << "             rfid " << m_pRFID->GetName().toStdString();
    FILE_LOG_L(laDEV, llINFO) << "  ... done";

    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief   Handles the classes configuration state.
 *
 *           This function connects each function module's signals to the internal slots.
 *           Furthermore, the station layout will be read.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if configuration was successfully executed
 *           otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
ReturnCode_t CGrapplerDevice::HandleConfigurationState()
{
    ReturnCode_t RetVal;

    RetVal = ReadStationLayout();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = ReadAdjustment();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            if(!connect(m_pMotorXAxis, SIGNAL(ReportSetStateAckn(quint32, ReturnCode_t)), this, SLOT(StepperMotorSetStateAckn(quint32, ReturnCode_t))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect X-motor 'ReportSetStateAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorYAxis, SIGNAL(ReportSetStateAckn(quint32, ReturnCode_t)), this, SLOT(StepperMotorSetStateAckn(quint32, ReturnCode_t))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Y-motor 'ReportSetStateAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorZAxis, SIGNAL(ReportSetStateAckn(quint32, ReturnCode_t)), this, SLOT(StepperMotorSetStateAckn(quint32, ReturnCode_t))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Z-motor 'ReportSetStateAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pMotorXAxis, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect X-motor 'ReportReferenceMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorYAxis, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Y_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Y-motor 'ReportReferenceMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorZAxis, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Z-motor 'ReportReferenceMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pMotorXAxis, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect X-motor 'ReportMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorYAxis, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Y_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Y-motor 'ReportMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorZAxis, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Z-motor 'ReportMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pMotorXAxis, SIGNAL(ReportPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorPosition(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect X-motor 'ReportPosition' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorYAxis, SIGNAL(ReportPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorPosition(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Y_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Y-motor 'ReportPosition' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorZAxis, SIGNAL(ReportPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorPosition(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Z-motor 'ReportPosition' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pMotorXAxis, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect x-motor 'ReportError' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorYAxis, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Y_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect y-motor 'ReportError' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorZAxis, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect z-motor 'ReportError' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pRFID, SIGNAL(ReportUserData(quint32, ReturnCode_t, quint8, quint32)), this, SLOT(RFIDDataReceived(quint32, ReturnCode_t, quint8, quint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RFID_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect RFID signal 'ReportReadData' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pRFID, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(RFIDError(quint32, quint16, quint16, quint16, QDateTime))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RFID_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect RFID signal 'ReportError' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pLiquidLevel, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(AnalogInputChanged(quint32, ReturnCode_t, quint16))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_LIQUIDLEV_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect liquid sensor signal 'ReportActInputValue' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pLiquidLevel, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(AnalogInputError(quint32, quint16, quint16, quint16, QDateTime))))
            {
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_LIQUIDLEV_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect liquid sensor signal 'ReportError' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
        }
    }

    m_PositionZAxisTransportUnload = GetProcSettingPosition("z_axis_pos_transport_unload");
    m_PositionZAxisTransportLoad   = GetProcSettingPosition("z_axis_pos_transport_load");
    m_PositionZAxisCuvette         = GetProcSettingPosition("z_axis_pos_cuvette");
    m_ProfileZAxisUpLoadSlow       = GetProcSettingPosition("z_axis_pull_slow_speed_motion_profile");
    m_ProfileZAxisUpLoadFast       = GetProcSettingPosition("z_axis_pull_fast_speed_motion_profile");
    //Todo : Folling entry shall be created in ProcessSettings.xml
    m_CmdDelayZAxisUpLoadFast      = GetProcSettingPosition("z_axis_pull_cmd_delay_fast_speed");
    m_PositionZAxisUpAcc           = GetProcSettingPosition("z_axis_pull_acc_position");
    m_ProfileZAxisUpUnload         = GetProcSettingPosition("z_axis_pull_unload_motion_profile");

    m_ProfileZAxisDownUnload       = GetProcSettingPosition("z_axis_down_unload_speed_motion_profile");
    m_ProfileZAxisDownLoad         = GetProcSettingPosition("z_axis_down_load_speed_motion_profile");

    m_ProfileXAxisUnload = GetProcSettingPosition("x_axis_unload_speed_motion_profile");
    m_ProfileXAxisLoad   = GetProcSettingPosition("x_axis_load_speed_motion_profile");
    m_ProfileYAxisUnload = GetProcSettingPosition("y_axis_unload_speed_motion_profile");
    m_ProfileYAxisLoad   = GetProcSettingPosition("y_axis_load_speed_motion_profile");

    m_PositionX1AxisPark   = GetProcSettingPosition("x1_axis_park_position");
    m_PositionX2AxisPark   = GetProcSettingPosition("x2_axis_park_position");
    m_PositionY1AxisPark   = GetProcSettingPosition("y1_axis_park_position");
    m_PositionY2AxisPark   = GetProcSettingPosition("y2_axis_park_position");
    m_PositionZ1AxisPark   = GetProcSettingPosition("z1_axis_park_position");
    m_PositionZ2AxisPark   = GetProcSettingPosition("z2_axis_park_position");

    //TODO : Following two lines shall be uncommented and to be initialised in constructor if those entries are relevant in ProcessSettings.xml
    //m_PositionZAxisLoadTransport   = GetProcSettingPosition("z_axis_load_transport_pos");
    //m_PositionZAxisUnloadTransport   = GetProcSettingPosition("z_axis_unload_transport_pos");

    m_OffsetYAxisLoad   = GetProcSettingPosition("y_axis_offset_rack_attached");
    m_OffsetYAxisUnload   = GetProcSettingPosition("y_axis_offset_rack_detached");

    m_ProfileYAxisGrabLoad   = GetProcSettingPosition("y_axis_grab_speed_motion_profile");

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Initialize and configures the devise tasks
 *
 *   This device has three device tasks
 *     - reference run
 *     - motor position request, get all motors position
 *     - station action, position the grappler and execute the requested action (rack grap/discard, rfid, liquid)
 *   The function calls the grappler navigator functions to configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if configuration was successfully executed
 *           otherwise DCL_ERR_FCT_CALL_FAILED
 */
/****************************************************************************/
ReturnCode_t CGrapplerDevice::HandleDeviceTasksConfigurationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    RetVal = m_pGrapplerNavigator->CreateReferenceRunFctModTasks(this, m_DeviceTask[GRAPPLER_DEV_TASK_REFRUN].m_FctModTaskMap);
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pGrapplerNavigator->CreatePositionReqFctModTasks(this, m_DeviceTask[GRAPPLER_DEV_TASK_POS_REQ].m_FctModTaskMap);
    }

    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pGrapplerNavigator->CreateStationActionFctModTasks(this, m_DeviceTask[GRAPPLER_DEV_TASK_GRAB].m_FctModTaskMap);
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
 */
/****************************************************************************/
void CGrapplerDevice::HandleIdleState()
{

    if(m_taskID == GRAPPLER_DEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_taskID == GRAPPLER_DEV_TASKID_REFRUN)
    {
        HandleReferenceRunTask();
    }
    else if((m_taskID == GRAPPLER_DEV_TASKID_ACTION_REQ) || (m_taskID == GRAPPLER_DEV_TASKID_ACTION_TIME_REQ))
    {
        HandleActionTask();
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_taskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_INVALID_STATE, (quint16) m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the reference run task
 *
 *   This task controls the reference run procedure.
 *   The function is called from HandleIdleState() if m_taskID == GRAPPLER_DEV_TASKID_REFRUN.
 *   It handles the reference run task, which consists of the following states:
 *
 *     GRAPPLER_DEV_TASK_STATE_REQ :
 *        Calls the GrapplerNavigator::GRAPPLER_DEV_TASK_STATE_ERROR function which creates the
 *        commands to execute all motor's reference run in correct order
 *
 *     GRAPPLER_DEV_TASK_STATE_PROGRESS_ACTION_STEPS:
 *        Starts the tasks, check timeout to observe the actions, and forwards the acknowledge to
 *        the interface class.
 *
 *     GRAPPLER_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class by switching to error main state to
 *        Send the task's acknowledge signal, hdlInfo is set from
 *        m_lastErrorHdlInfo, which contains the error code as returned by the
 *        failed code
 *
 *     All other states, which are unexpected, will result in a GRAPPLER_DEV_TASK_STATE_ERROR
 *      state to forward the error

 *  \return   void
 */
/****************************************************************************/
void CGrapplerDevice::HandleReferenceRunTask()
{
    if((m_pDevProc == 0) || (m_pGrapplerNavigator == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == GRAPPLER_DEV_TASK_STATE_REQ)
    {
        m_DeviceTask[GRAPPLER_DEV_TASK_REFRUN].m_TaskState = DeviceTask::DEV_TASK_STATE_START;
        m_taskState = GRAPPLER_DEV_TASK_STATE_PROGRESS_ACTION_STEPS;
    }
    else if (m_taskState == GRAPPLER_DEV_TASK_STATE_PROGRESS_ACTION_STEPS)
    {
        FunctionModuleTask* pFunctionModuleTask = 0;
        bool bActionStateInProgress = false;
        bool bActionStateWithErrorDetect = false;

        m_DeviceTask[GRAPPLER_DEV_TASK_REFRUN].m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;

        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(m_DeviceTask[GRAPPLER_DEV_TASK_REFRUN].m_FctModTaskMap,
                                                                                         bActionStateInProgress,
                                                                                         bActionStateWithErrorDetect);

        if(pFunctionModuleTask)
        {
            ReturnCode_t RetVal;

            FILE_LOG_L(laDEV, llINFO) << "  CGrapplerDevice: task for startup found.";
            RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
            {
                FILE_LOG_L(laDEV, llERROR) << "  CGrapplerDevice: failed to start task.";
            }
        }

        if(bActionStateWithErrorDetect == true)
        {
            m_taskState = GRAPPLER_DEV_TASK_STATE_ERROR;
        }
        else if(bActionStateInProgress == false)
        {
            //FunctionModuleTask* pFunctionModuleTask1;
            //QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(m_DeviceTask[GRAPPLER_DEV_TASK_REFRUN].m_FctModTaskMap);

            // set all tasks to state 'unused'
            /*while (iterFctModTask.hasNext())
            {
                pFunctionModuleTask1 = iterFctModTask.next().value();
                pFunctionModuleTask1->m_TaskState = FunctionModuleTask::FCTMOD_STATE_UNUSED;
            }*/

            m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[GRAPPLER_DEV_TASK_REFRUN].m_FctModTaskMap);

            // reset the device's state
            m_DeviceTask[GRAPPLER_DEV_TASK_REFRUN].m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;

            // route the acknowledge information to interface class
            //m_pIGrapplerDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo);

            //reset the states
            ResetTask();
        }
    }
    else if(m_taskState == GRAPPLER_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << "  CGrapplerDevice: error GRAPPLER_DEV_TASKID_REFRUN ";

        m_TransportState = TRANSP_STATE_REFPOS;

        //m_pIGrapplerDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo);

        ResetTask();

        // its a grappler error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = GRAPPLER_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_INVALID_STATE, (quint16) m_taskState);
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles all the tasks need to execute a requested action
 *
 *           An action is separated into several tasks:
 *            - three tasks to request the actual motor positions. The actual motor positions
 *              are the base to calculate the movements for this action
 *           When received the positions, a call of CalculateGrapplerActionTasks is done to calculate the tasks need to
 *           arrange the requested action. Mostly there are some positioning tasks to transport the grappler to
 *           a cuvette, attach to a rack or detach from rack.
 *           If an RFID read, or a liquid level read was requested, an RFID- or liquid level sensor data request task will
 *           be added.
 *
 *
 *
 ****************************************************************************/
void CGrapplerDevice::HandleActionTask()
{
    if((m_pDevProc == 0) || (m_pGrapplerNavigator == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == GRAPPLER_DEV_TASK_STATE_REQ)
    {
        // Request the motor positions before calculating and starting any movement.
        // Calculate the tasks to send the 'ActPositionReq' CAN message to all motors.
        // The actual position will be need by movement calculation for this action
        ReturnCode_t result;

        // calculate the tasks to request the motor positions
        result = m_pGrapplerNavigator->CalculatePositionRequestTasks(this, m_DeviceTask[GRAPPLER_DEV_TASK_POS_REQ].m_FctModTaskMap);
        if(result == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_taskState = GRAPPLER_DEV_TASK_STATE_ACTPOS_REQ_SEND;
            m_DeviceTask[GRAPPLER_DEV_TASK_POS_REQ].m_TaskState = DeviceTask::DEV_TASK_STATE_START;
        }
        else
        {
            m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_FAILED;
            m_taskState = GRAPPLER_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_REQACTPOS_FCT_CALL_FAILED, (quint16) m_taskID);

            FILE_LOG_L(laDEV, llERROR) << " CGrapplerDevice: error while CalulateActPosReq: " << (int) result;
        }
    }
    else if(m_taskState == GRAPPLER_DEV_TASK_STATE_ACTPOS_REQ_SEND)
    {
        // execute the 'position request' device task.
        // the 'ActPositionReq' will be send to each motor, and this state awaits the response CAN messages
        FunctionModuleTask* pFunctionModuleTask = 0;
        bool bActionStateInProgress = false;
        bool bActionStateWithErrorDetect = false;

        m_DeviceTask[GRAPPLER_DEV_TASK_POS_REQ].m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;

        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(m_DeviceTask[GRAPPLER_DEV_TASK_POS_REQ].m_FctModTaskMap,
                                                                                         bActionStateInProgress,
                                                                                         bActionStateWithErrorDetect);

        while(pFunctionModuleTask)
        {
            if(pFunctionModuleTask)
            {
                ReturnCode_t RetVal;

                FILE_LOG_L(laDEV, llINFO) << "  CGrapplerDevice: task for startup found.";
                RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                    m_taskState = GRAPPLER_DEV_TASK_STATE_ERROR;
                    SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_REQACTPOS_FCT_CALL_FAILED, (quint16) m_taskState);

                    FILE_LOG_L(laDEV, llERROR) << "  CGrapplerDevice: failed to start task.";
                    return;
                }
            }
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(m_DeviceTask[GRAPPLER_DEV_TASK_POS_REQ].m_FctModTaskMap,
                                                                                             bActionStateInProgress,
                                                                                             bActionStateWithErrorDetect);
        }

        if(bActionStateWithErrorDetect == true)
        {
            FILE_LOG_L(laDEV, llERROR) << "  CGrapplerDevice: Action state error.";
            m_taskState = GRAPPLER_DEV_TASK_STATE_ERROR;
        }
        else if(bActionStateInProgress == false)
        {
            FILE_LOG_L(laDEV, llINFO) << "  CGrapplerDevice: positions received, next start movement..";

            //reset the tasks to the 'unused' state
            m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[GRAPPLER_DEV_TASK_POS_REQ].m_FctModTaskMap);
            m_DeviceTask[GRAPPLER_DEV_TASK_POS_REQ].m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;

            m_taskState = GRAPPLER_DEV_TASK_STATE_ACTPOS_REQ_ACKN;
        }
    }
    else if(m_taskState == GRAPPLER_DEV_TASK_STATE_ACTPOS_REQ_ACKN)
    {
        // The motor positions have been received, calculate the task to execute the requested station action.
        ReturnCode_t result;
        bool TimeRequest = false;

        if(m_taskID == GRAPPLER_DEV_TASKID_ACTION_TIME_REQ)
        {
            TimeRequest = true;
        }
        // calculate the tasks to execute the requested action
        result = m_pGrapplerNavigator->CalculateGrapplerActionTasks(this, m_DeviceTask[GRAPPLER_DEV_TASK_GRAB].m_FctModTaskMap,
                                                                    m_nextStationColumn,
                                                                    m_nextStationRow,
                                                                    m_ActStationAction,
                                                                    TimeRequest);

        if(result == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_taskState = GRAPPLER_DEV_TASK_STATE_PROGRESS_ACTION_STEPS;
            m_DeviceTask[GRAPPLER_DEV_TASK_GRAB].m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
        }
        else
        {
            m_lastErrorHdlInfo = result;
            m_taskState = GRAPPLER_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_CALCSTEPS_FCT_CALL_FAILED, (quint16) m_taskID);

            FILE_LOG_L(laDEV, llERROR) << "   CGrapplerDevice: error while CalculateGrapplerActionTasks: " << (int) result;
        }
    }
    else if(m_taskState == GRAPPLER_DEV_TASK_STATE_PROGRESS_ACTION_STEPS)
    {
        // execute the function module tasks stored in the device task list
        FunctionModuleTask* pFunctionModuleTask = 0;
        bool bActionStateInProgress = false;
        bool bActionStateWithErrorDetect = false;

        // check for function module task pending to start
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(m_DeviceTask[GRAPPLER_DEV_TASK_GRAB].m_FctModTaskMap,
                                                                                         bActionStateInProgress,
                                                                                         bActionStateWithErrorDetect);
        if(pFunctionModuleTask)
        {
            ReturnCode_t RetVal;

            FILE_LOG_L(laDEV, llINFO) << "   task ready to start found.";
            RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_lastErrorHdlInfo = RetVal;
                m_taskState = GRAPPLER_DEV_TASK_STATE_ERROR;
                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_REQACTPOS_FCT_CALL_FAILED, (quint16) m_taskState);

                FILE_LOG_L(laDEV, llERROR) << "  CGrapplerDevice: failed to start task.";
                return;
            }
        }

        if(bActionStateWithErrorDetect == true)
        {
            FILE_LOG_L(laDEV, llINFO) << "   CGrapplerDevice: an error occured";
            m_taskState = GRAPPLER_DEV_TASK_STATE_ERROR;
        }
        else if(bActionStateInProgress == false)
        {
            //the request was finished, reset the tasks to 'unused' state
            m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[GRAPPLER_DEV_TASK_GRAB].m_FctModTaskMap);
            FILE_LOG_L(laDEV, llINFO) << "   CGrapplerDevice: we are finished, set all tasks to state 'unused'";

            //reset internal state
            ResetTask();

            //depending on the requested action, the result will be forwarded
            if(m_ActStationAction == STATION_ACTION_READ_RFID)
            {
                //m_pIGrapplerDev->RouteRFIDData(m_instanceID, m_lastErrorHdlInfo, m_actRfidDataMSB, m_actRfidDataLSB);
            }
            else if(m_ActStationAction == STATION_ACTION_READ_LIQUID_LEVEL)
            {
                //m_pIGrapplerDev->RouteLiquidLevelValue(m_instanceID, m_lastErrorHdlInfo, m_actLiquidLevelValue);
            }
            else  /// \todo complete the else...
            {
                if(m_ActStationAction == STATION_ACTION_LET_DOWN_RACK)
                {
                    m_TransportState = TRANSP_STATE_ATTACHED;
                }
                else if(m_ActStationAction == STATION_ACTION_PULL_UP_RACK)
                {
                    m_TransportState = TRANSP_STATE_RACK;
                }
                else if(m_ActStationAction == STATION_ACTION_ATTACH_TO_RACK)
                {
                    m_TransportState = TRANSP_STATE_ATTACHED;
                }
                else if(m_ActStationAction == STATION_ACTION_DETACH_RACK)
                {
                    m_TransportState = TRANSP_STATE_EMPTY;
                }
                //m_pIGrapplerDev->RoutePositioningAckn(m_instanceID, m_lastErrorHdlInfo);
            }
        }
    }
    else if(m_taskState == GRAPPLER_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << "   CGrapplerDevice: error GRAPPLER_DEV_TASKID_ACTION_REQ ";

        //depending on the requested action, the result will be forwarded
        if(m_ActStationAction == STATION_ACTION_READ_RFID)
        {
            //m_pIGrapplerDev->RouteRFIDData(m_instanceID, m_lastErrorHdlInfo, 0, 0);
        }
        else if(m_ActStationAction == STATION_ACTION_READ_LIQUID_LEVEL)
        {
            //m_pIGrapplerDev->RouteLiquidLevelValue(m_instanceID, m_lastErrorHdlInfo, 0);
        }
        else  /// \todo complete the else...
        {
            //m_pIGrapplerDev->RoutePositioningAckn(m_instanceID, m_lastErrorHdlInfo);
        }

        ResetTask();

        // its a grappler error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "   CGrapplerDevice: invalid state: " << (int) m_taskState;
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = GRAPPLER_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, ERROR_DCL_GRAPPLER_DEV_INVALID_STATE, (quint16) m_taskState);
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
 *     GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     GRAPPLER_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     GRAPPLER_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state
 *
 *     GRAPPLER_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 */
/****************************************************************************/
void CGrapplerDevice::HandleErrorState()
{
    if(m_errTaskState == GRAPPLER_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_errTaskState == GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIGrapplerDev != 0)
        {
            m_pIGrapplerDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }*/
        m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_errTaskState == GRAPPLER_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_errTaskState == GRAPPLER_DEV_ERRTASK_STATE_RESET)
    {
        ResetTask();
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;

        // reset the function module references
        m_pMotorXAxis = 0;
        m_pMotorYAxis = 0;
        m_pMotorZAxis = 0;
        m_pRFID = 0;
        m_pLiquidLevel = 0;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motors 'set state' acknowledge
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *                      otherwise the return value of the error detecting function
 *
 ****************************************************************************/
void CGrapplerDevice::StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
{
    FILE_LOG_L(laDEV, llINFO) << " CGrapplerDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

    CANStepperMotorTask* pMotorTask = 0;

    //get the first Motor task from the device task map, with state 'PROGRESS', and taskID 'STATE'
    pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    while(pMotorTask)
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pMotorTask->m_TaskIndex << " state to finished.";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
        }
        else
        {
            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while SetState.. error";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
        }

        //get the next motor task from the device task map, with state 'PROGRESS', and taskID 'STATE', if any
        pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    }
}

/****************************************************************************/
/*!
 *  \brief    Return function module tasks from device task currently in process
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   pMotorTaskPrev = DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *                      otherwise the return value of the error detecting function
 *  \iparam   FctTaskID = motor task id
 *
 *  \return   Function module task currently in process and matching the FctTaskID
 *
 ****************************************************************************/
CANStepperMotorTask* CGrapplerDevice::GetMotorFctModTaskInProcess(quint32 InstanceID,
                                                                  CANStepperMotorTask* pMotorTaskPrev,
                                                                  CANStepperMotorTask::CANStepperMotorTaskID_t FctTaskID)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llINFO) << " CGrapplerDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

    CANStepperMotorTask* pMotorTask = 0;
    CANStepperMotorTask* pMotorTaskRet = 0;

    for(quint8 idx = 0; idx < MAX_GRAPPLER_DEVICE_TASK_ID; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pMotorTaskPrev);
            while(pFunctionModuleTask)
            {
                FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState;
                Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);
                pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                if(pMotorTask)
                {
                    if((pMotorTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS) &&
                       (pMotorTask->m_MotorTaskID == FctTaskID))
                    {
                        pMotorTaskRet = pMotorTask;
                        break;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pMotorTask);
            }
        }
    }
    return pMotorTaskRet;

}

/****************************************************************************/
/*!
 *  \brief    This slot will be called from CANStepperMoter if a reference run was acknowledged
 *
 *
 *  \iparam   InstanceID = instance identifier of the CANStepperMotor
 *  \iparam   HdlInfo = return code
 *  \iparam   Position = The actual motor position
 *
 */
/****************************************************************************/
void CGrapplerDevice::StepperMotorReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
{
    Q_UNUSED(Position);
    FILE_LOG_L(laDEV, llINFO) << " CGrapplerDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

    CANStepperMotorTask* pMotorTask = 0;

    //get the first Motor task from the device task map, with state 'PROGRESS', and taskID 'STATE'
    pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    while(pMotorTask)
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pMotorTask->m_TaskIndex << " state to finished.";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
        }
        else
        {
            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while Reference run... error";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
        }
        pMotorTask->m_ActPos = (Position_t) Position;

        //get the next motor task from the device task map, with state 'PROGRESS', and taskID 'STATE', if any
        pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot will be called from CANStepperMotor if a movement was acknowledged
 *
 *
 *  \iparam   InstanceID = instance identifier of the CANStepperMotor
 *  \iparam   HdlInfo = return code
 *  \iparam   Position = Actual motor position
 *  \iparam   Speed = Actual motor speed
 *
 */
/****************************************************************************/
void CGrapplerDevice::StepperMotorMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position, qint16 Speed)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    Q_UNUSED(Speed);
    Q_UNUSED(Position);

    FILE_LOG_L(laDEV, llINFO) << "  CGrapplerDevice<" << (int) m_instanceID << ">StepperMotorMovementAckn 0x" << std::hex << InstanceID;

    //pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_taskList, instanceID, pFunctionModuleTask);
    for(quint8 idx = 0; idx < MAX_GRAPPLER_DEVICE_TASK_ID; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask)
                {
                    FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState;
                    Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);

                    if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
                    {
                        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
                        {
                            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
                            pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
                        }
                        else
                        {
                            // error was detected from motor side
                            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while driveToPosition.. error";
                            pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                        }

                        //break;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot receives the actual position from CANStepperMotor
 *
 *
 *  \iparam   InstanceID = instance identifier of the CANStepperMotor
 *  \iparam   HdlInfo = return code
 *  \iparam   Position = The actual motor position
 *
 */
/****************************************************************************/
void CGrapplerDevice::StepperMotorPosition(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    Q_UNUSED(HdlInfo);
    Q_UNUSED(Position);

    FILE_LOG_L(laDEV, llINFO) << "  CGrapplerDevice<" << (int) m_instanceID << ">::StepperMotorPosition 0x" << std::hex << InstanceID;
    //if(m_taskList.size() > 0)
    {
        //pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_taskList, instanceID, pFunctionModuleTask);
        for(quint8 idx = 0; idx < MAX_GRAPPLER_DEVICE_TASK_ID; idx++)
        {
            if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
            {
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
                break;
            }
        }

        if(pFunctionModuleTask)
        {
            if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
            {
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                    if((pMotorTask) && (pMotorTask->m_MotorTaskID == CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS))
                    {
                        FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished  (Motor).";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
                    }
                }
            }
        }
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
 *  \iparam   errorGroup = EVENT_GRP_DCL_FCTMOD_MOTOR_SLV
 *  \iparam   errorCode  = from CAN-message
 *  \iparam   errorData  = from CAN-message
 *  \iparam   errorTime  = error message reception time on master side
 *
 *  \return  void
 *
 ****************************************************************************/
void CGrapplerDevice::StepperMotorError(quint32 InstanceID, quint16 errorGroup, quint16 errorCode, quint16 errorData, QDateTime errorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(errorGroup, errorCode, errorData);
    m_lastErrorTime = errorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Handles the RFID data answer signal
 *
 *            The received data are stored at the field of the corresponding RFID-variables.
 *            The acknowledge to the interface class is send if all pending channels haver received
 *            the data. The acknwoledge is done in the task handling function
 *
 *  \iparam   instanceID = instance identifier of the function module
 *  \iparam   hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 *  \iparam   RFIDData = RFID data
 *
 *  \return  void
 *
 ****************************************************************************/
void CGrapplerDevice::RFIDDataReceived(quint32 instanceID, ReturnCode_t hdlInfo, quint8 Address, quint32 RFIDData)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llINFO) << "  CGrapplerDevice<" << (int) m_instanceID << ">::RFIDDataReceived 0x" << std::hex << instanceID;
    //if((m_pDevProc != 0) && (m_taskList.size() > 0))
    {
        //pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_taskList, instanceID, pFunctionModuleTask);
        for(quint8 idx = 0; idx < MAX_GRAPPLER_DEVICE_TASK_ID; idx++)
        {
            if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
            {
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, instanceID, pFunctionModuleTask);
                break;
            }
        }

        if((pFunctionModuleTask) && (pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_RFID))
        {
            if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
            {
                FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished (RFID).";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
            }
        }
    }
    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        m_actRfidDataMSB = RFIDData;
        m_actRfidDataLSB = 0;
    }
}

/****************************************************************************/
/*!
*  \brief    This slot receives the RFID modul error signal
*
*  \iparam   InstanceID = instance identifier of the device
*  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_RFID_SLV
*  \iparam   ErrorCode  = from CAN-message
*  \iparam   ErrorData  = from CAN-message
*  \iparam   ErrorTime  = error message reception time on master side
*
****************************************************************************/
void CGrapplerDevice::RFIDError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief   This slot receives the liquid sensor value
 *
 *           This function will be called from CANAnalogInput and forwards the
 *           analog value.
 *           The function is called after the analog value was requested,
 *           and this command was respond by the slave's function module.
 *
 *  \iparam   instanceID = instance identifier of the device
 *  \iparam   hdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 *  \iparam   inputValue = analog value
 *
 *
 ****************************************************************************/
void CGrapplerDevice::AnalogInputChanged(quint32 instanceID, ReturnCode_t hdlInfo, quint16 inputValue)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llINFO) << "  CGrapplerDevice<" << (int) m_instanceID << ">::AnalogInputChanged 0x" << std::hex << instanceID;
    //if((m_pDevProc != 0) && (m_taskList.size() > 0))
    {
        //pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_taskList, instanceID, pFunctionModuleTask);
        for(quint8 idx = 0; idx < MAX_GRAPPLER_DEVICE_TASK_ID; idx++)
        {
            if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
            {
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, instanceID, pFunctionModuleTask);
                break;
            }
        }

        if(pFunctionModuleTask)
        {
            FILE_LOG_L(laDEV, llINFO) << "  AnalogInputChanged.. " << (int) pFunctionModuleTask->m_TaskID << " state: " << (int) pFunctionModuleTask->m_TaskState;
        }
        if((pFunctionModuleTask) && (pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT))
        {
            if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
            {
                FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished (Liquid).";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
            }
        }
    }
    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        m_actLiquidLevelValue = inputValue;
    }
}

/****************************************************************************/
/*!
 *  \brief   This slot receives the current sensors error signals
 *
 *           This function will be called from CANAnalogInput and forwards the
 *           error information.
 *           The function is called if an error was detected by the slave's function module.
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   errorGroup = EVENT_GRP_DCL_FCTMOD_AINP_SLV or EVENT_GRP_DCL_FCTMOD_AINP_DCL
 *  \iparam   errorCode  = from CAN-message
 *  \iparam   errorData  = from CAN-message
 *  \iparam   errorTime  = error message reception time on master side
 *
 *  \return  void
 *
 ****************************************************************************/
void CGrapplerDevice::AnalogInputError(quint32 InstanceID, quint16 errorGroup, quint16 errorCode, quint16 errorData, QDateTime errorTime)
{
    Q_UNUSED(InstanceID);

    SetErrorParameter(errorGroup, errorCode, errorData);
    m_lastErrorTime = errorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_errTaskState = GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE;
}


void CGrapplerDevice::GetMovementInfo()
{
    ResetTask();
}


/****************************************************************************/
/*!
 *  \brief    Request the grapplers reference run
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted, otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CGrapplerDevice::ReqReferenceRun()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if((m_taskID == GRAPPLER_DEV_TASKID_FREE) && (m_taskState == GRAPPLER_DEV_TASK_STATE_FREE))
    {
        m_taskID = GRAPPLER_DEV_TASKID_REFRUN;
        m_taskState = GRAPPLER_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "    CGrapplerDevice::ReqReferenceRun ";
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "   CGrapplerDevice::ReqReferenceRun failed, taskID:" << (int) m_taskID <<" taskState:" << (int) m_taskState;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}


/****************************************************************************/
/*!
 *  \brief    Request a grappler action
 *
 *  An action can be one of:
 *   - position with grab or discard a rack
 *   - positioning with reading the RFID data
 *   - positioning with reading the liquid level at the station
 *
 *  \iparam   StationColumn = column of the target station
 *  \iparam   StationRow = row of the target station
 *  \iparam   StationAktion = the action, which should be executed at the station
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted, otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CGrapplerDevice::ReqStationAction(qint16 StationColumn, qint16 StationRow, StationAction_t StationAktion)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if(m_taskID == GRAPPLER_DEV_TASKID_FREE)
    {
        m_nextStationColumn = StationColumn;
        m_nextStationRow    = StationRow;

        //convert the old defines to the new actions
       /* if(stationAktion == STATION_ACTION_GRAB)
        {
            stationAktion = STATION_ACTION_MOVE_EMTPY_TO;
        }
        else if((stationAktion == STATION_ACTION_DISCARD) ||
                (stationAktion == STATION_ACTION_MOVEOVER))
        {
            stationAktion = STATION_ACTION_TRANSP_RACK_TO;
        }*/  // end of old define handling

        m_ActStationAction = StationAktion;
        /*if((m_TransportState == TRANSP_STATE_RACK) &&
           (stationAktion == STATION_ACTION_MOVE_EMTPY_TO) || (stationAktion == STATION_ACTION_DETACH_RACK))
        {
            //its not allowed to call a 'empty move' action when a rack is attached
            retval = DCL_ERR_INVALID_STATE;
        }
        else*/
        {
            m_taskID = GRAPPLER_DEV_TASKID_ACTION_REQ;
            m_taskState = GRAPPLER_DEV_TASK_STATE_REQ;
            FILE_LOG_L(laDEV, llINFO) << "   CGrapplerDevice::ReqStationAction: (col/row)" << m_nextStationColumn << ", " << m_nextStationRow <<
                    ", action:" << (int) StationAktion;
        }
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "   CGrapplerDevice::ReqStationAction failed, taskID:" << (int) m_taskID <<" taskState:" << (int) m_taskState;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Request the time consumption of a grappler action
 *
 *  An action can be one of:
 *   - position with grab or discard a rack
 *   - positioning with reading the RFID data
 *   - positioning with reading the liquid level at the station
 *
 *  \iparam   StationColumn = column of the target station
 *  \iparam   StationRow = row of the target station
 *  \iparam   StationAktion = the action, which should be executed at the station
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted, otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CGrapplerDevice::ReqStationActionTime(qint16 StationColumn, qint16 StationRow, StationAction_t StationAktion)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if(m_taskID == GRAPPLER_DEV_TASKID_FREE)
    {
        m_nextStationColumn = StationColumn;
        m_nextStationRow    = StationRow;
        m_ActStationAction = StationAktion;
        m_taskID = GRAPPLER_DEV_TASKID_ACTION_TIME_REQ;
        m_taskState = GRAPPLER_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "   CGrapplerDevice::ReqStationActionTime: (col/row)" << m_nextStationColumn << ", " << m_nextStationRow <<
                ", action:" << (int) m_ActStationAction;
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "   CGrapplerDevice::ReqStationAction failed, taskID:" << (int) m_taskID <<" taskState:" << (int) m_taskState;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Reset the task state machine
 */
/****************************************************************************/
void CGrapplerDevice::ResetTask()
{
    m_taskID = GRAPPLER_DEV_TASKID_FREE;
    m_taskState = GRAPPLER_DEV_TASK_STATE_FREE;
}

/****************************************************************************/
/*!
 *  \brief  Read the grappler's station layout data from file
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CGrapplerDevice::ReadStationLayout()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    QString errorStr;
    QString errorMsg;
    int errorLine;
    int errorColumn;

    QDomDocument domDocument;
    QString fileName = Global::SystemPaths::Instance().GetSettingsPath() + "/StationLayout.xml";

    FILE_LOG_L(laCONFIG_FILE, llINFO) <<  "  read station layout data for grappler " << std::hex << (int) m_instanceID <<
                                          " from '" << fileName.toStdString() << "'";

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        errorMsg = QObject::tr(" Cannot read file %1:\n%2.")
                                .arg(fileName)
                                .arg(file.errorString());
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_READ_FAILED, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn))
    {
        errorMsg = QObject::tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr);
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_PARSE_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "colorado_station_layout")
    {
        errorMsg = QObject::tr("The file is not a colorado station layout file. Tag 'station_layout' missed!");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();
        FILE_LOG_L(laCONFIG_FILE, llERROR) << "   '" << root.tagName().toStdString() << "'";

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_DOCTYPE_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else if (root.hasAttribute("version")
              && root.attribute("version") != "1.0")
    {
        errorMsg = QObject::tr("The colorado station layout file version is not valid.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_VERSION_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //##############################
    // station layout data
    //##############################
    QDomElement childStationLayout = root.firstChildElement("station_layout");
    if(childStationLayout.isNull())
    {
        errorMsg = QObject::tr("  child 'station_layout' not found.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_FORMAT_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        QDomNode nodeRow;
        QDomNode nodeRowIndex, nodePosIndex, nodeRowAdjust, nodeColumnAdjust;
        QDomNamedNodeMap attrmapNodeRow;
        QDomNamedNodeMap attrmapNodeVessel;
        QString strRowIndex, strPosIndex, strRowAdjust, strColumnAdjust;
        int posXAdjust, posYAdjust;
        int rowCount = 0, vesselCount = 0;
        short rowIndex, posIndex;;
        bool ok;


        nodeRow = childStationLayout.firstChildElement("row");
        FILE_LOG_L(laCONFIG_FILE, llINFO) <<  "row found.";
        while (!nodeRow.isNull())
        {
            rowCount++;

            FILE_LOG_L(laDEV, llINFO) << " open row.";
            attrmapNodeRow = nodeRow.attributes();
            nodeRowIndex = attrmapNodeRow.namedItem("index");
            strRowIndex = nodeRowIndex.nodeValue();
            rowIndex = strRowIndex.toShort(&ok, 10);
            rowIndex -= 1;

            QDomNode nodeVessel = nodeRow.firstChildElement("vessel");
            FILE_LOG_L(laCONFIG_FILE, llINFO) <<  "node vessel found.";
            while (!nodeVessel.isNull())
            {
                vesselCount++;
                attrmapNodeVessel = nodeVessel.attributes();

                nodePosIndex = attrmapNodeVessel.namedItem("pos");
                strPosIndex = nodePosIndex.nodeValue();
                posIndex = strPosIndex.toShort(&ok, 10);
                posIndex -= 1;

                nodeRowAdjust = attrmapNodeVessel.namedItem("row");
                strRowAdjust = nodeRowAdjust.nodeValue();
                posYAdjust = strRowAdjust.toInt(&ok, 10);

                nodeColumnAdjust = attrmapNodeVessel.namedItem("column");
                strColumnAdjust = nodeColumnAdjust.nodeValue();
                posXAdjust = strColumnAdjust.toInt(&ok, 10);

                m_stationPosition[rowIndex][posIndex].positionX = posXAdjust;
                m_stationPosition[rowIndex][posIndex].positionY = posYAdjust;

                FILE_LOG_L(laDEV, llINFO) << " at pos " << rowIndex << ", " << posIndex << ": row/column data: " <<
                        m_stationPosition[rowIndex][posIndex].positionX << " / " << m_stationPosition[rowIndex][posIndex].positionY;

                nodeVessel = nodeVessel.nextSiblingElement("vessel");
            }

            nodeRow = nodeRow.nextSiblingElement("row");
        }

        if((rowCount != 4) || (vesselCount != 4*18))
        {
            errorMsg = QObject::tr("  invalid format, row- or vessel count invalid.");
            FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

            SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_FORMAT_ERROR, vesselCount);
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

            return DCL_ERR_FCT_CALL_FAILED;
        }
    }

    return retCode;

}

/****************************************************************************/
/*!
 *  \brief    Read the grappler's adjustment data from file
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CGrapplerDevice::ReadAdjustment()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    QString errorStr;
    QString errorMsg;
    int errorLine;
    int errorColumn;

    QDomDocument domDocument;
    QString fileName = Global::SystemPaths::Instance().GetSettingsPath() + "/adjustment.xml";

    FILE_LOG_L(laCONFIG_FILE, llINFO) <<  "  read adjustment data for grappler " << std::hex << (int) m_instanceID <<
                                          " from '" << fileName.toStdString() << "'";

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        errorMsg = QObject::tr(" Cannot read file %1:\n%2.")
                                .arg(fileName)
                                .arg(file.errorString());
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_READ_FAILED, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn))
    {
        errorMsg = QObject::tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr);
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_PARSE_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "colorado_adjustment")
    {
        errorMsg = QObject::tr("The file is not a colorado adjustment file. Tag 'colorado_adjustment' missed!");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();
        FILE_LOG_L(laCONFIG_FILE, llERROR) << "   '" << root.tagName().toStdString() << "'";

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_DOCTYPE_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else if (root.hasAttribute("version")
              && root.attribute("version") != "1.0")
    {
        errorMsg = QObject::tr("The colorado adjustment file version is not valid.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_VERSION_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //##############################
    // adjustment_data
    //##############################
    QDomElement child = root.firstChildElement("adjustment_data");
    if(child.isNull())
    {
        errorMsg = QObject::tr("  child 'adjustment_data' not found.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_FORMAT_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        QDomNode nodeGrappler;
        QDomNode nodeRowIndex, nodePosIndex, nodeRowAdjust, nodeColumnAdjust;
        QDomNamedNodeMap attrmapNodeRow;
        QDomNamedNodeMap attrmapNodeVessel;
        QString strRowIndex, strPosIndex, strRowAdjust, strColumnAdjust;
        int posXAdjust, posYAdjust;
        int rowCount = 0, vesselCount = 0;
        short rowIndex, posIndex;;
        bool ok;

        if(m_instanceID == DEVICE_INSTANCE_ID_GRAPPLER_1)
        {
            FILE_LOG_L(laDEV, llDEBUG4) << " open grappler_left";
            nodeGrappler = child.firstChildElement("grappler_left");
        }
        else
        {
            FILE_LOG_L(laDEV, llDEBUG4) << " open grappler_right";
            nodeGrappler = child.firstChildElement("grappler_right");
        }

        if(!nodeGrappler.isNull())
        {
            QDomNode nodeRow = nodeGrappler.firstChildElement("row");
            FILE_LOG_L(laCONFIG_FILE, llDEBUG4) <<  "node base_module found.";
            while (!nodeRow.isNull())
            {
                rowCount++;

                FILE_LOG_L(laDEV, llDEBUG4) << " open row.";
                attrmapNodeRow = nodeRow.attributes();
                nodeRowIndex = attrmapNodeRow.namedItem("index");
                strRowIndex = nodeRowIndex.nodeValue();
                rowIndex = strRowIndex.toShort(&ok, 10);
                rowIndex -= 1;

                QDomNode nodeVessel = nodeRow.firstChildElement("vessel");
                FILE_LOG_L(laCONFIG_FILE, llDEBUG4) <<  "node vessel found.";
                while (!nodeVessel.isNull())
                {
                    vesselCount++;
                    attrmapNodeVessel = nodeVessel.attributes();

                    nodePosIndex = attrmapNodeVessel.namedItem("pos");
                    strPosIndex = nodePosIndex.nodeValue();
                    posIndex = strPosIndex.toShort(&ok, 10);
                    posIndex -= 1;

                    nodeRowAdjust = attrmapNodeVessel.namedItem("row");
                    strRowAdjust = nodeRowAdjust.nodeValue();
                    posYAdjust = strRowAdjust.toInt(&ok, 10);

                    nodeColumnAdjust = attrmapNodeVessel.namedItem("column");
                    strColumnAdjust = nodeColumnAdjust.nodeValue();
                    posXAdjust = strColumnAdjust.toInt(&ok, 10);

                    m_stationAdjust[rowIndex][posIndex].positionX = posXAdjust;
                    m_stationAdjust[rowIndex][posIndex].positionY = posYAdjust;

                    FILE_LOG_L(laDEV, llDEBUG4) << " at pos " << rowIndex << ", " << posIndex << ": row/column data: " <<
                                     m_stationAdjust[rowIndex][posIndex].positionX << " / " << m_stationAdjust[rowIndex][posIndex].positionY;

                    nodeVessel = nodeVessel.nextSiblingElement("vessel");
                }

                nodeRow = nodeRow.nextSiblingElement("row");
            }

            if((rowCount != 4) || (vesselCount != 4*18))
            {
                errorMsg = QObject::tr("  invalid format, row- or vessel count invalid.");
                FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_FORMAT_ERROR, vesselCount);
                m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

                return DCL_ERR_FCT_CALL_FAILED;
            }
        }
        else
        {
            errorMsg = QObject::tr("  child 'grappler_' not found.");
            FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

            SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_FORMAT_ERROR, 2);
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

            return DCL_ERR_FCT_CALL_FAILED;
        }
    }

    /*FILE_LOG_L(laDEV, llINFO) << "-------------------------------------------------";
    for(int indexRow = 0; indexRow < 4; indexRow++)
    {
        for(int indexColumn = 0; indexColumn < 18; indexColumn++)
        {
            FILE_LOG_L(laDEV, llINFO) << " at pos " << indexRow << ", " << indexColumn <<
                    ": row/column data: " << m_stationAdjust[indexRow][indexColumn].positionX << " / " << m_stationAdjust[indexRow][indexColumn].positionY;
        }
    }*/

    return retCode;

}

/****************************************************************************/
/*!
 *  \brief    Returns the Z-position for the specified action
 *
 *   Depending on the action, the z-axis must positioned to an action specific height
 *
 *  \xparam   PosZ = The z-position
 *  \iparam   StationAction = specified action
 *
 */
/****************************************************************************/
void CGrapplerDevice::GetZPositionFromAction(Position_t& PosZ, StationAction_t StationAction)
{
    if(StationAction == STATION_ACTION_MOVE_EMTPY_TO)
    {
        PosZ = GetProcSettingPosition("z_axis_pos_transport_unload");
    }
    else if(StationAction == STATION_ACTION_TRANSP_RACK_TO)
    {
        PosZ = GetProcSettingPosition("z_axis_pos_transport_load");
    }
    else if(StationAction == STATION_ACTION_ATTACH_TO_RACK)
    {
        PosZ = GetProcSettingPosition("z_axis_pos_cuvette");
    }
    else if(StationAction == STATION_ACTION_DETACH_RACK)
    {
        PosZ = m_PositionZAxisTransportUnload;//GetProcSettingPosition("z_axis_pos_transport_unload");
    }
    else if(StationAction == STATION_ACTION_LET_DOWN_RACK)
    {
        PosZ = GetProcSettingPosition("z_axis_pos_cuvette");
    }
    else if(StationAction == STATION_ACTION_PULL_UP_RACK)
    {
        PosZ = GetProcSettingPosition("z_axis_pos_transport_load");
    }
    else
    {
        //PfuiPfui!
        PosZ = 200;
        FILE_LOG_L(laDEV, llERROR) << " invalid StationAction: " << (int) StationAction;
    }
}

/****************************************************************************/
/*!
 *  \brief    Returns the x-y-position for the specified station
 *
 *   The station is specified via row and column. The function returns the x-y position used by the axis.
 *   The positions will be returned in motor half stepps
 *   The positions are calculated by adding the stations base position and its adjustment datas
 *
 *  \iparam   stationCol = the column of the target station
 *  \iparam   stationRow = the column of the target station
 *  \iparam   stationAction = Action at the target station
 *  \oparam   posX = the x-axis motor position in half steps
 *  \oparam   posY = the y-axis motor position in half steps
 *
 */
/****************************************************************************/
void CGrapplerDevice::GetStationXYPositionFromRowCol(qint16 stationCol, qint16 stationRow, StationAction_t stationAction, Position_t &posX, Position_t &posY)
{
    if((stationRow < 0) || (stationRow > 3) ||
       (stationCol < 0) || (stationCol > 17))
    {
        FILE_LOG_L(laDEV, llERROR) << "   Invlalid station col/row: " << stationCol << ", " << stationRow;
        return;
    }

    posX = m_stationPosition[stationRow][stationCol].positionX;
    posY = m_stationPosition[stationRow][stationCol].positionY;
    posX += m_stationAdjust[stationRow][stationCol].positionX;
    posY += m_stationAdjust[stationRow][stationCol].positionY;
    if(stationAction == STATION_ACTION_TRANSP_RACK_TO)
    {
        posY += GetProcSettingPosition("y_axis_offset_rack_attached");
    }
    else if(stationAction == STATION_ACTION_MOVE_EMTPY_TO)
    {
        posY += GetProcSettingPosition("y_axis_offset_rack_detached");
    }
    else if(stationAction == STATION_ACTION_ATTACH_TO_RACK)
    {
        posY += GetProcSettingPosition("y_axis_offset_rack_attached");
    }
    else if(stationAction == STATION_ACTION_DETACH_RACK)
    {
        posY += GetProcSettingPosition("y_axis_offset_rack_detached");
    }
    else if(stationAction == STATION_ACTION_LET_DOWN_RACK)
    {
        posY += GetProcSettingPosition("y_axis_offset_rack_attached");
    }
    else if(stationAction == STATION_ACTION_PULL_UP_RACK)
    {
        posY += GetProcSettingPosition("y_axis_offset_rack_attached");
    }

    FILE_LOG_L(laDEV, llINFO) << " Grappler: (col/row)" << stationCol << ", " << stationRow << " -> pos: " << posX << ", " << posY;
}

} //namespace

