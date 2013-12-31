/****************************************************************************/
/*! \file ExportController.cpp
 *
 *  \brief Implementation file for class ExportController.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2013-10-16
 *  $Author:    $ Raju
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

#include "ExportController/Include/ExportController.h"
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>
#include <Global/Include/Commands/AckOKNOK.h>

namespace Export {

/****************************************************************************/
/**
 * \brief Constructor.
 *
 * \param[in]   ThreadID   Unique Thread ID
 */
/****************************************************************************/
ExportController::ExportController(quint32 ThreadID)
    :ExternalProcessController::ExternalProcessController(EXPORT_PROCESS_NAME, ThreadID, true)
    , m_ProcessInitialized(false)
    , mp_ExportDevice(NULL)
{
    // this is to avoid the process to restart multiple times
    SetRestartProcess(false);
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
ExportController::~ExportController() {
    try {
        if (mp_ExportDevice != NULL) {
            delete mp_ExportDevice;
        }
        mp_ExportDevice = NULL;
    }
    CATCHALL_DTOR();
}


/****************************************************************************/
/**
 * \brief Create and configure needed objects.
 *
 * If something goes wrong, the already created objects must be deleted.
 */
/****************************************************************************/
void ExportController::CreateAndInitializeObjects() {

    qDebug() << "Export: initializing objects...";

    QString Path = Global::SystemPaths::Instance().GetSettingsPath() + NetworkBase::CMH_PATH_TO_SETTINGS;
    mp_ExportDevice = new ExternalProcessControl::ExternalProcessDevice(NetworkBase::NSE_TYPE_EXPORT, EXPORT_PROCESS_NAME, Path, this, this);

    // register ExportDevice with Base Class
    ExternalProcessController::RegisterExternalProcessDevice(mp_ExportDevice);

    // continue with initialization
    ExternalProcessController::CreateAndInitializeObjects();
}


/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Go signal.
 *
 * This means that everything is fine and normal operation started.
 * We are running in our own thread now.
 */
/****************************************************************************/
void ExportController::OnGoReceived() {
    if (!m_ProcessInitialized) {
        m_ProcessInitialized = true;
        qDebug() << (QString)("Platform Export: OnGo in Export controller");
        ExternalProcessController::OnGoReceived();
    }
}

/****************************************************************************/
/**
 * \brief This method is called when Export is connected and ready to work.
 *
 *        This means we are in the "Working" state.
 */
/****************************************************************************/
void ExportController::OnReadyToWork() {
    qDebug() << "ExportController: ready to work called.";
    if (mp_ExportDevice == NULL) {
        return;
    }    
}

/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Stop signal.
 *
 * This means that normal operation will stop after processing this signal.
 * We are still running in our own thread.\n
 */
/****************************************************************************/
void ExportController::OnStopReceived() {
    qDebug() << (QString)("Platform Export: OnStop received.");
    ExternalProcessController::OnStopReceived();
    m_ProcessInitialized = false;
}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 *  These are pure virtual function which needs to be written eventhough these
 *  are not used by this class.
 *
 * Power will fail shortly.
 *\iparam   PowerFailStage = powerfail stage
 */
/****************************************************************************/
void ExportController::OnPowerFail(const Global::PowerFailStages PowerFailStage) {
    Q_UNUSED(PowerFailStage)
}

/****************************************************************************/
/**
 * \brief Cleanup and destroy the created objects.
 */
/****************************************************************************/
void ExportController::CleanupAndDestroyObjects() {
    // destroy all objects
    qDebug() << (QString)("Platform Export: CleanupAndDestroyObjects called.");
    ExternalProcessController::CleanupAndDestroyObjects();
    m_ProcessInitialized = false;
}

/****************************************************************************/
/**
 * \brief  Informs Master Thread controller that Export process has stopped
 *         working.
 * \iparam StopForEver = true indicates to stop working for ever, since no
 *                       more retries would be carried out. When this happens
 *                      Export work flow needs to be stopped
 */
/****************************************************************************/
void ExportController::OnStopWorking(bool StopForEver)
{
    if(StopForEver) {
        emit ProcessStoppedForEver();
    }
}

}
