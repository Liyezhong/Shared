/****************************************************************************/
/*! \file ExportController.h
 *
 *  \brief Definition file for class ExportController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-11
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
#ifndef EXPORT_EXPORTCONTROLLER_H
#define EXPORT_EXPORTCONTROLLER_H

#include "ExternalProcessController/Include/ExternalProcessController.h"
#include "DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataExport.h"

namespace Export {

const QString EXPORT_PROCESS_NAME = "Platform Export"; ///< internal name of the external process
/****************************************************************************/
/**
 * \brief This is a Export Controller. It handles communication
 *        with Main and Export process.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class ExportController: public ExternalProcessControl::ExternalProcessController
{
    friend class TestExportController;

    Q_OBJECT
private:
    bool m_ProcessInitialized; ///< To store the process initialization    
    ExternalProcessControl::ExternalProcessDevice   *mp_ExportDevice; ///< Network Device for communication with GUI

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(ExportController)

protected:
    virtual void OnGoReceived();
    virtual void OnStopReceived();
    virtual void OnPowerFail(const Global::PowerFailStages PowerFailStage);
    virtual void OnReadyToWork();
    virtual void OnStopWorking(bool StopForEver = false);

public:
    ExportController(quint32 ThreadID);
    virtual ~ExportController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

signals:

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when external Process has stopped working.
     *
     */
     /****************************************************************************/
    void ProcessStoppedForEver();
};
}

#endif // EXPORT_EXPORTCONTROLLER_H
