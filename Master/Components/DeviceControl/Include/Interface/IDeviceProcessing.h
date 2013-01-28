/****************************************************************************/
/*! \file IDeviceProcessing.h
 *
 *  \brief Definition file for class IDeviceProcessing
 *
 *   Version: $ 0.1
 *   Date:    $ 08.08.2012
 *   Author:  $ Norbert Wiedmann / Stalin
 *
 *
 *  \b Description:
 *
 *       This module contains the interface to the IDeviceProcessing class
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

#ifndef IDEVICEPROCESSING_H
#define IDEVICEPROCESSING_H

#include <QHash>
#include <QObject>

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/Rfid15693.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  This is the interface class of the device control layer.
 *
 *      The class provides methods to operate with the device control layer's
 *      functionality.
 */
/****************************************************************************/
class IDeviceProcessing : public QObject
{
    Q_OBJECT

public:
    IDeviceProcessing();
    ~IDeviceProcessing();

    //! Emergency stop
    void EmergencyStop();   // should be called if the device's cover was opened by the user
    //! Shutdown the device
    void Shutdown();        // should be called if the device's power failed shortly
    //! Switch to standby mode
    void Standby();         // should be called to change to standby mode
    //! Clean up the environment
    void Destroy();         // should be called to finish all the activities

    //! Start device control layer configuration
    ReturnCode_t StartConfigurationService();
    //! Restart device control layer configuration
    ReturnCode_t RestartConfigurationService();
    //! Start device control layer diagnstic service
    ReturnCode_t StartDiagnosticService();
    //! Finisch device control layer diagnostic service
    ReturnCode_t CloseDiagnosticService();

    //! Start adjustment service
    ReturnCode_t StartAdjustmentService();

    //! Returns Device derived class pointer specified by instanceID
    CDeviceBase* GetDevice(DevInstanceID_t InstanceID);
    //! Return the pointer to the CBaseModule which is next in list
    CBaseModule* GetNode(bool First);

    void SetAdjustmentList(DataManager::CAdjustment AdjustmentList);

signals:
    /****************************************************************************/
    /*!
     *  \brief  Forward the 'intitialisation finished' notification
     *
     *  \iparam ReturnCode = DCL_ERR_FCT_CALL_SUCCESS if successful,
     *                       otherwise an error code
     */
    /****************************************************************************/
    void ReportInitializationFinished(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*!
     *  \brief  Forward the 'configuration finished' notification
     *
     *  \iparam ReturnCode = DCL_ERR_FCT_CALL_SUCCESS if successful,
     *                       otherwise an error code
     */
    /****************************************************************************/
    void ReportConfigurationFinished(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*!
     *  \brief  Forward the 'normal operation mode started' notification
     *
     *  \iparam ReturnCode = DCL_ERR_FCT_CALL_SUCCESS if successful,
     *                       otherwise an error code
     */
    /****************************************************************************/
    void ReportStartNormalOperationMode(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*!
     *  \brief  Forward error information
     *
     *  \iparam instanceID = Source of the error
     *  \iparam usErrorGroup = Error group
     *  \iparam usErrorID = Error ID
     *  \iparam usErrorData = Error data
     *  \iparam timeStamp = Timestamp
     */
    /****************************************************************************/
    void ReportEvent(quint32 EventCode, quint16 EventData, QDateTime TimeStamp);

    /****************************************************************************/
    /*!
     *  \brief  Forward error information
     *
     *  \iparam instanceID = Source of the error
     *  \iparam usErrorGroup = Error group
     *  \iparam usErrorID = Error ID
     *  \iparam usErrorData = Error data
     *  \iparam timeStamp = Timestamp
     *  \iparam strErrorInfo = Error string
     */
    /****************************************************************************/
    void ReportEventWithInfo(quint32 EventCode, quint16 EventData, QDateTime TimeStamp, QString EventInfo);

    /****************************************************************************/
    /*!
     *  \brief  Reports that the device control layer is stopped
     */
    /****************************************************************************/
    void ReportDestroyFinished();

    /****************************************************************************/
    /*!
     *  \brief  Emitted when all devices should shut down
     */
    /****************************************************************************/
    void DeviceShutdown();

private slots:
    //! Task handling
    void HandleTasks();

    //! Get the 'intitialisation finished' notification
    void OnInitializationFinished(ReturnCode_t);
    //! Get the 'configuration finished' notification
    void OnConfigurationFinished(ReturnCode_t);
    //! Forward the 'normal operation mode started' notification
    void OnStartNormalOperationMode(ReturnCode_t);

    //! Get error information from DeviceProcessing
    void OnEvent(quint32 EventCode, quint16 EventData, QDateTime TimeStamp);
    //! Get error information from DeviceProcessing
    void OnEventWithInfo(quint32 EventCode, quint16 EventData, QDateTime TimeStamp, QString EventInfo);

    //! Device control layer diagnostic service acknwoledge
    void OnDiagnosticServiceClosed(ReturnCode_t DiagnosticResult);

    void OnDestroyFinished();

    void ThreadStarted();

private:
    //! Handle the state 'Task request pending'
    void HandleTaskRequestState();

    DeviceProcessing *mp_DevProc;       //!< Device processing instance
    QThread m_DevProcThread;           //!< Device processing thread
    QTimer m_DevProcTimer;             //!< Device processing timer

    //! Task identifiers
    typedef enum {
        IDEVPROC_TASKID_INIT     = 0x00,    //!< Initialisation
        IDEVPROC_TASKID_FREE     = 0x01,    //!< Task free, nothing to do
        IDEVPROC_TASKID_ERROR    = 0x02,    //!< Error state
        IDEVPROC_TASKID_REQ_TASK = 0x03     //!< A reqest is active
    } IDeviceProcessingTaskID_t;

    //! Task states
    typedef enum {
        IDEVPROC_TASK_STATE_FREE     = 0x00,    //!< Task state free, ready for action request
        IDEVPROC_TASK_STATE_REQ      = 0x01,    //!< An action was requested, next step will be to forward the command
        IDEVPROC_TASK_STATE_REQ_SEND = 0x02,    //!< Command was forwarded, wait for acknowledge, check timeout
        IDEVPROC_TASK_STATE_ERROR    = 0x03     //!< Error, e.g. timeout while waiting for acknowledge, or error CAN-message received
    } IDeviceProcessingTaskState_t;

    IDeviceProcessingTaskID_t m_taskID;         //!< Task identifier
    IDeviceProcessingTaskState_t m_taskState;   //!< Task state

    DeviceProcTask::TaskID_t m_reqTaskID;           //!< Task identification
    DeviceProcTask::TaskPrio_t m_reqTaskPriority;   //!< Task priority
    quint16 m_reqTaskParameter1;    //!< Task parameter 1
    quint16 m_reqTaskParameter2;    //!< Task parameter 2

    QMutex m_IMutex;    //!< Handles thread safety of IDeviceProcessing
    QMutex m_Mutex;     //!< Handles thread safety of DeviceProcessing
};

} //namespace

#endif /* IDEVICEPROCESSING_H */
