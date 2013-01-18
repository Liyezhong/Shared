/****************************************************************************/
/*! \file Module.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CModule
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

#include "DeviceControl/Include/SlaveModules/Module.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"
#include "Global/Include/EventObject.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CModule
 *
 *  \iparam eObjectType = Object type
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to CAN communication object
 */
/****************************************************************************/
CModule::CModule(CModuleConfig::CANObjectType_t eObjectType, const CANMessageConfiguration *p_MessageConfiguration,
                 CANCommunicator* pCANCommunicator) :
    m_pCANCommunicator(pCANCommunicator), mp_MessageConfiguration(p_MessageConfiguration), m_pCANObjectConfig(0),
    m_lastEventHdlInfo(DCL_ERR_FCT_CALL_SUCCESS), m_lastEventGroup(0), m_lastEventCode(0), m_lastEventData(0),
    m_unCanIDEventInfo(0), m_unCanIDEventWarning(0), m_unCanIDEventError(0), m_unCanIDEventFatalError(0),
    m_unCanIDReqDataReset(0), m_unCanIDAcknDataReset(0),
    m_eObjectType(eObjectType), m_sOrderNr(0)
{
    m_TaskID = MODULE_TASKID_INIT;
    m_TimerRunning = COBJ_TIMER_STOPPED;

    if(ftime(&m_tbTimeDelay) != 0)
    {
        FILE_LOG_L(laINIT, llERROR) << " ftime failed.";
    }
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CModule
 */
/****************************************************************************/
CModule::~CModule()
{
    try
    {
        delete m_pCANObjectConfig;
    }
    catch (...)
    {
        // and exit
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  Initialize the module's event CAN message IDs
 *
 *      The CAN-IDs are read from the CAN-Message configuration class. The
 *      CAN-ID for event notification are determined here.
 *
 *  \iparam ModuleID = Identifier of the function module
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CModule::InitializeEventCANMessages(quint8 ModuleID)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bIfaceID;

    if(m_pCANObjectConfig == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    bIfaceID = m_pCANObjectConfig->m_sChannel;

    m_unCanIDEventInfo       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "EventInfo", bIfaceID, GetNodeID());
    m_unCanIDEventWarning    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "EventWarning", bIfaceID, GetNodeID());
    m_unCanIDEventError      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "EventError", bIfaceID, GetNodeID());
    m_unCanIDEventFatalError = mp_MessageConfiguration->GetCANMessageID(ModuleID, "EventFatalError", bIfaceID, GetNodeID());

    m_unCanIDReqDataReset    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqDataReset", bIfaceID, GetNodeID());
    m_unCanIDAcknDataReset   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AcknDataReset", bIfaceID, GetNodeID());

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Register the receive event CAN-messages to communication layer
 *
 *      Each receiveable CAN-message must be registered to the communication
 *      layer. This enables the communication layer to call the
 *      'HandleCANMessage(..)' function of this instance after receiption of
 *      the message.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CModule::RegisterEventCANMessages()
{
    ReturnCode_t RetVal;

    RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDEventInfo, this);
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDEventWarning, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDEventError, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDEventFatalError, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDAcknDataReset, this);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Start time measurement
 */
/****************************************************************************/
void CModule::StartTimeDelay()
{
    if(COBJ_TIMER_RUN == m_TimerRunning)
    {
        return;
    }

    if(ftime(&m_tbTimeDelay) == 0)
    {
        m_TimerRunning = COBJ_TIMER_RUN;
    }
}

/****************************************************************************/
/*!
 *  \brief  Time measurement
 *
 *      Returns the time passed since StartTimeDelay call.
 */
/****************************************************************************/
quint16 CModule::GetTimeDelay()
{
    struct timeb tp;
    quint16 sMilliSeconds, sSeconds;

    if(ftime(&tp) != 0)
    {
        return 0xFFFF;
    }

    if(COBJ_TIMER_STOPPED == m_TimerRunning)
        return 0xFFFF;

    sSeconds = tp.time - m_tbTimeDelay.time;

    if(tp.millitm < m_tbTimeDelay.millitm)
    {
        sMilliSeconds = tp.millitm + (1000-m_tbTimeDelay.millitm);
        sSeconds--;
    }
    else
        sMilliSeconds = tp.millitm - m_tbTimeDelay.millitm;

    sMilliSeconds = ((quint16)(sSeconds*1000)) + sMilliSeconds;

    return sMilliSeconds;
}

/****************************************************************************/
/*!
 *  \brief  Time measurement
 *
 *      Stops time measurement.
 */
/****************************************************************************/
void CModule::StopTimeDelay()
{
    m_TimerRunning = COBJ_TIMER_STOPPED;
}

/****************************************************************************/
/*!
 *  \brief  Returns the module handle
 *
 *      Returns the unique ID of a function module or CANNode. The module
 *      handle is a unsigned 32 bit variable and is composed by nodeID,
 *      nodeIndex and channel.
 *
 *      Bit 16..23:  channel
 *      Bit  7..11:  nodeIndex
 *      Bit  0...7:  nodeID (NodeType from hwconfiguration.xml)
 *
 *  \return The module handle
 */
/****************************************************************************/
quint32 CModule::GetModuleHandle()
{
    quint32 moduleHandle = 0;
    quint32 node;
    quint32 channel;

    if(m_pCANObjectConfig)
    {
        moduleHandle = m_pCANObjectConfig->m_sCANNodeType;
        node = m_pCANObjectConfig->m_sCANNodeIndex;
        moduleHandle |= (node << 8);
        channel = m_pCANObjectConfig->m_sChannel;
        moduleHandle |= (channel << 16);
    }

    return moduleHandle;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the data reset
 *
 *      The request will be acknowledge from slave by sending the CAN message
 *      'm_unCanIDAcknPartitionDataReset'
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CModule::SendCANMsgReqDataReset()
{
    ReturnCode_t result;
    can_frame canmsg;

    FILE_LOG_L(laCOMM, llDEBUG)  << "CANNode " << GetName().toStdString() << ": send partition data reset req.: 0x" << std::hex << m_unCanIDReqDataReset;

    canmsg.can_id = m_unCanIDReqDataReset;
    canmsg.can_dlc = 2;
    SetCANMsgDataU16(&canmsg, ComputePassword(), 0);
    result = m_pCANCommunicator->SendCOB(canmsg);

    return result;
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of an event CAN message
 *
 *  \iparam pCANframe = struct contains the data of the received CAN message
 *
 *  \return Platform event code
 */
/****************************************************************************/
quint32 CModule::HandleCANMsgEvent(can_frame* pCANframe)
{
    quint32 EventCode = 0;

    if(pCANframe->can_dlc == 6)
    {
        QString Type;

        // 16 Bit error group
        m_lastEventGroup = ((quint16) pCANframe->data[0]) << 8;
        m_lastEventGroup |= ((quint16) pCANframe->data[1]);
        // 16 Bit error code (MSB shows active / inactive)
        m_lastEventCode = ((quint16) pCANframe->data[2]) << 8;
        m_lastEventCode |= ((quint16) pCANframe->data[3]);
        // 16 Bit additional error data
        m_lastEventData = ((quint16) pCANframe->data[4]) << 8;
        m_lastEventData |= ((quint16) pCANframe->data[5]);

        if(pCANframe->can_id == m_unCanIDEventInfo) {
            Type = "info";
            m_lastEventHdlInfo = DCL_ERR_EXTERNAL_INFO;
        }
        else if(pCANframe->can_id == m_unCanIDEventWarning) {
            Type = "warning";
            m_lastEventHdlInfo = DCL_ERR_EXTERNAL_WARNING;
        }
        else if(pCANframe->can_id == m_unCanIDEventError) {
            Type = "error";
            m_lastEventHdlInfo = DCL_ERR_EXTERNAL_ERROR;
        }
        else {
            Type = "fatal error";
            m_lastEventHdlInfo = DCL_ERR_EXTERNAL_ERROR;
        }

        FILE_LOG_L(laFCT, llERROR) << " CModule: " << Type.toStdString() << " msg received:" <<
                                      " group: 0x" << std::hex << m_lastEventGroup <<
                                      " code: 0x" << std::hex << m_lastEventCode <<
                                      " data: 0x" << std::hex << m_lastEventData;

        m_lastEventTime = Global::AdjustedTime::Instance().GetCurrentDateTime();

        // this error was reported from external (CAN-bus)
        m_lastEventHdlInfo = DCL_ERR_EXTERNAL_ERROR;

        EventCode = BuildEventCode(m_lastEventGroup, m_lastEventCode, m_lastEventHdlInfo);

        Global::EventObject::Instance().RaiseEvent(EventCode, Global::FmtArgs() << GetName() << m_lastEventData);
    }

    return EventCode;
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of date reset acknowledgements
 *
 *      The reset of the module command has to be done in the subclass.
 *
 *  \iparam pCANframe = Received CAN message
 */
/****************************************************************************/
void CModule::HandleCANMsgAcknDataReset(can_frame* pCANframe)
{
    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString() << ": Acknowledge data reset received:"
                                  << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 1)
    {
        ReturnCode_t HdlInfo = DCL_ERR_FCT_CALL_SUCCESS;

        if(pCANframe->data[0] == 0)
        {
            //the command was not successfully executed
            HdlInfo = DCL_ERR_FCT_CALL_FAILED;
        }

        emit ReportDataResetAckn(GetModuleHandle(), HdlInfo);
    }
    else
    {
        emit ReportDataResetAckn(GetModuleHandle(), DCL_ERR_CANMSG_INVALID);
    }
}

/*****************************************************************************/
/*!
 *  \brief  Set bytes in CAN message's data from qint32
 *
 *  \iparam pCANframe = CAN message
 *  \iparam msgData = Data to be set
 *  \iparam offset  = Byte offset in the CAN message's data field
 */
/****************************************************************************/
void CModule::SetCANMsgDataS32(can_frame* pCANframe, qint32 msgData, quint8 offset)
{
    SetCANMsgDataU32(pCANframe, (quint32) msgData, offset);
}

/*****************************************************************************/
/*!
 *  \brief  Set bytes in CAN message's data from quint32
 *
 *  \iparam pCANframe = CAN message
 *  \iparam msgData = Data to be set
 *  \iparam offset  = Byte offset in the CAN message's data field
 */
/****************************************************************************/
void CModule::SetCANMsgDataU32 (can_frame* pCANframe, quint32 msgData, quint8 offset)
{
    if(offset <= 4)
    {
        pCANframe->data[offset++] = (quint8) ((msgData & 0xFF000000) >> 24);
        pCANframe->data[offset++] = (quint8) ((msgData & 0x00FF0000) >> 16);
        pCANframe->data[offset++] = (quint8) ((msgData & 0x0000FF00) >> 8);
        pCANframe->data[offset]   = (quint8) (msgData & 0x000000FF);
    }
}

/*****************************************************************************/
/*!
 *  \brief  Returns a quint16 from can message data
 *
 *  \iparam pCANframe = CAN message
 *  \iparam offset = Byte offset in the CAN message's data field
 *
 *  \return CAN data as quint16
 */
/****************************************************************************/
quint16 CModule::GetCANMsgDataU16 (can_frame* pCANframe, quint8 offset)
{
    quint16 msgData = 0;

    msgData = pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset];

    return msgData;
}

/*****************************************************************************/
/*!
 *  \brief  Returns a quint32 from can message data
 *
 *  \iparam pCANframe = CAN message
 *  \iparam offset = Byte offset in the CAN message's data field
 *
 *  \return CAN data as quint32
 */
/****************************************************************************/
quint32 CModule::GetCANMsgDataU32 (can_frame* pCANframe, quint8 offset)
{
    quint32 msgData = 0;

    msgData = pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset];

    return msgData;
}

/*****************************************************************************/
/*!
 *  \brief  Returns a quint64 from can message data
 *
 *  \iparam pCANframe = CAN message
 *
 *  \return CAN data as quint64
 */
/****************************************************************************/
quint64 CModule::GetCANMsgDataU64 (can_frame* pCANframe)
{
    quint64 msgData = 0;
    quint8 offset = 0;

    msgData = pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset];

    return msgData;
}

/*****************************************************************************/
/*!
 *  \brief  Set byte in CAN message data from qint16
 *
 *  \iparam pCANframe = CAN message
 *  \iparam msgData = Data to be set
 *  \iparam offset  = Byte offset in the CAN message's data field
 */
/****************************************************************************/
void CModule::SetCANMsgDataS16(can_frame* pCANframe, qint16 msgData, quint8 offset)
{
    SetCANMsgDataU16(pCANframe, (quint16) msgData, offset);
}

/*****************************************************************************/
/*!
 *  \brief  Set byte in CAN message data from quint16
 *
 *  \iparam pCANframe = CAN message
 *  \iparam msgData = Data to be set
 *  \iparam offset  = Byte offset in the CAN message's data field
 */
/****************************************************************************/
void CModule::SetCANMsgDataU16(can_frame* pCANframe, quint16 msgData, quint8 offset)
{
    if(offset <= 6)
    {
        pCANframe->data[offset++] = (quint8) ((msgData & 0xFF00) >> 8);
        pCANframe->data[offset]   = (quint8) (msgData & 0x00FF);
    }
}

/****************************************************************************/
/*!
 *  \brief  Computes the password for partition reset and format
 *
 *  \return Password
 */
/****************************************************************************/
quint16 CModule::ComputePassword()
{
    QDateTime CurrentDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QDate CurrentDate = CurrentDateTime.date();
    QTime CurrentTime = CurrentDateTime.time();

    return ((CurrentDate.year() * CurrentDate.month() * CurrentDate.day()) + CurrentTime.hour()) ^ 0x8320u;
}

quint32 CModule::BuildEventCode(quint16 ModuleId, quint16 SlaveEventCode, ReturnCode_t EventClass)
{
    quint32 EventCode = 0;

    switch (ModuleId) {
        case MODULE_ID_BASEMODULE:
            if (SlaveEventCode < 256) {
                EventCode = EVENT_GROUP_PLATFORM_SLAVE_HAL;
            }
            else {
                EventCode = EVENT_GROUP_PLATFORM_SLAVE_BASEMODULE;
            }
            break;
        case MODULE_ID_DIGITAL_OUT:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_DIGITALOUTPUT;
            break;
        case MODULE_ID_DIGITAL_IN:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_DIGITALINPUT;
            break;
        case MODULE_ID_ANALOG_OUT:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_ANALOGOUTPUT;
            break;
        case MODULE_ID_ANALOG_IN:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_ANALOGINPUT;
            break;
        case MODULE_ID_STEPPER:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_STEPPERMOTOR;
            break;
        case MODULE_ID_JOYSTICK:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_JOYSTICK;
            break;
        case MODULE_ID_RFID11785:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_RFIDISO11785;
            break;
        case MODULE_ID_RFID15693:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_RFIDISO15693;
            break;
        case MODULE_ID_TEMPERATURE:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_TEMPERATURECONTROL;
            break;
        case MODULE_ID_UART:
            EventCode = EVENT_GROUP_PLATFORM_SLAVE_UART;
            break;
    }

    switch (EventClass) {
        case DCL_ERR_EXTERNAL_WARNING:
            EventCode += 1 << 12;
            break;
        case DCL_ERR_EXTERNAL_INFO:
            EventCode += 2 << 12;
            break;
        default:
            break;
    }

    EventCode += SlaveEventCode;

    return EventCode;
}

} //namespace
