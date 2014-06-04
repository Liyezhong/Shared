/****************************************************************************/
/*! \file MasterLinkDevice.h
 *
 *  \brief MasterLinkDevice definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-21
 *   $Author:  $ M.Scherer
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

#ifndef NETWORKBASE_MASTERLINKDEVICE_H
#define NETWORKBASE_MASTERLINKDEVICE_H

#include "NetworkComponents/Include/MasterLinkController.h"
#include "NetworkComponents/Include/CreatorFunctor.h"
#include "Global/Include/SharedPointer.h"
#include "Global/Include/RefManager.h"
#include "Global/Include/Commands/PendingCmdDescriptor.h"
#include "Global/Include/Utils.h"

#include "Global/Include/Commands/Command.h"
#include "Global/Include/Commands/AckOKNOK.h"

namespace NetworkBase {

/// Typedef or a shared pointer of CreatorFunctor.
typedef Global::SharedPointer<NetworkBase::CreatorFunctor> CreatorFunctorShPtr_t;
/// Typedef for the CreatorFunctorShPtr_t functor hash.
typedef QHash<QString, CreatorFunctorShPtr_t> CreatorFunctorHash_t;
/// Typedef for the hash for acknowledge and timeout receivers.
typedef QHash<Global::tRefType, CreatorFunctorShPtr_t> AckAndTORcvHash_t;

/****************************************************************************/
/**
 * \brief This class handles the initialization of the user interface.
 */
/****************************************************************************/
class CMasterLinkDevice : public QObject
{
    Q_OBJECT

public:
    CMasterLinkDevice(QString ip, QString port, const NetworkBase::NetworkClientType_t clientType);
    virtual ~CMasterLinkDevice();
    bool NetworkInit();

    /****************************************************************************/
    /**
     * \brief Send a command to the master.
     *
     * Send a command to the master.
     *
     * \iparam   Cmd     The command.
     * \iparam   pRcv    The function pointer called on ack or timeout.
     * \iparam   pProcessorInstance  The instance containing the ack handler.
     * \return              The command's reference.
     */
    /****************************************************************************/
    template<class COMMAND, class ProcessorClass>
    inline Global::tRefType SendCmdToMaster(const COMMAND &Cmd,
            void(ProcessorClass::*pRcv)(Global::tRefType, const Global::AckOKNOK &), ProcessorClass *pProcessorInstance) {
        CHECKPTR(pRcv);
        // compute new reference
        Global::tRefType Ref = m_RefManager.GetNewRef();
//        qDebug() << "MasterLinkDevice::SendCmdToMaster, newRef=" << Ref
//                 << ", cmdType=" << Cmd.GetName()
//                 << ", timeout=" << Cmd.GetTimeout();

        // register OnTimeout
        if(Cmd.GetTimeout() != Global::Command::NOTIMEOUT) {
            // create descriptor
            Global::PendingCmdDescriptorShP_t PCDShP(new Global::PendingCmdDescriptor(this, Ref, Cmd.GetName(), Cmd.GetTimeout()));
            // connect descriptor to timeout slot
            CONNECTSIGNALSLOT(PCDShP.GetPointerToUserData(), TimeoutOccured(Global::tRefType, QString),
                              this, OnCommandTimeoutSlot(Global::tRefType, QString));
            // connect and start descriptor timeouts
            PCDShP->ConnectAndStartTimer();
            // now add to list of pending commands.
            static_cast<void>(
                // we DO NOT NEED the return value of insert
                m_PendingCommands.insert(Ref, PCDShP)
            );
            CreatorFunctorShPtr_t Functor(new NetworkBase::TemplateCreatorFunctor<Global::AckOKNOK, ProcessorClass>
                                          (pProcessorInstance, pRcv));
            // insert Functor
            static_cast<void>(
                // we DO NOT NEED the return value of insert
                m_AckAndTOReceivers.insert(Ref, Functor)
            );
        }
        // and send to master
        SendMsgToMaster<COMMAND>(Ref, Cmd);
        return Ref;
    }

    /****************************************************************************/
    /**
     * \brief Send an acknowledge to the master.
     *
     * Send an acknowledge to the master.
     *
     * \iparam   Ref     Command reference.
     * \iparam   Ack     The acknowledge.
     */
    /****************************************************************************/
    template<class ACKNOWLEDGE> inline void SendAckToMaster(Global::tRefType Ref, const ACKNOWLEDGE &Ack) {
        // send to master
        SendMsgToMaster<ACKNOWLEDGE>(Ref, Ack);
    }

    /****************************************************************************/
    /**
     * \brief Register a network message for processing.
     *
     * Register a network message (command or acknowledge) for processing.
     *
     * \iparam   FunctionPointer     The function pointer to register.
     * \iparam   pProcessorInstance  The instance which will process the message.
     */
    /****************************************************************************/
    template<class TheClass, class ProcessorClass>
    void RegisterNetMessage(void(ProcessorClass::*FunctionPointer)(Global::tRefType, const TheClass &), ProcessorClass *pProcessorInstance)
    {
        // create functor
        CreatorFunctorShPtr_t Functor(new NetworkBase::TemplateCreatorFunctor<TheClass, ProcessorClass> (pProcessorInstance, FunctionPointer));
        // and register
        RegisterCreatorFunctor(TheClass::NAME, Functor);
    }

private:
    // Network Interface
    NetworkBase::MasterLinkController          *mp_NetworkObject;      ///< The network layer of the GUI
    // Creator functors
    CreatorFunctorHash_t                    m_CreatorFunctors;      ///< Creator functors.
    Global::RefManager<Global::tRefType>    m_RefManager;           ///< Reference manager.
    Global::PendingCmdDescriptorPtrHash_t   m_PendingCommands;      ///< Commands waiting for acknowledge.
    AckAndTORcvHash_t                       m_AckAndTOReceivers;    ///< Receivers for acknowledges and timeouts.
private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CMasterLinkDevice)
    void RegisterCreatorFunctor(const QString &Name, const CreatorFunctorShPtr_t &Functor);
    CreatorFunctorShPtr_t GetCreatorFunctor(const QString &CmdName) const;

    /****************************************************************************/
    /**
     * \brief Send a message (command or acknowledge) to the master.
     *
     * Serialize a message (command or acknowledge) and send it to the master.
     *
     * \iparam   Ref     Message reference.
     * \iparam   Msg     The message.
     */
    /****************************************************************************/
    template<class MSG> void SendMsgToMaster(Global::tRefType Ref, const MSG &Msg) {
        //DEBUGWHEREAMI;
        // serialize data
        QByteArray Data;
        // create data stream
        QDataStream DS(&Data, QIODevice::WriteOnly);
        // set version
        DS.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        // write class name
        DS << MSG::NAME;
        // write Ref
        DS << Ref;
        // serialize contents
        DS << Msg;
        // send data
        mp_NetworkObject->SendMessageToMaster(MSG::SERIALIZERSTRING, Data, Ref);
    }

    /****************************************************************************/
    /**
     * \brief Remove a command from Pending commands.
     *
     * \iparam   Ref     Command reference.
     */
    /****************************************************************************/
    inline void RemoveFromPendingCommands(Global::tRefType Ref) {
        DEBUGWHEREAMI;
        qDebug() << "Ref = " << Ref;
        // remove from list of pending commands
        m_PendingCommands.remove(Ref);
    }

    void OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Acknowledge);
private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void OnCommandTimeoutSlot(Global::tRefType Ref, QString CommandName);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ProcessNetMessage(const QString &msgname, const QByteArray &barray);

signals:
    /****************************************************************************/
    /*!
     *  \brief    This signal is emmited when a command times out
     *
     *  \param    Ref = Command reference
     *  \param    CommandName = Command name
     *
     ****************************************************************************/
    void SigCmdTimeout(Global::tRefType Ref, const QString &CommandName);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when new DateTime sync is received
     *            from Master.
     *
     *  \param    dt = the actual date and time
     *
     ****************************************************************************/
    void SigDateAndTime(const QDateTime &dt);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when protocol peer connects
     *
     *  \param    name = name of the connected peer
     *
     ****************************************************************************/
    void SigMasterConnected(const QString &name);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when protocol peer disconnects
     *
     *  \param    name = name of the disconnected peer
     *
     ****************************************************************************/
    void SigMasterDisconnected(const QString &name);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to start timer , when working
     *            connection with Server fails
     *
     *
     ****************************************************************************/
     void StartConnectionLostTimer();
};

} // end namespace NetLayer

#endif // NETWORKBASE_MASTERLINKDEVICE_H
