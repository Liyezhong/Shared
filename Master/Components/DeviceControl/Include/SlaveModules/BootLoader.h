/****************************************************************************/
/*! \file BootLoader.h
 *
 *  \brief  Definition file for class CBootLoader.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 09.05.2012
 *   $Author:  $ M.Scherer
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CBootLoader
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

#ifndef DEVICECONTROL_BOOTLOADER_H
#define DEVICECONTROL_BOOTLOADER_H

#include "DeviceControl/Include/CanCommunication/CANCommunicator.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include <QFile>
#include <QThread>
#include <QTimer>

namespace DeviceControl
{

class CBaseModule;
class CANMessageConfiguration;

/****************************************************************************/
/*!
 *  \brief  Controls the boot loader of the Slaves
 *
 *      This class sends and receives messages to and from the boot loader of
 *      the Slaves. It is able to detect the need for a firmware update and
 *      to transmit a firmware image to the Slave. This module is always part
 *      of CBaseModule. I cannot be used without a valid base module.
 */
/****************************************************************************/
class CBootLoader : public QObject
{
    Q_OBJECT

public:   
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CBootLoader
     *
     *  \param p_CanMsgConfig
     *  \param CanNodeId
     *  \param p_CanCommunicator
     *  \param p_BaseModule
     *
     *  \return from CBootLoader
     */
    /****************************************************************************/
    CBootLoader(const DeviceControl::CANMessageConfiguration *p_CanMsgConfig, const quint32 CanNodeId,
                DeviceControl::CANCommunicator *p_CanCommunicator, DeviceControl::CBaseModule *p_BaseModule);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateFirmware
     *
     *  \param FirmwarePath =  QString type parameter
     *
     *  \return from UpdateFirmware
     */
    /****************************************************************************/
    ReturnCode_t UpdateFirmware(const QString &FirmwarePath);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateInfo
     *
     *  \param p_Info =  quint8 type parameter
     *  \param Size =  quint32 type parameter
     *  \param UpdateType =  quint8 type parameter
     *
     *  \return from UpdateInfo
     */
    /****************************************************************************/
    ReturnCode_t UpdateInfo(const quint8 *p_Info, quint32 Size, quint8 UpdateType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateBootLoader
     *
     *  \param BootLoaderPath =  QString type parameter
     *
     *  \return from UpdateBootLoader
     */
    /****************************************************************************/
    ReturnCode_t UpdateBootLoader(const QString &BootLoaderPath);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCanMessage
     *
     *  \param p_CanFrame =  can_frame type parameter
     *
     *  \return from HandleCanMessage
     */
    /****************************************************************************/
    void HandleCanMessage(const can_frame *p_CanFrame);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function WaitForUpdate
     *
     *  \param Wait = bool type parameter
     *
     *  \return from WaitForUpdate
     */
    /****************************************************************************/
    static void WaitForUpdate(bool Wait);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function BootFirmware
     *
     *  \return from BootFirmware
     */
    /****************************************************************************/
    ReturnCode_t BootFirmware();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Active
     *
     *  \return from Active
     */
    /****************************************************************************/
    bool Active() const;

    //! Internal states of the boot loader module
    typedef enum {
        BOOTLOADER_IDLE,        //!< Default or init state
        BOOTLOADER_ACTIVE,      //!< The boot loader is running
        BOOTLOADER_FIRMWARE,    //!< Firmware update active
        BOOTLOADER_INFO,        //!< Info block update active
        BOOTLOADER_BOOTLOADER   //!< Boot loader update active
    } State_t;

signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report the end of a firmware update
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the
     *                    error code
     */
    /****************************************************************************/
    void ReportUpdateFirmware(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report the end of a info block update
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the
     *                    error code
     */
    /****************************************************************************/
    void ReportUpdateInfo(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report the end of a boot loader update
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the
     *                    error code
     */
    /****************************************************************************/
    void ReportUpdateBootLoader(quint32 InstanceID, ReturnCode_t HdlInfo);

private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendData
     *
     *  \return from SendData
     */
    /****************************************************************************/
    ReturnCode_t SendData();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendModeRequest
     *
     *  \param StartUpdate = bool type parameter
     *
     *  \return from SendModeRequest
     */
    /****************************************************************************/
    ReturnCode_t SendModeRequest(bool StartUpdate);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendInfo
     *
     *  \return from SendInfo
     */
    /****************************************************************************/
    ReturnCode_t SendInfo();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendHeader
     *
     *  \return from SendHeader
     */
    /****************************************************************************/
    ReturnCode_t SendHeader();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CalculateCrc
     *
     *  \param p_Data =  quint8 type parameter
     *  \param DataSize =  quint32 type parameter
     *
     *  \return from CalculateCrc
     */
    /****************************************************************************/
    quint32 CalculateCrc(const quint8 *p_Data, quint32 DataSize);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCanMsgUpdateRequired
     *
     *  \param p_CanFrame =  can_frame type parameter
     *
     *  \return from HandleCanMsgUpdateRequired
     */
    /****************************************************************************/
    ReturnCode_t HandleCanMsgUpdateRequired(const can_frame *p_CanFrame);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCanMsgUpdateModeAck
     *
     *  \param p_CanFrame =  can_frame type parameter
     *
     *  \return from HandleCanMsgUpdateModeAck
     */
    /****************************************************************************/
    ReturnCode_t HandleCanMsgUpdateModeAck(const can_frame *p_CanFrame);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCanMsgUpdateHeaderAck
     *
     *  \param p_CanFrame =  can_frame type parameter
     *
     *  \return from HandleCanMsgUpdateHeaderAck
     */
    /****************************************************************************/
    ReturnCode_t HandleCanMsgUpdateHeaderAck(const can_frame *p_CanFrame);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCanMsgUpdateAck
     *
     *  \param p_CanFrame =  can_frame type parameter
     *
     *  \return from HandleCanMsgUpdateAck
     */
    /****************************************************************************/
    ReturnCode_t HandleCanMsgUpdateAck(const can_frame *p_CanFrame);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCanMsgUpdateTrailerAck
     *
     *  \param p_CanFrame =  can_frame type parameter
     *
     *  \return from HandleCanMsgUpdateTrailerAck
     */
    /****************************************************************************/
    ReturnCode_t HandleCanMsgUpdateTrailerAck(const can_frame *p_CanFrame);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCanMsgUpdateInfoInitAck
     *
     *  \param p_CanFrame =  can_frame type parameter
     *
     *  \return from HandleCanMsgUpdateInfoInitAck
     */
    /****************************************************************************/
    ReturnCode_t HandleCanMsgUpdateInfoInitAck(const can_frame *p_CanFrame);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCanMsgUpdateInfoAck
     *
     *  \param p_CanFrame =  can_frame type parameter
     *
     *  \return from HandleCanMsgUpdateInfoAck
     */
    /****************************************************************************/
    ReturnCode_t HandleCanMsgUpdateInfoAck(const can_frame *p_CanFrame);

    quint32 m_CanIdUpdateRequired;      //!< CAN-ID 'UpdateRequired'
    quint32 m_CanIdUpdateModeRequest;   //!< CAN-ID 'UpdateModeRequest'
    quint32 m_CanIdUpdateModeAck;       //!< CAN-ID 'UpdateModeAck'
    quint32 m_CanIdUpdateHeader;        //!< CAN-ID 'UpdateHeader'
    quint32 m_CanIdUpdateHeaderAck;     //!< CAN-ID 'UpdateHeaderAck'
    quint32 m_CanIdUpdateData0;         //!< CAN-ID 'UpdateData0'
    quint32 m_CanIdUpdateAck0;          //!< CAN-ID 'UpdateAck0'
    quint32 m_CanIdUpdateData1;         //!< CAN-ID 'UpdateData1'
    quint32 m_CanIdUpdateAck1;          //!< CAN-ID 'UpdateAck1'
    quint32 m_CanIdUpdateTrailer;       //!< CAN-ID 'UpdateTrailer'
    quint32 m_CanIdUpdateTrailerAck;    //!< CAN-ID 'UpdateTrailerAck'
    quint32 m_CanIdUpdateInfoInit;      //!< CAN-ID 'UpdateInfoInit'
    quint32 m_CanIdUpdateInfoInitAck;   //!< CAN-ID 'UpdateInfoInitAck'
    quint32 m_CanIdUpdateInfo;          //!< CAN-ID 'UpdateInfo'
    quint32 m_CanIdUpdateInfoAck;       //!< CAN-ID 'UpdateInfoAck'

    DeviceControl::CANCommunicator *mp_CanCommunicator; //!< Communicator object
    CBaseModule *mp_BaseModule;  //!< Base module assigned to the boot loader
    QFile m_FirmwareImage;  //!< Firmware image file to be programmed
    bool m_UpdateRequired;  //!< Indicates if an update is required or not
    static bool m_WaitForUpdate;   //!< Set to wait for an update
    quint8 m_UpdateType;    //!< Type of updated info (BoardInfo/BootInfo/BoardOptions)
    quint8 *mp_Info;        //!< Pointer to info blocks (BoardInfo/BootInfo/BoardOptions)
    quint32 m_InfoSize;     //!< Number of info block bytes
    State_t m_State;        //!< The current state of this class
    qint32 m_Count;         //!< Number of transmitted bytes acknowledged
    QTimer m_Timer;         //!< Generates a transaction timeout
    QMutex m_Mutex;         //!< Protects handle CAN message function

    static const qint32 m_Timeout;              //!< Transaction timeout
    static const quint32 m_Crc32Polynomial;     //!< CRC32 polynomial
    static const quint32 m_Crc32InitialValue;   //!< CRC32 start value

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot TimeoutHandler
     */
    /****************************************************************************/
    void TimeoutHandler();
};

} //namespace DeviceControl

#endif // DEVICECONTROL_BOOTLOADER_H
