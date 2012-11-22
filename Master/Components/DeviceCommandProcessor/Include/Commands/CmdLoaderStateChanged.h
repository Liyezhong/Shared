/****************************************************************************/
/*! \file CmdLoaderStateChanged.h
 *
 *  \brief Definition file for class CmdLoaderStateChanged.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDLOADERSTATECHANGED_H
#define DEVICECOMMANDPROCESSOR_CMDLOADERSTATECHANGED_H

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
class CmdLoaderStateChanged : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdLoaderStateChanged();                                                   ///< Not implemented.
    CmdLoaderStateChanged(const CmdLoaderStateChanged &);                     ///< Not implemented.
    const CmdLoaderStateChanged & operator = (const CmdLoaderStateChanged &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    bool m_Open;
    bool m_Locked;

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdLoaderStateChanged(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdLoaderStateChanged();
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
}; // end class CmdLoaderStateChanged

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDLOADERSTATECHANGED_H
