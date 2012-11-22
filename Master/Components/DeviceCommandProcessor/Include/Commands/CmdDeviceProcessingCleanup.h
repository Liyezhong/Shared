/****************************************************************************/
/*! \file CmdDeviceProcessingCleanup.h
 *
 *  \brief Definition file for class CmdDeviceProcessingCleanup.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDDEVICEPROCESSINGCLEANUP_H
#define DEVICECOMMANDPROCESSOR_CMDDEVICEPROCESSINGCLEANUP_H

#include <Global/Include/Commands/Command.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdDeviceProcessingCleanup : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdDeviceProcessingCleanup(const CmdDeviceProcessingCleanup &);                     ///< Not implemented.
    const CmdDeviceProcessingCleanup & operator = (const CmdDeviceProcessingCleanup &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   Timeout             Desired timeout.
     */
    /****************************************************************************/
    CmdDeviceProcessingCleanup();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdDeviceProcessingCleanup();
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
}; // end class CmdDeviceProcessingCleanup

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDDEVICEPROCESSINGCLEANUP_H
