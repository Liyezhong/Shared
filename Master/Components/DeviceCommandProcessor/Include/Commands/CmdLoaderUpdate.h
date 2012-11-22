/****************************************************************************/
/*! \file CmdLoaderUpdate.h
 *
 *  \brief Definition file for class CmdLoaderUpdate.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-06-29
 *  $Author:    $ N.Kamath
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
#ifndef CMDLOADERUPDATE_H
#define CMDLOADERUPDATE_H

#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControlGlobal.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief This command would be sent when a rack is inserted into the loader
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdLoaderUpdate : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdLoaderUpdate();                                                    ///< Not implemented.
    CmdLoaderUpdate(const CmdLoaderUpdate &);                       ///< Not implemented.
    const CmdLoaderUpdate & operator = (const CmdLoaderUpdate &);   ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.

    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device
    qint32 m_RackId;        ///< Rack RFID
    qint32 m_RackUserData;  ///< Rack user data providing information such as color
    DeviceControl::LoaderRFIDChannel_t m_StationID;    ///< Loading/unloading station number where the rack is placed
    bool m_Empty; ///< Informs if the station is empty. True-empty,false -not empty.
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdLoaderUpdate(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdLoaderUpdate();
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
}; // end class CmdLoaderUpdate

} // end namespace DeviceCommandProcessor
#endif // CMDLOADERUPDATE_H
