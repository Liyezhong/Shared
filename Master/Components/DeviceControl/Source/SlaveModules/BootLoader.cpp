/****************************************************************************/
/*! \file BootLoader.cpp
 *
 *  \brief Implementation file for class CBootLoader.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 09.05.2012
 *   $Author:  $ M.Scherer
 *
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CBootLoader
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

#include "DeviceControl/Include/SlaveModules/ModuleIDs.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/SlaveModules/BootLoader.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

namespace DeviceControl
{

const qint32 CBootLoader::m_Timeout = 2000;
const quint32 CBootLoader::m_Crc32Polynomial = 0x04C11DB7;
const quint32 CBootLoader::m_Crc32InitialValue = 0xFFFFFFFF;

/****************************************************************************/
/*!
 *  \brief  Constructor of the class CBootLoader
 *
 *  \iparam p_CanMsgConfig = List of available CAN messages
 *  \iparam CanNodeId = Node ID of the base module
 *  \iparam p_CanCommunicator = Send and receives CAN messages
 *  \iparam p_BaseModule = Base module the boot loader is assigned to
 */
/****************************************************************************/
CBootLoader::CBootLoader(const CANMessageConfiguration *p_CanMsgConfig, const quint32 CanNodeId,
                         DeviceControl::CANCommunicator *p_CanCommunicator, DeviceControl::CBaseModule *p_BaseModule) :
    mp_CanCommunicator(p_CanCommunicator), mp_BaseModule(p_BaseModule),
    m_UpdateRequired(false), m_WaitForUpdate(false), mp_Info(NULL), m_State(IDLE)
{
    if (p_CanMsgConfig == NULL) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator == NULL) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_BaseModule == NULL) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }

    m_CanIdUpdateRequired    = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateRequired", 0, CanNodeId);
    m_CanIdUpdateModeRequest = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateModeRequest", 0, CanNodeId);
    m_CanIdUpdateModeAck     = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateModeAck", 0, CanNodeId);
    m_CanIdUpdateHeader      = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateHeader", 0, CanNodeId);
    m_CanIdUpdateHeaderAck   = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateHeaderAck", 0, CanNodeId);
    m_CanIdUpdateData0       = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateData0", 0, CanNodeId);
    m_CanIdUpdateAck0        = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateAck0", 0, CanNodeId);
    m_CanIdUpdateData1       = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateData1", 0, CanNodeId);
    m_CanIdUpdateAck1        = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateAck1", 0, CanNodeId);
    m_CanIdUpdateTrailer     = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateTrailer", 0, CanNodeId);
    m_CanIdUpdateTrailerAck  = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateTrailerAck", 0, CanNodeId);
    m_CanIdUpdateInfoInit    = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateInfoInit", 0, CanNodeId);
    m_CanIdUpdateInfoInitAck = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateInfoInitAck", 0, CanNodeId);
    m_CanIdUpdateInfo        = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateInfo", 0, CanNodeId);
    m_CanIdUpdateInfoAck     = p_CanMsgConfig->GetCANMessageID(MODULE_ID_BASEMODULE, "UpdateInfoAck", 0, CanNodeId);

    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateRequired, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateModeAck, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateHeaderAck, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateAck0, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateAck1, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateTrailerAck, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateInfoInit, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateInfoInitAck, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateInfo, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }
    if (mp_CanCommunicator->RegisterCOB(m_CanIdUpdateInfoAck, mp_BaseModule) != DCL_ERR_FCT_CALL_SUCCESS) {
        THROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
    }

    m_Timer.setSingleShot(true);
    m_Timer.setInterval(m_Timeout);
    CONNECTSIGNALSLOT(&m_Timer, timeout(), this, TimeoutHandler());
}

/****************************************************************************/
/*!
 *  \brief  This initiates a firmware update
 *
 *  \iparam FirmwarePath = The path to the firmware image file
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed
 *          in transmit queue, otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBootLoader::UpdateFirmware(const QString &FirmwarePath)
{
    QMutexLocker Locker(&m_Mutex);

    if (m_State == FIRMWARE || m_State == INFO) {
        return DCL_ERR_INVALID_STATE;
    }

    m_FirmwareImage.setFileName(FirmwarePath);
    if (m_FirmwareImage.open(QIODevice::ReadOnly) == false) {
        return DCL_ERR_INVALID_PARAM;
    }

    SetState(FIRMWARE);
    m_Timer.start();

    return SendModeRequest(1);
}

/****************************************************************************/
/*!
 *  \brief  Tells the boot loader to wait for a software update
 *
 *  \iparam Wait = Wait for an upadate (true) or not (false)
 */
/****************************************************************************/
void CBootLoader::WaitForUpdate(bool Wait)
{
    QMutexLocker Locker(&m_Mutex);

    m_WaitForUpdate = Wait;
}

/****************************************************************************/
/*!
 *  \brief  Starts the firmware
 *
 *      This method prompts the boot loader to start the firmware image on
 *      the node. This is necessary after a firmware update or if the boot
 *      loader is in a wait state.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed
 *          in transmit queue, otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBootLoader::BootFirmware()
{
    QMutexLocker Locker(&m_Mutex);

    if (m_UpdateRequired == false) {
        ReturnCode_t Result = SendModeRequest(false);
        if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
            SetState(IDLE);
        }
        return Result;
    }
    return (DCL_ERR_INVALID_STATE);
}

/****************************************************************************/
/*!
 *  \brief  Sends a firmware data packet to the boot loader
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed
 *          in transmit queue, otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBootLoader::SendData()
{
    can_frame CanMsg;

    if (((m_FirmwareImage.pos() / 8) % 2) == 0) {
        CanMsg.can_id = m_CanIdUpdateData0;
    }
    else {
        CanMsg.can_id = m_CanIdUpdateData1;
    }

    CanMsg.can_dlc = m_FirmwareImage.read(reinterpret_cast<char *>(CanMsg.data), 8);

    if (CanMsg.can_dlc != 0) {
        return mp_CanCommunicator->SendCOB(CanMsg);
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief  This initiates an info block update
 *
 *  \iparam p_Info = The pointer to the info data
 *  \iparam Size = The size of info block in bytes
 *  \iparam UpdateType = The type of updated info blocks
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed
 *          in transmit queue, otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBootLoader::UpdateInfo(const quint8 *p_Info, quint32 Size, quint8 UpdateType)
{
    QMutexLocker Locker(&m_Mutex);

    if (p_Info == NULL || Size == 0 || UpdateType > 2) {
        return DCL_ERR_INVALID_PARAM;
    }

    if (m_State == FIRMWARE || m_State == INFO) {
        return DCL_ERR_INVALID_STATE;
    }

    mp_Info = new quint8[Size * sizeof(quint8)];
    if (mp_Info == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    else {
        memcpy(mp_Info, p_Info, Size * sizeof(quint8));

        m_InfoSize = Size;
        m_UpdateType = UpdateType;

        // Calculate CRC and fill it into last 4 bytes of info block
        quint32 Crc32 = CalculateCrc(mp_Info, Size-4);
        *((quint32*)(mp_Info + Size - 4)) = Crc32;

        SetState(INFO);
        m_Timer.start();

        return SendModeRequest(1);
    }
}

/****************************************************************************/
/*!
 *  \brief  Sends an info packet to the boot loader
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed
 *          in transmit queue, otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBootLoader::SendInfo()
{
    quint32 UnsentInfoSize = m_InfoSize - m_Count;

    if (mp_Info == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    if (UnsentInfoSize <= 0) {
        return DCL_ERR_FCT_CALL_SUCCESS;
    }

    can_frame CanMsg;

    CanMsg.can_id = m_CanIdUpdateInfo;

    if (UnsentInfoSize > 8) {
        memcpy(CanMsg.data, &mp_Info[m_Count], 8);
        CanMsg.can_dlc = 8;
    }
    else {
        memcpy(CanMsg.data, &mp_Info[m_Count], UnsentInfoSize);
        CanMsg.can_dlc = UnsentInfoSize;
    }

    return mp_CanCommunicator->SendCOB(CanMsg);
}


/****************************************************************************/
/*!
 *  \brief  Sends a mode request message to the node
 *
 *  \iparam StartUpdate = Sets if the mode request should start an update
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed
 *          in transmit queue, otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBootLoader::SendModeRequest(bool StartUpdate)
{
    can_frame CanMsg;

    CanMsg.can_id = m_CanIdUpdateModeRequest;
    CanMsg.can_dlc = 1;
    CanMsg.data[0] = StartUpdate;

    return mp_CanCommunicator->SendCOB(CanMsg);
}

/****************************************************************************/
/*!
 *  \brief  Calculates the CRC checksum of the firmware.
 *
 *  \iparam p_Data = Data block to be checked
 *  \iparam DataSize = Size of the data block
 *
 *  \return CRC32 checksum
 */
/****************************************************************************/
quint32 CBootLoader::CalculateCrc(const quint8 *p_Data, quint32 DataSize)
{
    quint32 Crc32 = m_Crc32InitialValue;

    // Perform modulo-2 division for each byte
    for (quint32 i = 0; i < DataSize; i++) {
        Crc32 ^= (*p_Data++) << (32 - 8);

        // Perform modulo-2 division for each bit in byte
        for (quint8 k = 0; k < 8; k++) {
            if (Crc32 & 0x80000000) {
                Crc32 = (Crc32 << 1) ^ m_Crc32Polynomial;
            }
            else {
                Crc32 = (Crc32 << 1);
            }
        }
    }
    return (Crc32 ^ m_Crc32InitialValue);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a CAN message
 *
 *  \iparam p_CanFrame = The data of the received CAN message
 */
/****************************************************************************/
void CBootLoader::HandleCanMessage(const can_frame *p_CanFrame)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    if (p_CanFrame->can_id == m_CanIdUpdateRequired) {
        ReturnCode = HandleCanMsgUpdateRequired(p_CanFrame);
    }
    else if (p_CanFrame->can_id == m_CanIdUpdateModeAck) {
        ReturnCode = HandleCanMsgUpdateModeAck(p_CanFrame);
    }
    else if (p_CanFrame->can_id == m_CanIdUpdateHeaderAck) {
        ReturnCode = HandleCanMsgUpdateHeaderAck(p_CanFrame);
    }
    else if (p_CanFrame->can_id == m_CanIdUpdateAck0 || p_CanFrame->can_id == m_CanIdUpdateAck1) {
        ReturnCode = HandleCanMsgUpdateAck(p_CanFrame);
    }
    else if (p_CanFrame->can_id == m_CanIdUpdateTrailerAck) {
        ReturnCode = HandleCanMsgUpdateTrailerAck(p_CanFrame);
    }
    else if (p_CanFrame->can_id == m_CanIdUpdateInfoInitAck) {
        ReturnCode = HandleCanMsgUpdateInfoInitAck(p_CanFrame);
    }
    else if (p_CanFrame->can_id == m_CanIdUpdateInfoAck) {
        ReturnCode = HandleCanMsgUpdateInfoAck(p_CanFrame);
    }

    if (ReturnCode != DCL_ERR_FCT_CALL_SUCCESS) {
        if (m_State == FIRMWARE) {
            emit ReportUpdateFirmware(mp_BaseModule->GetModuleHandle(), DCL_ERR_FCT_CALL_FAILED);
        }
        else if (m_State == INFO) {
            emit ReportUpdateInfo(mp_BaseModule->GetModuleHandle(), DCL_ERR_FCT_CALL_FAILED);
        }
        SetState(ACTIVE);
    }

    if (m_Timer.isActive() == true) {
        m_Timer.start();
    }
}

/****************************************************************************/
/*!
 *  \brief  Sets the main state of this module
 *
 *  \iparam State = The new state
 */
/****************************************************************************/
void CBootLoader::SetState(State_t State)
{
    if (State != m_State) {
        m_State = State;
        mp_BaseModule->BootLoaderUpdate(State);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a update required message
 *
 *  \iparam p_CanFrame = The data of the received CAN message
 */
/****************************************************************************/
ReturnCode_t CBootLoader::HandleCanMsgUpdateRequired(const can_frame *p_CanFrame)
{
    ReturnCode_t ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    if (p_CanFrame->can_dlc == 5) {
        m_UpdateRequired = p_CanFrame->data[0];

        if (!m_UpdateRequired && !m_WaitForUpdate) {
            SetState(IDLE);
            ReturnCode = SendModeRequest(0);
        }
        else {
            SetState(ACTIVE);
        }
    }
    else {
        ReturnCode = DCL_ERR_CANMSG_INVALID;
    }

    return ReturnCode;
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a update mode acknowledge message
 *
 *  \iparam p_CanFrame = The data of the received CAN message
 */
/****************************************************************************/
ReturnCode_t CBootLoader::HandleCanMsgUpdateModeAck(const can_frame *p_CanFrame)
{
    ReturnCode_t ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    if (p_CanFrame->can_dlc == 0) {
        if (m_State == FIRMWARE) {
            can_frame CanMsg;
            quint32 FirmwareSize = m_FirmwareImage.size();

            CanMsg.can_id = m_CanIdUpdateHeader;
            CanMsg.can_dlc = 8;
            CanMsg.data[0] = FirmwareSize >> 24;
            CanMsg.data[1] = FirmwareSize >> 16;
            CanMsg.data[2] = FirmwareSize >> 8;
            CanMsg.data[3] = FirmwareSize;
            CanMsg.data[4] = 0;
            CanMsg.data[5] = 0;
            CanMsg.data[6] = 0;
            CanMsg.data[7] = 0;

            ReturnCode = mp_CanCommunicator->SendCOB(CanMsg);
        }
        else if (m_State == INFO) {
            can_frame CanMsg;

            CanMsg.can_id = m_CanIdUpdateInfoInit;
            CanMsg.can_dlc = 8;
            CanMsg.data[0] = m_InfoSize >> 24;
            CanMsg.data[1] = m_InfoSize >> 16;
            CanMsg.data[2] = m_InfoSize >> 8;
            CanMsg.data[3] = m_InfoSize;
            CanMsg.data[4] = 0;
            CanMsg.data[5] = 0;
            CanMsg.data[6] = 0;
            CanMsg.data[7] = m_UpdateType;

            ReturnCode = mp_CanCommunicator->SendCOB(CanMsg);
        }
    }
    else {
        ReturnCode = DCL_ERR_CANMSG_INVALID;
    }

    return ReturnCode;
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a update header acknowledge message
 *
 *  \iparam p_CanFrame = The data of the received CAN message
 */
/****************************************************************************/
ReturnCode_t CBootLoader::HandleCanMsgUpdateHeaderAck(const can_frame *p_CanFrame)
{
    ReturnCode_t ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    if (p_CanFrame->can_dlc == 1) {
        if (p_CanFrame->data[0] == 0) {
            // Reset the receive counter
            m_Count = 0;
            // Send two data packets, should be changed later to increase bandwidth utilization
            ReturnCode = SendData();
            if (ReturnCode == DCL_ERR_FCT_CALL_SUCCESS) {
                ReturnCode = SendData();
            }
        }
        else {
            ReturnCode = DCL_ERR_INTERNAL_ERR;
        }
    }
    else {
        ReturnCode = DCL_ERR_CANMSG_INVALID;
    }

    return ReturnCode;
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a update data acknowledge message
 *
 *  \iparam p_CanFrame = The data of the received CAN message
 */
/****************************************************************************/
ReturnCode_t CBootLoader::HandleCanMsgUpdateAck(const can_frame *p_CanFrame)
{
    ReturnCode_t ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    if (p_CanFrame->can_dlc == 0) {
        // Increase the receive counter
        if (m_FirmwareImage.size() - m_Count >= 8) {
            m_Count += 8;
        }
        else {
            m_Count += m_FirmwareImage.size() - m_Count;
        }

        // The data was successfully transmitted
        if (m_Count == m_FirmwareImage.size()) {
            can_frame CanMsg;
            quint32 Crc32 = 0;

            if (m_FirmwareImage.reset() == false) {
                return DCL_ERR_INTERNAL_ERR;
            }

            quint8 *p_ImageData = new quint8[m_FirmwareImage.size()];
            if (p_ImageData != NULL) {
                quint32 ReadSize = m_FirmwareImage.read(reinterpret_cast<char *>(p_ImageData), m_FirmwareImage.size());
                Crc32 = CalculateCrc(p_ImageData, ReadSize);
            }
            else {
                return DCL_ERR_INTERNAL_ERR;
            }
            delete[] p_ImageData;

            CanMsg.can_id = m_CanIdUpdateTrailer;
            CanMsg.data[0] = Crc32 >> 24;
            CanMsg.data[1] = Crc32 >> 16;
            CanMsg.data[2] = Crc32 >> 8;
            CanMsg.data[3] = Crc32;
            CanMsg.can_dlc = 4;

            // Close the firmware image file
            m_FirmwareImage.close();

            ReturnCode = mp_CanCommunicator->SendCOB(CanMsg);
        }
        else {
            ReturnCode = SendData();
        }
    }
    else {
        ReturnCode = DCL_ERR_CANMSG_INVALID;
    }

    return ReturnCode;
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a update trailer acknowledge message
 *
 *  \iparam p_CanFrame = The data of the received CAN message
 */
/****************************************************************************/
ReturnCode_t CBootLoader::HandleCanMsgUpdateTrailerAck(const can_frame *p_CanFrame)
{
    ReturnCode_t ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    if (p_CanFrame->can_dlc == 1) {
        SetState(ACTIVE);
        if (p_CanFrame->data[0] == 0) {
            m_UpdateRequired = false;
            m_Timer.stop();
            emit ReportUpdateFirmware(mp_BaseModule->GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS);
        }
        else {
            ReturnCode = DCL_ERR_INTERNAL_ERR;
        }
    }
    else {
        ReturnCode = DCL_ERR_CANMSG_INVALID;
    }

    return ReturnCode;
}

/****************************************************************************/
/*!
 *  \brief  Handle the timeout of a firmware update
 */
/****************************************************************************/
void CBootLoader::TimeoutHandler()
{
    SetState(ACTIVE);
    emit ReportUpdateFirmware(mp_BaseModule->GetModuleHandle(), DCL_ERR_TIMEOUT);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a update info init acknowledge message
 *
 *  \iparam p_CanFrame = The data of the received CAN message
 */
/****************************************************************************/
ReturnCode_t CBootLoader::HandleCanMsgUpdateInfoInitAck(const can_frame *p_CanFrame)
{
    ReturnCode_t ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    if (p_CanFrame->can_dlc == 1) {
        if (p_CanFrame->data[0] == 0) {
            // Reset the receive counter
            m_Count = 0;
            // Send one info packets
            ReturnCode = SendInfo();
        }
        else {
            ReturnCode = DCL_ERR_INTERNAL_ERR;
        }
    }
    else {
        ReturnCode = DCL_ERR_CANMSG_INVALID;
    }

    return ReturnCode;
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a update info acknowledge message
 *
 *  \iparam p_CanFrame = The data of the received CAN message
 */
/****************************************************************************/
ReturnCode_t CBootLoader::HandleCanMsgUpdateInfoAck(const can_frame *p_CanFrame)
{
    ReturnCode_t ReturnCode = DCL_ERR_FCT_CALL_SUCCESS;

    // Normal ack when update has not been finished
    if (p_CanFrame->can_dlc == 0) {
        if (m_InfoSize - m_Count >= 8) {
            m_Count += 8;
        }
        else {
            m_Count = m_InfoSize;
        }
        ReturnCode = SendInfo();
    }
    // Normal ack when update has been finished
    // Check if CRC failed or not
    else if (p_CanFrame->can_dlc == 1) {
        delete[] mp_Info;
        SetState(ACTIVE);
        if (p_CanFrame->data[0] == 0) {
            emit ReportUpdateInfo(mp_BaseModule->GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS);
        }
        else {
            ReturnCode = DCL_ERR_INTERNAL_ERR;
        }
    }
    // Bad ack
    else {
        ReturnCode = DCL_ERR_CANMSG_INVALID;
    }

    return ReturnCode;
}

} //namespace DeviceControl
