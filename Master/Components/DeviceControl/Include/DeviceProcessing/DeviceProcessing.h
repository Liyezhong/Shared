/******************************************************************/
/*! \file DeviceProcessing.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the definition of the DeviceProcessing class
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

#ifndef DEVICEPROCESSING_H
#define DEVICEPROCESSING_H

#include <sys/timeb.h> // used for time measurement

#include <QMutex>
#include <QReadWriteLock>
#include <QWaitCondition>
#include <QList>
#include <QHash>
#include <QDomDocument>
#include <QFile>
#include <QTimer>

#include "DeviceControl/Include/CanCommunication/CANCommunicator.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcTask.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QEventLoop>
namespace DeviceControl
{

//! Process settings list type definition
typedef QHash<QString, QString> ProcessSettingsTable;
//! Object tree type definition
typedef QList<CBaseModule *> ObjectTree;

class CConfigurationService;
class CDiagnosticService;
class CAdjustmentService;
class CShutdownService;

class CANMessageConfiguration;
class ObjectTask;

/// \todo remove to configuration service?
class CBaseDevice;
class CFunctionModuleTaskManager;

//! List of tasks used by device processing
typedef QList<DeviceProcTask*> ListDeviceProcTask;

//! List of devices used by device processing
typedef QList<CBaseDevice*> ListDevice;

/****************************************************************************/
/*!
 *  \brief  This is the central class of the device control layer.
 *
 *      This class is the central class of the device control layer. The
 *      class provides the central handle task function and functions for all
 *      the stuff needed to control the devices. The class holds the object
 *      tree, which is a list of all class instances needed by the program to
 *      communicate with the firmware in slave boards. Examples for this
 *      classes are CBaseModule, CStepperMotor or CRfid11785. The
 *      DeviceProcessing class also starts the CAN bus communication.
 */
/****************************************************************************/
class DeviceProcessing : public QObject
{
    Q_OBJECT

public:
    DeviceProcessing(QObject *p_Parent);
    virtual ~DeviceProcessing();

    void ThrowError(DevInstanceID_t InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime ErrorTime);
    void ThrowError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime ErrorTime);
    void ThrowErrorWithInfo(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime ErrorTime, QString ErrorInfo);

    //! Returns the serial number from config file
    static bool GetSerialNumber(QString& SerialNo);

    //! Return the hardware config file name
    static QString GetHWConfigFile() { return m_HWConfigFileName; }

    ReturnCode_t BlockingForSyncCall(SyncCmdType_t CmdType);
    ReturnCode_t BlockingForSyncCall(SyncCmdType_t CmdType, ulong Timeout);
    void ResumeFromSyncCall(SyncCmdType_t CmdType, qint32 Value);
    //! Main state typde definition
    typedef enum {
        DP_MAIN_STATE_START           = 0x00,   /**< start state, after instantiation of the class */
        DP_MAIN_STATE_INIT            = 0x01,   /**< init state */
        DP_MAIN_STATE_INTERNAL_CONFIG = 0x02,   /**< internal config. state, reading all configuration files */
        DP_MAIN_STATE_WAIT_FOR_CONFIG = 0x03,   /**< wait for activation of configuration service */
        DP_MAIN_STATE_CONFIG          = 0x04,   /**< config. state, the configuration service runs */
        DP_MAIN_STATE_WAIT_FOR_ADJUST = 0x05,   /**< wait for activation of adjustment service */
        DP_MAIN_STATE_ADJUST          = 0x06,   /**< adjustment state, adjustment service runs */
        DP_MAIN_STATE_IDLE            = 0x07,   /**< idle state, ready for work, e.g. normal operation service, diag. service, shutdown.. */
        DP_MAIN_STATE_ERROR           = 0x08    /**< error state, serious harm appeared */
    } DeviceProcessingMainState_t;

    /*****************************************************************************/
    /*!
     *  \brief  Return the main state
     *
     *  \return Actual main state
     */
    /*****************************************************************************/
    DeviceProcessingMainState_t GetState() const { return m_MainState; }

    //!< return the CAN message configuration
    CANMessageConfiguration* GetMessageConfig();

    //virtual function derived from CANObjectBase
    //! Initialisation
    void Initialize();
    //! Task handling
    void HandleTasks();

    //! Access to CAN nodes stored at the object tree
    CBaseModule* GetCANNodeFromObjectTree(bool First);
    //! Insert a CANNode into the object tree
    void InsertCANNodeToObjectTree(CBaseModule* pCANNode);

    /*****************************************************************************/
    /*!
     *  \brief  Insert a device into the internal list
     *
     *  \iparam pBaseDevice = Pointer to device
     */
    /*****************************************************************************/
    void AddDevice(CBaseDevice* pBaseDevice) { m_DeviceList.insert(m_DeviceList.size(), pBaseDevice); }

    //! Returns a device from the internal list
    CBaseDevice* GetDevice(DevInstanceID_t InstanceID);

    //! Active the specified task
    ReturnCode_t ActivateTask(DeviceProcTask::TaskID_t taskID, quint16 taskParameter1, quint16 taskParameter2);

    /*****************************************************************************/
    /*!
     *  \brief  Set the hardware decription  DomDocument
     *
     *  \iparam DocHwDescr = Hardware decription as DomDocument
     */
    /*****************************************************************************/
    void SetXML_HWDescription(const QDomDocument &DocHwDescr) { m_DocHwDescr = DocHwDescr; }

    //! Output the actual object tree content
    void LogObjectTree(int nParameter);

    //! Return function module specified by it's instance identifier
    CFunctionModule* GetFunctionModule(quint32 InstanceID) const;

    //! Returns if the function module has been detetcted and configured via CAN
    bool CheckFunctionModuleExistence(quint32 InstanceID);

    /*****************************************************************************/
    /*!
     *  \brief  Returns the function module task manager
     *
     *  \return Pointer to function module task manager
     */
    /*****************************************************************************/
    CFunctionModuleTaskManager* GetFunctionModuleTaskManager() { return m_pFunctionModuleTaskManager; }

    //! Return the specified process setting parameter
    quint8  GetProcSettingUInt8(QString Key);
    //! Return the specified process setting parameter
    quint16 GetProcSettingUInt16(QString Key);
    //! Return the specified process setting parameter
    quint32 GetProcSettingUInt32(QString Key);
    //! Return the specified process setting parameter
    QString GetProcSettingString(QString Key);
    //! Return the specified process setting parameter
    Position_t GetProcSettingPosition(QString Key);
    //! Return the specified process setting parameter
    MotionProfileIdx_t GetProcSettingMotionProfileIdx(QString Key);

signals:
    //! Forward the 'intitialisation finished' notification
    void ReportInitializationFinished(ReturnCode_t);
    //! Forward the 'configuration finished' notification
    void ReportConfigurationFinished(ReturnCode_t);
    //! Forward the 'normal operation mode started' notification
    void ReportStartNormalOperationMode(ReturnCode_t);

    //! Forward error information to IDeviceProcessing
    void ReportError(DevInstanceID_t instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp);
    //! Forward error information to IDeviceProcessing
    void ReportErrorWithInfo(DevInstanceID_t instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo);

    //! Forward the 'Diagnostics service closed' to IDeviceProcessing
    void ReportDiagnosticServiceClosed(qint16 DiagnosticResult);

    //! Forward the 'Destroy finished' to IDeviceProcessing
    void ReportDestroyFinished();

private slots:
    //! Slot fucntion used to receive CAN message
    void ReceiveCANMessage(quint32 ID, quint8 data0, quint8 data1, quint8 data2, quint8 data3, quint8 data4, quint8 data5, quint8 data6, quint8 data7, quint8 dlc);

#ifdef HAL_CV_TEST
    //! Callback function for the timer used in eventloop
    void BlockingTimerCallback();
#endif

private:
    DeviceProcessing(const DeviceProcessing &);                     ///< Not implemented.
    const DeviceProcessing & operator = (const DeviceProcessing &); ///< Not implemented.

    //! Read the hardware configuration file
    ReturnCode_t ReadHWSettings();

    //! Set priority of the calling thread context
    ReturnCode_t SetPriority();
    //! Initialize the communication via CAN or tcp/ip
    ReturnCode_t InitCommunication();

    void SetErrorParameter(quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData);
    ReturnCode_t ReadProcessSettings();

    CANCommunicator m_canCommunicator;  //!< CAN bus communication class

    DeviceProcessingMainState_t m_MainState;    //!< The main state of the state machine

    /*! configuration sub state definitions */
    typedef enum {
        DP_SUB_STATE_CONFIG_UNDEF        = 0x00,    //!< undefined
        DP_SUB_STATE_CONFIG_START        = 0x01,    //!< start condition
        DP_SUB_STATE_CONFIG_INIT         = 0x02,    //!< internal initialisation
        DP_SUB_STATE_CONFIG_REGISTER     = 0x03,    //!< Register detected nodes
        DP_SUB_STATE_CONFIG_CONFIG       = 0x04,    //!< Configure nodes
        DP_SUB_STATE_CONFIG_INIT_DEVICES = 0x05,    //!< Init devices
        DP_SUB_STATE_CONFIG_CFG_DEVICES  = 0x06,    //!< Configure devices
        DP_SUB_STATE_CONFIG_FINISHED     = 0x07,    //!< Finished
        DP_SUB_STATE_CONFIG_ERROR        = 0x08     //!< Error state
    } DPSubStateConfig_t;
    DPSubStateConfig_t m_SubStateConfig;    //!< Configuration mode sub state

    /*! Normal operation control state type definition */
    typedef enum {
        DP_SUB_STATE_NORMAL_OP_START    = 0x01, //!< start condition
        DP_SUB_STATE_NORMAL_OP_INIT     = 0x02, //!< internal initialisation
        DP_SUB_STATE_NORMAL_OP_CONFIG   = 0x03, //!< configuration
        DP_SUB_STATE_NORMAL_OP_RUN      = 0x04, //!< idle state
        DP_SUB_STATE_NORMAL_OP_FINISH   = 0x05, //!< prepare finish
        DP_SUB_STATE_NORMAL_OP_FINISHED = 0x06  //!< finished, ready for exit
    } DPSubStateNormalOp_t;
    DPSubStateNormalOp_t m_SubStateNormalOp;    //!< Normal operation mode sub state

    /*! Diagnosis service control state type definition */
    typedef enum {
        DP_SUB_STATE_DIAG_START    = 0x01,
        DP_SUB_STATE_DIAG_INIT     = 0x02,
        DP_SUB_STATE_DIAG_CONFIG   = 0x03,
        DP_SUB_STATE_DIAG_IDLE     = 0x04,
        DP_SUB_STATE_DIAG_FINISHED = 0x05
    } DPSubStateDiag_t;
    DPSubStateDiag_t m_SubStateDiag;    //!< Diagnosis service state

    /*! Shutdown service control state type definition */
    typedef enum {
        DP_SUB_STATE_SHUTDOWN_START         = 0x01, //!< inital state
        DP_SUB_STATE_SHUTDOWN_NODES         = 0x02, //!< shutdown nodes
        DP_SUB_STATE_SHUTDOWN_CLOSE_COMM    = 0x03, //!< close communcation
        DP_SUB_STATE_SHUTDOWN_CLOSE_OBJECTS = 0x04, //!< delete all objects
        DP_SUB_STATE_SHUTDOWN_FINISHED      = 0x05, //!< finished
        DP_SUB_STATE_SHUTDOWN_ERROR         = 0x06  //!< error state
    } DPSubStateShutdown_t;
    DPSubStateShutdown_t m_SubStateShutDown;    //!< Shutdown service state

    /*! Adjustment service control state type definition */
    typedef enum {
        DP_SUB_STATE_ADJUST_START    = 0x01,    //!< initial state
        DP_SUB_STATE_ADJUST_INIT     = 0x02,    //!< initialisation
        DP_SUB_STATE_ADJUST_IDLE     = 0x03,    //!< ilde, proceeding adjustment
        DP_SUB_STATE_ADJUST_FINISHED = 0x04     //!< finished
    } DecProcSubStateAdjust_t;
    DecProcSubStateAdjust_t m_SubStateAdjust;   //!< Adjustment service state

    ObjectTree m_ObjectTree;    //!< List of all CAN object of the device
    QList<CBaseModule *>::iterator m_IterObjectTree;  //!< Iterator for external object tree access

    CANMessageConfiguration* m_pCANMessageConfiguration;    //!< Pointer to the CAN message configuration class

    //colorado device processing classes
    CConfigurationService* m_pConfigurationService; //!< configuration service class
    CDiagnosticService* m_pDiagnosticService;       //!< diagnostig operation service class
    CAdjustmentService* m_pAdjustmentService;       //!< adjustment service class
    CShutdownService* m_pShutdownService;           //!< shutdown service class

    ListDevice m_DeviceList;    //!< List containing all devices

    CFunctionModuleTaskManager* m_pFunctionModuleTaskManager;   //!< Function module's tasks list access

    ListDeviceProcTask m_lstTasks;  //!< Internal task list
    //and the tasks
    DeviceProcTask* m_pTaskConfig;          //!< Configuration task
    DeviceProcTask* m_pTaskNormalOperation; //!< Normal operation task
    DeviceProcTask* m_pTaskShutdown;        //!< Shutdown task
    DeviceProcTask* m_pTaskDestroy;         //!< Destroy task
    DeviceProcTask* m_pTaskDiagnostic;      //!< Diagnostic task
    DeviceProcTask* m_pTaskAdjustment;      //!< Adjustment task task
    DeviceProcTask* m_pTaskFirmwareUpdate;  //!< Firmware update  task

    DeviceProcTask* GetActiveTask(DeviceProcTask* pActTask);    //!< Returns the active task
    void TaskFinished(DeviceProcTask* pActiveTask);             //!< Marks the task as finished, task will be deleted from list

    void RegisterMetaTypes();
    ReturnCode_t InitCANMessages();
    ReturnCode_t ReadConfiguration();
    ReturnCode_t InitTasks();

    //! Return CANNode specified by it's ID
    CBaseModule* GetNodeFromID(quint16 nodeID) const;
    //! Return CANNode specified by it's key
    CBaseModule* GetNodeFromKey(QString nodeKey);

    void HandleTaskConfig(DeviceProcTask* pActiveTask);
    void HandleTaskDiagnostic(DeviceProcTask* pActiveTask);
    void HandleTaskNormalOperation(DeviceProcTask* pActiveTask);
    void HandleTaskAdjustment(DeviceProcTask* pActiveTask);
    void HandleTaskShutDown(DeviceProcTask* pActiveTask);

    void HandleCANNodesTask();
    void HandleDevicesTask();

    //! CAN message handling
    void HandleCanMessage(can_frame* pCANframe);

    ProcessSettingsTable m_ProcessSettingsTable;    //!< Process settings hash container

    QDomDocument m_DocHwDescr;  //!< Hardware specification XML file

    struct timeb m_tbTimerHeartbeatTime;    //!< Measurement of heartbeat interval
    void CheckMasterHeartbeat();

    quint32  m_ulCANNodeIDMaster;       //!< Master CAN-Node ID
    quint32  m_ulCanIDMasterHeartbeat;  //!< CAN-ID master heartbeat

    QString m_CanInterface; //!< CAN communication interface
    QString m_TcpInterface; //!< Tcp communication interface

    static QString m_HWConfigFileName;  //!< Config file name
    static QString m_SerialNo;          //!< Serial number, read from config file

    ReturnCode_t m_LastErrorHdlInfo;    //!< Last errorcode, e.g. received by a fuction call
    quint16      m_LastErrorGroup;      //!< Last error's group id
    quint16      m_LastErrorCode;       //!< Last error's error code
    quint16      m_LastErrorData;       //!< Last error's data
    QDateTime    m_LastErrorTime;       //!< Last error's time
    QString      m_LastErrorString;     //!< Last error information string
#ifndef HAL_CV_TEST
    QWaitCondition m_WaitConditionForSyncCall[SYNC_CMD_TOTAL_NUM]; //!< Last Wait condition array used for synchronized call
#else
   typedef struct
    {
       QEventLoop eventloop;
       bool timerActive;
       qint64 endTime;
    } EventLoopWithTimeout_t;

    EventLoopWithTimeout_t m_EventLoopsForSyncCall[SYNC_CMD_TOTAL_NUM];
    QTimer* m_pTimer;
#endif
};

} // namespace

#endif /* DEVICEPROCESSING_H */
