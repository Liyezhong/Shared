/****************************************************************************/
/*! \file AxedaController/Include/AxedaThreadController.h
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

#ifndef AXEDA_AXEDATHREADCONTROLLER_H
#define AXEDA_AXEDATHREADCONTROLLER_H

#include <ExternalProcessController/Include/ExternalProcessController.h>

namespace RCAgentNamespace {
   class CmdAxedaRemoteSessionRequest; ///< for tests
}


namespace Axeda {

/// internal name of the external process
const QString AXEDA_PROCESS_NAME = "Axeda Client";

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
class AxedaThreadController : public ExternalProcessControl::ExternalProcessController
{
    Q_OBJECT

private:

    AxedaThreadController();                                                    ///< Not implemented.
    AxedaThreadController(const AxedaThreadController &);                       ///< Not implemented.
    const AxedaThreadController & operator = (const AxedaThreadController &);   ///< Not implemented.

    void RegisterThreadAcksAndTimeouts();

protected:

    virtual void OnGoReceived();
    virtual void OnReadyToWork();
    virtual void OnStopReceived();
    virtual void OnPowerFail();

    /****************************************************************************/
    /**
     * \brief \todo comment.
     */
    /****************************************************************************/
    template<class TCAckClass>
    void RegisterAxedaDeviceAck()
    {
        // create functor
        Threads::AcknowledgeProcessorFunctor *pFunctor = new Threads::TemplateAcknowledgeProcessorFunctor<AxedaThreadController, Global::Acknowledge> (this, &AxedaThreadController::OnCommandAckReceived);
        // register functor
        RegisterAcknowledgeProcessorFunctor(TCAckClass::NAME, pFunctor);
    }

public:

    AxedaThreadController(Global::gSourceType TheHeartBeatSource);
    virtual ~AxedaThreadController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

    void OnCommandAckReceived(Global::tRefType Ref, const Global::Acknowledge &Ack);
    void OnCmdTimeout(Global::tRefType Ref, const QString &CmdName);

    void  OnCmdAxedaRemoteSessionRequest(Global::tRefType Ref, RCAgentNamespace::CmdAxedaRemoteSessionRequest *Cmd); ///< dummy. remove later

private:

    /// Network Device for communication with Remote Care Agent
    ExternalProcessControl::ExternalProcessDevice   *m_pAxedaDevice;
}; // end class AxedaThreadController

} // end namespace Axeda

#endif // AXEDA_AXEDATHREADCONTROLLER_H
