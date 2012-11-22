/****************************************************************************/
/*! \file CmdCountSlides.h
 *
 *  \brief Definition file for class CmdCountSlides.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDCOUNTSLIDES_H
#define DEVICECOMMANDPROCESSOR_CMDCOUNTSLIDES_H

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
class CmdCountSlides : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdCountSlides();                                                   ///< Not implemented.
    CmdCountSlides(const CmdCountSlides &);                     ///< Not implemented.
    const CmdCountSlides & operator = (const CmdCountSlides &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device
    quint32 m_rackId;

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdCountSlides(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdCountSlides();
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
}; // end class CmdCountSlides

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDCOUNTSLIDES_H
