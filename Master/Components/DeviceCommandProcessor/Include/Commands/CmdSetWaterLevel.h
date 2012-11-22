/****************************************************************************/
/*! \file CmdSetWaterLevel.h
 *
 *  \brief Definition file for class CmdSetWaterLevel.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDSETWATERLEVEL_H
#define DEVICECOMMANDPROCESSOR_CMDSETWATERLEVEL_H

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
class CmdSetWaterLevel : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdSetWaterLevel();                                                   ///< Not implemented.
    CmdSetWaterLevel(const CmdSetWaterLevel &);                     ///< Not implemented.
    const CmdSetWaterLevel & operator = (const CmdSetWaterLevel &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    QString m_Station;
    bool m_fill;

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdSetWaterLevel(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdSetWaterLevel();
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
}; // end class CmdSetWaterLevel

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDSETWATERLEVEL_H
