/****************************************************************************/
/*! \file RackTransferDevice.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    03.09.2010
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CRackTransferDevice
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

#ifndef RACKTRANSFERDEVICE_H
#define RACKTRANSFERDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

class WrapperDeviceRackTransfer;

namespace DeviceControl
{

class DeviceProcessing;
class CStepperMotor;

#define RTFR_TASK_REFRUN   0x00  //!< reference run
#define RTFR_TASK_SET_POS  0x01  //!< set position
#define RTFR_TASK_REQ_POS  0x02   //!< request position

/****************************************************************************/
/*! \class CRackTransferDevice
 *
 *  \brief This class provides functions to configure and control the rack transfer
 *
 */
/****************************************************************************/
class CRackTransferDevice : public CBaseDevice
{
    friend class ::WrapperDeviceRackTransfer;
    Q_OBJECT

public:
    CRackTransferDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    virtual ~CRackTransferDevice();

    //!< general task handling function
    void HandleTasks();

    //!< Request reference run
    ReturnCode_t ReqReferenceRun();
    //!< position the adapter
    ReturnCode_t ReqAdapterMoveToPosition(RackAdapterPosition_t position);
    //!< Request the adapter position
    ReturnCode_t ReqActAdapterPosition();

    ReturnCode_t ClearErrorState();

private slots:
     //!< motor error notification
     void StepperMotorError(quint32, quint16, quint16, quint16, QDateTime);
     //!< motor set state acknowledge notification
     void StepperMotorSetStateAckn(quint32, ReturnCode_t);
     //!< motor reference run acknowledge notification
     void StepperMotorReferenceMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);
     //!< motor movement acknowledge notification
     void StepperMotorMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position, qint16 speed);
     //!< actual motor position notification
     void StepperMotorPosition(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);

private:
     //!< Handle substate initialisation
     ReturnCode_t HandleInitializationState();
     //!< Handle substate configuration
     ReturnCode_t HandleConfigurationState();
     //!< Configure device tasks
     ReturnCode_t ConfigureDeviceTasks();
     //!< idle state handling task
     void HandleIdleState();
     //!< command handling task
     void HandleDeviceTaskActions();
     //!< error handling task
     void HandleErrorState();

    //!< Return the adapter position type from motor position
    RackAdapterPosition_t GetAdapterPosFromMotorPos(qint32 motorPos);
    //!< Return the motor position from oven adapter position type
    qint32 GetMotorPosFromAdapterPos(RackAdapterPosition_t adapterPos);

    CStepperMotor* m_pMotorRacktransfer; ///< Adapter motor

    //!< Task definitions
    typedef enum {
        RACKTRANSFER_TASKID_INIT        = 0x00, //!<initialisation state
        RACKTRANSFER_TASKID_FREE        = 0x01, //!< idle state
        RACKTRANSFER_TASKID_ERROR       = 0x02, //!< error occurred
        RACKTRANSFER_TASKID_COMMAND_HDL = 0x03  //!< at least one command is active and must be handled
    } RackTransferDevTaskID_t;
    RackTransferDevTaskID_t m_TaskID;   //!< Task identifier

    bool ActivateDeviceTask(quint8 DeviceTaskIndex);
    CANStepperMotorTask* GetMotorFctModTaskInProcess(quint32 InstanceID,
                                                     CANStepperMotorTask* pMotorTaskPrev,
                                                     CANStepperMotorTask::CANStepperMotorTaskID_t FctTaskID);

    DeviceTaskMap m_DeviceTaskMap;   ///< device task map of this device

    ///< error task states
    typedef enum {
        RACKTRANSFER_DEV_ERRTASK_STATE_FREE            = 0x00,  ///< task state free, no error
        RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_IFACE    = 0x01,  ///< reports the error to the interface class
        RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_DEVPROC  = 0x02,  ///< reports the error to device processing
        RACKTRANSFER_DEV_ERRTASK_STATE_IDLE            = 0x03,  ///< error idle state, do nothing, block anything
        RACKTRANSFER_DEV_ERRTASK_STATE_RESET           = 0x04   ///< reset error state, change to initial state
    } RackTransferDevErrTaskState_t;

    RackTransferDevErrTaskState_t m_ErrorTaskState;

    ///<variables from process settings
    MotionProfileIdx_t m_SpeedMotionProfile;  ///< motor speed motion profile for rack transfer device
    Position_t m_LoadPosition;    ///< motor position at rack transfer device's load position
    Position_t m_TransferPosition;      ///< motor position at rack transfer device's transfer position

};

}  //namespace

#endif /* RACKTRANSFERDEVICE_H */
