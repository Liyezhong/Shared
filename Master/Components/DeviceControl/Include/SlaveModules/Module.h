/****************************************************************************/
/*! \file DeviceControl/Include/SlaveModules/Module.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CModule
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

#ifndef DEVICECONTROL_MODULEPRIVATE_H
#define DEVICECONTROL_MODULEPRIVATE_H

#include <sys/socket.h>
#include <sys/timeb.h> // used for time measurement
#include <linux/can.h>
#include <QDateTime>
#include <QMutex>

#include "DeviceControl/Include/SlaveModules/ModuleConfig.h"

namespace DeviceControl
{

class CANCommunicator;
class CANMessageConfiguration;
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of macro MODULE_CMD_MAX_RESEND_TIME
 */
/****************************************************************************/
#define MODULE_CMD_MAX_RESEND_TIME (10)
/****************************************************************************/
/*!
 *  \brief  Base class for the base module and all function modules
 *
 *      The class provides some basic functionality, like name or key access.
 *      It provides some virtual methodes for task handling and CAN message
 *      receipt.
 */
/****************************************************************************/
class CModule : public QObject
{
    Q_OBJECT

public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CModule
     *
     *  \param eObjectType
     *  \param p_MessageConfiguration
     *  \param pCANCommunicator
     *
     *  \return from CModule
     */
    /****************************************************************************/
    CModule(CModuleConfig::CANObjectType_t eObjectType, const DeviceControl::CANMessageConfiguration *p_MessageConfiguration,
            CANCommunicator* pCANCommunicator);
    virtual ~CModule();

    //! Task handling function
    virtual void HandleTasks() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Request a data reset
     *
     *      Default implementation for this function. Can be overridden by
     *      modules that actually need it.
     *
     *  \return Returns DCL_ERR_FCT_NOT_IMPLEMENTED
     */
    /****************************************************************************/
    virtual ReturnCode_t ReqDataReset() { return DCL_ERR_FCT_NOT_IMPLEMENTED; }
	
    //pure virtual method for can message receive
    /****************************************************************************/
    /**
     * \brief   CAN message receiption handler
     *
     * \param   pCANframe = CAN-message to handle
     */
    /****************************************************************************/
    virtual void HandleCanMessage(can_frame* pCANframe) = 0;

    /****************************************************************************/
    /**
     * \brief   Initialisation
     *
     * \return  DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
     *          otherwise DCL_ERR_INVALID_STATE
     */
    /****************************************************************************/
    virtual ReturnCode_t Initialize() = 0;

    /****************************************************************************/
    /**
     * \brief   Pass the Configuration to the instance
     *
     * \param   pCANObjectConfig = Pointer to configuration object
     */
    /****************************************************************************/
    void SetCANConfiguration(CModuleConfig* pCANObjectConfig) { m_pCANObjectConfig = pCANObjectConfig; }

    /****************************************************************************/
    /**
     * \brief Set the name attribut
     *
     * \param   CANObjectName = Name attribut of this CAN object
     */
    /****************************************************************************/
    void SetName(QString CANObjectName) { m_pCANObjectConfig->m_strName = CANObjectName; }

    /****************************************************************************/
    /**
     * \brief Return the name attribut
     *
     * \return   Name attribut
     */
    /****************************************************************************/
    QString GetName() const { return m_pCANObjectConfig->m_strName; }

    /****************************************************************************/
    /**
     * \brief Set the key attribut
     *
     * \param   CANObjectKey = Key attribut of this CAN object
     */
    /****************************************************************************/
    void SetKey(QString CANObjectKey) { m_pCANObjectConfig->m_strKey = CANObjectKey; }

    /****************************************************************************/
    /**
     * \brief Return the key attribut
     *
     * \return   Key attribut
     */
    /****************************************************************************/
    QString GetKey() const { return m_pCANObjectConfig->m_strKey; }


    /****************************************************************************/
    /**
     * \brief Return the key attribute
     *
     * \todo rename to InstanceID
     *
     * \return   Key attribut
     */
    /****************************************************************************/
    quint32 GetModuleHandle();

    /****************************************************************************/
    /**
     * \brief  Returns the object type
     *
     * \return Object type
     */
    /****************************************************************************/
    CModuleConfig::CANObjectType_t GetType() const { return m_eObjectType; }

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StartTimeDelay
     *
     *  \return from StartTimeDelay
     */
    /****************************************************************************/
    void StartTimeDelay();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetTimeDelay
     *
     *  \return from GetTimeDelay
     */
    /****************************************************************************/
    quint16 GetTimeDelay();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StopTimeDelay
     *
     *  \return from StopTimeDelay
     */
    /****************************************************************************/
    void StopTimeDelay();

signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted when an error was detected or received from slave
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam ErrorGroup = Error group
     *  \iparam ErrorCode = Error code
     *  \iparam ErrorData = Additional error information
     *  \iparam ErrorTime = Error time
     */
    /****************************************************************************/
    void ReportError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime);
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted when an error was detected or received from slave
     *
     *  \iparam EventCode = Event code
     *  \iparam EventData = Event data
     *  \iparam EventTime = Event time
     */
    /****************************************************************************/
    void ReportEvent(quint32 EventCode, quint16 EventData, QDateTime EventTime);

    /****************************************************************************/
    /*!
     *  \brief  Report the data reset acknowledge
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportDataResetAckn(quint32 InstanceID, ReturnCode_t HdlInfo);

protected:
    /****************************************************************************/
    /*!
     *  \brief  Returns the node ID of the function module's node
     *
     *  \return Node ID
     */
    /****************************************************************************/
    virtual quint32 GetNodeID() const = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendCANMsgReqDataReset
     *
     *  \return from SendCANMsgReqDataReset
     */
    /****************************************************************************/
    ReturnCode_t SendCANMsgReqDataReset();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCANMsgError
     *
     *  \param pCANframe = can_frame type parameter
     *
     *  \return from HandleCANMsgError
     */
    /****************************************************************************/
    void HandleCANMsgError(can_frame* pCANframe);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function InitializeEventCANMessages
     *
     *  \param ModuleID = quint8 type parameter
     *
     *  \return from InitializeEventCANMessages
     */
    /****************************************************************************/
    ReturnCode_t InitializeEventCANMessages(quint8 ModuleID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RegisterEventCANMessages
     *
     *  \return from RegisterEventCANMessages
     */
    /****************************************************************************/
    ReturnCode_t RegisterEventCANMessages();
    void HandleCANMsgAcknDataReset(can_frame* pCANframe);   //!< Handles the receipt of can message 'AcknDataReset'

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetCANMsgDataU32
     *
     *  \param pCANframe = can_frame type parameter
     *  \param msgData =  quint32 type parameter
     *  \param offset =  quint8 type parameter
     *
     *  \return from SetCANMsgDataU32
     */
    /****************************************************************************/
    static void SetCANMsgDataU32(can_frame* pCANframe, quint32 msgData, quint8 offset);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetCANMsgDataS32
     *
     *  \param pCANframe = can_frame type parameter
     *  \param msgData =  qint32 type parameter
     *  \param offset =  quint8 type parameter
     *
     *  \return from SetCANMsgDataS32
     */
    /****************************************************************************/
    static void SetCANMsgDataS32(can_frame* pCANframe, qint32 msgData, quint8 offset);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetCANMsgDataU16
     *
     *  \param pCANframe = can_frame type parameter
     *  \param msgData =  quint16 type parameter
     *  \param offset =  quint8 type parameter
     *
     *  \return from SetCANMsgDataU16
     */
    /****************************************************************************/
    static void SetCANMsgDataU16(can_frame* pCANframe, quint16 msgData, quint8 offset);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetCANMsgDataS16
     *
     *  \param pCANframe = can_frame type parameter
     *  \param msgData =  qint16 type parameter
     *  \param offset =  quint8 type parameter
     *
     *  \return from SetCANMsgDataS16
     */
    /****************************************************************************/
    static void SetCANMsgDataS16(can_frame* pCANframe, qint16 msgData, quint8 offset);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetCANMsgDataU16
     *
     *  \param pCANframe = can_frame type parameter
     *  \param offset =  quint8 type parameter
     *
     *  \return from GetCANMsgDataU16
     */
    /****************************************************************************/
    static quint16 GetCANMsgDataU16(can_frame* pCANframe, quint8 offset);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetCANMsgDataS16
     *
     *  \param pCANframe = can_frame type parameter
     *  \param offset =  quint8 type parameter
     *
     *  \return from GetCANMsgDataS16
     */
    /****************************************************************************/
    static qint16 GetCANMsgDataS16(can_frame* pCANframe, quint8 offset);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetCANMsgDataU32
     *
     *  \param pCANframe = can_frame type parameter
     *  \param offset =  quint8 type parameter
     *
     *  \return from GetCANMsgDataU32
     */
    /****************************************************************************/
    static quint32 GetCANMsgDataU32(can_frame* pCANframe, quint8 offset);
    static quint64 GetCANMsgDataU64(can_frame* pCANframe);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetCANMsgDataU64
     *
     *  \param pCANframe = can_frame type parameter
     *  \param msgData =  quint64 type parameter
     *
     *  \return from SetCANMsgDataU64
     */
    /****************************************************************************/
    static void SetCANMsgDataU64 (can_frame* pCANframe, quint64 msgData);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ComputePassword
     *
     *  \return from ComputePassword
     */
    /****************************************************************************/
    quint16 ComputePassword();
    CANCommunicator* m_pCANCommunicator;                    //!< Communicator object
    const CANMessageConfiguration* mp_MessageConfiguration; //!< Message configuration

    CModuleConfig* m_pCANObjectConfig;  //!< Base class containing CAN object configuration

    // variables for error handling
    ReturnCode_t m_lastErrorHdlInfo;    //!< last errorcode, e.g. received by a fuction call
    quint16 m_lastErrorGroup;           //!< last error's group id
    quint16 m_lastErrorCode;            //!< last error's error code
    quint16 m_lastErrorData;            //!< last error's data
    QDateTime m_lastErrorTime;          //!< last error's time

    /* CAN-message IDs for event notification */
    quint32 m_unCanIDEventInfo;         //!< CAN-ID 'Event info' message */
    quint32 m_unCanIDEventWarning;      //!< CAN-ID 'Event Warning' message */
    quint32 m_unCanIDEventError;        //!< CAN-ID 'Error' message */
    quint32 m_unCanIDEventFatalError;   //!< CAN-ID 'FatalError' message */
    quint32 m_unCanIDReqDataReset;      /*!< CAN-ID 'ReqDataReset' message */
    quint32 m_unCanIDAcknDataReset;     /*!< CAN-ID 'AcknDataReset' message */

    /*! Node command state */
    typedef enum {
        MODULE_CMD_STATE_FREE     = 0x00,   //!< undefined command state
        MODULE_CMD_STATE_REQ      = 0x01,   //!< command was reqested, waits for being sent to slave
        MODULE_CMD_STATE_REQ_SEND = 0x02    //!< command requests was sent to the slave
    } ModuleCmdState_t;

    /*! Task state definitions */
    typedef enum {
        MODULE_TASKID_INIT        = 0x00,   ///< initialisation state
        MODULE_TASKID_FREE        = 0x01,   ///< idle state
        MODULE_TASKID_COMMAND_HDL = 0x02,   ///< command request state
        MODULE_TASKID_RECONFIGURE = 0x03    ///< reconfiguration state
    } ModuleTaskID_t;

    ModuleTaskID_t m_TaskID;    //!< Actual task ID

    QMutex m_Mutex; //!< Protects the task handling thread from request functions

private:
    CModule();                                    ///< Not implemented.
    CModule(const CModule &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief  Not implemented
     *
     *  \return from HandleIdleState
     */
    /****************************************************************************/
    const CModule & operator = (const CModule &);
    /****************************************************************************/
    /*!
     *  \brief  CAN message ID initialization
     *
     *  \return Initialisation result
     */
    /****************************************************************************/
    virtual ReturnCode_t InitializeCANMessages() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Registers the CAN messages to communication layer
     *
     *  \return Result
     */
    /****************************************************************************/
    virtual ReturnCode_t RegisterCANMessages() = 0;

    //!< Idle taks handling function
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleIdleState
     *
     *  \return from HandleIdleState
     */
    /****************************************************************************/
    virtual void HandleIdleState() = 0;

    CModuleConfig::CANObjectType_t m_eObjectType;   ///< object type
    struct timeb m_tbTimeDelay;   ///< time struct used for heart beat
    quint8 m_sOrderNr;            ///< order nr for gui lists

    /*! timer state */
    typedef enum {
        COBJ_TIMER_RUN         = 0x00,  ///< timer runs
        COBJ_TIMER_STOPPED     = 0x01   ///< timer stopped
    } TimerState_t;

    TimerState_t m_TimerRunning;    ///< timer state
};

} //namespace

#endif /* DEVICECONTROL_MODULEPRIVATE_H */
