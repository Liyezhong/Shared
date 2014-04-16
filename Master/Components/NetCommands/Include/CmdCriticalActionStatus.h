/****************************************************************************/
/*! \file CmdCriticalActionStatus.h
 *
 *  \brief CmdCriticalActionStatus command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-02-01
 *   $Author:  $ N.Kamath
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
#ifndef CMDCRITICALACTIONSTATUS_H
#define CMDCRITICALACTIONSTATUS_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/**
 * \brief Enum for Critical Action Status type.
 */
/****************************************************************************/
typedef enum {
    NO_CRITICAL_ACTION,
    CRITICAL_ACTION_IN_PROGRESS,
    CRITICAL_ACTION_COMPLETE
}CriticalActionStatus_t;
/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdCriticalActionStatus : public Global::Command {
private:
    /****************************************************************************/
    CmdCriticalActionStatus(const CmdCriticalActionStatus &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdCriticalActionStatus & operator = (const CmdCriticalActionStatus &); ///< Not implemented.

protected:
public:
    static QString  NAME;           ///< Command name.
    CriticalActionStatus_t m_CriticalActionStatus;  ///< Type of Critical action status
    bool    m_EvaluateCommand;        ///< true indicates evaluate the command, false indicates don't evaluate.
    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdCriticalActionStatus();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   TimeOut                 Timeout for command.
     * \iparam   CriticalActionStatus    Status of the CriticalAction
     */
    /****************************************************************************/
    CmdCriticalActionStatus(int TimeOut, CriticalActionStatus_t CriticalActionStatus);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdCriticalActionStatus();
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
}; // end class CmdCriticalActionStatus

}

#endif // CMDCRITICALACTIONSTATUS_H
