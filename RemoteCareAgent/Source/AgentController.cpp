/****************************************************************************/
/*! \file AgentController.cpp
 *
 *  \brief Implementation file for class AgentController. AgentController
 *  initializes Master connection and Remote Enterprise Server connection
 *  and acts as Bridge between this to units. Furthermore it provides global
 *  start and stop functionality of the RemoteCareAgent process.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 14.05.2013
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

#include <QMetaType>
#include <QSharedMemory>
#include <Global/Include/SystemPaths.h>

// Global Commands
#include <Global/Include/Commands/AckOKNOK.h>

// Internal
#include "AgentController.h"

namespace RCAgentNamespace {

/****************************************************************************/
/*!
 *  \brief    Default Constructor.
 *
 *  \iparam  ip    = IP adress
 *  \iparam  port  = Port number
 */
/****************************************************************************/
AgentController::AgentController(const QString &ip, const QString &port) :
    linkMaster(ip, port),
    waitLimit(5),
    initStatus (RCAGENT_INIT_FAILED)
{
}

/****************************************************************************/
/*!
 *  \brief    Destructor.
 */
/****************************************************************************/
AgentController::~AgentController()
{
}

/****************************************************************************/
/*!
 *  \brief   Initialize RemoteCare Component.
 *
 *  \return  true if successfully initialized, false otherwise
 */
/****************************************************************************/
bool AgentController::Initialize()
{
    try {
        /*!
         * First connect Master and then REServer.
         */
        if (!linkMaster.Initialize()) {
            // RCAgent cannot connect to Master, but maybe it can run Agent and report error to RC-EServer
            qDebug() << "AgentController ERROR: InitializeMasterLink failed !";
            return false;
        }

        if (!linkREServer.Initialize()) {
            // the Agent cannot run, but we can still connect to Master and report error
            qDebug() << "AgentController ERROR: InitializeAgent failed !";
            return false;
        }

        ConnectSignalSlots();

        /*!   ===  Calculate thread exit waiting time (in ms)  ===
         *  This is a timeout for RemoteCareAgent powerdown. To be able to power
         *  down gracefully the Controller needs to wait till AxedaAgent exits its
         *  working loop. Otherwise the RC-EServer might be left with some
         *  pending requests.
         *
         *  The waiting period depends on the ExecTime of the AxedaAgent.
         */
        waitLimit = static_cast<u_long>((linkREServer.GetExecTime() + 1) * 1000);
        qDebug() << "AgentController: the waitLimit is set to " << waitLimit;

    }
    catch(...) {
        // does not matter what it is, the agent controller failed to initialize.
        return false;
    }

    initStatus = RCAGENT_INITIALIZED_OK;

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Start RemoteCareAgent Component operation.
 *           This funciton checks initialization results and acts accordingly
 *
 *  \return  true if started successfully, false otherwise
 */
/****************************************************************************/
void AgentController::ConnectSignalSlots()
{
    // Connecting send message to Master
    CONNECTSIGNALSLOT(&linkREServer, SigAgentIncomingCommand(const Global::CommandShPtr_t &), &linkMaster,  OnAgentIncomingCommand(const Global::CommandShPtr_t &));
    CONNECTSIGNALSLOT(&linkREServer, SigRequestStatus(const Global::tRefType&, const bool &), &linkMaster,  OnAckCommand(const Global::tRefType&, const bool &));

    // Connecting send message from Master
    CONNECTSIGNALSLOT(&linkMaster, SigMasterLinkInitialiizationFailed(), &linkREServer,  OnMasterInitializationFailed());
    CONNECTSIGNALSLOT(&linkMaster,    SigReportEvent(
                                                        const Global::tRefType &,
                                                        const QString&,
                                                        const QString&,
                                                        const QString&,
                                                        const QString&
                                                     ),
                      &linkREServer,  SubmitEventRequest(
                                                            const Global::tRefType &,
                                                            const QString&,
                                                            const QString&,
                                                            const QString&,
                                                            const QString&
                                                        )
                      );
    CONNECTSIGNALSLOT(&linkMaster,    SigReportEventWithoutAck(
                                                        const QString&,
                                                        const QString&,
                                                        const QString&,
                                                        const QString&
                                                     ),
                      &linkREServer,  SubmitEventWithoutAck(
                                                            const QString&,
                                                            const QString&,
                                                            const QString&,
                                                            const QString&
                                                        )
                      );

    CONNECTSIGNALSLOT(&linkMaster,    SigReportDataItem(
                                                        const Global::tRefType &,
                                                        const QString&,
                                                        const RemoteCare::RCDataItemType_t &,
                                                        const RemoteCare::RCDataItemQuality_t &,
                                                        const QString&,
                                                        const QString&
                                                       ),
                      &linkREServer,  SubmitDataItemRequest(
                                                              const Global::tRefType &,
                                                              const QString&,
                                                              const RemoteCare::RCDataItemType_t &,
                                                              const RemoteCare::RCDataItemQuality_t &,
                                                              const QString&,
                                                              const QString&
                                                           )
                      );

    CONNECTSIGNALSLOT(&linkMaster, SigShutdownController(), this,  ExitNow());
    CONNECTSIGNALSLOT(&linkMaster, SigReconnect(const Global::tRefType &), this,  OnReconnect(const Global::tRefType&));

    CONNECTSIGNALSLOT(
                        &linkMaster, SigNotifyUpload(const Global::tRefType &, const QString&),
                        &linkREServer, SubmitUploadRequest(const Global::tRefType &, const QString&)
                      );

    CONNECTSIGNALSLOT(this, SigStopRemoteCareAgent(), &linkREServer, Stop());
    CONNECTSIGNALSLOT(this, SigAckToMaster(const Global::tRefType &, const bool &), &linkMaster, OnAckCommand(const Global::tRefType &, const bool &));
}

/****************************************************************************/
/*!
 *  \brief   Start RemoteCareAgent Component operation.
 *           This funciton checks initialization results and acts accordingly
 *
 *  \return  true if started successfully, false otherwise
 */
/****************************************************************************/
bool AgentController::Start()
{

    if (initStatus == RCAGENT_INITIALIZED_OK)
    {
            linkREServer.Start();
            return true;
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief  Reconnects to Remote Enterprise Server.
 */
/****************************************************************************/
void AgentController::OnReconnect(const Global::tRefType &Ref)
{
    emit SigAckToMaster(Ref, true);
    linkREServer.Reconnect();
}

/****************************************************************************/
/*!
 *  \brief   Stop RemoteCareAgent Component operation.
 */
/****************************************************************************/
void AgentController::Stop()
{
    // if Agent is running, tell it to exit the working loop
    emit SigStopRemoteCareAgent();
    // now turn asynchronous event to synchronous one:
    // wait till Agent exits its loop or quit on timeout.
    QTimer::singleShot((int)waitLimit, &eventLoop, SLOT(quit()));
    qDebug() << "\nEntering event loop at: " << QDateTime::currentDateTime().toString("hh:mm:ss");
    static_cast<void>(eventLoop.exec()); // don't care about return value here
    qDebug() << "Exiting event loop at: " << QDateTime::currentDateTime().toString("hh:mm:ss") << "\n";
}

/****************************************************************************/
/*!
 *  \brief   RemoteCareAgent Component operation finished --> Exit.
 */
/****************************************************************************/
void AgentController::ExitNow()
{
    qDebug() << "AgentController: exiting now...";
    Stop();
    emit SigOperationStopped();
}

} // end namespace
