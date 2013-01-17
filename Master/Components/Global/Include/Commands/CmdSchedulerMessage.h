/****************************************************************************/
/*! \file CmdSchedulerMessage.h
 *
 *  \brief Definition file for class CmdSchedulerMessage.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-06-13
 *  $Author:    $ Michael Thiel
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

#ifndef GLOBAL_CMDSSCHEDULERMESSAGE_H
#define GLOBAL_CMDSSCHEDULERMESSAGE_H

#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControl.h>

namespace Global {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdSchedulerMessage : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdSchedulerMessage(const CmdSchedulerMessage &);                     ///< Not implemented.
    const CmdSchedulerMessage & operator = (const CmdSchedulerMessage &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    qint16 m_MessageId;
    qint64 m_Ref;

    CmdSchedulerMessage();

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdSchedulerMessage();
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
}; // end class CmdSchedulerMessage

} // end namespace Global

#endif // GLOBAL_CMDSSCHEDULERMESSAGE_H
