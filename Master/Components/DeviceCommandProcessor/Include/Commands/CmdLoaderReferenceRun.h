/****************************************************************************/
/*! \file CmdLoaderReferenceRun.h
 *
 *  \brief Definition file for class CmdLoaderReferenceRun.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-11
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

#ifndef DEVICECOMMANDPROCESSOR_CMDLOADERREFERENCERUN_H
#define DEVICECOMMANDPROCESSOR_CMDLOADERREFERENCERUN_H

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
class CmdLoaderReferenceRun : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdLoaderReferenceRun();                                                    ///< Not implemented.
    CmdLoaderReferenceRun(const CmdLoaderReferenceRun &);                       ///< Not implemented.
    const CmdLoaderReferenceRun & operator = (const CmdLoaderReferenceRun &);   ///< Not implemented.
protected:
public:
    static QString                  NAME;              ///< Command name.
    DeviceControl::DevInstanceID_t  m_DevInstanceID;   ///< Instance ID of the target device

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * Constructor. It uses its own timeout which depends only on hardware.
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     *
     */
    /****************************************************************************/
    CmdLoaderReferenceRun(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdLoaderReferenceRun();
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

#endif // DEVICECOMMANDPROCESSOR_CMDLOADERREFERENCERUN_H
