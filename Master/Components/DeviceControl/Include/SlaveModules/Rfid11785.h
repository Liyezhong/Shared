/******************************************************************/
/*! \file Rfid11785.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CRfid11785
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

#ifndef DEVICECONTROL_RFID11785PRIVATE_H
#define DEVICECONTROL_RFID11785PRIVATE_H

#include "Global/Include/MonotonicTime.h"
#include "DeviceControl/Include/SlaveModules/FunctionModule.h"

namespace DeviceControl
{

class CANCommunicator;

#define MAX_RFID_CMD_IDX 2  ///< up to 2 module commands can be handled simultaneously

/****************************************************************************/
/*!
 *  \brief  This class implements the functionality to configure and control
 *          a slave's 'RFID_11785' function module
 */
/****************************************************************************/
class CRfid11785 : public CFunctionModule
{
    Q_OBJECT

public:
    CRfid11785(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
               CBaseModule* pParentNode);
    ~CRfid11785();

    //! Module initialisation
    ReturnCode_t Initialize();

    //! Task handling function
    void HandleTasks();
    //! CAN-message handling function
    void HandleCanMessage(can_frame* pCANframe);

    //! Set RFID module state and channel
    ReturnCode_t SetState(bool Enabled, quint8 Antenna);

    //! Set RFID login password
    ReturnCode_t SetLogin(quint32 Password);
    //! Request RFID unique identification number
    ReturnCode_t ReqUID();
    //! Request RFID user data
    ReturnCode_t ReqUserData(quint8 Address);
    //! Write RFID user data
    ReturnCode_t WriteUserData(quint8 Address, quint32 Data);
    //! Write RFID password
    ReturnCode_t WritePassword(quint32 Password);
    //! Write RFID configuration
    ReturnCode_t WriteConfiguration(bool ReadLogin, bool WriteLogin, bool ReaderTalkFirst);

    signals:
    /****************************************************************************/
    /*!
     *  \brief   This signal is emitted to forward the state setting acknowledge
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief   This signal is emitted to forward the login acknowledge
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportLoginAckn(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief   This signal is emitted to report the RFID tag's UID
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   UID        = Unique identification number
     */
    /****************************************************************************/
    void ReportUID(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 UID);

    /****************************************************************************/
    /*!
     *  \brief   This signal is emitted to report the RFID tag's user data
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   Address    = Target address of the read operation
     *  \param   UserData   = User data from RFID tag
     */
    /****************************************************************************/
    void ReportUserData(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Address, quint32 UserData);

    /****************************************************************************/
    /*!
     *  \brief   This signal is emitted to report data is written to the tag
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   Address    = Target address of the write operation
     */
    /****************************************************************************/
    void ReportWriteData(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Address);

    /****************************************************************************/
    /*!
     *  \brief   Report that the password is written to the tag.
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportWritePassword(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief   Report that the configuration is written to the tag.
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportWriteConfiguration(quint32 InstanceID, ReturnCode_t HdlInfo);

    private:
    //! CAN message ID initialization
    ReturnCode_t InitializeCANMessages();
    //! registers the CAN messages to communication layer
    ReturnCode_t RegisterCANMessages();

    //! Idle taks handling function
    void HandleIdleState();

    //! sends the configuration CAN messages
    ReturnCode_t SendCANMessageConfiguration();
    //! send the 'Login' CAN messages
    ReturnCode_t SendCANMsgLogin(quint32 Password);
    //! send the 'ReadUserData' CAN messages
    ReturnCode_t SendCANMsgReadUserData(quint8 Address);
    //! send the 'WriteUserData' CAN messages
    ReturnCode_t SendCANMsgWriteUserData(quint8 Address, quint32 Data);
    //! send the 'ReadUID' CAN messages
    ReturnCode_t SendCANMsgReadUID();
    //! send the 'WritePassword' CAN messages
    ReturnCode_t SendCANMsgWritePassword(quint32 Password);
    //! send the 'WriteConfig' CAN messages
    ReturnCode_t SendCANMsgWriteConfig(bool ReadLogin, bool WriteLogin, bool ReaderTalkFirst);

    //! handles the receipt of can message 'SetConfiguration'
    void HandleCANMsgLoginAckn(can_frame* pCANframe, ReturnCode_t hdlInfo);
    //! handles the receipt of can message 'UserData'
    void HandleCANMsgUserData(can_frame* pCANframe, ReturnCode_t hdlInfo);
    //! handles the receipt of can message 'WriteDataAckn'
    void HandleCANMsgWriteData(can_frame* pCANframe, ReturnCode_t hdlInfo);
    //! handles the receipt of can message 'UID'
    void HandleCANMsgUID(can_frame* pCANframe, ReturnCode_t hdlInfo);
    //! handles the receipt of can message 'WritePasswordAckn'
    void HandleCANMsgWritePassword(can_frame* pCANframe, ReturnCode_t hdlInfo);
    //! handles the receipt of can message 'WriteConfigAckn'
    void HandleCANMsgWriteConfig(can_frame* pCANframe, ReturnCode_t hdlInfo);

    //! command handling function
    void HandleCommandRequestTask();

    quint32 m_unCanIDConfig;               //!< CAN-message id of 'SetConfiguration' message
    quint32 m_unCanIDConfigAckn;           //!< CAN-message id of 'SetConfigurationackn' message
    quint32 m_unCanIDLogin;                //!< CAN-message id of 'Login' message
    quint32 m_unCanIDLoginAckn;            //!< CAN-message id of 'LoginAckn' message
    quint32 m_unCanIDWriteUserData;        //!< CAN-message id of 'WriteUserData' message
    quint32 m_unCanIDWriteUserDataAckn;    //!< CAN-message id of 'WriteUserDataAckn' message
    quint32 m_unCanIDWritePassword;        //!< CAN-message id of 'WritePassword' message
    quint32 m_unCanIDWritePasswordAckn;    //!< CAN-message id of 'WritePasswordAckn' message
    quint32 m_unCanIDWriteConfig;          //!< CAN-message id of 'WriteConfig' message
    quint32 m_unCanIDWriteConfigAckn;      //!< CAN-message id of 'WriteConfigAckn' message
    quint32 m_unCanIDUserDataReq;          //!< CAN-message id of 'ReadUserData' message
    quint32 m_unCanIDUserData;             //!< CAN-message id of 'UserData' message
    quint32 m_unCanIDUIDReq;               //!< CAN-message id of 'ReadUID' message
    quint32 m_unCanIDUID;                  //!< CAN-message id of 'UID' message

    /*! digital output command type definitions */
    typedef enum {
        FM_RFID_CMD_TYPE_UNDEF           = 0,   //!< undefined command type
        FM_RFID_CMD_TYPE_SET_CONFIG      = 1,   //!< set configuration (on/off, antenna)
        FM_RFID_CMD_TYPE_LOGIN           = 2,   //!< login
        FM_RFID_CMD_TYPE_USER_DATA_REQ   = 3,   //!< read user data
        FM_RFID_CMD_TYPE_USER_DATA_WRITE = 4,   //!< write user data
        FM_RFID_CMD_TYPE_PASSWORD_WRITE  = 5,   //!< write password
        FM_RFID_CMD_TYPE_CONGIG_WRITE    = 6,   //!< write configuration
        FM_RFID_CMD_TYPE_UID_REQ         = 7    //!< read UID
    } CANRFIDModuleCmdType_t;

    /*! module command data, used for internal data transfer*/
    typedef struct {
        CANRFIDModuleCmdType_t Type;        //!< command type
        ModuleCmdState_t State;             //!< command state
        Global::MonotonicTime ReqSendTime;  //!< time the command was executed
        qint32 Timeout;                     //!< timeout in ms
        quint32 Password;                   //!< RFID transponder password
        quint8 UserDataAddress;             //!< RFID Userdata address
        quint32 UserData;                   //!< RFID Userdata for write operation
        bool ReadLogin;                     //!< Read login bit of the tag
        bool WriteLogin;                    //!< Write login bit of the tag
        bool ReaderTalkFirst;               //!< RTF bit of the tag
    } ModuleCommand_t;

    //! array of module commands for simultaneously execution
    ModuleCommand_t m_ModuleCommand[MAX_RFID_CMD_IDX];

    //! set the module command type to free entry within array
    bool SetModuleTask(CANRFIDModuleCmdType_t CommandType, quint8* pCmdIndex = 0);
    //! clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANRFIDModuleCmdType_t CommandType);

    /*! configuration state parameter definition, this variables buffer the
     *  data need by the 'SetConfiguration' CAN-message */
    typedef struct {
        quint8 Enabled;     //!< Module enabled bit
        quint8 Antenna;     //!< Antenna selection
        quint8 DataRate;    //!< Data rate (00 = RF/64, 01 = RF/32, 10 = RF/16, 11: RF/8)
    } ConfigurationState_t;

    ConfigurationState_t m_ConfigurationState; //!< configuration state
};

} //namespace

#endif /* DEVICECONTROL_RFID11785PRIVATE_H */
