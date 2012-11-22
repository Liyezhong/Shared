/****************************************************************************/
/*! \file CmdCoverStateChanged.h
 *
 *  \brief Definition file for class CmdCoverStateChanged.
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
#ifndef CMDCOVERSTATECHANGED_H
#define CMDCOVERSTATECHANGED_H

#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControlGlobal.h>
#include <Global/Include/GlobalDefines.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief This command is sent whenever hood cover is opened or closed
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdCoverStateChanged : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdCoverStateChanged();                                                   ///< Not implemented.
    CmdCoverStateChanged(const CmdCoverStateChanged &);                     ///< Not implemented.
    const CmdCoverStateChanged & operator = (const CmdCoverStateChanged &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    Global::CoverState  m_CoverState; ///< Hood cover state- open/closed
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdCoverStateChanged(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdCoverStateChanged();
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
}; // end class CmdCoverStateChanged

} // end namespace DeviceCommandProcessor

#endif // CMDCOVERSTATECHANGED_H

