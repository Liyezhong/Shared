/****************************************************************************/
/*! \file CmdProblemReportReply.h
 *
 *  \brief CmdProblemReportReply command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.03.2011
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

#ifndef NETCOMMANDS_CMDPROBLEMREPORTREPLY_H
#define NETCOMMANDS_CMDPROBLEMREPORTREPLY_H

#include <Global/Include/Commands/Command.h>

#include <QStringList>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdProblemReportReply command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdProblemReportReply : public Global::Command {
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdProblemReportReply(int Timeout, const QStringList &Report);
    ~CmdProblemReportReply();
    virtual QString GetName() const;
    QStringList GetReport() const;
private:
    CmdProblemReportReply();                                                    ///< Not implemented.
    CmdProblemReportReply(const CmdProblemReportReply &);                       ///< Not implemented.
    const CmdProblemReportReply & operator = (const CmdProblemReportReply &);   ///< Not implemented.
private:
    QStringList     m_Report;   ///< The prblem report reply.
}; // end class CmdProblemReportReply

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDPROBLEMREPORTREPLY_H
