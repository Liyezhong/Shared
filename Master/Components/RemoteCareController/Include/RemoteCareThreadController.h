/****************************************************************************/
/*! \file RemoteCareController/Include/RemoteCareThreadController.h
 *
 *  \brief Definition file for class AxedaThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-30
 *  $Author:    $ J.Bugariu
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

#ifndef REMOTECARE_REMOTECARECONTROLLER_H
#define REMOTECARE_REMOTECARECONTROLLER_H

#include <ExternalProcessController/Include/ExternalProcessController.h>

namespace RemoteCare {

/// internal name of the external process
const QString REMOTECARE_PROCESS_NAME = "Axeda Client";

/****************************************************************************/
/**
 * \brief This is a thread controlling class for Axeda Client management.
 *
 * This class creates, initializes and controls all Axeda Client relevant
 * objects. This class also handles the system level communications:
 * event and action handling.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class RemoteCareController : public ExternalProcessControl::ExternalProcessController
{

    friend class TestRemoteCareController;

    Q_OBJECT

private:

    RemoteCareController();                                                    ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(RemoteCareController)
    void RegisterThreadAcksAndTimeouts();

protected:

    virtual void OnGoReceived();
    virtual void OnReadyToWork();
    virtual void OnStopReceived();
    virtual void OnPowerFail(const Global::PowerFailStages PowerFailStage);
    virtual void OnStopWorking(bool StopForEver = false);

    /****************************************************************************/
    /**
     * \brief \todo comment.
     */
    /****************************************************************************/
    template<class TCAckClass>
    void RegisterRemoteCareDeviceAck()
    {
        // create functor
        Threads::AcknowledgeProcessorFunctor *pFunctor = new Threads::TemplateAcknowledgeProcessorFunctor<RemoteCareController,
                Global::Acknowledge> (this, &RemoteCareController::OnCommandAckReceived);
        // register functor
        RegisterAcknowledgeProcessorFunctor(TCAckClass::NAME, pFunctor);
    }

public:

    RemoteCareController(quint32 ThreadID);
    virtual ~RemoteCareController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

    void OnCommandAckReceived(Global::tRefType Ref, const Global::Acknowledge &Ack);
    void OnCmdTimeout(Global::tRefType Ref, const QString &CmdName);

private:

    /// Network Device for communication with Remote Care Agent
    ExternalProcessControl::ExternalProcessDevice   *mp_RemoteCareDevice;

signals:
    /****************************************************************************/
    /**
     * \brief This signal is emitted when RCA is connected to master SW
     */
    /**************************************************************************/
    void RCAConnected();

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when external Process has stopped working.
     *
     */
     /****************************************************************************/
    void ProcessStoppedForEver();

}; // end class RemoteCareController

} // end namespace Axeda

#endif // REMOTECARE_REMOTECARECONTROLLER_H
