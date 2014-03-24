/****************************************************************************/
/*! \file BaseDevice.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    11.01.2011
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CBaseDevice
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

#ifndef BASEDEVICE_H
#define BASEDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/ModuleConfig.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"

namespace DeviceControl
{
typedef QList<CBaseModule *> ListBaseModule;
/****************************************************************************/
/*!
 *  \brief This is the base class of the device classes
 *
 *      The class provides device relevant base functions
 */
/****************************************************************************/
class CBaseDevice : public QObject
{
    Q_OBJECT

public:
    CBaseDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    virtual ~CBaseDevice();

    /*****************************************************************************/
    /*!
     *  \brief  Task handling function
     */
    /*****************************************************************************/
    virtual void HandleTasks() = 0;

    /*! main state */
    typedef enum {
        DEVICE_MAIN_STATE_START        = 0x00,  ///< first state after creation
        DEVICE_MAIN_STATE_INIT         = 0x01,  ///< init state, read the assigned function modules
        DEVICE_MAIN_STATE_CONFIG       = 0x02,  ///< configuration, connect the signals
        DEVICE_MAIN_STATE_DEV_TASK_CFG = 0x03,  ///< device tasks configuration state
        DEVICE_MAIN_STATE_FCT_MOD_CFG  = 0x04,  ///< fuction module configuration state, e.g. force reference run
        DEVICE_MAIN_STATE_IDLE         = 0x05,  ///< idle state, ready for use
        DEVICE_MAIN_STATE_ERROR        = 0x06   ///< error state, usage blocked
    } DeviceMainState_t;

    /*****************************************************************************/
    /*!
     *  \brief  Returns the main state of the device
     *
     *  \return Main state
     */
    /*****************************************************************************/
    DeviceMainState_t GetMainState() const { return m_MainState; }

    /*****************************************************************************/
    /*!
     *  \brief  Returns the instance identifier
     *
     *  \return Instance identifier
     */
    /*****************************************************************************/
    quint32 GetInstanceID() const { return m_instanceID; }

    /*****************************************************************************/
    /*!
     *  \brief  Returns the device type string
     *
     *  \return Type string
     */
    /*****************************************************************************/
    QString GetType() const { return m_Type; }

    /*****************************************************************************/
    /*!
     *  \brief  Set the device type string
     *
     *  \iparam Key = Device type string
     */
    /*****************************************************************************/
    void SetTypeKey(const QString &Key) { m_TypeKey = Key; }

    /*****************************************************************************/
    /*!
     *  \brief  Set the function module list member
     *
     *  \iparam FctModList = Function module list
     */
    /*****************************************************************************/
    void SetFunctionModuleList(const DeviceFctModList& FctModList) { m_FctModList = FctModList; }

    /*****************************************************************************/
    /*!
     *  \brief  Get the device processing member
     *
     *  \return Function module list
     */
    /*****************************************************************************/
    DeviceProcessing* GetDeviceProcessing() const { return m_pDevProc; }

    quint32 GetFctModInstanceFromKey(const QString &Key);

    //! Get Function module key from instance ID
    QString GetFctModKeyFromInstance(const quint32 instanceID);

    ListBaseModule GetBaseModuleList() const {return m_BaseModuleList;}
    bool InsertBaseModule(CBaseModule* pBase);
    CBaseModule* GetCANNodeFromID(quint16 CANNodeID);
    quint16 GetBaseModuleVoltage(quint16 CANNodeID);
    quint16 GetBaseModuleCurrent(quint16 CANNodeID);

    static CANStepperMotorTask*    GetNewCANStepperMotorTask(CANStepperMotorTask::CANStepperMotorTaskID_t MotorTaskID);
    static CANRFIDTask*            GetNewCANRFIDTask(CANRFIDTask::CANRFIDTaskID_t RFIDTaskID);
    static CANDigitalOutputTask*   GetNewCANDigitalOutputTask(CANDigitalOutputTask::CANDigitalOutputTaskID_t DigOutpTaskID);
    static CANDigitalInputTask*    GetNewCANDigitalInputTask(CANDigitalInputTask::CANDigitalInputTaskID_t DigInpTaskID);
    static CANTemperatureCtrlTask* GetNewCANTempCtrlTask(CANTemperatureCtrlTask::CANTempCtrlTaskID_t TempCtrlTaskID);

    static DeviceTask* GetNewDeviceTask(DeviceTask::DeviceTaskState_t TaskState, quint16 Key);

public slots:
    void OnFunctionModuleError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime);
    void OnReportVoltageState(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 Voltage);
    void OnReportCurrentState(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 Current);
protected:
    /// Compact function to set the error parameter and error time by one code line
    void SetErrorParameter(quint16 errorGroup, quint16 errorCode, quint16 errorData);

    MotionProfileIdx_t GetProcSettingMotionProfile(QString Key);
    Position_t         GetProcSettingPosition(QString Key);

    DeviceProcessing*   m_pDevProc;         ///< pointer to the device processing class

    DeviceFctModList    m_FctModList;       ///< contains function modules idetifiers

    DeviceMainState_t   m_MainState;        ///< main state
    DeviceMainState_t   m_MainStateOld;     ///< previous main state, used for logging

    QString             m_Type;             ///< Device type string, used for logging

    ReturnCode_t m_lastErrorHdlInfo;        ///< return value after a failed function call
    quint16      m_lastErrorGroup;          ///< error group of the error causing functionality
    quint16      m_lastErrorCode;           ///< error code of the error causing functionality
    quint16      m_lastErrorData;           ///< additional error data, filled by the error causing functionality
    QDateTime    m_lastErrorTime;           ///< time of error detection

    quint32 m_instanceID;           ///< identifier for instance identification

    Global::MonotonicTime m_stateTimer;     ///< timer for timeout observation
    qint16 m_stateTimespan;                 ///< max. time delay of current active timeout observation

    QString      m_TypeKey;                 ///< key, used for process settings access
    ListBaseModule m_BaseModuleList;        /// The list contain all the base modules.
    quint16 m_BaseModuleVoltage;            /// The base module's actual voltage
    quint16 m_BaseModuleCurrent;            /// The base module's actual current
    qint64 m_LastSensorCheckTime;           /// The last check sensor's time(in sec since Epoch)
};

} //namespace

#endif // BASEDEVICE_H
