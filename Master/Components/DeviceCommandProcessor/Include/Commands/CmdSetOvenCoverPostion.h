/****************************************************************************/
/*! \file CmdSetOvenCoverPostion.h
 *
 *  \brief Definition file for class CmdSetOvenCoverPostion.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-19
 *  $Author:    $ N.Wiedmann
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

#ifndef DEVICECOMMANDPROCESSOR_CMDSETOVENCOVERPOSTION_H
#define DEVICECOMMANDPROCESSOR_CMDSETOVENCOVERPOSTION_H

#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControlGlobal.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdSetOvenCoverPostion : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdSetOvenCoverPostion(const CmdSetOvenCoverPostion &);                       ///< Not implemented.
    const CmdSetOvenCoverPostion & operator = (const CmdSetOvenCoverPostion &);   ///< Not implemented.
protected:
public:
    static QString                       NAME;              ///< Command name.

    DeviceControl::OvenCoverPosition_t   m_OvenCoverPosition;   ///< Requested cover position

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * Constructor. It uses its own timeout which depends only on hardware.
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     *
     */
    /****************************************************************************/
    CmdSetOvenCoverPostion();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdSetOvenCoverPostion();
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
}; // end class CmdSetOvenCoverPostion

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDSETOVENCOVERPOSTION_H

