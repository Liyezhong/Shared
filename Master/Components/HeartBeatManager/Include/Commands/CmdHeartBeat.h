/****************************************************************************/
/*! \file HeartBeatManager/Include/Commands/CmdHeartBeat.h
 *
 *  \brief Definition file for class CmdHeartBeat.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-04-18
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

#ifndef HEARTBEATMANAGER_CMDHEARTBEAT_H
#define HEARTBEATMANAGER_CMDHEARTBEAT_H

#include <Global/Include/Commands/Command.h>

namespace HeartBeatManager {

/****************************************************************************/
/**
 * \brief Command heart beat signal.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdHeartBeat : public Global::Command {
private:
    quint32   m_ThreadId;                 ///< logging thread id for heartbeat check.
    /****************************************************************************/
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    CmdHeartBeat();                                              ///< Not implemented.

    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \iparam   Other  Const reference to other instance.
     *
     */
    /****************************************************************************/
    CmdHeartBeat(const CmdHeartBeat &Other); ///< Not implemented.

    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \iparam   Other  Const reference to other instance.
     *
     * \return   Const reference to self.
     */
    /****************************************************************************/
    const CmdHeartBeat & operator = (const CmdHeartBeat &Other);   ///< Not implemented.

protected:
public:
    static QString  NAME;       ///< Command name.
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   Timeout         Timeout for command.
     * \iparam   ThreadId        logging thread id for heartbeat check..
     */
    /****************************************************************************/
    CmdHeartBeat(int Timeout, quint32 ThreadId);

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdHeartBeat();

    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command name.
     */
    /****************************************************************************/
    virtual QString GetName() const {
        return NAME;
    }

    /****************************************************************************/
    /**
     * \brief Get caching mode.
     *
     * \return  Caching mode.
     */
    /****************************************************************************/
    inline quint32 GetThreadId() const {
        return m_ThreadId;
    }
}; // end class CmdHeartBeat

} // end namespace HeartBeatManager

#endif // HEARTBEATMANAGER_CMDHEARTBEAT_H
