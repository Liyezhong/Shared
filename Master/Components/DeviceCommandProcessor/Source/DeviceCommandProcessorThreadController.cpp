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
//#include <DeviceCommandProcessor/Include/DeviceCommandProcessorEventCodes.h>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"

#include <DeviceCommandProcessor/Include/Commands/CmdDeviceProcessingInit.h>
#include <DeviceCommandProcessor/Include/Commands/CmdDeviceProcessingCleanup.h>

#include <Global/Include/Commands/AckOKNOK.h>


#include "DeviceControl/Include/Devices/DeviceAgitation.h"
#include "DeviceControl/Include/Devices/DeviceDrawer.h"
#include "DeviceControl/Include/Devices/DeviceHeatedCuvettes.h"
#include "DeviceControl/Include/Devices/DeviceHood.h"
#include "DeviceControl/Include/Devices/DeviceOven.h"
#include "DeviceControl/Include/Devices/DeviceRackTransfer.h"

#include <stdlib.h>
#include <math.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
DeviceCommandProcessorThreadController::DeviceCommandProcessorThreadController(
    Global::gSourceType TheHeartBeatSource, QString name) :
    Threads::ThreadController(TheHeartBeatSource, name),
    m_RefInitDCL(Global::RefManager<Global::tRefType>::INVALID),
    m_InLoaderChangedRef(Global::RefManager<Global::tRefType>::INVALID),
    m_InUnloaderChangedRef(Global::RefManager<Global::tRefType>::INVALID)
  , m_pDeviceProcessing(NULL)
{
    qRegisterMetaType<DevInstanceID_t>("DevInstanceID_t");
    qRegisterMetaType<ReturnCode_t>("ReturnCode_t");

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

    m_pDeviceProcessing = new IDeviceProcessing();

    if (m_pDeviceProcessing) {
        CONNECTSIGNALSLOT(m_pDeviceProcessing, ReportInitializationFinished(ReturnCode_t),
                          this, DevProcInitialisationAckn(ReturnCode_t));
        CONNECTSIGNALSLOT(m_pDeviceProcessing, ReportConfigurationFinished(ReturnCode_t),
                          this, DevProcConfigurationAckn(ReturnCode_t));
        CONNECTSIGNALSLOT(m_pDeviceProcessing, ReportStartNormalOperationMode(ReturnCode_t),
                          this, DevProcStartNormalOpModeAckn(ReturnCode_t));
    }

    // register init and cleanup functions
    RegisterCommandForProcessing<DeviceCommandProcessor::CmdDeviceProcessingInit, DeviceCommandProcessorThreadController>(
            &DeviceCommandProcessorThreadController::OnDeviceProcessingInit, this);
    RegisterCommandForProcessing<DeviceCommandProcessor::CmdDeviceProcessingCleanup, DeviceCommandProcessorThreadController>(
            &DeviceCommandProcessorThreadController::OnDeviceProcessingCleanup, this);

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
    if (m_pDeviceProcessing) {
        m_pDeviceProcessing->Start();
    }
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
void DeviceCommandProcessorThreadController::OnDeviceProcessingInit(Global::tRefType Ref, const CmdDeviceProcessingInit &Cmd)
{
    DEBUGWHEREAMI;
    Q_UNUSED(Cmd);

    m_RefInitDCL = Ref;
//    SendAcknowledgeOK(Ref);
}

/****************************************************************************/
void DeviceCommandProcessorThreadController::OnDeviceProcessingCleanup(Global::tRefType Ref, const CmdDeviceProcessingCleanup &Cmd)
{
    DEBUGWHEREAMI;
    Q_UNUSED(Cmd);

    m_pDeviceProcessing->Destroy();
    SendAcknowledgeOK(Ref);
}

/*****************************************************************************/
/**
 *  \brief     Initialisation finished notification
 *
 *  \param     InstanceID  = Device instance ID for grappler identification
 *  \param     InitResult  = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *****************************************************************************/
void DeviceCommandProcessorThreadController::DevProcInitialisationAckn(ReturnCode_t InitResult)
{
    if(InitResult == DCL_ERR_FCT_CALL_SUCCESS)
    {
        // log success
        //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_FINISHED, EVENT_DEVICECOMMANDPROCESSOR_STRING_INITIALIZATION);

        // log start of command
        //SEND_INFO(EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_STARTED, EVENT_DEVICECOMMANDPROCESSOR_STRING_CONFIGURATION);
        ReturnCode_t RetVal = m_pDeviceProcessing->StartConfigurationService();

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
void DeviceCommandProcessorThreadController::DevProcConfigurationAckn(ReturnCode_t ConfigResult)
{
    //! \todo added DCL_ERR_TIMEOUT, as it was always returning DCL_ERR_TIMEOUT. It was the same way in ProtoTest.
    if(ConfigResult == DCL_ERR_FCT_CALL_SUCCESS || ConfigResult == DCL_ERR_TIMEOUT)
    {
    }
    else
    {
        // log error
        //SEND_FATAL_ERROR(EVENT_HimalayaDeviceCommandProcessor_ERROR_COMMAND_FAILED, EVENT_HimalayaDeviceCommandProcessor_STRING_CONFIGURATION);
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
void DeviceCommandProcessorThreadController::DevProcStartNormalOpModeAckn(ReturnCode_t HdlInfo)
{
    try {

        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS) {
            ConnectDevices();
            // log success
            //SEND_INFO(EVENT_HimalayaDeviceCommandProcessor_INFO_COMMAND_FINISHED, EVENT_HimalayaDeviceCommandProcessor_STRING_CONFIGURATION);
            // send positive acknowledge back.
            SendAcknowledgeOK(m_RefInitDCL);
        }
        else {
            // log error
            //SEND_FATAL_ERROR(EVENT_HimalayaDeviceCommandProcessor_ERROR_COMMAND_FAILED, EVENT_HimalayaDeviceCommandProcessor_STRING_CONFIGURATION);
            // send negative acknowledge back.
            SendAcknowledgeNOK(m_RefInitDCL);
        }
    }
    catch(...) {
        SendAcknowledgeNOK(m_RefInitDCL);
    }
}


/*****************************************************************************/
/**
 *  \brief     Mode change notification ( config - normal - diag - adjust - shutdown)
 *
 *  \param     InstanceID = Device instance ID for grappler identification
 *  \param     HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise error return code
 *  \param     NewState   = New state identification
 *****************************************************************************/
void DeviceCommandProcessorThreadController::DevProcModeChangeInfo(ReturnCode_t HdlInfo, quint8 NewState)
{
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
void DeviceCommandProcessorThreadController::StartAdjustmentServiceAckn(ReturnCode_t HdlInfo)
{
    Q_UNUSED(HdlInfo);
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


} // end namespace DeviceCommandProcessor
