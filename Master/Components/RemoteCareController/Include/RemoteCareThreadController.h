/****************************************************************************/
/*! \file RemoteCareController/Include/RemoteCareThreadController.h
 *
 *  \brief Definition file for class AxedaThreadController.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2014-03-13
 *  $Author:    $ Ramya GJ
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

    RemoteCareController();                                                    //!< Not implemented.
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

public:

    RemoteCareController(const quint32 ThreadID, const QString networkProcessName, NetworkBase::NetworkServerType_t serverType);
    virtual ~RemoteCareController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

    void OnCommandAckReceived(const Global::tRefType Ref, const Global::Acknowledge &Ack);
    void OnCmdTimeout(const Global::tRefType Ref, const QString &CmdName);

private:

    /// Network Device for communication with Remote Care Agent
    ExternalProcessControl::ExternalProcessDevice   *mp_RemoteCareDevice;

    /// internal name of the external process
    QString m_networkProcessName;       //REMOTECARE_PROCESS_NAME = "Axeda Client";
    NetworkBase::NetworkServerType_t m_networkServerType;


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

} // end namespace RemoteCare

#endif // REMOTECARE_REMOTECARECONTROLLER_H
