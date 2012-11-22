/****************************************************************************/
/*! \file CmdHeatStation.h
 *
 *  \brief Definition file for class CmdHeatStation.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDHEATSTATION_H
#define DEVICECOMMANDPROCESSOR_CMDHEATSTATION_H

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
class CmdHeatStation : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdHeatStation();                                                   ///< Not implemented.
    CmdHeatStation(const CmdHeatStation &);                     ///< Not implemented.
    const CmdHeatStation & operator = (const CmdHeatStation &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    QString m_Station;
    double m_targetTemperature;

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdHeatStation(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdHeatStation();
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
}; // end class CmdHeatStation

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDHEATSTATION_H
