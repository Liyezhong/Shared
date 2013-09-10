/****************************************************************************/
/*! \file SWUpdateManager.h
 *
 *  \brief Definition of SWUpdateManager
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-05-02
 *  $Author:    $ N.Kamath
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
#ifndef SWUPDATEMANAGER_H
#define SWUPDATEMANAGER_H

//Qt headers
#include <QObject>
#include <QTimer>

//Project headers
#include <SWUpdateManager/Include/SWUpdateEventCodes.h>
#include <Global/Include/GlobalDefines.h>


namespace ExternalProcessControl {
    class ExternalProcess;
}

namespace Threads {
    class MasterThreadController;
    class CommandChannel;
}

namespace NetCommands {
    class CmdSWUpdate;
}


namespace SWUpdate {

/****************************************************************************/
/*!
 *  \brief    This class manages the sw update workflow. Following
 *            functionalities are includes
 *            1) Update Software through remotecare / USB.
 *            2) Update Rollback folder on successful S/W update
 *            3) Error Handling -> includes power fail
 */
/****************************************************************************/
class SWUpdateManager : public QObject {

    Q_OBJECT

public:
    SWUpdateManager(Threads::MasterThreadController &MasterThreadRef);
    ~SWUpdateManager();
    Q_DISABLE_COPY(SWUpdateManager) //!< Disable copy and assignment
    void UpdateSoftware(const QString &Option, const QString &UpdateFrom);
    void PowerFailed();

private:
    QString m_UpdateOption; //!< "-check" or "-Rollback"
    ExternalProcessControl::ExternalProcess *mp_SWUpdateStarter; //!< SWUpdate script process.
    bool m_ScriptExited; //!< Flag indicating if script is running or exited
    Threads::MasterThreadController &m_MasterThreadControllerRef; //!< Reference to master thread controller reference.
    QTimer m_SWUpdateCheckTimer; //!< Timer to check unresponsive software update script


    void SWUpdateHandler(Global::tRefType Ref, const NetCommands::CmdSWUpdate &Cmd, Threads::CommandChannel &AckCommandChannel);
    void UpdateRebootFile(const QString UpdateStatus, const QString CheckStatus = "NA", const QString Rollback = "No");

private slots:
    void SWUpdateProcessExited(const QString &ScriptName, int ExitCode);
    void SWUpdateError(int ErrorCode);
    void SWUpdateStarted(const QString &Name);
    void OnNoResponseFromSWUpdate();

signals:
    void SWUpdateCheckComplete();
    void RollBackComplete();
    void WaitDialog(bool Display, Global::WaitDialogText_t);
    void SWUpdateStatus(bool InProgress);
};
}//End of namespace SWUpdate
#endif // SWUPDATEMANAGER_H
