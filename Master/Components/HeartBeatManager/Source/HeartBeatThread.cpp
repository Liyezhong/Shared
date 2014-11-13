/****************************************************************************/
/*! \file HeartBeatThread.cpp
 *
 *  \brief  HeartBeatManager Implementation
 *
 *   $Version: $ 0.1
 *   $Date:    $ 15.11.2012
 *   $Author:  $ Shuvasmita.S
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
//QT Headers
#include <QMetaType>
#include <QDebug>
//Project Headers
#include <HeartBeatManager/Include/HeartBeatThread.h>
#include <Threads/Include/MasterThreadController.h>
#include <Global/Include/Utils.h>
#include <Global/Include/Commands/CmdShutDown.h>

namespace HeartBeatManager{


/****************************************************************************/
/*!
 *  \brief    Constructor for the HeartBeatManagerThreadController
 *
 *  \iparam   ThreadID
 *
 *  \return
 *
 ****************************************************************************/
HeartBeatThreadController::HeartBeatThreadController(quint32 ThreadID)
    :  Threads::ThreadController(ThreadID, "HeartBeatCheck"),
      m_HeartbeatCheckTimeout(HEARTBEAT_CHECK_TIMEOUT), m_HeartbeatCheckTimer(this),
      m_ControllerHeartbeatTimeout(CONTROLLER_HERATBEAT_TIMEOUT),
      m_HeartBeatMutex(QMutex::Recursive),
      m_StopFlag(false)
{
    m_ActionTypeMap.insert("RESTART", Global::HEARTBEATACNTYPE_RESTART);
    m_ActionTypeMap.insert("SHUTDOWN", Global::HEARTBEATACNTYPE_SHUTDOWN);
    m_ActionTypeMap.insert("", Global::HEARTBEATACNTYPE_NONE);
}

/****************************************************************************/
/**
 * \brief Create and configure the loggers.
 *
 * If something goes wrong, the already created loggers are deleted.
 */
/****************************************************************************/
void HeartBeatThreadController::CreateAndInitializeObjects()
{
    try
    {
        ReadHeartBeatConfigFile(Global::SystemPaths::Instance().GetSettingsPath() + "/HeartBeatConfig.csv");

        // connect heartbeat check timer
        CONNECTSIGNALSLOT(&m_HeartbeatCheckTimer, timeout(), this, HeartbeatCheck());

    }
    CATCHALL();
}

/****************************************************************************/
/**
 * \brief Cleanup and destroy the loggers.
 */
/****************************************************************************/
void HeartBeatThreadController::CleanupAndDestroyObjects()
{
}

/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Go signal.
 *
 * This means that everything is fine and normal operation started.
 * We are running in our own thread now.\n
 * We do not have anything special to do, since objects are already configured
 * and ready for normal operation.
 */
/****************************************************************************/
void HeartBeatThreadController::OnGoReceived()
{
    StartHeartBeatCheckTimer();
}

/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Stop signal.
 *
 * This means that normal operation will stop after processing this signal.
 * We are still running in our own thread.\n
 * We do not have anything special to do.
 */
/****************************************************************************/
void HeartBeatThreadController::OnStopReceived()
{
    StopHeartBeatCheckTimer();
}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 *
 * We try to log the power fail into the event logger and close all open files.
 * After that we switch to a PowerFail state where only events are processed
 * and dumped to console.
 *
 * \iparam PowerFailStage = power fail stage information
 */
/****************************************************************************/
void HeartBeatThreadController::OnPowerFail(const Global::PowerFailStages PowerFailStage)
{
    Q_UNUSED(PowerFailStage)
    StopHeartBeatCheckTimer();
}

/****************************************************************************/
/**
 * \brief start heartbeat check timer only if m_HeartbeatCheckTimeout > 0.
 */
/****************************************************************************/
void HeartBeatThreadController::StartHeartBeatCheckTimer()
{
    QMutexLocker Lock(&m_HeartBeatMutex);
    if(m_HeartbeatCheckTimeout > 0)
    {
        if ( m_HeartbeatCheckTimer.interval() > 0) {
            m_HeartbeatCheckTimer.start();
        }
        else {
            m_HeartbeatCheckTimer.start(m_HeartbeatCheckTimeout);
        }
    }
    else
    {
        Global::EventObject::Instance().RaiseEvent(Threads::EVENT_THREADS_INFO_NO_HEARTBEAT_CHECKING);
    }
}

/****************************************************************************/
/**
 * \brief stop heartbeat check timer .
 */
/****************************************************************************/
void HeartBeatThreadController::StopHeartBeatCheckTimer()
{
    QMutexLocker Lock(&m_HeartBeatMutex);
    m_HeartbeatCheckTimer.stop();
    m_ArrivedHeartbeats.clear();
}

/****************************************************************************/
/**
 * \brief Add controllers for HeartBeat Check .
 *
 *  \iparam   ThreadID    thread id of controller to be stored for heartbeat check.
 */
/****************************************************************************/
void HeartBeatThreadController::AddControllerForHeartBeatCheck(quint32 ThreadID)
{
    m_HeartBeatMutex.lock();
    // remember its ID for heartbeat checks
    m_HeartbeatSources.insert(ThreadID);
    m_HeartBeatMissingCountHash.insert(ThreadID, 0);
    m_HeartBeatMutex.unlock();
}
/****************************************************************************/
/**
 * \brief Remove controllers from HeartBeat Check .
 *
 *  \iparam ThreadId thread id of controller to be removed from heartbeat check.
 */
/****************************************************************************/
void HeartBeatThreadController::RemoveControllerForHeartBeatCheck(quint32 ThreadId)
{
    m_HeartBeatMutex.lock();

    if(m_HeartbeatSources.contains(ThreadId))
    {
        m_HeartbeatSources.remove(ThreadId);
        /// Some threads are created on demand. Suppose if the thread is removed from the source
        /// at the same time thread received the heart beat then it logs unknown heartbeat.
        /// To avoid this situation add it in temporary variable and remove
        /// the data from the arrived heart beat source
        m_RemovedHeardBeats.insert(ThreadId);

        m_HeartBeatMissingCountHash.remove(ThreadId);
    }

    m_HeartBeatMutex.unlock();
}

/****************************************************************************/
/*!
 *  \brief    Destructor for the HeartBeatManagerThreadController
 *
 *  \return
 *
 ****************************************************************************/
HeartBeatThreadController::~HeartBeatThreadController()
{

}

/****************************************************************************/
/*!
 *  \brief    Sets the HeartBeat Check Timer
 *
 */
 /****************************************************************************/
void HeartBeatThreadController::SetHeartBeatCheckTimerValue(int Timeout)
{
    QMutexLocker Lock(&m_HeartBeatMutex);
    m_HeartbeatCheckTimeout=Timeout;
}

/****************************************************************************/
/*!
 *  \brief Configures heartbeat check. If flag is set heart beat is not checked.
 *  \iparam Flag =  set to true for stopping heartbeat check.
 *
 */
 /****************************************************************************/
void HeartBeatThreadController::DontCheckHeartBeat(const bool Flag)
{
    QMutexLocker Lock(&m_HeartBeatMutex);
    m_StopFlag = Flag;
}

/****************************************************************************/
/**
 * \brief Check if all controllers have sent their heartbeat signals.
 */
/****************************************************************************/
void HeartBeatThreadController::HeartbeatCheck()
{
    QMutexLocker Lock(&m_HeartBeatMutex);
    if (m_StopFlag) {
        return;
    }
    // check if all registered controllers have send their heartbeat signal
    if(m_HeartbeatSources == m_ArrivedHeartbeats)
    {
        // everything OK
        // clear arrived heartbeat sources
        m_ArrivedHeartbeats.clear();
        return;
    }
    // check missing sources, dont consider recently added heart beat sources,
    QSet<quint32> Missing = m_HeartbeatSources - m_ArrivedHeartbeats - m_RecentlyAddedHeardBeats;
    m_RecentlyAddedHeardBeats.clear();

    // check for unknown heart beats
    QSet<quint32> NotRegistered = m_ArrivedHeartbeats - m_HeartbeatSources - m_RemovedHeardBeats;
    m_RemovedHeardBeats.clear();

    // clear arrived heartbeat sources
    m_ArrivedHeartbeats.clear();

    for(QSet<quint32>::iterator it = Missing.begin(); it != Missing.end(); ++it)
    {
        OnMissingHeartBeats(*it);
    }

    for(QSet<quint32>::iterator it = NotRegistered.begin(); it != NotRegistered.end(); ++it)
    {
        Global::TranslatableString SourceString(*it);
        qDebug() << SourceString.GetString();
        Global::EventObject::Instance().RaiseEvent(Threads::EVENT_THREADS_ERROR_UNKNOWN_HEARTBEAT,Global::tTranslatableStringList() <<SourceString);
    }
}

/****************************************************************************/
/*!
 *  \brief    File parser for the heartbeatconfig.csv file
 *
 *  \param    filename
 *
 *  \return   Success or failure
 */
 /****************************************************************************/
bool HeartBeatThreadController::ReadHeartBeatConfigFile(QString filename)
{
    QMutexLocker Lock(&m_HeartBeatMutex);
    qDebug()<<"HeartBeatConfigFile path"<<filename;
    QFile file(filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"HeartBeatConfigFile File Open Failed" << file.errorString();
        return false;
    }

    m_HeartBeatInfoHash.clear();

    QTextStream stream ( &file );
    QString line;

    //required once the event is in table, even if nothing is specified in the required column
    while( !stream.atEnd() )
    {
        line = stream.readLine();
        if (line.left(1) != "#")
        {
            QStringList textList = line.split(",");
            HeartBeatManager::HeartBeatCSVInfo HeartBeatCsvInfo;

            if (textList.count() > 0)
            {
                //Get thread id
                quint32 ThreadId = textList.at(0).toInt();

                HeartBeatCsvInfo.SetThreadId(ThreadId); // only int check, no other validations done
            }


            //Get thread name macro
            if (textList.count() > 1)
            {
                QString ThreadMacroName = "";

                ThreadMacroName = textList.at(1).trimmed(); // No validations required

                HeartBeatCsvInfo.SetThreadMacroName(ThreadMacroName);
            }

            //Get thread name
            if (textList.count() > 2)
            {
                QString ThreadName = "";

                ThreadName = textList.at(2).trimmed(); // No validations required

                HeartBeatCsvInfo.SetThreadName(ThreadName);
            }

            //Get wait duration
            if (textList.count() > 3)
            {
                HeartBeatCsvInfo.SetAction(m_ActionTypeMap.value(textList.at(3).trimmed().toUpper(), Global::HEARTBEATACNTYPE_NONE));
            }

            //Get wait duration
            if (textList.count() > 4)
            {
                //Get thread id
                quint8 WaitDurationInHeartBeatTick = textList.at(4).toInt();

                HeartBeatCsvInfo.SetWaitDurationInHeartBeatTick(WaitDurationInHeartBeatTick); // only int check, no other validations done
            }

            m_HeartBeatInfoHash.insert(HeartBeatCsvInfo.GetThreadId(), HeartBeatCsvInfo);
        }
    }

    file.close();

    return true;
}

/****************************************************************************/
/*!
 *  \brief    take actions when heartbeat is not received from any thread
 *
 *  \param    ThreadId
 *
 ****************************************************************************/
void HeartBeatThreadController::OnMissingHeartBeats(quint32 ThreadId)
{
#if 0
    Global::EventObject::Instance().RaiseEvent(Threads::EVENT_THREADS_ERROR_NO_HEARTBEAT, Global::FmtArgs() << ThreadId);
    //send thread shutdown command to MasterThreadController
    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(new Global::CmdShutDown(true)));

    //increase the number of counts that the heartbeat has been missed from this controller
    if(m_HeartBeatInfoHash.contains(ThreadId))
    {


        HeartBeatCSVInfo HeartBeatInfo = m_HeartBeatInfoHash.value(ThreadId);

        if(m_HeartBeatMissingCountHash.contains(ThreadId))
        {
            qint8 MissingCount = m_HeartBeatMissingCountHash.value(ThreadId);
            MissingCount++;

            if(MissingCount > HeartBeatInfo.GetWaitDurationInHeartBeatTick())
            {
                if(Global::HEARTBEATACNTYPE_RESTART == HeartBeatInfo.GetAction())
                {
                    //remove this thread id from the list, so that this is not counted till it is registered next time

                    //send thread restart command to MasterThreadController
                    m_HeartBeatMissingCountHash.remove(ThreadId);

                    Global::EventObject::Instance().RaiseEvent(Threads::EVENT_THREADS_ERROR_NO_HEARTBEAT_RESTART, Global::FmtArgs() <<HeartBeatInfo.GetThreadName());

                    return;
                }
                else if(Global::HEARTBEATACNTYPE_SHUTDOWN == HeartBeatInfo.GetAction())
                {
                    //clear the list, so that none of the threads are counted
                    m_HeartBeatMissingCountHash.clear();

                    //log the message
                    Global::EventObject::Instance().RaiseEvent(Threads::EVENT_THREADS_ERROR_NO_HEARTBEAT_SHUTDOWN, Global::FmtArgs() <<HeartBeatInfo.GetThreadName());

                    //send thread shutdown command to MasterThreadController
                    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(new Global::CmdShutDown()));

                    return;
                }
            }
            else
            {
                m_HeartBeatMissingCountHash.remove(ThreadId);
                m_HeartBeatMissingCountHash.insert(ThreadId, MissingCount);
            }
        }

        Global::EventObject::Instance().RaiseEvent(Threads::EVENT_THREADS_ERROR_NO_HEARTBEAT, Global::FmtArgs() << ThreadId);

    }
    else
    {
        Global::EventObject::Instance().RaiseEvent(Threads::EVENT_THREADS_ERROR_UNKNOWN_HEARTBEAT, Global::FmtArgs() <<ThreadId);
    }
    #endif
}

/****************************************************************************/
/**
 * \brief Receive a heartbeat signals.
 *
 * \iparam   ThreadId    thread ID.
 */
/****************************************************************************/
void HeartBeatThreadController::HeartBeatSlot(quint32 ThreadId)
{
    m_HeartBeatMutex.lock();
    if(!m_ArrivedHeartbeats.contains(ThreadId))
    {
        // remember received heartbeat logging source.
        m_ArrivedHeartbeats.insert(ThreadId);
        //qDebug()<<"heart beat received from"<<ThreadId;
    }
    m_HeartBeatMutex.unlock();
}


} //End Of namespace HeartBeatManager
