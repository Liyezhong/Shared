/****************************************************************************/
/*! \file DeviceCommandProcessorThreadController.cpp
 *
 *  \brief Implementation file for class DeviceCommandProcessorThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-22
 *  $Author:    $ Norbert Wiedmann
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
/****************************************************************************/
#include <QStringList>
#include <QDebug>
#include <DeviceCommandProcessor/Include/DeviceCommandProcessorThreadController.h>
#include <DeviceCommandProcessor/Include/DeviceCommandProcessorEventCodes.h>

#include <Global/Include/Commands/AckOKNOK.h>
#include <DeviceCommandProcessor/Include/Commands/CmdDeviceProcessingInit.h>
#include <DeviceCommandProcessor/Include/Commands/CmdDeviceProcessingCleanup.h>
#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerAttachToRack.h>
#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerDetachFromRack.h>
#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerLetDownRack.h>
#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerMoveEmptyTo.h>
#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerPullUpRack.h>
#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerReferenceRun.h>
#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerTransportRackTo.h>
#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerReadRFID.h>
#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerReadLiquidLevel.h>
#include <DeviceCommandProcessor/Include/Commands/CmdLoaderReferenceRun.h>
#include <DeviceCommandProcessor/Include/Commands/CmdLoaderSetBlockingState.h>
#include <DeviceCommandProcessor/Include/Commands/CmdLoaderStateChanged.h>
#include <DeviceCommandProcessor/Include/Commands/CmdLoaderRacksChanged.h>
#include <DeviceCommandProcessor/Include/Commands/CmdAgitationReferenceRun.h>
#include <DeviceCommandProcessor/Include/Commands/CmdAgitationSetSpeed.h>
#include <DeviceCommandProcessor/Include/Commands/CmdOvenCoverReferenceRun.h>
#include <DeviceCommandProcessor/Include/Commands/CmdHeatStation.h>
#include <DeviceCommandProcessor/Include/Commands/CmdSetOvenCoverPostion.h>
#include <DeviceCommandProcessor/Include/Commands/CmdSetWaterLevel.h>
#include <DeviceCommandProcessor/Include/Commands/CmdCountSlides.h>
#include <DeviceCommandProcessor/Include/Commands/CmdEstimatedTime.h>
#include "DeviceCommandProcessor/Include/Commands/CmdLoaderUpdate.h"

#include "DeviceControl/Include/Devices/AgitationDevice.h"
#include "DeviceControl/Include/Devices/GrapplerDevice.h"
#include "DeviceControl/Include/Devices/HoodSensorDevice.h"
#include "DeviceControl/Include/Devices/LoaderDevice.h"
#include "DeviceControl/Include/Devices/OvenDevice.h"

namespace DeviceCommandProcessor {

/****************************************************************************/
DeviceCommandProcessorThreadController::DeviceCommandProcessorThreadController(Global::gSourceType TheLoggingSource) :
    Threads::ThreadController(TheLoggingSource, "DeviceCommand"),
    m_RefInitDCL(Global::RefManager<Global::tRefType>::INVALID),
    m_InLoaderChangedRef(Global::RefManager<Global::tRefType>::INVALID),
    m_InUnloaderChangedRef(Global::RefManager<Global::tRefType>::INVALID)
{
    for(quint8 Idx = 0; Idx < CMD_REF_CNT; Idx++)
    {
        m_CommandRef[Idx].m_State = DCP_TC_CMD_STATE_FREE;
    }
}

/****************************************************************************/
DeviceCommandProcessorThreadController::~DeviceCommandProcessorThreadController()
{
    try {
        DestroyObjects();
    } catch (...) {
        // to please lint
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::CreateAndInitializeObjects()
{
    qDebug() << "DeviceCommandProcessorThreadController::CreateAndInitializeObjects";

    try
    {
        // register OnDataChanged processing function

        // now register some processing functions:

        // init and cleanup
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdDeviceProcessingInit, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnDeviceProcessingInit, this);
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdDeviceProcessingCleanup, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnDeviceProcessingCleanup, this);
        //############################
        // grappler commands
        //  reference run
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdGrapplerReferenceRun, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerReferenceRun, this);
        //  move empty to
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdGrapplerMoveEmptyTo, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerMoveEmptyTo, this);
        //  transport rack to
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdGrapplerTransportRackTo, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerTransportRackTo, this);
        //  attach to rack
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdGrapplerAttachToRack, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerAttachToRack, this);
        //  detach to rack
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdGrapplerDetachFromRack, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerDetachFromRack, this);
        //  pull up rack
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdGrapplerPullUpRack, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerPullUpRack, this);
        //  let down rack
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdGrapplerLetDownRack, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerLetDownRack, this);
        //  read RFID data
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdGrapplerReadRFID, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerReadRFID, this);
        //  read liquid level
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdGrapplerReadLiquidLevel, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerReadLiquidLevel, this);
        //  count slides
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdCountSlides, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnGrapplerCountSlides, this);
        //#############################
        // Loader/unloader commands
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdLoaderReferenceRun, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnLoaderReferenceRun, this);
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdLoaderSetBlockingState, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnLoaderSetBlockingState, this);

        //#############################
        // Agitation commands
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdAgitationReferenceRun, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnAgitationReferenceRun, this);
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdAgitationSetSpeed, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnAgitationSetSpeed, this);

        //#############################
        // Water stations
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdSetWaterLevel, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnSetWaterLevel, this);

        //#############################
        // Oven commands
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdOvenCoverReferenceRun, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnOvenCoverReferenceRun, this);
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdSetOvenCoverPostion, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnSetOvenCoverPosition, this);
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdHeatStation, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnHeatStation, this);

        //#############################
        // Hood Sensor Commands
//        RegisterCommandForProcessing<DeviceCommandProcessor::CmdLoaderSetBlockingState, DeviceCommandProcessorThreadController>(
//                &DeviceCommandProcessorThreadController::OnHoodStatusReq, this);

        //#############################
        // Estimated time
        RegisterCommandForProcessing<DeviceCommandProcessor::CmdEstimatedTime, DeviceCommandProcessorThreadController>(
                &DeviceCommandProcessorThreadController::OnEstimatedTime, this);

        // register some acknowledges
        RegisterAcknowledgeForProcessing<Global::AckOKNOK, DeviceCommandProcessorThreadController>(&DeviceCommandProcessorThreadController::OnAckOKNOK, this);
        // everything OK
    }
    catch(...) {
        // destroy objects
        DestroyObjects();
        // rethrow exception
        throw;
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::CleanupAndDestroyObjects()
{
    // destroy objects
    DestroyObjects();
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::DestroyObjects()
{
    try
    {
    }
    catch(...)
    {
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGoReceived() {
    /// \todo implement
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnStopReceived() {
    /// \todo implement
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnPowerFail() {
    /// \todo implement
}

/****************************************************************************/
bool DeviceCommandProcessorThreadController::LocalProcessErrorEvent(const DataLogging::EventEntry &/*TheEventEntry*/) {
    /// \todo implement
    // no processing, just forward. At least now...
    return false;
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnCmdTimeout(Global::tRefType Ref, const QString &CmdName)
{
    /// \todo implement
    return;

    DEBUGWHEREAMI;
    DEBUGMSG(QString("Ref = ") + QString::number(Ref, 10) + " CmdName = " + CmdName);
    Q_UNUSED(Ref);
    Q_UNUSED(CmdName);
}

/****************************************************************************/
quint32 DeviceCommandProcessorThreadController::GrapplerNameByInstanceID(DeviceControl::DevInstanceID_t GrapplerID) const {
    quint32 Result = EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_UNKNOWN;
    /// \todo inconsitency between comment from DeviceControl::DEVICE_INSTANCE_ID_GRAPPLER_1(2) and moved grappler in colopia
    switch(GrapplerID) {
        case DeviceControl::DEVICE_INSTANCE_ID_GRAPPLER_1:  Result = EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_1; break;
        case DeviceControl::DEVICE_INSTANCE_ID_GRAPPLER_2:  Result = EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_2; break;
        default :                                           Result = EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_UNKNOWN;
    }
    return Result;
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnDeviceProcessingInit(Global::tRefType Ref, const CmdDeviceProcessingInit &Cmd)
{
    DEBUGWHEREAMI;
    Q_UNUSED(Cmd);

    m_RefInitDCL = Ref;
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnDeviceProcessingCleanup(Global::tRefType Ref, const CmdDeviceProcessingCleanup &Cmd)
{
    DEBUGWHEREAMI;
    Q_UNUSED(Cmd);

    m_DeviceProcessing.Destroy();
    SendAcknowledgeOK(Ref);
}


void DeviceCommandProcessorThreadController::OnGrapplerAttachToRack(Global::tRefType Ref, const CmdGrapplerAttachToRack &Cmd)
{
    qDebug() << "DeviceCommandProcessorThreadController::OnGrapplerAttachToRack";

    SendAcknowledgeOK(Ref);
}

void DeviceCommandProcessorThreadController::OnGrapplerDetachFromRack(Global::tRefType Ref, const CmdGrapplerDetachFromRack &Cmd)
{
    qDebug() << "DeviceCommandProcessorThreadController::OnGrapplerDetachFromRack";

    SendAcknowledgeOK(Ref);
}

void DeviceCommandProcessorThreadController::OnGrapplerLetDownRack(Global::tRefType Ref, const CmdGrapplerLetDownRack &Cmd)
{
    qDebug() << "DeviceCommandProcessorThreadController::OnGrapplerLetDownRack";
    SendAcknowledgeOK(Ref);
}

void DeviceCommandProcessorThreadController::OnGrapplerMoveEmptyTo(Global::tRefType Ref, const CmdGrapplerMoveEmptyTo &Cmd)
{
    qDebug() << "DeviceCommandProcessorThreadController::OnGrapplerMoveEmptyTo";

    SendAcknowledgeOK(Ref);
}

void DeviceCommandProcessorThreadController::OnGrapplerPullUpRack(Global::tRefType Ref, const CmdGrapplerPullUpRack &Cmd)
{
    qDebug() << "DeviceCommandProcessorThreadController::OnGrapplerPullUpRack";

    SendAcknowledgeOK(Ref);
}

void DeviceCommandProcessorThreadController::OnGrapplerReferenceRun(Global::tRefType Ref, const CmdGrapplerReferenceRun &Cmd)
{
    SendAcknowledgeOK(Ref);
}

void DeviceCommandProcessorThreadController::OnGrapplerTransportRackTo(Global::tRefType Ref, const CmdGrapplerTransportRackTo &Cmd)
{
    qDebug() << "DeviceCommandProcessorThreadController::OnGrapplerTransportRackTo";

    SendAcknowledgeOK(Ref);
}

void DeviceCommandProcessorThreadController::OnGrapplerReadRFID(Global::tRefType Ref, const CmdGrapplerReadRFID &Cmd)
{
    SendAcknowledgeOK(Ref);
}

void DeviceCommandProcessorThreadController::OnGrapplerReadLiquidLevel(Global::tRefType Ref, const CmdGrapplerReadLiquidLevel &Cmd)
{
    SendAcknowledgeOK(Ref);
}

#ifdef TEST_RUN
/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGrapplerReferenceRun(Global::tRefType Ref, const CmdGrapplerReferenceRun &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    quint8 ErrorInfo = 0;
    bool CmdAccepted = false;

    // log start of command
    //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_STARTED,
    //          Global::TranslatableString(EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_REFRUN,
     //                                    Global::tTranslatableStringList() << GrapplerNameByInstanceID(Cmd.m_DevInstanceID)));

    CHECKPTR(m_pDeviceProcessing);
    if(GetFreeCommandCtrlContainer(CmdIdx))
    {
        IGrapplerDevice* pIGrappler = m_pDeviceProcessing->GetIGrapplerDevice(Cmd.m_DevInstanceID);
        if(pIGrappler)
        {
            ReturnCode_t RetVal = pIGrappler->ReqReferenceRun();
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
            }
            else
            {
                ErrorInfo = 3;
            }
        }
        else
        {
            /// \todo log specific error
            ErrorInfo = 2;
        }
    }
    else
    {
        /// \todo log specific error
        ErrorInfo = 1;
    }

    if(CmdAccepted == false)
    {
        // log command failure
        //SEND_FATAL_ERROR(EVENT_DEVICECOMMANDPROCESSOR_ERROR_COMMAND_FAILED,
                        // Global::TranslatableString(EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_REFRUN,
                         //                           Global::tTranslatableStringList() << GrapplerNameByInstanceID(Cmd.m_DevInstanceID)));
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGrapplerMoveEmptyTo(Global::tRefType Ref, const CmdGrapplerMoveEmptyTo &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    bool CmdAccepted = false;

    if((m_pDeviceProcessing) && (GetFreeCommandCtrlContainer(CmdIdx)))
    {
        IGrapplerDevice* pIGrappler = m_pDeviceProcessing->GetIGrapplerDevice(Cmd.m_DevInstanceID);
        if(pIGrappler)
        {
            ReturnCode_t RetVal = pIGrappler->ReqStationAction(DeviceControl::STATION_ACTION_MOVE_EMTPY_TO, Cmd.m_Row, Cmd.m_Column);
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
                QString InfoArgString = QString("OnGrapplerMoveEmptyTo InstanceID  = 0x") + QString::number(Global::AsInt(Ref), 16).toUpper();
                //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_FINISHED, InfoArgString);
            }
        }
    }

    if(CmdAccepted == false)
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGrapplerTransportRackTo(Global::tRefType Ref, const CmdGrapplerTransportRackTo &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    bool CmdAccepted = false;

    if((m_pDeviceProcessing) && (GetFreeCommandCtrlContainer(CmdIdx)))
    {
        IGrapplerDevice* pIGrappler = m_pDeviceProcessing->GetIGrapplerDevice(Cmd.m_DevInstanceID);
        if(pIGrappler)
        {
            ReturnCode_t RetVal = pIGrappler->ReqStationAction(DeviceControl::STATION_ACTION_TRANSP_RACK_TO, Cmd.m_Row, Cmd.m_Column);
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
            }
        }
    }

    if(CmdAccepted == false)
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGrapplerAttachToRack(Global::tRefType Ref, const CmdGrapplerAttachToRack &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    bool CmdAccepted = false;

    if((m_pDeviceProcessing) && (GetFreeCommandCtrlContainer(CmdIdx)))
    {
        IGrapplerDevice* pIGrappler = m_pDeviceProcessing->GetIGrapplerDevice(Cmd.m_DevInstanceID);
        if(pIGrappler)
        {
            ReturnCode_t RetVal = pIGrappler->ReqStationAction(DeviceControl::STATION_ACTION_ATTACH_TO_RACK);
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
            }
        }
    }

    if(CmdAccepted == false)
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGrapplerDetachFromRack(Global::tRefType Ref, const CmdGrapplerDetachFromRack &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    bool CmdAccepted = false;

    if((m_pDeviceProcessing) && (GetFreeCommandCtrlContainer(CmdIdx)))
    {
        IGrapplerDevice* pIGrappler = m_pDeviceProcessing->GetIGrapplerDevice(Cmd.m_DevInstanceID);
        if(pIGrappler)
        {
            ReturnCode_t RetVal = pIGrappler->ReqStationAction(DeviceControl::STATION_ACTION_DETACH_RACK);
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
            }
        }
    }

    if(CmdAccepted == false)
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGrapplerLetDownRack(Global::tRefType Ref, const CmdGrapplerLetDownRack &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    bool CmdAccepted = false;

    if((m_pDeviceProcessing) && (GetFreeCommandCtrlContainer(CmdIdx)))
    {
        IGrapplerDevice* pIGrappler = m_pDeviceProcessing->GetIGrapplerDevice(Cmd.m_DevInstanceID);
        if(pIGrappler)
        {
            ReturnCode_t RetVal = pIGrappler->ReqStationAction(DeviceControl::STATION_ACTION_LET_DOWN_RACK);
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
            }
        }
    }

    if(CmdAccepted == false)
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGrapplerPullUpRack(Global::tRefType Ref, const CmdGrapplerPullUpRack &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    bool CmdAccepted = false;

    if((m_pDeviceProcessing) && (GetFreeCommandCtrlContainer(CmdIdx)))
    {
        IGrapplerDevice* pIGrappler = m_pDeviceProcessing->GetIGrapplerDevice(Cmd.m_DevInstanceID);
        if(pIGrappler)
        {
            ReturnCode_t RetVal = pIGrappler->ReqStationAction(DeviceControl::STATION_ACTION_PULL_UP_RACK);
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
            }
        }
    }

    if(CmdAccepted == false)
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGrapplerReadRFID(Global::tRefType Ref, const CmdGrapplerReadRFID &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    bool CmdAccepted = false;

    if((m_pDeviceProcessing) && (GetFreeCommandCtrlContainer(CmdIdx)))
    {
        IGrapplerDevice* pIGrappler = m_pDeviceProcessing->GetIGrapplerDevice(Cmd.m_DevInstanceID);
        if(pIGrappler)
        {
            ReturnCode_t RetVal = pIGrappler->ReqStationAction(DeviceControl::STATION_ACTION_READ_RFID, Cmd.m_Row, Cmd.m_Column);
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
            }
        }
    }

    if(CmdAccepted == false)
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnGrapplerReadLiquidLevel(Global::tRefType Ref, const CmdGrapplerReadLiquidLevel &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    bool CmdAccepted = false;

    if((m_pDeviceProcessing) && (GetFreeCommandCtrlContainer(CmdIdx)))
    {
        IGrapplerDevice* pIGrappler = m_pDeviceProcessing->GetIGrapplerDevice(Cmd.m_DevInstanceID);
        if(pIGrappler)
        {
            ReturnCode_t RetVal = pIGrappler->ReqStationAction(DeviceControl::STATION_ACTION_READ_LIQUID_LEVEL, Cmd.m_Row, Cmd.m_Column);
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
            }
        }
    }

    if(CmdAccepted == false)
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

#endif

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnLoaderReferenceRun(Global::tRefType Ref, const CmdLoaderReferenceRun &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;
    quint8 ErrorInfo = 0;
    bool CmdAccepted = false;

    QString ErrorArgString = QString("OnLoaderReferenceRun InstanceID  = 0x") + QString::number(Global::AsInt(Cmd.m_DevInstanceID), 16).toUpper();
    //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_ERROR_DEVCTRLLAYER, ErrorArgString);

    if(GetFreeCommandCtrlContainer(CmdIdx))
    {
        CLoaderDevice* pLoader = static_cast<CLoaderDevice*>(m_DeviceProcessing.GetDevice(Cmd.m_DevInstanceID));
        if(pLoader)
        {
            ReturnCode_t RetVal = pLoader->ReferenceRun();
            if(RetVal ==  DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
                CmdAccepted = true;
            }
            else
            {
                ErrorInfo = 3;
            }
        }
        else
        {
            /// \todo log specific error
            ErrorInfo = 2;
        }
    }
    else
    {
        /// \todo log specific error
        ErrorInfo = 1;
    }

    if(CmdAccepted == false)
    {
        // log command failure
        //SEND_FATAL_ERROR(EVENT_DEVICECOMMANDPROCESSOR_ERROR_DEVCTRLLAYER, ErrorArgString);
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

//*************************************************
void DeviceCommandProcessorThreadController::OnLoaderSetBlockingState(Global::tRefType Ref, const CmdLoaderSetBlockingState &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;

    if(GetFreeCommandCtrlContainer(CmdIdx))
    {
        CLoaderDevice* pLoader = static_cast<CLoaderDevice*>(m_DeviceProcessing.GetDevice(Cmd.m_DevInstanceID));
        if(pLoader)
        {
            ReturnCode_t RetVal = pLoader->SetBlockingState(Cmd.m_BlockingState);
            if(RetVal !=  DCL_ERR_FCT_CALL_SUCCESS)
            {
                // send negative acknowledge back.
                SendAcknowledgeNOK(Ref);
            }
            else
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
            }
        }
    }
    else
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

void DeviceCommandProcessorThreadController::OnEstimatedTime(Global::tRefType Ref, const CmdEstimatedTime &Cmd)
{
    qDebug() << "DeviceCommandProcessorThreadController::OnEstimatedTime" << Cmd.GetName();
}

//*************************************************
void DeviceCommandProcessorThreadController::OnAgitationReferenceRun(Global::tRefType Ref, const CmdAgitationReferenceRun &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;

    if(GetFreeCommandCtrlContainer(CmdIdx))
    {
        CAgitationDevice* pAgitationDevice = static_cast<CAgitationDevice*>(m_DeviceProcessing.GetDevice(DEVICE_INSTANCE_ID_AGITATION));
        if(pAgitationDevice)
        {
            ReturnCode_t RetVal = pAgitationDevice->ReqReferenceRun();
            if(RetVal !=  DCL_ERR_FCT_CALL_SUCCESS)
            {
                // send negative acknowledge back.
                SendAcknowledgeNOK(Ref);
            }
            else
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = (DeviceControl::DevInstanceID_t) 0; //Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
            }
        }
    }
    else
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnAgitationSetSpeed(Global::tRefType Ref, const CmdAgitationSetSpeed &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;

    if(GetFreeCommandCtrlContainer(CmdIdx))
    {
        CAgitationDevice* pAgitationDevice = static_cast<CAgitationDevice*>(m_DeviceProcessing.GetDevice(DEVICE_INSTANCE_ID_AGITATION));
        if(pAgitationDevice)
        {
            ReturnCode_t RetVal = pAgitationDevice->DriveSpeed(Cmd.m_AgitationSpeed);
            if(RetVal !=  DCL_ERR_FCT_CALL_SUCCESS)
            {
                // send negative acknowledge back.
                SendAcknowledgeNOK(Ref);
            }
            else
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = (DeviceControl::DevInstanceID_t) 0; //Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
            }
        }
    }
    else
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

void DeviceCommandProcessorThreadController::OnHeatStation(Global::tRefType Ref, const CmdHeatStation &Cmd)
{
    // todo
}

void DeviceCommandProcessorThreadController::OnGrapplerCountSlides(Global::tRefType Ref, const CmdCountSlides &Cmd)
{
    // todo
}

void DeviceCommandProcessorThreadController::OnSetOvenCoverPosition(Global::tRefType Ref, const CmdSetOvenCoverPostion &Cmd)
{
    // todo
}

void DeviceCommandProcessorThreadController::OnSetWaterLevel(Global::tRefType Ref, const CmdSetWaterLevel &Cmd)
{
    // todo
}

//*************************************************
void DeviceCommandProcessorThreadController::OnOvenCoverReferenceRun(Global::tRefType Ref, const CmdOvenCoverReferenceRun &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;

    if(GetFreeCommandCtrlContainer(CmdIdx))
    {
        COvenDevice* pOvenDevice = static_cast<COvenDevice*>(m_DeviceProcessing.GetDevice(DEVICE_INSTANCE_ID_OVEN));
        if(pOvenDevice)
        {
            ReturnCode_t RetVal = pOvenDevice->ReqReferenceRun();
            if(RetVal !=  DCL_ERR_FCT_CALL_SUCCESS)
            {
                // send negative acknowledge back.
                SendAcknowledgeNOK(Ref);
            }
            else
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = (DeviceControl::DevInstanceID_t) 0; //Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
            }
        }
    }
    else
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

//*************************************************
void DeviceCommandProcessorThreadController::OnHoodStatusReq(Global::tRefType Ref, const CmdLoaderSetBlockingState &Cmd)
{
    DEBUGWHEREAMI;

    quint8 CmdIdx = 0;

    if(GetFreeCommandCtrlContainer(CmdIdx))
    {
        CHoodSensorDevice* pHoodDev = static_cast<CHoodSensorDevice*>(m_DeviceProcessing.GetDevice(Cmd.m_DevInstanceID));
        if(pHoodDev)
        {
            ReturnCode_t RetVal = pHoodDev->ReqHoodStatus();

            if(RetVal !=  DCL_ERR_FCT_CALL_SUCCESS)
            {
                // send negative acknowledge back.
                SendAcknowledgeNOK(Ref);
            }
            else
            {
                m_CommandRef[CmdIdx].m_CommandName = Cmd.NAME;
                m_CommandRef[CmdIdx].m_DevInstanceID = Cmd.m_DevInstanceID;
                m_CommandRef[CmdIdx].m_RefType = Ref;
                m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_ACTIV;
            }
        }
    }
    else
    {
        // send negative acknowledge back.
        SendAcknowledgeNOK(Ref);
    }
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    DEBUGWHEREAMI;
    /// \todo implement
    Q_UNUSED(Ref);
    Q_UNUSED(Ack);
}



/****************************************************************************/
/*!
 *  \brief  Error notification from device control layer
 *
 *      All errors detected within the device control layer will be routed
 *      here.
 *
 *  \iparam InstanceID = device instance identifier
 *  \iparam ErrorGroup = Error group
 *  \iparam ErrorCode = Error code
 *  \iparam ErrorData = Additional error information
 *  \iparam ErrorTime = Error time
 */
/****************************************************************************/
void DeviceCommandProcessorThreadController::ThrowError(DevInstanceID_t InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData,const QDateTime & ErrorTime)
{
    QString ErrorArgString = QString("InstanceID = 0x") + QString::number(Global::AsInt(InstanceID), 16).toUpper() + " " +
                             QString("ErrorGroup = 0x") + QString::number(ErrorGroup, 16).toUpper() + " " +
                             QString("ErrorCode = 0x") + QString::number(ErrorCode, 16).toUpper() + " " +
                             QString("ErrorData = 0x") + QString::number(ErrorData, 16).toUpper() + " " +
                             QString("ErrorTime = ") + ErrorTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
    SEND_DEBUG(WHEREAMI + " " + ErrorArgString);

    //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_ERROR_DEVCTRLLAYER, ErrorArgString);
}


/*****************************************************************************/
/**
 *  \brief     Initialisation finished notification
 *
 *  \param     InstanceID  = Device instance ID for grappler identification
 *  \param     InitResult  = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::DevProcInitialisationAckn(DevInstanceID_t InstanceID, ReturnCode_t InitResult)
{
    Q_UNUSED(InstanceID);

    if(InitResult == DCL_ERR_FCT_CALL_SUCCESS)
    {
        // log success
        //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_FINISHED, EVENT_DEVICECOMMANDPROCESSOR_STRING_INITIALIZATION);

        // log start of command
        //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_STARTED, EVENT_DEVICECOMMANDPROCESSOR_STRING_CONFIGURATION);
        ReturnCode_t RetVal = m_DeviceProcessing.StartConfigurationService();

        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
        {
            // log failure
            //SEND_FATAL_ERROR(EVENT_DEVICECOMMANDPROCESSOR_ERROR_COMMAND_FAILED, EVENT_DEVICECOMMANDPROCESSOR_STRING_CONFIGURATION);
            SendAcknowledgeNOK(m_RefInitDCL);
        }
    }
    else
    {
        // log failure
        //SEND_FATAL_ERROR(EVENT_DEVICECOMMANDPROCESSOR_ERROR_COMMAND_FAILED, EVENT_DEVICECOMMANDPROCESSOR_STRING_INITIALIZATION);
        // send negative acknowledge back.
        SendAcknowledgeNOK(m_RefInitDCL);
    }
}

/*****************************************************************************/
/**
 *  \brief     Configuration finished notification
 *
 *  \param     InstanceID   = Device instance ID for grappler identification
 *  \param     ConfigResult = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::DevProcConfigurationAckn(DevInstanceID_t InstanceID, ReturnCode_t ConfigResult)
{
    Q_UNUSED(InstanceID);

    if(ConfigResult == DCL_ERR_FCT_CALL_SUCCESS)
    {
        // log success
        //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_FINISHED, EVENT_DEVICECOMMANDPROCESSOR_STRING_CONFIGURATION);
        // send positive acknowledge back.
        SendAcknowledgeOK(m_RefInitDCL);
    }
    else
    {
        // log error
        //SEND_FATAL_ERROR(EVENT_DEVICECOMMANDPROCESSOR_ERROR_COMMAND_FAILED, EVENT_DEVICECOMMANDPROCESSOR_STRING_CONFIGURATION);
        // send negative acknowledge back.
        SendAcknowledgeNOK(m_RefInitDCL);
    }
}

/*****************************************************************************/
/**
 *  \brief     Normal operation mode start notification
 *
 *  \param     InstanceID = Device instance ID for grappler identification
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::DevProcStartNormalOpModeAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
}

/*****************************************************************************/
/**
 *  \brief     Mode change notification ( config - normal - diag - adjust - shutdown)
 *
 *  \param     InstanceID = Device instance ID for grappler identification
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     NewState   = New state identification
 *****************************************************************************/
void DeviceCommandProcessorThreadController::DevProcModeChangeInfo(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, quint8 NewState)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(NewState);
}

/*****************************************************************************/
/**
 *  \brief     Adjustment service start notification
 *
 *  \param     InstanceID = Device instance ID for grappler identification
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::StartAdjustmentServiceAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
}

/*****************************************************************************/
/**
 *  \brief     Reference run acknowledge if grappler device
 *
 *  \param     InstanceID  = Device instance ID for grappler identification
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::GrapplerReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    quint8 CmdIdx = 0;
    QStringList CommandNameList;

    QString ErrorArgString = QString("GrapplerReferenceRunAckn = 0x") + QString::number(Global::AsInt(InstanceID), 16).toUpper();

    //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_ERROR_DEVCTRLLAYER, ErrorArgString);

    CommandNameList.append(CmdGrapplerReferenceRun::NAME);
    if(GetCommandCtrlContainer(CmdIdx, InstanceID, CommandNameList))
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            // log command termination
//            SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_FINISHED,
//                      Global::TranslatableString(EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_REFRUN,
//                                                 Global::tTranslatableStringList() << GrapplerNameByInstanceID(m_CommandRef[CmdIdx].m_DevInstanceID)));
            // send positive acknowledge back
            SendAcknowledgeOK(m_CommandRef[CmdIdx].m_RefType);
        }
        else
        {
            // log command failure
           // SEND_FATAL_ERROR(EVENT_DEVICECOMMANDPROCESSOR_ERROR_COMMAND_FAILED,
                         //    Global::TranslatableString(EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_REFRUN,
                                                        //Global::tTranslatableStringList() << GrapplerNameByInstanceID(m_CommandRef[CmdIdx].m_DevInstanceID)));
            // send negative acknowledge back
            SendAcknowledgeNOK(m_CommandRef[CmdIdx].m_RefType);
        }
        m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_FREE;
    }
    else
    {
        // send negative acknowledge back.
        //SendAcknowledgeNOK(m_RefGrappler1RefRun);
    }
}

/*******************************************************************/
/**
 *  \brief     Acknowledge of a positioning ( result of ReqStationAction(.., STATION_ACTION_GRAB - STATION_ACTION_DISCARD)
 *
 *  \param     InstanceID  = Device instance ID
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::GrapplerPositioningAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);

    quint8 CmdIdx = 0;
    QStringList CommandNameList;

    CommandNameList.append(CmdGrapplerMoveEmptyTo::NAME);
    CommandNameList.append(CmdGrapplerTransportRackTo::NAME);
    CommandNameList.append(CmdGrapplerPullUpRack::NAME);
    CommandNameList.append(CmdGrapplerLetDownRack::NAME);
    CommandNameList.append(CmdGrapplerAttachToRack::NAME);
    CommandNameList.append(CmdGrapplerDetachFromRack::NAME);

    if(GetCommandCtrlContainer(CmdIdx, InstanceID, CommandNameList))
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            // log command termination
            QString InfoArgString = QString("GrapplerPositioningAckn InstanceID  = 0x") + QString::number(Global::AsInt(InstanceID), 16).toUpper();
            //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_FINISHED, InfoArgString);
            InfoArgString = QString("GrapplerPositioningAckn InstanceID  = 0x") + QString::number(Global::AsInt(m_CommandRef[CmdIdx].m_RefType), 16).toUpper();
            //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_FINISHED, InfoArgString);
            // send positive acknowledge back
            SendAcknowledgeOK(m_CommandRef[CmdIdx].m_RefType);
        }
        else
        {
            // log command failure
            QString InfoArgString = QString("GrapplerPositioningAckn InstanceID  = 0x") + QString::number(Global::AsInt(InstanceID), 16).toUpper();
            //SEND_FATAL_ERROR(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_FINISHED, InfoArgString);

            // send negative acknowledge back
            SendAcknowledgeNOK(m_CommandRef[CmdIdx].m_RefType);
        }
        m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Return the index of a free command control element
 *
 *
 *  \param    CmdIdx = Index of the free command ctrl element
 *
 *  \return   true if a free command was found
 *
 ***************************************************************************/
bool DeviceCommandProcessorThreadController::GetFreeCommandCtrlContainer(quint8& CmdIdx)
{
    quint8 Idx;

    m_Mutex.lock();
    for(Idx = 0; Idx < CMD_REF_CNT; Idx++)
    {
        if(m_CommandRef[Idx].m_State == DCP_TC_CMD_STATE_FREE)
        {
            CmdIdx = Idx;
            m_Mutex.unlock();
            return true;
        }
    }
    m_Mutex.unlock();

    return false;
}

/****************************************************************************/
/*!
 *  \brief    Return the specified command control element
 *
 *
 *  \param    CmdIdx = Index of the free command ctrl element
 *  \param    InstanceID = Instance ID of the device the command was adressed to
 *  \param    CommandNameList = List of command names
 *
 *  \return   true if a free command was found
 *
 ***************************************************************************/
 bool DeviceCommandProcessorThreadController::GetCommandCtrlContainer(quint8& CmdIdx, DeviceControl::DevInstanceID_t InstanceID, QStringList& CommandNameList)
{
    quint8 Idx;
    quint8 CommandNameCnt = 0;

    CommandNameCnt = CommandNameList.size();

    m_Mutex.lock();
    for(Idx = 0; Idx < CMD_REF_CNT; Idx++)
    {
        if((m_CommandRef[Idx].m_State == DCP_TC_CMD_STATE_ACTIV) &&
           (m_CommandRef[Idx].m_DevInstanceID == InstanceID))
        {
            if(CommandNameList.contains(m_CommandRef[Idx].m_CommandName))
            {
                CmdIdx = Idx;
                m_Mutex.unlock();
                return true;
            }
        }
    }
    m_Mutex.unlock();

    return false;
}

/*******************************************************************/
/**
 *  \brief     Report the RFID data ( result of ReqStationAction(.., STATION_ACTION_READ_RFID)
 *
 *  \param     InstanceID  = Device instance ID
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     RfidMSB     = Most significant bart (Franken duerfen das) of the 64-bit RFID-data word
 *  \param     RfidLSB     = Low significant bart of the 64-bit RFID-data word
 *****************************************************************************/
void DeviceCommandProcessorThreadController::GrapplerReportRFIDData(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, quint32 RfidMSB, quint32 RfidLSB)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(RfidMSB);
    Q_UNUSED(RfidLSB);
}

/*******************************************************************/
/**
 *  \brief     Report the RFID data ( result of ReqStationAction(.., STATION_ACTION_READ_LIQUID_LEVEL)
 *
 *  \param     InstanceID  = Device instance ID
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     LiquidLevelValue = Measuered liquid level
 *****************************************************************************/
void DeviceCommandProcessorThreadController::GrapplerReportLiquidLevel(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, quint16 LiquidLevelValue)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(LiquidLevelValue);
}

//###################################################
//loader event notification
//###################################################

/******************************************************************************/
/**
 *  \brief     Reference run acknowledge
 *
 *  \param     InstanceID  = Device instance ID
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     Position    = Drawer position
 *****************************************************************************/
void DeviceCommandProcessorThreadController::LoaderReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, LoaderPosition_t Position)
{
    quint8 CmdIdx = 0;
    QStringList CommandNameList;

    Q_UNUSED(Position);

    //QString ErrorArgString = QString("LoaderReferenceRunAckn = 0x") + QString::number(Global::AsInt(InstanceID), 16).toUpper();
    //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_ERROR_DEVCTRLLAYER, ErrorArgString);

    CommandNameList.append(CmdLoaderReferenceRun::NAME);
    if(GetCommandCtrlContainer(CmdIdx, InstanceID, CommandNameList))
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            SendAcknowledgeOK(m_CommandRef[CmdIdx].m_RefType);
        }
        else
        {
            SendAcknowledgeNOK(m_CommandRef[CmdIdx].m_RefType);
        }
        m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_FREE;
    }
}

/******************************************************************************/
/**
 *  \brief     The drawer has finished a positioning ( triggered by application), or a position request was called
 *
 *  \param     InstanceID  = Device instance ID
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     Position    = Drawer position
 *****************************************************************************/
void DeviceCommandProcessorThreadController::LoaderReportActDrawerPosition(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, LoaderPosition_t Position)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(Position);

    // Read RFID Data when Loader is Closed
    if (LOADER_CLOSED == Position)
    {
        // Start reading from first slot
        ReturnCode_t RetVal = ReqRFIDData(LOADER_ID_RFID_1);
    }
}

/******************************************************************************/
/**
 *  \brief     The drawer has finished a positioning which was triggered by the user
 *
 *  \param     InstanceID  = Device instance ID
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     Position    = Drawer position
 *****************************************************************************/
void DeviceCommandProcessorThreadController::LoaderReportDrawerUserAction(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo,
                                                                          LoaderPosition_t Position, BlockState_t BlockState)
{
    if (HdlInfo == DCL_ERR_FCT_CALL_SUCCESS) {
        CmdLoaderStateChanged *Command= new CmdLoaderStateChanged(InstanceID);

        Command->m_Open = (Position == LOADER_OPEN);
        Command->m_Locked = (BlockState == BLOCKING_STATE_BLOCKED);

        SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(Command));
    }
    else {
        //! \todo Send error command
    }

    // Read RFID Data when Loader is Closed
    if (LOADER_CLOSED == Position)
    {
        // Start reading from first slot
        ReturnCode_t RetVal = ReqRFIDData(LOADER_ID_RFID_1);
    }
}

/******************************************************************************/
/**
 *  \brief     Rfid data have been read (use instanceID to determine which cannel (FCTMOD_RFID<1-5>_LOADER, e.g. FCTMOD_RFID2_LOADER)
 *
 *  \param     InstanceID  = Device instance ID
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     RfidChannel = RFID channel
 *  \param     RfidMSB     = MSB of RFID data
 *  \param     RfidLSB     = LSB of RFID data
 *****************************************************************************/
void DeviceCommandProcessorThreadController::LoaderReportRFIDData(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, LoaderRFIDChannel_t RfidChannel, quint32 RfidMSB, quint32 RfidLSB)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(RfidChannel);
    Q_UNUSED(RfidMSB);
    Q_UNUSED(RfidLSB);

    if (RfidMSB > 0)
    {
        qint64 Ref = GetNewCommandRef();

        DeviceCommandProcessor::CmdLoaderUpdate* commandPtr(new DeviceCommandProcessor::CmdLoaderUpdate(DeviceControl::DEVICE_INSTANCE_ID_LOADER));
        commandPtr->m_RackId = 1;
        commandPtr->m_StationID = RfidChannel;

        //Todo: As actual data in RFID tag do not have any meaning now, dummy data is sent. Actual data shall be sent later on.
        //commandPtr->m_RackUserData = RfidMSB;
        commandPtr->m_RackUserData = 11;

        SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
    }

    if (RfidChannel < LOADER_ID_RFID_5)
    {
        // Read next slot
        ReturnCode_t RetVal = ReqRFIDData((LoaderRFIDChannel_t)(RfidChannel+1));
    }
}

/******************************************************************************/
/**
 *  \brief     The drawers blocking state has changed
 *
 *  \param     InstanceID    = Device instance ID
 *  \param     HdlInfo       = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     BlockingState = Drawer blocking state
 *****************************************************************************/
void DeviceCommandProcessorThreadController::LoaderReportBlockingState(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, BlockState_t BlockingState)
{
    quint8 CmdIdx = 0;
    QStringList CommandNameList;

    //Todo: check for blocking state, as requested
    Q_UNUSED(BlockingState);

    //QString ErrorArgString = QString("LoaderReportBlockingState = 0x") + QString::number(Global::AsInt(InstanceID), 16).toUpper();
    //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_ERROR_DEVCTRLLAYER, ErrorArgString);

    CommandNameList.append(CmdLoaderSetBlockingState::NAME);
    if(GetCommandCtrlContainer(CmdIdx, InstanceID, CommandNameList))
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            SendAcknowledgeOK(m_CommandRef[CmdIdx].m_RefType);
        }
        else
        {
            SendAcknowledgeNOK(m_CommandRef[CmdIdx].m_RefType);
        }
        m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_FREE;
    }
}

//###################################################
//oven event notification
//###################################################
/******************************************************************************/
/**
 *  \brief     Oven status notification, from temperature control
 *
 *  \param     InstanceID     = Device instance ID
 *  \param     HdlInfo        = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     StatusTempCtrl = Status of the oven's temperature control
 *****************************************************************************/
void DeviceCommandProcessorThreadController::OvenReportActStatusTempCtrl(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, TempCtrlStatus_t StatusTempCtrl)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(StatusTempCtrl);
}

/******************************************************************************/
/**
 *  \brief     Oven temperature notification
 *
 *  \param     InstanceID    = Device instance ID
 *  \param     HdlInfo       = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     Temperature  = Oven temperature
 *****************************************************************************/
void DeviceCommandProcessorThreadController::OvenReportActTemperature(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, qint16 Temperature)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(Temperature);
}

/******************************************************************************/
/**
 *  \brief     The oven cover has finished its reference run
 *
 *  \param     InstanceID    = Device instance ID
 *  \param     HdlInfo       = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     OvenCoverPos  = Oven cover position
 *****************************************************************************/
void DeviceCommandProcessorThreadController::OvenCoverReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, OvenCoverPosition_t OvenCoverPos)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(OvenCoverPos);
}

/******************************************************************************/
/**
 *  \brief     The oven cover has finished a positioning, or a position request was called
 *
 *  \param     InstanceID    = Device instance ID
 *  \param     HdlInfo       = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     OvenCoverPos  = Oven cover position
 *****************************************************************************/
void DeviceCommandProcessorThreadController::OvenReportActCoverPosition(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, OvenCoverPosition_t OvenCoverPos)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(OvenCoverPos);
}

/******************************************************************************/
/**
 *  \brief     Operating mode of the oven's temperature control
 *
 *  \param     InstanceID    = Device instance ID
 *  \param     HdlInfo       = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     OperatingMode = Operating mode
 *****************************************************************************/
void DeviceCommandProcessorThreadController::OvenReportActOperatingMode(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, TempCtrlOperatingMode_t OperatingMode)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(OperatingMode);
}

/***************************************************************************/
/**
 *  \brief     Actual temperature notification
 *
 *  \param     InstanceID  = Device instance ID
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     VesselID    = Cuvette identification
 *  \param     Temperature = Temperature
 *****************************************************************************/
void DeviceCommandProcessorThreadController::HeatedVesselsReportActTemperature(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, HeatedVesselID_t VesselID, qint16 Temperature)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(VesselID);
    Q_UNUSED(Temperature);
}

/*************************************************************************/
/**
 *  \brief     Temperature control status notification
 *
 *  \param     InstanceID     = Device instance ID
 *  \param     HdlInfo        = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     StatusTempCtrl = Status of the temperature controls (similar for all!)
 *****************************************************************************/
void DeviceCommandProcessorThreadController::HeatedVesselsReportActStatusTempCtrl(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, TempCtrlStatus_t StatusTempCtrl)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(StatusTempCtrl);
}

/******************************************************************************/
/**
 *  \brief     Temperature control operating mode notification
 *
 *  \param     InstanceID     = Device instance ID
 *  \param     HdlInfo        = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     OperatingMode  = Oparating mode of the temperature controls (similar for all!)
 *****************************************************************************/
void DeviceCommandProcessorThreadController::HeatedVesselsReportActOperatingMode(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, TempCtrlOperatingMode_t OperatingMode)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(OperatingMode);
}

/******************************************************************************/
/**
 *  \brief     Rack transfer reference run notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     Position   = Rack adapter position
 *****************************************************************************/
void DeviceCommandProcessorThreadController::RackTransferReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, RackAdapterPosition_t Position)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(Position);
}

/*******************************************************************/
/**
 *  \brief     The rack transfer has finished a positioning, or a position request was called
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     Position   = Rack adapter position
 *****************************************************************************/
void DeviceCommandProcessorThreadController::RackTransferReportActAdapterPosition(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, RackAdapterPosition_t Position)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(Position);
}

//###################################################
//agitation event notification
//###################################################

/*******************************************************************/
/**
 *  \brief     Agitation reference run notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     Position   = Agitatiopn position
 *****************************************************************************/
void DeviceCommandProcessorThreadController::AgitationReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationPosition_t Position)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(Position);
}

/*******************************************************************/
/**
 *  \brief     Agitation position notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     Position   = Agitatiopn position
 *****************************************************************************/
void DeviceCommandProcessorThreadController::AgitationReportActPosition(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationPosition_t Position)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(Position);
}

/*******************************************************************/
/**
 *  \brief     Agitation speed notification
 *             This function is called whenever a speed command was executed and the target speed was reached
 *             Target speed can be AGITATION_SPEED_STOP, which means the agitation has stopped.
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     Speed      = Agitatiopn speed
 *****************************************************************************/
void DeviceCommandProcessorThreadController::AgitationReportActSpeed(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationSpeed_t Speed)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(Speed);
}
/*******************************************************************/
/**
 *  \brief     Current consumption notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     FanCurrent = Fan current
 *****************************************************************************/
void DeviceCommandProcessorThreadController::ExhaustReportFanCurrent(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, quint16 FanCurrent)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(FanCurrent);
}

/*******************************************************************/
/**
 *  \brief     Flow sensor status notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     FlowSensor = Flow sensor value
 *****************************************************************************/
void DeviceCommandProcessorThreadController::ExhaustReportFlowSensorStatus(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, quint16 FlowSensor)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(FlowSensor);
}

/*******************************************************************/
/**
 *  \brief     Collecting tray liquid level notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     LiquidLevel = Liquid level at collecting tray
 *****************************************************************************/
void DeviceCommandProcessorThreadController::WaterReportLiquidLevel(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, quint16 LiquidLevel)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(LiquidLevel);
}

/*******************************************************************/
/**
 *  \brief     Water valve status notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     ValveID     = Valve identification
 *  \param     ValveStatus = Valve status
 *****************************************************************************/
void DeviceCommandProcessorThreadController::WaterReportValveStatus(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, WaterValveID_t ValveID, quint8 ValveStatus)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(ValveID);
    Q_UNUSED(ValveStatus);
}


/*******************************************************************/
/**
 *  \brief     Inclination value notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     InclValue  = Inclination value
 *****************************************************************************/
void DeviceCommandProcessorThreadController::InclinometerReportInclination(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, qint16 InclValue)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(InclValue);
}

/*******************************************************************/
/**
 *  \brief     Inclination limit setting acknowledge notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::InclinometerReportInclinationLimitAck(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
}

/*******************************************************************/
/**
 *  \brief     Inclination reference position acknowledge notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     InclValue  = Inclination value
 *****************************************************************************/
void DeviceCommandProcessorThreadController::InclinometerReportReferencePosAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, qint16 InclValue)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(InclValue);
}

/*******************************************************************/
/**
 *  \brief     Reference run notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::CoverLineReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
}

/*******************************************************************/
/**
 *  \brief     Rack cover acknowledge notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::CoverLineCoverAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
}

//###################################################
//Rack handling device event notification
//###################################################
/*******************************************************************/
/**
 *  \brief     Reference run notification
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::RackHandlingReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
}

/*******************************************************************/
/**
 *  \brief     Rack handling positioning acknowledge notification
 *             Result of ReqStationAction(.., STATION_ACTION_GRAB - STATION_ACTION_DISCARD
 *
 *  \param     InstanceID = Device instance ID
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::RackHandlingPositioningAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
}

/******************************************************************************/
/**
 *  \brief     Hood State Req acknowledge
 *
 *  \param     InstanceID  = Device instance ID
 *  \param     HdlInfo     = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     HoodState   = Hood Status (Open / Close)
 *****************************************************************************/
void DeviceCommandProcessorThreadController::HoodStatusReqAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, HoodState_t HoodState)
{
    quint8 CmdIdx = 0;
    QStringList CommandNameList;

    Q_UNUSED(HoodState);

    //QString ErrorArgString = QString("LoaderReferenceRunAckn = 0x") + QString::number(Global::AsInt(InstanceID), 16).toUpper();
    //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_ERROR_DEVCTRLLAYER, ErrorArgString);

    /**
      \todo     Currently using CmdLoaderReferenceRun, instead appropriate command shall be used i.e. CmdHoodStatusUpdate.
      */
    CommandNameList.append(CmdLoaderReferenceRun::NAME);
    if(GetCommandCtrlContainer(CmdIdx, InstanceID, CommandNameList))
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            SendAcknowledgeOK(m_CommandRef[CmdIdx].m_RefType);
        }
        else
        {
            SendAcknowledgeNOK(m_CommandRef[CmdIdx].m_RefType);
        }

        m_CommandRef[CmdIdx].m_State = DCP_TC_CMD_STATE_FREE;
    }
}

/*******************************************************************/
/**
 *  \brief      To request RFID data
 *
 *  \param      RfidChannel = RF Channel ID
 *  \param
 *
 *  \return     Return Code
 */
 /******************************************************************/
ReturnCode_t DeviceCommandProcessorThreadController::ReqRFIDData(LoaderRFIDChannel_t RfidChannel)
{
//        if(GetFreeCommandCtrlContainer(CmdIdx))
    {
        CLoaderDevice* pLoader = static_cast<CLoaderDevice*>(m_DeviceProcessing.GetDevice(DEVICE_INSTANCE_ID_LOADER));

        if(pLoader)
        {
            ReturnCode_t RetVal = pLoader->ReqRFIDData(RfidChannel);
        }
    }
}

} // end namespace DeviceCommandProcessor
