/****************************************************************************/
/*! \file AgentController.h
 *
 *  \brief Definition file for class AgentController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 30.04.2013
 *  $Author:    $ B.Stach, Y.Novak
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

#include <Global/Include/Utils.h>
#include <Global/Include/RemoteCareTypes.h>

// command framework related includes
#include <Global/Include/SharedPointer.h>
// commads
#include <Global/Include/Commands/Command.h>
#include <Global/Include/Commands/AckOKNOK.h>
// Network
#include <NetworkComponents/Include/MasterLinkDevice.h>

//Internal
#include "REServerConnector.h"
#include "MasterConnector.h"

namespace RCAgentNamespace {

/// Initialization return codes of the Agent Controller
typedef enum {
    RCAGENT_INITIALIZED_OK          = 0x00, ///< Initialization succeeded: RCAgent can start normal operation
    RCAGENT_MASTERLINK_INIT_FAILED  = 0x01, ///< Initialization of the Link to Master failed: try to connect to Axeda and report error
    RCAGENT_AXEDA_INIT_FAILED       = 0x02, ///< Initialization of the Axeda Agent Embedded failed: try to connect to Master and report error
    RCAGENT_INIT_FAILED             = 0x03  ///< Initialization failed: RCAgent must shutdown
} RCAgentInitCodes_t;

/****************************************************************************/
/*!
 *  \brief This class is a main controller of the RemoteCareAgent operation.
 *         It controls connection to Master SW as well as Remote Enterprise
 *         Server state and operation.
 */
/****************************************************************************/
class AgentController : public QObject
{
    Q_OBJECT

    /// test classes are all friends:
    friend class TestAgentController;

public:

    AgentController(const QString &ip, const QString &port);

    virtual ~AgentController();

    bool Initialize();

signals:

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when Agent and its thread are stopped
     */
    /****************************************************************************/
    void SigOperationStopped();
    /****************************************************************************/
    /*!
     *  \brief   Emit this signal to request the RemoteCareAgent to quit
     */
    /*****************************************************************************/
    void SigStopRemoteCareAgent();
    /****************************************************************************/
    /*!
     *  \brief  Signals acknowledge to master
     *
     *  \iparam   Ref    = reference of the request
     *  \iparam   result = true/false
     */
    /*****************************************************************************/
    void SigAckToMaster(const Global::tRefType &Ref, const bool &result);

public slots:

     bool Start();
     void Stop();

private slots:

     void ExitNow();
     void OnReconnect(const Global::tRefType &);

private:
     Q_DISABLE_COPY(AgentController)

     void ConnectSignalSlots();

     REServerConnector              linkREServer;       ///< Link to the Remote Enterprise Server connector instance
     MasterConnector                linkMaster;         ///< Linkt to the Master connector instance

     QEventLoop                     eventLoop;          ///< Waiting for processed Events

     u_long                         waitLimit;          ///< The waiting period depends on the ExecTime of the Remote Enterprise Server.

     RCAgentInitCodes_t             initStatus;         ///< Status codes
};

} // end namespace

#endif // RCAGENTNAMESPACE_AGENTCONTROLLER_H
