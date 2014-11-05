/****************************************************************************/
/*! \file Include/HeartBeatThread.h
 *
 *  \brief  Definition of class HeartBeatManager
 *
 *  Version:    0.1
 *  Date:       2012-11-15
 *  Author:     Shuvasmita.S
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
#ifndef HEARTBEATTHREADCONTROLLER_H
#define HEARTBEATTHREADCONTROLLER_H
//QT Headers
#include <QObject>

//Project Headers
#include <Threads/Include/ThreadController.h>
#include <Global/Include/GlobalDefines.h>
#include <Threads/Include/ThreadController.h>
#include <../Include/PlatformEventCodes.h>
#include "../Components/Threads/Include/ThreadsEventCodes.h"
#include "QDebug"
#include <HeartBeatManager/Include/Commands/CmdAddControllerForHeartBeatCheck.h>
#include <HeartBeatManager/Include/Commands/CmdHeartBeat.h>
#include <HeartBeatManager/Include/Commands/CmdRemoveControllerForHeartBeatCheck.h>
#include <HeartBeatManager/Include/HeartBeatCSVInfo.h>



namespace HeartBeatManager {

const int HEARTBEAT_CHECK_TIMEOUT = 30000;       ///< Timeout for checking controller heartbeat i.e. 30000 ms..
const int CONTROLLER_HERATBEAT_TIMEOUT = 3000;  ///< Timeout for controller heartbeat i.e.3000 ms.

/****************************************************************************/
/**
 * \brief This Class shall be responsible to moniter the HearBeatPackets sent
 *
 */
/****************************************************************************/
class HeartBeatThreadController:public Threads::ThreadController
{
    Q_OBJECT
   friend class TestHeartBeatThread;
private:

       /****************************************************************************/
       /**
        * \brief Constructor.
        */
       /****************************************************************************/
       HeartBeatThreadController();               ///< Not implemented.

       /****************************************************************************/
       /*!
        *  \brief Disable copy and assignment operator.
        *
        */
       /****************************************************************************/
       Q_DISABLE_COPY(HeartBeatThreadController)

       bool ReadHeartBeatConfigFile(QString filename);

       void OnMissingHeartBeats(quint32 ThreadId);

       int                         m_HeartbeatCheckTimeout;            ///< Timeout for checking controller heartbeat. Default = 3000 ms. 0 means no heartbeat signal check is done!
       QTimer                      m_HeartbeatCheckTimer;              ///< Timer for controller heartbeat check.
       QSet<quint32>               m_HeartbeatSources;                 ///< All registered logging sources for heartbeat check.
       QSet<quint32>               m_ArrivedHeartbeats;                ///< All logging sources for which we received a heartbeat.
       QSet<quint32>               m_RemovedHeardBeats;                ///< Removed heart beats for shorter threads
       QSet<quint32>               m_RecentlyAddedHeardBeats;          ///< Recently added heart beats for shorter threads
       const int                   m_ControllerHeartbeatTimeout;       ///< Timeout for controller heartbeat. Default = 1000 ms. 0 means no heartbeat signal is send!
       QHash<quint32, HeartBeatManager::HeartBeatCSVInfo> m_HeartBeatInfoHash; ///< hash of heartbeatinfo against the thread id
       QHash<QString, Global::HeartBeatActionType> m_ActionTypeMap;             ///< enum map for action type and string
       QHash<quint32, quint8> m_HeartBeatMissingCountHash; ///< hash of number of times heartbeat missed against the thread id
       QMutex m_HeartBeatMutex; ///< Mutex
       bool m_StopFlag;

protected :

       virtual void OnGoReceived();
       virtual void OnStopReceived();
       virtual void OnPowerFail(const Global::PowerFailStages PowerFailStage);
public:

    HeartBeatThreadController(quint32 ThreadID);
    void RemoveControllerForHeartBeatCheck(quint32 ThreadId);
    void SetHeartBeatCheckTimerValue(int);
    void DontCheckHeartBeat(const bool Flag);
    virtual ~HeartBeatThreadController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

public slots:
    void HeartBeatSlot(quint32 ThreadId);

private slots:
    void HeartbeatCheck();
    void StartHeartBeatCheckTimer();
    void StopHeartBeatCheckTimer();
    void AddControllerForHeartBeatCheck(quint32 ThreadID);


signals:
    /****************************************************************************/
    /**
     * \brief signal to inform master thread controllers abt the missing heart beats.
     *
     * \iparam Missing = set of missing heart beat thread ids.
     */
    /****************************************************************************/
    void HeartBeatNotReceived(const QSet<quint32> Missing);


    /****************************************************************************/
    /**
     * \brief signal to request heartbeat of the thread controllers that they are alive.
     *
     */
    /****************************************************************************/
    void RequestHeartBeatOfThread();
};

} // end namespace HeartBeatManager


#endif // HEARTBEATTHREAD_H
