/****************************************************************************/
/*! \file CmdSwitchToService.h
 *
 *  \brief CmdSwitchToService command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.03.2011
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

#ifndef NETCOMMANDS_CMDSWITCHTOSERVICE_H
#define NETCOMMANDS_CMDSWITCHTOSERVICE_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdSwitchToService command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdSwitchToService : public Global::Command {
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdSwitchToService(int Timeout, const QString &Pin);
    ~CmdSwitchToService();
    virtual QString GetName() const;
    QString GetPin() const;
private:
    CmdSwitchToService();                                               ///< Not implemented.
    CmdSwitchToService(const CmdSwitchToService &);                     ///< Not implemented.
    const CmdSwitchToService & operator = (const CmdSwitchToService &); ///< Not implemented.
private:
    QString      m_Pin;     ///< Pin.
}; // end class CmdSwitchToService

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDSWITCHTOSERVICE_H
