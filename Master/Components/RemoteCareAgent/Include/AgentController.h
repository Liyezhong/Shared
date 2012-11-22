/****************************************************************************/
/*! \file AgentController.h
 *
 *  \brief Definition file for class AgentController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 04.07.2011
 *  $Author:    $ Y.Novak
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

#ifndef RCAGENTNAMESPACE_AGENTCONTROLLER_H
#define RCAGENTNAMESPACE_AGENTCONTROLLER_H

#include <NetworkComponents/Include/NetworkClientDevice.h>
#include <Global/Include/Utils.h>

// commad framework related includes
#include <Global/Include/SharedPointer.h>
#include <RemoteCareAgent/Include/CreatorFunctor.h>
#include <RemoteCareAgent/Include/DispatcherFunctor.h>
#include "Global/Include/Commands/PendingCmdDescriptor.h"
#include "Global/Include/RefManager.h"
// commads
#include <Global/Include/Commands/Command.h>
#include <Global/Include/Commands/AckOKNOK.h>

/// This is a default timeout value for RemoteCareAgent powerdown (in ms)
#define RCAGENT_EXIT_DEFAULT_WAIT 5000

namespace RCAgentNamespace {

/// the agent class
class AxedaAgent;
/// command classes
class CmdAxedaAlarm;
class CmdAxedaEvent;
class CmdAxedaDataItem;
class CmdAxedaUpload;
class CmdAxedaSetTag;
class CmdAxedaRemoteSessionStatus;
//class CmdPowerDown;

/// Type of data item value
typedef enum {
    ADI_Analog,         ///< analog
    ADI_Digital,        ///< digital (0/1)
    ADI_String,         ///< string
    ADI_Undefined       ///< undefined
} AxedaDataItemType_t;

/// Quality of the data item value
typedef enum {
    ADI_DataGood,       ///< good
    ADI_DataBad,        ///< bad
    ADI_DataUncertain   ///< uncertain (undefined)
} AxedaDataItemQuality_t;

/// Range for the digital data type
typedef enum {
    ADI_DataNull,       ///< digital 0
    ADI_DataOne,        ///< digital 1
    ADI_DataError       ///< erroneous value
} AxedaDigitalDataItemRange_t;

/// Initialization return codes of the Agent Controller
typedef enum {
    RCAGENT_INITIALIZED_OK = 0x00,         ///< Initialization succeeded: RCAgent can start normal operation
    RCAGENT_MASTERLINK_INIT_FAILED = 0x01, ///< Initialization of the Link to Master failed: try to connect to Axeda and report error
    RCAGENT_AXEDA_INIT_FAILED = 0x02,      ///< Initialization of the Axeda Agent Embedded failed: try to connect to Master and report error
    RCAGENT_INIT_FAILED = 0x03             ///< Initialization failed: RCAgent must shutdown
} RCAgentInitCodes_t;

/// Typedef for a shared pointer of CreatorFunctor
typedef Global::SharedPointer<RCAgentNamespace::CreatorFunctor>    CreatorFunctorShPtr_t;
/// Typedef for the CreatorFunctorShPtr_t functor hash
typedef QHash<QString, CreatorFunctorShPtr_t>                      CreatorFunctorHash_t;
/// Typedef for a shared pointer to Command dispatching
typedef Global::SharedPointer<RCAgentNamespace::DispatcherFunctor> DispatcherFunctorShPtr_t;
/// Typedef for DispatcherFunctorShPtr_t hash
typedef QHash<QString, DispatcherFunctorShPtr_t>                   DispatcherFunctorHash_t;

/// debug shared memory key for shutting Agent down
const QString DEBUG_SHARED_MEM_KEY = "AxedaShutdown";

/****************************************************************************/
/**
 *  \brief This class is a main controller of the RemoteCareAgent operation.
 *         It controls connection to Master SW as well as AxedaAgent state
 *         and operation.
 *  \todo:
 *   This class handles two connections: to Master and to Axeda Server.
 *   Its event-processing actions depend on status of both connections.
 *   It is more appropriate to handle that with a state machine (like
 *   ExternalProcessController) instead of IFs and THENs. But since I do not
 *   have much time, it remains a big fat TODO for Torsten. ;-)
 */
/****************************************************************************/
class AgentController : public QObject
{
    Q_OBJECT

    /// test classes are all friends:
    //friend class TestXXXX;

public:

    AgentController();
    virtual ~AgentController();
    RCAgentInitCodes_t Initialize(int NofArgs, char *args[]);

    /****************************************************************************/
    /**
     *  \brief  Send command to the master.
     *
     *  \param[in]   Cmd = The command to send.
     *  \return      Command's reference.
     */
    /****************************************************************************/
    template<class COMMAND> Global::tRefType SendCmdToMaster(const COMMAND &Cmd)
    {
        // compute new reference
        Global::tRefType Ref = m_RefManager.GetNewRef();
        // register OnTiemout
        if(Cmd.GetTimeout() != Global::Command::NOTIMEOUT) {
            // create descriptor
            Global::PendingCmdDescriptorShP_t PCDShP(new Global::PendingCmdDescriptor(this, Ref, Cmd.GetName(), Cmd.GetTimeout()));
            // connect descriptor to timeout slot
            CONNECTSIGNALSLOT(PCDShP.GetPointerToUserData(), TimeoutOccured(Global::tRefType, QString),
                              this, OnCmdTimeout(Global::tRefType, QString));
            // connect and start descriptor timeouts
            PCDShP->ConnectAndStartTimer();
            // now add to list of pending commands.
            static_cast<void>(
                // we DO NOT NEED the return value of insert
                m_PendingAcks.insert(Ref, PCDShP)
            );
        }
        // and send to master
        SendMsgToMaster<COMMAND>(Ref, Cmd);
        return Ref;
    }
    /****************************************************************************/
    /**
     *  \brief  Send an acknowledge to the master.
     *
     *  \param[in]   Ref = reference of the "acked" command.
     *  \param[in]   Ack = the acknowledge.
     */
    /****************************************************************************/
    template<class ACKNOWLEDGE> inline void SendAckToMaster(Global::tRefType Ref, const ACKNOWLEDGE &Ack)
    {
        // send to master
        SendMsgToMaster<ACKNOWLEDGE>(Ref, Ack);
    }

signals:

     /****************************************************************************/
     /*!
      *  \brief    This signal is emitted when Agent and its thread are stopped
      *
      ****************************************************************************/
     void SigOperationStopped();
     /****************************************************************************/
     /*!
      *  \brief    This signal is emitted to forward message to Master SW
      *
      *  \param[in] name = command name
      *  \param[in] payload = message payload
      *  \param[in] ref = application command reference
      *
      ****************************************************************************/
     void SigSendMessageToMaster(const QString &name, const QByteArray &payload, Global::tRefType ref);
     /****************************************************************************/
     /*!
      *  \brief   Emit this signal to submit Alarm to be sent to Server
      *
      *  \param[in] ref = request's reference
      *  \param[in] name = alarm's name
      *  \param[in] message = alarm's description text
      *  \param[in] severity = alarm's severity
      *  \param[in] condition = alarm's condition
      *  \param[in] time = Master's timestamp value
      *
      *****************************************************************************/
     void SigAlarmRequest(quint64 ref, QByteArray name, QByteArray message, const QString &severity, QByteArray condition, const QString &time);
     /****************************************************************************/
     /*!
      *  \brief   Emit this signal to submit Event to be sent to Server
      *
      *  \param[in] ref = request's reference
      *  \param[in] name = event's name
      *  \param[in] message = event's description text
      *  \param[in] severity = event's severity
      *  \param[in] time = Master's timestamp value
      *
      *****************************************************************************/
     void SigEventRequest(quint64 ref, QByteArray name, QByteArray message, const QString &severity, const QString &time);
     /****************************************************************************/
     /*!
      *  \brief    This signal is emitted to forward a DataItem to RC-EServer
      *
      *  \param[in] ref = request's reference
      *  \param[in] name = parameter's name
      *  \param[in] type = type of the parameter (analog/digital/string)
      *  \param[in] quality = quality of the parameter
      *  \param[in] value = the value of the parameter
      *  \param[in] time = Master's timestamp value
      *
      ****************************************************************************/
     void SigDataItemRequest(quint64 ref, QByteArray name, AxedaDataItemType_t type, AxedaDataItemQuality_t quality, QByteArray value, const QString &time);
     /****************************************************************************/
     /*!
      *  \brief   Emit this signal to submit a file to be uploaded to Server
      *
      *  \param[in] ref = request's reference
      *  \param[in] filepath = full path, including file name.
      *
      *****************************************************************************/
     void SigUploadRequest(quint64 ref, QByteArray filepath);
     /****************************************************************************/
     /*!
      *  \brief   Emit this signal to request the Axeda Agent to quit
      *
      *****************************************************************************/
     void SigStopAxedaAgent();
     /****************************************************************************/
     /*!
      *  \brief   Emit this signal to set Remote Session Status on Axeda Agent
      *
      *  \param[in] status = true for enable, false for disable
      *
      *****************************************************************************/
     void SigSetRemoteSessionStatus(bool status);

public slots:

     bool Start();
     void Stop();
     void Cleanup();
     void ConnectedToMaster(const QString &name);
     void MasterLinkProblem(const QString &name);
     void ProcessMasterMessage(const QString &name, const QByteArray &payload);
     void OnRequestStatus(quint64 ref, const QString &error);
     void OnMsgSendingResult(Global::tRefType ref, const QString &status);
     void OnCmdTimeout(Global::tRefType ref, const QString &cmdname);
     void OnAgentIncomingCommand(const Global::CommandShPtr_t &ptr);

private slots:

     void ExitNow();
     void RunShutDownMonitor(); ///< debug function

private:

     void InitializeShutDownMonitor(); ///< debug function
     bool InitializeMasterLink(const QString &ip, const QString &port);
     bool InitializeAgent();
     void StartAgent();
     void StartNetwork();

     void RegisterCreatorFunctor(const QString &cmdname, const CreatorFunctorShPtr_t &functor);
     CreatorFunctorShPtr_t GetCreatorFunctor(const QString &cmdname) const;
     void RegisterDispatcherFunctor(const QString &cmdname, const DispatcherFunctorShPtr_t &functor);
     DispatcherFunctorShPtr_t GetDispatcherFunctor(const QString &cmdname) const;
     void RemoveFromPendingAcks(Global::tRefType ref);

     void OnCmdAck(Global::tRefType ref, const Global::AckOKNOK &ack);
     void OnCmdAxedaAlarm(Global::tRefType ref, const CmdAxedaAlarm &cmd);
     void OnCmdAxedaEvent(Global::tRefType ref, const CmdAxedaEvent &cmd);
     void OnCmdAxedaDataItem(Global::tRefType ref, const CmdAxedaDataItem &cmd);
     void OnCmdAxedaUpload(Global::tRefType ref, const CmdAxedaUpload &cmd);
//     void OnCmdPowerDown(Global::tRefType ref, const Global::CmdPowerDown &cmd);
     void OnAgentRemoteSessionRequest(const Global::CommandShPtr_t &ptr);
     void OnCmdAxedaRemoteSessionStatus(Global::tRefType ref, const CmdAxedaRemoteSessionStatus &cmd);

     /****************************************************************************/
     /**
      * \brief Register an Agent message for processing.
      *
      * \param[in]   FunctionPointer    = The function pointer to register.
      * \param[in]   pProcessorInstance = The instance which will process the message.
      */
     /****************************************************************************/
     template<class TheClass, class ProcessorClass>
     void RegisterAgentMessage(void(ProcessorClass::*FunctionPointer)(const Global::CommandShPtr_t &ptr), ProcessorClass *pProcessorInstance)
     {
         // create functor
         DispatcherFunctorShPtr_t Functor(new RCAgentNamespace::TemplateDispatcherFunctor<TheClass, ProcessorClass> (pProcessorInstance, FunctionPointer));
         // and register
         RegisterDispatcherFunctor(TheClass::NAME, Functor);
     }
     /****************************************************************************/
     /**
      * \brief Register a Master message for processing.
      *
      * Register a Master message (command or acknowledge) for processing.
      *
      * \param[in]   FunctionPointer    = The function pointer to register.
      * \param[in]   pProcessorInstance = The instance which will process the message.
      */
     /****************************************************************************/
     template<class TheClass, class ProcessorClass>
     void RegisterMasterMessage(void(ProcessorClass::*FunctionPointer)(Global::tRefType, const TheClass &), ProcessorClass *pProcessorInstance)
     {
         // create functor
         CreatorFunctorShPtr_t Functor(new RCAgentNamespace::TemplateCreatorFunctor<TheClass, ProcessorClass> (pProcessorInstance, FunctionPointer));
         // and register
         RegisterCreatorFunctor(TheClass::NAME, Functor);
     }
     /****************************************************************************/
     /**
      * \brief Send a message (command or acknowledge) to the master.
      *
      * Serialize a message (command or acknowledge) and send it to the master.
      *
      * \param[in]   ref = Message reference.
      * \param[in]   msg = The message.
      */
     /****************************************************************************/
     template<class MSG> void SendMsgToMaster(Global::tRefType ref, const MSG &msg)
     {
         //qDebug() << "AgentController::SendMsgToMaster: with reference " << ref;
         // serialize data
         QByteArray Data;
         // create data stream
         QDataStream DS(&Data, QIODevice::WriteOnly);
         // set version
         DS.setVersion(static_cast<int>(QDataStream::Qt_4_0));
         // write class name
         DS << MSG::NAME;
         // write Ref
         DS << ref;
         // serialize contents
         DS << msg;
         // send data
         emit SigSendMessageToMaster(MSG::SERIALIZERSTRING, Data, ref);
     }

private:

     /// pointer to the actual Axeda Agent wrapper
     AxedaAgent          *m_myAxedaAgent;
     /// the thread for runnning the Axeda Agent
     QThread              m_myAgentThread;
     /// pointer to the Master Link Controller object
     NetworkBase::NetworkClientDevice *m_myMasterLinkDevice;
     /// the thread for runnning the NetLayer
     QThread              m_myNetLayerThread;
     /// time limit for Agent's thread exit waiting:
     unsigned long        m_WaitLimit;
     /// separate event loop for Agent's thread (it is used to avoid blocking)
     QEventLoop           m_EventLoop;
     /// status of AgentController initialization
     RCAgentInitCodes_t   m_InitStatus;
     /// debug timer for shared memory monitoring
     QTimer               m_DebugMonitorTimer;
     /// Hash of pointers to (registered) command creation functors
     CreatorFunctorHash_t m_CreatorFunctors;
     /// Hash of pointers to (registered) command dispatching functors
     DispatcherFunctorHash_t m_DispatcherFunctors;
     /// awaited acknowledges for sent commands
     Global::PendingCmdDescriptorPtrHash_t   m_PendingAcks;
     /// command reference generator
     Global::RefManager<Global::tRefType>   m_RefManager;
};

} // end namespace

#endif // RCAGENTNAMESPACE_AGENTCONTROLLER_H
