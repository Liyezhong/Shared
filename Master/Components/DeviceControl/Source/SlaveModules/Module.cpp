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

} //namespace
