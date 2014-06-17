/****************************************************************************/
/*! \file ExternalProcess.h
 *
 *  \brief ExternalProcess class definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.12.2010
 *   $Author:  $ Y.Novak
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

#ifndef EXTERNALPROCESSCONTROL_EXTERNALPROCESS_H
#define EXTERNALPROCESSCONTROL_EXTERNALPROCESS_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QDebug>

namespace ExternalProcessControl {

/// Waiting time after which we have to kill the process
const int PROCESS_KILL_TIMEOUT = 5000; // 5000 ms = 5 sec

/****************************************************************************/
/**
 * \brief  This class implements an External Process Manager.
 *
 *     This class is a wrapper for the Qt's QProcess module, which offers a
 *     platform-independent way to manage start, control and termination of
 *     external processes.
 */
/****************************************************************************/
class ExternalProcess : public QObject
{
    Q_OBJECT

    /// test classes are all friends:
    friend class TestExternalProcess;

public:

    ExternalProcess(const QString &name, QObject *pParent = 0);
    ~ExternalProcess();
    void Initialize();
    bool StartProcess(const QString &cmd);
    bool StartProcess(const QString &cmd, const QStringList &args);
    bool TerminateProcess();
    bool KillProcess();
    /****************************************************************************/
    /*!
     *  \brief    gets the process running state
     *
     *  \return    process running state
     */
     /****************************************************************************/
    QProcess::ProcessState IsRunning() { return m_myProcess->state();}

    /****************************************************************************/
    /*!
     *  \brief    wait for external process to finish
     */
     /****************************************************************************/
    void WaitForFinished() { m_myProcess->waitForFinished(-1); }

    /****************************************************************************/
    /*!
     *  \brief    gets the running process name
     *
     *  \return    running process name
     */
     /****************************************************************************/
    QString GetProcessName() const { return m_myName; }

signals:

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when external Process has quit and
     *            exited.
     *
     *  \param    name = internal name of the process
     *  \param    exitcode = exit code
     *
     ****************************************************************************/
    void ProcessExited(const QString &name, int exitcode);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when external Process has successfully
     *            started and is running.
     *
     *  \param    name = internal name of the process
     *
     ****************************************************************************/
    void ProcessStarted(const QString &name);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when external Process reports an error.
     *
     *  \param    error = error code
     *
     ****************************************************************************/
    void ProcessError(int error);

private slots:

    void ReadStdOut();
    void ReadStdError();
    void ProcessErrorOccured(QProcess::ProcessError err);
    void ProcessFinished(int exitcode);
    void ProcessRuns();

private:

    ExternalProcess();                                              //!< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(ExternalProcess)

    /// Internal name of the process
    QString      m_myName;
    /// the process which will be started and controlled by instance of this class
    QProcess    *m_myProcess;
};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_EXTERNALPROCESS_H
