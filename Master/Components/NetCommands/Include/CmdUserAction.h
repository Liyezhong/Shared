/****************************************************************************/
/*! \file CmdUserAction.cpp
 *
 *  \brief Definition of CmdUserACtion
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-04-11
 *  $Author:    $ N.Kamath
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef CMDUSERACTION_H
#define CMDUSERACTION_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/**
 * \brief  Sent from Main thread to GPIO thread to inform that user actions
 *         like BLG/Import etc are in progress
 *
 */
/****************************************************************************/
class CmdUserAction : public Global::Command {
private:
    /****************************************************************************/
    CmdUserAction(const CmdUserAction &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdUserAction & operator = (const CmdUserAction &); ///< Not implemented.

protected:
public:
    static QString  NAME;           ///< Command name.
    quint32 m_UserActionID;         ///< User Action ID
    bool m_UserActionCompleted;     ///< true indicates user action is completed
    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdUserAction();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   TimeOut                 Timeout for command.
     * \iparam   CurrentUserActionID     EventString(Translated) of user action
     * \iparam      UserActionCompleted     true if user action completed, else false
     */
    /****************************************************************************/
    CmdUserAction(int TimeOut, quint32 CurrentUserActionID, bool UserActionCompleted);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdUserAction();
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
}; // end class CmdUserAction

}
#endif // CMDUSERACTION_H
