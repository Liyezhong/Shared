/****************************************************************************/
/*! \file ExternalProcess.cpp
 *
 *  \brief ExternalProcess class implementation.
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

#include <ExternalProcessController/Include/ExternalProcess.h>
#include <Global/Include/TranslatableString.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>


namespace ExternalProcessControl {

/****************************************************************************/
/*!
 *  \brief    Constructor for the ExternalProcess
 *
 *
 *  \param    name = internal name of the process
 *  \param    pParent = pointer to the parent object
 *
 *
 ****************************************************************************/
ExternalProcess::ExternalProcess(const QString &name, QObject *pParent)
        : QObject(pParent),
        m_myName(name),
        m_myProcess(NULL)
{
}

/****************************************************************************/
/*!
 *  \brief    Destructor for the ExternalProcess
 *
 *
 ****************************************************************************/
ExternalProcess::~ExternalProcess()
{
    try {
        if (m_myProcess != NULL) {
            m_myProcess->deleteLater();
        }
        m_myProcess = NULL;
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
/*!
 *  \brief    This function initializes the ExternalProcess.
 *
 *      Call this function before using the created object.
 *
 ****************************************************************************/
void ExternalProcess::Initialize()
{
    try {
        m_myProcess = new QProcess(this);
        CONNECTSIGNALSLOT(m_myProcess, finished(int), this, ProcessFinished(int));
        CONNECTSIGNALSLOT(m_myProcess, started(), this, ProcessRuns());
        CONNECTSIGNALSLOT(m_myProcess, readyReadStandardOutput(), this, ReadStdOut());
        CONNECTSIGNALSLOT(m_myProcess, readyReadStandardError(), this, ReadStdError());
        CONNECTSIGNALSLOT(m_myProcess, error(QProcess::ProcessError), this, ProcessErrorOccured(QProcess::ProcessError));
    }
    CATCHALL();
}

/****************************************************************************/
/*!
 *  \brief    This function reads std output of the running external process.
 *
 *
 ****************************************************************************/
void ExternalProcess::ReadStdOut()
{
    /// \todo not sure we need this function, but... let it be here for now.

    QString result = "";
    if (m_myProcess != NULL) {
        m_myProcess->setReadChannel(QProcess::StandardOutput);
        while ( m_myProcess->canReadLine() ) {
            QString line = m_myProcess->readLine();
            //line.chop(1); // remove \n
            result.append(line);
        }
    }
    qDebug() << (QString)("ExternalProcess: StdOut read from process: \n" + result);

    /// \todo check process output and deside what to do !
}

/****************************************************************************/
/*!
 *  \brief    This function reads error output of the running external process.
 *
 *
 ****************************************************************************/
void ExternalProcess::ReadStdError()
{
    QString result = "";
    if (m_myProcess != NULL) {
        m_myProcess->setReadChannel(QProcess::StandardError);
        while ( m_myProcess->canReadLine() ) {
            QString line = m_myProcess->readLine();
            //line.chop(1); // remove \n
            result.append(line);
        }
    }
//    qDebug() << (QString)("ExternalProcess: ErrorCode read from process: \n" + result);

    /// \todo check process error and decide what to do !
}

/****************************************************************************/
/*!
 *  \brief    This slot is called when external process reports an error.
 *
 *
 *  \param    err = the error which occured
 *
 *  \warning  This slot will NOT be called if shell command was executed
 *            successfully, but failed to start the actual external
 *            process (see descriptioin of "ProcessRuns" slot).
 *
 *  \warning  If this slot is called, it might mean (depends on error) that
 *            the external process crashed, but is still resident in memory.
 *            User of this class might consider calling "KillProcess" function
 *            to make sure that the troubled external process is completely
 *            wiped out from the system.
 *
 *  \return
 *
 ****************************************************************************/
void ExternalProcess::ProcessErrorOccured(QProcess::ProcessError err)
{
    /// \todo evaluate the error code ?
    qDebug() << "ExternalProcess: ERROR occured: " + QString::number(static_cast<int>(err), 10);
    emit ProcessError(static_cast<int>(err));
}

/****************************************************************************/
/*!
 *  \brief    This slot is called when external process terminates.
 *
 *  \param    exitcode = the exit code of the process
 *
 *  \warning  This slot will NOT be called if process was unable to start.
 *            The "ProcessErrorOccured" slot will be called in this case.
 *            See also description of the "ProcessRuns" slot.
 *
 *  \warning  If this slot is called with exitcode "QProcess::NormalExit",
 *            it still might mean that the process has crashed, but "in a
 *            controlled way". The error code reported by "ProcessErrorOccured"
 *            needs to be evaluated to make sure why exactly process exited.
 *
 *
 ****************************************************************************/
void ExternalProcess::ProcessFinished(int exitcode)
{
    // external process exited
    /// \todo evaluate the exit code ?
    qDebug() << (QString)("ExternalProcess: " + m_myName + " exited with the code " + QString::number(exitcode, 10));
    emit ProcessExited(m_myName, exitcode);
}

/****************************************************************************/
/*!
 *  \brief    This slot is called when external process successfully starts.
 *
 *  \warning  Meaning of this slot will be different dependent on the
 *            way how external process was started. For example: if some
 *            shell command was used (e.g. "nice -n 5 ./gui -qws &"), this
 *            slot will be called if the shell command was successfully
 *            executed, which does not mean that the actual external process
 *            ("gui" in our case) was able to start. If shell command has a
 *            problem with its arguments (e.g. "nice" did not find the "gui"
 *            binary), then it will NOT report any errors, but will exit
 *            normally with some exit() code (in our example: code 127).
 *
 *
 ****************************************************************************/
void ExternalProcess::ProcessRuns()
{
    if (m_myProcess != NULL) {
        // external process started running
        qDebug() << QString("ExternalProcess: " + m_myName + " started. PID=" + QString::number(m_myProcess->pid()));
        emit ProcessStarted(m_myName);
    }
}

/****************************************************************************/
/*!
 *  \brief    This function starts external process's execution.
 *
 *  \param    cmd = the command to execute (path to the binary)
 *
 *  \return   true if system command was executed, false otherwise
 *
 ****************************************************************************/
bool ExternalProcess::StartProcess(const QString &cmd)
{
    if (m_myProcess != NULL) {
        qDebug() << (QString)("\nExternalProcess: trying to start " + m_myName + " process with command " + cmd + "\n");
        m_myProcess->start(cmd);
        return true;
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief    This function starts external process's execution.
 *
 *  \param    cmd = the command to execute (path to the binary)
 *  \param    args = arguments to be passed to the binary
 *
 *  \return   true if system command was executed, false otherwise
 *
 ****************************************************************************/
bool ExternalProcess::StartProcess(const QString &cmd, const QStringList &args)
{
    if (m_myProcess != NULL) {
        qDebug() << (QString)("\nExternalProcess: trying to start " + m_myName + " process...\n");
        m_myProcess->start(cmd, args);
        return true;
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief    This function terminates external process.
 *
 *            Use this function to properly exit the external process.
 *
 *  \return   true if system command was executed, false otherwise
 *
 ****************************************************************************/
bool ExternalProcess::TerminateProcess()
{
    if (m_myProcess != NULL) {
        qDebug() << (QString)("\nExternalProcess: trying to TERMINATE " + m_myName + " process...\n");
        static_cast<void>(
            // cannot use return value here anyway
            this->disconnect()
        );
        m_myProcess->terminate();
        if (!m_myProcess->waitForFinished(5000)) {
            m_myProcess->deleteLater();
            //lint -esym(423, ExternalProcessControl::ExternalProcess::m_myProcess)
            m_myProcess = NULL;
            return false;
        }
        else {
            m_myProcess->deleteLater();
            //lint -esym(423, ExternalProcessControl::ExternalProcess::m_myProcess)
            m_myProcess = NULL;
            return true;
        }
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief    This function kills external process.
 *
 *            Use this function to unconditionally kill the (hanging)
 *            external process.
 *
 *  \warning  This function might block for PROCESS_KILL_TIMEOUT milliseconds
 *            if the process does not want to die immediately.
 *
 *
 *  \return   true if system command was executed, false otherwise
 *  \warning  FALSE will be returned by the "waitForFinished" if external
 *            process is already dead (e.g. we get "FALSE" not because the
 *            call failed, but because there is nothing to kill anymore)
 *
 ****************************************************************************/
bool ExternalProcess::KillProcess()
{
    if (m_myProcess != NULL) {
        qDebug() << (QString)("\nExternalProcess: trying to KILL " + m_myName + " process...\n");
        static_cast<void>(
            // cannot use return value here anyway
            this->disconnect()
        );
        m_myProcess->kill();
        m_myProcess->waitForFinished(5000);
        m_myProcess->deleteLater();
        m_myProcess = NULL;
        return true;
    }
    return false;
}

} // end namespace ExternalProcessControl
