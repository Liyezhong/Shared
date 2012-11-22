/****************************************************************************/
/*! \file CmdStationState.h
 *
 *  \brief Definition file for class CmdStationState.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDSTATIONSTATE_H
#define DEVICECOMMANDPROCESSOR_CMDSTATIONSTATE_H

#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControlGlobal.h>
#include <Global/Include/GlobalDefines.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdStationState : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdStationState();                                                   ///< Not implemented.
    CmdStationState(const CmdStationState &);                     ///< Not implemented.
    const CmdStationState & operator = (const CmdStationState &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    QString m_Station;                      ///< Station  ID
    Global::HeatingState m_HeatingState;    ///< Heated cuvette/Oven Heating state
    quint32 m_Temperature;                  ///< Heated cuvette/Oven Temperature
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdStationState(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdStationState();
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
}; // end class CmdStationState

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDSTATIONSTATE_H
