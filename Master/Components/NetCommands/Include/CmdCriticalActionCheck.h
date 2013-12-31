/****************************************************************************/
/*! \file CmdCriticalActionCheck.h
 *
 *  \brief CmdCriticalActionCheck command definition.
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

#ifndef CMDCRITICALACTIONCHECK_H
#define CMDCRITICALACTIONCHECK_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdCriticalActionCheck : public Global::Command {
private:
    /****************************************************************************/
    CmdCriticalActionCheck(const CmdCriticalActionCheck &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdCriticalActionCheck & operator = (const CmdCriticalActionCheck &); ///< Not implemented.

protected:

public:
    static QString  NAME;           ///< Command name.

    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdCriticalActionCheck();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   TimeOut       Timeout for command.
     */
    /****************************************************************************/
    CmdCriticalActionCheck(int TimeOut);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdCriticalActionCheck();
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
}; // end class CmdCriticalActionCheck
}
#endif // CMDCRITICALACTIONCHECK_H
