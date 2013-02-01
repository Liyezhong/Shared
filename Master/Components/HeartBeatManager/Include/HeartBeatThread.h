/****************************************************************************/
/*! \file Include/HeartBeatManagerThreadController.h
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
#ifndef HEARTBEATTHREAD_H
#define HEARTBEATTHREAD_H
//QT Headers
#include <QObject>

//Project Headers
#include <Global/Include/GlobalDefines.h>
#include <Threads/Include/ThreadController.h>

namespace HeartBeatManager {

/****************************************************************************/
/**
 * \brief This Class shall be responsible to moniter the HearBeatPackets sent
 *
 */
/****************************************************************************/
class HeartBeatThread:public QObject
{
    Q_OBJECT
public:
    HeartBeatThread();
    virtual ~HeartBeatThread();
private:
    int     m_HeartbeatCheckTimeout;     ///< Timeout for checking controller heartbeat. Default = 3000 ms. 0 means no heartbeat signal check is done!
    QTimer  m_HeartbeatCheckTimer;       ///< Timer for controller heartbeat check.

private slots:
    void Run();

    /****************************************************************************/
    /**
     * \brief Check if all controllers have send their heartbeat signals.
     */
    /****************************************************************************/
    void HeartbeatCheck();


public slots:
    /****************************************************************************/
    /**
     * \brief Receive a heartbeat signals.
     *
     * \param[in]   TheHeartBeatSource    Logging source of sender.
     */
    /****************************************************************************/
    void HeartbeatSlot(const Global::gSourceType &TheHeartBeatSource);

signals:



};

} // end namespace HeartBeatManager


#endif // HEARTBEATTHREAD_H
