/****************************************************************************/
/*! \file CmdAgitationReferenceRun.h
 *
 *  \brief Definition file for class CmdAgitationReferenceRun.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDAGITATIONREFERENCERUN_H
#define DEVICECOMMANDPROCESSOR_CMDAGITATIONREFERENCERUN_H

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
class CmdAgitationReferenceRun : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    //CmdAgitationReferenceRun();                                                    ///< Not implemented.
    CmdAgitationReferenceRun(const CmdAgitationReferenceRun &);                       ///< Not implemented.
    const CmdAgitationReferenceRun & operator = (const CmdAgitationReferenceRun &);   ///< Not implemented.
protected:
public:
    static QString                  NAME;              ///< Command name.

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * Constructor. It uses its own timeout which depends only on hardware.
     *
     */
    /****************************************************************************/
    CmdAgitationReferenceRun();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdAgitationReferenceRun();
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

#endif // DEVICECOMMANDPROCESSOR_CMDAGITATIONREFERENCERUN_H

