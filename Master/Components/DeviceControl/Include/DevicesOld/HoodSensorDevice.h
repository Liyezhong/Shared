/****************************************************************************/
/*! \file HoodSensorDevice.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.08.2012
 *   $Author:  $ Stalin Govindarajan
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CHoodSensorDevice
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

#ifndef HOODSENSORDEVICE_H
#define HOODSENSORDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"

//class WrapperDeviceLoader;

namespace DeviceControl
{

class CDigitalInput;

#define HOOD_DEV_TASK_REQ_SENSOR_STATUS     0
#define HOOD_DEVICE_TASK_COUNT              1

/****************************************************************************/
/*! \class CHoodSensorDevice
 *
 *  \brief This is the class to sense/detect hood open event
 *         It contains the only one function module
 *         - digital input, connected to the sensor that detects hood open
 */
/****************************************************************************/
class CHoodSensorDevice : public CBaseDevice
{
    //friend class ::WrapperDeviceLoader;
    Q_OBJECT

public:
    CHoodSensorDevice(DeviceProcessing* pDeviceProcessing, QString Type, DevInstanceID_t instanceID);
    virtual ~CHoodSensorDevice();

    void HandleTasks();

    ReturnCode_t ReqHoodStatus();

private slots:
    //! Hood open/close-  sensor state changed
    void HoodStatusChanged(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 inputValue);

    //! Hood open/close- sensor error notification
    void HoodStatusError(quint32, quint16, quint16, quint16, QDateTime);

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

    CDigitalInput* m_pHoodSensor;    ///< pointer to hood sensor function module

    /*! error task states */
    typedef enum {
        HOOD_DEV_ERRTASK_STATE_FREE           = 0x00, ///< task state free, no error
        HOOD_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01, ///< reports the error to the interface class
        HOOD_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02, ///< reports the error to device processing
        HOOD_DEV_ERRTASK_STATE_IDLE           = 0x03, ///< error idle state, do nothing, block anything
        HOOD_DEV_ERRTASK_STATE_RESET          = 0x04  ///< reset error state, change to initial state
    } HoodDevErrTaskState_t;

    HoodDevErrTaskState_t m_ErrorTaskState;     ///< the error task state

    /*! Task definitions */
    typedef enum {
        HOODDEV_TASKID_INIT        = 0x00,    //! <initialisation state
        HOODDEV_TASKID_FREE        = 0x01,    //!< idle state
        HOODDEV_TASKID_ERROR       = 0x02,    //!< error occurred
        HOODDEV_TASKID_COMMAND_HDL = 0x03     //!< at least one device task is active and must be handled
    } HoodDevTaskID_t;
    HoodDevTaskID_t m_TaskID; ///< Task requested to the hood sensor

    //! Activate the device task and it's fct-mdoule tasks
    bool ActivateDeviceTask(quint8 DeviceTaskIndex);

    DeviceTask m_DeviceTask[HOOD_DEVICE_TASK_COUNT];  ///< device task control

    typedef enum {
        HOOD_CLOSED  = 0x00,
        HOOD_OPEN    = 0x01
    } HoodSensor_t;

    HoodState_t m_HoodState;

    // Variables from process settings
};

}  //namespace

#endif // CHOODSENSORDEVICE_H
