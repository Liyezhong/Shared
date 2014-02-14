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
    m_lastErrorHdlInfo(DCL_ERR_FCT_CALL_SUCCESS), m_lastErrorGroup(0), m_lastErrorCode(0), m_lastErrorData(0),
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
 *  \return  Returns the time passed since StartTimeDelay call.
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

    if(m_pCANObjectConfig)
    {
        moduleHandle = m_pCANObjectConfig->m_sCANNodeType;
        moduleHandle |= (((quint32)m_pCANObjectConfig->m_sCANNodeIndex) << 8); //lint !e571
        moduleHandle |= (((quint32)m_pCANObjectConfig->m_sChannel) << 16);
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
 *  \brief  Handles the reception of error-CAN message
 *
 *  \iparam pCANframe = struct contains the data of the received CAN message
 */
/****************************************************************************/
void CModule::HandleCANMsgError(can_frame* pCANframe)
{
    if(pCANframe->can_dlc == 6)
    {
        QString Type;

        // 16 Bit error group
        m_lastErrorGroup = ((quint16) pCANframe->data[0]) << 8;
        m_lastErrorGroup |= ((quint16) pCANframe->data[1]);
        // 16 Bit error code (MSB shows active / inactive)
        m_lastErrorCode = ((quint16) pCANframe->data[2]) << 8;
        m_lastErrorCode |= ((quint16) pCANframe->data[3]);
        // 16 Bit additional error data
        m_lastErrorData = ((quint16) pCANframe->data[4]) << 8;
        m_lastErrorData |= ((quint16) pCANframe->data[5]);

        if(pCANframe->can_id == m_unCanIDEventInfo) {
            Type = "info";
            m_lastErrorHdlInfo = DCL_ERR_EXTERNAL_INFO;
        }
        else if(pCANframe->can_id == m_unCanIDEventWarning) {
            Type = "warning";
            m_lastErrorHdlInfo = DCL_ERR_EXTERNAL_WARNING;
        }
        else if(pCANframe->can_id == m_unCanIDEventError) {
            Type = "error";
            m_lastErrorHdlInfo = DCL_ERR_EXTERNAL_ERROR;
        }
        else {
            Type = "fatal error";
            m_lastErrorHdlInfo = DCL_ERR_EXTERNAL_ERROR;
        }

        FILE_LOG_L(laFCT, llERROR) << " CModule: " << Type.toStdString() << " msg received:" <<
                                      " group: 0x" << std::hex << m_lastErrorGroup <<
                                      " code: 0x" << std::hex << m_lastErrorCode <<
                                      " data: 0x" << std::hex << m_lastErrorData;

        m_lastErrorTime = Global::AdjustedTime::Instance().GetCurrentDateTime();

        // this error was reported from external (CAN-bus)
        m_lastErrorHdlInfo = DCL_ERR_EXTERNAL_ERROR;
    }
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
    Q_ASSERT(pCANframe != 0);
    Q_ASSERT(offset <= 4);
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

    Q_ASSERT(pCANframe != 0);
    Q_ASSERT(offset <= 6);

    msgData = pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset];

    return msgData;
}
#ifdef PRE_ALFA_TEST
/*****************************************************************************/
/*!
 *  \brief  Returns a qint16 from can message data
 *
 *  \iparam pCANframe = CAN message
 *  \iparam offset = Byte offset in the CAN message's data field
 *
 *  \return CAN data as qint16
 */
/****************************************************************************/
qint16 CModule::GetCANMsgDataS16 (can_frame* pCANframe, quint8 offset)
{
    qint16 msgData = 0;

    Q_ASSERT(pCANframe != 0);
    Q_ASSERT(offset <= 6);

    msgData = pCANframe->data[offset++];
    msgData = (msgData << 8) | pCANframe->data[offset];

    return msgData;
}
#endif
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

    Q_ASSERT(pCANframe != 0);
    Q_ASSERT(offset <= 4);

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
 *  \iparam offset = Byte offset in the CAN message's data field
 *
 *  \return CAN data as quint64
 */
/****************************************************************************/
quint64 CModule::GetCANMsgDataU64 (can_frame* pCANframe)
{
    quint64 msgData = 0;
    quint8 offset = 0;

    Q_ASSERT(pCANframe != 0);

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
 *  \brief  Set bytes in CAN message's data from quint64
 *
 *  \iparam pCANframe = CAN message
 *  \iparam msgData = Data to be set
 */
/****************************************************************************/
void CModule::SetCANMsgDataU64 (can_frame* pCANframe, quint64 msgData)
{
    quint8 offset = 0;

    pCANframe->data[offset++] = (quint8) ((msgData & 0xFF00000000000000ull) >> 56);
    pCANframe->data[offset++] = (quint8) ((msgData & 0x00FF000000000000ull) >> 48);
    pCANframe->data[offset++] = (quint8) ((msgData & 0x0000FF0000000000ull) >> 40);
    pCANframe->data[offset++] = (quint8) ((msgData & 0x000000FF00000000ull) >> 32);
    pCANframe->data[offset++] = (quint8) ((msgData & 0x00000000FF000000ull) >> 24);
    pCANframe->data[offset++] = (quint8) ((msgData & 0x0000000000FF0000ull) >> 16);
    pCANframe->data[offset++] = (quint8) ((msgData & 0x000000000000FF00ull) >> 8);
    pCANframe->data[offset]   = (quint8) (msgData & 0x00000000000000FFull);
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
    Q_ASSERT(pCANframe != 0);
    Q_ASSERT(offset <= 6);
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
} //namespace
