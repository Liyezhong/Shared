/****************************************************************************/
/*! \file Include/HeartBeatCSVInfo.h
 *
 *  \brief Definition file for class EventCSVInfo.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-04-21
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

#ifndef HEARTBEATMANAGER_HEARTBEATCSVINFO_H
#define HEARTBEATMANAGER_HEARTBEATCSVINFO_H


#include <Global/Include/Utils.h>
#include <QStringList>

//lint -sem(HeartBeatManager::HeartBeatCSVInfo::CopyFrom,initializer)

namespace Global{

/****************************************************************************/
/**
 * \brief Enum containing all known action Types for heartbeat.
 */
/****************************************************************************/

enum HeartBeatActionType {
    HEARTBEATACNTYPE_SHUTDOWN,   ///< Proceed for System Shutdown
    HEARTBEATACNTYPE_RESTART,     ///< Proceed for System Reboot
    HEARTBEATACNTYPE_NONE           ///< No action
};
}

namespace HeartBeatManager{

/****************************************************************************/
/**
 * \brief Class used to store one lien from HeartBeatCSVInfo.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class HeartBeatCSVInfo {

public:

protected:
    quint32                 m_ThreadId;             ///< Logging thread id.
    QString                 m_ThreadMacroName;      ///< Name of thread id macro
    QString                 m_ThreadName;           ///< Name of thread
    Global::HeartBeatActionType      m_Action;                ///< Action to take when heartbeat misses
    quint8                  m_WaitDurationInHeartBeatTick;       ///< wait duration to take action if heartbeat is notreceived

    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const HeartBeatCSVInfo &rOther);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    HeartBeatCSVInfo();
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    HeartBeatCSVInfo(const HeartBeatCSVInfo &rOther);
    /****************************************************************************/

    /****************************************************************************/
    /**
     * \brief Constructor with a translatable string.
     *
     * \iparam   ThreadId
     * \iparam   ThreadMacroName
     * \iparam   ThreadName
     * \iparam   Action
     * \iparam   WaitDurationInHeartBeatTick
     */
    /****************************************************************************/
    HeartBeatCSVInfo(const quint32 &ThreadId, const QString &ThreadMacroName,
                 const QString &ThreadName, const Global::HeartBeatActionType Action,
                 const quint8 &WaitDurationInHeartBeatTick) ;

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~HeartBeatCSVInfo();

    /****************************************************************************/
    /**
     * \brief Get the Action
     *
     * \return  Action.
     */
    /****************************************************************************/
    inline Global::HeartBeatActionType GetAction() const {
        return m_Action;
    }

    /****************************************************************************/
    /**
     * \brief Set the Action
     *
     * \iparam  Action
     */
    /****************************************************************************/
    inline void SetAction(const Global::HeartBeatActionType Action) {
        m_Action = Action;
    }


    /****************************************************************************/
    /**
     * \brief Get the WaitDurationInHeartBeatTick
     *
     * \return  WaitDurationInHeartBeatTick
     */
    /****************************************************************************/
    inline quint8 GetWaitDurationInHeartBeatTick() const {
        return m_WaitDurationInHeartBeatTick;
    }

    /****************************************************************************/
    /**
     * \brief Set the WaitDurationInHeartBeatTick
     *
     * \iparam  WaitDurationInHeartBeatTick
     */
    /****************************************************************************/
    inline void SetWaitDurationInHeartBeatTick(const quint8 WaitDurationInHeartBeatTick) {
        m_WaitDurationInHeartBeatTick = WaitDurationInHeartBeatTick;
    }

    /****************************************************************************/
    /**
     * \brief Get the ThreadName
     *
     * \return  ThreadName
     */
    /****************************************************************************/
    inline QString GetThreadName() const {
        return m_ThreadName;
    }

    /****************************************************************************/
    /**
     * \brief Set the ThreadName
     *
     * \iparam  ThreadName
     */
    /****************************************************************************/
    inline void SetThreadName(const QString ThreadName) {
        m_ThreadName = ThreadName;
    }

    /****************************************************************************/
    /**
     * \brief Get the ThreadMacroName
     *
     * \return  ThreadMacroName
     */
    /****************************************************************************/
    inline QString GetThreadMacroName() const {
        return m_ThreadMacroName;
    }

    /****************************************************************************/
    /**
     * \brief Set the ThreadMacroName
     *
     * \iparam  ThreadMacroName
     */
    /****************************************************************************/
    inline void SetThreadMacroName(const QString ThreadMacroName) {
        m_ThreadMacroName = ThreadMacroName;
    }

    /****************************************************************************/
    /**
     * \brief Get the thread id
     *
     * \return  ThreadId
     */
    /****************************************************************************/
    inline quint32 GetThreadId() const {
        return m_ThreadId;
    }

    /****************************************************************************/
    /**
     * \brief Set the thread id
     *
     * \iparam  ThreadId
     */
    /****************************************************************************/
    inline void SetThreadId(const quint32 ThreadId) {
        m_ThreadId = ThreadId;
    }

}; // end class HeartBeatCSVInfo

} // end namespace HeartBeatManager

#endif // HEARTBEATMANAGER_HEARTBEATCSVINFO_H

