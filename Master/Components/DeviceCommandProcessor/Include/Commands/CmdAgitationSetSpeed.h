/****************************************************************************/
/*! \file CmdAgitationSetSpeed.h
 *
 *  \brief Definition file for class CmdAgitationSetSpeed.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-21
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

#ifndef DEVICECOMMANDPROCESSOR_CMDAGITATIONSETSPEED_H
#define DEVICECOMMANDPROCESSOR_CMDAGITATIONSETSPEED_H

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
class CmdAgitationSetSpeed : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    //CmdAgitationSetSpeed();                                                    ///< Not implemented.
    CmdAgitationSetSpeed(const CmdAgitationSetSpeed &);                       ///< Not implemented.
    const CmdAgitationSetSpeed & operator = (const CmdAgitationSetSpeed &);   ///< Not implemented.
protected:
public:
    static QString                  NAME;              ///< Command name.

    DeviceControl::AgitationSpeed_t m_AgitationSpeed;   ///< Requested agitation speed

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * Constructor. It uses its own timeout which depends only on hardware.
     *
     */
    /****************************************************************************/
    CmdAgitationSetSpeed();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdAgitationSetSpeed();
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
}; // end class CmdLoaderReferenceRun

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDAGITATIONSETSPEED_H

