/******************************************************************/
/*! \file Rfid15693.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CRfid15693
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

#ifndef DEVICECONTROL_RFID15693PRIVATE_H
#define DEVICECONTROL_RFID15693PRIVATE_H

#include "Global/Include/MonotonicTime.h"
#include "DeviceControl/Include/SlaveModules/FunctionModule.h"

namespace DeviceControl
{

class CANCommunicator;

#define MAX_RFID_CMD_IDX  2   ///< up to 2 module commands can be handled simultaneously

/****************************************************************************/
/*!
*   \brief This class implements the functionality to configure and control a
*          slave's 'RFID_15693' function module
*/
/****************************************************************************/
class CRfid15693 : public CFunctionModule
{
    Q_OBJECT

public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CRfid15693
     *
     *  \param p_MessageConfiguration
     *  \param pCANCommunicator
     *  \param pParentNode
     *
     *  \return from CRfid15693
     */
    /****************************************************************************/
    CRfid15693(const CANMessageConfiguration* p_MessageConfiguration, CANCommunicator* pCANCommunicator,
               CBaseModule* pParentNode);
    ~CRfid15693();

    //! Module initialisation
    ReturnCode_t Initialize();

    //! Task handling function
    void HandleTasks();
    //! CAN-message handling function
    void HandleCanMessage(can_frame* pCANframe);

    //! Set RFID module state
    ReturnCode_t SetConfig(bool Enabled);
    //! Acquire UID
    ReturnCode_t AcquireUid();
    //! Request a data block
    ReturnCode_t ReadBlock(quint8 Address);
    //! Write a data block
    ReturnCode_t WriteBlock(quint8 Address, quint32 Data);
    //! Lock a data block
    ReturnCode_t LockBlock(quint8 Address);

 signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to forward the state setting acknowledge
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportSetConfig(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to forward the acquire UID acknowledge
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Uid = Unique identification number
     */
    /****************************************************************************/
    void ReportAcquireUid(quint32 InstanceID, ReturnCode_t HdlInfo, quint64 Uid);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report the read data block
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Data = Read data block
     */
    /****************************************************************************/
    void ReportReadBlock(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 Data);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report that the data block was written
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportWriteBlock(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report that the data block was locked
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportLockBlock(quint32 InstanceID, ReturnCode_t HdlInfo);

 private:
    //! CAN message ID initialization
    ReturnCode_t InitializeCANMessages();
    //! registers the CAN messages to communication layer
    ReturnCode_t RegisterCANMessages();

    //! Idle taks handling function
    void HandleIdleState();

    //! sends the configuration CAN message
    ReturnCode_t SendCANMsgSetConfig(bool Enabled);
    //! send the acquire UID CAN message
    ReturnCode_t SendCANMsgAcquireUid();
    //! send the read data block CAN message
    ReturnCode_t SendCANMsgReadBlock(quint8 Address);
    //! send the write data block CAN message
    ReturnCode_t SendCANMsgWriteBlock(quint8 Address, quint32 Data);
    //! send the lock data block CAN message
    ReturnCode_t SendCANMsgLockBlock(quint8 Address);

    //! handles the receipt of can message 'SetConfig'
    void HandleCANMsgRespSetConfig(can_frame* pCANframe, ReturnCode_t hdlInfo);
    //! handles the receipt of can message 'AcquireUid'
    void HandleCANMsgRespAcquireUid(can_frame* pCANframe, ReturnCode_t hdlInfo);
    //! handles the receipt of can message 'ReadBlock'
    void HandleCANMsgReadBlock(can_frame* pCANframe, ReturnCode_t hdlInfo);
    //! handles the receipt of can message 'WriteBlock'
    void HandleCANMsgWriteBlock(can_frame* pCANframe, ReturnCode_t hdlInfo);
    //! handles the receipt of can message 'LockBlock'
    void HandleCANMsgLockBlock(can_frame* pCANframe, ReturnCode_t hdlInfo);

    //! command handling function
    void HandleCommandRequestTask();

    quint32 m_unCanIDSetConfig;         //!< CAN-message id of 'SetConfig' message
    quint32 m_unCanIDAcquireUid;        //!< CAN-message id of 'AcquireUid' message
    quint32 m_unCanIDRespAcquireUid;    //!< CAN-message id of 'RespAcquireUid' message
    quint32 m_unCanIDSetUid;            //!< CAN-message id of 'SetUid' message
    quint32 m_unCanIDReqUid;            //!< CAN-message id of 'ReqUid' message
    quint32 m_unCanIDRespUid;           //!< CAN-message id of 'RespUid' message
    quint32 m_unCanIDReadBlock;         //!< CAN-message id of 'ReadBlock' message
    quint32 m_unCanIDRespReadBlock;     //!< CAN-message id of 'RespReadBlock' message
    quint32 m_unCanIDWriteBlock;        //!< CAN-message id of 'WriteBlock' message
    quint32 m_unCanIDRespWriteBlock;    //!< CAN-message id of 'RespWriteBlock' message
    quint32 m_unCanIDLockBlock;         //!< CAN-message id of 'LockBlock' message
    quint32 m_unCanIDRespLockBlock;     //!< CAN-message id of 'RespLockBlock' message

    /*! RFID ISO/IEC 15693 command type definitions */
    typedef enum {
        FM_RFID_CMD_TYPE_UNDEF       = 0,   //!< Undefined command type
        FM_RFID_CMD_TYPE_SET_CONFIG  = 1,   //!< Set configuration (on/off)
        FM_RFID_CMD_TYPE_ACQUIRE_UID = 2,   //!< Acquire UID from RFID tag
        FM_RFID_CMD_TYPE_READ_BOCK   = 3,   //!< Read data block from tag
        FM_RFID_CMD_TYPE_WRITE_BOCK  = 4,   //!< Write data block to tag
        FM_RFID_CMD_TYPE_LOCK_BOCK   = 5    //!< Lock data block on tag
    } CANRFIDModuleCmdType_t;

    /*! module command data, used for internal data transfer*/
    typedef struct {
        CANRFIDModuleCmdType_t m_Type;          //!< Command type
        ModuleCmdState_t m_State;               //!< Command state
        Global::MonotonicTime m_ReqSendTime;    //!< Time the command was executed
        qint32 m_Timeout;                       //!< Timeout in ms
        quint8 m_Enabled;                       //!< Module enabled bit
        quint8 m_Address;                       //!< Block address
        quint32 m_Data;                         //!< Write data
    } ModuleCommand_t;

    //! Array of module commands for simultaneously execution
    ModuleCommand_t m_ModuleCommand[MAX_RFID_CMD_IDX];

    //! Set the module command type to free entry within array
    bool SetModuleTask(CANRFIDModuleCmdType_t CommandType, quint8* pCmdIndex = 0);
    //! Clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANRFIDModuleCmdType_t CommandType);
};

} //namespace

#endif /* DEVICECONTROL_RFID15693PRIVATE_H */
