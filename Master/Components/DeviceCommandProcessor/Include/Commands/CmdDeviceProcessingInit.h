/****************************************************************************/
/*! \file CmdDeviceProcessingInit.h
 *
 *  \brief Definition file for class CmdDeviceProcessingInit.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-01
 *  $Author:    $ Norbert Wiedmann
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

#ifndef DEVICECOMMANDPROCESSOR_CMDDEVICEPROCESSINGINIT_H
#define DEVICECOMMANDPROCESSOR_CMDDEVICEPROCESSINGINIT_H

#include <Global/Include/Commands/Command.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdDeviceProcessingInit : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdDeviceProcessingInit(const CmdDeviceProcessingInit &);                       ///< Not implemented.
    const CmdDeviceProcessingInit & operator = (const CmdDeviceProcessingInit &);   ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * Constructor. It uses its own timeout which depends only on hardware.
     */
    /****************************************************************************/
    CmdDeviceProcessingInit();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdDeviceProcessingInit();
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
}; // end class CmdDeviceProcessingInit

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDDEVICEPROCESSINGINIT_H
