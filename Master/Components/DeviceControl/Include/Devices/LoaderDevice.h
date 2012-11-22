/****************************************************************************/
/*! \file LoaderDevice.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CLoaderDevice
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

#ifndef LOADERDEVICE_H
#define LOADERDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"

class WrapperDeviceLoader;

namespace DeviceControl
{

#define LOADER_RFID_MAX  5  ///< number of RFID channels

class CStepperMotor;
class CRfid11785;
class CDigitalInput;
class CDigitalOutput;

#define LOADER_DEV_TASK_REFRUN            0   //!< reference run
#define LOADER_DEV_TASK_DRAWER_POS_SET    1   //!< set drawer position
#define LOADER_DEV_TASK_DRAWER_ACTPOS_REQ 2   //!< request actual drawer position
#define LOADER_DEV_TASK_READ_RFID         3   //!< read RFID data
#define LOADER_DEV_TASK_READ_TAG          4   //!< read RFID data
#define LOADER_DEVICE_TASK_COUNT          5   ///< number of device tasks

/****************************************************************************/
/*! \class CLoaderDevice
 *
 *  \brief This is the class to control a loader
 *         A loader contains the following function modules
 *         - stepper motor control for the drawer's movement,
 *         - digital input, connected to the button to open and close the drawer,
 *         - two digital outputs, connected to the LEDs which signal the state of the loader (blocked - free)
 *         - five RFID channels to read the racks data
 */
/****************************************************************************/
class CLoaderDevice : public CBaseDevice
{
    friend class ::WrapperDeviceLoader;
    Q_OBJECT

public:
    CLoaderDevice(DeviceProcessing* pDeviceProcessing, QString Type, DevInstanceID_t instanceID);
    virtual ~CLoaderDevice();

    void HandleTasks();

    ReturnCode_t ReferenceRun();                                        ///< reference run of the drawler's motor
    ReturnCode_t SetDrawerPosition(LoaderPosition_t loaderPosition);    ///< move the drawler's motor
    ReturnCode_t ReqActDrawerPosition();                                ///< request the drawler's motor position
    ReturnCode_t SetBlockingState(BlockState_t blockState);             ///< blocks /unblocks the drawler's motor
    ReturnCode_t ReqBlockingState();                                    ///< requestblocking state of the drawler
    ReturnCode_t ReqRFIDData(LoaderRFIDChannel_t rfidChannelReq);       ///< request RFID data
    ReturnCode_t ReqRFIDTAG(LoaderRFIDChannel_t RfidChannelReq);        ///< request RFID TAG

    BlockState_t GetBlockingState();

private slots:
    //! Drawer open/close-button state changed
    void DigInputChanged(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 inputValue);
    //! Drawer open/close-button error notification
    void DigInputError(quint32, quint16, quint16, quint16, QDateTime);

    //! Drawer motor error notification
    void StepperMotorError(quint32, quint16, quint16, quint16, QDateTime);
    //! Motor reference run acknowledge notification
    void StepperMotorReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 position);
    //! Motor set state acknowledge notification
    void StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo);
    //! Motor movement acknowledge notification
    void StepperMotorMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 position, qint16 speed);
    //! Motor position notification
    void StepperMotorActPosition(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 position);

    void RFIDDataReceived(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 RFIDData);
    //! RFID reader error notification
    void RFIDError(quint32, quint16, quint16, quint16, QDateTime);

private:
    /// handle the initialisation state
    ReturnCode_t HandleInitializationState();
    /// handle the configuration state
    ReturnCode_t HandleConfigurationState();
    /// handle the device task  configuration
    ReturnCode_t ConfigureDeviceTasks();
    /// handle the idle state, and all tasks
    void HandleIdleState();
    /// handle the error state
    void HandleErrorState();
    /// device task handling
    void HandleDeviceTaskActions();

    /// handle the LED control
    void HandleLEDState();
    /// get the actual drawer postion
    LoaderPosition_t GetDrawerPositionFromMotorPos(Position_t);
    /// returns the RFID-module array index of the RFID module channel
    int GetRFIDIndexFromChannel(LoaderRFIDChannel_t rfidChannel);
    /// returns the RFID-module channel of the RFID module array index
    LoaderRFIDChannel_t GetChannelFromRFIDIndex(int index);

    /// returns the Motor function module with the requested task
    CANStepperMotorTask* GetMotorFctModTaskInProcess(quint32 InstanceID,
                                                     CANStepperMotorTask* pMotorTaskPrev,
                                                     CANStepperMotorTask::CANStepperMotorTaskID_t FctTaskID);

    CStepperMotor* m_pMotor;             ///< pointer to drawer motor function module
    CRfid11785* m_pRFID;                 ///< pointer to RFID function modules
    CDigitalInput* m_pDrawerButton;      ///< pointer to loader button function module
    CDigitalOutput* m_pDrawerLEDFree;    ///< pointer to loader status 'free' LED
    CDigitalOutput* m_pDrawerLEDBlocked; ///< pointer to loader status 'blocked' LED
    bool m_RFIDused;                            ///< usage flag for RFID module

    /*! Task definitions */
    typedef enum {
        LOADERDEV_TASKID_INIT        = 0x00,    //! <initialisation state
        LOADERDEV_TASKID_FREE        = 0x01,    //!< idle state
        LOADERDEV_TASKID_ERROR       = 0x02,    //!< error occurred
        LOADERDEV_TASKID_COMMAND_HDL = 0x03     //!< at least one device task is active and must be handled
    } LoaderDevTaskID_t;
    LoaderDevTaskID_t m_TaskID; ///< Task requested to the loader

    /*! Device command type definitions */
    typedef enum {
        LOADERDEV_CMD_TYPE_UNDEF     = 0x00,    //!< undefined command type
        LOADERDEV_CMD_REQ_REFRUN     = 0x01,    //!< reference run was requested
        LOADERDEV_CMD_SET_DRAWER_POS = 0x02,    //!< move to position was requested
        LOADERDEV_CMD_REQ_DRAWER_POS = 0x03,    //!< actual position was requested
        LOADERDEV_CMD_REQ_RFID_DATA  = 0x04     //!< actual rfid data was requested
    } LoaderDevCmdType_t;

    //! Activate the device task and it's fct-mdoule tasks
    bool ActivateDeviceTask(quint8 DeviceTaskIndex);

    DeviceTask m_DeviceTask[LOADER_DEVICE_TASK_COUNT];  ///< device task control

    /*! error task states */
    typedef enum {
        LOADER_DEV_ERRTASK_STATE_FREE           = 0x00, ///< task state free, no error
        LOADER_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01, ///< reports the error to the interface class
        LOADER_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02, ///< reports the error to device processing
        LOADER_DEV_ERRTASK_STATE_IDLE           = 0x03, ///< error idle state, do nothing, block anything
        LOADER_DEV_ERRTASK_STATE_RESET          = 0x04  ///< reset error state, change to initial state
    } LoaderDevErrTaskState_t;

    /*! general RFID channel communication task states */
    typedef enum {
        RFID_CHANNEL_STATE_FREE     = 0x00, ///< the rfid channel state is free, ready for action request
        RFID_CHANNEL_STATE_REQ_SEND = 0x01, ///< an action was requested, ( data request send or write operation, wait for response or acknowledge)
        RFID_CHANNEL_STATE_RECEIVED = 0x02, ///< The response or acknowledge was received
        RFID_CHANNEL_STATE_ERROR    = 0x03  ///< error
    } RFIDChannelState_t;

    LoaderDevErrTaskState_t m_ErrorTaskState;   ///< the error task state

    LoaderPosition_t   m_reqLoaderPosition; ///< requested drawer position
    LoaderPosition_t   m_ActLoaderPosition; ///< actual drawer position (as lastly received)
    quint32            m_rfidMSB[5];        ///< RFID data MSB
    quint32            m_rfidLSB[5];        ///< RFID data LSB
    RFIDChannelState_t m_rfidState[5];      ///< RFID module state: 0 - free, 1 - read req. send, 2 - data received, 3 - error
    ReturnCode_t       m_rfidHdlInfo[5];    ///< RFID module return values
    qint16             m_rfidErrorCode[5];  ///< RFID module return values

    LoaderRFIDChannel_t m_ReqRfidChannel;   ///< RFID channel to read from

    bool m_bLEDRefresh;                     ///< LED refresh detection flag

    BlockState_t m_blockState;              ///< drawer blocking state
    bool m_bAcknowledgeNeed;                ///< flag to control movement acknowledge

    // Variables from process settings
    MotionProfileIdx_t m_DrawerOpenSpeedMotionProfile;  ///< motor speed motion profile for drawer open
    MotionProfileIdx_t m_DrawerCloseSpeedMotionProfile; ///< motor speed motion profile for drawer close
    Position_t m_DrawerClosedPosition;  ///< motor position at drawer closed position
    Position_t m_DrawerOpenPosition;    ///< motor position at drawer open position
};

}  //namespace

#endif /* LOADERDEVICE_H */
