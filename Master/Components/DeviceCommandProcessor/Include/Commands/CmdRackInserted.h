/****************************************************************************/
/*! \file CmdRackInserted.h
 *
 *  \brief Definition file for class CmdRackInserted.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDRACKINSERTED_H
#define DEVICECOMMANDPROCESSOR_CMDRACKINSERTED_H

#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControlGlobal.h>
#include <DataManager/Containers/Racks/Include/Rack.h>
#include <DataManager/Containers/StainingPrograms/Include/Program.h>
#include <DataManager/Containers/StainingReagents/Include/DataReagentList.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief \todo [MW] comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdRackInserted : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdRackInserted();                                                    ///< Not implemented.
    CmdRackInserted(const CmdRackInserted &);                       ///< Not implemented.
    const CmdRackInserted & operator = (const CmdRackInserted &);   ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DataManager::CRack m_Rack;          ///< Rack Object
    DataManager::CProgram m_Program;    ///< Program Object
    DataManager::CDataReagentList m_ReagentList;    ///< Reagent container
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device
    int m_OvenTemperature;  ///< Oven Temperature , this is set by Colorado Master Thread, value fetched from User settings.


    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdRackInserted(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdRackInserted();
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
}; // end class CmdRackInserted

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDRACKINSERTED_H

